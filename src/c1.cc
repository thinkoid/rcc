// -*- mode: c++; -*-

// c1, the code generator: the typed tree in, x86-64 assembly text out,
// AT&T syntax for the system assembler.  docs/backend.md is the
// design; its chapters 4 to 8 are what this file implements, and each
// function names the row it writes.  This is the first slice:
// integers, control and calls.  A construct of a later slice throws
// logic_error("c1: not yet: ..."), and a tree the parser cannot make
// throws logic_error("c1: ..."), as emit.cc does.

#include "c1.hh"

#include <algorithm>
#include <array>
#include <cstdint>
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

// The expression registers, chapter 4: the caller-saved set, in the
// order a new value takes them, rax first.
enum reg_t { rax, rcx, rdx, rsi, rdi, r8, r9, r10, r11 };

constexpr int register_count = 9;

// The convention's argument registers, in order, 3.2.
constexpr reg_t argument_registers[] = { rdi, rsi, rdx, rcx, r8, r9 };

// A value the generator holds: in an expression register, or in its
// spill slot once the register was needed for another.  The age orders
// the values for the spill, the oldest first.  valid is the width, in
// bits, up to which the register's bits are the value extended by its
// type's signedness: what the instruction that wrote it left there.  A
// load extends to 32 or 64, a 32-bit write clears the upper half, a
// narrowing cast leaves the bits above its width stale; a widening
// cast that finds the value valid to its width writes nothing.
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

// The state of one file's generation.  The labels are numbered across
// the file; everything else is reset per function.  The body is a
// buffer, since the prologue's frame size is known only after it.
struct state_t
{
    std::ostream &out;
    int labels = 0;

    std::ostringstream body { };

    // each local's and parameter's displacement off rbp
    std::unordered_map< const symbol_t *, int > slots { };

    // the bytes the slots take below rbp, and whether the function
    // calls, which keeps it out of the red zone
    int frame = 0;
    bool calls = false;

    // the value each register holds, or -1; values[i] is the value i
    std::array< int, register_count > owner { };
    std::vector< held_t > values { };
    long age = 0;
    int live = 0;

    std::string function { };
    int ret = 0;
    bool terminated = false;
};

[[noreturn]] void
not_yet(std::string_view what)
{
    throw std::logic_error(std::format("c1: not yet: {}", what));
}

// types

// The integer type of an operand.  A pointer or an array is the next
// slice; anything else the parser does not make.
const int_t &
integer(const type_t &t)
{
    if (is< pointer_t >(t) || is< array_t >(t))
        not_yet("a pointer or an array");

    if (!is< int_t >(t))
        throw std::logic_error("c1: a non-integer operand");

    return as< int_t >(t);
}

// The width of an operator's operation, 32 or 64 bits: the parser
// promotes every narrower operand before an operator sees it.
int
operation_width(const type_t &t)
{
    const int width = integer(t).width;
    if (width < 32)
        throw std::logic_error("c1: an operator on a narrow operand");

    return width;
}

// A constant of N bits as the assembler takes it: the value modulo
// 2^N, written signed.  G2: the low bits are the value.
long long
normalized(long long value, int width)
{
    if (width >= 64)
        return value;

    const auto mask = (1ull << width) - 1;
    const auto sign = 1ull << (width - 1);
    const auto bits = static_cast< unsigned long long >(value) & mask;

    return static_cast< long long >(bits ^ sign) -
           static_cast< long long >(sign);
}

// A constant narrower than int as the promotion to int reads it: the
// low bits of the value, sign- or zero-extended by the type.
long long
widened(long long value, const int_t &t)
{
    if (t.is_signed)
        return normalized(value, t.width);

    const auto mask = (1ull << t.width) - 1;

    return static_cast< long long >(static_cast< unsigned long long >(value) &
                                    mask);
}

bool
fits_imm32(long long value)
{
    return value >= INT32_MIN && value <= INT32_MAX;
}

// The width a value is valid to after an instruction writes it at
// width: a 32-bit write clears the upper half of the register, which
// is the extension of an unsigned value and not of a signed one.
int
written(int width, bool is_signed)
{
    return width == 32 && !is_signed ? 64 : width;
}

// operands

char
suffix(int width)
{
    switch (width) {
    case 8: return 'b';
    case 16: return 'w';
    case 32: return 'l';
    default: return 'q';
    }
}

// A mnemonic with the suffix of its width.
std::string
sized(std::string_view stem, int width)
{
    return std::format("{}{}", stem, suffix(width));
}

// A register by its name at a width in bits.
std::string
reg(reg_t r, int width)
{
    static constexpr std::string_view names[register_count][4] = {
        { "al", "ax", "eax", "rax" },      { "cl", "cx", "ecx", "rcx" },
        { "dl", "dx", "edx", "rdx" },      { "sil", "si", "esi", "rsi" },
        { "dil", "di", "edi", "rdi" },     { "r8b", "r8w", "r8d", "r8" },
        { "r9b", "r9w", "r9d", "r9" },     { "r10b", "r10w", "r10d", "r10" },
        { "r11b", "r11w", "r11d", "r11" },
    };

    const int column = width == 8 ? 0 : width == 16 ? 1 : width == 32 ? 2 : 3;

    return std::format("%{}", names[r][column]);
}

