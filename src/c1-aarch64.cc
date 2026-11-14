// -*- mode: c++; -*-

// c1, the code generator for aarch64: the typed tree in, assembly text
// out, for clang and GNU as.  docs/aarch64.md is the design, with
// docs/backend.md where it says the model carries; this file implements
// its chapters 4 to 8 and 10, with c1-common.cc and c1-stack.hh, and
// each function names the row it writes.  It holds slices 1 to 3: the
// frame and straight-line code on integers, conditions and control, and
// calls; the address of an object, the indirection through a pointer,
// the use of an array and a string literal, pointer arithmetic and the
// subscript; the register variable: the save area, the binding, the
// borrowed read, plain assignment, the declaration, the register
// parameter, the held values of backend.md 6.7, the compound
// assignment, the ++ and the -- in place, and the access that steps a
// register pointer.  A construct of a later slice throws
// logic_error("c1: not yet: ..."), and a tree the parser cannot make
// throws logic_error("c1: ...").

#include "c1.hh"
#include "c1-common.hh"

#include <algorithm>
#include <array>
#include <bit>
#include <format>
#include <initializer_list>
#include <optional>
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

// The ten register-variable registers, chapter 4: the callee-saved set
// but the frame pointer, in the order the variables take them, x19
// first.  The state keeps no owner for them, since a read of a register
// variable is borrowed, never owned.
constexpr reg_t variable_registers[] = { x19, x20, x21, x22, x23,
                                         x24, x25, x26, x27, x28 };

// The convention's argument registers, in order, chapter 3.
constexpr reg_t argument_registers[] = { x0, x1, x2, x3, x4, x5, x6, x7 };

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

    // each register variable's register; how many are live, the most
    // live at once, and the registers the save area holds, which the
    // body's first slot lies above
    std::unordered_map< const symbol_t *, reg_t > registers { };
    int variables = 0;
    int used = 0;
    int saved = 0;

    // the read of a compound assignment's register variable, which its
    // operation writes in place
    int in_place = -1;

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

// Labels are placed in source order, and nothing is reordered after
// them, backend.md 7.2.  No alignment directive precedes a loop's head:
// nothing is known of this target's processors until it is measured on
// one (7.2, call 7).
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

#include "c1-stack.hh"

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

value_t rvalue(state_t &, const expr_t &);
void cbranch(state_t &, const expr_t &, int label, bool sense);
void effect(state_t &, const expr_t &);

// register variables

