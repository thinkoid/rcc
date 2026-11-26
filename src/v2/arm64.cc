// -*- mode: c++; -*-

// aarch64's text, declared in arm64.hh: assembly for clang and GNU as,
// as rcc writes it.  docs/aarch64.md is the design, with
// docs/backend.md where it says the model carries, and each function
// names the row or the part of the frame it writes.  The frame is the
// frame record and the locals' slots above it, chapter 5.

#include "arm64.hh"

#include "assembly.hh"

#include <format>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace rcc::arm64 {

namespace {

// The fifteen expression registers, chapter 4, in the order a new
// value takes them.
constexpr reg_t expression_registers[] = { x0, x1,  x2,  x3,  x4,  x5,  x6, x7,
                                           x9, x10, x11, x12, x13, x14, x15 };

// The convention's result register, chapter 3.
constexpr reg_t result_register = x0;

// The frame record, the caller's x29 and x30, at the bottom of the
// frame, chapter 5.1.
constexpr long frame_record = 16;

// The largest displacement off x29 a 4-byte access encodes, 2.3: 4095
// times 4, the scaled form.
constexpr long reach = 4095 * 4;

// operands

// A register by its name at a width in bits: the w name for 32 and
// below, the x name for 64.
std::string
reg(reg_t r, int width)
{
    return std::format("{}{}", width == 64 ? 'x' : 'w', static_cast< int >(r));
}

// The zero register at 32 bits, chapter 2.1.
constexpr std::string_view zero = "wzr";

// An immediate, as the assembler reads it.
std::string
imm(long long value)
{
    return std::format("#{}", value);
}

// A slot as the address operand of an access, 5.3: [x29, #d], which
// allocate() keeps in reach.
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

// A jump to the function's label n; no path reaches what follows it.
void
jump(state_t &s, int n)
{
    insn(s, "b", { label_name(s.function, n) });
    s.terminated = true;
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

} // namespace

// A register for a value: the lowest free one.  With every one held rcc
// spills the oldest, 6.2, which rcc-v2 does not yet.
reg_t
take(state_t &s)
{
    for (const reg_t r : expression_registers)
        if (!s.registers.owner.contains(r))
            return r;

    throw std::logic_error("v2: not yet: a spill");
}

// The next slot above the last, an int's 4 bytes, which keep every
// slot at an int's alignment; its displacement off x29 is recorded
// under the local's number.  G12: a slot is never shared and never
// reused inside the activation.  A slot past reach is reached through
// x16 in rcc, 5.3, which rcc-v2 does not yet; below it the frame is far
// from the target's limit.
void
allocate(state_t &s, slot_t where)
{
    const long displacement = s.frame;
    if (displacement > reach)
        throw std::logic_error(
            "v2: not yet: a slot beyond the reach of [x29, #d]");

    s.frame = displacement + 4;
    s.slots.emplace(where.id, static_cast< int >(displacement));
}

void
into(state_t &s, const literal_t &l, reg_t r)
{
    constant(s, r, l.value, 32);
}

void
into(state_t &s, const zero_t &, reg_t r)
{
    constant(s, r, 0, 32);
}

// The load row of 2.2: ldr w0, [x29, #16].  G8: one ldr.
void
into(state_t &s, const load_t &l, reg_t r)
{
    insn(s, "ldr", { reg(r, 32), slot_operand(slot_of(s, l.where)) });
}

// neg and mvn with r as their destination and their operand: neg w0, w0,
// mvn w0, w0.  G1: neg of INT_MIN is INT_MIN.
void
operate(state_t &s, unary_op_t op, reg_t r)
{
    const auto operand = reg(r, 32);
    insn(s, op == unary_op_t::neg ? "neg" : "mvn", { operand, operand });
}

// The store row of 2.2: str w0, [x29, #16].
void
store(state_t &s, reg_t r, slot_t where)
{
    insn(s, "str", { reg(r, 32), slot_operand(slot_of(s, where)) });
}

// str takes no immediate, but the zero register is an operand like any
// other: str wzr, [x29, #16].
void
initialize(state_t &s, slot_t where, const zero_t &)
{
    insn(s, "str", { zero, slot_operand(slot_of(s, where)) });
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

// A size by which sp moves, the frame's, as the immediates of sub and
// add, 5.2: 12 bits, or 12 bits shifted by 12.  The upper part first; a
// part that is zero is not written, so a size under 4096 bytes or a
// multiple of 4096 is one instruction.
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

// A definition: the body generated into the state's buffer, then
// written inside the frame, the frame record and the slots, whose size
// is known after the body, rounded up to 16.  The one epilogue is
// reached with x0 as it is, G5's some value, but for main, which
// returns 0 when it falls off its end, 5.1.2.3.4.  Every function has
// external linkage, so its symbol is global, .globl, chapter 10.
void
function(std::ostream &out, const function_definition_t &fd)
{
    state_t s { .function = fd.name, .frame = frame_record };
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

    epilogue(out, s, size);
    out << std::format("\t.cfi_endproc\n"
                       "\t.size\t{}, .-{}\n",
                       fd.name, fd.name);
}

} // namespace

// The functions the unit defines, in order, chapter 10.  The note
// section marks the stack not executable.
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

} // namespace rcc::arm64