// A slot: a displacement off the frame pointer.
std::string
at(int displacement)
{
    return std::format("{}(%rbp)", displacement);
}

std::string
imm(long long value)
{
    return std::format("${}", value);
}

std::string
label_name(int n)
{
    return std::format(".L{}", n);
}

// writing

// One instruction: a tab, the mnemonic, a tab and the operands
// separated by a comma and a space, as gcc -S writes them.
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

// Labels are numbered across the file, the epilogue's included, so
// no two functions' .L labels collide.
int
new_label(state_t &s)
{
    return ++s.labels;
}

void
place_label(state_t &s, int n)
{
    s.body << label_name(n) << ":\n";
    s.terminated = false;
}

void
jump(state_t &s, int n)
{
    insn(s, "jmp", { label_name(n) });
    s.terminated = true;
}

// the frame

// The next slot below the last, of size bytes at align; returns its
// displacement.  G12: a slot is never shared and never reused inside
// the activation.
int
allocate(state_t &s, long size, int align)
{
    s.frame = static_cast< int >((s.frame + size + align - 1) / align * align);

    return -s.frame;
}

int
slot_of(const state_t &s, const symbol_t *sym)
{
    const auto it = s.slots.find(sym);
    if (it == s.slots.end())
        throw std::logic_error("c1: a name read before its declaration");

    return it->second;
}

// The local or parameter an lvalue names.  A dereference is the next
// slice, a register variable the one after, a global later still.
const symbol_t *
object_of(const expr_t &e)
{
    if (!is< name_t >(e))
        not_yet("an lvalue other than a name");

    const auto *sym = as< name_t >(e).sym;
    if (sym->storage == storage_t::function)
        throw std::logic_error("c1: a function as an object");

    if (sym->storage == storage_t::global)
        not_yet("a global");

    if (sym->in_register)
        not_yet("a register variable");

    return sym;
}

// The extending mov that brings a value of type from to a width of 32
// or 64 bits, and the width of its destination register.  A 32-bit
// unsigned value widens to 64 by a movl, since a write to the 32-bit
// name clears the upper half.
std::pair< std::string, int >
extension(const int_t &from, int to)
{
    if (from.width == 64)
        return { "movq", 64 };

    if (from.width == 32 && (to == 32 || !from.is_signed))
        return { "movl", 32 };

    if (from.width == 32)
        return { "movslq", 64 };

    const char sign = from.is_signed ? 's' : 'z';

    return { std::format("mov{}{}{}", sign, suffix(from.width), suffix(to)),
             to };
}

// The store row: the narrow mov of the slot's width; G2, the low bytes
// of the register are the value.
void
store(state_t &s, reg_t r, int displacement, int width)
{
    insn(s, sized("mov", width), { reg(r, width), at(displacement) });
}

// the register stack

unsigned
bit(reg_t r)
{
    return 1u << r;
}

// Stores the value in r to the next spill slot, all 8 bytes, and frees
// the register.
void
spill(state_t &s, reg_t r)
{
    const int id = s.owner[r];
    const int displacement = allocate(s, 8, 8);

    insn(s, "movq", { reg(r, 64), at(displacement) });

    s.values[id].reg = -1;
    s.values[id].spill = displacement;
    s.owner[r] = -1;
}

void
spill_all(state_t &s)
{
    for (int r = 0; r < register_count; ++r)
        if (s.owner[r] >= 0)
            spill(s, static_cast< reg_t >(r));
}

