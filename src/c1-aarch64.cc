// -*- mode: c++; -*-

// c1, the code generator for aarch64: the typed tree in, assembly text
// out, for clang and GNU as.  docs/aarch64.md is the design, with
// docs/backend.md where it says the model carries; this file
// implements its chapters 4 to 6 and 10, with c1-common.cc, and each
// function names the row it writes.  This is the first part of the
// first slice: the frame and straight-line code on integers.  A
// construct of a later step or slice throws logic_error("c1: not yet:
// ..."), and a tree the parser cannot make throws logic_error("c1:
// ...").

#include "c1.hh"
#include "c1-common.hh"

#include <algorithm>
#include <array>
#include <format>
#include <initializer_list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rcc {

namespace {

// The general registers by number, chapter 2.1: x29 the frame pointer,
// x30 the link register.
enum reg_t {
    x0,
    x1,
    x2,
    x3,
    x4,
    x5,
    x6,
    x7,
    x8,
    x9,
    x10,
    x11,
    x12,
    x13,
    x14,
    x15,
    x16,
    x17,
    x18,
    x19,
    x20,
    x21,
    x22,
    x23,
    x24,
    x25,
    x26,
    x27,
    x28,
    x29,
    x30
};

// The fifteen expression registers, chapter 4: the caller-saved set
// without x8, in the order a new value takes them, x0 first.  The state
// keeps an owner for each register below x16, so x8 has an entry that is
// never taken.  x16 and x17 are the scratch pair and never hold a value.
constexpr reg_t expression_registers[] = { x0, x1,  x2,  x3,  x4,  x5,  x6, x7,
                                           x9, x10, x11, x12, x13, x14, x15 };

constexpr int register_count = x16;

// The convention's argument registers, in order, chapter 3.
constexpr reg_t argument_registers[] = { x0, x1, x2, x3, x4, x5, x6, x7 };

// The frame record, the caller's x29 and x30, at the bottom of the
// frame, chapter 5.1.
constexpr int frame_record = 16;

// A value the generator holds: in an expression register, or in its
// spill slot once the register was needed for another.  The age orders
// the values for the spill, the oldest first.  valid is the width, in
// bits, up to which the register's bits are the value extended by its
// type's signedness: what the instruction that wrote it left there.  A
// load extends to 32 or 64, a write to a w name clears the upper half, a
// narrowing cast leaves the bits above its width stale; a widening cast
// that finds the value valid to its width writes nothing.
struct held_t
{
    int reg;
    int spill;
    long age;
    bool live;
    int valid;
};

// A handle on a held value; the default is the no-value of a void
// expression.
struct value_t
{
    int id = -1;
};

// The state of one file's generation.  Everything is reset per
// function, the label count included.  The body is a buffer, since the
// prologue's frame size is known only after it.
struct state_t
{
    std::ostream &out;

    // the target's largest frame
    long frame_limit;

    std::ostringstream body { };

    // each local's and parameter's displacement off x29
    std::unordered_map< const symbol_t *, int > slots { };

    // the bytes from x29 to the end of the last slot
    long frame = 0;

    // the value each register holds, or -1; values[i] is the value i
    std::array< int, register_count > owner { };
    std::vector< held_t > values { };
    long age = 0;
    int live = 0;

