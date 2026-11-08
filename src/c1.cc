// -*- mode: c++; -*-

// c1, the code generator: the typed tree in, x86-64 assembly text out,
// AT&T syntax for the system assembler.  docs/backend.md is the
// design; its chapters 4 to 8 and 10 are what this file implements,
// and each function names the row it writes.  These are the first three
// slices: integers, control and calls; pointers, arrays and string
// literals; register variables.  A construct of a later slice throws
// logic_error("c1: not yet: ..."), and a tree the parser cannot make
// throws logic_error("c1: ...").

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

// The registers, chapter 4: the nine expression registers, the
// caller-saved set in the order a new value takes them, rax first; then
// the five of the register variables, callee-saved, in the order the
// variables take them, rbx first.
enum reg_t {
    rax,
    rcx,
    rdx,
    rsi,
    rdi,
    r8,
    r9,
    r10,
    r11,

    rbx,
    r12,
    r13,
    r14,
    r15
};

constexpr int register_count = 9;

constexpr reg_t variable_registers[] = { rbx, r12, r13, r14, r15 };

// The convention's argument registers, in order, 3.2.
constexpr reg_t argument_registers[] = { rdi, rsi, rdx, rcx, r8, r9 };

// A value the generator holds: in an expression register, or in its
// spill slot once the register was needed for another.  The age orders
// the values for the spill, the oldest first.  valid is the width, in
// bits, up to which the register's bits are the value extended by its
// type's signedness: what the instruction that wrote it left there.  A
// load extends to 32 or 64, a 32-bit write clears the upper half, a
// narrowing cast leaves the bits above its width stale; a widening
// cast that finds the value valid to its width writes nothing.  A
// read of a register variable is fixed: variable names it, and reg is
// the variable's register, which the value does not own.  A fixed value
// is never spilled, freed or written, and it survives a call, since the
// callee saves the register.
struct held_t
{
    int reg;
    int spill;
    long age;
    bool live;
    int valid;
    const symbol_t *variable;
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

    // the bytes of each string literal met, in order; the index is the
    // number of its label
    std::vector< std::string > strings { };

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

    // each register variable's register; how many are live, the most
    // live at once, and the registers the prologue pushes, which the
    // body's first slot lies below
    std::unordered_map< const symbol_t *, reg_t > registers { };
    int variables = 0;
    int used = 0;
    int pushes = 0;

