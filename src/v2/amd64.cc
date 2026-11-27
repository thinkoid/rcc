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

// The copy of a register into another at 32 bits, the source first in
// AT&T order, rcc's mov() sized l: movl %edx, %eax.
void
mov(state_t &s, reg_t from, reg_t to)
{
    insn(s, "movl", { reg(from, 32), reg(to, 32) });
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
// arithmetic_mnemonic() sized l.
std::string_view
mnemonic(binary_op_t op)
{
    switch (op) {
    case binary_op_t::add: return "addl";
    case binary_op_t::sub: return "subl";
    case binary_op_t::bit_and: return "andl";
    case binary_op_t::bit_or: return "orl";
    case binary_op_t::bit_xor: return "xorl";
    }
}

} // namespace

// A register for a value: the lowest free one.  With every one held rcc
// spills the oldest, backend.md 6.2, which rcc-v2 does not yet.
reg_t
take(state_t &s)
{
    for (const reg_t r : expression_registers)
        if (!s.registers.owner.contains(r))
            return r;

    throw std::logic_error("v2: not yet: a spill");
}

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
value(state_t &s, const literal_t &l)
{
    const value_t v = fresh(s);
    constant(s, reg_of(s, v), l.value, 32);

    return v;
}

value_t
value(state_t &s, const zero_t &)
{
    const value_t v = fresh(s);
    constant(s, reg_of(s, v), 0, 32);

    return v;
}

// The load row, 6.3.2.1p2: movl -4(%rbp), %eax.  G8: one mov.
value_t
value(state_t &s, const load_t &l)
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

// The value goes to the convention's result register, 7.2, rcc's
// return_stmt(): it is held where its row took it, and moved when that
// is elsewhere.  A statement starts with no value held, so the first
// register a row takes, the lowest, is the result register.  Every
// return jumps to the one epilogue, the last statement of the body
// included: the jump to the next instruction is what the statement
// says, and removing it was the optional peephole pass's work in the
// PDP-11 compiler, which rcc has no counterpart of.
void
generate(state_t &s, const return_t &r)
{
    const value_t v = value(s, r.value);
    const reg_t from = reg_of(s, v);
    if (from != result_register)
        mov(s, from, result_register);

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
