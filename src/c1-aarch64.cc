// -*- mode: c++; -*-

// c1, the code generator for aarch64: the typed tree in, assembly text
// out, for clang and GNU as.  docs/aarch64.md is the design, with
// docs/backend.md where it says the model carries; this file implements
// its chapters 4 to 8 and 10, with c1-common.cc, c1-stack.hh and
// c1-statements.hh, and each function names the row it writes.  It
// writes the frame and straight-line code on integers, conditions and
// control, the switch among it, and calls; the address of an object,
// the indirection through a pointer, the use of an array and a string
// literal, pointer arithmetic and the subscript; the register variable:
// the save area, the binding, the borrowed read, plain assignment, the
// declaration, the register parameter, the held values of backend.md
// 6.7, the compound assignment, the ++ and the -- in place, and the
// access that steps a register pointer; a function's address as a value
// and the call through a pointer, chapters 8 and 10; and an object of
// static storage, reached by its name when the file defines it and
// through the GOT when it only declares it, chapter 10.  A size past a
// limit of the target throws logic_error("c1: not yet: ..."), and a
// tree the parser cannot make throws logic_error("c1: ...").

#include "c1.hh"
#include "c1-common.hh"

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <format>
#include <initializer_list>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
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

// The ten register-variable registers, chapter 4: the callee-saved set
// but the frame pointer, in the order the variables take them, x19
// first.  The state keeps no owner for them, since a read of a register
// variable is borrowed, never owned.
constexpr reg_t variable_registers[] = { x19, x20, x21, x22, x23,
                                         x24, x25, x26, x27, x28 };

// The parser's register_index is below the target's count, target.hh,
// and variable_register() takes the index into this table unchecked.
static_assert(static_cast< int >(std::size(variable_registers)) ==
              aarch64.register_variables);

// The convention's argument registers, in order, chapter 3.
constexpr reg_t argument_registers[] = { x0, x1, x2, x3, x4, x5, x6, x7 };

// The convention's result register, chapter 3.
constexpr reg_t result_register = x0;

// The alignment of a loop's head, as .p2align's power of two; 0 is
// none.  .p2align 4, 5 and 6 were measured on two cores and none was
// adopted (7.2, and call 7 of chapter 15; cost.md 4.2).
constexpr int loop_alignment = 0;

// The frame record, the caller's x29 and x30, at the bottom of the
// frame, chapter 5.1.
constexpr int frame_record = 16;

// The state of one file's generation.  Everything is reset per function,
// the label count included, but the string literals, which are numbered
// across the file.  The body is a buffer, since the prologue's frame
// size is known only after it.
struct state_t
{
    std::ostream &out;

    // the target's largest frame
    long frame_limit;

    // the functions and the objects the unit defines, whose addresses
    // are PC-relative
    std::unordered_set< const symbol_t * > defined { };

    // the string literals' bytes in the order met; .Ls<n> is the n-th
    std::vector< std::string > strings { };

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

    // the registers the save area holds, which the body's first slot
    // lies above
    int saved = 0;

    // the read of a compound assignment's register variable, which its
    // operation writes in place
    int in_place = -1;

    // the registers a take leaves for last, the one a consumer names
    // while the place of an assignment or of a step is generated (6.1)
    unsigned reserved = 0;

    std::string function { };
    int labels = 0;
    int ret = 0;

    // no path reaches what is written next: the last instruction written
    // is an unconditional jump, and no label is placed after it.  jump()
    // and the table's jump set it and place_label() clears it; an if
    // whose body ends so writes no jump over its else part, and main
    // writes no return of 0 (backend.md 7.2)
    bool terminated = false;