// The next of variable_registers for a register variable met in
// declaration order, backend.md chapter 4; a block's end gives its
// variables' registers back, so a later block takes them again from the
// first.  The parser gives one live past the last of them a stack slot.
void
bind(state_t &s, const symbol_t *sym)
{
    if (s.variables == static_cast< int >(std::size(variable_registers)))
        throw std::logic_error("c1: an eleventh register variable");

    s.registers.insert_or_assign(sym, variable_registers[s.variables++]);
    s.used = std::max(s.used, s.variables);
}

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
// return's mov into x0 is the third copy chapter 4 keeps.
reg_t
destination(state_t &s, value_t v, unsigned avoid = 0)
{
    if (!borrowed(s, v) || v.id == s.in_place)
        return reg_of(s, v, avoid);

    check_movable(s, v.id);

    const reg_t r = take(s, avoid);

    auto &h = s.values[v.id];
    h.reg = r;
    h.variable = nullptr;
    s.owner[r] = v.id;

    return r;
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
// extended into a fresh register, sxtw x0, w19, by destination(), 6.1.
value_t
extended(state_t &s, value_t v, const int_t &from, int to)
{
    if (s.values[v.id].valid >= to)
        return v;

    const auto [mnemonic, valid] = extension(from, std::max(32, to));
    const reg_t source = reg_of(s, v);
    const reg_t r = destination(s, v);
    const int width = from.is_signed ? valid : 32;

    insn(s, mnemonic, { reg(r, width), reg(source, 32) });
    s.values[v.id].valid = valid;

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

    insn(s, mnemonic,
         { reg(variable_register(s, sym), width), reg(from, width) });
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
// form, [x0, w1, sxtw #2].  A string literal's address is its label,
// which no access takes as an operand: the literal is an array and is
// used only by its address as a value, chapter 10.  A register pointer
// stepped by a constant the form encodes is its register as the base and
// the step, which the access makes, [x19], #1 or [x19, #1]!.  The access
// consumes the values.
struct address_t
{
    value_t base { };
    value_t index { };
    extend_t extend = extend_t::lsl;
    int shift = 0;
    int displacement = 0;
    std::string label { };
    step_t step { };
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
// spilled base or index is reloaded first, outside avoid and outside the
// other's register; the reload is complete before the access, so x16
// holds nothing across it.  A step is stepped_operand()'s.
std::string
operand(state_t &s, const address_t &a, int width, unsigned avoid = 0)
{
    if (!a.label.empty())
        throw std::logic_error("c1: a string literal as an access's operand");

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

// The address of a slot or of a string literal as a value, valid to
// 64.  A literal's label is formed PC-relative in two instructions,
// adrp L, .Ls0 for its 4 KiB page and add L, L, :lo12:.Ls0 for the
// offset in it, chapter 3.  A slot is add L, x29, #d; a displacement
// add's 12 bits do not encode is the two adds of 5.3, the upper part
// shifted first, and one when the lower part is zero.  The sum is
// formed in L, so x16 is not used.  A dereference's address is never
// here: address_value() takes it as the pointer's value.
value_t
address_of(state_t &s, const address_t &a)
{
    if (a.base.id >= 0)
        throw std::logic_error("c1: a held base taken as a value");

    const auto v = fresh(s, 64);
    const auto r = reg(reg_of(s, v), 64);

    if (!a.label.empty()) {
        insn(s, "adrp", { r, a.label });
        insn(s, "add", { r, r, std::format(":lo12:{}", a.label) });
        return v;
    }

    const int upper = a.displacement & ~0xfff;
    const int lower = a.displacement & 0xfff;

    if (upper == 0) {
        insn(s, "add", { r, "x29", imm(lower) });
        return v;
    }

    insn(s, "add", { r, "x29", imm(upper) });
    if (lower != 0)
        insn(s, "add", { r, r, imm(lower) });

    return v;
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

// The address an access through a dereference designates, 6.5.4.2p4,
// for a load, a fetch or a store: the pointer's value as the base, [x0],
// whatever computed it and however it is aligned (G8, G11).  An add of
// a pointer and an integer, the shape of a subscript (6.5.3.2p2), is the
// machine's addressing form: the pointer the base, the integer the
// index, each generated in the order written (G13), the index by
// index_of(), and the scale the size of the pointee, a scalar of 1, 2,
// 4 or 8 bytes, since an access reads no array.  A constant index is
// the displacement c * s when the access encodes it, 2.3; any other is
// an index like a variable one, held whole, lsl.  Any other pointer is
// computed into a register and is the base; nothing looks through a
// cast of the pointer.  The pointer is evaluated here, before anything
// the access goes with.
//
// A register pointer stepped by a constant the form encodes is the
// first case: its register is the base, a borrowed read, and the access
// makes the step, 6.3.  The step then moves from here to the
// access, past between, the operand the caller generates between the
// two, a store's right operand; so the case is not taken when between
// names the variable, and the step is written here as before, G13.
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

    // *p, and *E for any E that is no sum: *f(), *(char *) q, *p++ on
    // a pointer in a slot, *p++ = *p, and a difference, *(p - 1).  E's
    // value is the base, [x0].
    if (!is< binary_t >(p) || as< binary_t >(p).op != binary_op_t::add)
        return { .base = rvalue(s, p) };

    // From here a sum of a pointer and an integer: p[n], n[p], *(p +
    // n) and *(n + p), which are one tree.  Either operand may be the
    // pointer; the other is n.
    const auto &b = as< binary_t >(p);
    const bool left = is< pointer_t >(b.lhs->type);
    const auto &pointer = left ? *b.lhs : *b.rhs;
    const auto &n = left ? *b.rhs : *b.lhs;
    const long size = stride(p.type);

    // p[3], 3[p], p[-1]: a constant n whose product with the size the
    // access encodes is the displacement, [x0, #12], and n generates
    // nothing.  One it does not encode, p[5000], goes on as an index.
    if (is< literal_t >(n)) {
        const auto d = scaled(as< literal_t >(n).value, size);
        if (in_reach(d, static_cast< int >(size)))
            return { .base = rvalue(s, pointer),
                     .displacement = static_cast< int >(d) };
    }

    // p[i], *(p + i): the pointer is written first, so the base is
    // generated first and the index after it (G13), [x0, w1, sxtw #2].
    if (left) {
        const auto base = rvalue(s, pointer);
        const auto index = index_of(s, n);
        return { .base = base,
                 .index = index.value,
                 .extend = index.extend,
                 .shift = shift_of(size) };
    }

    // i[p], *(i + p): the integer is written first, so the index is
    // generated first.  The operand is the same, [x1, w0, sxtw #2].
    const auto index = index_of(s, n);
    const auto base = rvalue(s, pointer);

    return { .base = base,
             .index = index.value,
             .extend = index.extend,
             .shift = shift_of(size) };
}

// The address of an lvalue: a local's or a parameter's slot, a string
// literal's label, or what a dereference designates.  It serves loads
// and stores alike, so the addressing row is written once.  between is
// the operand the caller generates after the address and before its
// access, an assignment's right operand, or null, for pointed().
address_t
address(state_t &s, const expr_t &e, const expr_t *between = nullptr)
{
    if (is< string_t >(e))
        return { .label = string_label(s.strings, as< string_t >(e)) };

    if (is< unary_t >(e) && as< unary_t >(e).op == unary_op_t::deref)
        return pointed(s, *as< unary_t >(e).arg, between);

    return slot_address(slot_of(s, object_of(e)));
}

// The address of an lvalue as a value: &x, and an array's decay,
// 6.3.2.1p3.  The address of *E is taken and no access made, 6.5.4.2p3:
// "neither that operator nor the & operator is evaluated".  A bare base
// is its own value and no instruction: &*p is p, and so is &p[0], whose
// access is [x0] as *p's is.  Any other subscript's address is the row
// of p + n, the machine having no lea, so &p[i] and &p[3] are the add;
// a row of an array of arrays, m[i], decays the same way, and m[0] to
// m.  Anything else is a slot or a string literal, whose address
// address_of() forms.
value_t
address_value(state_t &s, const expr_t &e)
{
    if (!is< unary_t >(e) || as< unary_t >(e).op != unary_op_t::deref)
        return address_of(s, address(s, e));

    const auto &p = *as< unary_t >(e).arg;
    if (const auto *base = bare_base(p))
        return rvalue(s, *base);

    return rvalue(s, p);
}

// The load row, 6.3.2.1p2: the value at an address into a register at
// a width of 32 or 64, a char or a short by the extending load of its
// signedness, ldrsb w0, [x29, #16].  G8, G10: the load of 2.2 at the
// lvalue's width and extension, whatever computed the address.  A held
// base and index are consumed first, so the value may take one of their
// registers, ldr w0, [x0], ldr w0, [x0, w1, sxtw #2].  For a slot the
// register is taken first, since the take may spill through x16, and
// the slot's operand is formed after it.
value_t
load(state_t &s, const address_t &a, const int_t &t, int width)
{
    const auto [mnemonic, into] = load_of(t, width);

    if (a.base.id >= 0) {
        const auto from = operand(s, a, t.width);
        release(s, a);

        const auto v = fresh(s, written(into, t.is_signed));
        insn(s, mnemonic, { reg(reg_of(s, v), into), from });

        return v;
    }

    const auto v = fresh(s, written(into, t.is_signed));
    const reg_t r = reg_of(s, v);

    const auto from = operand(s, a, t.width);
    insn(s, mnemonic, { reg(r, into), from });

    return v;
}

// The load row of a read-modify-write, a compound assignment or a ++
// or --: the address stays held for the store through it, since E1 is
// evaluated once, 6.5.17.3p4.  The value's register is taken outside
// the address's, and the operand formed after the take.
value_t
fetch(state_t &s, const address_t &a, const int_t &t, int width)
{
    const auto [mnemonic, into] = load_of(t, width);

    const auto v = fresh(s, written(into, t.is_signed), registers_of(s, a));
    const reg_t r = reg_of(s, v);

    const auto from = operand(s, a, t.width, bit(r));
    insn(s, mnemonic, { reg(r, into), from });

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

    constant(s, variable_register(s, sym), value, width);

    return true;
}

// The store row of an initializer, chapter 6.6: e written into the
// object at to, at the object's width t, its value not wanted.  A zero
// constant is the store of the zero register, str wzr, [x29, #16], and
// no register is held; anything else, any other constant included, is
// its value in a register and the store, since str takes no immediate.
// to is a slot, which has no evaluation, or an address the caller
// evaluated before e, so G13 holds; the caller consumes it.
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
// w19.
value_t
divide(state_t &s, binary_op_t op, value_t l, value_t r, const int_t &t)
{
    const int width = t.width;
    const auto mnemonic = t.is_signed ? "sdiv" : "udiv";

    const auto [l_reg, r_reg] = operands(s, l, r);
    const auto left = reg(l_reg, width);
    const auto right = reg(r_reg, width);

    if (op == binary_op_t::div) {
        release(s, r);

        const auto result = reg(destination(s, l), width);
        insn(s, mnemonic, { result, left, right });
    }
    else {
        const reg_t n = take(s, bit(l_reg) | bit(r_reg));

        const auto quotient = reg(n, width);
        insn(s, mnemonic, { quotient, left, right });

        release(s, r);

        const auto result = reg(destination(s, l), width);
        insn(s, "msub", { result, quotient, right, left });
    }

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
        const auto to = reg(destination(s, l), width);
        insn(s, mnemonic, { to, left, r.immediate });
        wrote(s, l, width, t.is_signed);
        return l;
    }

    const auto [l_reg, r_reg] = operands(s, l, r.value);
    release(s, r.value);

    const auto to = reg(destination(s, l), width);
    insn(s, mnemonic, { to, reg(l_reg, width), reg(r_reg, width) });
    wrote(s, l, width, t.is_signed);

    return l;
}

// An arithmetic, bitwise or shift operator on the held left value and
// the generated right operand, in type t, chapter 6.1: op L, L, R, the
// result in L, or op L, L, #c for an immediate.  A borrowed read as L
// writes into a fresh register, op N, L, R, by destination(), unless it
// is the variable a compound assignment writes in place, add w19, w19,
// w0; as R it is the operand as it is.  G1: add, sub, mul and lsl wrap;
// mul is the same instruction for unsigned, the low bits agree.
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
        const auto to = reg(destination(s, l), width);
        insn(s, mnemonic, { to, left, r.immediate });
        wrote(s, l, width, integer(t).is_signed);
        return l;
    }

    const auto [l_reg, r_reg] = operands(s, l, r.value);
    release(s, r.value);

    const auto to = reg(destination(s, l), width);
    insn(s, mnemonic, { to, reg(l_reg, width), reg(r_reg, width) });
    wrote(s, l, width, integer(t).is_signed);

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

// The n of p + n, n + p and p - n, and of p += n and p -= n, generated
// as the row takes it, 6.3.  A constant is its product with the size.
// For a size of 1, 2, 4 or 8 any other n is the index of index_of(),
// read by the shifted or extended operand of add and sub; for any other
// size it is generated whole, the cast writing its extension, and the
// row multiplies it by the size.
right_t
displacement(state_t &s, const expr_t &n, long size)
{
    if (is< literal_t >(n))
        return product(s, scaled(as< literal_t >(n).value, size));

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
// product of a factor, which is n's, never lands in it.
value_t
offset(state_t &s,
       binary_op_t op,
       value_t pointer,
       const right_t &n,
       bool pointer_first)
{
    const auto mnemonic = op == binary_op_t::add ? "add" : "sub";

    if (!n.immediate.empty()) {
        const auto from = reg(reg_of(s, pointer), 64);
        const auto to = reg(destination(s, pointer), 64);
        insn(s, mnemonic, { to, from, n.immediate });
        wrote(s, pointer, 64, false);
        return pointer;
    }

    const auto [first, second] = pointer_first ? operands(s, pointer, n.value) :
                                                 operands(s, n.value, pointer);
    const reg_t p = pointer_first ? first : second;
    reg_t i = pointer_first ? second : first;

    if (n.factor != 1) {
        const reg_t factor = take(s, bit(p) | bit(i));
        constant(s, factor, n.factor, 64);

        const reg_t product = destination(s, n.value, bit(p) | bit(factor));
        insn(s, "mul", { reg(product, 64), reg(i, 64), reg(factor, 64) });
        i = product;
    }

    const auto index = index_operand(i, n.extend, n.shift);
    const bool plain = n.extend == extend_t::lsl && n.shift == 0;

    const auto result = pointer_first ? pointer : n.value;
    release(s, pointer_first ? n.value : pointer);

    const auto to = reg(destination(s, result), 64);

    if (pointer_first)
        insn(s, mnemonic, { to, reg(p, 64), index });
    else if (plain)
        insn(s, mnemonic, { to, reg(i, 64), reg(p, 64) });
    else
        insn(s, mnemonic, { to, reg(p, 64), index });

    wrote(s, result, 64, false);

    return result;
}

// A binary + or - of the pointer type t, 6.5.7p9: the pointer and the
// integer, the integer widened to 64 bits by the parser, each generated
// in the order written (G13), a constant as its product with the size,
// whose immediate takes no register.
value_t
pointer_arithmetic(state_t &s, const binary_t &b, const type_t &t)
{
    const long size = stride(t);

    if (is< pointer_t >(b.lhs->type)) {
        const auto l = rvalue(s, *b.lhs);
        return offset(s, b.op, l, displacement(s, *b.rhs, size), true);
    }

    const auto n = displacement(s, *b.lhs, size);
    const auto p = rvalue(s, *b.rhs);

    return offset(s, b.op, p, n, false);
}

// p - q, 6.5.7p10: the difference of the addresses divided by the size,
// a long.  G6: a division, sub L, L, R and then mov R, #s; sdiv L, L,
// R, the size held, since sdiv takes no immediate; never a shift, since
// the two pointers are any two addresses.  A size of 1 is the sub
// alone.
value_t
difference(state_t &s, const binary_t &b)
{
    const long size = stride(b.lhs->type);

    const auto l = rvalue(s, *b.lhs);
    const auto r = rvalue(s, *b.rhs);
    const auto [left, right] = operands(s, l, r);
    release(s, r);

    const auto to = reg(destination(s, l), 64);
    insn(s, "sub", { to, reg(left, 64), reg(right, 64) });
    wrote(s, l, 64, true);

    if (size == 1)
        return l;

    const auto d = fresh(s, 64);
    constant(s, reg_of(s, d), size, 64);

    return divide(s, binary_op_t::div, l, d, int_t { 64, true });
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

// cmp L, R for a relational or equality operator, left operand first
// (G13), or cmp L, #c for a constant cmp encodes, the immediate of add
// and sub, 2.4.  A negative constant is written as the source has it,
// cmp w0, #-1, which the assembler encodes as cmn w0, #1, the same
// flags, 6.4.  The flags are set and the right value freed; the left
// one stays held, for cset or for the caller to free.
value_t
compare(state_t &s, const binary_t &b)
{
    const int width = operation_width(b.lhs->type);

    const auto l = rvalue(s, *b.lhs);
    const auto r = right_operand(s, b.op, *b.rhs, width);

    if (!r.immediate.empty()) {
        insn(s, "cmp", { reg(reg_of(s, l), width), r.immediate });
        return l;
    }

    const auto [left, right] = operands(s, l, r.value);
    insn(s, "cmp", { reg(left, width), reg(right, width) });

    release(s, r.value);

    return l;
}

// A comparison as a value, the row of 6.3: cmp, then cset of the
// condition into L's w name, the int the operator yields, in one
// instruction where x86 needs two.  A borrowed read as L is only read by
// the cmp, and the cset takes a fresh register, by destination().
value_t
comparison(state_t &s, const binary_t &b)
{
    const bool is_signed = integer(b.lhs->type).is_signed;

    const auto l = compare(s, b);
    insn(s, "cset", { reg(destination(s, l), 32), condition(b.op, is_signed) });
    wrote(s, l, 32, true);

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

// &&, || and ?: as a value: jump code into one register on both paths,
// mov L, #1 and mov L, #0 on the arms of the logical operators, never
// csel, 6.3.  before_paths() first gives every held value one location
// on both, so the paths leave the register map as they found it
// whatever their operands do.
value_t
logical(state_t &s, const expr_t &e)
{
    const auto incoming = before_paths(s, e);

    const int no = new_label(s);
    const int end = new_label(s);

    cbranch(s, e, no, false);

    const auto v = fresh(s, 32);
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

// 6.5.16p5: the arm not chosen is not evaluated, and the jump says so;
// never csel.  Each arm's value is moved into the register of the first
// arm's, by mov at the operation's width, backend.md 7.2.  That register
// is an expression register: a borrowed read as the first arm is copied
// into one, mov w0, w19, chapter 4.
value_t
conditional(state_t &s, const expr_t &e)
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

    cbranch(s, *c.cond, no, false);

    const int width = std::max(32, integer(t).width);

    const auto yes = rvalue(s, *c.yes);
    if (borrowed(s, yes))
        copy(s, yes, take(s, 0));

    const reg_t into = reg_of(s, yes);
    const int valid = s.values[yes.id].valid;
    release(s, yes);
    jump(s, end);

    place_label(s, no);
    const auto other = rvalue(s, *c.no);
    const reg_t from = reg_of(s, other);
    int other_valid = s.values[other.id].valid;
    if (from != into) {
        insn(s, "mov", { reg(into, width), reg(from, width) });
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
    // 6.5.18p2: the left operand is evaluated as a void expression,
    // the effect context of chapter 6.6
    case binary_op_t::comma: {
        effect(s, *b.lhs);
        return rvalue(s, *b.rhs);
    }
    default: break;
    }

    if (is_comparison(b.op))
        return comparison(s, b);

    if (is< pointer_t >(e.type))
        return pointer_arithmetic(s, b, e.type);

    if (is< pointer_t >(b.lhs->type))
        return difference(s, b);

    // G13: the left operand first, held while the right one is generated
    const auto l = rvalue(s, *b.lhs);
    const auto r = right_operand(s, b.op, *b.rhs, operation_width(e.type));

    return arithmetic(s, b.op, l, r, e.type);
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
step_register(state_t &s, unary_op_t op, const symbol_t *sym)
{
    const auto &t = integer(sym->type);
    const int width = register_width(sym);
    const long amount = is< pointer_t >(sym->type) ? stride(sym->type) : 1;

    const bool up = op == unary_op_t::pre_inc || op == unary_op_t::post_inc;
    const bool post = op == unary_op_t::post_inc || op == unary_op_t::post_dec;

    const auto old = post ? read(s, sym) : value_t { };
    preserve(s, sym);

    const auto r = reg(variable_register(s, sym), width);

    const auto n = product(s, amount);
    const auto by =
        n.immediate.empty() ? reg(reg_of(s, n.value), width) : n.immediate;
    insn(s, up ? "add" : "sub", { r, r, by });
    release(s, n.value);

    if (t.width < 32)
        establish(s, sym, variable_register(s, sym));

    return post ? old : read(s, sym);
}

// ++ and --, either side: the address once, the load, add or sub of 1,
// or of the size for a pointer (6.5.4.1p2, 6.5.3.5p2), the store
// through the same address, held between the two; G1, the machine
// wraps.  The size is the immediate where add and sub encode it, and
// otherwise held in a register and freed, as the product of p + n is.
// A char or a short is loaded extended to 32 bits and its low bytes
// stored, so no operator sees 8 or 16 bits.  The prefix forms operate
// on the loaded value's register and yield it.  The postfix forms name
// another register as the destination, add w1, w0, #1, and yield the
// old value where the load left it: nothing is copied, 6.1.  A register
// variable's is step_register()'s.
value_t
step(state_t &s, unary_op_t op, const expr_t &arg)
{
    if (const auto *sym = register_variable(arg))
        return step_register(s, op, sym);

    const auto &t = integer(arg.type);
    const int width = std::max(32, t.width);
    const long amount = is< pointer_t >(arg.type) ? stride(arg.type) : 1;

    const bool up = op == unary_op_t::pre_inc || op == unary_op_t::post_inc;
    const bool post = op == unary_op_t::post_inc || op == unary_op_t::post_dec;

    const auto place = address(s, arg);
    const auto l = fetch(s, first_access(place), t, width);
    const reg_t r = reg_of(s, l);

    const auto v = post ? fresh(s, written(width, t.is_signed),
                                bit(r) | registers_of(s, place)) :
                          l;
    const reg_t into = reg_of(s, v);

    const auto n =
        product(s, amount, bit(r) | bit(into) | registers_of(s, place));
    const auto by =
        n.immediate.empty() ? reg(reg_of(s, n.value), width) : n.immediate;
    insn(s, up ? "add" : "sub", { reg(into, width), reg(r, width), by });
    release(s, n.value);

    store(s, v, last_access(place), t.width);
    release(s, place);

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
    // G1: neg of INT_MIN is INT_MIN.  A borrowed read writes into a
    // fresh register, neg w0, w19, by destination().
    case unary_op_t::neg:
    case unary_op_t::bit_not: {
        const auto &type = integer(t);
        const int width = operation_width(t);

        const auto v = rvalue(s, *u.arg);
        const auto left = reg(reg_of(s, v), width);
        const auto to = reg(destination(s, v), width);
        insn(s, u.op == unary_op_t::neg ? "neg" : "mvn", { to, left });
        wrote(s, v, width, type.is_signed);

        return v;
    }

    // as a value, the row of 6.3: the test of zero, then cset L, eq.
    // The operand is not promoted, so it is tested at its own width.  A
    // borrowed read is only tested, and the cset takes a fresh register.
    case unary_op_t::log_not: {
        const auto v = rvalue(s, *u.arg);
        test_zero(s, v, integer(u.arg->type), std::nullopt);

        insn(s, "cset", { reg(destination(s, v), 32), "eq" });
        wrote(s, v, 32, true);

        return v;
    }

    case unary_op_t::pre_inc:
    case unary_op_t::pre_dec:
    case unary_op_t::post_inc:
    case unary_op_t::post_dec: return step(s, u.op, *u.arg);

    case unary_op_t::addr: return address_value(s, *u.arg);

    // the load row from [L], at the object's width and extension
    case unary_op_t::deref: {
        const auto &type = integer(t);
        return load(s, pointed(s, *u.arg), type, std::max(32, type.width));
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
        establish(s, sym, variable_register(s, sym));
}

// Assignment to a register variable, chapter 4, after preserve() has
// copied every read of it an enclosing expression still holds, x + (x
// = e).  Plain: a constant is the constant row into the variable's
// register, mov w20, #0, and the value is a read of the variable;
// anything else is the right operand, then the mov of establish() into
// the register, mov w20, w0, or mov w19, w20 from another register
// variable, and the value stays the right operand's, valid to what it
// left, as for a slot.  Compound: the right operand first, then the
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

        const auto v = rvalue(s, *a.rhs);
        establish(s, sym, reg_of(s, v));

        return v;
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

// Plain or compound assignment to an object in memory; its value is
// the value stored, the low bytes of L.  The lvalue's address comes
// first (G13) and is held across the right operand.  Plain: the right
// operand, then the store.  Compound: the right operand, then, as one
// piece and nothing evaluated between, the load of the object through
// the held address, extended into the common type the parser chose, the
// operation, and the store of the result's low bytes back through the
// same address (G2), so the object is read after the right operand.
// 6.5.17.3p4: "with respect to an indeterminately sequenced function
// call, the operation of a compound assignment is a single evaluation."
// p += n and p -= n are the row of p + n, n its right operand.  An
// assignment to a register variable is assign_register()'s.
value_t
assign(state_t &s, const assign_t &a)
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

    const auto place = address(s, *a.lhs, a.rhs.get());

    if (!a.op) {
        const auto v = rvalue(s, *a.rhs);
        store(s, v, place, t.width);
        release(s, place);
        return v;
    }

    const int width = operation_width(a.common);
    const auto r = stepped ? displacement(s, *a.rhs, stride(a.lhs->type)) :
                             right_operand(s, *a.op, *a.rhs, width);

    // the value of the expression has the lvalue's type: the bits
    // above its width are the operation's, not the value's extension
    const auto l = fetch(s, first_access(place), t, width);
    const auto v = stepped ? offset(s, *a.op, l, r, true) :
                             arithmetic(s, *a.op, l, r, a.common);
    store(s, v, last_access(place), t.width);
    release(s, place);
    s.values[v.id].valid = std::min(s.values[v.id].valid, t.width);

    return v;
}

// An integer conversion, 6.3.  G2: narrowing is no instruction, the
// narrower register name is the value; widening extends by the
// operand's signedness, and only when the register is not already
// valid to the new width, as it is after the load of a char or a 32-bit
// write of an unsigned value.  A change of signedness alone is nothing.
// A pointer is its integer view, so pointer to pointer is nothing, an
// integer widens to a pointer by its own signedness (6.3.2.3p5), sxtw
// for an int, and a pointer narrows to an integer by its low bits (p6).
// An array decays to its address, 6.3.2.1p3, a string literal's
// included.  A cast to void evaluates its operand for its effect alone,
// chapter 6.6.
value_t
cast(state_t &s, const cast_t &c, const type_t &to)
{
    if (is< void_t >(to)) {
        effect(s, *c.arg);
        return { };
    }

    if (is< array_t >(c.arg->type))
        return address_value(s, *c.arg);

    const auto &from = integer(c.arg->type);
    const auto &into = integer(to);

    auto v = rvalue(s, *c.arg);
    if (into.width > from.width)
        v = extended(s, v, from, into.width);

    auto &valid = s.values[v.id].valid;
    valid = std::min(valid, into.width);

    return v;
}

// The outgoing area of a call with stacked arguments past the eighth,
// chapter 8: 8 bytes for each, rounded up to 16, since sp is a multiple
// of 16 at the call (chapter 3).  It is opened by one sub, whose
// immediate is 12 bits; an area the immediate does not encode is a
// not-yet.
long
outgoing_size(std::size_t stacked)
{
    const long size = (8 * static_cast< long >(stacked) + 15) / 16 * 16;
    if (size > 0xfff)
        not_yet("an outgoing area of 4096 bytes or more");

    return size;
}

// The address of the outgoing area's k-th 8 bytes, [sp, #8k]: the
// ninth argument at [sp], as clang -S writes it, the tenth at [sp, #8].
// The area is under 4096 bytes, so every one is in reach of the store.
std::string
outgoing_operand(std::size_t k)
{
    if (k == 0)
        return "[sp]";

    return std::format("[sp, #{}]", 8 * k);
}

// Chapter 8, every argument a hidden local of its call.  Every held
// value is spilled; each argument, in order and complete before the
// next (G13), is written to a slot of its own, taken before it is
// evaluated, by the store row of 6.6; what the arguments left held is
// spilled in its turn, backend.md 6.7.  The arguments past the eighth
// are copied from their slots to the outgoing area, opened below the
// frame for this call alone, and the first eight are loaded into x0 to
// x7, a narrow one extended to 32 bits by its type's load.  G16: bl f,
// the name alone, since the linker makes a PLT entry where one is
// needed, and a variadic callee gets nothing of its own (chapter 3).
// No unwind directive is written around the area, since the canonical
// frame address is computed from x29.  The result is in x0, valid to
// its type's width and no further, 6.3: the convention leaves the bits
// above a narrow one unspecified, and the width rule extends it where
// it is used wider.
value_t
call(state_t &s, const call_t &c, const type_t &t)
{
    const auto &callee = callee_of(c);
    const auto &f = as< function_t >(callee.type);

    spill_all(s);

    std::vector< std::pair< int, int_t > > slots;
    for (std::size_t i = 0; i < c.args.size(); ++i) {
        const auto &arg = c.args[i];
        const auto &type = integer(arg.type);

        if (i < f.params.size() && !(arg.type == *f.params[i]))
            throw std::logic_error("c1: an argument unlike its parameter");

        const int displacement =
            allocate(s, size_of(arg.type), align_of(arg.type));
        initialize(s, arg, slot_address(displacement), type);

        slots.emplace_back(displacement, type);
    }

    // the call destroys every expression register: nothing is held
    // from here to the bl
    spill_all(s);

    const std::size_t registers = std::size(argument_registers);
    const std::size_t stacked =
        slots.size() > registers ? slots.size() - registers : 0;
    const long area = stacked > 0 ? outgoing_size(stacked) : 0;

    if (area > 0)
        insn(s, "sub", { "sp", "sp", imm(area) });

    for (std::size_t i = registers; i < slots.size(); ++i) {
        const auto &[displacement, type] = slots[i];

        copy_in(s, displacement, type.width);
        copy_out(s, outgoing_operand(i - registers), type.width);
    }

    for (std::size_t i = 0; i < std::min(registers, slots.size()); ++i) {
        const auto &[displacement, type] = slots[i];
        const auto [mnemonic, width] = load_of(type, std::max(32, type.width));

        const auto from = slot_operand(s, displacement, type.width / 8);
        insn(s, mnemonic, { reg(argument_registers[i], width), from });
    }

    insn(s, "bl", { callee.name });

    if (area > 0)
        insn(s, "add", { "sp", "sp", imm(area) });

    if (is< void_t >(t))
        return { };

    return hold(s, x0, integer(t).width);
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
        if (const auto *sym = register_variable(e))
            return read(s, sym);

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
        return call(s, as< call_t >(e), e.type);

    if (is< cast_t >(e))
        return cast(s, as< cast_t >(e), e.type);

    return conditional(s, e);
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
            const bool is_signed = integer(b.lhs->type).is_signed;
            release(s, compare(s, b));

            const auto op = sense ? b.op : inverse(b.op);
            insn(s, std::format("b.{}", condition(op, is_signed)),
                 { label_name(s.function, label) });
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

// statements

void statement(state_t &, const stmt_t &);

void
expr_stmt(state_t &s, const expr_stmt_t &x)
{
    if (x.e)
        effect(s, *x.e);
}

// A register variable takes the next of the ten where its declaration
// is met, chapter 4.  A constant initializer is the constant row into
// the register, mov w19, #0, and any other is its value and the mov of
// establish().  G5: without one, nothing writes the value.  The register
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
    bind(s, d.sym);

    if (d.init && constant_into(s, d.sym, *d.init))
        return;

    if (d.init) {
        const auto v = rvalue(s, *d.init);
        establish(s, d.sym, reg_of(s, v));
        release(s, v);
        return;
    }

    const auto &t = integer(d.sym->type);
    if (t.width < 32 || (t.width == 32 && !t.is_signed))
        establish(s, d.sym, variable_register(s, d.sym));
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
    if (sym->in_register) {
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

// A block's register variables give their registers back at its end.
void
block_stmt(state_t &s, const block_t &b)
{
    const int variables = s.variables;

    for (const auto &st : b.body)
        statement(s, st);

    s.variables = variables;
}

// backend.md 7.2, with b for jmp: the condition's false exit to the
// else part, or past the statement without one.
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

// G14, as for while.  The step has its label, for continue.  The init
// is a statement, a declaration of several declarators a block of
// them, and its names are in scope for the whole loop, 6.8.6.4p1, so
// their registers are given back at the loop's end; such a block's own
// end is not theirs.
void
for_stmt(state_t &s, const for_t &f)
{
    const int variables = s.variables;

    if (f.init && is< block_t >(*f.init))
        for (const auto &st : as< block_t >(*f.init).body)
            statement(s, st);
    else if (f.init)
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
        effect(s, *f.step);
    jump(s, cond);

    place_label(s, end);

    s.variables = variables;
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
// body's slots come after theirs.  Each takes the next slot, in
// declaration order.  Each of the first eight is stored there from its
// argument register at its width, str w0, [x29, #16], 5.2; one past the
// eighth is copied there by stack_parameters(), once the frame's size
// says where the caller left it.  A register parameter takes the next of
// the ten, in parameter order, chapter 4, and no slot.  One of the first
// eight is moved into it from its argument register by establish(),
// which extends a narrow one, sxtb w19, w0: the convention leaves the
// bits above a narrow argument unspecified, and the argument register is
// caller-saved, so it would not survive the first call.  The moves and
// the stores are written in parameter order, after the save area.
void
parameters(state_t &s, const function_decl_t &fd)
{
    for (std::size_t i = 0; i < fd.params.size(); ++i) {
        const auto *p = fd.params[i];
        const auto &t = integer(p->type);

        if (p->in_register) {
            bind(s, p);
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
// where the add into x16 does not reach, 5.3: a not-yet.
void
stack_parameters(state_t &s, const function_decl_t &fd, long size)
{
    const std::size_t registers = std::size(argument_registers);

    for (std::size_t i = registers; i < fd.params.size(); ++i) {
        const auto *p = fd.params[i];
        const auto &t = integer(p->type);
        const int width = t.width;

        const long above = size + 8 * static_cast< long >(i - registers);
        if (above >= 1L << 24)
            not_yet("a stack parameter 16 MiB or more above the frame record");

        if (p->in_register) {
            const auto [mnemonic, into] = load_of(t, register_width(p));
            const auto from =
                slot_operand(s, static_cast< int >(above), width / 8);
            insn(s, mnemonic, { reg(variable_register(s, p), into), from });
            continue;
        }

        copy_in(s, static_cast< int >(above), width);
        copy_out(s, slot_operand(s, slot_of(s, p), width / 8), width);
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

// The register-variable registers a function uses, which its prologue
// saves, backend.md chapter 5: its register parameters and the most
// register variables its body has live at once.
int
registers_saved(const function_decl_t &fd)
{
    const auto parameters = std::ranges::count_if(
        fd.params, [](const symbol_t *p) { return p->in_register; });

    return static_cast< int >(parameters) + registers_in(fd.body->body);
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

// A definition.  The registers the save area holds are counted from the
// tree first, since every slot lies above them; the body is generated
// next, into the buffer, and the prologue written after it, when the
// frame's size is known; so are the copies of the parameters past the
// eighth, which follow the prologue and precede the body.  The first
// slot lies above the frame record and the save area, and this is the
// one place that says so.  The one epilogue is reached with x0 as it
// is, G5's some value, but for main, which returns 0 when it falls off
// its end, 5.1.2.3.4.
void
function(state_t &s, const function_decl_t &fd)
{
    s.body.str({ });
    s.slots.clear();
    s.owner.fill(-1);
    s.values.clear();
    s.age = 0;
    s.live = 0;
    s.registers.clear();
    s.variables = 0;
    s.used = 0;
    s.saved = registers_saved(fd);
    s.in_place = -1;
    s.frame = saved_at(s.saved);
    s.function = fd.sym->name;
    s.labels = 0;
    s.ret = new_label(s);
    s.terminated = false;

    parameters(s, fd);
    block_stmt(s, *fd.body);

    if (s.used != s.saved)
        throw std::logic_error(
            "c1: the registers saved miscount the register variables");

    if (!s.terminated && s.function == "main")
        insn(s, "mov", { "w0", imm(0) });

    const long size = (s.frame + 15) / 16 * 16;

    const auto body = s.body.str();
    s.body.str({ });
    stack_parameters(s, fd, size);

    const auto &name = s.function;
    s.out << std::format("\t.globl\t{}\n"
                         "\t.type\t{}, @function\n"
                         "{}:\n"
                         "\t.cfi_startproc\n",
                         name, name, name);

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
// The string literals follow the last function, chapter 10.  The note
// section marks the stack not executable.
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

    literals(s.out, s.strings);

    out << "\t.section\t.note.GNU-stack,\"\",@progbits\n";
}

} // namespace rcc