    std::string function { };
    int labels = 0;
    int ret = 0;
    bool terminated = false;
};

// operands

// A register by its name at a width in bits: the w name for 32 and
// below, the x name for 64.
std::string
reg(reg_t r, int width)
{
    return std::format("{}{}", width == 64 ? 'x' : 'w', static_cast< int >(r));
}

// The zero register at a width, chapter 2.1.
std::string_view
zero(int width)
{
    return width == 64 ? "xzr" : "wzr";
}

std::string
imm(long long value)
{
    return std::format("#{}", value);
}

// writing

// One instruction: a tab, the mnemonic, a tab and the operands
// separated by a comma and a space, as clang -S writes them.
void
insn(state_t &s,
     std::string_view mnemonic,
     std::initializer_list< std::string_view > operands = { })
{
    s.body << '\t' << mnemonic;

    const char *separator = "\t";
    for (const auto operand : operands) {
        s.body << separator << operand;
        separator = ", ";
    }

    s.body << '\n';
}

// Labels are counted per function, the epilogue's first.  The
// function's name in the label keeps the file's labels unique, since an
// identifier holds no dot.
int
new_label(state_t &s)
{
    return ++s.labels;
}

void
jump(state_t &s, int n)
{
    insn(s, "b", { label_name(s.function, n) });
    s.terminated = true;
}

// the frame

// The next slot above the last, of size bytes at align; returns its
// displacement off x29.  G12: a slot is never shared and never reused
// inside the activation.  Up to the target's frame limit the frame,
// rounded up to 16, is two immediates of sub, 5.2, and every slot is
// in reach of an add into x16, 5.3.  The parser refuses a single local
// over the limit at its declaration; a sum of smaller ones is refused
// here.
int
allocate(state_t &s, long size, int align)
{
    const long displacement = (s.frame + align - 1) / align * align;

    s.frame = displacement + size;
    if (s.frame > s.frame_limit)
        not_yet("a frame of 16 MiB or more");

    return static_cast< int >(displacement);
}

int
slot_of(const state_t &s, const symbol_t *sym)
{
    const auto it = s.slots.find(sym);
    if (it == s.slots.end())
        throw std::logic_error("c1: a name read before its declaration");

    return it->second;
}

// Whether an access of size bytes encodes the displacement off its
// base, 2.3: a multiple of the size no greater than 4095 times it.  No
// displacement off x29 is negative, so the unscaled form is not asked.
bool
in_reach(long displacement, int size)
{
    return displacement % size == 0 && displacement / size <= 4095;
}

// A slot as the address operand of an access of size bytes, 5.3:
// [x29, #d] in reach, else through x16, the add of the displacement's
// upper 12 bits, shifted, and the access of its low 12.  The add is
// written here, so the operand is formed just before its access, after
// anything that could spill, since a spill uses x16 too.
std::string
slot_operand(state_t &s, int displacement, int size)
{
    if (in_reach(displacement, size))
        return std::format("[x29, #{}]", displacement);

    const int upper = displacement & ~0xfff;
    const int lower = displacement & 0xfff;

    insn(s, "add", { "x16", "x29", imm(upper) });
    if (lower == 0)
        return "[x16]";

    return std::format("[x16, #{}]", lower);
}

// An lvalue's address as the operand of the instruction that reads or
// writes it: in this step a local's or a parameter's slot.
struct address_t
{
    int displacement;
};

address_t
slot_address(int displacement)
{
    return { .displacement = displacement };
}

std::string
operand(state_t &s, const address_t &a, int width)
{
    return slot_operand(s, a.displacement, width / 8);
}

// The load of 2.2 that brings a value of type t into a register of 32
// or 64 bits, and the width of its destination.  A narrow load extends
// by its mnemonic; to 64 a signed one is the x form of the same
// mnemonic, ldrsw for an int, and an unsigned one is the w form, since
// a write to the w name clears the upper half.
std::pair< std::string_view, int >
load_of(const int_t &t, int to)
{
    if (t.width == 64)
        return { "ldr", 64 };

    if (t.width == 32 && (to == 32 || !t.is_signed))
        return { "ldr", 32 };

    if (t.width == 32)
        return { "ldrsw", 64 };

    if (!t.is_signed)
        return { t.width == 8 ? "ldrb" : "ldrh", 32 };

    return { t.width == 8 ? "ldrsb" : "ldrsh", to };
}

// The store of 2.2 at the destination's width; G2, the low bytes of the
// register are the value.
std::string_view
store_of(int width)
{
    switch (width) {
    case 8: return "strb";
    case 16: return "strh";
    default: return "str";
    }
}

// the register stack

unsigned
bit(reg_t r)
{
    return 1u << r;
}

// Stores the value in r to the next spill slot, all 8 bytes, and frees
// the register, 6.2.
void
spill(state_t &s, reg_t r)
{
    const int id = s.owner[r];
    const int displacement = allocate(s, 8, 8);

    const auto to = slot_operand(s, displacement, 8);
    insn(s, "str", { reg(r, 64), to });

    s.values[id].reg = -1;
    s.values[id].spill = displacement;
    s.owner[r] = -1;
}

// A register for a value: the lowest free one outside avoid, or, when
// every one is held, the register of the oldest held value outside
// avoid, which is spilled first.
reg_t
take(state_t &s, unsigned avoid)
{
    for (const reg_t r : expression_registers)
        if (s.owner[r] < 0 && !(avoid & bit(r)))
            return r;

    int victim = -1;
    for (const reg_t r : expression_registers) {
        if (avoid & bit(r))
            continue;

        if (victim < 0 ||
            s.values[s.owner[r]].age < s.values[s.owner[victim]].age)
            victim = r;
    }

    if (victim < 0)
        throw std::logic_error("c1: no register to spill");
    spill(s, static_cast< reg_t >(victim));

    return static_cast< reg_t >(victim);
}

value_t
hold(state_t &s, reg_t r, int valid)
{
    const int id = static_cast< int >(s.values.size());

    s.values.push_back({ r, 0, s.age++, true, valid });
    s.owner[r] = id;
    ++s.live;

    return { id };
}

// A new value in a register taken for it, valid to the width the
// instruction about to write it leaves.
value_t
fresh(state_t &s, int valid, unsigned avoid = 0)
{
    return hold(s, take(s, avoid), valid);
}

// The value was just written at width by an instruction of its type.
void
wrote(state_t &s, value_t v, int width, bool is_signed)
{
    s.values[v.id].valid = written(width, is_signed);
}

// The register a value is in; a spilled value is reloaded first, all 8
// bytes, into a free register outside avoid.
reg_t
reg_of(state_t &s, value_t v, unsigned avoid = 0)
{
    if (v.id < 0)
        throw std::logic_error("c1: a void expression used as a value");

    if (s.values[v.id].reg >= 0)
        return static_cast< reg_t >(s.values[v.id].reg);

    const reg_t r = take(s, avoid);
    const auto from = slot_operand(s, s.values[v.id].spill, 8);
    insn(s, "ldr", { reg(r, 64), from });

    s.values[v.id].reg = r;
    s.owner[r] = v.id;

    return r;
}

// The value is consumed: its register, if it has one, is free.
void
release(state_t &s, value_t v)
{
    if (v.id < 0)
        return;

    auto &h = s.values[v.id];
    if (h.reg >= 0)
        s.owner[h.reg] = -1;

    h.reg = -1;
    h.live = false;
    --s.live;
}

// The registers of a binary node's two values, either reloaded if it
// was spilled, neither reload taking the other's register.
std::pair< reg_t, reg_t >
operands(state_t &s, value_t l, value_t r)
{
    const int held = s.values[l.id].reg;
    const reg_t right =
        reg_of(s, r, held >= 0 ? bit(static_cast< reg_t >(held)) : 0);
    const reg_t left = reg_of(s, l, bit(right));

    return { left, right };
}

value_t rvalue(state_t &, const expr_t &);
void effect(state_t &, const expr_t &);

// The register-to-register extension that brings a value of type from
// to a width of 32 or 64 bits, and the width it leaves the value valid
// to, 6.3: sxtb, sxth or sxtw at the new width for a signed value;
// uxtb, uxth, or mov wL, wL for an unsigned int, on the w name, whose
// write clears the upper half and so extends to 64 as well.
std::pair< std::string_view, int >
extension(const int_t &from, int to)
{
    if (from.is_signed) {
        switch (from.width) {
        case 8: return { "sxtb", to };
        case 16: return { "sxth", to };
        default: return { "sxtw", 64 };
        }
    }

