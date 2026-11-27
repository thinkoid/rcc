// -*- mode: c++; -*-

// x86-64's text, declared in amd64.hh: AT&T syntax for the system
// assembler, as rcc writes it.  docs/backend.md is the design, and each
// function names the row or the part of the frame it writes.  The
// frame is rbp's and the locals' slots below it, chapter 5.

#include "amd64.hh"

#include "assembly.hh"
#include "target.hh"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iterator>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace rcc::amd64 {

namespace {

// The nine expression registers, chapter 4, in the order a new value
// takes them.
constexpr reg_t expression_registers[] = { rax, rcx, rdx, rsi, rdi,
                                           r8,  r9,  r10, r11 };

// The convention's argument registers, in order, 3.2.
constexpr reg_t argument_registers[] = { rdi, rsi, rdx, rcx, r8, r9 };

// The convention's result register, 3.2.
constexpr reg_t result_register = rax;

// The red zone, 3.2: the 128 bytes below rsp, which a function that
// calls nothing may use without moving rsp.
constexpr long red_zone = 128;

// operands

// A register by its name at a width in bits.
std::string
reg(reg_t r, int width)
{
    static constexpr std::string_view names[][4] = {
        { "al", "ax", "eax", "rax" },      { "cl", "cx", "ecx", "rcx" },
        { "dl", "dx", "edx", "rdx" },      { "sil", "si", "esi", "rsi" },
        { "dil", "di", "edi", "rdi" },     { "r8b", "r8w", "r8d", "r8" },
        { "r9b", "r9w", "r9d", "r9" },     { "r10b", "r10w", "r10d", "r10" },
        { "r11b", "r11w", "r11d", "r11" },
    };

    const int column = width == 8 ? 0 : width == 16 ? 1 : width == 32 ? 2 : 3;

    return std::format("%{}", names[r][column]);
}

// The suffix of a mnemonic for a width in bits.
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

// A mnemonic with the suffix of its width, rcc's sized(): addl, addq.
std::string
sized(std::string_view stem, int width)
{
    return std::format("{}{}", stem, suffix(width));
}

// A slot: a displacement off the frame pointer.
std::string
at(int displacement)
{
    return std::format("{}(%rbp)", displacement);
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

// One instruction, assembly.hh's: it is the last one now, so what the
// one before it left in ZF no longer stands, and the record is
// withdrawn.
void
insn(state_t &s,
     std::string_view mnemonic,
     std::initializer_list< std::string_view > operands)
{
    s.zero_flag = 0;
    rcc::insn(s, mnemonic, operands);
}

// The instruction just written set ZF from the result it computed at
// width, as Intel's reference has add, sub, and, or, xor, neg and test
// do (backend.md 7.1); it stands until the next instruction or label.
void
flagged(state_t &s, int width)
{
    s.zero_flag = width;
}

// The copy of a register into another at a width, 32 or 64 bits, the
// source first in AT&T order, rcc's mov(): movl %edx, %eax, movq %rax,
// %rsi.
void
mov(state_t &s, reg_t from, reg_t to, int width)
{
    insn(s, sized("mov", width), { reg(from, width), reg(to, width) });
}

// The extending mov that brings a value of the view from to a width of
// 32 or 64 bits, and the width of its destination register, rcc's
// extension(): a long's movq, an int's movl, and to 64 bits its movslq.
std::pair< std::string, int >
extension(view_t from, int to)
{
    if (from == view_t::i64)
        return { "movq", 64 };

    if (to == 32)
        return { "movl", 32 };

    return { "movslq", 64 };
}

// constants

// Whether a value is a 32-bit immediate, which a 64-bit instruction
// sign-extends.
bool
fits_imm32(long long value)
{
    return value >= INT32_MIN && value <= INT32_MAX;
}

// The constant row, 6.3: value into r at width, movl $c, L at 32 bits
// and movq $c, L at 64.  movq's immediate is 32 bits, sign-extended, so
// a 64-bit value outside int's range is movabsq, the one mov that
// carries 64 bits; 4294967295 as a long is one.
void
constant(state_t &s, reg_t r, long long value, int width)
{
    if (width == 32)
        insn(s, "movl", { imm(value), reg(r, 32) });
    else if (fits_imm32(value))
        insn(s, "movq", { imm(value), reg(r, 64) });
    else
        insn(s, "movabsq", { imm(value), reg(r, 64) });
}

// operators

// The instruction of a binary operator, rcc's arithmetic_mnemonic() and
// shift_mnemonic() before sized(): sar for >>, signed, and idiv for /
// and % alike, which idiv() writes.
std::string_view
mnemonic(binary_op_t op)
{
    switch (op) {
    case binary_op_t::add: return "add";
    case binary_op_t::sub: return "sub";
    case binary_op_t::mul: return "imul";
    case binary_op_t::sdiv:
    case binary_op_t::srem: return "idiv";
    case binary_op_t::shl: return "shl";
    case binary_op_t::ashr: return "sar";
    case binary_op_t::slt:
    case binary_op_t::sle:
    case binary_op_t::sgt:
    case binary_op_t::sge:
    case binary_op_t::eq:
    case binary_op_t::ne:
    case binary_op_t::log_and:
    case binary_op_t::log_or:
        throw std::logic_error("v2: an operator of no arithmetic instruction");
    case binary_op_t::bit_and: return "and";
    case binary_op_t::bit_or: return "or";
    case binary_op_t::bit_xor: return "xor";
    }
}

// The condition code of a comparison of op, the suffix of setcc and
// jcc, rcc's condition(): l le g ge for the signed ones.
std::string_view
condition(binary_op_t op)
{
    switch (op) {
    case binary_op_t::slt: return "l";
    case binary_op_t::sle: return "le";
    case binary_op_t::sgt: return "g";
    case binary_op_t::sge: return "ge";
    case binary_op_t::eq: return "e";
    default: return "ne";
    }
}

} // namespace

std::string
imm(long long value)
{
    return std::format("${}", value);
}

// The next slot below the last, of size bytes at align, a local's or a
// spill's; its displacement off rbp.  G12: a slot is never shared and
// never reused inside the activation.  A frame past the target's limit
// is rcc's error, which no program rcc-v2 compiles comes near: it is
// refused.
int
allocate(state_t &s, long size, int align)
{
    s.frame = (s.frame + size + align - 1) / align * align;
    if (s.frame > x86_64.frame_limit)
        throw std::logic_error("v2: not yet: a frame past the target's limit");

    return static_cast< int >(-s.frame);
}

// The two instructions the register stack writes, called by name from
// register-stack.hh.  The store of a register to a spill slot, all 8
// bytes, backend.md 6.2: movq %rax, -16(%rbp).
void
store_spill(state_t &s, reg_t r, int displacement)
{
    insn(s, "movq", { reg(r, 64), at(displacement) });
}

// The reload of a register from a spill slot, all 8 bytes, 6.2.
void
reload_spill(state_t &s, reg_t r, int displacement)
{
    insn(s, "movq", { at(displacement), reg(r, 64) });
}

// A register for a value: the lowest free one outside avoid and
// s.reserved, else the lowest free one outside avoid, or, when every
// one is held, the register of the oldest held value outside avoid,
// the lowest number, which is spilled first.  The reservation is an
// avoidance, not a pin.
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

// A label: a jump may reach it from elsewhere, with the flags its own
// path left.
void
place_label(state_t &s, int n)
{
    s.body << label_name(s.function, n) << ":\n";
    s.terminated = false;
    s.zero_flag = 0;
}

void
jump(state_t &s, int n)
{
    insn(s, "jmp", { label_name(s.function, n) });
    s.terminated = true;
}

void
align_loop_head(state_t &s)
{
    s.body << "\t.p2align 5\n";
}

// jcc of the condition, chapter 7.1.
void
jump_if(state_t &s, binary_op_t op, int label)
{
    insn(s, std::format("j{}", condition(op)),
         { label_name(s.function, label) });
}

namespace {

// Moves the value in from to the free register to, at a width: a real
// mov, and the record follows it (chapter 6.5), so the value's handle
// names to from here on.  A value held across a branch is not moved,
// check_movable().
void
move(state_t &s, reg_t from, reg_t to, int width)
{
    const int id = s.registers.owner.at(from);
    check_movable(s, value_t { id });

    mov(s, from, to, width);

    s.registers.values[id].reg = to;
    s.registers.owner.erase(from);
    s.registers.owner.emplace(to, id);
}

// Frees r for a pinned instruction: a value held there moves, all 64
// bits, to a free register outside avoid.
void
vacate(state_t &s, reg_t r, unsigned avoid)
{
    if (s.registers.owner.contains(r))
        move(s, r, take(s, avoid | bit(r)), 64);
}

// Puts a shift's count, held in right, into cl, rcc's count_into_cl():
// a value held in rcx moves out first, to a register outside avoid,
// and the count is moved there at its own view, movl %edx, %ecx for an
// int, movq %rdx, %rcx for a long.
void
count_into_cl(state_t &s, reg_t right, unsigned avoid, view_t count)
{
    if (right == rcx)
        return;

    vacate(s, rcx, avoid | bit(right));
    move(s, right, rcx, width(count));
}

// The register of wanted, the set of at most one expression register
// that the consumer of a value names, rcc's wanted_register(): where a
// row may write it now, held by no value and left for last by no
// reservation.  None for an empty set, and for a register that is held
// or reserved.
std::optional< reg_t >
wanted_register(const state_t &s, unsigned wanted)
{
    for (const reg_t r : expression_registers)
        if (wanted & bit(r))
            return !s.registers.owner.contains(r) && !(s.reserved & bit(r)) ?
                       std::optional< reg_t >(r) :
                       std::nullopt;

    return std::nullopt;
}

// The register a held value is in, as a set for take() to avoid; none
// for a value spilled.
unsigned
held_in(const state_t &s, value_t v)
{
    const auto r = s.registers.values[v.id].reg;

    return r ? bit(*r) : 0;
}

// The operand by which an instruction reads the held value v at the
// view, rcc's source_of(): its register, or for a spilled value its
// slot, read in place at the instruction's width, -16(%rbp), the bytes
// at the slot's low end, and not reloaded (6.2).
std::string
source_of(state_t &s, value_t v, view_t view)
{
    const auto &h = s.registers.values[v.id];
    if (!h.reg)
        return at(h.spill);

    return reg(reg_of(s, v), width(view));
}

// Whether the row of op on the held values l and r writes r's register,
// rcc's right_written(): an operator that commutes, +, *, &, | or ^,
// whose right operand is in a register when the instruction is written,
// so that the instruction names that register its destination and L
// its source.  The register is the one the consumer names, wanted,
// where L's register would be written and moved; or L is a spilled
// value, which the row would reload, copied, where no free register is
// named for the reload and R's register is left for last by no
// reservation and by no pinned instruction ahead.  A spilled L is then
// read in its slot, the instruction's source, addl -16(%rbp), %ecx
// (6.2).
bool
right_written(
    const state_t &s, binary_op_t op, value_t r, unsigned wanted, bool copied)
{
    if (!commutes(op))
        return false;

    const auto reg = s.registers.values[r.id].reg;
    if (!reg)
        return false;

    const unsigned own = bit(*reg);
    if (wanted & own)
        return true;

    return copied && !wanted_register(s, wanted) &&
           !((s.reserved | s.ahead) & own);
}

// right_written() of the held values l and r, l copied where it is
// spilled, rcc's written_in_right().
bool
written_in_right(
    const state_t &s, binary_op_t op, value_t l, value_t r, unsigned wanted)
{
    const bool copied = !s.registers.values[l.id].reg;

    return right_written(s, op, r, wanted, copied);
}

// G4: the instruction the machine has, idiv, rcc's idiv(), a fault on
// x86 for a zero divisor or the most negative value divided by -1.  rax
// and rdx hold nothing but, it may be, the dividend in left: it goes to
// rax, rdx is sign-filled, cltd at 32 bits and cqto at 64, and the
// division of the divisor's operand leaves the quotient in rax and the
// remainder in rdx.
void
idiv(
    state_t &s, binary_op_t op, reg_t left, std::string_view divisor, int width)
{
    if (left != rax)
        mov(s, left, rax, width);

    insn(s, width == 32 ? "cltd" : "cqto", { });
    insn(s, sized(mnemonic(op), width), { divisor });
}

// The register a division's dividend is in once rax and rdx are
// vacated, rcc's dividend(): the one it is held in, or rax for a
// dividend spilled, which is reloaded there and not into another
// register to be moved again.  A dividend held in rdx, which the
// division overwrites, moves to rax for a quotient, by the mov idiv()
// writes in any case, so that it is in rax and the quotient stays where
// idivl leaves it.  A remainder's value is rdx whatever the dividend's
// register, so its dividend stays.
reg_t
dividend(state_t &s, binary_op_t op, value_t l, int width)
{
    const reg_t r = reg_of(s, l, ~bit(rax));
    if (r != rdx || op != binary_op_t::sdiv)
        return r;

    move(s, rdx, rax, width);

    return rax;
}

// The pinned pair of a division by a held divisor, rcc's
// pin_division(), chapter 6.5: nothing looks at the divisor, which is
// in a register even when it is a constant.  The divisor moves out of
// rax or rdx first, all 64 bits, to a register outside both and the
// dividend's, movq %rax, %rsi; then the values held there move out,
// vacate(), and a dividend spilled is reloaded into rax.  Both values
// are still held after idiv(), the dividend in the register returned.
reg_t
pin_division(state_t &s, binary_op_t op, value_t l, value_t r, int width)
{
    const unsigned pinned = bit(rax) | bit(rdx);
    const unsigned in = held_in(s, l);

    reg_t right = reg_of(s, r, in);
    if (right == rax || right == rdx) {
        const reg_t to = take(s, pinned | in);
        move(s, right, to, 64);
        right = to;
    }

    for (const reg_t p : { rax, rdx })
        if (!(in & bit(p)))
            vacate(s, p, pinned | in | bit(right));

    const reg_t left = dividend(s, op, l, width);
    idiv(s, op, left, reg(right, width), width);

    return left;
}

// The pinned pair of a division by the object in a slot, chapter 6.5:
// the slot is idiv's operand, idivl -8(%rbp), and the values held in
// rax and rdx move out, vacate(); a dividend spilled is reloaded into
// rax after them.  The dividend is still held, in the register
// returned.
reg_t
pin_division(state_t &s, binary_op_t op, value_t l, slot_t divisor, int width)
{
    const unsigned pinned = bit(rax) | bit(rdx);
    const unsigned in = held_in(s, l);

    for (const reg_t p : { rax, rdx })
        if (!(in & bit(p)))
            vacate(s, p, pinned | in);

    const reg_t left = dividend(s, op, l, width);
    idiv(s, op, left, at(slot_of(s, divisor)), width);

    return left;
}

// The value of the division whose dividend l idivl took from left, the
// end of rcc's divide(): a remainder is a new value in rdx and l is
// freed (chapter 6.5), as the Seventh Edition compiler's row of a
// remainder returns the odd register of its pair, where div leaves it;
// so is a quotient whose consumer names rax, wanted, a new value in
// rax, where left is another register (6.1); any other quotient goes
// back into left, movl %eax, %ecx.
value_t
divided(state_t &s,
        binary_op_t op,
        value_t l,
        reg_t left,
        int width,
        unsigned wanted)
{
    const bool stays = left != rax && wanted_register(s, wanted) == rax;
    if (op == binary_op_t::srem || stays) {
        release(s, l);
        return hold(s, op == binary_op_t::srem ? rdx : rax);
    }

    if (left != rax)
        mov(s, rax, left, width);

    return l;
}

} // namespace

unsigned
held_out(const state_t &s, value_t l, unsigned named)
{
    return held_in(s, l) & named ? named : 0;
}

// The constant row, 6.3: at 32 bits, or 64 for a long.  G2: the low
// bits are the value.
value_t
constant_value(state_t &s, long long value, view_t view)
{
    const value_t v = fresh(s);
    constant(s, reg_of(s, v), value, width(view));

    return v;
}

// The load row, 6.3.2.1p2, rcc's load() at the object's width:
// movl -4(%rbp), %eax, movq -8(%rbp), %rax.  G8: one mov.
value_t
load(state_t &s, slot_t where, view_t view)
{
    const auto [mnemonic, to] = extension(view, width(view));

    const value_t v = fresh(s);
    insn(s, mnemonic, { at(slot_of(s, where)), reg(reg_of(s, v), to) });

    return v;
}

value_t
extending_load(state_t &s, slot_t where, view_t from)
{
    const auto [mnemonic, to] = extension(from, 64);

    const value_t v = fresh(s);
    insn(s, mnemonic, { at(slot_of(s, where)), reg(reg_of(s, v), to) });

    return v;
}

// rcc's extended(): the extension in v's own register, but in the one
// the consumer names where a row may write it now.
value_t
extended(state_t &s, value_t v, view_t from, view_t to, unsigned wanted)
{
    const auto [mnemonic, bits] = extension(from, width(to));
    const reg_t source = reg_of(s, v);
    const auto named = wanted_register(s, wanted);

    if (!named) {
        insn(s, mnemonic, { reg(source, width(from)), reg(source, bits) });
        return v;
    }

    release(s, v);

    const value_t e = hold(s, *named);
    insn(s, mnemonic, { reg(source, width(from)), reg(*named, bits) });

    return e;
}

// neg and not have one address, the operand's register: negl %eax,
// notq %rax.  G1: neg of the most negative value is itself.  neg sets
// ZF from its result, and not sets no flag (7.1).
void
operate(state_t &s, unary_op_t op, reg_t r, view_t view)
{
    const int bits = width(view);

    insn(s, sized(op == unary_op_t::neg ? "neg" : "not", bits),
         { reg(r, bits) });
    if (op == unary_op_t::neg)
        flagged(s, bits);
}

// op R, L, two addresses, the result in L, which is r: addl $5, %eax,
// addq -16(%rbp), %rcx.  add, sub, and, or and xor set ZF from L, and
// imul and the shifts do not (7.1).
void
operate(
    state_t &s, binary_op_t op, reg_t r, std::string_view source, view_t view)
{
    const int bits = width(view);

    insn(s, sized(mnemonic(op), bits), { source, reg(r, bits) });
    if (flag_operator(op))
        flagged(s, bits);
}

// addl -8(%rbp), %eax.
void
operate(state_t &s, binary_op_t op, reg_t r, slot_t source, view_t view)
{
    operate(s, op, r, at(slot_of(s, source)), view);
}

// addl %ecx, %eax.
void
operate(state_t &s, binary_op_t op, reg_t r, reg_t source, view_t view)
{
    operate(s, op, r, reg(source, width(view)), view);
}

// op R, L into L, R released, rcc's arithmetic() on two held values,
// chapter 6.1, the registers of operands(), a spilled value reloaded;
// but an operator that commutes writes R's register where
// written_in_right() says, from L's register or its spill slot, and L
// is released: the count (0 << d) + (d = c) is addl %edx, %ecx, in the
// ecx its consumer names, and a spilled L with no register named is
// addl -16(%rbp), %ecx.
value_t
arithmetic(state_t &s,
           binary_op_t op,
           value_t l,
           value_t r,
           view_t view,
           unsigned wanted)
{
    if (written_in_right(s, op, l, r, wanted)) {
        const reg_t right = reg_of(s, r);
        operate(s, op, right, source_of(s, l, view), view);
        release(s, l);
        return r;
    }

    const auto [left, right] = operands(s, l, r);
    operate(s, op, left, right, view);
    release(s, r);

    return l;
}

// imull $5, %eax, %eax.
void
multiply(
    state_t &s, reg_t r, reg_t source, std::string_view factor, view_t view)
{
    const int bits = width(view);

    insn(s, sized("imul", bits), { factor, reg(source, bits), reg(r, bits) });
}

// imull $5, -4(%rbp), %eax.
void
multiply(
    state_t &s, reg_t r, slot_t source, std::string_view factor, view_t view)
{
    const int bits = width(view);

    insn(s, sized("imul", bits),
         { factor, at(slot_of(s, source)), reg(r, bits) });
}

// rcc's divide() by a held divisor: the division, pin_division(), the
// divisor consumed, then the value, divided().
value_t
divide(state_t &s,
       binary_op_t op,
       value_t l,
       value_t r,
       view_t view,
       unsigned wanted)
{
    const reg_t left = pin_division(s, op, l, r, width(view));
    release(s, r);

    return divided(s, op, l, left, width(view), wanted);
}

// rcc's divide() by the object in a slot.
value_t
divide(state_t &s,
       binary_op_t op,
       value_t l,
       slot_t divisor,
       view_t view,
       unsigned wanted)
{
    const reg_t left = pin_division(s, op, l, divisor, width(view));

    return divided(s, op, l, left, width(view), wanted);
}

// rcc's shift() for a held count, chapter 6.5: the count's register,
// then the shifted value's, a spilled one reloaded outside it and rcx;
// the count goes into cl, count_into_cl(), and the value in rcx moves
// out of its way, the shifted value too if it stands there, to a
// register outside its own and the count's, all 64 bits; the shifted
// value's register is read after the moves, since its handle follows
// it.  G3: the instruction masks the count to 5 bits, and nothing else
// does.  The rows generated the shifted value with rcx reserved and the
// count where cl is likely, so the moves are for a value that is in the
// way all the same.
void
shift(
    state_t &s, binary_op_t op, value_t l, value_t r, view_t view, view_t count)
{
    const reg_t right = reg_of(s, r, held_in(s, l));
    const reg_t left = reg_of(s, l, bit(right) | bit(rcx));
    count_into_cl(s, right, bit(left), count);

    const int bits = width(view);
    insn(s, sized(mnemonic(op), bits), { "%cl", reg(reg_of(s, l), bits) });
    release(s, r);
}

// cmp R, L in AT&T order, the source first: cmpl $5, %eax.
void
compare(state_t &s, value_t l, std::string_view source, view_t view)
{
    const int bits = width(view);

    insn(s, sized("cmp", bits), { source, reg(reg_of(s, l), bits) });
}

// cmpl -8(%rbp), %eax.
void
compare(state_t &s, value_t l, slot_t source, view_t view)
{
    compare(s, l, at(slot_of(s, source)), view);
}

// cmpl %ecx, %eax: operands() asks for r's register first, then l's.
void
compare(state_t &s, value_t l, value_t r, view_t view)
{
    const int bits = width(view);

    const auto [left, right] = operands(s, l, r);
    insn(s, sized("cmp", bits), { reg(right, bits), reg(left, bits) });

    release(s, r);
}

// cmpl $5, -4(%rbp).
void
compare(state_t &s, slot_t where, std::string_view immediate, view_t view)
{
    insn(s, sized("cmp", width(view)), { immediate, at(slot_of(s, where)) });
}

// rcc's comparison(): the setcc writes l's register, where the cmp left
// l, but where the consumer names a register, wanted, that is free now,
// setl %al after cmpl %eax, %ecx, l released; and where the cmp held
// nothing, the register wanted names or a fresh one.  setcc writes the
// low byte alone, so movzbl extends it to the int the operator yields.
value_t
setcc(state_t &s, binary_op_t op, std::optional< value_t > l, unsigned wanted)
{
    const auto named = wanted_register(s, wanted);
    if (!l || named) {
        if (l)
            release(s, *l);
        l = named ? hold(s, *named) : fresh(s);
    }

    const reg_t r = reg_of(s, *l);
    insn(s, std::format("set{}", condition(op)), { reg(r, 8) });
    insn(s, "movzbl", { reg(r, 8), reg(r, 32) });

    return *l;
}

void
test_zero(state_t &s, value_t v, view_t view, int label, bool sense)
{
    compare(s, v, imm(0), view);
    jump_if(s, sense ? binary_op_t::ne : binary_op_t::eq, label);
}

void
in_place_branch(state_t &s, slot_t where, view_t view, int label, bool sense)
{
    compare(s, where, imm(0), view);
    jump_if(s, sense ? binary_op_t::ne : binary_op_t::eq, label);
}

// In every row of a flag condition rcc-v2 writes, the instruction that
// sets ZF is the row's last, at the condition's width, so the cmp is
// the record's guard alone.
void
flag_branch(state_t &s, value_t v, view_t view, int label, bool sense)
{
    if (s.zero_flag != width(view))
        compare(s, v, imm(0), view);
    release(s, v);

    jump_if(s, sense ? binary_op_t::ne : binary_op_t::eq, label);
}

// test R, L in AT&T order: testl $1, %eax.
void
test(state_t &s, value_t l, std::string_view source, view_t view)
{
    const int bits = width(view);

    insn(s, sized("test", bits), { source, reg(reg_of(s, l), bits) });
    flagged(s, bits);
}

// testl -8(%rbp), %eax.
void
test(state_t &s, value_t l, slot_t source, view_t view)
{
    test(s, l, at(slot_of(s, source)), view);
}

// testl %ecx, %eax: operands() asks for r's register first, then l's.
void
test(state_t &s, value_t l, value_t r, view_t view)
{
    const int bits = width(view);

    const auto [left, right] = operands(s, l, r);
    insn(s, sized("test", bits), { reg(right, bits), reg(left, bits) });
    flagged(s, bits);

    release(s, r);
}

// testl $1, -4(%rbp).
void
test(state_t &s, slot_t where, std::string_view immediate, view_t view)
{
    const int bits = width(view);

    insn(s, sized("test", bits), { immediate, at(slot_of(s, where)) });
    flagged(s, bits);
}

void
move_immediate(state_t &s, reg_t r, int value)
{
    constant(s, r, value, 32);
}

// The store row, rcc's store(): the mov of the view's width, movl %eax,
// -4(%rbp), movq %rax, -8(%rbp); G2, the low bytes of the register are
// the value.
void
store(state_t &s, reg_t r, slot_t where, view_t view)
{
    store(s, reg(r, width(view)), where, view);
}

// The store of an immediate, rcc's initialize(): movl $7, -4(%rbp),
// movq $0, -8(%rbp).
void
store(state_t &s, std::string_view immediate, slot_t where, view_t view)
{
    insn(s, sized("mov", width(view)), { immediate, at(slot_of(s, where)) });
}

// The count's low 8 bits as written, not masked to the width, rcc's
// right_operand(): shll $33, %eax, and $255 for -1.
std::string
count_of(long long count)
{
    return imm(count & 0xff);
}

unsigned
call_pins(const state_t &s, const std::vector< argument_t > &args)
{
    unsigned pinned = bit(rax);
    for (const argument_t &a : args)
        pinned |= pinned_within(s, a.value);

    return pinned;
}

// The value goes to the convention's result register, 7.2, rcc's
// return_stmt() and rvalue_in(): the value's row is handed the result
// register as the one its consumer names, and the value is moved where
// its row left it elsewhere.  A statement starts with no value held, so
// the first register a row takes, the lowest, is the result register.
// Every return jumps to the one epilogue, the last statement of the
// body included: the jump to the next instruction is what the statement
// says, and removing it was the optional peephole pass's work in the
// PDP-11 compiler, which rcc has no counterpart of.
// The move is at the result's width, movl %edx, %eax for an int
// remainder and movq %rdx, %rax for a long one, rcc's return_stmt() at
// max(32, width).
void
return_stmt(state_t &s, const expr_t &e, view_t view)
{
    const value_t v = value(s, e, bit(result_register));
    const reg_t from = reg_of(s, v);
    if (from != result_register)
        mov(s, from, result_register, width(view));

    release(s, v);

    jump(s, s.ret);
}

// rcc's call() after the arguments' stores, chapter 8: a slot is 4
// bytes, and a push writes 8, so the stacked arguments, past the sixth,
// are pushed from their slots through rax, last first, movl -40(%rbp),
// %eax and pushq %rax, over a pad that keeps rsp a multiple of 16 at
// the call, subq $8, %rsp, for an odd count of them; the first six are
// loaded into their registers, movl -16(%rbp), %edi.  G16: call f@PLT
// for every function, the executable being position-independent, and
// nothing before it, since no callee is variadic.  The pushes and the
// pad are given back after it, addq $16, %rsp.  The function now calls,
// so its frame moves rsp.
void
transfer(state_t &s,
         std::string_view name,
         const std::vector< argument_t > &args)
{
    const std::size_t registers = std::size(argument_registers);
    const std::size_t stacked =
        args.size() > registers ? args.size() - registers : 0;
    const bool pad = stacked % 2 != 0;

    if (pad)
        insn(s, "subq", { imm(8), "%rsp" });

    for (std::size_t i = args.size(); i-- > registers;) {
        insn(s, "movl", { at(slot_of(s, args[i].where)), "%eax" });
        insn(s, "pushq", { "%rax" });
    }

    for (std::size_t i = 0; i < std::min(registers, args.size()); ++i)
        insn(s, "movl",
             { at(slot_of(s, args[i].where)), reg(argument_registers[i], 32) });

    insn(s, "call", { std::format("{}@PLT", name) });

    const long popped = 8 * static_cast< long >(stacked) + (pad ? 8 : 0);
    if (popped > 0)
        insn(s, "addq", { imm(popped), "%rsp" });

    s.calls = true;
}

namespace {

// the frame

// Chapter 5: rbp is the frame pointer in every function, pushed and
// pointed at the frame, each with its place in the unwind table.  No
// register is pushed below it.  The slots lie below rbp, and a function
// that calls nothing keeps a frame of up to the red zone's 128 bytes
// there without moving rsp; a larger one, or any frame of a function
// that calls, moves rsp below it by its size, which keeps rsp a
// multiple of 16 at every call.
void
prologue(std::ostream &out, long size, bool calls)
{
    out << "\tpushq\t%rbp\n"
           "\t.cfi_def_cfa_offset 16\n"
           "\t.cfi_offset %rbp, -16\n"
           "\tmovq\t%rsp, %rbp\n"
           "\t.cfi_def_cfa_register %rbp\n";

    if (size > 0 && (calls || size > red_zone))
        out << std::format("\tsubq\t${}, %rsp\n", size);
}

// The one epilogue, leave, which puts rsp back at rbp, below the frame
// or not; the directive after it is the unwind table's, since the
// canonical frame address is rsp + 8 from leave to ret.
void
epilogue(std::ostream &out, const state_t &s)
{
    out << label_name(s.function, s.ret) << ":\n";

    out << "\tleave\n"
           "\t.cfi_def_cfa %rsp, 8\n"
           "\tret\n";
}

// The parameters' half of the prologue, before the body, since the
// body's slots come after theirs, rcc's parameters(): each of the first
// six takes the next slot and is stored there from its argument
// register, movl %edi, -4(%rbp); the seventh and later are where the
// caller pushed them, 16(%rbp) and up, 8 bytes apart, and are read and
// written there.  Each displacement is recorded under the parameter's
// number, the first locals' (docs/typed-tree.md 4.5).
void
parameters(state_t &s, std::size_t count)
{
    const std::size_t registers = std::size(argument_registers);

    for (std::size_t i = 0; i < count; ++i) {
        const int id = static_cast< int >(i);

        if (i >= registers) {
            s.slots.emplace(id, 16 + 8 * static_cast< int >(i - registers));
            continue;
        }

        s.slots.emplace(id, allocate(s, 4, 4));
        store(s, argument_registers[i], slot_t { id }, view_t::i32);
    }
}

// A function's symbol is global, .globl, where its linkage is external,
// and local otherwise, chapter 10, rcc's binding().
void
binding(std::ostream &out, const function_decl_t &fd)
{
    if (fd.external)
        out << std::format("\t.globl\t{}\n", fd.name);
}

// A definition: the body generated into the state's buffer, then
// written inside the frame, whose size is known after the body: the
// slots rounded up to 16, so that a frame in the red zone stays in it.
// The one epilogue is reached with rax as it is, G5's some value, but
// for main, which returns 0 when it falls off its end, 5.1.2.3.4, by
// its name, whatever its result type.
void
function(std::ostream &out, const function_decl_t &fd)
{
    state_t s { .function = fd.name };
    s.ret = new_label(s);

    parameters(s, fd.parameters);
    generate(s, *fd.body);

    if (!s.terminated && s.function == "main")
        constant(s, result_register, 0, 32);

    binding(out, fd);
    out << std::format("\t.type\t{}, @function\n"
                       "{}:\n"
                       "\t.cfi_startproc\n",
                       fd.name, fd.name);

    const long size = (s.frame + 15) / 16 * 16;

    prologue(out, size, s.calls);

    out << s.body.str();

    epilogue(out, s);
    out << std::format("\t.cfi_endproc\n"
                       "\t.size\t{}, .-{}\n",
                       fd.name, fd.name);
}

} // namespace

// The functions the unit defines, in order; one it only declares writes
// nothing, since the assembler leaves its symbol to the link.  The note
// section marks the stack not executable.
void
generate(const translation_unit_t &unit,
         std::string_view file,
         std::ostream &assembly)
{
    assembly << std::format("\t.file\t{}\n\t.text\n", quoted_file(file));

    for (const function_decl_t &fd : unit.functions)
        if (fd.body)
            function(assembly, fd);

    assembly << "\t.section\t.note.GNU-stack,\"\",@progbits\n";
}

} // namespace rcc::amd64
