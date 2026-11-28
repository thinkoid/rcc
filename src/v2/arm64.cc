// -*- mode: c++; -*-

// aarch64's text, declared in arm64.hh: assembly for clang and GNU as,
// as rcc writes it.  docs/aarch64.md is the design, with
// docs/backend.md where it says the model carries, and each function
// names the row or the part of the frame it writes.  The frame is the
// frame record and the locals' slots above it, chapter 5.

#include "arm64.hh"

#include "assembly.hh"

#include <algorithm>
#include <bit>
#include <cstddef>
#include <format>
#include <iterator>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

namespace rcc::arm64 {

namespace {

// The fifteen expression registers, chapter 4, in the order a new
// value takes them.
constexpr reg_t expression_registers[] = { x0, x1,  x2,  x3,  x4,  x5,  x6, x7,
                                           x9, x10, x11, x12, x13, x14, x15 };

// The convention's argument registers, in order, chapter 3.
constexpr reg_t argument_registers[] = { x0, x1, x2, x3, x4, x5, x6, x7 };

// The convention's result register, chapter 3.
constexpr reg_t result_register = x0;

// The frame record, the caller's x29 and x30, at the bottom of the
// frame, chapter 5.1.
constexpr long frame_record = 16;

// The largest displacement off x29 an access of size bytes encodes,
// 2.3: 4095 times the size, the scaled form.
constexpr long
reach(long size)
{
    return 4095 * size;
}

// operands

// A register by its name at a width in bits: the w name for 32 and
// below, the x name for 64.
std::string
reg(reg_t r, int width)
{
    return std::format("{}{}", width == 64 ? 'x' : 'w', static_cast< int >(r));
}

// The zero register by its name at a width in bits, chapter 2.1: wzr,
// xzr.
std::string_view
zero(int width)
{
    return width == 64 ? "xzr" : "wzr";
}

// An immediate, as the assembler reads it.
std::string
imm(long long value)
{
    return std::format("#{}", value);
}

// A slot as the address operand of an access, 5.3: [x29, #d], which
// allocate() keeps in reach of an access of the slot's size.
std::string
slot_operand(int displacement)
{
    return std::format("[x29, #{}]", displacement);
}

// The displacement the local's declaration recorded; a local read
// before it is a tree the parser does not make.
int
slot_of(const state_t &s, slot_t where)
{
    const auto it = s.slots.find(where.id);
    if (it == s.slots.end())
        throw std::logic_error("v2: a name read before its declaration");

    return it->second;
}

// writing

// The copy of a register into another at a width, the destination
// first, 2.2, rcc's mov(): mov w0, w19, mov x0, x1.
void
mov(state_t &s, reg_t from, reg_t to, int width)
{
    insn(s, "mov", { reg(to, width), reg(from, width) });
}

// The load of 2.2 that brings a value of the view, read at the sign, to
// a register of 32 or 64 bits, and the width of its destination, rcc's
// load_of().  A narrow load extends by its mnemonic; to 64 a signed one
// is the x form of the same mnemonic, ldrsw for an int, and an unsigned
// one is the w form, since a write to the w name clears the upper half.
std::pair< std::string_view, int >
load_of(view_t view, sign_t sign, int to)
{
    if (view == view_t::i64)
        return { "ldr", 64 };

    if (view == view_t::i32 && (to == 32 || sign == sign_t::unsigned_))
        return { "ldr", 32 };

    if (view == view_t::i32)
        return { "ldrsw", 64 };

    if (sign == sign_t::unsigned_)
        return { view == view_t::i8 ? "ldrb" : "ldrh", 32 };

    return { view == view_t::i8 ? "ldrsb" : "ldrsh", to };
}

// The store of 2.2 at the destination's width, rcc's store_of(); G2,
// the low bytes of the register are the value.
std::string_view
store_of(view_t view)
{
    switch (view) {
    case view_t::i8: return "strb";
    case view_t::i16: return "strh";
    default: return "str";
    }
}

// The plain load of the view, the store's mirror, rcc's
// plain_load_of(): ldrb, ldrh or ldr, the load of the unsigned type.  A
// copy stores the same width back, so what the load writes above the
// bytes is never read.
std::string_view
plain_load_of(view_t view)
{
    switch (view) {
    case view_t::i8: return "ldrb";
    case view_t::i16: return "ldrh";
    default: return "ldr";
    }
}

// x17, which no value is held in, by its name at the view's width: w17,
// or x17 for 64 bits.
std::string_view
x17_at(view_t view)
{
    return view == view_t::i64 ? "x17" : "w17";
}

// A copy from memory to memory, which the machine has no instruction
// for, carries its bytes through x17, chapter 4: the plain load of the
// view, then the store of the same view, rcc's copy_in() and
// copy_out(): ldrb w17, [x29, #64] and strb w17, [x29, #48].
void
copy_in(state_t &s, std::string_view from, view_t view)
{
    insn(s, plain_load_of(view), { x17_at(view), from });
}

void
copy_out(state_t &s, std::string_view to, view_t view)
{
    insn(s, store_of(view), { x17_at(view), to });
}

// The register-to-register extension that brings a value of the view
// from, read at the sign, to a width of 32 or 64 bits, and the width it
// leaves the value valid to, rcc's extension(), 6.3: sxtb, sxth or sxtw
// at the new width for a signed value; uxtb, uxth, or mov wL, wL for an
// unsigned int, on the w name, whose write clears the upper half and so
// extends to 64 as well.
std::pair< std::string_view, int >
extension(view_t from, sign_t sign, int to)
{
    if (sign == sign_t::signed_) {
        switch (from) {
        case view_t::i8: return { "sxtb", to };
        case view_t::i16: return { "sxth", to };
        default: return { "sxtw", 64 };
        }
    }