    switch (from.width) {
    case 8: return { "uxtb", 64 };
    case 16: return { "uxth", 64 };
    default: return { "mov", 64 };
    }
}

// The value extended by from's signedness to a width of 32 or 64, where
// it is not valid to it already, the width rule: the instruction is
// written on the value's own register, sxtw xL, wL.
value_t
extended(state_t &s, value_t v, const int_t &from, int to)
{
    if (s.values[v.id].valid >= to)
        return v;

    const auto [mnemonic, valid] = extension(from, std::max(32, to));
    const reg_t r = reg_of(s, v);
    const int width = from.is_signed ? valid : 32;

    insn(s, mnemonic, { reg(r, width), reg(r, 32) });
    s.values[v.id].valid = valid;

    return v;
}

// addresses

// The address of an lvalue: a local's or a parameter's slot.  A string
// literal's label and what a dereference designates are slice 2's.
address_t
address(state_t &s, const expr_t &e)
{
    if (is< string_t >(e))
        not_yet("a string literal");

    if (is< unary_t >(e) && as< unary_t >(e).op == unary_op_t::deref)
        not_yet("the indirection operator *");

    return slot_address(slot_of(s, object_of(e)));
}

// The load row, 6.3.2.1p2: the value at an address into a register at
// a width of 32 or 64, a char or a short by the extending load of its
// signedness, ldrsb w0, [x29, #16].  The register is taken before the
// operand is formed, since the take may spill through x16.
value_t
load(state_t &s, const address_t &a, const int_t &t, int width)
{
    const auto [mnemonic, into] = load_of(t, width);

    const auto v = fresh(s, written(into, t.is_signed));
    const reg_t r = reg_of(s, v);

    const auto from = operand(s, a, t.width);
    insn(s, mnemonic, { reg(r, into), from });

    return v;
}

// The store row through an address, at the lvalue's width, strb w0,
// [x29, #16].
void
store(state_t &s, value_t v, const address_t &a, int width)
{
    const reg_t r = reg_of(s, v);

    const auto to = operand(s, a, width);
    insn(s, store_of(width), { reg(r, width), to });
}

// constants

// Whether add, sub and cmp encode a constant of an operation of width
// bits, 2.4: 0 to 4095, or that shifted left by 12, or the negative of
// either, which the assembler encodes as the other operation: add w0,
// w0, #-1 is sub w0, w0, #1.  The source's operator is written.
bool
arithmetic_immediate(long long value, int width)
{
    const auto v = normalized(value, width);
    const auto magnitude = v < 0 ? 0ull - static_cast< unsigned long long >(v) :
                                   static_cast< unsigned long long >(v);

    return magnitude <= 0xfff ||
           ((magnitude & 0xfff) == 0 && magnitude >> 12 <= 0xfff);
}

// The bits of a constant at width, as an unsigned number.
unsigned long long
bits_of(long long value, int width)
{
    const auto u = static_cast< unsigned long long >(value);

    return width == 64 ? u : u & ((1ull << width) - 1);
}

// Whether and, orr, eor and tst encode a constant of an operation of
// width bits, 2.4: a bitmask, a run of ones rotated within an element of
// 2, 4, 8, 16, 32 or 64 bits and the element repeated across the
// register.  The element is the shortest period of the bits; it is a
// rotated run when, read as a circle, its bits change exactly twice.
// Neither 0 nor all ones is a bitmask.
bool
bitmask_immediate(long long value, int width)
{
    auto u = bits_of(value, width);
    if (width == 32)
        u |= u << 32;

    if (u == 0 || u == ~0ull)
        return false;

    int element = 64;
    while (element > 2) {
        const int half = element / 2;
        const auto mask = (1ull << half) - 1;
        if ((u & mask) != ((u >> half) & mask))
            break;

        element = half;
    }

    int changes = 0;
    for (int i = 0; i < element; ++i)
        changes += ((u >> i) & 1) != ((u >> ((i + 1) % element)) & 1);

    return changes == 2;
}

// A bitmask is written in hex, its bits at the operation's width.
std::string
bitmask_of(long long value, int width)
{
    return std::format("#0x{:x}", bits_of(value, width));
}

// The constant row of 6.4: value into r at width.  One mov when the
// instruction encodes the value, and mov is three instructions under
// one name, 2.4: 16 bits at one position over zeros, the same over
// ones, and a bitmask, which is orr from the zero register.  Otherwise
// the register starts from zeros or from ones, whichever leaves fewer
// 16-bit chunks to write, zeros on a tie: a mov of the lowest chunk
// that differs, over the rest of the start, then one movk for each
// other chunk that differs.  A mov's immediate is the register's value
// after it, written signed, or in hex for a bitmask, as and writes one.
void
constant(state_t &s, reg_t r, long long value, int width)
{
    const auto u = bits_of(value, width);
    const int chunks = width / 16;
    const auto chunk = [u](int i) { return (u >> 16 * i) & 0xffff; };

    int zeros = 0;
    int ones = 0;
    for (int i = 0; i < chunks; ++i) {
        zeros += chunk(i) != 0;
        ones += chunk(i) != 0xffff;
    }

    const auto to = reg(r, width);

    if (zeros <= 1 || ones <= 1) {
        insn(s, "mov", { to, imm(normalized(value, width)) });
        return;
    }

    if (bitmask_immediate(value, width)) {
        insn(s, "mov", { to, bitmask_of(value, width) });
        return;
    }

    const unsigned long long start = ones < zeros ? 0xffff : 0;

    bool first = true;
    for (int i = 0; i < chunks; ++i) {
        if (chunk(i) == start)
            continue;

        if (first) {
            auto v = bits_of(start ? -1 : 0, width);
            v = (v & ~(0xffffull << 16 * i)) | chunk(i) << 16 * i;

            insn(s, "mov",
                 { to, imm(normalized(static_cast< long long >(v), width)) });
            first = false;
            continue;
        }

        insn(s, "movk",
             { to, std::format("#0x{:x}", chunk(i)),
               std::format("lsl #{}", 16 * i) });
    }
}

// A constant that must be held, in a register taken for it.  A
// constant narrower than int is written as its promotion, so the
// register holds it extended.
value_t
literal(state_t &s, const literal_t &l, const type_t &t)
{
    const auto &type = integer(t);
    const int width = std::max(32, type.width);
    const auto value =
        type.width < 32 ? widened(l.value, type) : normalized(l.value, width);

    const auto v = fresh(s, written(width, type.is_signed));
    constant(s, reg_of(s, v), value, width);

    return v;
}

// The store row of an initializer, chapter 6.6: e written into the
// object at to, at the object's width t, its value not wanted.  A zero
// constant is the store of the zero register, str wzr, [x29, #16], and
// no register is held; anything else, any other constant included, is
// its value in a register and the store, since str takes no immediate.
// to is a slot, which has no evaluation, so G13 holds.
void
initialize(state_t &s, const expr_t &e, const address_t &to, const int_t &t)
{
    if (is< literal_t >(e) &&
        normalized(as< literal_t >(e).value, t.width) == 0) {
        const auto operand_to = operand(s, to, t.width);
        insn(s, store_of(t.width), { zero(t.width), operand_to });
        return;
    }

    const auto v = rvalue(s, e);
    store(s, v, to, t.width);
    release(s, v);
}

// expressions

const char *
arithmetic_mnemonic(binary_op_t op)
{
    switch (op) {
    case binary_op_t::add: return "add";
    case binary_op_t::sub: return "sub";
    case binary_op_t::mul: return "mul";
    case binary_op_t::bit_and: return "and";
    case binary_op_t::bit_or: return "orr";
    case binary_op_t::bit_xor: return "eor";
    default: break;
    }

    throw std::logic_error("c1: not an arithmetic operator");
}

// The right operand of an operator's row, generated: the immediate the
// instruction takes, or the value held, with the width of its type.  A
// binary operator generates it after its left operand, a compound
// assignment before it reads its object (G13); the row is the same
// function for both.
struct right_t
{
    std::string immediate { };
    value_t value { };
    int width = 0;
};

// The right operand of op in an operation of width bits, as its row
// takes it, 6.4.  A constant is an immediate where the instruction
// encodes it, and then nothing is evaluated: add and sub's 12 bits, the
// bitmask of and, orr and eor, a shift's count masked to the width,
// which is what the instruction itself does to a count in a register
// (G3).  mul, sdiv and udiv take no immediate, and their constant is
// held, as is one its instruction does not encode.
right_t
right_operand(state_t &s, binary_op_t op, const expr_t &rhs, int width)
{
    const bool constant = is< literal_t >(rhs);
    const auto value = constant ? as< literal_t >(rhs).value : 0;

    if (constant && is_shift(op))
        return { .immediate = imm(value & (width - 1)) };

    const bool additive = op == binary_op_t::add || op == binary_op_t::sub;
    if (constant && additive && arithmetic_immediate(value, width))
        return { .immediate = imm(normalized(value, width)) };

    const bool logical = op == binary_op_t::bit_and ||
                         op == binary_op_t::bit_or ||
                         op == binary_op_t::bit_xor;
    if (constant && logical && bitmask_immediate(value, width))
        return { .immediate = bitmask_of(value, width) };

    return { .value = rvalue(s, rhs), .width = operation_width(rhs.type) };
}

// G4: the instruction the machine has, sdiv or udiv, which does not
// trap, 2.6: x / 0 is 0 and INT_MIN / -1 is INT_MIN; nothing looks at
// the divisor, which is in a register even when it is a constant.  The
// remainder has no instruction: the quotient into a register N taken
// and freed, then msub, L - N * R, sdiv N, L, R; msub L, N, R, L.
value_t
divide(state_t &s, binary_op_t op, value_t l, value_t r, const int_t &t)
{
    const int width = t.width;
    const auto mnemonic = t.is_signed ? "sdiv" : "udiv";

    const auto [l_reg, r_reg] = operands(s, l, r);
    const auto left = reg(l_reg, width);
    const auto right = reg(r_reg, width);

    if (op == binary_op_t::div) {
        insn(s, mnemonic, { left, left, right });
    }
    else {
        const auto quotient = reg(take(s, bit(l_reg) | bit(r_reg)), width);
        insn(s, mnemonic, { quotient, left, right });
        insn(s, "msub", { left, quotient, right, left });
    }

    release(s, r);
    wrote(s, l, width, t.is_signed);

    return l;
}

// G3: the instruction masks the count to the width, and nothing else
// does, 2.7.  A constant count is the immediate; a variable one is in
// any register, named at the width of the value shifted, lsl x0, x0,
// x1 for an int count, since only its low 6 bits are read.
const char *
shift_mnemonic(binary_op_t op, bool is_signed)
{
    return op == binary_op_t::shl ? "lsl" : is_signed ? "asr" : "lsr";
}

value_t
shift(state_t &s, binary_op_t op, value_t l, const right_t &r, const int_t &t)
{
    const int width = t.width;
    const auto mnemonic = shift_mnemonic(op, t.is_signed);

    if (!r.immediate.empty()) {
        const auto left = reg(reg_of(s, l), width);
        insn(s, mnemonic, { left, left, r.immediate });
        wrote(s, l, width, t.is_signed);
        return l;
    }

    const auto [l_reg, r_reg] = operands(s, l, r.value);
    const auto left = reg(l_reg, width);
    insn(s, mnemonic, { left, left, reg(r_reg, width) });

    release(s, r.value);
    wrote(s, l, width, t.is_signed);

    return l;
}

// An arithmetic, bitwise or shift operator on the held left value and
// the generated right operand, in type t, chapter 6.1: op L, L, R, the
// result in L, or op L, L, #c for an immediate.  G1: add, sub, mul and
// lsl wrap; mul is the same instruction for unsigned, the low bits
// agree.
value_t
arithmetic(
    state_t &s, binary_op_t op, value_t l, const right_t &r, const type_t &t)
{
    const int width = operation_width(t);

    if (op == binary_op_t::div || op == binary_op_t::rem)
        return divide(s, op, l, r.value, integer(t));

    if (is_shift(op))
        return shift(s, op, l, r, integer(t));

    const auto mnemonic = arithmetic_mnemonic(op);

    if (!r.immediate.empty()) {
        const auto left = reg(reg_of(s, l), width);
        insn(s, mnemonic, { left, left, r.immediate });
        wrote(s, l, width, integer(t).is_signed);
        return l;
    }

    const auto [l_reg, r_reg] = operands(s, l, r.value);
    const auto left = reg(l_reg, width);
    insn(s, mnemonic, { left, left, reg(r_reg, width) });

    release(s, r.value);
    wrote(s, l, width, integer(t).is_signed);

    return l;
}

value_t
binary(state_t &s, const expr_t &e)
{
    const auto &b = as< binary_t >(e);

    switch (b.op) {
    case binary_op_t::log_and:
    case binary_op_t::log_or: not_yet("&& or || as a value");
    // 6.5.18p2: the left operand is evaluated as a void expression,
    // the effect context of chapter 6.6
    case binary_op_t::comma: {
        effect(s, *b.lhs);
        return rvalue(s, *b.rhs);
    }
    default: break;
    }

    if (is_comparison(b.op))
        not_yet("a comparison as a value");

    if (is< pointer_t >(e.type))
        not_yet("pointer arithmetic");

    if (is< pointer_t >(b.lhs->type))
        not_yet("a difference of pointers");

    // G13: the left operand first, held while the right one is generated
    const auto l = rvalue(s, *b.lhs);
    const auto r = right_operand(s, b.op, *b.rhs, operation_width(e.type));

    return arithmetic(s, b.op, l, r, e.type);
}

// ++ and --, either side: the address once, the load, add or sub of 1
// (6.5.4.1p2, 6.5.3.5p2), the store through the same address; G1, the
// machine wraps.  A char or a short is loaded extended to 32 bits and
// its low bytes stored, so no operator sees 8 or 16 bits.  The prefix
// forms operate on the loaded value's register and yield it.  The
// postfix forms name another register as the destination, add w1, w0,
// #1, and yield the old value where the load left it: nothing is
// copied, 6.1.
value_t
step(state_t &s, unary_op_t op, const expr_t &arg)
{
    if (register_variable(arg))
        not_yet("a register variable");

    if (is< pointer_t >(arg.type))
        not_yet("++ or -- of a pointer");

    const auto &t = integer(arg.type);
    const int width = std::max(32, t.width);

    const bool up = op == unary_op_t::pre_inc || op == unary_op_t::post_inc;
    const bool post = op == unary_op_t::post_inc || op == unary_op_t::post_dec;

    const auto place = address(s, arg);
    const auto l = load(s, place, t, width);
    const reg_t r = reg_of(s, l);

    const auto v = post ? fresh(s, written(width, t.is_signed), bit(r)) : l;
    insn(s, up ? "add" : "sub",
         { reg(reg_of(s, v), width), reg(r, width), imm(1) });
    store(s, v, place, t.width);

    if (post) {
        release(s, v);
        return l;
    }

    s.values[l.id].valid = std::min(written(width, t.is_signed), t.width);

    return l;
}

value_t
unary(state_t &s, const unary_t &u, const type_t &t)
{
    switch (u.op) {
    // G1: neg of INT_MIN is INT_MIN
    case unary_op_t::neg:
    case unary_op_t::bit_not: {
        const auto &type = integer(t);
        const int width = operation_width(t);

        const auto v = rvalue(s, *u.arg);
        const auto left = reg(reg_of(s, v), width);
        insn(s, u.op == unary_op_t::neg ? "neg" : "mvn", { left, left });
        wrote(s, v, width, type.is_signed);

        return v;
    }

    case unary_op_t::log_not: not_yet("! as a value");

    case unary_op_t::pre_inc:
    case unary_op_t::pre_dec:
    case unary_op_t::post_inc:
    case unary_op_t::post_dec: return step(s, u.op, *u.arg);

    case unary_op_t::addr: not_yet("the address operator &");

    case unary_op_t::deref: not_yet("the indirection operator *");
    }

    throw std::logic_error("c1: an unknown unary operator");
}

// Plain or compound assignment to a slot; its value is the value
// stored, the low bytes of L.  Plain: the right operand, then the
// store.  Compound: the right operand first, then, as one piece and
// nothing evaluated between, the load of the object extended into the
// common type the parser chose, the operation, and the store of the
// result's low bytes back (G2), so the object is read after the right
// operand (G13).  6.5.17.3p4: "with respect to an indeterminately
// sequenced function call, the operation of a compound assignment is a
// single evaluation."
value_t
assign(state_t &s, const assign_t &a)
{
    const auto &t = integer(a.lhs->type);
    const bool shifted = a.op && is_shift(*a.op);

    if (register_variable(*a.lhs))
        not_yet("a register variable");

    if (a.op && is< pointer_t >(a.lhs->type))
        not_yet("+= or -= on a pointer");

    // the parser converted the right operand to the common type, or
    // promoted a shift count on its own
    if (!shifted && !(a.rhs->type == a.common))
        throw std::logic_error("c1: an assignment across two types");

    const auto place = address(s, *a.lhs);

    if (!a.op) {
        const auto v = rvalue(s, *a.rhs);
        store(s, v, place, t.width);
        return v;
    }

    const int width = operation_width(a.common);
    const auto r = right_operand(s, *a.op, *a.rhs, width);

    // the value of the expression has the lvalue's type: the bits
    // above its width are the operation's, not the value's extension
    const auto l = load(s, place, t, width);
    const auto v = arithmetic(s, *a.op, l, r, a.common);
    store(s, v, place, t.width);
    s.values[v.id].valid = std::min(s.values[v.id].valid, t.width);

    return v;
}

// An integer conversion, 6.3.  G2: narrowing is no instruction, the
// narrower register name is the value; widening extends by the
// operand's signedness, and only when the register is not already
// valid to the new width, as it is after the load of a char or a 32-bit
// write of an unsigned value.  A change of signedness alone is nothing.
// A cast to void evaluates its operand for its effect alone, chapter
// 6.6.  An array, which decays here, 6.3.2.1p3, is slice 2's.
value_t
cast(state_t &s, const cast_t &c, const type_t &to)
{
    if (is< void_t >(to)) {
        effect(s, *c.arg);
        return { };
    }

    if (is< string_t >(*c.arg))
        not_yet("a string literal");

    if (is< array_t >(c.arg->type))
        not_yet("the use of an array");

    const auto &from = integer(c.arg->type);
    const auto &into = integer(to);

    auto v = rvalue(s, *c.arg);
    if (into.width > from.width)
        v = extended(s, v, from, into.width);

    auto &valid = s.values[v.id].valid;
    valid = std::min(valid, into.width);

    return v;
}

// The value of an expression in a register, its code written first.
// An array is never a value: the parser decays it, 6.3.2.1p3, and a
// string literal is an array.
value_t
rvalue(state_t &s, const expr_t &e)
{
    if (is< array_t >(e.type))
        throw std::logic_error("c1: an array as a value");

    if (is< literal_t >(e))
        return literal(s, as< literal_t >(e), e.type);

    if (is< name_t >(e)) {
        if (register_variable(e))
            not_yet("a register variable");

        const auto &t = integer(e.type);
        return load(s, address(s, e), t, std::max(32, t.width));
    }

    if (is< unary_t >(e))
        return unary(s, as< unary_t >(e), e.type);

    if (is< binary_t >(e))
        return binary(s, e);

    if (is< assign_t >(e))
        return assign(s, as< assign_t >(e));

    if (is< call_t >(e))
        not_yet("a call");

    if (is< cast_t >(e))
        return cast(s, as< cast_t >(e), e.type);

    not_yet("a conditional expression");
}

// ++ and -- for their effect: the load, add or sub of 1 and the store,
// chapter 6.6, in one register, since nothing reads the old value: the
// prefix form, its value discarded.
void
modify(state_t &s, const unary_t &u)
{
    const bool up = u.op == unary_op_t::pre_inc || u.op == unary_op_t::post_inc;
    const auto prefix = up ? unary_op_t::pre_inc : unary_op_t::pre_dec;

    release(s, step(s, prefix, *u.arg));
}

// The effect context, chapter 6.6, Ritchie's efftab beside regtab and
// cctab: an expression evaluated for its side effects, its value not
// wanted.  The machine has no memory forms, so a compound assignment is
// its value context's load, operation and store, and the value is
// discarded.  A plain assignment is the store row of an initializer,
// initialize(), which stores a zero from the zero register.  The
// context reaches through the operators that discard a value: both
// operands of a comma, whose left one is in this context wherever the
// comma stands (6.5.18p2), and, by cast(), the operand of a cast to
// void; anything else is its value, discarded.
void
effect(state_t &s, const expr_t &e)
{
    if (is< assign_t >(e) && !as< assign_t >(e).op) {
        const auto &a = as< assign_t >(e);

        if (register_variable(*a.lhs))
            not_yet("a register variable");

        initialize(s, *a.rhs, address(s, *a.lhs), integer(a.lhs->type));
        return;
    }

    if (is< unary_t >(e) && is_step(as< unary_t >(e).op)) {
        modify(s, as< unary_t >(e));
        return;
    }

    if (is< binary_t >(e) && as< binary_t >(e).op == binary_op_t::comma) {
        effect(s, *as< binary_t >(e).lhs);
        effect(s, *as< binary_t >(e).rhs);
        return;
    }

    release(s, rvalue(s, e));
}

// statements

void statement(state_t &, const stmt_t &);

void
expr_stmt(state_t &s, const expr_stmt_t &x)
{
    if (x.e)
        effect(s, *x.e);
}

// G12: the slot is taken where the declaration is met, one for the
// whole activation, and before the initializer, whose scope already
// holds the name, 6.2.1p7.  G5: without an initializer nothing is
// stored; the slot holds what it holds.  The initializer is the store
// row of chapter 6.6, into the slot.  An array's slot is at the type's
// alignment, chapter 5.1, its displacement the array's lowest address.
void
decl_stmt(state_t &s, const decl_stmt_t &d)
{
    const auto *sym = d.sym;
    if (sym->in_register)
        not_yet("a register variable");

    const int displacement =
        allocate(s, size_of(sym->type), align_of(sym->type));
    s.slots.emplace(sym, displacement);

    if (!d.init)
        return;

    initialize(s, *d.init, slot_address(displacement), integer(sym->type));
}

void
block_stmt(state_t &s, const block_t &b)
{
    for (const auto &st : b.body)
        statement(s, st);
}

// The value goes to x0.  A char or a short is not extended: the
// convention leaves the bits above a narrow result unspecified, and the
// caller extends it, 6.3.  Every return jumps to the one epilogue, the
// last statement of the body included: the jump to the next instruction
// is what the statement says, and removing it was the optional peephole
// pass's work in the PDP-11 compiler, which rcc has no counterpart of.
void
return_stmt(state_t &s, const return_t &r)
{
    if (r.value) {
        const int width = std::max(32, integer(r.value->type).width);

        const auto v = rvalue(s, *r.value);
        const reg_t from = reg_of(s, v);
        if (from != x0)
            insn(s, "mov", { reg(x0, width), reg(from, width) });

        release(s, v);
    }

    jump(s, s.ret);
}

// Every value an expression statement holds is consumed by its end;
// nothing is held from one statement to the next.
void
statement(state_t &s, const stmt_t &st)
{
    if (is< expr_stmt_t >(st))
        expr_stmt(s, as< expr_stmt_t >(st));
    else if (is< decl_stmt_t >(st))
        decl_stmt(s, as< decl_stmt_t >(st));
    else if (is< block_t >(st))
        block_stmt(s, as< block_t >(st));
    else if (is< if_t >(st))
        not_yet("an if statement");
    else if (is< while_t >(st))
        not_yet("a while statement");
    else if (is< for_t >(st))
        not_yet("a for statement");
    else
        return_stmt(s, as< return_t >(st));

    if (s.live != 0)
        throw std::logic_error("c1: a value held across a statement");
}

// functions

// The parameters' half of the prologue, before the body, since the
// body's slots come after theirs.  Each of the first eight takes the
// next slot and is stored there from its argument register at its
// width, str w0, [x29, #16], 5.2.
void
parameters(state_t &s, const function_decl_t &fd)
{
    for (std::size_t i = 0; i < fd.params.size(); ++i) {
        const auto *p = fd.params[i];
        const auto &t = integer(p->type);

        if (p->in_register)
            not_yet("a register parameter");

        if (i >= std::size(argument_registers))
            not_yet("a parameter past the eighth");

        const int displacement =
            allocate(s, size_of(p->type), align_of(p->type));
        s.slots.emplace(p, displacement);

        const auto to = slot_operand(s, displacement, t.width / 8);
        insn(s, store_of(t.width), { reg(argument_registers[i], t.width), to });
    }
}

// The frame's size as the immediates of sub and add, 5.2: 12 bits, or
// 12 bits shifted by 12.  The upper part first; a part that is zero is
// not written, so a frame under 4096 bytes or a multiple of 4096 is one
// instruction.
std::vector< long >
frame_parts(long size)
{
    std::vector< long > parts;
    for (const long part : { size & ~0xfffL, size & 0xfffL })
        if (part != 0)
            parts.push_back(part);

    return parts;
}

// Chapter 5.2: the frame opened by sub, the frame record stored at its
// bottom, x29 pointed at it.  Each sub is followed by the canonical frame
// address's offset from sp so far, and the record by the places of x29
// and x30 off the canonical frame address, which is sp before the sub.
void
prologue(state_t &s, long size)
{
    long opened = 0;
    for (const long part : frame_parts(size)) {
        opened += part;
        s.out << std::format("\tsub\tsp, sp, #{}\n"
                             "\t.cfi_def_cfa_offset {}\n",
                             part, opened);
    }

    s.out << std::format("\tstp\tx29, x30, [sp]\n"
                         "\t.cfi_offset w29, {}\n"
                         "\t.cfi_offset w30, {}\n"
                         "\tmov\tx29, sp\n"
                         "\t.cfi_def_cfa_register w29\n",
                         -size, 8 - size);
}

// The one epilogue: the frame record loaded, the frame closed by add.
// The label is reached with sp equal to x29, so the canonical frame
// address moves to sp + size there, before the load gives x29 back to
// the caller; after it x29 and x30 are in their registers again, and
// each add leaves the address at what is left of the frame above sp.
void
epilogue(state_t &s, long size)
{
    s.out << label_name(s.function, s.ret) << ":\n"
          << std::format("\t.cfi_def_cfa wsp, {}\n", size)
          << "\tldp\tx29, x30, [sp]\n"
             "\t.cfi_restore w29\n"
             "\t.cfi_restore w30\n";

    long left = size;
    for (const long part : frame_parts(size)) {
        left -= part;
        s.out << std::format("\tadd\tsp, sp, #{}\n"
                             "\t.cfi_def_cfa_offset {}\n",
                             part, left);
    }

    s.out << "\tret\n";
}

// A definition.  The body is generated first, into the buffer, and the
// prologue written after it, when the frame's size is known.  The first
// slot lies above the frame record, and this is the one place that says
// so.  The one epilogue is reached with x0 as it is, G5's some value,
// but for main, which returns 0 when it falls off its end, 5.1.2.3.4.
void
function(state_t &s, const function_decl_t &fd)
{
    s.body.str({ });
    s.slots.clear();
    s.owner.fill(-1);
    s.values.clear();
    s.age = 0;
    s.live = 0;
    s.frame = frame_record;
    s.function = fd.sym->name;
    s.labels = 0;
    s.ret = new_label(s);
    s.terminated = false;

    parameters(s, fd);
    block_stmt(s, *fd.body);

    if (!s.terminated && s.function == "main")
        insn(s, "mov", { "w0", imm(0) });

    const long size = (s.frame + 15) / 16 * 16;

    const auto &name = s.function;
    s.out << std::format("\t.globl\t{}\n"
                         "\t.type\t{}, @function\n"
                         "{}:\n"
                         "\t.cfi_startproc\n",
                         name, name, name);

    prologue(s, size);

    s.out << s.body.str();

    epilogue(s, size);
    s.out << std::format("\t.cfi_endproc\n"
                         "\t.size\t{}, .-{}\n",
                         name, name);
}

} // namespace

// The functions the unit defines, in order; one it only declares
// writes nothing, since the assembler leaves its symbol to the link.
// The note section marks the stack not executable.
void
generate_aarch64(const unit_t &unit,
                 const target_t &target,
                 std::string_view file,
                 std::ostream &out)
{
    state_t s { .out = out, .frame_limit = target.frame_limit };

    out << std::format("\t.file\t{}\n\t.text\n", quoted_file(file));

    for (const auto &fd : unit.functions)
        if (fd.body)
            function(s, fd);

    out << "\t.section\t.note.GNU-stack,\"\",@progbits\n";
}

} // namespace rcc