// A register for a value: the lowest free one outside avoid, or, when
// every one is held, the register of the oldest held value outside
// avoid, which is spilled first.
reg_t
take(state_t &s, unsigned avoid)
{
    for (int r = 0; r < register_count; ++r)
        if (s.owner[r] < 0 && !(avoid & bit(static_cast< reg_t >(r))))
            return static_cast< reg_t >(r);

    int victim = -1;
    for (int r = 0; r < register_count; ++r) {
        if (avoid & bit(static_cast< reg_t >(r)))
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

// The register a value is in; a spilled value is reloaded first into a
// free register outside avoid.
reg_t
reg_of(state_t &s, value_t v, unsigned avoid = 0)
{
    if (v.id < 0)
        throw std::logic_error("c1: a void expression used as a value");

    if (s.values[v.id].reg >= 0)
        return static_cast< reg_t >(s.values[v.id].reg);

    const reg_t r = take(s, avoid);
    insn(s, "movq", { at(s.values[v.id].spill), reg(r, 64) });

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

// Moves the value in from to the free register to, at a width: a real
// mov, and the map follows it (chapter 6.5).
void
move(state_t &s, reg_t from, reg_t to, int width)
{
    insn(s, sized("mov", width), { reg(from, width), reg(to, width) });

    const int id = s.owner[from];
    s.values[id].reg = to;
    s.owner[to] = id;
    s.owner[from] = -1;
}

// Frees r for a pinned instruction: a value held there moves, all 64
// bits, to a free register outside avoid.
void
vacate(state_t &s, reg_t r, unsigned avoid)
{
    if (s.owner[r] >= 0)
        move(s, r, take(s, avoid | bit(r)), 64);
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

// expressions

value_t rvalue(state_t &, const expr_t &);
void cbranch(state_t &, const expr_t &, int label, bool sense);

// Whether e is a constant the instruction can take as an immediate: x86
// immediates are 32 bits, sign-extended in a 64-bit operation.
bool
immediate(const expr_t &e, int width)
{
    return is< literal_t >(e) &&
           fits_imm32(normalized(as< literal_t >(e).value, width));
}

std::string
immediate_of(const expr_t &e, int width)
{
    return imm(normalized(as< literal_t >(e).value, width));
}

// A constant that must be held: mov $c, L, or movabsq for a 64-bit one
// no immediate holds.  A constant narrower than int is written as its
// promotion, so the register holds it extended.
value_t
literal(state_t &s, const literal_t &l, const type_t &t)
{
    const auto &type = integer(t);
    const int width = std::max(32, type.width);
    const auto value =
        type.width < 32 ? widened(l.value, type) : normalized(l.value, width);

    const auto v = fresh(s, written(width, type.is_signed));
    const reg_t r = reg_of(s, v);

    if (width == 32)
        insn(s, "movl", { imm(value), reg(r, 32) });
    else if (fits_imm32(value))
        insn(s, "movq", { imm(value), reg(r, 64) });
    else
        insn(s, "movabsq", { imm(value), reg(r, 64) });

    return v;
}

// The read of a local or a parameter, 6.3.2.1p2, into a new register
// at a width of 32 or 64: a char or a short by the extending mov of
// its signedness.
value_t
load(state_t &s, int displacement, const int_t &t, int width)
{
    const auto [mnemonic, into] = extension(t, width);

    const auto v = fresh(s, written(into, t.is_signed));
    insn(s, mnemonic, { at(displacement), reg(reg_of(s, v), into) });

    return v;
}

const char *
arithmetic_mnemonic(binary_op_t op)
{
    switch (op) {
    case binary_op_t::add: return "add";
    case binary_op_t::sub: return "sub";
    case binary_op_t::mul: return "imul";
    case binary_op_t::bit_and: return "and";
    case binary_op_t::bit_or: return "or";
    case binary_op_t::bit_xor: return "xor";
    default: break;
    }

    throw std::logic_error("c1: not an arithmetic operator");
}

// G4: the instruction the machine has, idiv or div, a fault on x86 for
// a zero divisor or INT_MIN / -1; nothing looks at the divisor, which
// is in a register even when it is a constant.  The dividend goes to
// rax and rdx is cleared or sign-filled; a value held in either, or a
// divisor there, moves out first (chapter 6.5).  The quotient or the
// remainder comes back into L.
value_t
divide(state_t &s, binary_op_t op, value_t l, const expr_t &rhs, const int_t &t)
{
    const int width = t.width;

    const auto r = rvalue(s, rhs);
    auto [left, right] = operands(s, l, r);

    const unsigned pinned = bit(rax) | bit(rdx);
    if (right == rax || right == rdx) {
        const reg_t to = take(s, pinned | bit(left));
        move(s, right, to, 64);
        right = to;
    }

    for (const reg_t p : { rax, rdx })
        if (p != left)
            vacate(s, p, pinned | bit(left) | bit(right));

    if (left != rax)
        insn(s, sized("mov", width), { reg(left, width), reg(rax, width) });

    if (!t.is_signed)
        insn(s, "xorl", { "%edx", "%edx" });
    else
        insn(s, width == 32 ? "cltd" : "cqto");

    insn(s, sized(t.is_signed ? "idiv" : "div", width), { reg(right, width) });

    const reg_t result = op == binary_op_t::div ? rax : rdx;
    if (left != result)
        insn(s, sized("mov", width), { reg(result, width), reg(left, width) });

    release(s, r);
    wrote(s, l, width, t.is_signed);

    return l;
}

// G3: the instruction masks the count to the width, and nothing else
// does.  A constant count is the immediate, its low 8 bits, which is
// what the instruction encodes; a variable one goes to cl, a value
// held in rcx moving out first (chapter 6.5).
value_t
shift(state_t &s, binary_op_t op, value_t l, const expr_t &rhs, const int_t &t)
{
    const int width = t.width;
    const auto stem = op == binary_op_t::shl ? "shl" :
                      t.is_signed            ? "sar" :
                                               "shr";

    if (is< literal_t >(rhs)) {
        insn(s, sized(stem, width),
             { imm(as< literal_t >(rhs).value & 0xff),
               reg(reg_of(s, l), width) });
        wrote(s, l, width, t.is_signed);
        return l;
    }

    const auto r = rvalue(s, rhs);
    auto [left, right] = operands(s, l, r);

    if (left == rcx) {
        const reg_t to = take(s, bit(rcx) | bit(right));
        move(s, left, to, 64);
        left = to;
    }

    if (right != rcx) {
        vacate(s, rcx, bit(left) | bit(right));
        move(s, right, rcx, width);
    }

    insn(s, sized(stem, width), { "%cl", reg(left, width) });

    release(s, r);
    wrote(s, l, width, t.is_signed);

    return l;
}

// An arithmetic, bitwise or shift operator on the held left value and
// the right operand, in type t, chapter 6.1: op R, L, the result in L.
// A constant right operand is an immediate where the instruction takes
// one; imul's is the three-operand form.  G1: add, sub, imul and shl
// wrap; imul is the same instruction for unsigned, the low bits agree.
value_t
arithmetic(
    state_t &s, binary_op_t op, value_t l, const expr_t &rhs, const type_t &t)
{
    const int width = operation_width(t);

    if (op == binary_op_t::div || op == binary_op_t::rem)
        return divide(s, op, l, rhs, integer(t));

    if (op == binary_op_t::shl || op == binary_op_t::shr)
        return shift(s, op, l, rhs, integer(t));

    const auto mnemonic = sized(arithmetic_mnemonic(op), width);

    if (immediate(rhs, width)) {
        const auto left = reg(reg_of(s, l), width);
        const auto c = immediate_of(rhs, width);

        if (op == binary_op_t::mul)
            insn(s, mnemonic, { c, left, left });
        else
            insn(s, mnemonic, { c, left });

        wrote(s, l, width, integer(t).is_signed);
        return l;
    }

    const auto r = rvalue(s, rhs);
    const auto [left, right] = operands(s, l, r);
    insn(s, mnemonic, { reg(right, width), reg(left, width) });

    release(s, r);
    wrote(s, l, width, integer(t).is_signed);

    return l;
}

bool
is_comparison(binary_op_t op)
{
    switch (op) {
    case binary_op_t::lt:
    case binary_op_t::gt:
    case binary_op_t::le:
    case binary_op_t::ge:
    case binary_op_t::eq:
    case binary_op_t::ne: return true;
    default: return false;
    }
}

// The comparison that is true exactly when op is false.
binary_op_t
inverse(binary_op_t op)
{
    switch (op) {
    case binary_op_t::lt: return binary_op_t::ge;
    case binary_op_t::ge: return binary_op_t::lt;
    case binary_op_t::gt: return binary_op_t::le;
    case binary_op_t::le: return binary_op_t::gt;
    case binary_op_t::eq: return binary_op_t::ne;
    default: return binary_op_t::eq;
    }
}

// The condition code of a comparison, the suffix of jcc and setcc:
// signed or unsigned by the operands' type, which the parser's
// conversions made common.
std::string_view
condition(binary_op_t op, bool is_signed)
{
    switch (op) {
    case binary_op_t::lt: return is_signed ? "l" : "b";
    case binary_op_t::le: return is_signed ? "le" : "be";
    case binary_op_t::gt: return is_signed ? "g" : "a";
    case binary_op_t::ge: return is_signed ? "ge" : "ae";
    case binary_op_t::eq: return "e";
    default: return "ne";
    }
}

// cmp R, L for a relational or equality operator, a constant right
// operand as an immediate.  The flags are set and the right value
// freed; the left one stays held, for setcc or for the caller to free.
value_t
compare(state_t &s, const binary_t &b)
{
    const int width = operation_width(b.lhs->type);
    const auto mnemonic = sized("cmp", width);

    const auto l = rvalue(s, *b.lhs);

    if (immediate(*b.rhs, width)) {
        insn(s, mnemonic,
             { immediate_of(*b.rhs, width), reg(reg_of(s, l), width) });
        return l;
    }

    const auto r = rvalue(s, *b.rhs);
    const auto [left, right] = operands(s, l, r);
    insn(s, mnemonic, { reg(right, width), reg(left, width) });

    release(s, r);

    return l;
}

// A comparison as a value: cmp, setcc into the low byte of L, movzbl
// to the int the operator yields.
value_t
comparison(state_t &s, const binary_t &b)
{
    const bool is_signed = integer(b.lhs->type).is_signed;

    const auto l = compare(s, b);
    const reg_t r = reg_of(s, l);

    insn(s, std::format("set{}", condition(b.op, is_signed)), { reg(r, 8) });
    insn(s, "movzbl", { reg(r, 8), reg(r, 32) });
    wrote(s, l, 32, true);

    return l;
}

// &&, || and ?: as a value: jump code into one register on both paths.
// Every held value is spilled first, as for a call, so the paths leave
// the register map as they found it whatever their operands do.
value_t
logical(state_t &s, const expr_t &e)
{
    spill_all(s);

    const int no = new_label(s);
    const int end = new_label(s);

    cbranch(s, e, no, false);

    const auto v = fresh(s, 32);
    const auto r = reg(reg_of(s, v), 32);

    insn(s, "movl", { imm(1), r });
    jump(s, end);

    place_label(s, no);
    insn(s, "movl", { imm(0), r });

    place_label(s, end);

    return v;
}

// 6.5.16p5: the arm not chosen is not evaluated, and the jump says so;
// never cmov.  Each arm's value is moved into the register of the
// first arm's.
value_t
conditional(state_t &s, const conditional_t &c, const type_t &t)
{
    spill_all(s);

    const int no = new_label(s);
    const int end = new_label(s);

    cbranch(s, *c.cond, no, false);

    if (is< void_t >(t)) {
        release(s, rvalue(s, *c.yes));
        jump(s, end);

        place_label(s, no);
        release(s, rvalue(s, *c.no));

        place_label(s, end);
        return { };
    }

    const int width = std::max(32, integer(t).width);

    const auto yes = rvalue(s, *c.yes);
    const reg_t into = reg_of(s, yes);
    const int valid = s.values[yes.id].valid;
    release(s, yes);
    jump(s, end);

    place_label(s, no);
    const auto other = rvalue(s, *c.no);
    const reg_t from = reg_of(s, other);
    int other_valid = s.values[other.id].valid;
    if (from != into) {
        insn(s, sized("mov", width), { reg(from, width), reg(into, width) });
        other_valid = written(width, integer(t).is_signed);
    }
    release(s, other);

    place_label(s, end);

    // valid to what both arms left
    return hold(s, into, std::min(valid, other_valid));
}

value_t
binary(state_t &s, const expr_t &e)
{
    const auto &b = as< binary_t >(e);

    switch (b.op) {
    case binary_op_t::log_and:
    case binary_op_t::log_or: return logical(s, e);
    case binary_op_t::comma:
        release(s, rvalue(s, *b.lhs));
        return rvalue(s, *b.rhs);
    default: break;
    }

    if (is_comparison(b.op))
        return comparison(s, b);

    // G13: the left operand first, held while the right one is generated
    const auto l = rvalue(s, *b.lhs);

    return arithmetic(s, b.op, l, *b.rhs, e.type);
}

// ++ and --, either side: the load, add or sub of 1, the store; G1, the
// machine wraps.  A char or a short is loaded extended to 32 bits and
// its low bytes stored, so no operator sees 8 or 16 bits.  The postfix
// forms copy the old value to another register first and yield it.
value_t
step(state_t &s, const unary_t &u)
{
    const auto *sym = object_of(*u.arg);
    const auto &t = integer(u.arg->type);
    const int displacement = slot_of(s, sym);
    const int width = std::max(32, t.width);

    const auto l = load(s, displacement, t, width);
    const reg_t r = reg_of(s, l);

    const bool up = u.op == unary_op_t::pre_inc || u.op == unary_op_t::post_inc;
    const bool post =
        u.op == unary_op_t::post_inc || u.op == unary_op_t::post_dec;

    value_t old;
    if (post) {
        old = fresh(s, s.values[l.id].valid, bit(r));
        insn(s, sized("mov", width),
             { reg(r, width), reg(reg_of(s, old), width) });
    }

    insn(s, sized(up ? "add" : "sub", width), { imm(1), reg(r, width) });
    store(s, r, displacement, t.width);
    s.values[l.id].valid = std::min(written(width, t.is_signed), t.width);

    if (!post)
        return l;

    release(s, l);

    return old;
}

value_t
unary(state_t &s, const unary_t &u, const type_t &t)
{
    switch (u.op) {
    // G1: neg of INT_MIN is INT_MIN
    case unary_op_t::neg:
    case unary_op_t::bit_not: {
        const int width = operation_width(t);
        const auto v = rvalue(s, *u.arg);
        insn(s, sized(u.op == unary_op_t::neg ? "neg" : "not", width),
             { reg(reg_of(s, v), width) });
        return v;
    }

    // as a value; the operand is not promoted, so it is compared at its
    // own width
    case unary_op_t::log_not: {
        const int width = integer(u.arg->type).width;
        const auto v = rvalue(s, *u.arg);
        const reg_t r = reg_of(s, v);
        insn(s, sized("cmp", width), { imm(0), reg(r, width) });
        insn(s, "sete", { reg(r, 8) });
        insn(s, "movzbl", { reg(r, 8), reg(r, 32) });
        return v;
    }

    case unary_op_t::pre_inc:
    case unary_op_t::pre_dec:
    case unary_op_t::post_inc:
    case unary_op_t::post_dec: return step(s, u);

    case unary_op_t::addr: not_yet("unary &");
    case unary_op_t::deref: not_yet("unary *");
    }

    throw std::logic_error("c1: an unknown unary operator");
}

// Plain or compound assignment; its value is the value stored, the low
// bytes of L.  A compound one loads the old value before the right
// operand is evaluated (G13), extended into the common type the parser
// chose, 6.5.17.3p4, and stores the result's low bytes back (G2).
value_t
assign(state_t &s, const assign_t &a)
{
    const auto *sym = object_of(*a.lhs);
    const auto &t = integer(a.lhs->type);
    const int displacement = slot_of(s, sym);

    if (!(a.rhs->type == a.common))
        throw std::logic_error("c1: an assignment across two types");

    if (!a.op) {
        const auto v = rvalue(s, *a.rhs);
        store(s, reg_of(s, v), displacement, t.width);
        return v;
    }

    // the value of the expression has the lvalue's type: the bits
    // above its width are the operation's, not the value's extension
    const auto l = load(s, displacement, t, operation_width(a.common));
    const auto v = arithmetic(s, *a.op, l, *a.rhs, a.common);
    store(s, reg_of(s, v), displacement, t.width);
    s.values[v.id].valid = std::min(s.values[v.id].valid, t.width);

    return v;
}

// An integer conversion.  G2: narrowing is no instruction, the narrower
// register name is the value; widening extends by the operand's
// signedness, and only when the register is not already valid to the
// new width, as it is after the load of a char or a 32-bit write of an
// unsigned value.  A change of signedness alone is nothing.  A cast to
// void discards the value.
value_t
cast(state_t &s, const cast_t &c, const type_t &to)
{
    if (is< void_t >(to)) {
        release(s, rvalue(s, *c.arg));
        return { };
    }

    const auto &from = integer(c.arg->type);
    const auto &into = integer(to);

    const auto v = rvalue(s, *c.arg);
    auto &h = s.values[v.id];

    if (into.width > from.width && h.valid < into.width) {
        const auto [mnemonic, width] =
            extension(from, std::max(32, into.width));
        const reg_t r = reg_of(s, v);
        insn(s, mnemonic, { reg(r, from.width), reg(r, width) });
        h.valid = written(width, from.is_signed);
    }

    h.valid = std::min(h.valid, into.width);

    return v;
}

// The function a call names; an indirect call is a later slice.
const symbol_t &
callee_of(const call_t &c)
{
    const auto &e = *c.callee;
    if (!is< name_t >(e) || as< name_t >(e).sym->storage != storage_t::function)
        not_yet("an indirect call");

    return *as< name_t >(e).sym;
}

// Chapter 8, every argument a hidden local of its call.  Every held
// value is spilled; each argument, in order and complete before the
// next (G13), is stored to its own slot at its width; then the stack
// arguments are pushed from their slots, last first, over a pad that
// keeps rsp a multiple of 16 at the call, and the first six are loaded
// into their registers, extended to 32 bits if narrower.  G16: the
// call is made with the call site's types and the convention's
// registers.  The result is in rax, the lowest free register, and a
// narrow one is read at its width, since the callee extended it.
value_t
call(state_t &s, const call_t &c, const type_t &t)
{
    const auto &callee = callee_of(c);
    const auto &f = as< function_t >(callee.type);

    // a pointer result is the next slice
    if (!is< void_t >(t))
        integer(t);

    spill_all(s);

    std::vector< std::pair< int, int_t > > slots;
    for (std::size_t i = 0; i < c.args.size(); ++i) {
        const auto &arg = c.args[i];
        const auto &type = integer(arg.type);

        if (i < f.params.size() && !(arg.type == *f.params[i]))
            throw std::logic_error("c1: an argument unlike its parameter");

        const auto v = rvalue(s, arg);
        const int displacement =
            allocate(s, size_of(arg.type), align_of(arg.type));
        store(s, reg_of(s, v), displacement, type.width);
        release(s, v);

        slots.emplace_back(displacement, type);
    }

    const std::size_t registers = std::size(argument_registers);
    const std::size_t stacked =
        slots.size() > registers ? slots.size() - registers : 0;
    const bool pad = stacked % 2 != 0;

    if (pad)
        insn(s, "subq", { imm(8), "%rsp" });

    // a slot narrower than 8 bytes is not pushed directly, which would
    // read the bytes above it
    for (std::size_t i = slots.size(); i-- > registers;) {
        const auto &[displacement, type] = slots[i];

        if (type.width == 64) {
            insn(s, "pushq", { at(displacement) });
            continue;
        }

        const auto [mnemonic, width] = extension(type, 32);
        insn(s, mnemonic, { at(displacement), reg(rax, width) });
        insn(s, "pushq", { "%rax" });
    }

    for (std::size_t i = 0; i < std::min(registers, slots.size()); ++i) {
        const auto &[displacement, type] = slots[i];

        const auto [mnemonic, width] =
            extension(type, std::max(32, type.width));
        insn(s, mnemonic,
             { at(displacement), reg(argument_registers[i], width) });
    }

    // the upper bound of the vector registers used, none
    if (f.variadic)
        insn(s, "movl", { imm(0), "%eax" });

    // every call through the PLT: the executable is position-independent
    insn(s, "call", { callee.name + "@PLT" });

    const long popped = 8 * static_cast< long >(stacked) + (pad ? 8 : 0);
    if (popped > 0)
        insn(s, "addq", { imm(popped), "%rsp" });

    s.calls = true;

    if (is< void_t >(t))
        return { };

    // the callee extended a narrow result to 32 bits; above a 32-bit
    // result the convention promises nothing
    return hold(s, rax, std::max(32, integer(t).width));
}

// The value of an expression in a register, its code written first.
// No value has a pointer or array type in this slice.
value_t
rvalue(state_t &s, const expr_t &e)
{
    if (is< pointer_t >(e.type) || is< array_t >(e.type))
        not_yet("a pointer or an array");

    if (is< literal_t >(e))
        return literal(s, as< literal_t >(e), e.type);

    if (is< name_t >(e)) {
        const auto *sym = object_of(e);
        const auto &t = integer(e.type);
        return load(s, slot_of(s, sym), t, std::max(32, t.width));
    }

    if (is< string_t >(e))
        not_yet("a string literal");

    if (is< unary_t >(e))
        return unary(s, as< unary_t >(e), e.type);

    if (is< binary_t >(e))
        return binary(s, e);

    if (is< assign_t >(e))
        return assign(s, as< assign_t >(e));

    if (is< call_t >(e))
        return call(s, as< call_t >(e), e.type);

    if (is< cast_t >(e))
        return cast(s, as< cast_t >(e), e.type);

    return conditional(s, as< conditional_t >(e), e.type);
}

// Jump code, chapter 7.1: a jump to label when e is true (sense) or
// false (!sense), a fall through otherwise, and no value.  A comparison
// is cmp and jcc; ! flips the sense; && and || are the shape of the
// short circuit, 6.5.14p4, 6.5.15p4, the right operand not reached when
// the left decides.  Any other scalar, a constant included, is its
// value compared with 0 at its own width.
void
cbranch(state_t &s, const expr_t &e, int label, bool sense)
{
    if (is< unary_t >(e) && as< unary_t >(e).op == unary_op_t::log_not) {
        cbranch(s, *as< unary_t >(e).arg, label, !sense);
        return;
    }

    if (is< binary_t >(e)) {
        const auto &b = as< binary_t >(e);

        if (is_comparison(b.op)) {
            const bool is_signed = integer(b.lhs->type).is_signed;
            release(s, compare(s, b));

            const auto op = sense ? b.op : inverse(b.op);
            insn(s, std::format("j{}", condition(op, is_signed)),
                 { label_name(label) });
            return;
        }

        if (b.op == binary_op_t::log_and || b.op == binary_op_t::log_or) {
            // && false and || true: either operand decides, to label
            const bool is_and = b.op == binary_op_t::log_and;
            if (sense != is_and) {
                cbranch(s, *b.lhs, label, sense);
                cbranch(s, *b.rhs, label, sense);
                return;
            }

            // && true and || false: the left operand decides the other
            // way, past the right one
            const int skip = new_label(s);
            cbranch(s, *b.lhs, skip, !sense);
            cbranch(s, *b.rhs, label, sense);
            place_label(s, skip);
            return;
        }
    }

    const int width = integer(e.type).width;
    const auto v = rvalue(s, e);
    insn(s, sized("cmp", width), { imm(0), reg(reg_of(s, v), width) });
    release(s, v);

    insn(s, sense ? "jne" : "je", { label_name(label) });
}

// statements

void statement(state_t &, const stmt_t &);

void
expr_stmt(state_t &s, const expr_stmt_t &x)
{
    if (x.e)
        release(s, rvalue(s, *x.e));
}

// G12: the slot is taken where the declaration is met, one for the
// whole activation, and before the initializer, whose scope already
// holds the name, 6.2.1p7.  G5: without an initializer nothing is
// stored; the slot holds what it holds.
void
decl_stmt(state_t &s, const decl_stmt_t &d)
{
    const auto *sym = d.sym;
    if (sym->in_register)
        not_yet("a register variable");

    const auto &t = integer(sym->type);
    const int displacement =
        allocate(s, size_of(sym->type), align_of(sym->type));
    s.slots.emplace(sym, displacement);

    if (!d.init)
        return;

    const auto v = rvalue(s, *d.init);
    store(s, reg_of(s, v), displacement, t.width);
    release(s, v);
}

void
block_stmt(state_t &s, const block_t &b)
{
    for (const auto &st : b.body)
        statement(s, st);
}

void
if_stmt(state_t &s, const if_t &i)
{
    const int no = new_label(s);
    cbranch(s, i.cond, no, false);

    statement(s, *i.yes);

    if (!i.no) {
        place_label(s, no);
        return;
    }

    const int end = new_label(s);
    jump(s, end);

    place_label(s, no);
    statement(s, *i.no);

    place_label(s, end);
}

// G14: the condition at the top, where it is written, tested once per
// iteration and once more; no rotation and no reasoning about it.
void
while_stmt(state_t &s, const while_t &w)
{
    const int cond = new_label(s);
    const int end = new_label(s);

    place_label(s, cond);
    cbranch(s, w.cond, end, false);

    statement(s, *w.body);
    jump(s, cond);

    place_label(s, end);
}

// G14, as for while.  The step has its label, for continue.
void
for_stmt(state_t &s, const for_t &f)
{
    if (f.init)
        statement(s, *f.init);

    const int cond = new_label(s);
    const int next = new_label(s);
    const int end = new_label(s);

    place_label(s, cond);
    if (f.cond)
        cbranch(s, *f.cond, end, false);

    statement(s, *f.body);

    place_label(s, next);
    if (f.step)
        release(s, rvalue(s, *f.step));
    jump(s, cond);

    place_label(s, end);
}

// The value goes to rax, and a char or short result not already valid
// to 32 bits is extended in eax by its signedness: the callee extends,
// so a caller that reads 32 bits reads the value.  Every return jumps
// to the one epilogue, the last statement of the body included: the
// jump to the next instruction is what the statement says, and
// removing it was the optional peephole pass's work in the PDP-11
// compiler, which rcc has no counterpart of.
void
return_stmt(state_t &s, const return_t &r)
{
    if (r.value) {
        const auto &t = integer(r.value->type);
        const int width = std::max(32, t.width);

        const auto v = rvalue(s, *r.value);
        const reg_t from = reg_of(s, v);
        if (from != rax)
            insn(s, sized("mov", width), { reg(from, width), reg(rax, width) });

        if (s.values[v.id].valid < 32) {
            const auto [mnemonic, into] = extension(t, 32);
            insn(s, mnemonic, { reg(rax, t.width), reg(rax, into) });
        }

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
        if_stmt(s, as< if_t >(st));
    else if (is< while_t >(st))
        while_stmt(s, as< while_t >(st));
    else if (is< for_t >(st))
        for_stmt(s, as< for_t >(st));
    else
        return_stmt(s, as< return_t >(st));

    if (s.live != 0)
        throw std::logic_error("c1: a value held across a statement");
}

// functions

// The parameters' half of the prologue, before the body, since the
// body's slots come after theirs.  Each of the first six takes the next
// slot and is stored there from its argument register with the mov of
// its width; the seventh and later are where the caller pushed them,
// 16(%rbp) and up, 8 bytes apart, and are read from there.
void
parameters(state_t &s, const function_decl_t &fd)
{
    const std::size_t registers = std::size(argument_registers);

    for (std::size_t i = 0; i < fd.params.size(); ++i) {
        const auto *p = fd.params[i];
        if (p->in_register)
            not_yet("a register parameter");

        const auto &t = integer(p->type);

        if (i >= registers) {
            s.slots.emplace(p, 16 + 8 * static_cast< int >(i - registers));
            continue;
        }

        const int displacement =
            allocate(s, size_of(p->type), align_of(p->type));
        s.slots.emplace(p, displacement);
        store(s, argument_registers[i], displacement, t.width);
    }
}

// Chapter 5: rbp is the frame pointer in every function, and the frame
// is the slots' total rounded up to 16 bytes, so rsp is a multiple of
// 16 at every call.  A function that calls nothing keeps a frame of up
// to 128 bytes in the red zone and moves no rsp.
void
prologue(state_t &s, int frame)
{
    s.out << "\tpushq\t%rbp\n"
             "\t.cfi_def_cfa_offset 16\n"
             "\t.cfi_offset %rbp, -16\n"
             "\tmovq\t%rsp, %rbp\n"
             "\t.cfi_def_cfa_register %rbp\n";

    if (frame > 0 && (s.calls || frame > 128))
        s.out << std::format("\tsubq\t${}, %rsp\n", frame);
}

// A definition.  The body is generated first, into the buffer, and the
// prologue written after it, when the frame's size is known.  The one
// epilogue is reached with rax as it is, G5's some value, but for main,
// which returns 0 when it falls off its end, 5.1.2.3.4.
void
function(state_t &s, const function_decl_t &fd)
{
    s.body.str({ });
    s.slots.clear();
    s.frame = 0;
    s.calls = false;
    s.owner.fill(-1);
    s.values.clear();
    s.age = 0;
    s.live = 0;
    s.function = fd.sym->name;
    s.ret = new_label(s);
    s.terminated = false;

    // a pointer result is the next slice
    const auto &result = *as< function_t >(fd.sym->type).result;
    if (!is< void_t >(result))
        integer(result);

    parameters(s, fd);
    block_stmt(s, *fd.body);

    if (!s.terminated && s.function == "main")
        insn(s, "movl", { imm(0), "%eax" });

    const auto &name = s.function;
    s.out << std::format("\t.globl\t{}\n"
                         "\t.type\t{}, @function\n"
                         "{}:\n"
                         "\t.cfi_startproc\n",
                         name, name, name);

    prologue(s, (s.frame + 15) / 16 * 16);

    s.out << s.body.str();

    s.out << std::format("{}:\n"
                         "\tleave\n"
                         "\t.cfi_def_cfa %rsp, 8\n"
                         "\tret\n"
                         "\t.cfi_endproc\n"
                         "\t.size\t{}, .-{}\n",
                         label_name(s.ret), name, name);
}

// The file name as a .file directive's string.
std::string
quoted_file(std::string_view file)
{
    const auto slash = file.rfind('/');
    if (slash != std::string_view::npos)
        file.remove_prefix(slash + 1);

    std::string text = "\"";
    for (const char c : file) {
        if (c == '"' || c == '\\')
            text += '\\';
        text += c;
    }

    return text + '"';
}

} // namespace

// The functions the unit defines, in order; one it only declares
// writes nothing, since the assembler leaves its symbol to the link.
// The note section marks the stack not executable.
void
generate(const unit_t &unit, std::string_view file, std::ostream &out)
{
    state_t s { .out = out };

    out << std::format("\t.file\t{}\n\t.text\n", quoted_file(file));

    for (const auto &fd : unit.functions)
        if (fd.body)
            function(s, fd);

    out << "\t.section\t.note.GNU-stack,\"\",@progbits\n";
}

} // namespace rcc
