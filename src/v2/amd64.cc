// -*- mode: c++; -*-

// x86-64's text, declared in amd64.hh: AT&T syntax for the system
// assembler, as rcc writes it.  docs/backend.md is the design, and each
// function names the row or the part of the frame it writes.  The
// frame is rbp's and holds nothing else, chapter 5.

#include "amd64.hh"

#include "assembly.hh"

#include <cstdint>
#include <format>
#include <ostream>
#include <string>
#include <string_view>

namespace rcc::amd64 {

namespace {

// The convention's result register, 3.2.
constexpr reg_t result_register = rax;

// operands

// A register by its name at a width in bits.
std::string
reg(reg_t r, int width)
{
    static constexpr std::string_view names[][4] = {
        { "al", "ax", "eax", "rax" },
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

// writing

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

} // namespace

void
into(state_t &s, const literal_t &l, reg_t r)
{
    constant(s, r, l.value, 32);
}

// The value goes to the convention's result register, which the return
// names, 7.2.  Every return jumps to the one epilogue, the last
// statement of the body included: the jump to the next instruction is
// what the statement says, and removing it was the optional peephole
// pass's work in the PDP-11 compiler, which rcc has no counterpart of.
void
generate(state_t &s, const return_t &r)
{
    into(s, r.value, result_register);
    jump(s, s.ret);
}

namespace {

// the frame

// Chapter 5: rbp is the frame pointer in every function, pushed and
// pointed at the frame, each with its place in the unwind table.  No
// register is pushed below it and no slot is taken, so rsp does not
// move again.
void
prologue(std::ostream &out)
{
    out << "\tpushq\t%rbp\n"
           "\t.cfi_def_cfa_offset 16\n"
           "\t.cfi_offset %rbp, -16\n"
           "\tmovq\t%rsp, %rbp\n"
           "\t.cfi_def_cfa_register %rbp\n";
}

// The one epilogue, leave; the directive after it is the unwind
// table's, since the canonical frame address is rsp + 8 from leave to
// ret.
void
epilogue(std::ostream &out, const state_t &s)
{
    out << label_name(s.function, s.ret) << ":\n";

    out << "\tleave\n"
           "\t.cfi_def_cfa %rsp, 8\n"
           "\tret\n";
}

// A definition: the body generated into the state's buffer, then
// written inside the frame.  The one epilogue is reached with rax as it
// is, G5's some value, but for main, which returns 0 when it falls off
// its end, 5.1.2.3.4.  Every function has external linkage, so its
// symbol is global, .globl, chapter 10.
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

    prologue(out);

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