    switch (from) {
    case view_t::i8: return { "uxtb", 64 };
    case view_t::i16: return { "uxth", 64 };
    default: return { "mov", 64 };
    }
}

// constants

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

// Whether add and sub encode a constant of an operation of width bits,
// 2.4: 0 to 4095, or that shifted left by 12, or the negative of
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

// operators

// The instruction of a binary operator, rcc's arithmetic_mnemonic() and
// shift_mnemonic(): asr and lsr for >>, sdiv and udiv for /, and for %
// the quotient's, before its msub.
std::string_view
mnemonic(binary_op_t op)
{
    switch (op) {
    case binary_op_t::add: return "add";
    case binary_op_t::sub: return "sub";
    case binary_op_t::mul: return "mul";
    case binary_op_t::sdiv:
    case binary_op_t::srem: return "sdiv";
    case binary_op_t::udiv:
    case binary_op_t::urem: return "udiv";
    case binary_op_t::shl: return "lsl";
    case binary_op_t::ashr: return "asr";
    case binary_op_t::lshr: return "lsr";
    case binary_op_t::slt:
    case binary_op_t::sle:
    case binary_op_t::sgt:
    case binary_op_t::sge:
    case binary_op_t::ult:
    case binary_op_t::ule:
    case binary_op_t::ugt:
    case binary_op_t::uge:
    case binary_op_t::eq:
    case binary_op_t::ne:
    case binary_op_t::log_and:
    case binary_op_t::log_or:
        throw std::logic_error("v2: an operator of no arithmetic instruction");
    case binary_op_t::bit_and: return "and";
    case binary_op_t::bit_or: return "orr";
    case binary_op_t::bit_xor: return "eor";
    }
}

// The immediate of op that encodes value at width bits, 2.4, or none:
// add and sub take 12 bits, or 12 bits shifted by 12, of either sign,
// written signed, and so does cmp, the instruction of every comparison;
// a shift its count masked to the width, which is what the instruction
// does to a count in a register (G3), lsl w0, w0, #1 for a << 33; and,
// orr and eor a bitmask, written in hex; mul, sdiv and msub take none,
// and && and || have no instruction.
std::optional< std::string >
immediate_of(binary_op_t op, long long value, int width)
{
    switch (op) {
    case binary_op_t::add:
    case binary_op_t::sub:
    case binary_op_t::slt:
    case binary_op_t::sle:
    case binary_op_t::sgt:
    case binary_op_t::sge:
    case binary_op_t::ult:
    case binary_op_t::ule:
    case binary_op_t::ugt:
    case binary_op_t::uge:
    case binary_op_t::eq:
    case binary_op_t::ne:
        if (arithmetic_immediate(value, width))
            return imm(value);
        break;
    case binary_op_t::mul:
    case binary_op_t::sdiv:
    case binary_op_t::udiv:
    case binary_op_t::srem:
    case binary_op_t::urem:
    case binary_op_t::log_and:
    case binary_op_t::log_or: break;
    case binary_op_t::shl:
    case binary_op_t::ashr:
    case binary_op_t::lshr: return imm(value & (width - 1));
    case binary_op_t::bit_and:
    case binary_op_t::bit_or:
    case binary_op_t::bit_xor:
        if (bitmask_immediate(value, width))
            return bitmask_of(value, width);
        break;
    }

    return std::nullopt;
}

// The register of wanted, the set of at most one expression register
// that the consumer of v's value names, x0 for a return's (6.1), where
// the instruction about to write v may write it, rcc's
// wanted_register(): free, or v's own, since the instruction reads
// every source before it writes, add w0, w1, w0.  None for an empty
// set, and for a register that holds another live value, which the
// instruction does not read.
std::optional< reg_t >
wanted_register(const state_t &s, value_t v, unsigned wanted)
{
    for (const reg_t r : expression_registers)
        if (wanted & bit(r)) {
            const auto it = s.registers.owner.find(r);
            return it == s.registers.owner.end() || it->second == v.id ?
                       std::optional< reg_t >(r) :
                       std::nullopt;
        }

    return std::nullopt;
}

// The register a row's last instruction writes, v's from here on,
// rcc's final_destination(): the register the consumer names, wanted,
// where the instruction may write it, and the register v held is
// free, since the instruction has read it; otherwise v's own.  The
// register is written after the row has read every operand, so no
// condition on them is needed.  A value held across a branch is not
// moved, check_movable(), as rcc's named_destination() has it.
reg_t
final_destination(state_t &s, value_t v, unsigned wanted)
{
    const auto named = wanted_register(s, v, wanted);
    if (!named)
        return reg_of(s, v);

    check_movable(s, v);
    s.registers.owner.erase(reg_of(s, v));
    s.registers.values[v.id].reg = *named;
    s.registers.owner.emplace(*named, v.id);

    return *named;
}

// The condition of a comparison of op, the suffix of b.cc and the
// operand of cset, rcc's condition(): lt le gt ge for the signed ones,
// lo ls hi hs for the unsigned ones.
std::string_view
condition(binary_op_t op)
{
    switch (op) {
    case binary_op_t::slt: return "lt";
    case binary_op_t::sle: return "le";
    case binary_op_t::sgt: return "gt";
    case binary_op_t::sge: return "ge";
    case binary_op_t::ult: return "lo";
    case binary_op_t::ule: return "ls";
    case binary_op_t::ugt: return "hi";
    case binary_op_t::uge: return "hs";
    case binary_op_t::eq: return "eq";
    default: return "ne";
    }
}

} // namespace

// The next slot above the last, of size bytes at align, a local's or a
// spill's; its displacement off x29.  G12: a slot is never shared and
// never reused inside the activation.  A slot past the reach of an
// access of its size is reached through x16 in rcc, 5.3, which rcc-v2
// does not yet; below it the frame is far from the target's limit.
int
allocate(state_t &s, long size, int align)
{
    const long displacement = (s.frame + align - 1) / align * align;
    if (displacement > reach(size))
        throw std::logic_error(
            "v2: not yet: a slot beyond the reach of [x29, #d]");

    s.frame = displacement + size;

    return static_cast< int >(displacement);
}

// The two instructions the register stack writes, called by name from
// register-stack.hh.  The store of a register to a spill slot, all 8
// bytes, 6.2: str x0, [x29, #24].
void
store_spill(state_t &s, reg_t r, int displacement)
{
    insn(s, "str", { reg(r, 64), slot_operand(displacement) });
}

// The reload of a register from a spill slot, all 8 bytes, 6.2.
void
reload_spill(state_t &s, reg_t r, int displacement)
{
    insn(s, "ldr", { reg(r, 64), slot_operand(displacement) });
}

// A register for a value, rcc's take(): the lowest free one outside
// avoid and s.reserved, else the lowest free one outside avoid, or,
// when every one is held, the register of the oldest held value outside
// avoid, the lowest number, which is spilled first.  The reservation is
// an order of preference, not a pin.
reg_t
take(state_t &s, unsigned avoid)
{
    for (const unsigned out : { avoid | s.reserved, avoid })
        for (const reg_t r : expression_registers)
            if (!s.registers.owner.contains(r) && !(out & bit(r)))
                return r;

    std::optional< reg_t > victim;
    for (const reg_t r : expression_registers) {
        if (avoid & bit(r))
            continue;

        if (!victim || s.registers.owner.at(r) < s.registers.owner.at(*victim))
            victim = r;
    }

    if (!victim)
        throw std::logic_error("v2: no register to spill");
    spill(s, *victim);

    return *victim;
}

// Labels are placed in source order, and nothing is reordered after
// them, backend.md 7.2.
void
place_label(state_t &s, int n)
{
    s.body << label_name(s.function, n) << ":\n";
    s.terminated = false;
}

void
jump(state_t &s, int n)
{
    insn(s, "b", { label_name(s.function, n) });
    s.terminated = true;
}

void
align_loop_head(state_t &)
{
}

// b.cc of the condition, 7.1.
void
jump_if(state_t &s, binary_op_t op, int label)
{
    insn(s, std::format("b.{}", condition(op)),
         { label_name(s.function, label) });
}

void
test_zero(state_t &s,
          value_t v,
          view_t view,
          int valid,
          std::optional< int > label,
          bool sense)
{
    const reg_t r = reg_of(s, v);

    if (valid < 32) {
        const long long low_bits = (1ll << width(view)) - 1;
        insn(s, "tst", { reg(r, 32), bitmask_of(low_bits, 32) });

        if (label)
            insn(s, sense ? "b.ne" : "b.eq",
                 { label_name(s.function, *label) });
        return;
    }

    if (!label) {
        insn(s, "cmp", { reg(r, width(view)), imm(0) });
        return;
    }

    insn(s, sense ? "cbnz" : "cbz",
         { reg(r, width(view)), label_name(s.function, *label) });
}

// rcc's logical() writes the mov itself, never constant()'s choice of
// mov forms, which writes the same text for 0 and 1.
void
move_immediate(state_t &s, reg_t r, int value)
{
    insn(s, "mov", { reg(r, 32), imm(value) });
}

// The constant row of 6.4, at 32 bits or 64, a narrow constant as its
// promotion.  G2: the low bits are the value.
value_t
constant_value(state_t &s, long long value, view_t view)
{
    const value_t v = fresh(s);
    constant(s, reg_of(s, v), value, promoted_width(view));

    return v;
}

address_t
address(state_t &s, slot_t where)
{
    return { .displacement = slot_of(s, where) };
}

address_t
address(state_t &s, slot_t where, between_t)
{
    return address(s, where);
}

namespace {

// The address of an object the unit only declares, from the GOT, rcc's
// symbol_address() for one it does not define, chapter 10: it may be in
// a shared library, which a PC-relative reference does not reach from a
// position-independent executable, so its address is the one the
// dynamic linker wrote, loaded into a register taken for it, adrp x0,
// :got:g and ldr x0, [x0, :got_lo12:g].
value_t
symbol_address(state_t &s, const global_t &g)
{
    const value_t v = fresh(s);
    const auto r = reg(reg_of(s, v), 64);

    insn(s, "adrp", { r, std::format(":got:{}", g.name) });
    insn(s, "ldr", { r, std::format("[{}, :got_lo12:{}]", r, g.name) });

    return v;
}

} // namespace

// Each access the source writes loads the address once: g = g + 1 is
// two pairs from the GOT, or two adrp x16 where the unit defines g.
address_t
address(state_t &s, const global_t &where)
{
    if (s.defined.contains(where.name))
        return { .label = where.name };

    return { .base = symbol_address(s, where) };
}

address_t
address(state_t &s, const global_t &where, between_t)
{
    return address(s, where);
}

// The pointer's address is formed where the object's load would be,
// outside what the caller reserves around address().
address_t
address(state_t &s, const indirect_t &where, between_t between)
{
    return pointed(s, where.pointer, between, where.size);
}

namespace {

// The register a held value is in, as a set for take() to avoid; none
// for a value spilled.
unsigned
held_in(const state_t &s, value_t v)
{
    const auto r = s.registers.values[v.id].reg;

    return r ? bit(*r) : 0;
}

// The index as the last operand of an access, or of add and sub, which
// take the same forms, rcc's index_operand(), 2.3: x2, lsl #2, or w2,
// sxtw #2, and the forms without the shift, x2 and w2, sxtw.
std::string
index_operand(reg_t r, extend_t extend, int shift)
{
    if (extend == extend_t::lsl && shift == 0)
        return reg(r, 64);

    if (extend == extend_t::lsl)
        return std::format("{}, lsl #{}", reg(r, 64), shift);

    const auto name = extend == extend_t::sxtw ? "sxtw" : "uxtw";
    if (shift == 0)
        return std::format("{}, {}", reg(r, 32), name);

    return std::format("{}, {} #{}", reg(r, 32), name, shift);
}

} // namespace

unsigned
registers_of(const state_t &s, const address_t &a)
{
    return (a.base ? held_in(s, *a.base) : 0) |
           (a.index ? held_in(s, *a.index) : 0);
}

// A displacement is written and an index is not where both are set, as
// rcc writes them; no maker sets both.
std::string
operand(state_t &s, const address_t &a, unsigned avoid)
{
    if (a.constant)
        throw std::logic_error("v2: a constant index not held");

    if (!a.label.empty()) {
        insn(s, "adrp", { "x16", a.label });
        return std::format("[x16, :lo12:{}]", a.label);
    }

    if (!a.base)
        return slot_operand(a.displacement);

    avoid |= registers_of(s, a);

    std::string index;
    if (a.index) {
        const reg_t r = reg_of(s, *a.index, avoid);
        avoid |= bit(r);
        index = ", " + index_operand(r, a.extend, a.shift);
    }

    const std::string base = reg(reg_of(s, *a.base, avoid), 64);
    if (a.displacement != 0)
        return std::format("[{}, #{}]", base, a.displacement);

    return std::format("[{}{}]", base, index);
}

void
release(state_t &s, const address_t &a)
{
    if (a.base)
        release(s, *a.base);

    if (a.index)
        release(s, *a.index);
}

address_t
held_index(state_t &s, address_t a, unsigned avoid)
{
    if (!a.constant)
        return a;

    const value_t held = hold(s, take(s, avoid | registers_of(s, a)));
    constant(s, reg_of(s, held), *a.constant, 64);
    a.index = held;
    a.constant.reset();

    return a;
}

namespace {

// A slot's address, x29 plus its displacement, into the register r,
// 5.3, rcc's frame_address(): add r, x29, #d, where the add's 12 bits
// encode d.  rcc writes two adds past them, which rcc-v2 does not yet.
void
frame_address(state_t &s, reg_t r, int displacement)
{
    if (displacement > 0xfff)
        throw std::logic_error(
            "v2: not yet: a slot's address beyond 4095 bytes of x29");

    insn(s, "add", { reg(r, 64), "x29", imm(displacement) });
}

} // namespace

value_t
address_of(state_t &s, const address_t &a)
{
    const value_t v = fresh(s);
    frame_address(s, reg_of(s, v), a.displacement);

    return v;
}

namespace {

// The load of the object at an address at the view, by the sign, into
// a register taken for it at a width of 32 or 64 bits, load_of()'s,
// rcc's load(): through a base, the operand formed and the base given
// back first, so the value may take the base's register, ldr w0, [x0];
// otherwise the register taken first, since the take may spill, and the
// operand formed after it, so a global's adrp x16 stands right before
// its access.  G8: one ldr.
value_t
load_to(state_t &s, const address_t &a, view_t view, sign_t sign, int to)
{
    const auto [mnemonic, bits] = load_of(view, sign, to);

    if (a.base) {
        const std::string from = operand(s, a);
        release(s, a);

        const value_t v = fresh(s);
        insn(s, mnemonic, { reg(reg_of(s, v), bits), from });

        return v;
    }

    const value_t v = fresh(s);
    const reg_t r = reg_of(s, v);

    const std::string from = operand(s, a);
    insn(s, mnemonic, { reg(r, bits), from });

    return v;
}

} // namespace

// The load row of 2.2: ldrsb w0, [x29, #16], ldr w0, [x29, #16], ldr x0,
// [x29, #16].
value_t
load(state_t &s, const address_t &a, view_t view, sign_t sign)
{
    return load_to(s, a, view, sign, promoted_width(view));
}

value_t
extending_load(state_t &s, const address_t &a, view_t from)
{
    return load_to(s, a, from, sign_t::signed_, 64);
}

// rcc's extended(): the extension() of the value's register, named at
// 32 bits, into final_destination(), named at the new width for a
// signed value and at 32 for an unsigned one: sxtw x0, w0, sxtb w0, w0,
// uxtb w0, w0, mov w0, w0.
value_t
extended(
    state_t &s, value_t v, view_t from, sign_t sign, view_t to, unsigned wanted)
{
    const auto [mnemonic, valid] = extension(from, sign, promoted_width(to));
    const reg_t source = reg_of(s, v);
    const reg_t r = final_destination(s, v, wanted);
    const int bits = sign == sign_t::signed_ ? valid : 32;

    insn(s, mnemonic, { reg(r, bits), reg(source, 32) });

    return v;
}

// neg and mvn, rcc's unary(): neg w0, w0, mvn x0, x0, and neg w0, w1
// into the register the consumer names.  G1: neg of the most negative
// value is itself.
value_t
negated(state_t &s, unary_op_t op, value_t v, view_t view, unsigned wanted)
{
    const auto from = reg(reg_of(s, v), width(view));
    const auto to = reg(final_destination(s, v, wanted), width(view));
    insn(s, op == unary_op_t::neg ? "neg" : "mvn", { to, from });

    return v;
}

// op L, L, R, three addresses, rcc's arithmetic() and divide(): the two
// registers of operands(), a spilled value reloaded, R released, then
// the instruction into final_destination(), add w0, w0, w1.  A shift's
// count in a register is named at the width of the value shifted, lsl
// w0, w0, w1, and the instruction reads its low 5 bits (G3).  G4: sdiv
// does not trap, 2.6: x / 0 is 0 and INT_MIN / -1 is INT_MIN.  % has no
// instruction: the quotient into a register N taken outside L and R,
// which both hold values, and not held, then R released and msub, L -
// N * R, sdiv w2, w0, w1 and msub w0, w2, w1, w0.
value_t
arithmetic(state_t &s,
           binary_op_t op,
           value_t l,
           value_t r,
           view_t view,
           unsigned wanted)
{
    const int bits = width(view);

    const auto [left, right] = operands(s, l, r);
    const auto from = reg(left, bits);
    const auto by = reg(right, bits);

    if (!is_remainder(op)) {
        release(s, r);

        const auto to = reg(final_destination(s, l, wanted), bits);
        insn(s, mnemonic(op), { to, from, by });

        return l;
    }

    const auto quotient = reg(take(s, bit(left) | bit(right)), bits);
    insn(s, mnemonic(op), { quotient, from, by });

    release(s, r);

    const auto to = reg(final_destination(s, l, wanted), bits);
    insn(s, "msub", { to, quotient, by, from });

    return l;
}

// A constant is the immediate where the instruction encodes it, add w0,
// w0, #5, and is otherwise the right operand of the row on two held
// values, its value in a register of its own: mov w1, #4097, add w0,
// w0, w1.
value_t
arithmetic(state_t &s,
           binary_op_t op,
           value_t l,
           long long constant,
           view_t view,
           unsigned wanted)
{
    const int bits = width(view);

    const auto immediate = immediate_of(op, constant, bits);
    if (!immediate)
        return arithmetic(s, op, l, constant_value(s, constant, view), view,
                          wanted);

    const auto from = reg(reg_of(s, l), bits);
    const auto to = reg(final_destination(s, l, wanted), bits);
    insn(s, mnemonic(op), { to, from, *immediate });

    return l;
}

namespace {

// The store of the held value v through the address at, held, rcc's
// store(): v's register, reloaded outside the address's, then the
// address's operand outside v's.
void
stored(state_t &s, value_t v, const address_t &at, view_t view)
{
    const reg_t r = reg_of(s, v, registers_of(s, at));
    store(s, r, operand(s, at, bit(r)), view);
}

} // namespace

value_t
compound(state_t &s,
         binary_op_t op,
         const address_t &at,
         view_t view,
         sign_t sign,
         view_t operation,
         long long constant,
         unsigned wanted)
{
    const int bits = width(operation);

    const auto immediate = immediate_of(op, constant, bits);
    if (!immediate)
        return compound(s, op, at, view, sign, operation,
                        constant_value(s, constant, operation), wanted);

    const value_t l = fetch(s, at, view, sign, bits);
    const auto from = reg(reg_of(s, l), bits);
    const auto to = reg(final_destination(s, l, wanted), bits);
    insn(s, mnemonic(op), { to, from, *immediate });
    stored(s, l, at, view);
    release(s, at);

    return l;
}

value_t
compound(state_t &s,
         binary_op_t op,
         const address_t &at,
         view_t view,
         sign_t sign,
         view_t operation,
         value_t r,
         unsigned wanted)
{
    const value_t l = fetch(s, at, view, sign, width(operation));
    const value_t v = arithmetic(s, op, l, r, operation, wanted);
    stored(s, v, at, view);
    release(s, at);

    return v;
}

value_t
fetch(state_t &s, const address_t &at, view_t view, sign_t sign, int to)
{
    const auto [mnemonic, bits] = load_of(view, sign, to);

    const value_t v = hold(s, take(s, registers_of(s, at)));
    const reg_t r = reg_of(s, v);

    const std::string from = operand(s, at, bit(r));
    insn(s, mnemonic, { reg(r, bits), from });

    return v;
}

// The add or sub of 1, at the promoted width, the store at the
// object's: G1, the machine wraps.
value_t
step(state_t &s, step_op_t op, const address_t &at, view_t view, sign_t sign)
{
    const int bits = promoted_width(view);

    const value_t l = fetch(s, at, view, sign, bits);
    const reg_t r = reg_of(s, l);
    const value_t v =
        is_postfix(op) ? hold(s, take(s, bit(r) | registers_of(s, at))) : l;

    insn(s, is_increment(op) ? "add" : "sub",
         { reg(reg_of(s, v), bits), reg(r, bits), imm(1) });

    stored(s, v, at, view);
    release(s, at);

    if (is_postfix(op))
        release(s, v);

    return l;
}

// cmp L, R, the destination's order, cmp w0, w1: operands() asks for
// r's register first, then l's.
void
compare(state_t &s, value_t l, value_t r, view_t view)
{
    const int bits = width(view);

    const auto [left, right] = operands(s, l, r);
    insn(s, "cmp", { reg(left, bits), reg(right, bits) });

    release(s, r);
}

// cmp's immediate is the class of add and sub, whichever the comparison
// (2.4), immediate_of() for ==; a negative one is written as the source
// has it, cmp w0, #-1, which the assembler encodes as cmn w0, #1.  A
// constant it does not encode is held after l, which may spill l, and
// compared as any held value.
void
compare(state_t &s, value_t l, long long constant, view_t view)
{
    const auto immediate = immediate_of(binary_op_t::eq, constant, width(view));
    if (!immediate) {
        compare(s, l, constant_value(s, constant, view), view);
        return;
    }

    insn(s, "cmp", { reg(reg_of(s, l), width(view)), *immediate });
}

value_t
cset(state_t &s, binary_op_t op, value_t v, unsigned wanted)
{
    insn(s, "cset",
         { reg(final_destination(s, v, wanted), 32), condition(op) });

    return v;
}

// The store row of 2.2, rcc's store_of(): strb w0, [x29, #16], str w0,
// [x29, #16], str x0, [x29, #16].
void
store(state_t &s, reg_t r, std::string_view to, view_t view)
{
    insn(s, store_of(view), { reg(r, width(view)), to });
}

// str takes no immediate, but the zero register is an operand like any
// other, rcc's initialize(): strb wzr, [x29, #16], str xzr, [x29, #16],
// the operand formed in a statement of its own before the str.
void
store_zero(state_t &s, const address_t &at, view_t view)
{
    const address_t held = held_index(s, at, 0);
    const std::string to = operand(s, held);
    insn(s, store_of(view), { zero(width(view)), to });
    if (at.constant)
        release(s, *held.index);
}

// The value's register first, then the constant index held outside it,
// rcc's initialize() and store().
void
store(state_t &s, value_t v, const address_t &at, view_t view)
{
    const reg_t r = reg_of(s, v, registers_of(s, at));
    const address_t held = held_index(s, at, bit(r));
    store(s, r, operand(s, held, bit(r)), view);
    if (at.constant)
        release(s, *held.index);
}

namespace {

// The register of wanted that no value holds, rcc's wanted_register()
// of no value, which a conditional hands its arms: none for an empty set
// and for a held register.
std::optional< reg_t >
free_register(const state_t &s, unsigned wanted)
{
    for (const reg_t r : expression_registers)
        if (wanted & bit(r))
            return s.registers.owner.contains(r) ? std::nullopt :
                                                   std::optional< reg_t >(r);

    return std::nullopt;
}

} // namespace

bool
widens_arms(const state_t &, const expr_t &yes, const expr_t &no, unsigned)
{
    return arm_widening(yes) == arm_widening_t::free ||
           arm_widening(no) == arm_widening_t::free;
}

// The join's mov at the conditional's width, max(32, width), rcc's, and
// the join's width recorded at the sign, rcc's hold() of it: the lesser
// of the arms', an arm a mov brought there valid as the mov writes it,
// written(), an unsigned arm moved by a movl valid to 64 bits.
value_t
conditional(state_t &s,
            const expr_t &cond,
            const expr_t &yes,
            const expr_t &no,
            view_t view,
            sign_t sign,
            unsigned wanted)
{
    const auto incoming = before_paths(s);

    const int other = new_label(s);
    const int end = new_label(s);

    branch(s, cond, other, false);

    const int bits = promoted_width(view);
    const auto named = free_register(s, wanted);
    const unsigned arm = named ? bit(*named) : 0;

    const value_t first = value(s, yes, arm);
    reg_t join = reg_of(s, first);
    int valid = held_valid(s, first, yes, sign);
    if (named && join != *named) {
        mov(s, join, *named, bits);
        join = *named;
        valid = written(bits, sign);
    }
    release(s, first);
    jump(s, end);

    place_label(s, other);
    const value_t second = value(s, no, arm);
    const reg_t from = reg_of(s, second);
    int other_valid = held_valid(s, second, no, sign);
    if (from != join) {
        mov(s, from, join, bits);
        other_valid = written(bits, sign);
    }
    release(s, second);

    place_label(s, end);

    const value_t v = hold(s, join);
    s.registers.values[v.id].joined = std::min(valid, other_valid);

    return v;
}

// switches

// A promoted value is valid to its view's width, 32 bits at least, so
// its test of zero is cbz w0 or cbz x0 and never tst.
void
equal_jump(state_t &s, value_t v, long long value, view_t view, int label)
{
    if (value == 0) {
        test_zero(s, v, view, width(view), label);
        return;
    }

    compare(s, v, value, view);
    jump_if(s, binary_op_t::eq, label);
}

value_t
subtracted(state_t &s, value_t v, long long first, view_t view)
{
    return arithmetic(s, binary_op_t::sub, v, first, view, 0);
}

// The index's register is read before x16 holds anything, so a reload
// of a spilled index stands before the adrp (chapter 4).
void
table_jump(state_t &s, value_t index, int, view_t view, int table)
{
    const std::string name = label_name(s.function, table);
    const reg_t r = reg_of(s, index);
    const std::string entry = view == view_t::i64 ?
                                  std::format("[x16, {}, lsl #3]", reg(r, 64)) :
                                  std::format("[x16, {}, uxtw #3]", reg(r, 32));

    insn(s, "adrp", { "x16", name });
    insn(s, "add", { "x16", "x16", ":lo12:" + name });
    insn(s, "ldr", { "x16", entry });
    insn(s, "br", { "x16" });

    release(s, index);

    s.terminated = true;
}

// The size's register is the lowest free, outside v's; the quotient's is
// taken outside both and held, the size's given back before the msub,
// which is the last to read it.
std::pair< value_t, value_t >
divided(state_t &s, value_t v, long long size, view_t view)
{
    const int bits = width(view);

    const value_t d = constant_value(s, size, view);
    const auto [left, right] = operands(s, v, d);

    const value_t quotient = hold(s, take(s, bit(left) | bit(right)));
    const std::string q = reg(reg_of(s, quotient), bits);
    insn(s, "udiv", { q, reg(left, bits), reg(right, bits) });

    release(s, d);
    insn(s, "msub", { reg(left, bits), q, reg(right, bits), reg(left, bits) });

    return { quotient, v };
}

// pointers

bool
in_reach(long long displacement, int size)
{
    const bool scaled = displacement >= 0 && displacement % size == 0 &&
                        displacement / size <= 4095;

    return scaled || (displacement >= -256 && displacement <= 255);
}

namespace {

// The instructions constant() writes for value at width, rcc's
// constant_length(): one where a mov encodes it, and otherwise one for
// each chunk that differs from the start, the lesser of those over zeros
// and those over ones.
int
constant_length(long long value, int width)
{
    const auto u = bits_of(value, width);

    int zeros = 0;
    int ones = 0;
    for (int i = 0; i < width / 16; ++i) {
        zeros += ((u >> 16 * i) & 0xffff) != 0;
        ones += ((u >> 16 * i) & 0xffff) != 0xffff;
    }

    if (zeros <= 1 || ones <= 1 || bitmask_immediate(value, width))
        return 1;

    return std::min(zeros, ones);
}

// Whether a slot's displacement is one 5.3 reaches, through x16 past
// the access's own reach, rcc's in_frame_reach().
bool
in_frame_reach(long long displacement)
{
    return displacement >= 0 && displacement < (1LL << 24);
}

// The n of p + n, n + p and p - n as the row takes it, rcc's right_t: an
// immediate, or a value held, read by the form's extend and shifted by
// the stride's log.
struct addend_t
{
    std::string immediate = { };
    std::optional< value_t > value = std::nullopt;
    extend_t extend = extend_t::lsl;
    int shift = 0;
};

// The log of a stride, the shift of the form that scales by it.
int
shift_of(long stride)
{
    return std::countr_zero(static_cast< unsigned long >(stride));
}

// The product c * s of a constant n and the stride, rcc's product(): the
// immediate where add and sub encode it, add x0, x0, #-4 written with
// the source's operator; otherwise held in a register by the constant
// row of 6.4, taken outside avoid.
addend_t
product(state_t &s, long long d, unsigned avoid = 0)
{
    if (arithmetic_immediate(d, 64))
        return { .immediate = imm(d) };

    const value_t v = hold(s, take(s, avoid));
    constant(s, reg_of(s, v), d, 64);

    return { .value = v };
}

// The n of p + n, n + p and p - n generated as the row takes it, rcc's
// displacement(): a constant as its product with the stride; any other
// n the index of index_of(), shifted by the add, add x0, x0, w1, sxtw #2.
addend_t
displacement(state_t &s,
             const expr_t &n,
             std::optional< long long > constant,
             long stride)
{
    if (constant)
        return product(s, scaled< binary_op_t::add >(*constant, stride));

    const index_t index = index_of(s, n);

    return { .value = index.value,
             .extend = index.extend,
             .shift = shift_of(stride) };
}

// The row of p + n and p - n, rcc's offset(), 6.3, G6: the address plus
// or minus n, modulo 2^64, on the held pointer and the generated n, the
// result in L: add L, L, #c*s for an immediate; add L, L, R, lsl #2 or
// add L, L, wR, sxtw #2 with n its last operand; n written first, n + p,
// is L, a plain register keeping the source's order, add L, L, R, and
// the shifted or extended one, which the instruction takes last only,
// add L, R, wL, sxtw #2.  The register the consumer names, wanted, is
// the destination where it may be written (6.1).
value_t
offset(state_t &s,
       binary_op_t op,
       value_t pointer,
       const addend_t &n,
       bool pointer_first,
       unsigned wanted = 0)
{
    const auto mnemonic = op == binary_op_t::add ? "add" : "sub";

    if (!n.value) {
        const auto from = reg(reg_of(s, pointer), 64);
        const auto to = reg(final_destination(s, pointer, wanted), 64);
        insn(s, mnemonic, { to, from, n.immediate });
        return pointer;
    }

    const auto [first, second] = pointer_first ?
                                     operands(s, pointer, *n.value) :
                                     operands(s, *n.value, pointer);
    const reg_t p = pointer_first ? first : second;
    const reg_t i = pointer_first ? second : first;

    const auto index = index_operand(i, n.extend, n.shift);
    const bool plain = n.extend == extend_t::lsl && n.shift == 0;

    const value_t result = pointer_first ? pointer : *n.value;
    release(s, pointer_first ? *n.value : pointer);

    const auto to = reg(final_destination(s, result, wanted), 64);

    if (pointer_first || !plain)
        insn(s, mnemonic, { to, reg(p, 64), index });
    else
        insn(s, mnemonic, { to, reg(i, 64), reg(p, 64) });

    return result;
}

// p + n on a slot's address as a base, x29 the sum's source, rcc's
// frame_sum(): add xR, x29, R in the index's register by the form's
// extend and shift, add x0, x29, w0, sxtw #2; the value is the sum.
value_t
frame_sum(state_t &s, const addend_t &n)
{
    const reg_t i = reg_of(s, *n.value);
    insn(s, "add", { reg(i, 64), "x29", index_operand(i, n.extend, n.shift) });

    return *n.value;
}

// An access through a row by a variable index, or by a constant only,
// rcc's row_access_t, read off the tree before anything is generated:
// denoted()'s index, if any, its order and stride, and its base; the
// base's address as a slot's, folded()'s, or as based()'s pointer and
// displacement, the constants with the walk's bytes, the total with
// the access's column, and whether the index is the access's register
// offset.  A row on a label, layers(), is no case of this language.
struct row_access_t
{
    expr_t pointer;
    std::optional< expr_t > index;
    bool pointer_first;
    long stride;
    std::optional< address_t > slot = std::nullopt;
    std::optional< based_t > plan = std::nullopt;
    long long carried = 0;
    long long total = 0;
    bool register_offset = false;
};

// Whether a row's index is the access's register offset, rcc's
// offset_index(), 6.3: a row with one, with no constant at all and
// nothing evaluated between the address and the access, of an index the
// access's form scales as it is, at the access's own size.
bool
offset_index(const row_access_t &row, int size, bool nothing_between)
{
    return row.index && !row.slot && row.total == 0 && nothing_between &&
           row.stride == size;
}

// The access of size bytes at column into a row by a variable index, or
// by a constant only, where it takes the row's constants or its index
// into its own form, rcc's row_access(), 6.3; none where it takes
// neither, and none on a local's address with no index, whose operand is
// folded()'s.
std::optional< row_access_t >
row_access(state_t &s,
           const expr_t &row,
           long long column,
           int size,
           bool nothing_between)
{
    const denoted_t at = denoted(row, { });

    row_access_t a { .pointer = *at.base,
                     .index = at.index,
                     .pointer_first = !at.index_first,
                     .stride = at.index_stride };

    const auto named = named_offset(a.pointer);
    if (!a.index && (at.bytes == 0 || named))
        return std::nullopt;

    a.slot = named ? folded(s, named) : std::nullopt;
    if (named && !a.slot)
        return std::nullopt;

    a.plan = named ? std::nullopt : based(a.pointer);
    a.carried = wrapped_add(at.bytes, a.slot ? a.slot->displacement :
                                      a.plan ? a.plan->displacement :
                                               0);
    a.total = wrapped_add(a.carried, column);
    if (a.carried != 0 && !in_reach(a.total, size))
        return std::nullopt;

    a.register_offset = offset_index(a, size, nothing_between);
    if (!a.slot && a.carried == 0 && !a.register_offset)
        return std::nullopt;

    return a;
}

// The base of a row's sum, generated, rcc's row_base(): nothing for a
// slot, x29 being the base, or the value of based()'s pointer or of the
// pointer itself.
std::optional< value_t >
row_base(state_t &s, const row_access_t &row)
{
    if (row.slot)
        return std::nullopt;

    return value(s, row.plan ? row.plan->pointer : row.pointer);
}

// The operand of an access through a row, rcc's row_address(), 6.3: the
// pointer's base and the index in the order written (G13), then the
// index the register offset, or its sum with the base, with x29 for a
// slot, frame_sum()'s, and the constants the access's displacement past
// the sum.  A row with no index is the base and the total.
address_t
row_address(state_t &s, const row_access_t &row)
{
    if (!row.index)
        return { .base = row_base(s, row),
                 .displacement = static_cast< int >(row.total) };

    std::optional< value_t > base;
    if (row.pointer_first)
        base = row_base(s, row);

    const addend_t index =
        displacement(s, *row.index, std::nullopt, row.stride);

    if (!row.pointer_first)
        base = row_base(s, row);

    if (row.register_offset)
        return { .base = base,
                 .index = index.value,
                 .extend = index.extend,
                 .shift = index.shift };

    if (row.slot)
        return { .base = frame_sum(s, index),
                 .displacement = static_cast< int >(row.total) };

    return { .base =
                 offset(s, binary_op_t::add, *base, index, row.pointer_first),
             .displacement = static_cast< int >(row.total) };
}

// An access's offset in bytes that no displacement holds, rcc's
// held_offset(), 6.3: held in a register by the constant row of 6.4
// after the base and read as the access's register offset, unscaled.
address_t
held_offset(state_t &s, value_t base, long long bytes)
{
    const value_t held = hold(s, take(s, held_in(s, base)));
    constant(s, reg_of(s, held), bytes, 64);

    return { .base = base, .index = held };
}

// Whether an access of size bytes holds its constants' total, rcc's
// holds_total(), 6.3: denoted()'s walk under the pointer, with no index
// and on a base that is no local's address, and the access's own d,
// whose total no displacement reaches; held once where that is fewer
// instructions than the parts take at the least.
bool
holds_total(const denoted_t &at, long long d, int size, bool whole)
{
    if (at.index || at.constants == 0 || named_offset(*at.base))
        return false;

    const long long total = wrapped_add(at.bytes, d);
    if (in_reach(total, size))
        return false;

    const int under = at.constants == 1 ? at.written : at.bytes != 0;
    const int own = in_reach(d, size) ? 0 : 1;
    const int least =
        whole ? std::min(under + own, product_length(total)) : under + own;

    return constant_length(total, 64) < least;
}

// The address of a base and an index read before a store's right
// operand, rcc's index_across(), 6.3: the two registers the form reads,
// but where that operand holds a call, between, and the convention
// would spill both across it, their sum, one register held.
address_t
index_across(state_t &s,
             value_t base,
             index_t index,
             int shift,
             bool pointer_first,
             between_t between)
{
    if (between != between_t::call || !held_in(s, base) ||
        !held_in(s, index.value))
        return { .base = base,
                 .index = index.value,
                 .extend = index.extend,
                 .shift = shift };

    const addend_t n { .value = index.value,
                       .extend = index.extend,
                       .shift = shift };

    return { .base = offset(s, binary_op_t::add, base, n, pointer_first) };
}

// The index form of an access through a sum, rcc's indexed_address():
// the base and the index generated in the order written (G13), then
// held across between, index_across()'s.
address_t
indexed_address(state_t &s, const sum_t &sum, between_t between)
{
    const int shift = shift_of(sum.stride);

    if (!sum.index_first) {
        const value_t base = value(s, sum.pointer);
        const index_t index = index_of(s, sum.index);
        return index_across(s, base, index, shift, true, between);
    }

    const index_t index = index_of(s, sum.index);
    const value_t base = value(s, sum.pointer);

    return index_across(s, base, index, shift, false, between);
}

// The value of a row on a local by a variable index, rcc's named_row():
// denoted()'s base a local's address, the constants the slot's own add,
// and then the index added in the order written (G13).
std::optional< value_t >
named_row(state_t &s, const expr_t &e, unsigned wanted)
{
    const denoted_t at = denoted(e, { });
    if (!at.index || at.constants == 0)
        return std::nullopt;

    auto named = named_offset(*at.base);
    if (!named)
        return std::nullopt;

    named->offset = wrapped_add(named->offset, at.bytes);
    const auto a = folded(s, named);
    if (!a)
        return std::nullopt;

    std::optional< value_t > base;
    if (!at.index_first)
        base = address_of(s, *a);

    const addend_t n =
        displacement(s, *at.index, std::nullopt, at.index_stride);

    if (at.index_first)
        base = address_of(s, *a);

    return offset(s, binary_op_t::add, *base, n, !at.index_first, wanted);
}

// The value of a pointer whose sums by a constant are more than one,
// rcc's joined(): denoted()'s base, a variable index on the way added to
// it in the order written, and the constants as one add of their sum,
// add x0, x0, #12 for p + 1 + 2, where that is fewer instructions than
// the rows write for the parts; none for one constant, whose add is the
// rows' own, and for a local's address.
std::optional< value_t >
joined(state_t &s, const expr_t &e, unsigned wanted)
{
    const denoted_t at = denoted(e, { });
    if (at.constants < 2 || named_offset(*at.base))
        return std::nullopt;

    if ((at.bytes == 0 ? 0 : product_length(at.bytes)) >= at.written)
        return std::nullopt;

    if (!at.index) {
        if (at.bytes == 0)
            return value(s, *at.base, wanted);

        const value_t base = value(s, *at.base);
        return offset(s, binary_op_t::add, base, product(s, at.bytes), true,
                      wanted);
    }

    std::optional< value_t > base;
    if (!at.index_first)
        base = value(s, *at.base);

    const addend_t n =
        displacement(s, *at.index, std::nullopt, at.index_stride);

    if (at.index_first)
        base = value(s, *at.base);

    const bool last = at.bytes == 0;
    const value_t sum = offset(s, binary_op_t::add, *base, n, !at.index_first,
                               last ? wanted : 0);
    if (last)
        return sum;

    return offset(s, binary_op_t::add, sum, product(s, at.bytes), true, wanted);
}

} // namespace

int
product_length(long long d)
{
    if (arithmetic_immediate(d, 64))
        return 1;

    return constant_length(d, 64) + 1;
}

std::optional< based_t >
based(const expr_t &pointer)
{
    const auto at = lvalue_offset(pointer);
    if (!at || named_offset(pointer))
        return std::nullopt;

    return based(at->address, at->pointer, at->offset);
}

std::optional< based_t >
based(const expr_t &address, const expr_t &q, long long offset)
{
    if (const auto inner = based(q))
        return based_t { inner->pointer,
                         wrapped_add(inner->displacement, offset) };

    if (const auto sum = constant_sum(q))
        return based_t { sum->pointer, wrapped_add(sum->displacement, offset) };

    return based_t { address, offset };
}

std::optional< address_t >
folded(state_t &s, const std::optional< named_offset_t > &named)
{
    if (!named)
        return std::nullopt;

    const long long displacement =
        wrapped_add(slot_of(s, named->slot), named->offset);
    if (!in_frame_reach(displacement))
        return std::nullopt;

    return address_t { .displacement = static_cast< int >(displacement) };
}

// named_access(): the displacement or its low 12 bits in reach.
std::optional< address_t >
folded_access(state_t &s,
              const std::optional< named_offset_t > &named,
              int size)
{
    const auto a = folded(s, named);
    if (!a || (!in_reach(a->displacement, size) &&
               !in_reach(a->displacement & 0xfff, size)))
        return std::nullopt;

    if (!in_reach(a->displacement, size))
        throw std::logic_error(
            "v2: not yet: a slot beyond the reach of [x29, #d]");

    return a;
}

std::optional< address_t >
summed(state_t &s,
       const std::optional< based_t > &plan,
       int size,
       bool nothing_between)
{
    if (!plan)
        return std::nullopt;

    if (const auto row = row_access(s, plan->pointer, plan->displacement, size,
                                    nothing_between))
        return row_address(s, *row);

    const bool reached = in_reach(plan->displacement, size);
    if (!reached && nothing_between &&
        !arithmetic_immediate(plan->displacement, 64))
        return held_offset(s, value(s, plan->pointer), plan->displacement);

    if (!reached)
        return std::nullopt;

    return address_t { .base = value(s, plan->pointer),
                       .displacement = static_cast< int >(plan->displacement) };
}

address_t
erased_pointed(state_t &s, const expr_t &self, between_t between, int size)
{
    if (const auto a = folded_access(s, named_offset(self), size))
        return *a;

    if (const auto a =
            summed(s, based(self), size, between == between_t::nothing))
        return *a;

    return { .base = value(s, self) };
}

address_t
sum_address(state_t &s,
            const sum_t &sum,
            bool under,
            const expr_t &pointer,
            between_t between,
            int size)
{
    const bool nothing_between = between == between_t::nothing;

    std::optional< long long > d;
    if (sum.constant)
        d = scaled< binary_op_t::add >(*sum.constant, sum.stride);

    // p[3], (p + 1)[2]: the constants one displacement, [x0, #12]
    if (d)
        if (const auto row =
                row_access(s, sum.pointer, *d, size, nothing_between))
            return row_address(s, *row);

    if (d && nothing_between)
        if (const denoted_t at = denoted(sum.pointer, { });
            holds_total(at, *d, size, under))
            return held_offset(s, value(s, *at.base),
                               wrapped_add(at.bytes, *d));

    if (d && in_reach(*d, size))
        return { .base = value(s, sum.pointer),
                 .displacement = static_cast< int >(*d) };

    // p[i] = g(): the row of a place held across a call, its base and its
    // index summed, the constants under the pointer the access's
    // displacement
    if (!d && between == between_t::call)
        if (const auto row = row_access(s, sum.self, 0, size, false))
            return row_address(s, *row);

    // under a cast, the index form only where it saves the add, and only
    // at a stride of the access's size
    if (under && (!nothing_between || (d && arithmetic_immediate(*d, 64)) ||
                  sum.stride != size))
        return { .base = value(s, pointer) };

    // p[5000] = g(): the constant index held back past the right operand
    if (d && !nothing_between)
        return { .base = value(s, sum.pointer),
                 .shift = shift_of(sum.stride),
                 .constant = *sum.constant };

    return indexed_address(s, sum, between);
}

index_t
widened_index(state_t &s, value_t v, int valid, view_t from, sign_t sign)
{
    if (valid < 32) {
        v = extended(s, v, from, sign, view_t::i32, 0);
        valid = written(32, sign);
    }

    if (valid >= 64)
        return { v };

    return { v, sign == sign_t::signed_ ? extend_t::sxtw : extend_t::uxtw };
}

value_t
pointer_arithmetic(state_t &s,
                   binary_op_t op,
                   const sum_t &sum,
                   const std::optional< named_offset_t > &named,
                   unsigned wanted)
{
    if (const auto a = folded(s, named))
        return address_of(s, *a);

    if (const auto v = named_row(s, sum.self, wanted))
        return *v;

    if (const auto v = joined(s, sum.self, wanted))
        return *v;

    if (!sum.index_first) {
        const value_t l = value(s, sum.pointer);
        return offset(s, op, l,
                      displacement(s, sum.index, sum.constant, sum.stride),
                      true, wanted);
    }

    const addend_t n = displacement(s, sum.index, sum.constant, sum.stride);
    const value_t p = value(s, sum.pointer);

    return offset(s, op, p, n, false, wanted);
}

// The sub into the left value's register, then the stride held in a
// register taken after it, and the division.
value_t
difference(state_t &s,
           const expr_t &lhs,
           const expr_t &rhs,
           long stride,
           unsigned wanted)
{
    const value_t l = value(s, lhs);
    const value_t r = value(s, rhs);
    const value_t d = arithmetic(s, binary_op_t::sub, l, r, view_t::i64, 0);

    const value_t size = constant_value(s, stride, view_t::i64);

    return arithmetic(s, binary_op_t::sdiv, d, size, view_t::i64, wanted);
}

// The value goes to the convention's result register, 7.2, rcc's
// return_stmt() and rvalue_in(): the value's row is handed the result
// register as the one its consumer names, and the value is moved where
// its row left it elsewhere.  A statement starts with no value held, so
// the first register a row takes, the lowest, is the result register.
// Every return jumps to the one epilogue, the last statement of the
// body included: the jump to the next instruction is what the statement
// says, and removing it was the optional peephole pass's work in the
// PDP-11 compiler, which rcc has no counterpart of.  The move is at the
// result's width, mov w0, w1 for an int and mov x0, x1 for a long,
// rcc's return_stmt() at max(32, width).
void
return_stmt(state_t &s, const expr_t &e, view_t view)
{
    const value_t v = value_in(s, e, bit(result_register));
    const reg_t from = reg_of(s, v);
    if (from != result_register)
        mov(s, from, result_register, promoted_width(view));

    release(s, v);

    jump(s, s.ret);
}

namespace {

// A size by which sp moves, the frame's or a call's outgoing area's, as
// the immediates of sub and add, 5.2: 12 bits, or 12 bits shifted by
// 12.  The upper part first; a part that is zero is not written, so a
// size under 4096 bytes or a multiple of 4096 is one instruction.
std::vector< long >
frame_parts(long size)
{
    std::vector< long > parts;
    for (const long part : { size & ~0xfffL, size & 0xfffL })
        if (part != 0)
            parts.push_back(part);

    return parts;
}

// the call

// The outgoing area of a call with arguments past the eighth, rcc's
// outgoing_size(), chapter 8: 8 bytes for each, rounded up to 16, since
// sp is a multiple of 16 at the call (chapter 3).  It is opened and
// closed in the parts of frame_parts(), as the frame is, which reach 16
// MiB less 16; an area of 16 MiB or more is rcc's not yet.
long
outgoing_size(std::size_t stacked)
{
    const long size = (8 * static_cast< long >(stacked) + 15) / 16 * 16;
    if (size >= 1L << 24)
        throw std::logic_error(
            "v2: not yet: an outgoing area of 16 MiB or more");

    return size;
}

// The address of the outgoing area's k-th 8 bytes for a store of the
// view, rcc's outgoing_operand(), [sp, #8k]: the ninth argument at
// [sp], as clang -S writes it, the tenth at [sp, #8].  Past the store's
// reach, 2.3, rcc forms the address in x16, which rcc-v2 does not yet.
std::string
outgoing_operand(std::size_t k, view_t view)
{
    const long displacement = 8 * static_cast< long >(k);
    if (displacement > reach(width(view) / 8))
        throw std::logic_error(
            "v2: not yet: an argument beyond the reach of [sp, #d]");

    return displacement == 0 ? "[sp]" : std::format("[sp, #{}]", displacement);
}

// The names the assembler reads as a register's where a branch's
// operand may be a symbol's, chapter 8, in lower case: x0 to x31 and w0
// to w31, the b, h, s, d, q and v names of the vector registers 0 to
// 31, sp, wsp, xzr, wzr, lr and fp, and the special registers it names
// beside them.  It reads them so in any case.
const std::unordered_set< std::string > &
register_names()
{
    static const auto names = [] {
        std::unordered_set< std::string > set {
            "sp",   "wsp",  "xzr", "wzr", "lr",   "fp",   "nzcv", "fpcr",
            "fpsr", "fpmr", "ffr", "vg",  "vgx2", "vgx4", "zt0",
        };
        for (const char letter : std::string_view("xwbhsdqv"))
            for (int n = 0; n < 32; ++n)
                set.insert(std::format("{}{}", letter, n));

        return set;
    }();

    return names;
}

// A callee's name as the operand of bl, rcc's branch_target(), chapter
// 8: in quotes where the assembler would read it as a register's, bl
// "x1", which it takes for the symbol, and as it is elsewhere.
std::string
branch_target(std::string_view name)
{
    std::string lower { name };
    for (char &c : lower)
        if (c >= 'A' && c <= 'Z')
            c = static_cast< char >(c - 'A' + 'a');

    if (register_names().contains(lower))
        return std::format("\"{}\"", name);

    return std::string { name };
}

} // namespace

// rcc's call() after the arguments' stores, chapter 8: the arguments
// past the eighth are copied from their slots to their own 8 bytes of
// the outgoing area, opened below the frame for this call alone, at
// their views by the plain load, ldrh w17, [x29, #60] and strh w17,
// [sp], in order, so a signed one is not extended there: the rest of
// the 8 bytes are padding to the convention; the first eight are loaded
// into w0 to w7 by the load of their parameter's type, a narrow one
// extended to 32 bits by its sign, ldrsb w0, [x29, #17], ldr x1, [x29,
// #24].  G16: bl f, the name alone, since the linker
// makes a PLT entry where one is needed, quoted where it is a
// register's, branch_target().  No unwind directive is written around
// the area, since the canonical frame address is computed from x29.
void
transfer(state_t &s,
         std::string_view name,
         const std::vector< argument_t > &args)
{
    const std::size_t registers = std::size(argument_registers);
    const std::size_t stacked =
        args.size() > registers ? args.size() - registers : 0;
    const long area = stacked > 0 ? outgoing_size(stacked) : 0;

    for (const long part : frame_parts(area))
        insn(s, "sub", { "sp", "sp", imm(part) });

    for (std::size_t i = registers; i < args.size(); ++i) {
        const view_t view = view_of(args[i].value);

        copy_in(s, slot_operand(slot_of(s, args[i].where)), view);
        copy_out(s, outgoing_operand(i - registers, view), view);
    }

    for (std::size_t i = 0; i < std::min(registers, args.size()); ++i) {
        const view_t view = view_of(args[i].value);
        const auto [mnemonic, to] =
            load_of(view, args[i].sign, promoted_width(view));

        insn(s, mnemonic,
             { reg(argument_registers[i], to),
               slot_operand(slot_of(s, args[i].where)) });
    }

    insn(s, "bl", { branch_target(name) });

    for (const long part : frame_parts(area))
        insn(s, "add", { "sp", "sp", imm(part) });
}

namespace {

// the frame

// The parameters' half of the prologue, rcc's parameters(), 5.2: each
// takes the next slot, in parameter order, of its view's bytes at as
// many bytes' alignment, as a local does, its displacement recorded
// under the parameter's number, the first locals' (docs/typed-tree.md
// 4.5); each of the first eight is stored there from its argument
// register by the store of its view, strb w0, [x29, #16], str x1, [x29,
// #24], and one past the eighth is copied there by stack_parameters(),
// once the frame's size says where the caller left it.  The convention
// leaves the bits of an argument register above a char or a short
// unspecified, so the store writes the low bytes alone, and each read is
// the extending load of its place.
void
parameters(state_t &s, const std::vector< view_t > &views)
{
    for (std::size_t i = 0; i < views.size(); ++i) {
        const int id = static_cast< int >(i);
        const int bytes = width(views[i]) / 8;
        s.slots.emplace(id, allocate(s, bytes, bytes));

        if (i < std::size(argument_registers))
            store(s, argument_registers[i],
                  slot_operand(slot_of(s, slot_t { id })), views[i]);
    }
}

// The displacement off x29 of the parameter of index i past the eighth,
// 5.2: the caller left it above the frame, at [x29, #T + 8(i - 8)], 8
// bytes apart whatever the views, and T, the frame's size, is known
// only once the body is done.  Past the reach of the load of the view,
// rcc holds the upper part in x16, 5.3, which rcc-v2 does not yet.
int
above_frame(std::size_t i, long size, view_t view)
{
    const long past = static_cast< long >(i - std::size(argument_registers));
    const long displacement = size + 8 * past;
    if (displacement > reach(width(view) / 8))
        throw std::logic_error(
            "v2: not yet: a parameter beyond the reach of [x29, #d]");

    return static_cast< int >(displacement);
}

// The parameters past the eighth, each copied to its slot at its view,
// rcc's stack_parameters() and stack_copy(), 5.2, in parameter order
// after the stores of the first eight: ldrb w17, [x29, #64] and strb
// w17, [x29, #48].  The copy is the plain load, which reads a narrow
// one at its width, since the convention leaves the bits above it
// unspecified, and the store of its view.
void
stack_parameters(state_t &s, const std::vector< view_t > &views, long size)
{
    for (std::size_t i = std::size(argument_registers); i < views.size(); ++i) {
        const int id = static_cast< int >(i);

        copy_in(s, slot_operand(above_frame(i, size, views[i])), views[i]);
        copy_out(s, slot_operand(slot_of(s, slot_t { id })), views[i]);
    }
}

// Chapter 5.2: the frame opened by sub, the frame record stored at its
// bottom, x29 pointed at it.  Each sub is followed by the canonical frame
// address's offset from sp so far, and the record by the places of x29
// and x30 off the canonical frame address, which is sp before the sub.
void
prologue(std::ostream &out, long size)
{
    long opened = 0;
    for (const long part : frame_parts(size)) {
        opened += part;
        out << std::format("\tsub\tsp, sp, #{}\n"
                           "\t.cfi_def_cfa_offset {}\n",
                           part, opened);
    }

    out << std::format("\tstp\tx29, x30, [sp]\n"
                       "\t.cfi_offset w29, {}\n"
                       "\t.cfi_offset w30, {}\n"
                       "\tmov\tx29, sp\n"
                       "\t.cfi_def_cfa_register w29\n",
                       -size, 8 - size);
}

// The one epilogue: the frame record loaded, the frame closed by add.
// The label is reached with sp equal to x29, so the canonical frame
// address moves to sp + size there, before the load gives x29 back to
// the caller; after the load x29 and x30 are in their registers again,
// and each add leaves the address at what is left of the frame above
// sp.
void
epilogue(std::ostream &out, const state_t &s, long size)
{
    out << label_name(s.function, s.ret) << ":\n";

    out << std::format("\t.cfi_def_cfa wsp, {}\n", size);

    out << "\tldp\tx29, x30, [sp]\n"
           "\t.cfi_restore w29\n"
           "\t.cfi_restore w30\n";

    long left = size;
    for (const long part : frame_parts(size)) {
        left -= part;
        out << std::format("\tadd\tsp, sp, #{}\n"
                           "\t.cfi_def_cfa_offset {}\n",
                           part, left);
    }

    out << "\tret\n";
}

// A definition: the parameters' stores and the body generated into the
// state's buffer, each taken out of it in turn, then written inside the
// frame, the frame record and the slots, whose size is known after the
// body, rounded up to 16, with the copies of the parameters past the
// eighth, which read the size, between the two.  The one epilogue is
// reached with x0 as it is, G5's some value, but for main, which
// returns 0 when it falls off its end, 5.1.2.3.4, by its name, whatever
// its result type.
void
function(std::ostream &out,
         const function_decl_t &fd,
         const std::unordered_set< std::string > &defined)
{
    state_t s { .defined = defined,
                .function = fd.name,
                .frame = frame_record };
    s.ret = new_label(s);

    parameters(s, fd.parameters);
    const std::string stores = s.body.str();
    s.body.str({ });

    generate(s, *fd.body);

    if (!s.terminated && s.function == "main")
        constant(s, result_register, 0, 32);

    const long size = (s.frame + 15) / 16 * 16;

    const std::string body = s.body.str();
    s.body.str({ });
    stack_parameters(s, fd.parameters, size);

    binding(out, fd.name, fd.external);
    out << std::format("\t.type\t{}, @function\n"
                       "{}:\n"
                       "\t.cfi_startproc\n",
                       fd.name, fd.name);

    prologue(out, size);

    out << stores << s.body.str() << body;

    epilogue(out, s, size);
    out << std::format("\t.cfi_endproc\n"
                       "\t.size\t{}, .-{}\n",
                       fd.name, fd.name);
}

} // namespace

// The functions the unit defines, in order, chapter 10; one it only
// declares writes nothing, since the assembler leaves its symbol to the
// link.  The names of the objects the unit defines are taken before the
// first function, the unit being parsed whole, so an object defined
// after its use is reached by its label.  The data of the objects it
// defines follows the last function, with nothing between.  The note
// section marks the stack not executable.
void
generate(const translation_unit_t &unit,
         std::string_view file,
         std::ostream &assembly)
{
    assembly << std::format("\t.file\t{}\n\t.text\n", quoted_file(file));

    const auto defined = defined_symbols(unit);
    for (const function_decl_t &fd : unit.functions)
        if (fd.body)
            function(assembly, fd, defined);

    data(assembly, unit.objects);

    assembly << "\t.section\t.note.GNU-stack,\"\",@progbits\n";
}

} // namespace rcc::arm64