    // the statements around the one generated that a break jumps out
    // of, innermost last, and the label of each label the source
    // names, in the order met; and of each case and default label, by
    // the parser's number
    std::vector< jump_target_t > jump_targets { };
    std::unordered_map< std::string, int > user_labels { };
    std::unordered_map< int, int > case_labels { };
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

// the frame

// A size by which sp moves, the frame's or a call's outgoing area, as
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
// base, 2.3: a multiple of the size from 0 to 4095 times it, the scaled
// form, or -256 to 255, the unscaled one; the assembler picks, and both
// are written ldr.  A slot's displacement is positive and a multiple of
// its size, so the scaled form alone decides for it.
bool
in_reach(long long displacement, int size)
{
    const bool scaled = displacement >= 0 && displacement % size == 0 &&
                        displacement / size <= 4095;

    return scaled || (displacement >= -256 && displacement <= 255);
}

void constant(state_t &, reg_t, long long value, int width);

// A slot as the address operand of an access of size bytes, 5.3:
// [x29, #d] in reach, else through x16, the add of the displacement's
// upper 12 bits, shifted, and the access of its low 12.  An upper part
// of 16 MiB or more, which the add's immediate does not encode, is held
// in x16 by the constant row of 6.4 and added, add x16, x29, x16: only
// a stack parameter of a frame near its limit lies there, 5.2.  The add
// is written here, so the operand is formed just before its access,
// after anything that could spill, since a spill uses x16 too.
std::string
slot_operand(state_t &s, int displacement, int size)
{
    if (in_reach(displacement, size))
        return std::format("[x29, #{}]", displacement);

    const int upper = displacement & ~0xfff;
    const int lower = displacement & 0xfff;

    if (upper < 1 << 24)
        insn(s, "add", { "x16", "x29", imm(upper) });
    else {
        constant(s, x16, upper, 64);
        insn(s, "add", { "x16", "x29", "x16" });
    }

    if (lower == 0)
        return "[x16]";

    return std::format("[x16, #{}]", lower);
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

// The plain load of width bits, the store's mirror: ldrb, ldrh or ldr,
// the load of the unsigned type.  A copy stores the same width back, so
// what the load writes above the bytes is never read.
std::string_view
plain_load_of(int width)
{
    switch (width) {
    case 8: return "ldrb";
    case 16: return "ldrh";
    default: return "ldr";
    }
}

// A copy from memory to memory, which the machine has no instruction
// for, carries its bytes through x17, chapter 4: the plain load of the
// source's width into x17, then the store of the same width.  Each end's
// operand is formed just before its access, so either may go through
// x16, 5.3.  copy_in() is the load from a displacement off x29.
void
copy_in(state_t &s, int displacement, int width)
{
    const auto from = slot_operand(s, displacement, width / 8);
    insn(s, plain_load_of(width), { reg(x17, width), from });
}

void
copy_out(state_t &s, std::string_view to, int width)
{
    insn(s, store_of(width), { reg(x17, width), to });
}

// the register stack

// The three instructions the register stack writes, called by name from
// c1-stack.hh.  The store of a register to a spill slot, all 8 bytes,
// 6.2: str x19, [x29, #40].
void
store_spill(state_t &s, reg_t r, int displacement)
{
    const auto to = slot_operand(s, displacement, 8);
    insn(s, "str", { reg(r, 64), to });
}

// The reload of a register from a spill slot, all 8 bytes, 6.2.
void
reload_spill(state_t &s, reg_t r, int displacement)
{
    const auto from = slot_operand(s, displacement, 8);
    insn(s, "ldr", { reg(r, 64), from });
}

// The copy of a register into another at a width, the destination
// first, 2.2: mov w0, w19.
void
mov(state_t &s, reg_t from, reg_t to, int width)
{
    insn(s, "mov", { reg(to, width), reg(from, width) });
}

reg_t take(state_t &, unsigned avoid);
void cbranch(state_t &, const expr_t &, int label, bool sense);
void
arm_into(state_t &, const expr_t &, const into_t &, const type_t * = nullptr);

#include "c1-stack.hh"

// A register for a value: the lowest free one outside avoid and
// s.reserved, else the lowest free one outside avoid, or, when every one
// is held, the register of the oldest held value outside avoid, which is
// spilled first.  The reservation is an order of preference, not a pin.
reg_t
take(state_t &s, unsigned avoid)
{
    for (const unsigned out : { avoid | s.reserved, avoid })
        for (const reg_t r : expression_registers)
            if (s.owner[r] < 0 && !(out & bit(r)))
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
rvalue(state_t &, const expr_t &, into_t * = nullptr, unsigned wanted = 0);
void effect(state_t &, const expr_t &);

// register variables

// The register a row writes its result into, chapter 6.1: an owned
// value's own, reloaded outside avoid if it was spilled; for a borrowed
// read a fresh expression register outside avoid, which the value owns
// from here on, and no instruction.  No copy precedes the instruction
// that writes: the row names the variable's register as its source and
// the fresh one as its destination, sub w0, w19, w20.  The caller takes
// the source's register before this, since the value's register is the
// destination's after it.  A binary row frees R first, in 6.1's order,
// R freed and then the result placed: the instruction consumes R, so
// the fresh register is the lowest free one with R's among them, mul
// w0, w19, w0.  The read of a compound assignment's own variable,
// s.in_place, is the exception: its destination is the variable's
// register, add w19, w19, w0, chapter 4.  The copies that remain are
// three: preserve() and the first arm of a ?: copy a read, and a
// return's mov into x0 is the third copy chapter 4 keeps.  The value is
// valid from here on to valid, the width the instruction about to write
// it leaves, as for fresh().
reg_t
destination(state_t &s, value_t v, int valid, unsigned avoid = 0)
{
    if (!borrowed(s, v) || v.id == s.in_place) {
        const reg_t r = reg_of(s, v, avoid);
        s.values[v.id].valid = valid;
        return r;
    }

    check_movable(s, v.id);

    const reg_t r = take(s, avoid);

    auto &h = s.values[v.id];
    h.reg = r;
    h.valid = valid;
    h.variable = nullptr;
    s.owner[r] = v.id;

    return r;
}

// The register of wanted, the set of at most one expression register
// that the consumer of v's value names, x0 for a return's (6.1), where
// the instruction about to write v may write it: free, or v's own,
// since the instruction reads every source before it writes, add w0,
// w1, w0.  None for an empty set, for a register that holds another
// live value, which the instruction does not read, and for the read of
// a compound assignment's own variable, written in place.
std::optional< reg_t >
wanted_register(const state_t &s, value_t v, unsigned wanted)
{
    if (v.id >= 0 && v.id == s.in_place)
        return std::nullopt;

    for (const reg_t r : expression_registers)
        if (wanted & bit(r))
            return s.owner[r] < 0 || s.owner[r] == v.id ?
                       std::optional< reg_t >(r) :
                       std::nullopt;

    return std::nullopt;
}

// The register a consumer names, r, as the destination of the
// instruction about to write v, 6.1: v owns it from here on, valid to
// valid, and the register v held is free, since the instruction has
// read it.
reg_t
named_destination(state_t &s, value_t v, reg_t r, int valid)
{
    check_movable(s, v.id);

    auto &h = s.values[v.id];
    if (h.reg >= 0 && !h.variable)
        s.owner[h.reg] = -1;

    h.reg = r;
    h.valid = valid;
    h.variable = nullptr;
    s.owner[r] = v.id;

    return r;
}

// The register a row's last instruction writes, its result valid to
// valid: the register of a variable handed down, into, where that
// leaves the variable's representation, and v is then the variable's
// read, chapter 4, add w19, w0, w1; the register the consumer names,
// wanted, where the instruction may write it, add w0, w1, w0 for a
// return's (6.1); otherwise destination()'s.  The register is written
// after the row has read every operand, so no condition on them is
// needed.
reg_t
final_destination(
    state_t &s, value_t &v, int valid, into_t *into, unsigned wanted = 0)
{
    if (takes(into, valid)) {
        v = taken(s, *into, v);
        return variable_register(into->variable);
    }

    if (const auto r = wanted_register(s, v, wanted))
        return named_destination(s, v, *r, valid);

    return destination(s, v, valid);
}

// Before a register variable is written, each of its reads still held
// is copied into an expression register, so it keeps the value it read:
// G13, an operand's side effects complete before the next operand
// starts.  An access that steps its base writes the variable too, and
// the base, a read of it, is kept: it is the operand the access writes.
// The copies are taken outside avoid, the registers of that access.
void
preserve(state_t &s,
         const symbol_t *sym,
         value_t kept = { },
         unsigned avoid = 0)
{
    for (std::size_t id = 0; id < s.values.size(); ++id) {
        const auto &h = s.values[id];
        if (!h.live || h.variable != sym || static_cast< int >(id) == kept.id)
            continue;

        copy(s, { static_cast< int >(id) }, take(s, avoid));
    }
}

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
// written on the value's own register, sxtw xL, wL.  A borrowed read is
// extended into a fresh register, sxtw x0, w19, by destination(), 6.1,
// a register variable handed down by its cast is written by the
// extension, sxtw x19, w0, chapter 4, and so is the register the
// consumer names, wanted, sxtw x0, w1 for a return's (6.1).
value_t
extended(state_t &s,
         value_t v,
         const int_t &from,
         int to,
         into_t *into = nullptr,
         unsigned wanted = 0)
{
    if (s.values[v.id].valid >= to)
        return v;

    const auto [mnemonic, valid] = extension(from, std::max(32, to));
    const reg_t source = reg_of(s, v);
    const reg_t r = final_destination(s, v, valid, into, wanted);
    const int width = from.is_signed ? valid : 32;

    insn(s, mnemonic, { reg(r, width), reg(source, 32) });

    return v;
}

// The mov into a register variable's register from the register from
// that keeps the representation, chapter 4: the register holds the
// value extended to 32 bits by the variable's signedness, which for an
// unsigned written at 32 is its extension to 64, and a long or a pointer
// whole.  mov w20, w0 for an int or an unsigned, mov x20, x0 for a long
// or a pointer; for a narrow type the extension of 6.3, sxtb w19, w0,
// uxtb w19, w0, sxth or uxth, where the store row has its narrow store:
// G2's truncation and the representation in one instruction.
void
establish(state_t &s, const symbol_t *sym, reg_t from)
{
    const auto &t = integer(sym->type);
    const int width = register_width(sym);
    const std::string_view mnemonic =
        t.width < 32 ? extension(t, 32).first : "mov";

    insn(s, mnemonic, { reg(variable_register(sym), width), reg(from, width) });
}

// A value no row wrote into the register variable sym's register moved
// in, establish()'s mov, but for a read of sym itself, under conversions
// that write nothing too: the register holds the variable's
// representation already, which the mov would write again, so r = c ? x
// : r on a register r writes no mov w19, w19 on the second path, and r =
// r nothing at all (chapter 4).  A narrow variable's re-extension after
// an operation in place is a write of new bits, settle()'s, and stays.
void
move_in(state_t &s, const symbol_t *sym, value_t v)
{
    if (s.values[v.id].variable == sym)
        return;

    establish(s, sym, reg_of(s, v));
}

// addresses

// The size of the object a pointer points to, the stride of its
// arithmetic, 6.5.7p9.
long
stride(const type_t &pointer)
{
    return size_of(*as< pointer_t >(pointer).pointee);
}

// Whether a size is a scale of the machine's addressing form, 2.3: the
// sizes of an access, which the form's index is shifted by.
bool
is_scale(long size)
{
    return size == 1 || size == 2 || size == 4 || size == 8;
}

// Whether a size is a power of two above 16, which the extended
// operand of add and sub does not shift by and the shifted one does,
// 2.3: an index of such a size is read whole, add x0, x0, x1, lsl #5.
bool
is_wide_scale(long size)
{
    return std::has_single_bit(static_cast< unsigned long >(size)) && size > 16;
}

int
shift_of(long size)
{
    return std::countr_zero(static_cast< unsigned long >(size));
}

// How an operand form reads its index register, 2.3: the x register
// whole, or the w register extended to 64 bits by the form, signed or
// unsigned; either then shifted by the scale.
enum class extend_t { lsl, sxtw, uxtw };

// The index as the last operand of an access, or of add and sub, which
// take the same forms, 2.3: x2, lsl #2, or w2, sxtw #2, and for a size
// of 1 the forms without the shift, x2 and w2, sxtw.
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

// The integer operand of a subscript or of pointer arithmetic,
// generated, and how the form reads it: the paragraph "The index" under
// 6.3's table.  The parser widened it to 64 bits by a cast, and the form
// extends a 32-bit index itself, so the cast is looked through: its
// operand is generated and what is held decides.  Valid to 64 bits, a
// long or an unsigned written at 32, it is the x register, lsl; valid
// to 32, the w register, sxtw for a signed operand and uxtw for an
// unsigned one, as an unsigned call result is; narrower, as a char call
// result is, the cast's instruction extends it to 32 first, sxtb w2, w2,
// and the form does the rest.  An index that is no cast is 64 bits.
struct index_t
{
    value_t value { };
    extend_t extend = extend_t::lsl;
};

index_t
index_of(state_t &s, const expr_t &n)
{
    if (!is< cast_t >(n))
        return { .value = rvalue(s, n) };

    const auto &arg = *as< cast_t >(n).arg;
    const auto &from = integer(arg.type);

    auto v = rvalue(s, arg);
    if (s.values[v.id].valid < 32)
        v = extended(s, v, from, 32);

    if (s.values[v.id].valid >= 64)
        return { .value = v };

    return { .value = v,
             .extend = from.is_signed ? extend_t::sxtw : extend_t::uxtw };
}

// The step of 2.3's two forms that write their base: the register
// variable stepped, by how many bytes, -256 to 255, and whether before
// the access, the pre-indexed form, or after it, the post-indexed one.
// A null variable is no step.
struct step_t
{
    const symbol_t *variable = nullptr;
    long long bytes = 0;
    bool before = false;
};

// An lvalue's address as the operand of the instruction that reads or
// writes it, 2.3: a slot off x29; a held base, the pointer's value,
// [x0]; a subscript's, the base and a constant displacement, the index
// times the size, [x0, #8], or the base and a held index read by the
// form, [x0, w1, sxtw #2].  A global the unit defines is its label,
// reached through x16 as a far slot is, [x16, :lo12:n] after adrp x16,
// n, chapter 10; the label's displacement is the offset of an element
// by a constant, carried by both, adrp x16, n+12 and [x16,
// :lo12:n+12] (6.3).  A string literal's address is its label too.  A
// register pointer stepped by a constant the form encodes is its
// register as the base and the step, which the access makes, [x19], #1
// or [x19, #1]!.  A constant index a store writes after its right
// operand is the constant, not yet in a register, held_index()'s.  The
// access consumes the values.
struct address_t
{
    value_t base { };
    value_t index { };
    extend_t extend = extend_t::lsl;
    int shift = 0;
    int displacement = 0;
    std::string label { };
    step_t step { };
    std::optional< long long > constant { };
};

address_t
slot_address(int displacement)
{
    return { .displacement = displacement };
}

// The registers an address's values are in, for a take or a reload to
// stay clear of.
unsigned
registers_of(const state_t &s, const address_t &a)
{
    unsigned held = 0;
    for (const auto v : { a.base, a.index })
        if (v.id >= 0 && s.values[v.id].reg >= 0)
            held |= bit(static_cast< reg_t >(s.values[v.id].reg));

    return held;
}

// The operand of an access that steps its base, 2.3: the post-indexed
// form, [x19], #1, the access and then the step, or the pre-indexed one,
// [x19, #-4]!, the step and then the access.  The access writes the
// variable, so every other read of it still held is copied first, as
// before any write of it, chapter 4; the base is the variable's
// register, never a copy.
std::string
stepped_operand(state_t &s, const address_t &a, unsigned avoid)
{
    preserve(s, a.step.variable, a.base, avoid);

    const auto base = reg(reg_of(s, a.base, avoid), 64);
    if (a.step.before)
        return std::format("[{}, #{}]!", base, a.step.bytes);

    return std::format("[{}], #{}", base, a.step.bytes);
}

// The address as the operand of an access of width bits.  A slot is
// [x29, #d], or through x16 beyond reach, its add written here, 5.3.  A
// global's label is the far slot's mechanism with the symbol's page,
// adrp x16, n written here and [x16, :lo12:n] the operand, chapter 10:
// two instructions an access, and the object's alignment, at its
// definition, is what the scaled :lo12: form needs.  A displacement of
// the label is in both, so the page is the element's: adrp x16, n+12
// and [x16, :lo12:n+12].  A label with a base is the page plus a row's
// sum held in the base, row_address()'s, and the operand the label's
// offset in the page, [x0, :lo12:rows+2].  A spilled base or index is
// reloaded first, outside avoid and outside the other's register; the
// reload is complete before the access, so x16 holds nothing across
// it.  A step is stepped_operand()'s.
std::string
operand(state_t &s, const address_t &a, int width, unsigned avoid = 0)
{
    if (a.constant)
        throw std::logic_error("c1: a constant index not held");

    if (!a.label.empty() && a.base.id >= 0) {
        const auto base = reg(reg_of(s, a.base, avoid), 64);
        return std::format("[{}, :lo12:{}]", base,
                           symbol_plus(a.label, a.displacement));
    }

    if (!a.label.empty()) {
        const auto symbol = symbol_plus(a.label, a.displacement);
        insn(s, "adrp", { "x16", symbol });
        return std::format("[x16, :lo12:{}]", symbol);
    }

    if (a.base.id < 0)
        return slot_operand(s, a.displacement, width / 8);

    avoid |= registers_of(s, a);

    if (a.step.variable)
        return stepped_operand(s, a, avoid);

    std::string index;
    if (a.index.id >= 0) {
        const reg_t r = reg_of(s, a.index, avoid);
        avoid |= bit(r);
        index = ", " + index_operand(r, a.extend, a.shift);
    }

    const reg_t base = reg_of(s, a.base, avoid);

    if (a.displacement != 0)
        return std::format("[{}, #{}]", reg(base, 64), a.displacement);

    return std::format("[{}{}]", reg(base, 64), index);
}

// The address is consumed: its registers are free.
void
release(state_t &s, const address_t &a)
{
    release(s, a.base);
    release(s, a.index);
}

// The address of the first and of the last of two accesses through one,
// the load and the store of a compound assignment or of a ++ through a
// pointer.  The step is made once: by the pre-indexed form at the first
// and the plain base at the last, ldrb w0, [x19, #1]!; strb w0, [x19];
// or by the plain base at the first and the post-indexed form at the
// last, ldrb w0, [x19]; strb w0, [x19], #1.  An address with no step is
// both, 6.3.
address_t
first_access(address_t a)
{
    if (!a.step.before)
        a.step = { };

    return a;
}

address_t
last_access(address_t a)
{
    if (a.step.before)
        a.step = { };

    return a;
}

// A slot's address, x29 plus its displacement, into the register r,
// 5.3: add r, x29, #d, and where the add's 12 bits do not encode d the
// two adds, the upper part shifted first, and one when the lower part
// is zero.  The sum is formed in r, so x16 is not used unless it is r.
void
frame_address(state_t &s, const std::string &r, long displacement)
{
    const long upper = displacement & ~0xfffL;
    const long lower = displacement & 0xfffL;

    if (upper == 0) {
        insn(s, "add", { r, "x29", imm(lower) });
        return;
    }

    insn(s, "add", { r, "x29", imm(upper) });
    if (lower != 0)
        insn(s, "add", { r, r, imm(lower) });
}

// The address of a slot or of a label as a value, valid to 64, a name's
// plus a constant included (6.3).  A label is formed PC-relative in two
// instructions, adrp L, .Ls0 for its 4 KiB page and add L, L,
// :lo12:.Ls0 for the offset in it, chapter 3, each with the label's
// displacement, adrp L, table+12.  A slot is frame_address()'s, add L,
// x29, #d.  Any other dereference's address is never here:
// address_value() takes it as the pointer's value.  The row reads no
// operand, so a register variable handed down is L, written by the
// slot's add, add x19, x29, #16, and by the label's pair and the two
// adds alike, adrp x19, buf; add x19, x19, :lo12:buf (chapter 4).
value_t
address_of(state_t &s, const address_t &a, into_t *into = nullptr)
{
    if (a.base.id >= 0)
        throw std::logic_error("c1: a held base taken as a value");

    const bool direct = takes(into, 64);
    const auto v = direct ? value_t { } : fresh(s, 64);
    const auto r =
        reg(direct ? variable_register(into->variable) : reg_of(s, v), 64);

    if (!a.label.empty()) {
        const auto symbol = symbol_plus(a.label, a.displacement);
        insn(s, "adrp", { r, symbol });
        insn(s, "add", { r, r, std::format(":lo12:{}", symbol) });
    }
    else
        frame_address(s, r, a.displacement);

    return direct ? taken(s, *into) : v;
}

// The address of a function or of an object as a value, valid to 64,
// chapter 10.  One the unit defines is PC-relative, as a string
// literal's address is, adrp L, f and add L, L, :lo12:f.  One it only
// declares may be in a shared library, which a PC-relative reference
// does not reach from a position-independent executable, so its address
// is loaded from the GOT, adrp L, :got:f and ldr L, [L, :got_lo12:f],
// the one the dynamic linker wrote, the address the library's own code
// takes of it.  Either pair reads no operand, so a register variable
// handed down is L, adrp x19, :got:f; ldr x19, [x19, :got_lo12:f]
// (chapter 4).
value_t
symbol_address(state_t &s, const symbol_t &f, into_t *into = nullptr)
{
    const auto name = assembly_name(f);
    if (s.defined.contains(&f))
        return address_of(s, { .label = std::string(name) }, into);

    const bool direct = takes(into, 64);
    const auto v = direct ? value_t { } : fresh(s, 64);
    const auto r =
        reg(direct ? variable_register(into->variable) : reg_of(s, v), 64);

    insn(s, "adrp", { r, std::format(":got:{}", name) });
    insn(s, "ldr", { r, std::format("[{}, :got_lo12:{}]", r, name) });

    return direct ? taken(s, *into) : v;
}

// The address of a global as an access's operand, chapter 10.  One the
// unit defines is its label, which operand() reaches through x16.  One
// it only declares has its address loaded from the GOT into a register,
// and the access is the indirection through it, [x0], as *p's is with p
// loaded: each access the source writes loads the address once.
address_t
global_address(state_t &s, const symbol_t &g)
{
    if (s.defined.contains(&g))
        return { .label = std::string(assembly_name(g)) };

    return { .base = symbol_address(s, g) };
}

// The step a pointer expression makes, when 2.3's forms that write
// their base can make it: p++ and p-- after the access, ++p and --p
// before it, by the size of what p points to, 6.5.4.1p2, and p += c and
// p -= c before it, c a constant, by c times the size, G6; p a register
// variable, since the forms write a register, and the bytes in their
// reach, -256 to 255.  Anything else is no step.
step_t
step_of(const expr_t &p)
{
    step_t step;

    if (is< unary_t >(p) && is_step(as< unary_t >(p).op)) {
        const auto op = as< unary_t >(p).op;
        const bool up = op == unary_op_t::pre_inc || op == unary_op_t::post_inc;
        const long size = stride(p.type);

        step = { .variable = register_variable(*as< unary_t >(p).arg),
                 .bytes = up ? size : -size,
                 .before =
                     op == unary_op_t::pre_inc || op == unary_op_t::pre_dec };
    }
    else if (is< assign_t >(p)) {
        const auto &a = as< assign_t >(p);
        if (!a.op || !is< literal_t >(*a.rhs))
            return { };

        if (*a.op != binary_op_t::add && *a.op != binary_op_t::sub)
            return { };

        const auto bytes =
            scaled(as< literal_t >(*a.rhs).value, stride(p.type));
        const auto negated = static_cast< long long >(
            0ull - static_cast< unsigned long long >(bytes));

        step = { .variable = register_variable(*a.lhs),
                 .bytes = *a.op == binary_op_t::add ? bytes : negated,
                 .before = true };
    }

    if (step.bytes < -256 || step.bytes > 255)
        return { };

    return step;
}

// Whether a displacement off x29 is one 5.3 reaches: the add into x16
// of its upper 12 bits, shifted, and the access of its low 12, so 0 to
// 16 MiB less 1.  A slot's always is.
bool
in_frame_reach(long long displacement)
{
    return displacement >= 0 && displacement < (1LL << 24);
}

// The operand of an address by a name and a constant, Ritchie's &x+c,
// 6.3: the object's slot or label with the offset folded into its
// displacement, [x29, #d+12] for a[3] on an int a[8] at d, through x16
// beyond reach (5.3), and adrp x16, table+12 and [x16,
// :lo12:table+12] for table[3]; no instruction forms the object's
// address first.  None for a slot's displacement 5.3 does not reach and
// a label's offset outside its object, and the rows of *p and p + n
// reach the address.
std::optional< address_t >
folded(state_t &s, const expr_t &pointer)
{
    const auto named = named_offset(pointer, s.defined);
    if (!named)
        return std::nullopt;

    const auto &object = *named->object;
    const bool slot = is< name_t >(object) && !global_of(object);
    const auto displacement =
        wrapped_add(slot ? slot_of(s, object_of(object)) : 0, named->offset);
    if (slot ? !in_frame_reach(displacement) :
               !within_object(*named) || displacement > INT32_MAX)
        return std::nullopt;

    if (slot)
        return slot_address(static_cast< int >(displacement));

    const auto label = is< string_t >(object) ?
                           string_label(s.strings, as< string_t >(object)) :
                           std::string(assembly_name(*global_of(object)));

    return address_t { .displacement = static_cast< int >(displacement),
                       .label = label };
}

// The register variable an operand is generated into where an operation
// that reads nothing else of it writes the variable after, chapter 4: a
// copy of the one handed down, so that the operand's own row may write
// the variable's register and the instruction after it, offset()'s add
// or sub of a constant, or an operation's, still writes it, ldr x19,
// [x29, #24]; add x19, x19, #12 for q = p[1] on int (*p)[3].  An operand
// that no row writes, a register variable's read, the variable's own
// among them, and a call's result, which the call's row leaves in x0,
// leave the copy untaken, and the instruction writes the variable from
// the operand's register as before, add x19, x19, #4 for q = q + 1.
// None where none is handed down or a row has written the variable.
std::optional< into_t >
base_into(const into_t *into)
{
    if (!into || into->taken)
        return std::nullopt;

    return into_t { into->variable, false, into->as };
}

// The base and the displacement of an address by an lvalue and a
// constant, read off the tree before anything is generated: the pointer
// whose value is the base, and the displacement in bytes.
struct based_t
{
    const expr_t *pointer;
    long long displacement;
};

// The base and the displacement of an address by any lvalue that is no
// name's and a constant, 6.3: the lvalue's address plus the constant,
// down through each dereference whose own pointer is such an address,
// and through one plus a constant index of any size, p[1] on int
// (*p)[3]; the pointer at the bottom is the base.  p[1][2] is p and 20,
// other[1][2] on an extern int other[][3] the GOT's load and 20.  None
// for a name's address, folded()'s, and for any other pointer.
std::optional< based_t >
based(const expr_t &pointer,
      const std::unordered_set< const symbol_t * > &defined)
{
    const auto at = lvalue_offset(pointer);
    if (!at || named_offset(pointer, defined))
        return std::nullopt;

    const auto &lvalue = *at->lvalue;
    if (!is< unary_t >(lvalue) || as< unary_t >(lvalue).op != unary_op_t::deref)
        return based_t { at->address, at->offset };

    const auto &q = *as< unary_t >(lvalue).arg;
    if (const auto inner = based(q, defined))
        return based_t { inner->pointer,
                         wrapped_add(inner->displacement, at->offset) };

    if (!is< binary_t >(q) || as< binary_t >(q).op != binary_op_t::add)
        return based_t { at->address, at->offset };

    const auto &b = as< binary_t >(q);
    const bool left = is< pointer_t >(b.lhs->type);
    const auto &index = left ? *b.rhs : *b.lhs;
    if (!is< literal_t >(index))
        return based_t { at->address, at->offset };

    const auto d = scaled(as< literal_t >(index).value, stride(q.type));

    return based_t { left ? b.lhs.get() : b.rhs.get(),
                     wrapped_add(d, at->offset) };
}

// summed() and pointed() ask arithmetic_immediate(), which comes with
// the constant row of 6.4, after them.
bool arithmetic_immediate(long long value, int width);

// denoted() counts p + n's rows by product_length(), which comes with
// the constant row of 6.4, after it.
int product_length(long long);

// The address a pointer denotes before any instruction forms it, 6.3:
// the pointer under the casts between pointer types and the sums by a
// constant met on the way, their constants summed in bytes, each scaled
// by its own sum's stride, and at most one sum by a variable index, its
// index, its stride and whether it is written first; the walk stops at
// any other pointer, the base.  ((char *) p + 4 + i * 4) on an int *p
// is p, 4, and the index i * 4 at a stride of 1.  It steps through each
// row's decay of *q, or &*q, to q (6.5.4.2p3), p[i][2]'s row on int
// (*p)[3] being p + i, and p[1] + 2 being p and 20; top is the first
// sum it meets.  constants counts the sums by a constant, and written
// the instructions the row of p + n writes for them, one by one,
// product_length()'s.
struct denoted_t
{
    const expr_t *base;
    long long bytes = 0;
    int constants = 0;
    int written = 0;
    const expr_t *index = nullptr;
    long index_stride = 0;
    bool index_first = false;
    const expr_t *top = nullptr;
};

denoted_t
denoted(const expr_t &p)
{
    denoted_t d { .base = &p };

    for (;;) {
        const auto &e = *d.base;
        if (const auto at = lvalue_offset(e);
            at && at->address == &e && is< unary_t >(*at->lvalue) &&
            as< unary_t >(*at->lvalue).op == unary_op_t::deref) {
            d.base = as< unary_t >(*at->lvalue).arg.get();
            continue;
        }

        if (is< cast_t >(e) && is< pointer_t >(e.type) &&
            is< pointer_t >(as< cast_t >(e).arg->type)) {
            d.base = as< cast_t >(e).arg.get();
            continue;
        }

        if (!is< binary_t >(e) || !is< pointer_t >(e.type))
            return d;

        if (!d.top)
            d.top = &e;

        const auto &b = as< binary_t >(e);
        if (b.op != binary_op_t::add && b.op != binary_op_t::sub)
            return d;

        const bool left = is< pointer_t >(b.lhs->type);
        const auto &n = left ? *b.rhs : *b.lhs;
        d.base = left ? b.lhs.get() : b.rhs.get();

        if (is< literal_t >(n)) {
            const auto bytes = scaled(as< literal_t >(n).value, stride(e.type));
            const auto negated = static_cast< long long >(
                0ull - static_cast< unsigned long long >(bytes));
            d.bytes = wrapped_add(d.bytes,
                                  b.op == binary_op_t::add ? bytes : negated);
            ++d.constants;
            d.written += product_length(bytes);
        }
        else if (b.op == binary_op_t::add && !d.index) {
            d.index = &n;
            d.index_stride = stride(e.type);
            d.index_first = !left;
        }
        else
            return d;
    }
}

// An access through a row by a variable index, or by a constant only,
// read off the tree before anything is generated, 6.3: denoted()'s
// index, if any, its order and stride, and its base; the base's address
// as a slot's or a label's, folded()'s, or as based()'s pointer and
// displacement, the constant with the walk's bytes and the total with
// the access's column, and whether the index is the access's register
// offset.
struct row_access_t
{
    const expr_t *pointer;
    const expr_t *index;
    bool pointer_first;
    long stride;
    std::optional< address_t > slot { };
    std::optional< based_t > plan { };
    long long carried = 0;
    long long total = 0;
    bool label = false;
    bool register_offset = false;
};

// Whether a label's offset in the page is an access's displacement, 6.3
// and chapter 10: within the label's object, as folded() asks, and the
// label plus the offset a multiple of the access's size, which the
// scaled :lo12: form needs at link time: the object aligned to the size
// and the offset a multiple of it.  A char array aligns to 1, so *(int
// *) (bytes + 4) on a char bytes[16] is no such access.
bool
label_carries(named_offset_t named, long long total, int size)
{
    named.offset = total;

    const auto &object = *named.object;
    const auto &type =
        is< name_t >(object) ? as< name_t >(object).sym->type : object.type;

    return within_object(named) && total % size == 0 &&
           align_of(type) >= size && total <= INT32_MAX;
}

// Whether the operand of a name and a constant, folded()'s, takes an
// access of size bytes, 2.3: a slot where slot_operand() encodes its
// displacement, itself or its low 12 bits past x16's add, which an
// offset no multiple of the size reaches only in the unscaled 9 bits;
// a label where label_carries().
bool
named_access(const named_offset_t &named, const address_t &a, int size)
{
    if (!a.label.empty())
        return label_carries(named, a.displacement, size);

    return in_reach(a.displacement, size) ||
           in_reach(a.displacement & 0xfff, size);
}

// Whether an access of size bytes takes a row's constants past the sum,
// 6.3: a label's by label_carries(), any other where in_reach() holds
// after the sum.
bool
row_reaches(const row_access_t &row, const named_offset_t *named, int size)
{
    if (row.label)
        return label_carries(*named, row.total, size);

    return row.carried == 0 || in_reach(row.total, size);
}

// Whether a row's index is the access's register offset, 6.3: a row
// with one, with no constant at all and nothing evaluated between the
// address and the access, a product of no scale, or an index the
// access's form scales, at a size of 1 or the access's own.  Not at a
// power of two above 16, whose index the shifted operand of the add
// scales, is_wide_scale(), and the access's form does not.
bool
offset_index(const row_access_t &row, int size, bool nothing_between)
{
    return row.index && !row.slot && row.total == 0 && nothing_between &&
           !is_wide_scale(row.stride) &&
           (!is_scale(row.stride) || row.stride == 1 || row.stride == size);
}

// The access of size bytes at column into a row by a variable index, or
// by a constant only, where it takes the row's constants or its index
// into its own form, 6.3; none where it takes neither, p[i][2] keeping
// the sum and [x0, #8].  The row is denoted()'s walk: its base, its
// index wherever it stands, and the constants above the index and under
// it, through their casts, which join the base's own, folded()'s or
// based()'s, and the column: ((int (*)[3]) (c + 4))[i][2] on a char *c
// is the index's product and add after c's load and ldr w0, [x0, #12],
// p[i].a[3] on a member int a[8] at 32 of records of 168 bytes the
// product's add and ldr w0, [x0, #44], and ((int *) (p + 1))[3] on a
// char *p ldr w0, [x0, #13].  On a label the total is the page's
// offset, tab[i].m at 8 being adrp x0, tab+8, the product's add and ldr
// w0, [x0, :lo12:tab+8], where label_carries() holds for the total; a
// total out of reach, or a label's it refuses, keeps the rows as
// before.  With no index the constants are the column's displacement,
// p->a[3] being ldr w0, [x0, #44] after p's load; none with no constant
// under the row, and none on a name, whose operand is folded()'s.
std::optional< row_access_t >
row_access(state_t &s,
           const expr_t &row,
           long long column,
           int size,
           bool nothing_between)
{
    const auto at = denoted(row);

    row_access_t a { .pointer = at.base,
                     .index = at.index,
                     .pointer_first = !at.index_first,
                     .stride = at.index_stride };

    const auto named = named_offset(*a.pointer, s.defined);
    if (!a.index && (at.bytes == 0 || named))
        return std::nullopt;

    a.slot = named ? folded(s, *a.pointer) : std::nullopt;
    if (named && !a.slot)
        return std::nullopt;

    a.plan = named ? std::nullopt : based(*a.pointer, s.defined);
    a.carried = wrapped_add(at.bytes, a.slot ? a.slot->displacement :
                                      a.plan ? a.plan->displacement :
                                               0);
    a.total = wrapped_add(a.carried, column);
    a.label = a.slot && !a.slot->label.empty();
    if (!row_reaches(a, named ? &*named : nullptr, size))
        return std::nullopt;

    a.register_offset = offset_index(a, size, nothing_between);
    if (!a.slot && a.carried == 0 && !a.register_offset)
        return std::nullopt;

    return a;
}

// row_address() needs right_t, displacement() and offset(), which come
// after pointed(), its caller and summed()'s.
address_t row_address(state_t &, const row_access_t &);

// An access's offset in bytes that no displacement of the access holds,
// 6.3: held in a register by the constant row of 6.4 after the base and
// read as the access's register offset, unscaled, ldrb w0, [x0, x1]
// after mov x1, #2147483648, which any access size takes.
address_t
held_offset(state_t &s, value_t base, long long bytes)
{
    const auto held = fresh(s, 64, registers_of(s, { .base = base }));
    constant(s, reg_of(s, held), bytes, 64);

    return address_t { .base = base, .index = held };
}

// The operand of an address by any other lvalue and a constant, 6.3:
// based()'s base, its pointer's value, with the displacement, when the
// access encodes it (2.3), [x0, #20] for p[1][2] on int (*p)[3], and no
// instruction forms the intermediate address.  Only the base is
// evaluated.  A row by a variable index is a base the row of p + n
// forms, add x0, x0, w1, sxtw #3, since no form holds an index with a
// displacement, and for an access the row's own constant goes past the
// sum, row_address()'s.  For the address as a value, as_value, which
// joined() leaves one constant and a name's row under an index, the add
// of the base and the displacement is to encode it instead (2.4), add
// x0, x0, #-4 after the sum for p[i] - 1 on int (*p)[3].  For an access
// with nothing evaluated between it and its address, a displacement no
// immediate holds is held_offset()'s; none for any other out of reach,
// and the rows of *p and p + n reach the address.  A register variable
// handed down, with the address as a value, is the destination of the
// base's row: the variable itself where the displacement is 0, and
// base_into()'s copy under any other, so that q = &xa[3] on an array
// the unit only declares is adrp x19, :got:xa; ldr x19, [x19,
// :got_lo12:xa], and the add of the displacement after it writes the
// variable (chapter 4).  A base that is a sum by a variable index takes
// it too: its add is its own instruction whichever register it writes,
// since no form holds an index with a displacement.
std::optional< address_t >
summed(state_t &s,
       const expr_t &pointer,
       bool as_value = false,
       into_t *into = nullptr,
       bool nothing_between = true)
{
    const auto plan = based(pointer, s.defined);
    if (!plan)
        return std::nullopt;

    // p[1][i][0] on int (*p)[3][3]: the sum, then [x0, #36]
    if (!as_value) {
        const int size = static_cast< int >(stride(pointer.type));
        if (const auto row = row_access(s, *plan->pointer, plan->displacement,
                                        size, nothing_between))
            return row_address(s, *row);
    }

    const bool reached = as_value ?
                             arithmetic_immediate(plan->displacement, 64) :
                             in_reach(plan->displacement,
                                      static_cast< int >(stride(pointer.type)));
    // other[1][0] on char (*other)[2147483648]: the row's offset held,
    // [x0, x1], where the add would hold it too
    if (!reached && !as_value && nothing_between &&
        !arithmetic_immediate(plan->displacement, 64))
        return held_offset(s, rvalue(s, *plan->pointer), plan->displacement);

    if (!reached)
        return std::nullopt;

    auto onto = plan->displacement != 0 ? base_into(into) : std::nullopt;
    into_t *to = plan->displacement == 0 ? into : onto ? &*onto : nullptr;

    return address_t { .base = rvalue(s, *plan->pointer, to),
                       .displacement = static_cast< int >(plan->displacement) };
}

// index_across() needs right_t and offset(), and calls_within() sits
// with the conditional's rows: both come after pointed(), their caller.
address_t index_across(
    state_t &, value_t, index_t, int shift, bool pointer_first, const expr_t *);
bool calls_within(const expr_t &);

// The index form of an access through a sum, sum_address()'s, 6.3:
// the base and the index generated in the order written (G13), then
// held across between by index_across().  p[i], *(p + i): the pointer
// is written first, so the base is generated first and the index after
// it, [x0, w1, sxtw #2].  i[p], *(i + p): the integer is written first,
// so the index is generated first; the operand is the same, [x1, w0,
// sxtw #2].
address_t
indexed_address(state_t &s,
                const expr_t &pointer,
                const expr_t &n,
                long size,
                bool pointer_first,
                const expr_t *between)
{
    if (pointer_first) {
        const auto base = rvalue(s, pointer);
        const auto index = index_of(s, n);
        return index_across(s, base, index, shift_of(size), true, between);
    }

    const auto index = index_of(s, n);
    const auto base = rvalue(s, pointer);

    return index_across(s, base, index, shift_of(size), false, between);
}

// The address of an access through a sum of a pointer and an integer,
// pointed()'s, 6.3: the shape of a subscript (6.5.3.2p2) is the
// machine's addressing form, the pointer the base, the integer the
// index, each generated in the order written (G13), the index by
// index_of(), and the scale the size of the pointee, a scalar of 1, 2,
// 4 or 8 bytes.  A constant index is the displacement c * s when the
// access encodes it, 2.3; any other is an index like a variable one,
// held whole, lsl.  A cast between pointer types directly over
// denoted()'s top sum is looked through, p the cast and sum the sum: a
// constant's bytes at any stride are the access's displacement, *(int
// *) (c + 1) on a char *c being ldrsw x0, [x0, #1] and p[1].x at offset
// 0 of a record of S bytes [x0, #S]; the index form under the cast only
// at a stride of 1 or the access's size, where its bytes are the
// offset, unscaled or scaled by the access's size, *(int *) (c + 4097)
// being mov x1, #4097; ldrsw x0, [x0, x1], where it saves the add and
// nothing is evaluated between the address and the access.  The
// constants under the pointer, a variable index among them, are the
// row's plan's, row_access()'s.
address_t
sum_address(state_t &s,
            const expr_t &p,
            const expr_t &sum,
            const expr_t *between)
{
    // p[n], n[p], *(p + n) and *(n + p), which are one tree, under a
    // cast or not.  Either operand may be the pointer; the other is n,
    // scaled by the sum's stride.
    const auto &b = as< binary_t >(sum);
    const bool left = is< pointer_t >(b.lhs->type);
    const auto &pointer = left ? *b.lhs : *b.rhs;
    const auto &n = left ? *b.rhs : *b.lhs;
    const long size = stride(sum.type);
    const int access = static_cast< int >(stride(p.type));

    // p[3], 3[p], p[-1], *(int *) (c + 1): a constant n whose product
    // with the size the access encodes is the displacement, [x0, #12],
    // and n generates nothing.  One it does not encode, p[5000], goes on
    // as an index.
    const auto d = is< literal_t >(n) ? scaled(as< literal_t >(n).value, size) :
                                        std::optional< long long > { };

    // ((int *) (p + 1))[3] on a char *p: the constants under the pointer
    // and n's are one displacement, [x0, #13]; tab[i].m on a global
    // array of records the label's row, ldr w0, [x0, :lo12:tab+8]
    if (d)
        if (const auto row = row_access(s, pointer, *d, access, !between))
            return row_address(s, *row);

    if (d && in_reach(*d, access))
        return { .base = rvalue(s, pointer),
                 .displacement = static_cast< int >(*d) };

    // p->a[i] += f(): a place held across a call is one register, its
    // base and its index summed, and the constants under the pointer stay
    // the access's displacement, the row's plan's, [x2, #32] after it
    if (!d && between && calls_within(*between))
        if (const auto row = row_access(s, sum, 0, access, false))
            return row_address(s, *row);

    // a record's element for a copy, whose index the add of 6.6 takes
    // shifted by at most 4, add x0, x0, w1, sxtw #4 on records of 16
    // bytes; at any other size, p[i] on records of 12 or 32 bytes, the
    // row of p + n, the sum in one register
    if (!std::has_single_bit(static_cast< unsigned long >(size)) || size > 16)
        return { .base = rvalue(s, p) };

    // under a cast, the index form only where it saves the add: not for
    // a constant the add encodes, *(int *) (c + 257), and not where
    // anything is evaluated between the address and the access, since it
    // holds two registers; and only at a stride the form scales by, 1 or
    // the access's size
    if (&sum != &p && (between || (d && arithmetic_immediate(*d, 64)) ||
                       (size != 1 && size != access)))
        return { .base = rvalue(s, p) };

    // p[5000] = f(): a constant index reads nothing, so it is written
    // after the store's right operand, held_index()'s, and no register
    // holds it across that operand
    if (d && between)
        return { .base = rvalue(s, pointer),
                 .shift = shift_of(size),
                 .constant = as< literal_t >(n).value };

    return indexed_address(s, pointer, n, size, left, between);
}

// The address an access through a dereference designates, 6.5.4.2p4,
// for a load, a fetch or a store: the pointer's value as the base,
// [x0], whatever computed it and however it is aligned (G8, G11).  A
// register pointer stepped by a constant the form encodes is the first
// case: its register is the base, a borrowed read, and the access makes
// the step, 6.3.  The step then moves from here to the access, past
// between, the operand the caller generates between the two, a store's
// right operand; so the case is not taken when between names the
// variable, and the step is written here as before, G13.  A name's
// address plus a constant is the second: the one operand of folded(),
// where the access encodes it, named_access()'s, and nothing evaluated.
// Any other lvalue's address plus a constant is the third: summed()'s
// base, with the constant in the displacement.  A sum of a pointer and
// an integer, under a cast or not, is sum_address()'s; any other
// pointer is computed into a register and is the base.  The pointer is
// evaluated here, before anything the access goes with.
address_t
pointed(state_t &s, const expr_t &p, const expr_t *between = nullptr)
{
    // *p++, *p--, *++p, *--p, *(p += 4) and *(p -= 4) on a register
    // pointer, each step's bytes in -256 to 255: [x19], #1 for p++ and
    // p--, [x19, #1]! for the others.  Not for *p++ = *p, whose right
    // operand names p.
    const auto step = step_of(p);
    if (step.variable && !(between && names(*between, step.variable)))
        return { .base = read(s, step.variable), .step = step };

    // a[3], table[3], m[1][2], *(a + 3), *a and *&x on an object the
    // slot or the label reaches: [x29, #28], [x16, :lo12:table+12]
    if (const auto a = folded(s, p);
        a && named_access(*named_offset(p, s.defined), *a,
                          static_cast< int >(stride(p.type))))
        return *a;

    // p[1][2] on int (*p)[3], other[1][2] through the GOT: [x0, #20]
    if (const auto a = summed(s, p, false, nullptr, !between))
        return *a;

    // *p, and *E for any E that is no sum: *f(), *(char *) q, *p++ on
    // a pointer in a slot, *p++ = *p, and a difference, *(p - 1).  E's
    // value is the base, [x0].
    const auto at = denoted(p);
    const bool under = is< cast_t >(p) && as< cast_t >(p).arg.get() == at.top &&
                       !named_offset(*at.top, s.defined);
    const auto &sum = under ? *at.top : p;
    if (!is< binary_t >(sum) || as< binary_t >(sum).op != binary_op_t::add)
        return { .base = rvalue(s, p) };

    return sum_address(s, p, sum, between);
}

// The address of an lvalue: a local's or a parameter's slot, a global's
// label or its address from the GOT, a string literal's label, or what a
// dereference designates.  It serves loads and stores alike, so the
// addressing row is written once.  between is the operand the caller
// generates after the address and before its access, an assignment's
// right operand, or null, for pointed().
address_t
address(state_t &s, const expr_t &e, const expr_t *between = nullptr)
{
    if (is< string_t >(e))
        return { .label = string_label(s.strings, as< string_t >(e)) };

    if (is< unary_t >(e) && as< unary_t >(e).op == unary_op_t::deref)
        return pointed(s, *as< unary_t >(e).arg, between);

    if (const auto *g = global_of(e))
        return global_address(s, *g);

    return slot_address(slot_of(s, object_of(e)));
}

// The address of the place of an assignment or of a step, generated
// with the registers in left taken last, the one the consumer of the
// value names (6.1): the stored value, the operation of x op= e and the
// old value of x++ then take it, ldr x0, [x29, #32]; str x0, [x1, w2,
// sxtw #3] for return p[i] = x on a long *p.  The reservation is an
// order of preference, not a pin: a call's result within the place is
// in x0 by the convention, and the name then stops at the instruction.
address_t
address_avoiding(state_t &s,
                 const expr_t &e,
                 unsigned left,
                 const expr_t *between = nullptr)
{
    const unsigned outer = std::exchange(s.reserved, s.reserved | left);
    const auto a = address(s, e, between);
    s.reserved = outer;

    return a;
}

// The address of an lvalue as a value: &x, and an array's decay,
// 6.3.2.1p3.  The address of *E is taken and no access made, 6.5.4.2p3:
// "neither that operator nor the & operator is evaluated".  A bare base
// is its own value and no instruction: &*p is p, and so is &p[0], whose
// access is [x0] as *p's is.  Any other subscript's address is the row
// of p + n, the machine having no lea, so &p[i] and &p[3] are the add;
// a row of an array of arrays, m[i], decays the same way, and m[0] to
// m.  A global's address is the row of chapter 10, PC-relative or from
// the GOT.  Anything else is a slot or a string literal, whose address
// address_of() forms.  A register variable handed down goes to the row
// that forms the address, chapter 4, and so does the register the
// consumer names, wanted, to the row of p + n for &p[i], add x0, x1, w0,
// sxtw #2 for a return's (6.1).
value_t
address_value(state_t &s,
              const expr_t &e,
              into_t *into = nullptr,
              unsigned wanted = 0)
{
    if (const auto *g = global_of(e))
        return symbol_address(s, *g, into);

    if (!is< unary_t >(e) || as< unary_t >(e).op != unary_op_t::deref)
        return address_of(s, address(s, e), into);

    const auto &p = *as< unary_t >(e).arg;
    if (const auto *base = bare_base(p))
        return rvalue(s, *base, into, wanted);

    return rvalue(s, p, into, wanted);
}

// A function designator decayed to a pointer, 6.3.2.1p4: a function's
// name is its address, and *p through a pointer to a function is p's
// value, the * written as no instruction.  A register variable handed
// down goes to the row of either (chapter 4), and the register the
// consumer names, wanted, to p's (6.1).
value_t
function_pointer(state_t &s,
                 const expr_t &designator,
                 into_t *into = nullptr,
                 unsigned wanted = 0)
{
    if (is< name_t >(designator))
        return symbol_address(s, *as< name_t >(designator).sym, into);

    return rvalue(s, *as< unary_t >(designator).arg, into, wanted);
}

// The load row, 6.3.2.1p2: the value at an address into a register at
// a width of 32 or 64, a char or a short by the extending load of its
// signedness, ldrsb w0, [x29, #16].  G8, G10: the load of 2.2 at the
// lvalue's width and extension, whatever computed the address.  A held
// base and index are consumed first, so the value may take one of their
// registers, ldr w0, [x0], ldr w0, [x0, w1, sxtw #2].  For a slot the
// register is taken first, since the take may spill through x16, and
// the slot's operand is formed after it.  A register variable handed
// down is the load's register where that leaves its representation, ldr
// w19, [x29, #24], chapter 4, but for an access that steps the
// variable's own register: a load whose data register is its base,
// stepped, is unpredictable (2.3).
value_t
load(state_t &s,
     const address_t &a,
     const int_t &t,
     int width,
     into_t *into = nullptr)
{
    const auto [mnemonic, to] = load_of(t, width);
    const int valid = written(to, t.is_signed);

    if (takes(into, valid) && a.step.variable != into->variable) {
        const auto from = operand(s, a, t.width);
        release(s, a);

        insn(s, mnemonic, { reg(variable_register(into->variable), to), from });

        return taken(s, *into);
    }

    if (a.base.id >= 0) {
        const auto from = operand(s, a, t.width);
        release(s, a);

        const auto v = fresh(s, valid);
        insn(s, mnemonic, { reg(reg_of(s, v), to), from });

        return v;
    }

    const auto v = fresh(s, valid);
    const reg_t r = reg_of(s, v);

    const auto from = operand(s, a, t.width);
    insn(s, mnemonic, { reg(r, to), from });

    return v;
}

// The load row of a read-modify-write, a compound assignment or a ++
// or --: the address stays held for the store through it, since E1 is
// evaluated once, 6.5.17.3p4.  The value's register is taken outside
// the address's, and the operand formed after the take.  Handed a
// register variable, the load writes the variable's register where
// that leaves its representation, as load() does, and the value is the
// variable's read (chapter 4); not where the address reads the
// variable's register, r = a[r]++, since the store after the load goes
// through the same address.
value_t
fetch(state_t &s,
      const address_t &a,
      const int_t &t,
      int width,
      into_t *into = nullptr)
{
    const auto [mnemonic, to] = load_of(t, width);
    const int valid = written(to, t.is_signed);

    if (takes(into, valid) && a.step.variable != into->variable &&
        !(registers_of(s, a) & bit(variable_register(into->variable)))) {
        const auto from = operand(s, a, t.width);
        insn(s, mnemonic, { reg(variable_register(into->variable), to), from });
        return taken(s, *into);
    }

    const auto v = fresh(s, valid, registers_of(s, a));
    const reg_t r = reg_of(s, v);

    const auto from = operand(s, a, t.width, bit(r));
    insn(s, mnemonic, { reg(r, to), from });

    return v;
}

// The store row through an address, at the lvalue's width, strb w0,
// [x29, #16] or str x1, [x0].  The address stays held, for the caller
// to consume.
void
store(state_t &s, value_t v, const address_t &a, int width)
{
    const reg_t r = reg_of(s, v, registers_of(s, a));

    const auto to = operand(s, a, width, bit(r));
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

// The instructions constant() writes for value at width: one where a
// mov encodes it, and otherwise one for each chunk that differs from
// the start, the lesser of those over zeros and those over ones.
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

// A constant that must be held, in a register taken for it.  A
// constant narrower than int is written as its promotion, so the
// register holds it extended.  Handed a register variable, the
// constant is the constant row into its register, mov w20, #5, as an
// operation's left operand that left_into() hands it (chapter 4).
value_t
literal(state_t &s, const literal_t &l, const type_t &t, into_t *into = nullptr)
{
    const auto &type = integer(t);
    const int width = std::max(32, type.width);
    const auto value =
        type.width < 32 ? widened(l.value, type) : normalized(l.value, width);
    const int valid = written(width, type.is_signed);

    if (takes(into, valid)) {
        constant(s, variable_register(into->variable), value, width);
        return taken(s, *into);
    }

    const auto v = fresh(s, valid);
    constant(s, reg_of(s, v), value, width);

    return v;
}

// A constant assigned to a register variable, or initializing it, is
// the constant row of 6.4 into its register, mov w20, #0, chapter 4.
// The parser folded the constant to the variable's type, so a narrow
// one is in range and is written as its promotion, which is the
// register's representation.  False when e is not a constant.
bool
constant_into(state_t &s, const symbol_t *sym, const expr_t &e)
{
    if (!is< literal_t >(e))
        return false;

    const auto &t = integer(sym->type);
    const int width = register_width(sym);
    const auto value = t.width < 32 ?
                           widened(as< literal_t >(e).value, t) :
                           normalized(as< literal_t >(e).value, width);

    constant(s, variable_register(sym), value, width);

    return true;
}

// An address's constant index written after the store's right
// operand, 6.3: the constant row of 6.4 into a register outside the
// address's and avoid, the operand's, read by the form, bl f; mov x1,
// #5000; str w0, [x2, x1, lsl #2].  Any other address is as it is.
address_t
held_index(state_t &s, address_t a, unsigned avoid)
{
    if (!a.constant)
        return a;

    const auto held = fresh(s, 64, avoid | registers_of(s, a));
    constant(s, reg_of(s, held), *a.constant, 64);
    a.index = held;
    a.constant.reset();

    return a;
}

// The store row of an initializer, chapter 6.6: e written into the
// object at to, at the object's width t, its value not wanted.  A zero
// constant is the store of the zero register, str wzr, [x29, #16], and
// no register is held; anything else, any other constant included, is
// its value in a register and the store, since str takes no immediate.
// to is a slot, which has no evaluation, or an address the caller
// evaluated before e, so G13 holds; the caller consumes it, and a
// constant index held after e, held_index()'s, is released here.
void
initialize(state_t &s, const expr_t &e, const address_t &to, const int_t &t)
{
    if (is< literal_t >(e) &&
        normalized(as< literal_t >(e).value, t.width) == 0) {
        const auto at = held_index(s, to, 0);
        const auto operand_to = operand(s, at, t.width);
        insn(s, store_of(t.width), { zero(t.width), operand_to });
        if (to.constant)
            release(s, at.index);
        return;
    }

    const auto v = rvalue(s, e);
    const auto at = held_index(s, to, bit(reg_of(s, v)));
    store(s, v, at, t.width);
    if (to.constant)
        release(s, at.index);
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
// function for both.  The n of pointer arithmetic is read by add and
// sub through the form's extension and shift, or multiplied by the
// factor first, 6.3.
struct right_t
{
    std::string immediate { };
    value_t value { };
    int width = 0;
    extend_t extend = extend_t::lsl;
    int shift = 0;
    long factor = 1;
};

// The right operand of op in an operation of width bits, as its row
// takes it, 6.4.  A constant is an immediate where the instruction
// encodes it, and then nothing is evaluated: add, sub and cmp's 12
// bits, the bitmask of and, orr and eor, a shift's count masked to the
// width, which is what the instruction itself does to a count in a
// register (G3).  mul, sdiv and udiv take no immediate, and their
// constant is held, as is one its instruction does not encode.
right_t
right_operand(state_t &s, binary_op_t op, const expr_t &rhs, int width)
{
    const bool constant = is< literal_t >(rhs);
    const auto value = constant ? as< literal_t >(rhs).value : 0;

    if (constant && is_shift(op))
        return { .immediate = imm(value & (width - 1)) };

    // add, sub and cmp are one class of immediate, 2.4
    const bool twelve_bits =
        op == binary_op_t::add || op == binary_op_t::sub || is_comparison(op);
    if (constant && twelve_bits && arithmetic_immediate(value, width))
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
// and freed, then msub, L - N * R, sdiv N, L, R; msub L, N, R, L.  msub
// is the last to read R, so R is freed before the result is placed, as
// in every binary row.  A borrowed read as L then writes into the fresh
// register of destination(), which is R's own when R held one, sdiv w1,
// w19, w0; msub w0, w1, w0, w19, and the quotient's when R is a
// register variable too, sdiv w0, w19, w20; msub w0, w0, w20, w19.  In
// place the destination is L: sdiv w0, w19, w20; msub w19, w0, w20,
// w19.  A register variable handed down is the last instruction's
// destination, msub w19, w2, w1, w0, as in every row (chapter 4), and
// so is the register the consumer names, wanted, msub w0, w2, w0, w1
// for a return's (6.1): the sdiv writes N alone, taken outside L and R,
// so it overwrites no source of the msub, and the msub reads N, R and L
// before it writes.
value_t
divide(state_t &s,
       binary_op_t op,
       value_t l,
       value_t r,
       const int_t &t,
       into_t *into = nullptr,
       unsigned wanted = 0)
{
    const int width = t.width;
    const int valid = written(width, t.is_signed);
    const auto mnemonic = t.is_signed ? "sdiv" : "udiv";

    const auto [l_reg, r_reg] = operands(s, l, r);
    const auto left = reg(l_reg, width);
    const auto right = reg(r_reg, width);

    if (op == binary_op_t::div) {
        release(s, r);

        const auto result =
            reg(final_destination(s, l, valid, into, wanted), width);
        insn(s, mnemonic, { result, left, right });
    }
    else {
        const reg_t n = take(s, bit(l_reg) | bit(r_reg));

        const auto quotient = reg(n, width);
        insn(s, mnemonic, { quotient, left, right });

        release(s, r);

        const auto result =
            reg(final_destination(s, l, valid, into, wanted), width);
        insn(s, "msub", { result, quotient, right, left });
    }

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
shift(state_t &s,
      binary_op_t op,
      value_t l,
      const right_t &r,
      const int_t &t,
      into_t *into = nullptr,
      unsigned wanted = 0)
{
    const int width = t.width;
    const int valid = written(width, t.is_signed);
    const auto mnemonic = shift_mnemonic(op, t.is_signed);

    if (!r.immediate.empty()) {
        const auto left = reg(reg_of(s, l), width);
        const auto to =
            reg(final_destination(s, l, valid, into, wanted), width);
        insn(s, mnemonic, { to, left, r.immediate });
        return l;
    }

    const auto [l_reg, r_reg] = operands(s, l, r.value);
    release(s, r.value);

    const auto to = reg(final_destination(s, l, valid, into, wanted), width);
    insn(s, mnemonic, { to, reg(l_reg, width), reg(r_reg, width) });

    return l;
}

// An arithmetic, bitwise or shift operator on the held left value and
// the generated right operand, in type t, chapter 6.1: op L, L, R, the
// result in L, or op L, L, #c for an immediate.  A borrowed read as L
// writes into a fresh register, op N, L, R, by destination(), unless it
// is the variable a compound assignment writes in place, add w19, w19,
// w0; as R it is the operand as it is.  A register variable handed down
// is the destination, add w19, w0, w1 (chapter 4), and so is the
// register the consumer names, wanted, add w0, w1, w0 for a return's
// (6.1).  G1: add, sub, mul and lsl wrap; mul is the same instruction
// for unsigned, the low bits agree.
value_t
arithmetic(state_t &s,
           binary_op_t op,
           value_t l,
           const right_t &r,
           const type_t &t,
           into_t *into = nullptr,
           unsigned wanted = 0)
{
    const int width = operation_width(t);
    const int valid = written(width, integer(t).is_signed);

    if (op == binary_op_t::div || op == binary_op_t::rem)
        return divide(s, op, l, r.value, integer(t), into, wanted);

    if (is_shift(op))
        return shift(s, op, l, r, integer(t), into, wanted);

    const auto mnemonic = arithmetic_mnemonic(op);

    if (!r.immediate.empty()) {
        const auto left = reg(reg_of(s, l), width);
        const auto to =
            reg(final_destination(s, l, valid, into, wanted), width);
        insn(s, mnemonic, { to, left, r.immediate });
        return l;
    }

    const auto [l_reg, r_reg] = operands(s, l, r.value);
    release(s, r.value);

    const auto to = reg(final_destination(s, l, valid, into, wanted), width);
    insn(s, mnemonic, { to, reg(l_reg, width), reg(r_reg, width) });

    return l;
}

// The product c * s of a constant n and the size, as the row of p + n
// takes it, G6: the immediate when add and sub encode it, 2.4, a
// negative one written with the source's operator, add x0, x0, #-4
// (6.4); otherwise held in a register by the constant row of 6.4, and
// added as a held n of a size of 1 is.
right_t
product(state_t &s, long long d, unsigned avoid = 0)
{
    if (arithmetic_immediate(d, 64))
        return { .immediate = imm(d) };

    const auto v = fresh(s, 64, avoid);
    constant(s, reg_of(s, v), d, 64);

    return { .value = v, .width = 64 };
}

// The instructions of p + n's row for a product d, product()'s: the add,
// and before it the constant row's where add's immediate does not
// encode d.
int
product_length(long long d)
{
    if (arithmetic_immediate(d, 64))
        return 1;

    return constant_length(d, 64) + 1;
}

// The n of p + n, n + p and p - n, and of p += n and p -= n, generated
// as the row takes it, 6.3.  A constant is its product with the size.
// For a size of 1, 2, 4 or 8 any other n is the index of index_of(),
// read by the shifted or extended operand of add and sub; for a power
// of two above 16 it is generated whole, its extension by its load or
// the cast's sxtw, and read by the shifted operand, ldrsw x1, [x29,
// #24]; add x0, x0, x1, lsl #5 on records of 32 bytes; for any other
// size it is generated whole and the row multiplies it by the size.
right_t
displacement(state_t &s, const expr_t &n, long size)
{
    if (is< literal_t >(n))
        return product(s, scaled(as< literal_t >(n).value, size));

    if (is_wide_scale(size))
        return { .value = rvalue(s, n), .width = 64, .shift = shift_of(size) };

    if (!is_scale(size))
        return { .value = rvalue(s, n), .width = 64, .factor = size };

    const auto index = index_of(s, n);

    return { .value = index.value,
             .width = 64,
             .extend = index.extend,
             .shift = shift_of(size) };
}

// The row of p + n and p - n, 6.3, G6: the address plus or minus n
// times the size, modulo 2^64, on the held pointer and the generated n,
// the result in L.  An immediate is add L, L, #c*s.  A held n of a size
// other than 1, 2, 4 or 8 is multiplied first, mov N, #s; mul R, R, N,
// N taken and freed.  Then add or sub with n as its last operand, add
// L, L, R, add L, L, R, lsl #2, add L, L, wR, sxtw #2.  When n is
// written first, n + p, it is L: a plain register keeps the source's
// order, add L, L, R, and the shifted or extended one, which the
// instruction takes last only, is add L, R, wL, sxtw #2.  A borrowed read
// as the value an instruction writes, L, or n under the mul, writes into
// a fresh register by destination(), add x0, x19, w20, sxtw #2, 6.1.  p
// += n writes the pointer in place, add x19, x19, w0, sxtw #2, and the
// product of a factor, which is n's, never lands in it.  A register
// variable handed down is the last add's or sub's destination, add x19,
// x0, w1, sxtw #2 (chapter 4), and so is the register the consumer
// names, wanted, add x0, x1, w0, sxtw #2 for a return's (6.1).
value_t
offset(state_t &s,
       binary_op_t op,
       value_t pointer,
       const right_t &n,
       bool pointer_first,
       into_t *into = nullptr,
       unsigned wanted = 0)
{
    const auto mnemonic = op == binary_op_t::add ? "add" : "sub";

    if (!n.immediate.empty()) {
        const auto from = reg(reg_of(s, pointer), 64);
        const auto to =
            reg(final_destination(s, pointer, 64, into, wanted), 64);
        insn(s, mnemonic, { to, from, n.immediate });
        return pointer;
    }

    const auto [first, second] = pointer_first ? operands(s, pointer, n.value) :
                                                 operands(s, n.value, pointer);
    const reg_t p = pointer_first ? first : second;
    reg_t i = pointer_first ? second : first;

    if (n.factor != 1) {
        const reg_t factor = take(s, bit(p) | bit(i));
        constant(s, factor, n.factor, 64);

        const reg_t product = destination(s, n.value, 64, bit(p) | bit(factor));
        insn(s, "mul", { reg(product, 64), reg(i, 64), reg(factor, 64) });
        i = product;
    }

    const auto index = index_operand(i, n.extend, n.shift);
    const bool plain = n.extend == extend_t::lsl && n.shift == 0;

    auto result = pointer_first ? pointer : n.value;
    release(s, pointer_first ? n.value : pointer);

    const auto to = reg(final_destination(s, result, 64, into, wanted), 64);

    if (pointer_first)
        insn(s, mnemonic, { to, reg(p, 64), index });
    else if (plain)
        insn(s, mnemonic, { to, reg(i, 64), reg(p, 64) });
    else
        insn(s, mnemonic, { to, reg(p, 64), index });

    return result;
}

// The address of a base and an index read before a store's right
// operand, 6.3: the two registers the form reads, [x2, w1, sxtw #2],
// but where that operand holds a call and the convention would spill
// both across it, the sum, add x0, x0, w1, sxtw #2, one register held
// and spilled, and the access through it, [x1].  A register variable's
// read is no register to spill, and a value an earlier call spilled
// would be reloaded for the add, so the form stays where either is.
address_t
index_across(state_t &s,
             value_t base,
             index_t index,
             int shift,
             bool pointer_first,
             const expr_t *between)
{
    if (!between || !calls_within(*between) || borrowed(s, base) ||
        borrowed(s, index.value) || s.values[base.id].reg < 0 ||
        s.values[index.value.id].reg < 0)
        return { .base = base,
                 .index = index.value,
                 .extend = index.extend,
                 .shift = shift };

    const right_t n { .value = index.value,
                      .width = 64,
                      .extend = index.extend,
                      .shift = shift };

    return { .base = offset(s, binary_op_t::add, base, n, pointer_first) };
}

// p + n on a slot's address as a base, the frame pointer the sum's
// source: the index's product, mul after mov N, #s at a size no form
// scales by, then add xR, x29, R in the index's register, add x0, x29,
// x0, or the form's extend and shift, add x0, x29, w0, sxtw #3.  The
// value is the sum, in a register of its own.
value_t
frame_sum(state_t &s, const right_t &n)
{
    reg_t i = reg_of(s, n.value);

    if (n.factor != 1) {
        const reg_t factor = take(s, bit(i));
        constant(s, factor, n.factor, 64);

        const reg_t product = destination(s, n.value, 64, bit(factor));
        insn(s, "mul", { reg(product, 64), reg(i, 64), reg(factor, 64) });
        i = product;
    }

    const auto index = index_operand(i, n.extend, n.shift);
    const reg_t to = destination(s, n.value, 64);
    insn(s, "add", { reg(to, 64), "x29", index });

    return n.value;
}

// The base of a row's sum, generated: nothing for a slot, x29 being
// the base; a label's page at the access's offset, adrp x0, rows+2; or
// the value of based()'s pointer or of the pointer itself.
value_t
row_base(state_t &s, const row_access_t &row)
{
    if (row.label) {
        const auto page = fresh(s, 64);
        const auto symbol = symbol_plus(row.slot->label, row.total);
        insn(s, "adrp", { reg(reg_of(s, page), 64), symbol });
        return page;
    }

    if (row.slot)
        return { };

    return rvalue(s, row.plan ? *row.plan->pointer : *row.pointer);
}

// A row's index as the access's register offset, 6.3: a product of no
// scale in its own register, mul after mov N, #s, then [x0, x1]; an
// index the access's form scales as it is, [x0, w1, sxtw #3].
address_t
row_register_offset(state_t &s, value_t base, const right_t &index)
{
    if (index.factor != 1) {
        const unsigned held = registers_of(s, { .base = base });
        const reg_t i = reg_of(s, index.value, held);
        const reg_t factor = take(s, bit(i) | held);
        constant(s, factor, index.factor, 64);

        const reg_t product =
            destination(s, index.value, 64, bit(factor) | held);
        insn(s, "mul", { reg(product, 64), reg(i, 64), reg(factor, 64) });
    }

    return address_t { .base = base,
                       .index = index.value,
                       .extend = index.extend,
                       .shift = index.shift };
}

// The operand of an access through a row by a variable index,
// row_access()'s, 6.3: the pointer's base and the index in the order
// written (G13), then the index the register offset, or its product
// added to the base, to x29 for a slot, and the constants the access's
// displacement past the sum, so that p[1][i][0] on int (*p)[3][3] is
// mul x1, x1, x2; add x0, x0, x1; ldr w0, [x0, #36] after p's load, and
// rows[i][2] on a global adrp x0, rows+2, the product's add and ldrb
// w0, [x0, :lo12:rows+2].  The sum writes a register of its own, never
// a register variable's.  A row with no index is the base and the
// total, [x0, #44] after p's load for p->a[3].
address_t
row_address(state_t &s, const row_access_t &row)
{
    if (!row.index)
        return address_t { .base = row_base(s, row),
                           .displacement = static_cast< int >(row.total) };

    value_t base { };
    if (row.pointer_first)
        base = row_base(s, row);

    const auto index = displacement(s, *row.index, row.stride);

    if (!row.pointer_first)
        base = row_base(s, row);

    if (row.register_offset)
        return row_register_offset(s, base, index);

    if (row.slot && !row.label)
        return address_t { .base = frame_sum(s, index),
                           .displacement = static_cast< int >(row.total) };

    const auto sum =
        offset(s, binary_op_t::add, base, index, row.pointer_first);

    return address_t { .base = sum,
                       .displacement = static_cast< int >(row.total),
                       .label = row.label ? row.slot->label : std::string { } };
}

// The value of a pointer whose sums by a constant, through its casts
// and its rows' layers, are more than one, 6.3: denoted()'s base, a
// variable index on the way added to it in the order written, and the
// constants as one add of their sum, product()'s, add x0, x0, #13 for
// &((int *) (p + 1))[3] on a char *p, add x0, x0, #20 for &p[1][2] on
// int (*p)[3], and for &((int (*)[3]) (c + 4))[i][2] on a char *c the
// index's product and add after c's load and add x0, x0, #12, the
// column's 8 with the 4 under the row; none where they sum to 0, &((int
// *) (p + 4))[-1] being p.  A sum that add's immediate does not encode
// is held, mov x1, #32777; add x0, x0, x1 for &((long *) (p +
// 1))[4097], where that is fewer instructions than the rows write for
// the parts, denoted()'s written; otherwise the rows add the parts as
// before, add x0, x0, #65536; add x0, x0, #16, where the sum held is
// three.  A register variable handed down is the last instruction's
// destination, and with no index the base's row's too, base_into()'s,
// as for p + c.  None for one constant, whose add is the rows' own or
// summed()'s, and for a name's address, whose slot's or label's add
// carries the part within its object, folded()'s, a row's on a name
// under its index included.
std::optional< value_t >
joined(state_t &s, const expr_t &e, into_t *into, unsigned wanted)
{
    const auto at = denoted(e);
    if (at.constants < 2 || named_offset(*at.base, s.defined))
        return std::nullopt;

    if ((at.bytes == 0 ? 0 : product_length(at.bytes)) >= at.written)
        return std::nullopt;

    if (!at.index) {
        if (at.bytes == 0)
            return rvalue(s, *at.base, into, wanted);

        auto onto = base_into(into);
        const auto base = rvalue(s, *at.base, onto ? &*onto : nullptr);
        return offset(s, binary_op_t::add, base, product(s, at.bytes), true,
                      into, wanted);
    }

    value_t base { };
    if (!at.index_first)
        base = rvalue(s, *at.base);

    const auto n = displacement(s, *at.index, at.index_stride);

    if (at.index_first)
        base = rvalue(s, *at.base);

    const bool last = at.bytes == 0;
    const auto sum = offset(s, binary_op_t::add, base, n, !at.index_first,
                            last ? into : nullptr, last ? wanted : 0);
    if (last)
        return sum;

    return offset(s, binary_op_t::add, sum, product(s, at.bytes), true, into,
                  wanted);
}

// A binary + or - of a pointer type, 6.5.7p9: the pointer and the
// integer, the integer widened to 64 bits by the parser, each generated
// in the order written (G13), a constant as its product with the size,
// whose immediate takes no register.  A name's address plus a constant
// is the address of folded()'s one operand, add x0, x29, #28 for a + 3,
// 6.3; constants under the pointer, through its casts and its rows'
// layers, are one add with this one, joined()'s, add x0, x0, #20 for
// &p[1][2] on int (*p)[3]; and any other lvalue's address plus a
// constant is summed()'s base and the one add of its displacement, add
// x0, x0, #-4 after the sum for p[i] - 1; a displacement of 0 is the
// base and no instruction, as &*p is p.  A register variable handed
// down goes to the row that writes the sum, chapter 4, and so does the
// register the consumer names, wanted (6.1); by a constant, to the
// pointer's row too, base_into()'s, so that q = p[1] into a register
// int *q is ldr x19, [x29, #24]; add x19, x19, #12, and to summed()'s
// base.  Not by an integer that is no constant: generated after the
// pointer it could read the variable the pointer's row wrote, and
// generated before it, n + p, its value could be a read of the variable
// that row overwrites.
value_t
pointer_arithmetic(state_t &s,
                   const expr_t &e,
                   into_t *into = nullptr,
                   unsigned wanted = 0)
{
    if (const auto a = folded(s, e))
        return address_of(s, *a, into);

    // &((int *) (p + 1))[3] on a char *p: add x0, x0, #13
    if (const auto v = joined(s, e, into, wanted))
        return *v;

    if (const auto a = summed(s, e, true, into)) {
        if (a->displacement == 0)
            return a->base;

        return offset(s, binary_op_t::add, a->base,
                      { .immediate = imm(a->displacement) }, true, into,
                      wanted);
    }

    const auto &b = as< binary_t >(e);
    const long size = stride(e.type);

    if (is< pointer_t >(b.lhs->type)) {
        auto onto = is< literal_t >(*b.rhs) ? base_into(into) : std::nullopt;
        const auto l = rvalue(s, *b.lhs, onto ? &*onto : nullptr);
        return offset(s, b.op, l, displacement(s, *b.rhs, size), true, into,
                      wanted);
    }

    const auto n = displacement(s, *b.lhs, size);
    auto onto = is< literal_t >(*b.lhs) ? base_into(into) : std::nullopt;
    const auto p = rvalue(s, *b.rhs, onto ? &*onto : nullptr);

    return offset(s, b.op, p, n, false, into, wanted);
}

// p - q, 6.5.7p10: the difference of the addresses divided by the size,
// a long.  G6: a division, sub L, L, R and then mov R, #s; sdiv L, L,
// R, the size held, since sdiv takes no immediate; never a shift, since
// the two pointers are any two addresses.  A size of 1 is the sub
// alone.  A register variable handed down is the last instruction's
// destination, the sub's or the sdiv's (chapter 4), and so is the
// register the consumer names, wanted (6.1).  Before an sdiv the sub
// writes the variable too, base_into()'s copy, and the sdiv divides it
// in place, sub x20, x21, x0; mov x0, #4; sdiv x20, x20, x0 for w = q -
// v: both pointers are read by the sub, and the size's mov writes an
// expression register, so nothing reads the variable between.
value_t
difference(state_t &s,
           const binary_t &b,
           into_t *into = nullptr,
           unsigned wanted = 0)
{
    const long size = stride(b.lhs->type);

    auto l = rvalue(s, *b.lhs);
    const auto r = rvalue(s, *b.rhs);
    const auto [left, right] = operands(s, l, r);
    release(s, r);

    const bool last = size == 1;
    auto onto = last ? std::nullopt : base_into(into);
    into_t *sub_into = last ? into : nullptr;
    if (onto)
        sub_into = &*onto;

    const auto to =
        reg(final_destination(s, l, 64, sub_into, last ? wanted : 0), 64);
    insn(s, "sub", { to, reg(left, 64), reg(right, 64) });

    if (last)
        return l;

    const auto d = fresh(s, 64);
    constant(s, reg_of(s, d), size, 64);

    return divide(s, binary_op_t::div, l, d, int_t { 64, true }, into, wanted);
}

// The condition of a comparison, the suffix of b.cc and the operand of
// cset, 2.5: signed or unsigned by the operands' type, which the
// parser's conversions made common; a pointer's is unsigned, G7.
std::string_view
condition(binary_op_t op, bool is_signed)
{
    switch (op) {
    case binary_op_t::lt: return is_signed ? "lt" : "lo";
    case binary_op_t::le: return is_signed ? "le" : "ls";
    case binary_op_t::gt: return is_signed ? "gt" : "hi";
    case binary_op_t::ge: return is_signed ? "ge" : "hs";
    case binary_op_t::eq: return "eq";
    default: return "ne";
    }
}

// cmp L, R of the held left value l and the right operand rhs, at
// width, or cmp L, #c for a constant cmp encodes, the immediate of add
// and sub, 2.4, whichever the comparison.  A negative constant is
// written as the source has it, cmp w0, #-1, which the assembler
// encodes as cmn w0, #1, the same flags, 6.4.  The flags are set and the
// right value freed; l stays held.
void
compare_with(state_t &s, value_t l, const expr_t &rhs, int width)
{
    const auto r = right_operand(s, binary_op_t::eq, rhs, width);

    if (!r.immediate.empty()) {
        insn(s, "cmp", { reg(reg_of(s, l), width), r.immediate });
        return;
    }

    const auto [left, right] = operands(s, l, r.value);
    insn(s, "cmp", { reg(left, width), reg(right, width) });

    release(s, r.value);
}

// A comparison, or an operation of +, *, &, | or ^, in the order its
// row generates it, 6.1: a constant on the left is the constant on the
// right, the comparison mirrored, so 5 < x is cmp w0, #5 after the load
// of x, as x > 5 is, and 5 + x is add w0, w0, #5.  A constant the
// instruction does not encode is held in a register in either order,
// and no instruction here names an object in memory, so it is taken on
// the right too: its mov then comes after the right operand, which
// holds nothing across that operand's evaluation, 5 * f() among them.
// A pointer's sum keeps the rows of p + n, which take n as it comes.
operation_t
ordered(const binary_t &b)
{
    return constant_first(b) ? mirrored(b) : as_written(b);
}

// cmp for a relational or equality operator, left operand first (G13).
// The left value stays held, for cset or for the caller to free.
value_t
compare(state_t &s, const operation_t &b)
{
    const auto l = rvalue(s, *b.lhs);
    compare_with(s, l, *b.rhs, operation_width(b.lhs->type));

    return l;
}

// The branch to label on the flags a comparison set, when op holds:
// b.cc of the condition, 7.1.
void
jump_if(state_t &s, binary_op_t op, bool is_signed, int label)
{
    insn(s, std::format("b.{}", condition(op, is_signed)),
         { label_name(s.function, label) });
}

// A comparison as a value, the row of 6.3: cmp, then cset of the
// condition into L's w name, the int the operator yields, in one
// instruction where x86 needs two, valid to 64 bits, truth_valid, so
// (long) (a < b) writes nothing after it.  A borrowed read as L is only
// read by the cmp, and the cset takes a fresh register, by
// destination(); a register variable handed down is the cset's, cset
// w19, lt, an int's, a long's and an unsigned long's (chapter 4), and
// so is the register the consumer names, wanted, cset w0, lt for a
// return's (6.1).
value_t
comparison(state_t &s,
           const operation_t &b,
           into_t *into = nullptr,
           unsigned wanted = 0)
{
    const bool is_signed = integer(b.lhs->type).is_signed;

    auto l = compare(s, b);
    insn(s, "cset",
         { reg(final_destination(s, l, truth_valid, into, wanted), 32),
           condition(b.op, is_signed) });

    return l;
}

// The test of zero of a held value v of type t, 7.1, the one function
// the jump code and ! share.  With a label, a jump to it when v is
// nonzero (sense) or zero (!sense): cbnz or cbz, on the w name or the x
// name by the type's width, one instruction and no flags (call 3).
// Without one, the flags for the caller's cset, eq when v is zero: cmp
// L, #0, the row of !.  A value valid to less than 32 bits, the operand
// of a narrowing cast, has stale bits above its type's width, and the
// machine compares whole registers only: either way it is tst of the
// low bits alone, tst w0, #0xff, then b.ne or b.eq to the label.
void
test_zero(state_t &s,
          value_t v,
          const int_t &t,
          std::optional< int > label,
          bool sense = false)
{
    const reg_t r = reg_of(s, v);

    if (s.values[v.id].valid < 32) {
        const long long low_bits = (1ll << t.width) - 1;
        insn(s, "tst", { reg(r, 32), bitmask_of(low_bits, 32) });

        if (label)
            insn(s, sense ? "b.ne" : "b.eq",
                 { label_name(s.function, *label) });
        return;
    }

    if (!label) {
        insn(s, "cmp", { reg(r, t.width), imm(0) });
        return;
    }

    insn(s, sense ? "cbnz" : "cbz",
         { reg(r, t.width), label_name(s.function, *label) });
}

// && and || as a value: jump code into one register on both paths,
// mov L, #1 and mov L, #0, never csel, 6.3, the value valid to 64 bits,
// truth_valid.  before_paths() first gives every held value one location
// on both, so the paths leave the register map as they found it
// whatever their operands do.  Handed a register variable, each path
// writes the variable's register, mov w19, #1 and mov w19, #0, after
// the jump code has read every operand, and the value is the
// variable's read (chapter 4); the assignment's preserve() has copied
// every read of the variable before the first divergence.
value_t
logical(state_t &s, const expr_t &e, into_t *into = nullptr)
{
    const auto incoming = before_paths(s, e);

    const int no = new_label(s);
    const int end = new_label(s);

    cbranch(s, e, no, false);

    if (takes(into, truth_valid)) {
        const auto r = reg(variable_register(into->variable), 32);

        insn(s, "mov", { r, imm(1) });
        jump(s, end);

        place_label(s, no);
        insn(s, "mov", { r, imm(0) });

        place_label(s, end);

        return taken(s, *into);
    }

    const auto v = fresh(s, truth_valid);
    const auto r = reg(reg_of(s, v), 32);

    insn(s, "mov", { r, imm(1) });
    jump(s, end);

    place_label(s, no);
    insn(s, "mov", { r, imm(0) });

    place_label(s, end);

    return v;
}

// A conditional whose value is not wanted, a void one or one in the
// effect context: the jump code of 6.5.16p5, and each arm for its
// effect, chapter 6.6, so a zero assigned in an arm is one store.
void
conditional_effect(state_t &s, const expr_t &e)
{
    const auto &c = as< conditional_t >(e);

    const auto incoming = before_paths(s, e);

    const int no = new_label(s);
    const int end = new_label(s);

    cbranch(s, *c.cond, no, false);

    effect(s, *c.yes);
    jump(s, end);

    place_label(s, no);
    effect(s, *c.no);

    place_label(s, end);
}

// The load of a signed object in memory extended to a cast's width,
// 6.3: ldrsw x0, [x29, #16] for (long) i on an int, ldrsb x0 for a
// signed char.  It reads the bytes the load at 32 bits reads, at the
// object's width (G10), and its displacement scales by that width as
// the plain load's does, so a slot it does not reach goes through x16
// alike (5.3); the cast then finds the value valid to its width.  The
// rule is the Seventh Edition compiler's: match(), in c10.c, tries a
// row again with the conversion dropped, and its tables convert an
// operand in memory in the instruction that reads it.
value_t
extending_load(state_t &s,
               const expr_t &e,
               const int_t &from,
               int to,
               into_t *into = nullptr)
{
    return load(s, address(s, e), from, std::max(32, to), into);
}

// The integer conversion of the operand e to the type to, 6.3.  G2:
// narrowing is no instruction, the narrower register name is the value;
// widening extends by the operand's signedness, and only when the
// register is not already valid to the new width, as it is after the
// load of a char or a 32-bit write of an unsigned value.  A signed
// object in memory is loaded extended to the new width,
// extending_load(), so the load is the widening; an unsigned one is
// loaded as any read, valid to 64 bits by the load's own extension.
// The object may be under casts that keep its bits, object_under()'s:
// (long) (int) u is ldrsw x0, [x29, #16], and (unsigned long)
// (unsigned) i and (long) (char) c the ldr and the ldrb, valid to 64
// bits, each reading the object at its own width (G10); and under a
// promotion, widened_read()'s, (long) (int) c on a signed char the one
// ldrsb x0, and on an unsigned char the one ldrb w0, valid to 64 bits.
// A narrowing cast under the widening keeps its load and the extension
// after it.  A signed object widened to an unsigned of 32 bits is its
// load into the w register, which clears the upper half, so the value
// is valid to 64 bits as the unsigned it is: (long) (unsigned) c is the
// one ldrsb w0, and no mov w0, w0 clears it again; not a register
// variable's read, whose upper half is its own type's (chapter 4).
// A constant widened into a register variable is the constant of the
// new type, mov x19, #-1, and so is a signed one for any other
// consumer, mov x0, #-1.  A change of signedness alone is nothing.  A pointer is its
// integer view, so pointer to pointer is nothing, an integer widens to
// a pointer by its own signedness (6.3.2.3p5), sxtw for an int, and a
// pointer narrows to an integer by its low bits (p6).  A register
// variable handed down goes on to the operand's row through() a
// conversion that leaves its representation, and to the extension when
// the operand's row did not take it: the last instruction written for
// the conversion writes the register, ldrsb w19, [x29, #16] for
// register int x = c, sxtw x19, w0 for register long y = a + b (chapter
// 4).  The register the consumer of the value names, wanted, goes on to
// the operand and to the extension, sxtw x0, w1 for a return's (6.1).
value_t
converted(state_t &s,
          const expr_t &e,
          const type_t &to,
          into_t *into = nullptr,
          unsigned wanted = 0)
{
    const auto &from = integer(e.type);
    const auto &target = integer(to);
    const bool widens = target.width > from.width;
    const auto onward = through(into, target, widens);

    // c ? x : -1 into a register long, mov x19, #-1, and returned as a
    // long, mov x0, #-1
    if (widens && is< literal_t >(e)) {
        const auto value = widened(as< literal_t >(e).value, from);
        if (takes(onward, 64)) {
            const int width = register_width(onward->variable);
            constant(s, variable_register(onward->variable),
                     normalized(value, width), width);
            return taken(s, *onward);
        }

        if (from.is_signed)
            return literal(s, { value }, to);
    }

    const auto read = widens ? widened_read(e) : widened_read_t { };
    const bool loads =
        read.object && (read.type->is_signed || read.object != &e);
    const int width = loads && read.type->is_signed ? target.width : 32;

    auto v = loads ?
                 extending_load(s, *read.object, *read.type, width, onward) :
                 rvalue(s, e, onward, wanted);
    if (widens)
        v = extended(s, v, from, target.width, onward, wanted);

    auto &valid = s.values[v.id].valid;
    valid = std::min(valid, target.width);

    // (long) (unsigned) c on a signed char: ldrsb w0, [x29, #16], its
    // write of the w register the zero extension of the unsigned
    if (loads && read.type->is_signed && !target.is_signed && width == 32 &&
        !borrowed(s, v))
        valid = written(32, false);

    return v;
}

// An arm of a branch into a register variable computed into the
// variable's register where its row can, chapter 4: a constant by the
// constant row, mov w19, #0, constant_into()'s, as an assignment's, and
// any other arm by its row handed the variable as path, ldr w19, [x29,
// #20] or add w19, w19, #1 for an arm r + 1, which reads the variable
// before the path writes it.  path says whether the row did; the value
// is nothing for a constant.
value_t
arm_value(state_t &s, const expr_t &arm, into_t &path)
{
    if (constant_into(s, path.variable, arm)) {
        path.taken = true;
        return { };
    }

    return rvalue(s, arm, &path);
}

// One path of a branch into a register variable, chapter 4: the arm's
// value written into the variable's register as an assignment's right
// operand is, by arm_value() where the arm's row can, and otherwise by
// move_in()'s mov after it, none for the variable's own read, r = c ? x
// : r.  Under a conversion that widens the
// conditional, to, the arm is converted()'s, its last instruction
// writing the variable extended, ldrsw x19, [x29, #28] for an int in
// memory.  The value is consumed.
void
arm_into(state_t &s, const expr_t &arm, const into_t &into, const type_t *to)
{
    into_t path { into.variable, false, into.as };
    const auto v = to ? converted(s, arm, *to, &path) : arm_value(s, arm, path);
    if (!path.taken)
        move_in(s, into.variable, v);

    release(s, v);
}

// Whether the row of an arm of ?: handed a register variable writes the
// variable's register itself, chapter 4.  A row here writes its
// destination with its last instruction, after it has read every
// operand, so the rows that do are all but a few, and those named are
// the ones certain to: a constant, its mov; an object in memory, its
// load, but through the variable's own register stepped by the access,
// whose load cannot write its base (2.3); a truth value, the cset or the
// mov #1 and mov #0; an operation on integers, +, -, *, /, %, the
// shifts, &, |, ^, - and ~, the division's msub or sdiv included; a step
// of an object in memory whose address does not read the variable, its
// load, or for a prefix step of 32 bits or more its add; and the
// comma's right operand.  A call's result, a register variable's read,
// an assignment and any other row are no row that writes it.
bool
lands_in(const expr_t &arm, const symbol_t *variable)
{
    if (is< literal_t >(arm))
        return true;

    if (in_memory(arm))
        return !is< unary_t >(arm) ||
               step_of(*as< unary_t >(arm).arg).variable != variable;

    if (is< binary_t >(arm)) {
        const auto &b = as< binary_t >(arm);
        if (b.op == binary_op_t::comma)
            return lands_in(*b.rhs, variable);

        if (is_comparison(b.op) || b.op == binary_op_t::log_and ||
            b.op == binary_op_t::log_or)
            return true;

        return !is< pointer_t >(arm.type) && !is< pointer_t >(b.lhs->type);
    }

    if (!is< unary_t >(arm))
        return false;

    const auto &u = as< unary_t >(arm);
    switch (u.op) {
    case unary_op_t::neg:
    case unary_op_t::bit_not:
    case unary_op_t::log_not: return true;
    case unary_op_t::post_inc:
    case unary_op_t::post_dec:
        return in_memory(*u.arg) && !names(*u.arg, variable);
    case unary_op_t::pre_inc:
    case unary_op_t::pre_dec:
        return in_memory(*u.arg) && !names(*u.arg, variable) &&
               integer(u.arg->type).width >= 32;
    default: return false;
    }
}

// Whether the conversion that widens an arm of ?: into a register
// variable costs the arm no instruction of its own, backend.md 6.1 and
// chapter 4: its row writes the variable's register extended, a
// constant, mov x19, #-1, an object in memory, by its extending load,
// ldrsw x19, [x29, #28], under casts that keep its bits and under a
// promotion too, widened_read()'s, ldrb w19 on a char, and a truth
// value, cset w19, lt, valid to 64 bits; or its value is moved into the
// join in any case, and the extension is written where the move would
// be, a register variable's read, sxtw x19, w20.
bool
widens_free(const expr_t &arm)
{
    if (is< literal_t >(arm) || widened_read(arm).object ||
        register_variable(arm))
        return true;

    if (is< unary_t >(arm))
        return as< unary_t >(arm).op == unary_op_t::log_not;

    if (!is< binary_t >(arm))
        return false;

    const auto op = as< binary_t >(arm).op;

    return is_comparison(op) || op == binary_op_t::log_and ||
           op == binary_op_t::log_or;
}

// Whether a conditional handed a register variable may have each path
// write the variable's register, chapter 4: one of the variable's own
// type, or of the type into's row computes in, computed_type(), so that
// each arm is the right operand of an assignment to it; for a pointer
// variable, one of any pointer type, whose conversion to the variable's
// writes nothing (6.3), q = &(c ? m : n)[1][2] on two pointers to rows
// being ldr x19 on each path and add x19, x19, #20.
bool
joins_into(const expr_t &e, const into_t *into)
{
    if (!into || into->taken)
        return false;

    const auto &type = computed_type(*into);

    return e.type == type || (is< pointer_t >(e.type) && is< pointer_t >(type));
}

// Whether a conditional under a conversion that widens it has the
// conversion handed to each arm, backend.md 6.1 and chapter 4: where one
// arm at least widens free, widens_free(), so that the other's
// extension, if it needs one, is the one the join would have had after
// it, and no path writes more than the join and the extension after
// it.  An extension here writes any register from any other, so an arm
// whose value is elsewhere is extended into the join, where the join
// would have moved it.  For a register variable handed down, of the
// type its row computes in, each path writes the variable, l = c ? x :
// y into a register long from two ints being ldrsw x19, [x29, #28] and
// ldrsw x19, [x29, #32]; for any other consumer the arms join extended,
// in the register it names where it names one, return a ? (*p)++ : b
// as a long being sxtw x0, w0 after the step and ldrsw x0, [x29, #32],
// and l = c ? a : b on two signed chars into a long in memory ldrsb
// x0 on each path and no sxtw after the join, where the widening is a
// signed one, since an unsigned value written at 32 bits is valid to
// 64 already.  Two arms that each need an extension of their own, two
// sums or two stores through a call's result, keep the join and the
// one extension after it.
bool
widens_arms(const cast_t &c, const type_t &to, const into_t *into)
{
    if (!is< conditional_t >(*c.arg) || !is< int_t >(to) ||
        !is< int_t >(c.arg->type) ||
        integer(to).width <= integer(c.arg->type).width)
        return false;

    const bool paths = into && !into->taken;
    if (paths && !(to == computed_type(*into)))
        return false;

    if (!paths && !integer(c.arg->type).is_signed)
        return false;

    const auto &k = as< conditional_t >(*c.arg);

    return widens_free(*k.yes) || widens_free(*k.no);
}

// Whether e holds a call anywhere, whose result the convention puts in
// x0 (chapter 8).
bool
calls_within(const expr_t &e)
{
    if (is< call_t >(e))
        return true;

    if (is< unary_t >(e))
        return calls_within(*as< unary_t >(e).arg);

    if (is< binary_t >(e)) {
        const auto &b = as< binary_t >(e);
        return calls_within(*b.lhs) || calls_within(*b.rhs);
    }

    if (is< assign_t >(e)) {
        const auto &a = as< assign_t >(e);
        return calls_within(*a.lhs) || calls_within(*a.rhs);
    }

    if (is< cast_t >(e))
        return calls_within(*as< cast_t >(e).arg);

    if (is< conditional_t >(e)) {
        const auto &k = as< conditional_t >(e);
        return calls_within(*k.cond) || calls_within(*k.yes) ||
               calls_within(*k.no);
    }

    return false;
}

// Whether the rows of an arm of ?: handed the register a consumer names,
// x0 for a return's, write it with no instruction of their own, 6.1, or
// leave the value where a join in any other register moves it too.  A
// row writes its destination last, after it has read its sources, and
// nothing but the arm is held at its top, so those named are the ones
// certain to: a constant, a load, a call's result, a truth value, an
// operation on integers, - and ~, a conversion of an arm that does, and
// the comma's right operand; a register variable's read, an assignment
// to one and a step of one, whose value is copied into the join in any
// case; and an assignment, a compound assignment or a step of an object
// in memory whose place holds no call, since the place is generated with
// x0 left for last, and for a plain assignment whose right operand
// writes x0.  A place that holds a call's result holds x0 at the store:
// *g() = a writes no x0, and nor does any row not named.
bool
takes_name(const expr_t &arm)
{
    if (is< literal_t >(arm) || is< call_t >(arm) || in_memory(arm) ||
        register_variable(arm))
        return true;

    if (is< cast_t >(arm))
        return takes_name(*as< cast_t >(arm).arg);

    if (is< binary_t >(arm)) {
        const auto &b = as< binary_t >(arm);
        if (b.op == binary_op_t::comma)
            return takes_name(*b.rhs);

        if (is_comparison(b.op) || b.op == binary_op_t::log_and ||
            b.op == binary_op_t::log_or)
            return true;

        return !is< pointer_t >(arm.type) && !is< pointer_t >(b.lhs->type);
    }

    if (is< assign_t >(arm)) {
        const auto &a = as< assign_t >(arm);
        if (register_variable(*a.lhs))
            return true;

        return !calls_within(*a.lhs) && (a.op || takes_name(*a.rhs));
    }

    if (!is< unary_t >(arm))
        return false;

    const auto &u = as< unary_t >(arm);
    if (u.op == unary_op_t::neg || u.op == unary_op_t::bit_not ||
        u.op == unary_op_t::log_not)
        return true;

    return is_step(u.op) &&
           (register_variable(*u.arg) || !calls_within(*u.arg));
}

// The second path of a conditional each of whose paths writes the
// register variable, chapter 4: the branch past it, the second arm into
// the variable, arm_into()'s, converted to to where it is given, and the
// join, after which the value is the variable's read.
value_t
second_path(state_t &s,
            const expr_t &arm,
            into_t &into,
            const type_t *to,
            int no,
            int end)
{
    jump(s, end);

    place_label(s, no);
    arm_into(s, arm, into, to);

    place_label(s, end);

    return taken(s, into);
}

// 6.5.16p5: the arm not chosen is not evaluated, and the jump says so;
// never csel.  The arms join in one expression register: the one the
// conditional's consumer names, wanted, where it names one, a return's
// x0 (6.1), and otherwise the first arm's, a borrowed read as the first
// arm copied into one, mov w0, w19, chapter 4.  Each arm's value is
// moved into the join where it is elsewhere, by mov at the operation's
// width, backend.md 7.2.  Each arm is generated for the named join, as
// for a consumer that names it, so that its last instruction writes it
// where it can: return c ? p[i]++ : 0 is the first arm's ldr w0, [x1,
// w2, sxtw #2] and the second arm's mov w0, #0, and no move after the
// join.  Handed a register variable that joins_into(), the first arm is
// computed into it where its row can, arm_value()'s, and then each path
// writes the variable and no register joins them: r = c ? x : y is ldr
// w19, [x29, #28] on the first path and the second arm's arm_into(),
// ldr w19, [x29, #32], and the value is the variable's read.  An arm
// reads the variable on its own path before the path writes it, and the
// assignment's preserve() has copied every read of it held across the
// branch.  Where the first arm's row does not write the variable, the
// first path moves its value in and the second arm's row writes it,
// where lands_in() says: r = c ? f(x) : a / b is mov w19, w0 after the
// call and sdiv w19, w0, w1; where an arm is the variable's own read,
// which writes nothing, the condition branches past the other path
// alone, one_path()'s.  Under a conversion that widens the
// conditional, to, which cast() hands down where widens_arms(), each arm
// is converted: into the variable on each path, or into the join, return
// a ? (*p)++ : b as a long being sxtw x0, w0 after the step and ldrsw
// x0, [x29, #32], with no extension after the join.  Otherwise the arms
// join as for any consumer and the assignment moves the join in after it
// (chapter 4).
value_t
conditional(state_t &s,
            const expr_t &e,
            into_t *into = nullptr,
            unsigned wanted = 0,
            const type_t *to = nullptr)
{
    const auto &c = as< conditional_t >(e);
    const auto &t = e.type;

    if (is< void_t >(t)) {
        conditional_effect(s, e);
        return { };
    }

    const auto incoming = before_paths(s, e);

    const int no = new_label(s);
    const int end = new_label(s);

    // r = c ? r : x on a register r: cbnz past the second path alone
    const bool joins = !to && joins_into(e, into);
    if (joins &&
        (own_read(*c.yes, into->variable) || own_read(*c.no, into->variable)))
        return one_path(s, c, *into, end);

    cbranch(s, *c.cond, no, false);

    if (to && into) {
        arm_into(s, *c.yes, *into, to);
        return second_path(s, *c.no, *into, to, no, end);
    }

    const auto &type = to ? *to : t;
    const int width = std::max(32, integer(type).width);

    const bool is_signed = integer(type).is_signed;
    const auto named = wanted_register(s, { }, wanted);
    const unsigned arm = named ? bit(*named) : 0;

    into_t path { joins ? into->variable : nullptr, false,
                  joins ? into->as : nullptr };
    const auto yes = joins ? arm_value(s, *c.yes, path) :
                     to    ? converted(s, *c.yes, *to, nullptr, arm) :
                             rvalue(s, *c.yes, nullptr, arm);

    if (path.taken || (joins && lands_in(*c.no, into->variable))) {
        if (!path.taken)
            move_in(s, into->variable, yes);
        release(s, yes);

        return second_path(s, *c.no, *into, nullptr, no, end);
    }

    if (borrowed(s, yes))
        copy(s, yes, named ? *named : take(s, 0));

    reg_t join = reg_of(s, yes);
    int valid = s.values[yes.id].valid;
    if (named && join != *named) {
        mov(s, join, *named, width);
        join = *named;
        valid = written(width, is_signed);
    }
    release(s, yes);
    jump(s, end);

    place_label(s, no);
    const auto other = to ? converted(s, *c.no, *to, nullptr, arm) :
                            rvalue(s, *c.no, nullptr, arm);
    const reg_t from = reg_of(s, other);
    int other_valid = s.values[other.id].valid;
    if (from != join) {
        mov(s, from, join, width);
        other_valid = written(width, is_signed);
    }
    release(s, other);

    place_label(s, end);

    // valid to what both arms left
    return hold(s, join, std::min(valid, other_valid));
}

// Whether the left operand of L op R, of type t, is generated into the
// register variable handed down, chapter 4, so that its row writes the
// variable's register and the operation reads it there, ldr w19, [x29,
// #24]; add w19, w19, #7 for r = x + 7.  R is generated after L's row
// has written the register, so R reads nothing of the variable: a
// constant, held or an immediate, or another register variable's read,
// under any conversion; r = x + r, whose R is the variable, would read
// x.  The operation is in the type the variable's value is computed
// in, computed_type(), so that the variable's read after L's row is
// valid to the operation's width.  r = r + x, whose L is the variable's
// own read, is the one instruction already.
bool
left_into(const operation_t &o, const type_t &t, const into_t *into)
{
    if (!into || into->taken || !(t == computed_type(*into)))
        return false;

    const expr_t *r = o.rhs;
    while (is< cast_t >(*r))
        r = as< cast_t >(*r).arg.get();

    if (is< literal_t >(*r))
        return true;

    const auto *other = register_variable(*r);

    return other && other != into->variable;
}

// A binary operator's row.  The register the consumer of its value
// names, wanted, goes to the row's last instruction, which writes it
// where it is free, add w0, w1, w0 after a right operand that took x0;
// the operands are generated as for any consumer, since the instruction
// names its destination apart from its sources (6.1).  A register
// variable handed down goes to the row's last instruction, and to the
// left operand's row where left_into() holds (chapter 4).
value_t
binary(state_t &s, const expr_t &e, into_t *into = nullptr, unsigned wanted = 0)
{
    const auto &b = as< binary_t >(e);

    switch (b.op) {
    case binary_op_t::log_and:
    case binary_op_t::log_or: return logical(s, e, into);
    // 6.5.18p2: the left operand is evaluated as a void expression,
    // the effect context of chapter 6.6
    case binary_op_t::comma: {
        effect(s, *b.lhs);
        return rvalue(s, *b.rhs, into, wanted);
    }
    default: break;
    }

    if (is_comparison(b.op))
        return comparison(s, ordered(b), into, wanted);

    if (is< pointer_t >(e.type))
        return pointer_arithmetic(s, e, into, wanted);

    if (is< pointer_t >(b.lhs->type))
        return difference(s, b, into, wanted);

    // G13: the left operand first, held while the right one is
    // generated; a constant on the left of an operator that commutes is
    // taken on the right (6.1)
    const auto o = ordered(b);
    auto onto = left_into(o, e.type, into) ? base_into(into) : std::nullopt;
    const auto l = rvalue(s, *o.lhs, onto ? &*onto : nullptr);
    const auto r = right_operand(s, o.op, *o.rhs, operation_width(e.type));

    return arithmetic(s, o.op, l, r, e.type, into, wanted);
}

// ++ and -- of a register variable, chapter 4: add or sub of 1, or of
// the size for a pointer, on its register in place, add w21, w21, #1,
// the size as step() has it, then the re-extension of a narrow one,
// sxtb w19, w19, which is establish() from the variable's own register.
// The postfix forms read the variable first, and preserve() copies the
// read into an expression register before the write, mov w0, w19: that
// copy is the value, G13's copy of chapter 4.  The prefix forms yield a
// read of the variable after the write, as a compound assignment does.
value_t
step_register(state_t &s,
              unary_op_t op,
              const symbol_t *sym,
              into_t *into = nullptr)
{
    const auto &t = integer(sym->type);
    const int width = register_width(sym);
    const long amount = is< pointer_t >(sym->type) ? stride(sym->type) : 1;

    const bool up = op == unary_op_t::pre_inc || op == unary_op_t::post_inc;
    const bool post = op == unary_op_t::post_inc || op == unary_op_t::post_dec;

    // r = s++: the old value copied into r's register, which is G13's
    // copy, mov w20, w19 (chapter 4)
    const bool copies =
        post && takes(into, represented(sym)) && into->variable != sym;
    if (copies)
        insn(s, "mov",
             { reg(variable_register(into->variable), width),
               reg(variable_register(sym), width) });

    const auto old = post && !copies ? read(s, sym) : value_t { };
    preserve(s, sym);

    const auto r = reg(variable_register(sym), width);

    const auto n = product(s, amount);
    const auto by =
        n.immediate.empty() ? reg(reg_of(s, n.value), width) : n.immediate;
    insn(s, up ? "add" : "sub", { r, r, by });
    release(s, n.value);

    if (t.width < 32)
        establish(s, sym, variable_register(sym));

    if (copies)
        return taken(s, *into);

    return post ? old : read(s, sym);
}

// ++ and --, either side: the address once, the load, add or sub of 1,
// or of the size for a pointer (6.5.4.1p2, 6.5.3.5p2), the store
// through the same address, held between the two; G1, the machine
// wraps.  The size is the immediate where add and sub encode it, and
// otherwise held in a register and freed, as the product of p + n is.
// A char or a short is loaded extended to 32 bits and its low bytes
// stored, so no operator sees 8 or 16 bits.  The prefix forms operate
// on the loaded value's register and yield it, and handed a register
// variable, where the object is 32 bits or more and its address reads
// no register of the variable, load into the variable's register and
// step it there, ldr w19, [x29, #24]; add w19, w19, #1, so that the
// store takes the value from there and the value is the variable's
// read (chapter 4).  A narrower object's sum is no value of its type,
// and the conversion after it writes the variable.  The postfix forms
// name another register as the destination, add w1, w0, #1, and yield
// the old value where the load left it: nothing is copied, 6.1.  The
// place is generated with the register the consumer names, wanted,
// left for last, so that the load takes it, ldr w0, [x1, w2, sxtw #2];
// add w3, w0, #1 for return p[i]++ (6.1).  A register variable's is
// step_register()'s.
value_t
step(state_t &s,
     unary_op_t op,
     const expr_t &arg,
     into_t *into = nullptr,
     unsigned wanted = 0)
{
    if (const auto *sym = register_variable(arg))
        return step_register(s, op, sym, into);

    const auto &t = integer(arg.type);
    const int width = std::max(32, t.width);
    const long amount = is< pointer_t >(arg.type) ? stride(arg.type) : 1;

    const bool up = op == unary_op_t::pre_inc || op == unary_op_t::post_inc;
    const bool post = op == unary_op_t::post_inc || op == unary_op_t::post_dec;

    const auto place = address_avoiding(s, arg, wanted);

    // r = ++x: the load and the add write r's register and the store
    // takes it from there, ldr w19, [x29, #24]; add w19, w19, #1; str
    // w19, [x29, #24] (chapter 4); where the load cannot write it, the
    // add does, add w19, w0, #1
    const bool steps_into =
        !post && t.width >= 32 && takes(into, written(width, t.is_signed)) &&
        !(registers_of(s, place) & bit(variable_register(into->variable)));

    const auto l = fetch(s, first_access(place), t, width,
                         post || steps_into ? into : nullptr);
    const reg_t r = reg_of(s, l);
    const bool loaded_into = steps_into && into->taken;

    const auto v = post        ? fresh(s, written(width, t.is_signed),
                                       bit(r) | registers_of(s, place)) :
                   loaded_into ? l :
                   steps_into  ? taken(s, *into) :
                                 l;
    const reg_t result = reg_of(s, v);

    const auto n =
        product(s, amount, bit(r) | bit(result) | registers_of(s, place));
    const auto by =
        n.immediate.empty() ? reg(reg_of(s, n.value), width) : n.immediate;
    insn(s, up ? "add" : "sub", { reg(result, width), reg(r, width), by });
    release(s, n.value);

    store(s, v, last_access(place), t.width);
    release(s, place);

    if (loaded_into)
        return l;

    if (post || steps_into) {
        release(s, post ? v : l);
        return post ? l : v;
    }

    s.values[l.id].valid = stored_valid(written(width, t.is_signed), width, t);

    return l;
}

// The rows of the unary operators.  A register variable handed down is
// the destination of the row's last instruction, neg w19, w0, cset
// w19, eq, ldr w19, [x0] (chapter 4); a step writes its value before
// its store, and keeps it in L.  The register the consumer names,
// wanted, is the destination of neg, mvn and the cset of !, neg w0, w1
// for a return's, and a step's place is generated with it left for
// last (6.1).
value_t
unary(state_t &s,
      const unary_t &u,
      const type_t &t,
      into_t *into = nullptr,
      unsigned wanted = 0)
{
    switch (u.op) {
    // G1: neg of INT_MIN is INT_MIN.  A borrowed read writes into a
    // fresh register, neg w0, w19, by destination().
    case unary_op_t::neg:
    case unary_op_t::bit_not: {
        const auto &type = integer(t);
        const int width = operation_width(t);

        auto v = rvalue(s, *u.arg);
        const auto left = reg(reg_of(s, v), width);
        const auto to =
            reg(final_destination(s, v, written(width, type.is_signed), into,
                                  wanted),
                width);
        insn(s, u.op == unary_op_t::neg ? "neg" : "mvn", { to, left });

        return v;
    }

    // as a value, the row of 6.3: the test of zero, then cset L, eq,
    // valid to 64 bits, truth_valid.  The operand is not promoted, so it
    // is tested at its own width.  A borrowed read is only tested, and
    // the cset takes a fresh register.
    case unary_op_t::log_not: {
        auto v = rvalue(s, *u.arg);
        test_zero(s, v, integer(u.arg->type), std::nullopt);

        insn(s, "cset",
             { reg(final_destination(s, v, truth_valid, into, wanted), 32),
               "eq" });

        return v;
    }

    case unary_op_t::pre_inc:
    case unary_op_t::pre_dec:
    case unary_op_t::post_inc:
    case unary_op_t::post_dec: return step(s, u.op, *u.arg, into, wanted);

    case unary_op_t::addr: return address_value(s, *u.arg, into, wanted);

    // the load row from [L], at the object's width and extension
    case unary_op_t::deref: {
        const auto &type = integer(t);
        return load(s, pointed(s, *u.arg), type, std::max(32, type.width),
                    into);
    }
    }

    throw std::logic_error("c1: an unknown unary operator");
}

// A compound assignment's result into its register variable, chapter
// 4.  In place, the operation wrote the register at the operation's
// width, and a narrow variable is re-extended, sxtb w19, w19, as is a
// 32-bit one an operation of 64 bits wrote whole, mov w19, w19;
// otherwise the result is moved in by establish(), mov w19, w0.
void
settle(state_t &s, const symbol_t *sym, value_t v, int width)
{
    if (!borrowed(s, v)) {
        establish(s, sym, reg_of(s, v));
        return;
    }

    const auto &t = integer(sym->type);
    if (t.width < 32 || width > register_width(sym))
        establish(s, sym, variable_register(sym));
}

// The value of e written into a register variable, chapter 4: by the
// last instruction of the row at the top of e, handed the variable,
// where that leaves the variable's representation, add w19, w0, w1, ldr
// w19, [x29, #24], and the value is the variable's read; otherwise into
// a register of its own, then move_in()'s mov, mov w20, w0, or mov w19,
// w20 from another register variable, and nothing for the variable's
// own read, r = r; the value stays e's, valid to what it left, as for a
// slot.
value_t
computed_into(state_t &s, const symbol_t *sym, const expr_t &e)
{
    into_t into { sym };
    const auto v = rvalue(s, e, &into);
    if (!into.taken)
        move_in(s, sym, v);

    return v;
}

// Assignment to a register variable, chapter 4, after preserve() has
// copied every read of it an enclosing expression still holds, x + (x
// = e).  Plain: a constant is the constant row into the variable's
// register, mov w20, #0, and the value is a read of the variable;
// anything else is the right operand, computed into the register by
// computed_into().  Compound: the right operand first, then the
// operation's row with the variable's register as L and as the
// destination, in place, add w19, w19, w0, add w19, w19, #5, sdiv w19,
// w19, w0; p += n and p -= n by the row of p + n, add x19, x19, w0, sxtw
// #2.  The value is a read of the variable after the write.  The
// variable is read by the operation, after the right operand, as a
// slot's object is (G13), so no read of it is held across the right
// operand: r += (r = 2) is mov w19, #2 and add w19, w19, w19.  The read
// is extended into a fresh register first when the operation is wider
// than the register is valid to, sxtw x1, w19 for int += long, and
// settle() moves the result in.
value_t
assign_register(state_t &s, const assign_t &a, const symbol_t *sym)
{
    preserve(s, sym);

    if (!a.op) {
        if (constant_into(s, sym, *a.rhs))
            return read(s, sym);

        return computed_into(s, sym, *a.rhs);
    }

    const int width = operation_width(a.common);
    const bool stepped = is< pointer_t >(sym->type);

    const auto r = stepped ? displacement(s, *a.rhs, stride(sym->type)) :
                             right_operand(s, *a.op, *a.rhs, width);
    const auto l = extended(s, read(s, sym), integer(sym->type), width);

    const int outer = std::exchange(s.in_place, l.id);
    const auto v = stepped ? offset(s, *a.op, l, r, true) :
                             arithmetic(s, *a.op, l, r, a.common);
    s.in_place = outer;

    settle(s, sym, v, width);
    release(s, v);

    return read(s, sym);
}

// Plain or compound assignment to an object in memory; its value is the
// value stored, the low bytes of L.  The lvalue's address comes first
// (G13) and is held across the right operand, but for a constant index,
// which reads nothing and is written after it, held_index()'s, so that
// p[5000] = f() spills p alone across the call.  Plain: the right
// operand, then the store.  Compound: the right operand, then, as one
// piece and nothing evaluated between, the load of the object through
// the held address, extended into the common type the parser chose, the
// operation, and the store of the result's low bytes back through the
// same address (G2), so the object is read after the right operand.
// 6.5.17.3p4: "with respect to an indeterminately sequenced function
// call, the operation of a compound assignment is a single evaluation."
// p += n and p -= n are the row of p + n, n its right operand.  An
// assignment to a register variable is assign_register()'s.  The
// register the consumer of the value names, wanted, goes to the right
// operand of a plain one and to the operation of a compound one, whose
// last instruction writes it, and the place is generated with it left
// for last, address_avoiding()'s: return p[i] = x is ldr x0, [x29,
// #32]; str x0, [x1, w2, sxtw #3] (6.1).
value_t
assign(state_t &s, const assign_t &a, unsigned wanted = 0)
{
    const auto &t = integer(a.lhs->type);
    const bool stepped = a.op && is< pointer_t >(a.lhs->type);
    const bool shifted = a.op && is_shift(*a.op);

    // the parser converted the right operand to the common type, widened
    // it to 64 bits for a pointer, or promoted a shift count on its own
    if (!stepped && !shifted && !(a.rhs->type == a.common))
        throw std::logic_error("c1: an assignment across two types");

    if (const auto *sym = register_variable(*a.lhs))
        return assign_register(s, a, sym);

    const auto place = address_avoiding(s, *a.lhs, wanted, a.rhs.get());

    if (!a.op) {
        const auto v = rvalue(s, *a.rhs, nullptr, wanted);
        const auto at = held_index(s, place, bit(reg_of(s, v)));
        store(s, v, at, t.width);
        release(s, at);
        return v;
    }

    const int width = operation_width(a.common);
    const auto r = stepped ? displacement(s, *a.rhs, stride(a.lhs->type)) :
                             right_operand(s, *a.op, *a.rhs, width);
    const auto at =
        held_index(s, place, r.value.id >= 0 ? bit(reg_of(s, r.value)) : 0u);

    // the value of the expression has the lvalue's type: the bits
    // above its width are the operation's, not the value's extension
    const auto l = fetch(s, first_access(at), t, width);
    const auto v = stepped ?
                       offset(s, *a.op, l, r, true, nullptr, wanted) :
                       arithmetic(s, *a.op, l, r, a.common, nullptr, wanted);
    store(s, v, last_access(at), t.width);
    release(s, at);
    s.values[v.id].valid = stored_valid(s.values[v.id].valid, width, t);

    return v;
}

// The operand of a conversion that retypes(), into the variable's
// register, chapter 4.  An object in memory, under casts that keep its
// bits, is loaded at the extension of the conversion's type, ldr w19,
// [x29, #32] for register unsigned u = i and ldrsb w19, [x29, #24] for
// register signed char c = uc, the object read at its own width (G10).
// Any other operand of 32 bits or more is its own row, handed the
// variable as computed in the operand's type: the row's last
// instruction writes the register at the variable's width, and a write
// of a w register clears the upper half, so u = a + b on two ints is
// add w19, w0, w1.  A narrower operand is no row of its own, and is as
// any conversion's.
value_t
retyped(state_t &s, const cast_t &c, const int_t &target, into_t &into)
{
    if (const auto *object = object_under(c))
        return load(s, address(s, *object), target, std::max(32, target.width),
                    &into);

    into_t computed { into.variable, false, &c.arg->type };
    auto v = rvalue(s, *c.arg, target.width < 32 ? nullptr : &computed);
    into.taken = computed.taken;
    if (into.taken)
        return v;

    auto &valid = s.values[v.id].valid;
    valid = std::min(valid, target.width);

    return v;
}

// Whether a conversion to the type to changes the signedness alone of a
// value handed a register variable whose value is computed in to
// already, chapter 4: a second such conversion under the one that
// retypes() the variable, (int) u under v = 5 + (int) u into a register
// unsigned v.  The two change no bit at the variable's width, so the
// operand is computed in its own type as the first's is, ldr w19, [x29,
// #24].  retypes() takes the first alone, on both targets.
bool
retypes_again(const cast_t &c, const type_t &to, const into_t *into)
{
    if (!into || into->taken || !into->as || !(to == *into->as))
        return false;

    if (!is< int_t >(to) || !is< int_t >(c.arg->type))
        return false;

    const auto &from = integer(c.arg->type);
    const auto &target = integer(to);

    return from.width == target.width && from.is_signed != target.is_signed;
}

// A cast: a change of signedness alone that retypes(), retyped()'s; a
// conditional under a conversion that widens it, each arm widened where
// widens_arms() says, into a register variable's register on each path
// (chapter 4) or into the join (6.1); any other integer conversion,
// converted()'s.  An array decays to its address, 6.3.2.1p3, a string
// literal's included, and a function designator to the pointer to the
// function, p4, a register variable handed down going to the row that
// forms the address.  A cast to void evaluates its operand for its
// effect alone, chapter 6.6.
value_t
cast(state_t &s,
     const cast_t &c,
     const type_t &to,
     into_t *into = nullptr,
     unsigned wanted = 0)
{
    if (is< void_t >(to)) {
        effect(s, *c.arg);
        return { };
    }

    const auto &target = integer(to);

    if (is< array_t >(c.arg->type))
        return address_value(s, *c.arg, through(into, target, false), wanted);

    if (is< function_t >(c.arg->type))
        return function_pointer(s, *c.arg, through(into, target, false),
                                wanted);

    if (retypes(c, to, into) || retypes_again(c, to, into))
        return retyped(s, c, target, *into);

    // l = c ? x : y into a register long, ldrsw x19, [x29, #28] on each
    // path; return a ? (*p)++ : b as a long, ldrsw x0, [x29, #32] on the
    // second
    if (widens_arms(c, to, into))
        return conditional(s, *c.arg, into && !into->taken ? into : nullptr,
                           wanted, &to);

    return converted(s, *c.arg, to, into, wanted);
}

// The outgoing area of a call with stacked arguments past the eighth,
// chapter 8: 8 bytes for each, rounded up to 16, since sp is a multiple
// of 16 at the call (chapter 3).  It is opened and closed in the parts
// of frame_parts(), as the frame is, which reach 16 MiB less 16; an
// area of 16 MiB or more is a not-yet.
long
outgoing_size(std::size_t stacked)
{
    const long size = (8 * static_cast< long >(stacked) + 15) / 16 * 16;
    if (size >= 1L << 24)
        not_yet("an outgoing area of 16 MiB or more");

    return size;
}

// The address of the outgoing area's k-th 8 bytes for a store of size
// bytes, [sp, #8k]: the ninth argument at [sp], as clang -S writes it,
// the tenth at [sp, #8].  Past the store's reach, 2.3, the address is
// formed from sp as a far slot's is from x29, 5.3: add x16, sp, #upper
// and the store to [x16, #lower].  The add is written here, after the
// argument's load into x17, so x16 holds nothing else.
std::string
outgoing_operand(state_t &s, std::size_t k, int size)
{
    const long displacement = 8 * static_cast< long >(k);
    if (in_reach(displacement, size))
        return displacement == 0 ? "[sp]" :
                                   std::format("[sp, #{}]", displacement);

    const long upper = displacement & ~0xfffL;
    const long lower = displacement & 0xfffL;

    insn(s, "add", { "x16", "sp", imm(upper) });
    if (lower == 0)
        return "[x16]";

    return std::format("[x16, #{}]", lower);
}

// copy_object() comes with the copy of a record, after call(), which
// copies a list argument with it.
void copy_object(state_t &, address_t, address_t, const record_t &);

// An argument's slot, chapter 8: its displacement and its type, or, for
// a va_list, the slot of the list's copy, whose address is passed.
struct argument_slot_t
{
    int displacement;
    int_t type;
    bool by_reference = false;
};

// A va_list argument, 7.16p3, chapter 8: the convention passes a record
// of more than 16 bytes as the address of a copy the caller makes, so
// the list is copied into a slot of the call's own, 32 bytes, by the
// copy of a record, copy_object(), four ldr x17 and str x17 units, when
// the argument's turn comes (G13): an argument after it that reads or
// steps the list leaves the copy as it was.  The slot's address is the
// argument, add x2, x29, #d.
argument_slot_t
list_copy(state_t &s, const expr_t &arg)
{
    const auto &r = *as< record_type_t >(arg.type).record;
    const int displacement = allocate(s, static_cast< long >(r.size),
                                      static_cast< int >(r.alignment));
    copy_object(s, slot_address(displacement), address(s, arg), r);

    return { .displacement = displacement,
             .type = { .width = 64, .is_signed = false },
             .by_reference = true };
}

// Each argument of a call written to a slot of its own, chapter 8, in
// order and complete before the next (G13): taken before the argument
// is evaluated and written by the store row of 6.6, or for a va_list
// the list's copy, list_copy().
std::vector< argument_slot_t >
argument_slots(state_t &s, const call_t &c, const function_t &f)
{
    std::vector< argument_slot_t > slots;
    for (std::size_t i = 0; i < c.args.size(); ++i) {
        const auto &arg = c.args[i];

        if (i < f.params.size() && !(arg.type == *f.params[i]))
            throw std::logic_error("c1: an argument unlike its parameter");

        if (is< record_type_t >(arg.type)) {
            slots.push_back(list_copy(s, arg));
            continue;
        }

        const auto &type = integer(arg.type);
        const int displacement =
            allocate(s, size_of(arg.type), align_of(arg.type));
        initialize(s, arg, slot_address(displacement), type);

        slots.push_back({ .displacement = displacement, .type = type });
    }

    return slots;
}

// The first eight arguments into x0 to x7, chapter 8: each loaded from
// its slot, a narrow one extended by its load, and a va_list its
// copy's address, add x1, x29, #272.
void
load_arguments(state_t &s, const std::vector< argument_slot_t > &slots)
{
    const std::size_t registers = std::size(argument_registers);

    for (std::size_t i = 0; i < std::min(registers, slots.size()); ++i) {
        const auto &[displacement, type, by_reference] = slots[i];
        if (by_reference) {
            frame_address(s, reg(argument_registers[i], 64), displacement);
            continue;
        }

        const auto [mnemonic, width] = load_of(type, std::max(32, type.width));
        const auto from = slot_operand(s, displacement, type.width / 8);
        insn(s, mnemonic, { reg(argument_registers[i], width), from });
    }
}

// The callee of a call through a pointer, chapter 8: a hidden local of
// its call, as an argument is, an 8-byte slot taken before the callee
// is evaluated and written by the store row of 6.6.  Returns the slot's
// displacement.
int
callee_slot(state_t &s, const expr_t &callee)
{
    const int displacement = allocate(s, 8, 8);
    initialize(s, callee, slot_address(displacement), integer(callee.type));

    return displacement;
}

// The call through the callee's slot, after the argument registers are
// loaded: the slot loaded into x16, which the convention lets a call
// destroy (chapter 3), and blr x16.  A slot beyond the load's reach
// forms its address in x16 first, as any far slot does, 5.3, and x16 is
// loaded through it.
void
call_through(state_t &s, int displacement)
{
    const auto from = slot_operand(s, displacement, 8);

    insn(s, "ldr", { "x16", from });
    insn(s, "blr", { "x16" });
}

// Chapter 8, every argument a hidden local of its call.  Every held
// value is spilled; the callee of a call through a pointer is evaluated
// first, as the source writes it (G13), and written to its slot; each
// argument, in order and complete before the next, is written to a slot
// of its own, taken before it is evaluated, by the store row of 6.6; a
// va_list is copied to a slot of its own in its turn, list_copy(), and
// passes as that slot's address; what the arguments left held is
// spilled in its turn, backend.md 6.7. The arguments past the eighth
// are copied from their slots to the outgoing area, opened below the
// frame for this call alone, and the first eight are loaded into x0 to
// x7, a narrow one extended to 32 bits by its type's load.  G16: bl f,
// the name alone, since the linker makes a PLT entry where one is
// needed, or the call through the callee's slot, call_through(); a
// variadic callee gets nothing of its own (chapter 3).  No unwind
// directive is written around the area, since the canonical frame
// address is computed from x29.  The result is in x0, valid to its
// type's width and no further, 6.3: the convention leaves the bits
// above a narrow one unspecified, and the width rule extends it where
// it is used wider.
value_t
call(state_t &s, const call_t &c, const type_t &t)
{
    const auto *named = named_callee(c);
    const auto &f = callee_type(c);

    spill_all(s);

    const int target = named ? 0 : callee_slot(s, *c.callee);

    const auto slots = argument_slots(s, c, f);

    // the call destroys every expression register: nothing is held
    // from here to the bl
    spill_all(s);

    const std::size_t registers = std::size(argument_registers);
    const std::size_t stacked =
        slots.size() > registers ? slots.size() - registers : 0;
    const long area = stacked > 0 ? outgoing_size(stacked) : 0;

    for (const long part : frame_parts(area))
        insn(s, "sub", { "sp", "sp", imm(part) });

    for (std::size_t i = registers; i < slots.size(); ++i) {
        const auto &[displacement, type, by_reference] = slots[i];

        if (by_reference)
            frame_address(s, "x17", displacement);
        else
            copy_in(s, displacement, type.width);
        copy_out(s, outgoing_operand(s, i - registers, type.width / 8),
                 type.width);
    }

    load_arguments(s, slots);

    if (named)
        insn(s, "bl", { assembly_name(*named) });
    else
        call_through(s, target);

    for (const long part : frame_parts(area))
        insn(s, "add", { "sp", "sp", imm(part) });

    if (is< void_t >(t))
        return { };

    return hold(s, x0, integer(t).width);
}

// The value of an expression in a register, its code written first.
// An array is never a value: the parser decays it, 6.3.2.1p3, and a
// string literal is an array.  A register variable handed down, into,
// goes to the row at the top of e, whose last instruction writes the
// variable's register where it can (chapter 4): each path of &&, || and
// ?: writes it, a read of a register variable is no row, and a call's
// result and an assignment keep their own register.  wanted is the
// register the consumer of e's value names, x0 for a return's, as a set
// of one, or none (6.1): unary(), binary(), assign() and cast() hand it
// to the row's last instruction, and a row that writes a fresh register
// takes it as the lowest free one.  A conditional below a cast, a comma
// or an assignment is handed it where each arm takes it, takes_name():
// two arms whose places hold a call's result in x0 join in one other
// register with no move, where a join in x0 moves each, and under a cast
// that widens, the extension after the join reads that register where it
// is.  rvalue_in() hands the name to a conditional at the top of the
// value whatever its arms.
value_t
rvalue(state_t &s, const expr_t &e, into_t *into, unsigned wanted)
{
    if (is< array_t >(e.type))
        throw std::logic_error("c1: an array as a value");

    if (is< literal_t >(e))
        return literal(s, as< literal_t >(e), e.type, into);

    if (is< name_t >(e)) {
        if (const auto *sym = register_variable(e))
            return read(s, sym);

        const auto &t = integer(e.type);
        return load(s, address(s, e), t, std::max(32, t.width), into);
    }

    if (is< unary_t >(e))
        return unary(s, as< unary_t >(e), e.type, into, wanted);

    if (is< binary_t >(e))
        return binary(s, e, into, wanted);

    if (is< assign_t >(e))
        return assign(s, as< assign_t >(e), wanted);

    if (is< call_t >(e))
        return call(s, as< call_t >(e), e.type);

    if (is< cast_t >(e))
        return cast(s, as< cast_t >(e), e.type, into, wanted);

    const auto &k = as< conditional_t >(e);
    const bool arms = takes_name(*k.yes) && takes_name(*k.no);

    return conditional(s, e, into, arms ? wanted : 0);
}

// The value of e for a consumer that names the register r, a return's
// for x0 (backend.md 6.1): e's rows compute it in r where the last
// instruction can write it, a conditional at the top of e joins its
// arms in r, each arm's rows handed r, and the consumer moves the value
// where it is elsewhere (aarch64.md 6.1).
value_t
rvalue_in(state_t &s, const expr_t &e, reg_t r)
{
    if (is< conditional_t >(e))
        return conditional(s, e, nullptr, bit(r));

    return rvalue(s, e, nullptr, bit(r));
}

// Jump code, chapter 7.1: a jump to label when e is true (sense) or
// false (!sense), a fall through otherwise, and no value.  A comparison
// is cmp and b.cc; ! flips the sense; && and || are the shape of the
// short circuit, 6.5.14p4, 6.5.15p4, the right operand not reached when
// the left decides.  Any other scalar, a constant included, is its
// value and the test of zero.
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
            const auto c = ordered(b);
            const bool is_signed = integer(c.lhs->type).is_signed;
            release(s, compare(s, c));

            jump_if(s, sense ? c.op : inverse(c.op), is_signed, label);
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

        // a comma's right operand is the condition
        if (b.op == binary_op_t::comma) {
            effect(s, *b.lhs);
            cbranch(s, *b.rhs, label, sense);
            return;
        }
    }

    const auto v = rvalue(s, e);
    test_zero(s, v, integer(e.type), label, sense);
    release(s, v);
}

// ++ and -- for their effect: the load, add or sub of 1 and the store,
// chapter 6.6, in one register, since nothing reads the old value: the
// prefix form, its value discarded.  A register variable's prefix form
// is add w19, w19, #1 alone, and copies no read.
void
modify(state_t &s, const unary_t &u)
{
    const bool up = u.op == unary_op_t::pre_inc || u.op == unary_op_t::post_inc;
    const auto prefix = up ? unary_op_t::pre_inc : unary_op_t::pre_dec;

    release(s, step(s, prefix, *u.arg));
}

// the copy of a record

// The step of a register pointer a copy's place carries, 6.6: an add of
// the variable's own, add x19, x19, #8, its other reads still held
// copied first, as before any write of it (chapter 4).  A copy makes
// many accesses, so no one of them makes the step.
void
make_step(state_t &s, const step_t &step, value_t base)
{
    preserve(s, step.variable, base);

    const auto r = reg(variable_register(step.variable), 64);
    insn(s, "add", { r, r, imm(step.bytes) });
}

// A place's held index added to its base, the row of p + n, so that one
// register holds the address and the units are displacements off it,
// add x0, x0, x1, lsl #3, 6.6.
address_t
index_added(state_t &s, address_t a)
{
    if (a.index.id < 0)
        return a;

    const right_t n {
        .value = a.index, .width = 64, .extend = a.extend, .shift = a.shift
    };
    a.base = offset(s, binary_op_t::add, a.base, n, true);
    a.index = { };

    return a;
}

// A place as an address in a register of the copy's own, 6.6: a slot's
// or a label's address_of(), a base plus its displacement by the row of
// p + c, a row's page plus its label's offset, add x0, x0,
// :lo12:rows+2, and a register variable's read copied, since the loop
// steps the register.
value_t
owned_address(state_t &s, const address_t &a)
{
    if (a.base.id < 0)
        return address_of(s, a);

    if (!a.label.empty()) {
        const right_t lo12 {
            .immediate = ":lo12:" + symbol_plus(a.label, a.displacement)
        };
        return offset(s, binary_op_t::add, a.base, lo12, true);
    }

    if (a.displacement != 0)
        return offset(s, binary_op_t::add, a.base, product(s, a.displacement),
                      true);

    if (borrowed(s, a.base))
        copy(s, a.base, take(s, registers_of(s, a)));

    return a.base;
}

// Whether each unit of a copy, at the place's displacement and up, is
// an access of the place's own form, 2.3: a slot or a base in reach of
// each unit, a row's page, whose label's offset any unit carries, or a
// label of one or two units, each its page and its access through x16,
// two instructions where forming the address is two and a unit one.
bool
units_in_place(const address_t &a, long size, int unit)
{
    if (!a.label.empty())
        return a.base.id >= 0 || size / unit <= 2;

    for (long k = 0; k < size; k += unit)
        if (!in_reach(a.displacement + k, unit))
            return false;

    return true;
}

// A copy's place whose units are its displacement plus their offsets,
// 6.6: the index added first, then the place as it is where each unit
// reaches, else its address formed once, owned_address()'s.
address_t
unit_place(state_t &s, const address_t &a, long size, int unit)
{
    const auto at = index_added(s, a);
    if (units_in_place(at, size, unit))
        return at;

    return { .base = owned_address(s, at) };
}

// The copy unrolled, 6.6: each unit loaded into x17 by the plain load
// of its width and stored before the next is loaded, lowest address
// first, ldrh w17, [x0]; strh w17, [x1], each operand formed just
// before its access, so a slot or a label may go through x16 (5.3).
void
copy_units(
    state_t &s, const address_t &to, const address_t &from, long size, int unit)
{
    const int width = unit * 8;

    for (long k = 0; k < size; k += unit) {
        auto source = from;
        auto target = to;
        source.displacement += static_cast< int >(k);
        target.displacement += static_cast< int >(k);

        insn(s, plain_load_of(width),
             { reg(x17, width), operand(s, source, width) });
        insn(s, store_of(width),
             { reg(x17, width), operand(s, target, width) });
    }
}

// The copy of more than eight units, 6.6: the two addresses in
// registers of the copy's own and the count of units in x16, then a
// loop of four instructions whatever the size, each unit through x17
// by post-indexed accesses that step both addresses, lowest address
// first, and the count down to zero: ldr x17, [x1], #8; str x17, [x0],
// #8; subs x16, x16, #1; b.ne.  Both registers are taken before the
// loop's label, so nothing is reloaded inside it.
void
copy_loop(
    state_t &s, const address_t &to, const address_t &from, long size, int unit)
{
    const auto target = owned_address(s, index_added(s, to));
    const auto source = owned_address(s, index_added(s, from));
    const reg_t t = reg_of(s, target);
    const reg_t f = reg_of(s, source, bit(t));
    const int width = unit * 8;

    constant(s, x16, size / unit, 64);

    const int head = new_label(s);
    place_label(s, head);
    insn(s, plain_load_of(width),
         { reg(x17, width), std::format("[{}], #{}", reg(f, 64), unit) });
    insn(s, store_of(width),
         { reg(x17, width), std::format("[{}], #{}", reg(t, 64), unit) });
    insn(s, "subs", { "x16", "x16", imm(1) });
    insn(s, "b.ne", { label_name(s.function, head) });

    release(s, target);
    release(s, source);
}

// The object of record r at from copied to to, both places evaluated,
// in units of the type's alignment, padding included, no call, chapter
// 6.6.  A register pointer's step is its own add, before the copy for
// ++p and after it for p++.  Up to eight units the copy is unrolled,
// copy_units(); more are copy_loop()'s.
void
copy_object(state_t &s, address_t to, address_t from, const record_t &r)
{
    const step_t steps[] = { to.step, from.step };
    const value_t bases[] = { to.base, from.base };
    for (int k = 0; k < 2; ++k)
        if (steps[k].variable && steps[k].before)
            make_step(s, steps[k], bases[k]);

    to.step = { };
    from.step = { };

    const long size = static_cast< long >(r.size);
    const int unit = static_cast< int >(r.alignment);

    if (size / unit > 8)
        copy_loop(s, to, from, size, unit);
    else {
        const auto target = unit_place(s, to, size, unit);
        const auto source = unit_place(s, from, size, unit);
        copy_units(s, target, source, size, unit);
        release(s, target);
        release(s, source);
    }

    for (int k = 0; k < 2; ++k)
        if (steps[k].variable && !steps[k].before)
            make_step(s, steps[k], bases[k]);
}

// A struct or union assignment, 6.5.17.2p2, its value not wanted: the
// right operand's object copied into the left's, copy_object().  The
// left's place first, then the right's (G13), the left's held across
// the right's by the rule of a store's address, as a scalar
// assignment's is: one register where the right holds a call, and a
// constant index written after it, held_index()'s.
void
copy_record(state_t &s, const assign_t &a, const record_t &r)
{
    auto to = address(s, *a.lhs, a.rhs.get());
    auto from = address(s, *a.rhs);
    to = held_index(s, to, registers_of(s, from));

    copy_object(s, to, from, r);
}

// The effect context, chapter 6.6, Ritchie's efftab beside regtab and
// cctab: an expression evaluated for its side effects, its value not
// wanted.  The machine has no memory forms, so a compound assignment is
// its value context's load, operation and store, or its operation in
// place on a register variable, and the value is discarded.  A plain
// assignment is the lvalue's address, then the store row of an
// initializer through it, initialize(), which stores a zero from the
// zero register, str wzr, [x0]; to a register variable it is the mov
// into its register, as for its value.  The context reaches through the
// operators that discard a value: a conditional's arms, both operands
// of a comma, whose left one is in this context wherever the comma
// stands (6.5.18p2), and, by cast(), the operand of a cast to void;
// anything else is its value, discarded.
void
effect(state_t &s, const expr_t &e)
{
    if (is< assign_t >(e) && is< record_type_t >(e.type)) {
        copy_record(s, as< assign_t >(e), *as< record_type_t >(e.type).record);
        return;
    }

    if (is< assign_t >(e) && !as< assign_t >(e).op) {
        const auto &a = as< assign_t >(e);

        if (const auto *sym = register_variable(*a.lhs)) {
            release(s, assign_register(s, a, sym));
            return;
        }

        const auto place = address(s, *a.lhs, a.rhs.get());
        initialize(s, *a.rhs, place, integer(a.lhs->type));
        release(s, place);
        return;
    }

    if (is< unary_t >(e) && is_step(as< unary_t >(e).op)) {
        modify(s, as< unary_t >(e));
        return;
    }

    if (is< conditional_t >(e)) {
        conditional_effect(s, e);
        return;
    }

    if (is< binary_t >(e) && as< binary_t >(e).op == binary_op_t::comma) {
        effect(s, *as< binary_t >(e).lhs);
        effect(s, *as< binary_t >(e).rhs);
        return;
    }

    release(s, rvalue(s, e));
}

// switches

// A case's test in a switch's dispatch, 7.3: the comparison row of the
// held value and the case's constant, cmp w0, #5, then b.eq to its
// label.  The Seventh Edition compiler tested a 0 where it compared
// another value, tst for cmp; this machine's test of zero is cbz, one
// instruction, 7.1 and call 3 of chapter 15.
void
equal_jump(state_t &s, value_t v, const expr_t &constant, int label)
{
    const auto &t = integer(constant.type);
    if (normalized(as< literal_t >(constant).value, t.width) == 0) {
        test_zero(s, v, t, label);
        return;
    }

    compare_with(s, v, constant, t.width);
    jump_if(s, binary_op_t::eq, true, label);
}

// The jump through a switch's table by a held index of width bits, 7.3:
// the table's page and offset into x16, adrp and add, as a literal's
// address is formed; the entry loaded through it by the form that
// scales the index, and extends a 32-bit one unsigned, ldr x16, [x16,
// w0, uxtw #3], or reads a 64-bit one whole, [x16, x0, lsl #3]; then br
// x16.  The index is formed first, so a reload of it is done before x16
// holds anything (chapter 4).
void
table_jump(state_t &s, value_t index, int width, int table)
{
    const auto name = label_name(s.function, table);
    const reg_t r = reg_of(s, index);
    const auto extend = width == 64 ? extend_t::lsl : extend_t::uxtw;

    insn(s, "adrp", { "x16", name });
    insn(s, "add", { "x16", "x16", ":lo12:" + name });
    insn(s, "ldr",
         { "x16", std::format("[x16, {}]", index_operand(r, extend, 3)) });
    insn(s, "br", { "x16" });

    release(s, index);

    s.terminated = true;
}

// The division of a hash switch, 7.3: the table's size held
// in a register, as the divisor of x / 7 is, since udiv takes no
// immediate; the quotient by udiv into a register taken for it, and the
// remainder by msub into v's, or into a fresh one when v is a borrowed
// read, as divide() writes the remainder of %.  Both are kept and
// valid to 64 as unsigned values; v and the divisor are consumed.
std::pair< value_t, value_t >
divided(state_t &s, value_t v, const expr_t &tabs, int width)
{
    const int valid = written(width, false);

    const auto d = rvalue(s, tabs);
    const auto [l_reg, r_reg] = operands(s, v, d);
    const auto left = reg(l_reg, width);
    const auto right = reg(r_reg, width);

    const auto quotient = fresh(s, valid, bit(l_reg) | bit(r_reg));
    const auto q = reg(reg_of(s, quotient), width);
    insn(s, "udiv", { q, left, right });

    release(s, d);

    const auto remainder = reg(destination(s, v, valid), width);
    insn(s, "msub", { remainder, q, right, left });

    return { quotient, v };
}

// statements

// A register variable is in the one of the ten its index names,
// chapter 4.  A constant initializer is the constant row into
// the register, mov w19, #0, and any other is computed into the
// register, as an assignment's right operand is, by computed_into().
// G5: without one, nothing writes the value.  The register
// holds what the declaration finds in it: what the prologue left, where
// no variable of the function held the register before, and otherwise
// what the last variable to hold it left.  Every read before the first
// write reads that.  Where the representation claims more bits than the
// type has, for a char, a short or an unsigned, it is established on
// those contents by the extension of the register into itself, sxtb w19,
// w19 or mov w19, w19: the value of the type in the low bits is
// unchanged, and the register is the extension every read assumes.
void
register_decl(state_t &s, const decl_stmt_t &d)
{
    if (d.init && constant_into(s, d.sym, *d.init))
        return;

    if (d.init) {
        release(s, computed_into(s, d.sym, *d.init));
        return;
    }

    const auto &t = integer(d.sym->type);
    if (t.width < 32 || (t.width == 32 && !t.is_signed))
        establish(s, d.sym, variable_register(d.sym));
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
    if (in_register(*sym)) {
        register_decl(s, d);
        return;
    }

    const int displacement =
        allocate(s, size_of(sym->type), align_of(sym->type));
    s.slots.emplace(sym, displacement);

    if (!d.init)
        return;

    initialize(s, *d.init, slot_address(displacement), integer(sym->type));
}

#include "c1-statements.hh"

// functions

// The parameters' half of the prologue, before the body, since the
// body's slots come after theirs.  Each takes the next slot, in
// declaration order.  Each of the first eight is stored there from its
// argument register at its width, str w0, [x29, #16], 5.2; one past the
// eighth is copied there by stack_parameters(), once the frame's size
// says where the caller left it.  A register parameter is in the one of
// the ten its index names, the next in parameter order, chapter 4, and
// has no slot.  One of the first eight is moved into it from its
// argument register by establish(), which extends a narrow one, sxtb
// w19, w0: the convention leaves the bits above a narrow argument
// unspecified, and the argument register is caller-saved, so it would
// not survive the first call.  The moves and the stores are written in
// parameter order, after the save area.
void
parameters(state_t &s, const function_decl_t &fd)
{
    for (std::size_t i = 0; i < fd.params.size(); ++i) {
        const auto *p = fd.params[i];
        const auto &t = integer(p->type);

        if (in_register(*p)) {
            if (i < std::size(argument_registers))
                establish(s, p, argument_registers[i]);
            continue;
        }

        const int displacement =
            allocate(s, size_of(p->type), align_of(p->type));
        s.slots.emplace(p, displacement);

        if (i >= std::size(argument_registers))
            continue;

        const auto to = slot_operand(s, displacement, t.width / 8);
        insn(s, store_of(t.width), { reg(argument_registers[i], t.width), to });
    }
}

// The parameters past the eighth, copied to their slots, 5.2: the
// caller left the one of index i above the frame, at [x29, #T + 8(i -
// 8)], and T, the frame's size, is known only once the body is done.
// The copy is the plain load of the parameter's width, which reads a
// narrow one at its width, since the convention leaves the bits above it
// unspecified (chapter 3), and the store to its slot.  A register
// parameter is loaded straight into its register instead, by the load
// of its type, which extends a narrow one, ldrsb w19, [x29, #48], chapter
// 4.  A frame at its limit puts the third of them 16 MiB above x29,
// past the add's immediate, where slot_operand() holds the upper part
// in x16, 5.3.
void
stack_parameters(state_t &s, const function_decl_t &fd, long size)
{
    const std::size_t registers = std::size(argument_registers);

    for (std::size_t i = registers; i < fd.params.size(); ++i) {
        const auto *p = fd.params[i];
        const auto &t = integer(p->type);
        const int width = t.width;

        const long above = size + 8 * static_cast< long >(i - registers);

        if (in_register(*p)) {
            const auto [mnemonic, into] = load_of(t, register_width(p));
            const auto from =
                slot_operand(s, static_cast< int >(above), width / 8);
            insn(s, mnemonic, { reg(variable_register(p), into), from });
            continue;
        }

        copy_in(s, static_cast< int >(above), width);
        copy_out(s, slot_operand(s, slot_of(s, p), width / 8), width);
    }
}

// The area va_start reads, fd.va_area, of a variadic definition that
// calls it, 7.16.1.4, the convention's register save area, chapter 5.2:
// the 200 bytes of the parser's hidden local, written first after the
// prologue, before anything of the function's own, so that it holds the
// argument registers as they arrived: x0 to x7 from its start by four
// stp, the named parameters' among them, and q0 to q7 from 64 by four
// stp q, all of them whatever the caller passed, for a callee of
// another compiler's that reads a double from the list; then at 192 the
// address of the first argument the caller left on the stack past the
// named ones, x29 plus the frame's size and 8 for each named parameter
// past the eighth, formed in x16, add x16, x29, #size.  Its slot is
// aligned to 16 and is the function's first, so each stp reaches it.
void
fill_va_area(state_t &s, const function_decl_t &fd, long size)
{
    const int at = slot_of(s, fd.va_area);

    for (int k = 0; k < 8; k += 2)
        insn(s, "stp",
             { reg(argument_registers[k], 64),
               reg(argument_registers[k + 1], 64),
               std::format("[x29, #{}]", at + 8 * k) });

    for (int k = 0; k < 8; k += 2)
        insn(s, "stp",
             { std::format("q{}", k), std::format("q{}", k + 1),
               std::format("[x29, #{}]", at + 64 + 16 * k) });

    const long named = static_cast< long >(fd.params.size());
    const long past = std::max(0L, named - 8);

    frame_address(s, "x16", size + 8 * past);
    insn(s, "str", { "x16", std::format("[x29, #{}]", at + 192) });
}

// The displacement off x29 of the k-th register of the save area, 5.1:
// 8 bytes each, from just above the frame record.
int
saved_at(int k)
{
    return frame_record + 8 * k;
}

// Chapter 5.2: the frame opened by sub, the frame record stored at its
// bottom, x29 pointed at it.  Each sub is followed by the canonical frame
// address's offset from sp so far, and the record by the places of x29
// and x30 off the canonical frame address, which is sp before the sub.
// The save area follows, the registers the function uses stored from
// [x29, #16] in pairs by stp and an odd last one by str, each with its
// place off the canonical frame address.
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

    for (int k = 0; k < s.saved; k += 2) {
        const reg_t first = variable_registers[k];

        if (k + 1 == s.saved) {
            s.out << std::format("\tstr\t{}, [x29, #{}]\n"
                                 "\t.cfi_offset {}, {}\n",
                                 reg(first, 64), saved_at(k), reg(first, 32),
                                 saved_at(k) - size);
            continue;
        }

        const reg_t second = variable_registers[k + 1];
        s.out << std::format("\tstp\t{}, {}, [x29, #{}]\n"
                             "\t.cfi_offset {}, {}\n"
                             "\t.cfi_offset {}, {}\n",
                             reg(first, 64), reg(second, 64), saved_at(k),
                             reg(first, 32), saved_at(k) - size,
                             reg(second, 32), saved_at(k + 1) - size);
    }
}

// The one epilogue: the save area loaded, the frame record loaded, the
// frame closed by add.  The label is reached with sp equal to x29, so
// the canonical frame address moves to sp + size there, before the load
// gives x29 back to the caller.  Each register of the save area is
// loaded as the prologue stored it, and each load is followed by the
// note that the register holds the caller's value again; after the
// record's load x29 and x30 are in their registers again, and each add
// leaves the address at what is left of the frame above sp.
void
epilogue(state_t &s, long size)
{
    s.out << label_name(s.function, s.ret) << ":\n"
          << std::format("\t.cfi_def_cfa wsp, {}\n", size);

    for (int k = 0; k < s.saved; k += 2) {
        const reg_t first = variable_registers[k];

        if (k + 1 == s.saved) {
            s.out << std::format("\tldr\t{}, [x29, #{}]\n"
                                 "\t.cfi_restore {}\n",
                                 reg(first, 64), saved_at(k), reg(first, 32));
            continue;
        }

        const reg_t second = variable_registers[k + 1];
        s.out << std::format("\tldp\t{}, {}, [x29, #{}]\n"
                             "\t.cfi_restore {}\n"
                             "\t.cfi_restore {}\n",
                             reg(first, 64), reg(second, 64), saved_at(k),
                             reg(first, 32), reg(second, 32));
    }

    s.out << "\tldp\tx29, x30, [sp]\n"
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

// A definition.  The registers the save area holds are the parser's
// count, the registers the function's variables take, chapter 5.1,
// known before the body, since every slot lies above them; the body is
// generated next, into the buffer, and the prologue written after it,
// when the frame's size is known; so are the copies of the parameters
// past the eighth, which follow the prologue and precede the body.  The
// first slot lies above the frame record and the save area, and this is
// the one place that says so.  The one epilogue is reached with x0 as
// it is, G5's some value, but for main, which returns 0 when it falls
// off its end, 5.1.2.3.4.
void
function(state_t &s, const function_decl_t &fd)
{
    s.body.str({ });
    s.slots.clear();
    s.owner.fill(-1);
    s.values.clear();
    s.age = 0;
    s.live = 0;
    s.saved = fd.registers;
    s.in_place = -1;
    s.frame = saved_at(s.saved);
    s.function = fd.sym->name;
    s.labels = 0;
    s.ret = new_label(s);
    s.terminated = false;
    s.jump_targets.clear();
    s.user_labels.clear();
    s.case_labels.clear();

    if (fd.va_area)
        s.slots.emplace(fd.va_area, allocate(s, size_of(fd.va_area->type), 16));

    parameters(s, fd);
    block_stmt(s, *fd.body);

    if (!s.terminated && s.function == "main")
        insn(s, "mov", { "w0", imm(0) });

    const long size = (s.frame + 15) / 16 * 16;

    const auto body = s.body.str();
    s.body.str({ });
    if (fd.va_area)
        fill_va_area(s, fd, size);
    stack_parameters(s, fd, size);

    const auto name = assembly_name(*fd.sym);
    binding(s.out, *fd.sym);
    s.out << std::format("\t.type\t{}, @function\n"
                         "{}:\n"
                         "\t.cfi_startproc\n",
                         name, name);

    prologue(s, size);

    s.out << s.body.str() << body;

    epilogue(s, size);
    s.out << std::format("\t.cfi_endproc\n"
                         "\t.size\t{}, .-{}\n",
                         name, name);
}

} // namespace

// The functions the unit defines, in order; one it only declares
// writes nothing, since the assembler leaves its symbol to the link.
// The string literals follow the last function, and the objects the
// unit defines follow them, chapter 10.  The note section marks the
// stack not executable.
void
generate_aarch64(const unit_t &unit,
                 const target_t &target,
                 std::string_view file,
                 std::ostream &out)
{
    state_t s { .out = out,
                .frame_limit = target.frame_limit,
                .defined = defined_symbols(unit) };

    out << std::format("\t.file\t{}\n\t.text\n", quoted_file(file));

    for (const auto &fd : unit.functions) {
        if (!fd.body)
            continue;

        try {
            function(s, fd);
        }
        catch (const refusal_t &e) {
            refused_in(fd.sym->name, e);
        }
    }

    data(s.out, s.strings, unit.objects, target);

    out << "\t.section\t.note.GNU-stack,\"\",@progbits\n";
}

} // namespace rcc
