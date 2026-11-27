// -*- mode: c++; -*-

// x86-64's text, declared in amd64.hh: AT&T syntax for the system
// assembler, as rcc writes it.  docs/backend.md is the design, and each
// function names the row or the part of the frame it writes.  The
// frame is rbp's and the locals' slots below it, chapter 5.

#include "amd64.hh"

#include "assembly.hh"
#include "target.hh"

#include <cstdint>
#include <format>
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

// The copy of a register into another at a width, 32 or 64 bits, the
// source first in AT&T order, rcc's mov(): movl %edx, %eax, movq %rax,
// %rsi.
void
mov(state_t &s, reg_t from, reg_t to, int width)
{
    insn(s, width == 64 ? "movq" : "movl",
         { reg(from, width), reg(to, width) });
}

// A jump to the function's label n; no path reaches what follows it.
void
jump(state_t &s, int n)
{
    insn(s, "jmp", { label_name(s.function, n) });
    s.terminated = true;
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
    case binary_op_t::bit_and: return "andl";
    case binary_op_t::bit_or: return "orl";
    case binary_op_t::bit_xor: return "xorl";
    }
}

} // namespace

// A register for a value: the lowest free one outside avoid and
// s.reserved, else the lowest free one outside avoid.  The reservation
// is an avoidance, not a pin.  With none free outside avoid rcc spills
// the oldest held value there, backend.md 6.2, which rcc-v2 does not
// yet.
reg_t
take(state_t &s, unsigned avoid)
{
    for (const unsigned out : { avoid | s.reserved, avoid })
        for (const reg_t r : expression_registers)
            if (!s.registers.owner.contains(r) && !(out & bit(r)))
                return r;

    throw std::logic_error("v2: not yet: a spill");
}

namespace {

// Moves the value in from to the free register to, at a width: a real
// mov, and the record follows it (chapter 6.5), so the value's handle
// names to from here on.
void
move(state_t &s, reg_t from, reg_t to, int width)
{
    const int id = s.registers.owner.at(from);

    mov(s, from, to, width);

    s.registers.values[id] = to;
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

// Whether the row of op on the held values l and r writes r's register,
// rcc's right_written(): an operator that commutes, +, *, &, | or ^,
// whose right operand is in the register the consumer names, wanted,
// where l's register would be written and moved.
bool
right_written(const state_t &s, binary_op_t op, value_t r, unsigned wanted)
{
    return commutes(op) && (wanted & bit(reg_of(s, r)));
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
// vacated, rcc's dividend(): the one it is held in.  A dividend held in
// rdx, which the division overwrites, moves to rax for a quotient, by
// the mov idiv() writes in any case, so that it is in rax and the
// quotient stays where idivl leaves it.  A remainder's value is rdx
// whatever the dividend's register, so its dividend stays.
reg_t
dividend(state_t &s, binary_op_t op, value_t l)
{
    const reg_t r = reg_of(s, l);
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
// vacate().  Both values are still held after idiv(), the dividend in
// the register returned.
reg_t
pin_division(state_t &s, binary_op_t op, value_t l, value_t r)
{
    const unsigned pinned = bit(rax) | bit(rdx);
    const unsigned in = bit(reg_of(s, l));

    reg_t right = reg_of(s, r);
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
// and rdx move out, vacate().  The dividend is still held, in the
// register returned.
reg_t
pin_division(state_t &s, binary_op_t op, value_t l, slot_t divisor)
{
    const unsigned pinned = bit(rax) | bit(rdx);
    const unsigned in = bit(reg_of(s, l));

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

// The next slot below the last, an int's 4 bytes, which keep every
// slot at an int's alignment; its displacement off rbp is recorded
// under the local's number.  G12: a slot is never shared and never
// reused inside the activation.  A frame past the target's limit is
// rcc's error, which no program rcc-v2 compiles comes near: it is
// refused.
void
allocate(state_t &s, slot_t where)
{
    s.frame += 4;
    if (s.frame > x86_64.frame_limit)
        throw std::logic_error("v2: not yet: a frame past the target's limit");

    s.slots.emplace(where.id, static_cast< int >(-s.frame));
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
// notl %eax.  G1: neg of INT_MIN is INT_MIN.
void
operate(state_t &s, unary_op_t op, reg_t r)
{
    insn(s, op == unary_op_t::neg ? "negl" : "notl", { reg(r, 32) });
}

// op R, L, two addresses, the result in L, which is r: addl $5, %eax.
void
operate(state_t &s, binary_op_t op, reg_t r, std::string_view immediate)
{
    insn(s, mnemonic(op), { immediate, reg(r, 32) });
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
// chapter 6.1; but an operator that commutes writes R's register where
// it is the one the consumer names, right_written(), and L is released:
// the count (0 << d) + (d = c) is addl %edx, %ecx, in the ecx its
// consumer names.
value_t
arithmetic(state_t &s, binary_op_t op, value_t l, value_t r, unsigned wanted)
{
    if (right_written(s, op, r, wanted)) {
        operate(s, op, reg_of(s, r), reg_of(s, l));
        release(s, l);
        return r;
    }

    operate(s, op, reg_of(s, l), reg_of(s, r));
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

// rcc's shift() for a held count, chapter 6.5: the count goes into cl,
// count_into_cl(), and the value in rcx moves out of its way, the
// shifted value too if it stands there, to a register outside its own
// and the count's, all 64 bits; the shifted value's register is read
// after the moves, since its handle follows it.  G3: the instruction
// masks the count to 5 bits, and nothing else does.  The rows
// generated the shifted value with rcx reserved and the count where cl
// is likely, so the moves are for a value that is in the way all the
// same.
void
shift(state_t &s, binary_op_t op, value_t l, value_t r)
{
    count_into_cl(s, reg_of(s, r), bit(reg_of(s, l)));

    insn(s, mnemonic(op), { "%cl", reg(reg_of(s, l), 32) });
    release(s, r);
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

// A definition: the body generated into the state's buffer, then
// written inside the frame, whose size is known after the body: the
// slots rounded up to 16, so that a frame in the red zone stays in it.
// The one epilogue is reached with rax as it is, G5's some value, but
// for main, which returns 0 when it falls off its end, 5.1.2.3.4.
// Every function has external linkage, so its symbol is global,
// .globl, chapter 10.
void
function(std::ostream &out, const function_definition_t &fd)
{
    state_t s { .function = fd.name };
    s.ret = new_label(s);

    generate(s, fd.body);

    if (!s.terminated && s.function == "main")
        constant(s, result_register, 0, 32);

    out << std::format("\t.globl\t{}\n"
                       "\t.type\t{}, @function\n"
                       "{}:\n"
                       "\t.cfi_startproc\n",
                       fd.name, fd.name, fd.name);

    const long size = (s.frame + 15) / 16 * 16;

    prologue(out, size);

    out << s.body.str();

    epilogue(out, s);
    out << std::format("\t.cfi_endproc\n"
                       "\t.size\t{}, .-{}\n",
                       fd.name, fd.name);
}

} // namespace

// The functions the unit defines, in order.  The note section marks the
// stack not executable.
void
generate(const translation_unit_t &unit,
         std::string_view file,
         std::ostream &assembly)
{
    assembly << std::format("\t.file\t{}\n\t.text\n", quoted_file(file));

    for (const function_definition_t &fd : unit.functions)
        function(assembly, fd);

    assembly << "\t.section\t.note.GNU-stack,\"\",@progbits\n";
}

} // namespace rcc::amd64