    // the read of a compound assignment's register variable, which its
    // operation writes in place
    int in_place = -1;

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

// The integer type of an operand.  A pointer is a 64-bit unsigned
// quantity to the machine, so its integer view is unsigned long's: the
// width rule, the suffixes and the unsigned conditions of G7 apply to
// it unchanged.  An array is never a value, since the parser decays
// every array it reads, 6.3.2.1p3; anything else the parser does not
// make.
const int_t &
integer(const type_t &t)
{
    static constexpr int_t address_view { 64, false };

    if (is< pointer_t >(t))
        return address_view;

    if (is< array_t >(t))
        throw std::logic_error("c1: an array as a value");

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

// The size of the object a pointer points to, the stride of its
// arithmetic, 6.5.7p9.  It is an immediate of add, imul and mov, so it
// must fit 32 bits.
long
stride(const type_t &pointer)
{
    const long size = size_of(*as< pointer_t >(pointer).pointee);
    if (!fits_imm32(size))
        not_yet("an element of 2 GiB or more");

    return size;
}

// Whether a size is a scale of the machine's addressing form.
bool
is_scale(long size)
{
    return size == 1 || size == 2 || size == 4 || size == 8;
}

// A constant index times the size, modulo 2^64: G6, the address
// arithmetic wraps.
long long
scaled(long long index, long size)
{
    return static_cast< long long >(static_cast< unsigned long long >(index) *
                                    static_cast< unsigned long long >(size));
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
    static constexpr std::string_view names[][4] = {
        { "al", "ax", "eax", "rax" },      { "cl", "cx", "ecx", "rcx" },
        { "dl", "dx", "edx", "rdx" },      { "sil", "si", "esi", "rsi" },
        { "dil", "di", "edi", "rdi" },     { "r8b", "r8w", "r8d", "r8" },
        { "r9b", "r9w", "r9d", "r9" },     { "r10b", "r10w", "r10d", "r10" },
        { "r11b", "r11w", "r11d", "r11" }, { "bl", "bx", "ebx", "rbx" },
        { "r12b", "r12w", "r12d", "r12" }, { "r13b", "r13w", "r13d", "r13" },
        { "r14b", "r14w", "r14d", "r14" }, { "r15b", "r15w", "r15d", "r15" },
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

// The local or parameter a name designates, which has a slot.  A
// register variable has none, and the parser refuses its address,
// 6.5.4.2p1; a global is a later slice.
const symbol_t *
object_of(const expr_t &e)
{
    if (!is< name_t >(e))
        throw std::logic_error("c1: an lvalue the parser does not make");

    const auto *sym = as< name_t >(e).sym;
    if (sym->storage == storage_t::function)
        throw std::logic_error("c1: a function as an object");

    if (sym->storage == storage_t::global)
        not_yet("a global");

    if (sym->in_register)
        throw std::logic_error("c1: the address of a register variable");

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

// The store row: the narrow mov of the destination's width; G2, the
// low bytes of the register are the value.
void
store(state_t &s, reg_t r, std::string_view to, int width)
{
    insn(s, sized("mov", width), { reg(r, width), to });
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

    s.values.push_back({ r, 0, s.age++, true, valid, nullptr });
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

// The value is consumed: its register, if it has one and owns it, is
// free.
void
release(state_t &s, value_t v)
{
    if (v.id < 0)
        return;

    auto &h = s.values[v.id];
    if (h.reg >= 0 && !h.variable)
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

value_t rvalue(state_t &, const expr_t &);
void cbranch(state_t &, const expr_t &, int label, bool sense);

// register variables

// Whether a value is a register variable's read.
bool
fixed(const state_t &s, value_t v)
{
    return v.id >= 0 && s.values[v.id].variable != nullptr;
}

// The register a register variable lives in, from its declaration to
// the end of its block.
reg_t
variable_register(const state_t &s, const symbol_t *sym)
{
    const auto it = s.registers.find(sym);
    if (it == s.registers.end())
        throw std::logic_error(
            "c1: a register variable before its declaration");

    return it->second;
}

// The width a register variable's register is written at: 64 bits for a
// long or a pointer, 32 for the rest.
int
register_width(const symbol_t *sym)
{
    return std::max(32, integer(sym->type).width);
}

// The register variable an lvalue names, or null.  The parser gave the
// symbol its register, or declared the sixth an ordinary local.
const symbol_t *
register_variable(const expr_t &e)
{
    if (!is< name_t >(e))
        return nullptr;

    const auto *sym = as< name_t >(e).sym;

    return sym->in_register ? sym : nullptr;
}

// The next of the five for a register variable met in declaration
// order; a block's end gives its variables' registers back, so a later
// block takes them again from rbx.  The parser keeps a sixth live one
// out of a register.
void
bind(state_t &s, const symbol_t *sym)
{
    if (s.variables == static_cast< int >(std::size(variable_registers)))
        throw std::logic_error("c1: a sixth register variable");

    s.registers.insert_or_assign(sym, variable_registers[s.variables++]);
    s.used = std::max(s.used, s.variables);
}

// A read of a register variable: its register as an operand, and no
// instruction.  The register holds the value as establish() left it,
// which is what the read is valid to.
value_t
read(state_t &s, const symbol_t *sym)
{
    const auto &t = integer(sym->type);
    const int id = static_cast< int >(s.values.size());

    s.values.push_back({ variable_register(s, sym), 0, s.age++, true,
                         written(register_width(sym), t.is_signed), sym });
    ++s.live;

    return { id };
}

// The mov into a register variable's register, from source at the
// variable's width, that keeps the invariant: the register holds the
// value extended to 32 bits by the variable's signedness, which for an
// unsigned written at 32 is its extension to 64, and a long or a pointer
// whole.  movl for an int or an unsigned, movq for a long or a pointer;
// for a narrow type the extending movsbl, movzbl, movswl or movzwl,
// where the store row has its narrow mov: G2's truncation and the
// invariant in one instruction.
void
establish(state_t &s, const symbol_t *sym, std::string_view source)
{
    const auto &t = integer(sym->type);
    const auto [mnemonic, width] = extension(t, register_width(sym));

    insn(s, mnemonic, { source, reg(variable_register(s, sym), width) });
}

void
establish(state_t &s, const symbol_t *sym, reg_t from)
{
    establish(s, sym, reg(from, integer(sym->type).width));
}

// A constant assigned to a register variable, or initializing it, is
// the immediate into the register, the constant row: mov $c, %r12d.
// The parser folded the constant to the variable's type, so a narrow
// one is in range and its extension is the immediate's own; a 64-bit
// constant no 32-bit immediate holds is movabsq.  False when e is not
// a constant.
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

    const auto to = reg(variable_register(s, sym), width);

    if (width == 32)
        insn(s, "movl", { imm(value), to });
    else if (fits_imm32(value))
        insn(s, "movq", { imm(value), to });
    else
        insn(s, "movabsq", { imm(value), to });

    return true;
}

// Copies a fixed value into the free expression register to, with the
// mov of its variable's register width; the value is then to's, valid
// to what it was, since the mov carries every bit the read was valid
// to.
void
copy(state_t &s, value_t v, reg_t to)
{
    auto &h = s.values[v.id];
    const int width = register_width(h.variable);

    insn(s, sized("mov", width),
         { reg(static_cast< reg_t >(h.reg), width), reg(to, width) });

    h.reg = to;
    h.variable = nullptr;
    s.owner[to] = v.id;
}

// The register of a value an instruction is about to write.  A fixed
// value is first copied into an expression register outside avoid,
// since its register is the variable; the read of a compound
// assignment's own variable, s.in_place, is the one written in place.
reg_t
writable(state_t &s, value_t v, unsigned avoid = 0)
{
    if (!fixed(s, v) || v.id == s.in_place)
        return reg_of(s, v, avoid);

    const reg_t r = take(s, avoid);
    copy(s, v, r);

    return r;
}

// Before a register variable is written, each of its reads still held
// is copied into an expression register, so it keeps the value it
// read: G13, an operand's side effects complete before the next operand
// starts, and i = i++ + ++i has one value.
void
preserve(state_t &s, const symbol_t *sym)
{
    for (std::size_t id = 0; id < s.values.size(); ++id)
        if (s.values[id].live && s.values[id].variable == sym)
            copy(s, { static_cast< int >(id) }, take(s, 0));
}

bool
is_step(unary_op_t op)
{
    return op == unary_op_t::pre_inc || op == unary_op_t::pre_dec ||
           op == unary_op_t::post_inc || op == unary_op_t::post_dec;
}

// Whether evaluating e may write the register variable sym: an
// assignment to it or a ++ or -- of it anywhere in e.
bool
writes(const expr_t &e, const symbol_t *sym)
{
    if (is< unary_t >(e)) {
        const auto &u = as< unary_t >(e);
        return (is_step(u.op) && register_variable(*u.arg) == sym) ||
               writes(*u.arg, sym);
    }

    if (is< binary_t >(e))
        return writes(*as< binary_t >(e).lhs, sym) ||
               writes(*as< binary_t >(e).rhs, sym);

    if (is< assign_t >(e)) {
        const auto &a = as< assign_t >(e);
        return register_variable(*a.lhs) == sym || writes(*a.lhs, sym) ||
               writes(*a.rhs, sym);
    }

    if (is< call_t >(e)) {
        const auto &c = as< call_t >(e);
        return writes(*c.callee, sym) ||
               std::ranges::any_of(c.args, [sym](const expr_t &arg) {
                   return writes(arg, sym);
               });
    }

    if (is< cast_t >(e))
        return writes(*as< cast_t >(e).arg, sym);

    if (is< conditional_t >(e)) {
        const auto &c = as< conditional_t >(e);
        return writes(*c.cond, sym) || writes(*c.yes, sym) ||
               writes(*c.no, sym);
    }

    return false;
}

// Before the two paths of &&, || or ?: as a value: every held value is
// spilled, as for a call, and a read of a register variable that e
// writes is copied first, so it is spilled with them.  preserve() on
// one path alone would leave the paths with two register maps.
void
before_paths(state_t &s, const expr_t &e)
{
    for (std::size_t id = 0; id < s.values.size(); ++id) {
        const auto &h = s.values[id];
        if (h.live && h.variable && writes(e, h.variable))
            copy(s, { static_cast< int >(id) }, take(s, 0));
    }

    spill_all(s);
}

// The value extended by from's signedness to a width of 32 or 64, where
// it is not valid to it already, the width rule.  A fixed value is
// extended into a fresh register, and the value is then that one.
value_t
extended(state_t &s, value_t v, const int_t &from, int to)
{
    if (s.values[v.id].valid >= to)
        return v;

    const auto [mnemonic, width] = extension(from, std::max(32, to));
    const reg_t source = reg_of(s, v);

    if (!fixed(s, v)) {
        insn(s, mnemonic, { reg(source, from.width), reg(source, width) });
        s.values[v.id].valid = written(width, from.is_signed);
        return v;
    }

    release(s, v);

    const auto e = fresh(s, written(width, from.is_signed));
    insn(s, mnemonic, { reg(source, from.width), reg(reg_of(s, e), width) });

    return e;
}


// addresses

// An lvalue's address as the operand of the instruction that reads or
// writes it: a slot off rbp, a label off rip, or the machine's
// addressing form, a held base with a displacement or with a held
// index scaled by 1, 2, 4 or 8.  The access consumes the values.
struct address_t
{
    value_t base { };
    value_t index { };
    long scale = 1;
    long long displacement = 0;
    std::string label { };
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

// The address in AT&T syntax, d(B,I,s).  A spilled base or index is
// reloaded first, outside avoid and outside the other's register.
std::string
operand(state_t &s, const address_t &a, unsigned avoid = 0)
{
    if (!a.label.empty())
        return std::format("{}(%rip)", a.label);

    if (a.base.id < 0)
        return at(static_cast< int >(a.displacement));

    avoid |= registers_of(s, a);

    std::string index;
    if (a.index.id >= 0) {
        const reg_t r = reg_of(s, a.index, avoid);
        avoid |= bit(r);
        index = std::format(",{},{}", reg(r, 64), a.scale);
    }

    const reg_t base = reg_of(s, a.base, avoid);
    const auto displacement =
        a.displacement != 0 ? std::to_string(a.displacement) : std::string();

    return std::format("{}({}{})", displacement, reg(base, 64), index);
}

// The address is consumed: its registers are free.
void
release(state_t &s, const address_t &a)
{
    release(s, a.base);
    release(s, a.index);
}

// The load row, 6.3.2.1p2: the value at an address into a register at
// a width of 32 or 64, a char or a short by the extending mov of its
// signedness.  G8 to G11: one mov, whatever computed the address and
// however it is aligned.  The address is consumed first, so the value
// takes the lowest free register, the left operand's: mov (L,R,s), L.
value_t
load(state_t &s, const address_t &a, const int_t &t, int width)
{
    const auto [mnemonic, into] = extension(t, width);

    const auto from = operand(s, a);
    release(s, a);

    const auto v = fresh(s, written(into, t.is_signed));
    insn(s, mnemonic, { from, reg(reg_of(s, v), into) });

    return v;
}

// The load row of a read-modify-write, a compound assignment or a ++
// or --: the address stays held for the store through it, since E1 is
// evaluated once, 6.5.17.3p4.
value_t
fetch(state_t &s, const address_t &a, const int_t &t, int width)
{
    const auto [mnemonic, into] = extension(t, width);

    const auto from = operand(s, a);
    const auto v = fresh(s, written(into, t.is_signed), registers_of(s, a));
    insn(s, mnemonic, { from, reg(reg_of(s, v), into) });

    return v;
}

// The store row through an address, at the lvalue's width.  The
// address stays held, for the caller to consume.
void
store(state_t &s, value_t v, const address_t &a, int width)
{
    const reg_t r = reg_of(s, v, registers_of(s, a));
    store(s, r, operand(s, a, bit(r)), width);
}

// The address as a value, valid to 64: the leaq of the operand form,
// where the load would have been a mov.  A base alone is its own
// value, so &*p is p and no instruction, 6.5.4.2p3.
value_t
address_of(state_t &s, const address_t &a)
{
    if (a.base.id >= 0 && a.index.id < 0 && a.displacement == 0)
        return a.base;

    const auto from = operand(s, a);
    release(s, a);

    const auto v = fresh(s, 64);
    insn(s, "leaq", { from, reg(reg_of(s, v), 64) });

    return v;
}

// A string literal's array, 6.4.5p6: one per literal met, in the order
// met, never merged with an equal one, which 6.4.5p7 leaves
// unspecified, and writable (G17).  generate() writes them to .data.
std::string
string_label(state_t &s, const string_t &str)
{
    const auto label = std::format(".Ls{}", s.strings.size());
    s.strings.push_back(str.bytes);

    return label;
}

// The address a dereference designates, 6.5.4.2p4, of an object of
// type pointee: the addressing row.  An add of a pointer and an
// integer, the shape of a subscript (6.5.3.2p2), is the machine's
// addressing form when the size is a scale: the pointer the base, the
// integer the index, each generated in the order written (G13); a
// constant index is the displacement c * size when it fits 32 bits.
// Any other pointer, an add of another size included, is computed into
// a register and is the base; nothing looks through a cast.
address_t
pointed(state_t &s, const expr_t &p, const type_t &pointee)
{
    const long size = size_of(pointee);

    if (!is_scale(size) || !is< binary_t >(p) ||
        as< binary_t >(p).op != binary_op_t::add)
        return { .base = rvalue(s, p) };

    const auto &b = as< binary_t >(p);
    const bool left = is< pointer_t >(b.lhs->type);
    const auto &pointer = left ? *b.lhs : *b.rhs;
    const auto &index = left ? *b.rhs : *b.lhs;

    if (is< literal_t >(index)) {
        const auto d = scaled(as< literal_t >(index).value, size);
        if (fits_imm32(d))
            return { .base = rvalue(s, pointer), .displacement = d };
    }

    const auto l = rvalue(s, *b.lhs);
    const auto r = rvalue(s, *b.rhs);

    return { .base = left ? l : r, .index = left ? r : l, .scale = size };
}

// The address of an lvalue: a local's or a parameter's slot, a string
// literal's label, or what a dereference designates.  It serves &,
// array decay, loads and stores alike, so the addressing row is
// written once.
address_t
address(state_t &s, const expr_t &e)
{
    if (is< string_t >(e))
        return { .label = string_label(s, as< string_t >(e)) };

    if (is< unary_t >(e) && as< unary_t >(e).op == unary_op_t::deref)
        return pointed(s, *as< unary_t >(e).arg, e.type);

    return slot_address(slot_of(s, object_of(e)));
}

// expressions

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
// remainder comes back into L.  The divisor r is held, generated by
// the caller.  A register variable as the dividend is moved to rax and
// as the divisor is the operand of idiv; the result of a fixed dividend
// is a new value in rax or rdx, since L is the variable.
value_t
divide(state_t &s, binary_op_t op, value_t l, value_t r, const int_t &t)
{
    const int width = t.width;

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

    if (fixed(s, l)) {
        release(s, r);
        release(s, l);
        return hold(s, result, written(width, t.is_signed));
    }

    if (left != result)
        insn(s, sized("mov", width), { reg(result, width), reg(left, width) });

    release(s, r);
    wrote(s, l, width, t.is_signed);

    return l;
}

// G3: the instruction masks the count to the width, and nothing else
// does.  A constant count is the immediate, its low 8 bits, which is
// what the instruction encodes; a variable one goes to cl, a value
// held in rcx moving out first (chapter 6.5), a register variable
// copied there.
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
               reg(writable(s, l), width) });
        wrote(s, l, width, t.is_signed);
        return l;
    }

    const auto r = rvalue(s, rhs);
    auto [left, right] = operands(s, l, r);
    left = writable(s, l, bit(right) | bit(rcx));

    if (left == rcx) {
        const reg_t to = take(s, bit(rcx) | bit(right));
        move(s, left, to, 64);
        left = to;
    }

    if (right != rcx) {
        vacate(s, rcx, bit(left) | bit(right));

        if (fixed(s, r))
            copy(s, r, rcx);
        else
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
// A register variable is R as it is; as L it is copied first, unless
// it is the variable a compound assignment writes in place.
value_t
arithmetic(
    state_t &s, binary_op_t op, value_t l, const expr_t &rhs, const type_t &t)
{
    const int width = operation_width(t);

    if (op == binary_op_t::div || op == binary_op_t::rem)
        return divide(s, op, l, rvalue(s, rhs), integer(t));

    if (op == binary_op_t::shl || op == binary_op_t::shr)
        return shift(s, op, l, rhs, integer(t));

    const auto mnemonic = sized(arithmetic_mnemonic(op), width);

    if (immediate(rhs, width)) {
        const auto left = reg(writable(s, l), width);
        const auto c = immediate_of(rhs, width);

        if (op == binary_op_t::mul)
            insn(s, mnemonic, { c, left, left });
        else
            insn(s, mnemonic, { c, left });

        wrote(s, l, width, integer(t).is_signed);
        return l;
    }

    const auto r = rvalue(s, rhs);
    const reg_t right = operands(s, l, r).second;
    const reg_t left = writable(s, l, bit(right));
    insn(s, mnemonic, { reg(right, width), reg(left, width) });

    release(s, r);
    wrote(s, l, width, integer(t).is_signed);

    return l;
}

// The arithmetic row of p + n, n + p and p - n, 6.5.7p9, both values
// held and the result in L.  G6: the address plus or minus n times
// the size, modulo 2^64.  A size of 1 is add or sub; + with another
// scale is one leaq (P,N,s); - with a scale, and either with any other
// size, multiplies the index by the size first.  L and R are as the
// source wrote the operands: n + p has the index in L.
value_t
offset(state_t &s,
       binary_op_t op,
       value_t l,
       value_t r,
       long size,
       bool pointer_first)
{
    const auto mnemonic = op == binary_op_t::add ? "addq" : "subq";

    if (size != 1 && op == binary_op_t::add && is_scale(size))
        return address_of(s, { .base = pointer_first ? l : r,
                               .index = pointer_first ? r : l,
                               .scale = size });

    auto [left, right] = operands(s, l, r);

    // the multiply writes the index, and the add or sub writes L
    if (size != 1) {
        auto &index = pointer_first ? right : left;
        index = writable(s, pointer_first ? r : l,
                         bit(pointer_first ? left : right));
        insn(s, "imulq", { imm(size), reg(index, 64), reg(index, 64) });
    }

    left = writable(s, l, bit(right));
    insn(s, mnemonic, { reg(right, 64), reg(left, 64) });

    release(s, r);
    wrote(s, l, 64, false);

    return l;
}

// p + n and p - n with the pointer held in l and n not yet generated,
// and p += n and p -= n on the loaded pointer.  A constant n is the
// immediate c * size, or, when no 32-bit immediate holds the product,
// the product held in a register; the parser retyped a constant index
// to 64 bits, so p += 2 on an int * is addq $8.
value_t
displaced(state_t &s, binary_op_t op, value_t l, const expr_t &n, long size)
{
    if (!is< literal_t >(n))
        return offset(s, op, l, rvalue(s, n), size, true);

    const auto d = scaled(as< literal_t >(n).value, size);

    if (fits_imm32(d)) {
        insn(s, op == binary_op_t::add ? "addq" : "subq",
             { imm(d), reg(writable(s, l), 64) });
        wrote(s, l, 64, false);
        return l;
    }

    const auto r = fresh(s, 64);
    insn(s, "movabsq", { imm(d), reg(reg_of(s, r), 64) });

    return offset(s, op, l, r, 1, true);
}

// A binary + or - of the pointer type t: the pointer and the integer,
// the integer widened to 64 bits by the parser, in the order written
// (G13).
value_t
pointer_arithmetic(state_t &s, const binary_t &b, const type_t &t)
{
    const long size = stride(t);
    const auto l = rvalue(s, *b.lhs);

    if (is< pointer_t >(b.lhs->type))
        return displaced(s, b.op, l, *b.rhs, size);

    return offset(s, b.op, l, rvalue(s, *b.rhs), size, false);
}

// p - q, 6.5.7p10: the difference of the addresses divided by the size,
// a long.  G6: a division, idivq by the size held in a register, not a
// shift, since the two pointers are any two addresses; a size of 1 is
// the subq alone.
value_t
difference(state_t &s, const binary_t &b)
{
    const long size = stride(b.lhs->type);

    const auto l = rvalue(s, *b.lhs);
    const auto r = rvalue(s, *b.rhs);
    const reg_t right = operands(s, l, r).second;
    const reg_t left = writable(s, l, bit(right));
    insn(s, "subq", { reg(right, 64), reg(left, 64) });

    release(s, r);
    wrote(s, l, 64, true);

    if (size == 1)
        return l;

    const auto d = fresh(s, 64);
    insn(s, "movq", { imm(size), reg(reg_of(s, d), 64) });

    return divide(s, binary_op_t::div, l, d, int_t { 64, true });
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
// conversions made common; a pointer's is unsigned, G7.
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
// to the int the operator yields.  A register variable as L is only
// read by the cmp, and the setcc takes a fresh register.
value_t
comparison(state_t &s, const binary_t &b)
{
    const bool is_signed = integer(b.lhs->type).is_signed;

    auto l = compare(s, b);
    if (fixed(s, l)) {
        release(s, l);
        l = fresh(s, 32);
    }

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
    before_paths(s, e);

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
// first arm's, which is an expression register: a register variable as
// the first arm is copied into one.
value_t
conditional(state_t &s, const expr_t &e)
{
    const auto &c = as< conditional_t >(e);
    const auto &t = e.type;

    before_paths(s, e);

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
    const reg_t into = writable(s, yes);
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

    if (is< pointer_t >(e.type))
        return pointer_arithmetic(s, b, e.type);

    if (is< pointer_t >(b.lhs->type))
        return difference(s, b);

    // G13: the left operand first, held while the right one is generated
    const auto l = rvalue(s, *b.lhs);

    return arithmetic(s, b.op, l, *b.rhs, e.type);
}

// ++ and -- of a register variable: add or sub of 1, or of the size for
// a pointer, on its register in place, then the re-extension of a narrow
// one, movsbl %bl, %ebx, which is the store row.  The postfix forms
// read the variable first, and preserve() copies the read into a fresh
// register before the write, movl %ebx, %eax: that copy is the value.
// The prefix forms yield a read of the variable after the write, as a
// compound assignment does.
value_t
step_register(state_t &s, const unary_t &u, const symbol_t *sym)
{
    const auto &t = integer(sym->type);
    const int width = register_width(sym);
    const long amount = is< pointer_t >(sym->type) ? stride(sym->type) : 1;

    const bool up = u.op == unary_op_t::pre_inc || u.op == unary_op_t::post_inc;
    const bool post =
        u.op == unary_op_t::post_inc || u.op == unary_op_t::post_dec;

    const auto old = post ? read(s, sym) : value_t { };
    preserve(s, sym);

    const reg_t r = variable_register(s, sym);
    insn(s, sized(up ? "add" : "sub", width), { imm(amount), reg(r, width) });
    if (t.width < 32)
        establish(s, sym, r);

    return post ? old : read(s, sym);
}

// ++ and --, either side: the address once, the load, add or sub of 1,
// or of the size for a pointer (6.5.4.1p2, 6.5.3.5p2), the store
// through the same address; G1, the machine wraps.  A char or a short
// is loaded extended to 32 bits and its low bytes stored, so no
// operator sees 8 or 16 bits.  The postfix forms copy the old value to
// another register first and yield it.
value_t
step(state_t &s, const unary_t &u)
{
    if (const auto *sym = register_variable(*u.arg))
        return step_register(s, u, sym);

    const auto &t = integer(u.arg->type);
    const int width = std::max(32, t.width);
    const long amount = is< pointer_t >(u.arg->type) ? stride(u.arg->type) : 1;

    const auto place = address(s, *u.arg);
    const auto l = fetch(s, place, t, width);
    const reg_t r = reg_of(s, l);

    const bool up = u.op == unary_op_t::pre_inc || u.op == unary_op_t::post_inc;
    const bool post =
        u.op == unary_op_t::post_inc || u.op == unary_op_t::post_dec;

    value_t old;
    if (post) {
        old = fresh(s, s.values[l.id].valid, bit(r) | registers_of(s, place));
        insn(s, sized("mov", width),
             { reg(r, width), reg(reg_of(s, old), width) });
    }

    insn(s, sized(up ? "add" : "sub", width), { imm(amount), reg(r, width) });
    store(s, l, place, t.width);
    release(s, place);
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
             { reg(writable(s, v), width) });
        return v;
    }

    // as a value; the operand is not promoted, so it is compared at its
    // own width.  A register variable is read by the cmp, and the sete
    // takes a fresh register, as for a comparison.
    case unary_op_t::log_not: {
        const int width = integer(u.arg->type).width;
        auto v = rvalue(s, *u.arg);
        insn(s, sized("cmp", width), { imm(0), reg(reg_of(s, v), width) });

        if (fixed(s, v)) {
            release(s, v);
            v = fresh(s, 32);
        }

        const reg_t r = reg_of(s, v);
        insn(s, "sete", { reg(r, 8) });
        insn(s, "movzbl", { reg(r, 8), reg(r, 32) });
        wrote(s, v, 32, true);
        return v;
    }

    case unary_op_t::pre_inc:
    case unary_op_t::pre_dec:
    case unary_op_t::post_inc:
    case unary_op_t::post_dec: return step(s, u);

    case unary_op_t::addr: return address_of(s, address(s, *u.arg));

    case unary_op_t::deref: {
        const auto &type = integer(t);
        return load(s, pointed(s, *u.arg, t), type, std::max(32, type.width));
    }
    }

    throw std::logic_error("c1: an unknown unary operator");
}

// A compound assignment's result into its register variable.  In
// place, the operation wrote the register at the operation's width,
// and a narrow variable is re-extended, as is a 32-bit one an operation
// of 64 bits wrote whole; otherwise the result is moved in.
void
settle(state_t &s, const symbol_t *sym, value_t v, int width)
{
    if (!fixed(s, v)) {
        establish(s, sym, reg_of(s, v));
        return;
    }

    const auto &t = integer(sym->type);
    if (t.width < 32 || width > register_width(sym))
        establish(s, sym, variable_register(s, sym));
}

// Assignment to a register variable, after preserve() has copied every
// read of it still held.  Plain: a constant is the immediate into the
// variable's register and the value is a read of the variable;
// otherwise the right operand into L, then the mov of establish() into
// the register, movl %eax, %ebx, or movl %r12d, %ebx from another
// register variable, and the value stays L, as for a slot.  Compound: the operation row with the variable's
// register as L, in place, addl %eax, %ebx, but for a division, whose
// quotient comes back from rax by the mov; the value is a read of the
// variable after the write.  The read is taken before the right
// operand, as the slot's load is (6.5.17.3p4), and is extended into a
// fresh register first when the operation is wider than the register
// is valid to, movslq %ebx, %rax for int += long.
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
    const auto l = extended(s, read(s, sym), integer(sym->type), width);

    const int outer = std::exchange(s.in_place, l.id);
    const auto v = is< pointer_t >(sym->type) ?
                       displaced(s, *a.op, l, *a.rhs, stride(sym->type)) :
                       arithmetic(s, *a.op, l, *a.rhs, a.common);
    s.in_place = outer;

    settle(s, sym, v, width);
    release(s, v);

    return read(s, sym);
}

// Plain or compound assignment; its value is the value stored, the low
// bytes of L.  The lvalue's address comes first (G13) and is held
// across the right operand.  A compound one loads the old value before
// the right operand is evaluated, extended into the common type the
// parser chose, 6.5.17.3p4, and stores the result's low bytes back
// through the same address (G2); p += n and p -= n step the pointer by
// n times the size.
value_t
assign(state_t &s, const assign_t &a)
{
    const auto &t = integer(a.lhs->type);
    const bool stepped = a.op && is< pointer_t >(a.lhs->type);

    // the parser converted the right operand to the common type, or
    // widened it to 64 bits for a pointer
    if (!stepped && !(a.rhs->type == a.common))
        throw std::logic_error("c1: an assignment across two types");

    if (const auto *sym = register_variable(*a.lhs))
        return assign_register(s, a, sym);

    const auto place = address(s, *a.lhs);

    if (!a.op) {
        const auto v = rvalue(s, *a.rhs);
        store(s, v, place, t.width);
        release(s, place);
        return v;
    }

    // the value of the expression has the lvalue's type: the bits
    // above its width are the operation's, not the value's extension
    const auto l = fetch(s, place, t, operation_width(a.common));
    const auto v = stepped ?
                       displaced(s, *a.op, l, *a.rhs, stride(a.lhs->type)) :
                       arithmetic(s, *a.op, l, *a.rhs, a.common);
    store(s, v, place, t.width);
    release(s, place);
    s.values[v.id].valid = std::min(s.values[v.id].valid, t.width);

    return v;
}

// An integer conversion.  G2: narrowing is no instruction, the narrower
// register name is the value; widening extends by the operand's
// signedness, and only when the register is not already valid to the
// new width, as it is after the load of a char or a 32-bit write of an
// unsigned value.  A change of signedness alone is nothing.  A pointer
// is its integer view, so pointer to pointer is nothing, an integer
// widens to a pointer by its own signedness (6.3.2.3p5), and a pointer
// narrows to an integer by its low bits (p6).  An array decays to its
// address, 6.3.2.1p3.  A cast to void discards the value.  A register
// variable widens into a fresh register, movslq %ebx, %rax, and narrows
// as its read: the read, not the variable, is valid to less.
value_t
cast(state_t &s, const cast_t &c, const type_t &to)
{
    if (is< void_t >(to)) {
        release(s, rvalue(s, *c.arg));
        return { };
    }

    if (is< array_t >(c.arg->type))
        return address_of(s, address(s, *c.arg));

    const auto &from = integer(c.arg->type);
    const auto &into = integer(to);

    auto v = rvalue(s, *c.arg);
    if (into.width > from.width)
        v = extended(s, v, from, into.width);

    auto &valid = s.values[v.id].valid;
    valid = std::min(valid, into.width);

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
        store(s, reg_of(s, v), at(displacement), type.width);
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
// is cmp and jcc; ! flips the sense; && and || are the shape of the
// short circuit, 6.5.14p4, 6.5.15p4, the right operand not reached when
// the left decides.  Any other scalar, a constant or a pointer
// included, is its value compared with 0 at its own width.
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

// A register variable takes the next of the five where its declaration
// is met, and an initializer is moved into the register by establish().
// G5: without one, nothing writes the value; the register holds what
// the prologue left in it, and every read before the first write reads
// that.  Where the invariant claims more bits than the type has, for a
// char, a short or an unsigned, it is established on those contents by
// the mov from the register to itself, movsbl %bl, %ebx: the value of
// the type in the low bits is unchanged, and the register is the
// extension every read assumes.
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
// stored; the slot holds what it holds.  An array's slot is at the
// type's alignment, chapter 5, and its displacement is the array's
// lowest address, the elements upward from it.
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

    const auto &t = integer(sym->type);
    const auto v = rvalue(s, *d.init);
    store(s, reg_of(s, v), at(displacement), t.width);
    release(s, v);
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

// G14, as for while.  The step has its label, for continue.  The
// variables the init declares are in scope for the whole loop,
// 6.8.6.4p1, so their registers are given back at the loop's end; a
// declaration of several declarators is a block of declarations, whose
// own end is not theirs.
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
        release(s, rvalue(s, *f.step));
    jump(s, cond);

    place_label(s, end);

    s.variables = variables;
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
// 16(%rbp) and up, 8 bytes apart, and are read from there.  A register
// parameter takes the next of the five, in parameter order, and is
// moved into it from its argument register or its stack slot by
// establish(), which extends a narrow one: the ABI leaves the upper bits
// of a char or a short argument undefined, and the argument register is
// caller-saved, so it would not survive the first call.  The moves and
// the stores are written in parameter order, after the pushes.
void
parameters(state_t &s, const function_decl_t &fd)
{
    const std::size_t registers = std::size(argument_registers);

    for (std::size_t i = 0; i < fd.params.size(); ++i) {
        const auto *p = fd.params[i];
        const auto &t = integer(p->type);
        const int above =
            16 + 8 * (static_cast< int >(i) - static_cast< int >(registers));

        if (p->in_register) {
            bind(s, p);
            establish(s, p,
                      i < registers ? reg(argument_registers[i], t.width) :
                                      at(above));
            continue;
        }

        if (i >= registers) {
            s.slots.emplace(p, above);
            continue;
        }

        const int displacement =
            allocate(s, size_of(p->type), align_of(p->type));
        s.slots.emplace(p, displacement);
        store(s, argument_registers[i], at(displacement), t.width);
    }
}

// The most register variables live at once in a block's items, those
// of nested blocks included: a declaration adds one to the count to its
// block's end.  It is the count c1 reaches walking the block, which is
// the parser's assignment, known before the body is generated, since
// the slots lie below the pushes.
int registers_in(const stmt_t &);

bool
declares_register(const stmt_t &st)
{
    return is< decl_stmt_t >(st) && as< decl_stmt_t >(st).sym->in_register;
}

int
registers_in(const std::vector< stmt_t > &items)
{
    int live = 0;
    int most = 0;

    for (const auto &st : items) {
        live += declares_register(st);
        most = std::max(most, live + registers_in(st));
    }

    return most;
}

int
registers_in(const stmt_t &st)
{
    if (is< block_t >(st))
        return registers_in(as< block_t >(st).body);

    if (is< if_t >(st)) {
        const auto &i = as< if_t >(st);
        return std::max(registers_in(*i.yes), i.no ? registers_in(*i.no) : 0);
    }

    if (is< while_t >(st))
        return registers_in(*as< while_t >(st).body);

    // the init's declarations live with the body, 6.8.6.4p1
    if (is< for_t >(st)) {
        const auto &f = as< for_t >(st);

        int init = 0;
        if (f.init)
            init = registers_in(*f.init) + declares_register(*f.init);

        return init + registers_in(*f.body);
    }

    return 0;
}

// The registers of the five a function uses: its register parameters
// and the most register variables its body has live at once.
int
pushes_of(const function_decl_t &fd)
{
    const auto parameters = std::ranges::count_if(
        fd.params, [](const symbol_t *p) { return p->in_register; });

    return static_cast< int >(parameters) + registers_in(fd.body->body);
}

// Chapter 5: rbp is the frame pointer in every function; the registers
// of the five the function uses are pushed below it, in order, each
// with its place in the unwind table, -16 - 8k off the canonical frame
// address for the k-th.  The frame is the slots below the pushes, sized
// so that rsp is a multiple of 16 at every call: after pushq %rbp it
// is, so the pushes and the subq together are.  A function that calls
// nothing keeps up to 128 bytes of slots in the red zone, measured from
// rsp after the pushes, and moves no rsp.
void
prologue(state_t &s, int size, bool sub)
{
    s.out << "\tpushq\t%rbp\n"
             "\t.cfi_def_cfa_offset 16\n"
             "\t.cfi_offset %rbp, -16\n"
             "\tmovq\t%rsp, %rbp\n"
             "\t.cfi_def_cfa_register %rbp\n";

    for (int k = 0; k < s.pushes; ++k) {
        const auto r = reg(variable_registers[k], 64);
        s.out << std::format("\tpushq\t{}\n\t.cfi_offset {}, {}\n", r, r,
                             -16 - 8 * (k + 1));
    }

    if (sub)
        s.out << std::format("\tsubq\t${}, %rsp\n", size);
}

// The one epilogue: rsp back to the pushes, if the prologue moved it
// below them, the pops in reverse order, then leave; the directive after
// it is the unwind table's, since the canonical frame address is rsp + 8
// from leave to ret.
void
epilogue(state_t &s, bool sub)
{
    s.out << label_name(s.ret) << ":\n";

    if (sub && s.pushes > 0)
        s.out << std::format("\tleaq\t{}, %rsp\n", at(-8 * s.pushes));

    for (int k = s.pushes; k-- > 0;)
        s.out << std::format("\tpopq\t{}\n", reg(variable_registers[k], 64));

    s.out << "\tleave\n"
             "\t.cfi_def_cfa %rsp, 8\n"
             "\tret\n";
}

// A definition.  The registers the prologue pushes are counted from the
// tree first, since every slot lies below them; the body is generated
// next, into the buffer, and the prologue written after it, when the
// frame's size is known.  The one
// epilogue is reached with rax as it is, G5's some value, but for main,
// which returns 0 when it falls off its end, 5.1.2.3.4.
void
function(state_t &s, const function_decl_t &fd)
{
    s.body.str({ });
    s.slots.clear();
    s.calls = false;
    s.owner.fill(-1);
    s.values.clear();
    s.age = 0;
    s.live = 0;
    s.registers.clear();
    s.variables = 0;
    s.used = 0;
    s.pushes = pushes_of(fd);
    s.in_place = -1;
    s.frame = 8 * s.pushes;
    s.function = fd.sym->name;
    s.ret = new_label(s);
    s.terminated = false;

    parameters(s, fd);
    block_stmt(s, *fd.body);

    if (s.used != s.pushes)
        throw std::logic_error(
            "c1: the pushes miscount the register variables");

    if (!s.terminated && s.function == "main")
        insn(s, "movl", { imm(0), "%eax" });

    const int saved = 8 * s.pushes;
    const int size = (s.frame + 15) / 16 * 16 - saved;
    const bool sub = size > 0 && (s.calls || s.frame - saved > 128);

    const auto &name = s.function;
    s.out << std::format("\t.globl\t{}\n"
                         "\t.type\t{}, @function\n"
                         "{}:\n"
                         "\t.cfi_startproc\n",
                         name, name, name);

    prologue(s, size, sub);

    s.out << s.body.str();

    epilogue(s, sub);
    s.out << std::format("\t.cfi_endproc\n"
                         "\t.size\t{}, .-{}\n",
                         name, name);
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

// A string literal's bytes as the operand of .string: printable ASCII
// as itself but " and \ escaped, a newline, a tab and a return as the
// C escapes the assembler reads, every other byte a three-digit octal
// escape.  The directive appends the terminating zero, 6.4.5p6.
std::string
quoted_bytes(std::string_view bytes)
{
    std::string text = "\"";
    for (const char c : bytes) {
        const auto byte = static_cast< unsigned char >(c);

        if (c == '\n' || c == '\t' || c == '\r') {
            text += c == '\n' ? "\\n" : c == '\t' ? "\\t" : "\\r";
            continue;
        }

        if (byte < 0x20 || byte > 0x7e) {
            text += std::format("\\{:03o}", byte);
            continue;
        }

        if (c == '"' || c == '\\')
            text += '\\';
        text += c;
    }

    return text + '"';
}

// Chapter 10: the string literals, each its own labeled array in .data,
// never .rodata and never merged (G17), with no alignment directive,
// since a char array aligns to 1.
void
literals(const state_t &s)
{
    if (s.strings.empty())
        return;

    s.out << "\t.data\n";

    for (std::size_t i = 0; i < s.strings.size(); ++i)
        s.out << std::format(".Ls{}:\n\t.string\t{}\n", i,
                             quoted_bytes(s.strings[i]));
}

} // namespace

// The functions the unit defines, in order; one it only declares
// writes nothing, since the assembler leaves its symbol to the link.
// The string literals follow the last function.  The note section
// marks the stack not executable.
void
generate(const unit_t &unit, std::string_view file, std::ostream &out)
{
    state_t s { .out = out };

    out << std::format("\t.file\t{}\n\t.text\n", quoted_file(file));

    for (const auto &fd : unit.functions)
        if (fd.body)
            function(s, fd);

    literals(s);

    out << "\t.section\t.note.GNU-stack,\"\",@progbits\n";
}

} // namespace rcc
