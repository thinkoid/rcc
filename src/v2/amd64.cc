// -*- mode: c++; -*-

// x86-64's text, declared in amd64.hh: AT&T syntax for the system
// assembler, as rcc writes it.  docs/backend.md is the design, and each
// function names the row or the part of the frame it writes.  The
// frame is rbp's and the locals' slots below it, chapter 5.

#include "amd64.hh"

#include "assembly.hh"
#include "target.hh"

#include <cstddef>
#include <cstdint>
#include <format>
#include <iterator>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>

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

// An immediate, as the assembler reads it.
std::string
imm(long long value)
{
    return std::format("${}", value);
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
    s.zero_flag = false;
    rcc::insn(s, mnemonic, operands);
}

// The instruction just written set ZF from the value it computed, as
// Intel's reference has add, sub, and, or, xor, neg and test do
// (backend.md 7.1); it stands until the next instruction or label.
void
flagged(state_t &s)
{
    s.zero_flag = true;
}

// The copy of a register into another at a width, 32 or 64 bits, the
// source first in AT&T order, rcc's mov(): movl %edx, %eax, movq %rax,
// %rsi.
void
mov(state_t &s, reg_t from, reg_t to, int width)
{
    insn(s, width == 64 ? "movq" : "movl",
         { reg(from, width), reg(to, width) });
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

// The instruction of a binary operator at 32 bits, rcc's
// arithmetic_mnemonic() and shift_mnemonic() sized l: sar for >>,
// signed, and idiv for / and % alike, which idiv() writes.
std::string_view
mnemonic(binary_op_t op)
{
    switch (op) {
    case binary_op_t::add: return "addl";
    case binary_op_t::sub: return "subl";
    case binary_op_t::mul: return "imull";
    case binary_op_t::div:
    case binary_op_t::rem: return "idivl";
    case binary_op_t::shl: return "shll";
    case binary_op_t::shr: return "sarl";
    case binary_op_t::lt:
    case binary_op_t::gt:
    case binary_op_t::le:
    case binary_op_t::ge:
    case binary_op_t::eq:
    case binary_op_t::ne:
    case binary_op_t::log_and:
    case binary_op_t::log_or:
        throw std::logic_error("v2: an operator of no arithmetic instruction");
    case binary_op_t::bit_and: return "andl";
    case binary_op_t::bit_or: return "orl";
    case binary_op_t::bit_xor: return "xorl";
    }
}

// The condition code of a comparison of op on int, the suffix of setcc
// and jcc, rcc's condition() on signed operands.
std::string_view
condition(binary_op_t op)
{
    switch (op) {
    case binary_op_t::lt: return "l";
    case binary_op_t::le: return "le";
    case binary_op_t::gt: return "g";
    case binary_op_t::ge: return "ge";
    case binary_op_t::eq: return "e";
    default: return "ne";
    }
}

} // namespace

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
    s.zero_flag = false;
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
// and the count is moved there at its own width.
void
count_into_cl(state_t &s, reg_t right, unsigned avoid)
{
    if (right == rcx)
        return;

    vacate(s, rcx, avoid | bit(right));
    move(s, right, rcx, 32);
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

// The operand by which an instruction reads the held value v at 32
// bits, rcc's source_of(): its register, or for a spilled value its
// slot, read in place at the instruction's width, -16(%rbp), 4 bytes at
// the slot's low end, and not reloaded (6.2).
std::string
source_of(state_t &s, value_t v)
{
    const auto &h = s.registers.values[v.id];
    if (!h.reg)
        return at(h.spill);

    return reg(reg_of(s, v), 32);
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

// G4: the instruction the machine has, idivl, rcc's idiv(), a fault on
// x86 for a zero divisor or INT_MIN / -1.  rax and rdx hold nothing
// but, it may be, the dividend in left: it goes to rax, rdx is
// sign-filled, and the division of the divisor's operand leaves the
// quotient in rax and the remainder in rdx.
void
idiv(state_t &s, binary_op_t op, reg_t left, std::string_view divisor)
{
    if (left != rax)
        mov(s, left, rax, 32);

    insn(s, "cltd", { });
    insn(s, mnemonic(op), { divisor });
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
dividend(state_t &s, binary_op_t op, value_t l)
{
    const reg_t r = reg_of(s, l, ~bit(rax));
    if (r != rdx || op != binary_op_t::div)
        return r;

    move(s, rdx, rax, 32);

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
pin_division(state_t &s, binary_op_t op, value_t l, value_t r)
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

    const reg_t left = dividend(s, op, l);
    idiv(s, op, left, reg(right, 32));

    return left;
}

// The pinned pair of a division by the int in a slot, chapter 6.5: the
// slot is idivl's operand, idivl -8(%rbp), and the values held in rax
// and rdx move out, vacate(); a dividend spilled is reloaded into rax
// after them.  The dividend is still held, in the register returned.
reg_t
pin_division(state_t &s, binary_op_t op, value_t l, slot_t divisor)
{
    const unsigned pinned = bit(rax) | bit(rdx);
    const unsigned in = held_in(s, l);

    for (const reg_t p : { rax, rdx })
        if (!(in & bit(p)))
            vacate(s, p, pinned | in);

    const reg_t left = dividend(s, op, l);
    idiv(s, op, left, at(slot_of(s, divisor)));

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
divided(state_t &s, binary_op_t op, value_t l, reg_t left, unsigned wanted)
{
    const bool stays = left != rax && wanted_register(s, wanted) == rax;
    if (op == binary_op_t::rem || stays) {
        release(s, l);
        return hold(s, op == binary_op_t::rem ? rdx : rax);
    }

    if (left != rax)
        mov(s, rax, left, 32);

    return l;
}

} // namespace

unsigned
held_out(const state_t &s, value_t l, unsigned named)
{
    return held_in(s, l) & named ? named : 0;
}

value_t
value(state_t &s, const literal_t &l, unsigned)
{
    const value_t v = fresh(s);
    constant(s, reg_of(s, v), l.value, 32);

    return v;
}

value_t
value(state_t &s, const zero_t &, unsigned)
{
    const value_t v = fresh(s);
    constant(s, reg_of(s, v), 0, 32);

    return v;
}

// The load row, 6.3.2.1p2: movl -4(%rbp), %eax.  G8: one mov.
value_t
value(state_t &s, const load_t &l, unsigned)
{
    const value_t v = fresh(s);
    insn(s, "movl", { at(slot_of(s, l.where)), reg(reg_of(s, v), 32) });

    return v;
}

// neg and not have one address, the operand's register: negl %eax,
// notl %eax.  G1: neg of INT_MIN is INT_MIN.  neg sets ZF from its
// result, and not sets no flag (7.1).
void
operate(state_t &s, unary_op_t op, reg_t r)
{
    insn(s, op == unary_op_t::neg ? "negl" : "notl", { reg(r, 32) });
    if (op == unary_op_t::neg)
        flagged(s);
}

// op R, L, two addresses, the result in L, which is r: addl $5, %eax,
// addl -16(%rbp), %ecx.  add, sub, and, or and xor set ZF from L, and
// imul and the shifts do not (7.1).
void
operate(state_t &s, binary_op_t op, reg_t r, std::string_view source)
{
    insn(s, mnemonic(op), { source, reg(r, 32) });
    if (flag_operator(op))
        flagged(s);
}

// addl -8(%rbp), %eax.
void
operate(state_t &s, binary_op_t op, reg_t r, slot_t source)
{
    operate(s, op, r, at(slot_of(s, source)));
}

// addl %ecx, %eax.
void
operate(state_t &s, binary_op_t op, reg_t r, reg_t source)
{
    operate(s, op, r, reg(source, 32));
}

// op R, L into L, R released, rcc's arithmetic() on two held values,
// chapter 6.1, the registers of operands(), a spilled value reloaded;
// but an operator that commutes writes R's register where
// written_in_right() says, from L's register or its spill slot, and L
// is released: the count (0 << d) + (d = c) is addl %edx, %ecx, in the
// ecx its consumer names, and a spilled L with no register named is
// addl -16(%rbp), %ecx.
value_t
arithmetic(state_t &s, binary_op_t op, value_t l, value_t r, unsigned wanted)
{
    if (written_in_right(s, op, l, r, wanted)) {
        const reg_t right = reg_of(s, r);
        operate(s, op, right, source_of(s, l));
        release(s, l);
        return r;
    }

    const auto [left, right] = operands(s, l, r);
    operate(s, op, left, right);
    release(s, r);

    return l;
}

// imull $5, %eax, %eax.
void
multiply(state_t &s, reg_t r, reg_t source, std::string_view factor)
{
    insn(s, "imull", { factor, reg(source, 32), reg(r, 32) });
}

// imull $5, -4(%rbp), %eax.
void
multiply(state_t &s, reg_t r, slot_t source, std::string_view factor)
{
    insn(s, "imull", { factor, at(slot_of(s, source)), reg(r, 32) });
}

// rcc's divide() by a held divisor: the division, pin_division(), the
// divisor consumed, then the value, divided().
value_t
divide(state_t &s, binary_op_t op, value_t l, value_t r, unsigned wanted)
{
    const reg_t left = pin_division(s, op, l, r);
    release(s, r);

    return divided(s, op, l, left, wanted);
}

// rcc's divide() by the int in a slot.
value_t
divide(state_t &s, binary_op_t op, value_t l, slot_t divisor, unsigned wanted)
{
    return divided(s, op, l, pin_division(s, op, l, divisor), wanted);
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
shift(state_t &s, binary_op_t op, value_t l, value_t r)
{
    const reg_t right = reg_of(s, r, held_in(s, l));
    const reg_t left = reg_of(s, l, bit(right) | bit(rcx));
    count_into_cl(s, right, bit(left));

    insn(s, mnemonic(op), { "%cl", reg(reg_of(s, l), 32) });
    release(s, r);
}

// cmp R, L in AT&T order, the source first: cmpl $5, %eax.
void
compare(state_t &s, value_t l, std::string_view source)
{
    insn(s, "cmpl", { source, reg(reg_of(s, l), 32) });
}

// cmpl -8(%rbp), %eax.
void
compare(state_t &s, value_t l, slot_t source)
{
    compare(s, l, at(slot_of(s, source)));
}

// cmpl %ecx, %eax: operands() asks for r's register first, then l's.
void
compare(state_t &s, value_t l, value_t r)
{
    const auto [left, right] = operands(s, l, r);
    insn(s, "cmpl", { reg(right, 32), reg(left, 32) });

    release(s, r);
}

// cmpl $5, -4(%rbp).
void
compare(state_t &s, slot_t where, std::string_view immediate)
{
    insn(s, "cmpl", { immediate, at(slot_of(s, where)) });
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

value_t
value(state_t &s, const unary_t< unary_op_t::log_not > &u, unsigned wanted)
{
    const value_t v = value(s, u.arg);
    compare(s, v, imm(0));

    return setcc(s, binary_op_t::eq, v, wanted);
}

void
test_zero(state_t &s, value_t v, int label, bool sense)
{
    compare(s, v, imm(0));
    jump_if(s, sense ? binary_op_t::ne : binary_op_t::eq, label);
}

void
value_branch(state_t &s, const load_t &l, int label, bool sense)
{
    compare(s, l.where, imm(0));
    jump_if(s, sense ? binary_op_t::ne : binary_op_t::eq, label);
}

// In every row of a flag condition rcc-v2 writes, the instruction that
// sets ZF is the row's last, so the cmp is the record's guard alone.
void
flag_branch(state_t &s, value_t v, int label, bool sense)
{
    if (!s.zero_flag)
        compare(s, v, imm(0));
    release(s, v);

    jump_if(s, sense ? binary_op_t::ne : binary_op_t::eq, label);
}

// test R, L in AT&T order: testl $1, %eax.
void
test(state_t &s, value_t l, std::string_view source)
{
    insn(s, "testl", { source, reg(reg_of(s, l), 32) });
    flagged(s);
}

// testl -8(%rbp), %eax.
void
test(state_t &s, value_t l, slot_t source)
{
    test(s, l, at(slot_of(s, source)));
}

// testl %ecx, %eax: operands() asks for r's register first, then l's.
void
test(state_t &s, value_t l, value_t r)
{
    const auto [left, right] = operands(s, l, r);
    insn(s, "testl", { reg(right, 32), reg(left, 32) });
    flagged(s);

    release(s, r);
}

// testl $1, -4(%rbp).
void
test(state_t &s, slot_t where, std::string_view immediate)
{
    insn(s, "testl", { immediate, at(slot_of(s, where)) });
    flagged(s);
}

void
move_immediate(state_t &s, reg_t r, int value)
{
    constant(s, r, value, 32);
}

// The store row: movl %eax, -4(%rbp).
void
store(state_t &s, reg_t r, slot_t where)
{
    store(s, reg(r, 32), where);
}

// The store of an immediate: movl $7, -4(%rbp).
void
store(state_t &s, std::string_view immediate, slot_t where)
{
    insn(s, "movl", { immediate, at(slot_of(s, where)) });
}

// x86 immediates are 32 bits, so every int is one.
std::string
immediate_of(const literal_t &l)
{
    return imm(l.value);
}

std::string
immediate_of(const zero_t &)
{
    return imm(0);
}

// The count's low 8 bits as written, not masked to 5, rcc's
// right_operand(): shll $33, %eax, and $255 for -1.
std::string
count_of(int count)
{
    return imm(count & 0xff);
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
void
generate(state_t &s, const return_t &r)
{
    const value_t v = value(s, r.value, bit(result_register));
    const reg_t from = reg_of(s, v);
    if (from != result_register)
        mov(s, from, result_register, 32);

    release(s, v);

    jump(s, s.ret);
}

namespace {

// the frame

// Chapter 5: rbp is the frame pointer in every function, pushed and
// pointed at the frame, each with its place in the unwind table.  No
// register is pushed below it.  The slots lie below rbp, and a
// function that calls nothing keeps a frame of up to the red zone's
// 128 bytes there without moving rsp; a larger one moves rsp below it
// by its size.
void
prologue(std::ostream &out, long size)
{
    out << "\tpushq\t%rbp\n"
           "\t.cfi_def_cfa_offset 16\n"
           "\t.cfi_offset %rbp, -16\n"
           "\tmovq\t%rsp, %rbp\n"
           "\t.cfi_def_cfa_register %rbp\n";

    if (size > red_zone)
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
        const int above = 16 + 8 * (id - static_cast< int >(registers));

        if (i >= registers) {
            s.slots.emplace(id, above);
            continue;
        }

        s.slots.emplace(id, allocate(s, 4, 4));
        store(s, argument_registers[i], slot_t { id });
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

    prologue(out, size);

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
