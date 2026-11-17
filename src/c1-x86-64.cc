// -*- mode: c++; -*-

// c1, the code generator: the typed tree in, x86-64 assembly text out,
// AT&T syntax for the system assembler.  docs/backend.md is the design;
// its chapters 4 to 8 and 10 are what this file implements, with
// c1-common.cc, c1-stack.hh and c1-statements.hh, and each function
// names the row it writes.  It writes the frame and the code on
// integers, the conditions and the control, the switch among it, and
// the calls; pointers, arrays and string literals; the register
// variables; a function's address as a value and the call through a
// pointer, chapters 8 and 10; and an object of static storage, reached
// by its name when the file defines it and through the GOT when it
// only declares it, chapter 10.  A size past a limit of the target
// throws logic_error("c1: not yet: ..."), and a tree the parser cannot
// make throws logic_error("c1: ...").

#include "c1.hh"
#include "c1-common.hh"

#include <algorithm>
#include <array>
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

// The nine expression registers, chapter 4: the caller-saved set, in
// the order a new value takes them, rax first.  The state keeps an
// owner for each register numbered below register_count, which are
// these nine.
constexpr reg_t expression_registers[] = { rax, rcx, rdx, rsi, rdi,
                                           r8,  r9,  r10, r11 };

constexpr int register_count = 9;

constexpr reg_t variable_registers[] = { rbx, r12, r13, r14, r15 };

// The parser's register_index is below the target's count, target.hh,
// and variable_register() takes the index into this table unchecked.
static_assert(static_cast< int >(std::size(variable_registers)) ==
              x86_64.register_variables);

// The convention's argument registers, in order, 3.2.
constexpr reg_t argument_registers[] = { rdi, rsi, rdx, rcx, r8, r9 };

// The convention's result register, 3.2.
constexpr reg_t result_register = rax;

// The alignment of a loop's head, as .p2align's power of two; 0 is
// none.  The head opens a 32-byte window of the code: Zen 2 fetches 32
// bytes a cycle, and where the head falls in its window moves a hot
// loop's time (cost.md 4.2).  .p2align 6 was measured and not adopted:
// it wins one hot loop and loses another by more.
constexpr int loop_alignment = 5;

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

    // the bytes of each string literal met, in order; the index is the
    // number of its label
    std::vector< std::string > strings { };

    std::ostringstream body { };

    // each local's and parameter's displacement off rbp
    std::unordered_map< const symbol_t *, int > slots { };

    // the bytes the slots take below rbp, and whether the function
    // calls, which keeps it out of the red zone
    long frame = 0;
    bool calls = false;

    // the value each register holds, or -1; values[i] is the value i
    std::array< int, register_count > owner { };
    std::vector< held_t > values { };
    long age = 0;
    int live = 0;

    // the registers the prologue pushes, which the body's first slot
    // lies below
    int saved = 0;

    // the read of a compound assignment's register variable, which its
    // operation writes in place
    int in_place = -1;

    // the registers take() leaves for last while a value meant for a
    // pinned register is generated, so it lands there
    unsigned reserved = 0;

    // the width of the result from which the last instruction written
    // set ZF, or 0 when it set none: flagged() states it after such an
    // instruction, and the next instruction or label withdraws it
    int zero_flag = 0;

    std::string function { };
    int labels = 0;
    int ret = 0;
    bool terminated = false;

    // the statements around the one generated that a break jumps out
    // of, innermost last, and the label of each label the source
    // names, in the order met; and of each case and default label, by
    // the parser's number
    std::vector< jump_target_t > jump_targets { };
    std::unordered_map< std::string, int > user_labels { };
    std::unordered_map< int, int > case_labels { };
};

// types

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

// writing

// One instruction: a tab, the mnemonic, a tab and the operands
// separated by a comma and a space, as gcc -S writes them.  It is the
// last instruction now, so what the one before it left in ZF no longer
// stands.
void
insn(state_t &s,
     std::string_view mnemonic,
     std::initializer_list< std::string_view > operands = { })
{
    s.zero_flag = 0;

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

// The instruction just written set ZF from the result it computed at
// width, as Intel's reference has add, sub, and, or, xor, neg and test
// do, and imul, the shifts and the moves do not (backend.md 7.1).  It
// stands until the next instruction or label.
void
flagged(state_t &s, int width)
{
    s.zero_flag = width;
}

// the frame

// The next slot below the last, of size bytes at align; returns its
// displacement.  G12: a slot is never shared and never reused inside
// the activation.  Up to the target's frame limit the frame, rounded up
// to 16, is still the 32-bit immediate of subq, and every slot's
// displacement off rbp is a 32-bit one.  The parser refuses a single
// local over the limit at its declaration; a sum of smaller ones is
// refused here.
int
allocate(state_t &s, long size, int align)
{
    s.frame = (s.frame + size + align - 1) / align * align;
    if (s.frame > s.frame_limit)
        not_yet("a frame of 2 GiB or more");

    return static_cast< int >(-s.frame);
}

int
slot_of(const state_t &s, const symbol_t *sym)
{
    const auto it = s.slots.find(sym);
    if (it == s.slots.end())
        throw std::logic_error("c1: a name read before its declaration");

    return it->second;
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

// The constant row, backend.md 6.3: value into r at width, movl $c, L
// at 32 bits and movq $c, L at 64.  movq's immediate is 32 bits,
// sign-extended, so a 64-bit value outside int's range is movabsq, the
// one mov that carries 64 bits; 4294967295 as a long is one.
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

// the register stack

// The three instructions the register stack writes, called by name from
// c1-stack.hh.  The store of a register to a spill slot, all 8 bytes,
// backend.md 6.2: movq %rbx, -24(%rbp).
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

// The copy of a register into another at a width, the source first in
// AT&T order: movl %ebx, %eax.
void
mov(state_t &s, reg_t from, reg_t to, int width)
{
    insn(s, sized("mov", width), { reg(from, width), reg(to, width) });
}

reg_t take(state_t &, unsigned avoid);

#include "c1-stack.hh"

// A register for a value: the lowest free one outside avoid and
// s.reserved, else the lowest free one outside avoid, or, when every
// one is held, the register of the oldest held value outside avoid,
// which is spilled first.  The reservation is an avoidance, not a pin.
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

// Moves the value in from to the free register to, at a width: a real
// mov, and the map follows it (chapter 6.5).
void
move(state_t &s, reg_t from, reg_t to, int width)
{
    const int id = s.owner[from];
    check_movable(s, id);

    mov(s, from, to, width);

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

// the pinned instructions

// The registers the instruction of op pins, rhs its right operand,
// chapter 6.5: rax and rdx for / and %, rcx for << and >> by a count
// that is no constant, which is the immediate.
unsigned
pins(binary_op_t op, const expr_t &rhs)
{
    if (op == binary_op_t::div || op == binary_op_t::rem)
        return bit(rax) | bit(rdx);

    if (is_shift(op) && !is< literal_t >(rhs))
        return bit(rcx);

    return 0;
}

// The registers the instruction of e's own operator pins: those of a
// binary operator and of a compound assignment, and rax and rdx for
// p - q, which divides by a size other than 1 (6.3).  None for
// anything else; a call's result is in rax by the convention, but the
// call spills every held value first (chapter 8), so nothing held is
// in its way.
unsigned
pins(const expr_t &e)
{
    if (is< assign_t >(e)) {
        const auto &a = as< assign_t >(e);
        return a.op ? pins(*a.op, *a.rhs) : 0;
    }

    if (!is< binary_t >(e))
        return 0;

    const auto &b = as< binary_t >(e);
    if (b.op == binary_op_t::sub && !is< pointer_t >(e.type) &&
        is< pointer_t >(b.lhs->type))
        return stride(b.lhs->type) != 1 ? bit(rax) | bit(rdx) : 0;

    return pins(b.op, *b.rhs);
}

// The registers a node holds out of the choice for the value it holds
// while it generates rhs, its left operand or an assignment's place,
// chapter 6.5: those rhs's top operator pins, read through a cast,
// which converts in the register of its operand and pins none.  The
// value then does not stand where the instruction wants its operands.
// A division or a shift deeper in rhs changes nothing.
unsigned
pinned_by(const expr_t &rhs)
{
    const expr_t *top = &rhs;
    while (is< cast_t >(*top))
        top = as< cast_t >(*top).arg.get();

    return pins(*top);
}

// The registers an assignment holds its place out of, chapter 6.5:
// those its own instruction pins, rax and rdx for /= and %=, rcx for
// <<= and >>= by a variable count, and those its right operand's top
// operator pins, as for any node.
unsigned
pinned_by(const assign_t &a)
{
    return (a.op ? pins(*a.op, *a.rhs) : 0) | pinned_by(*a.rhs);
}

value_t rvalue(state_t &, const expr_t &, into_t * = nullptr);
void cbranch(state_t &, const expr_t &, int label, bool sense);
void effect(state_t &, const expr_t &);

// The value of e generated while the registers in pinned are left for
// last, so that it does not land where an instruction generated after
// it wants its operands (chapter 6.5).  The reservation is an
// avoidance, not a pin (take()), and a call or a division inside e
// keeps its own pin (rvalue()).
value_t
rvalue_avoiding(state_t &s,
                const expr_t &e,
                unsigned pinned,
                into_t *into = nullptr)
{
    const unsigned reserved = s.reserved;
    s.reserved |= pinned;
    const auto v = rvalue(s, e, into);
    s.reserved = reserved;

    return v;
}

// register variables

// The mov into a register variable's register, from source at the
// variable's width, that keeps the representation: the register holds
// the value extended to 32 bits by the variable's signedness, which for
// an unsigned written at 32 is its extension to 64, and a long or a
// pointer whole.  movl for an int or an unsigned, movq for a long or a
// pointer; for a narrow type the extending movsbl, movzbl, movswl or
// movzwl, where the store row has its narrow mov: G2's truncation and
// the representation in one instruction.
void
establish(state_t &s, const symbol_t *sym, std::string_view source)
{
    const auto &t = integer(sym->type);
    const auto [mnemonic, width] = extension(t, register_width(sym));

    insn(s, mnemonic, { source, reg(variable_register(sym), width) });
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

    constant(s, variable_register(sym), value, width);

    return true;
}

// The register of a value an instruction is about to write, the value
// valid from here on to valid, the width that instruction leaves, as for
// fresh().  A borrowed read is first copied into an expression register
// outside avoid, since its register is the variable; the read of a
// compound assignment's own variable, s.in_place, is the one written in
// place.
reg_t
writable(state_t &s, value_t v, int valid, unsigned avoid = 0)
{
    if (!borrowed(s, v) || v.id == s.in_place) {
        const reg_t r = reg_of(s, v, avoid);
        s.values[v.id].valid = valid;
        return r;
    }

    const reg_t r = take(s, avoid);
    copy(s, v, r);
    s.values[v.id].valid = valid;

    return r;
}

// The value was just written at width by an instruction of its type, in
// a row that takes its register by other means than writable().
void
wrote(state_t &s, value_t v, int width, bool is_signed)
{
    s.values[v.id].valid = written(width, is_signed);
}

// Before a register variable is written, each of its reads still held
// is copied into an expression register, so it keeps the value it
// read: G13, an operand's side effects complete before the next operand
// starts, and i = i++ + ++i has one value.
void
preserve(state_t &s, const symbol_t *sym)
{
    for (std::size_t id = 0; id < s.values.size(); ++id) {
        const auto &h = s.values[id];
        if (!h.live || h.variable != sym)
            continue;

        copy(s, { static_cast< int >(id) }, take(s, 0));
    }
}

// The value extended by from's signedness to a width of 32 or 64, where
// it is not valid to it already, the width rule.  A borrowed read is
// extended into a fresh register, and the value is then that one.
value_t
extended(state_t &s, value_t v, const int_t &from, int to)
{
    if (s.values[v.id].valid >= to)
        return v;

    const auto [mnemonic, width] = extension(from, std::max(32, to));
    const reg_t source = reg_of(s, v);

    if (!borrowed(s, v)) {
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
// writes it: a slot off rbp, a label off rip with a displacement of
// its own, table+12(%rip), or the machine's addressing form, a held
// base with a displacement or with a held index scaled by 1, 2, 4 or 8.
// A slot with a held index is the form with rbp the base,
// -24(%rbp,%rcx,4).  The access consumes the values.
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

// The address in AT&T syntax, d(B,I,s), rbp the base of a slot.  A
// spilled base or index is reloaded first, outside avoid and outside
// the other's register.
std::string
operand(state_t &s, const address_t &a, unsigned avoid = 0)
{
    if (!a.label.empty())
        return std::format("{}(%rip)", symbol_plus(a.label, a.displacement));

    if (a.base.id < 0 && a.index.id < 0)
        return at(static_cast< int >(a.displacement));

    avoid |= registers_of(s, a);

    std::string index;
    if (a.index.id >= 0) {
        const reg_t r = reg_of(s, a.index, avoid);
        avoid |= bit(r);
        index = std::format(",{},{}", reg(r, 64), a.scale);
    }

    const auto base =
        a.base.id < 0 ? std::string("%rbp") : reg(reg_of(s, a.base, avoid), 64);
    const auto displacement =
        a.displacement != 0 ? std::to_string(a.displacement) : std::string();

    return std::format("{}({}{})", displacement, base, index);
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
// Handed a register variable, the load writes the variable's register
// where that leaves its representation, movl -12(%rbp), %ebx (chapter
// 4).
value_t
load(state_t &s,
     const address_t &a,
     const int_t &t,
     int width,
     into_t *into = nullptr)
{
    const auto [mnemonic, to] = extension(t, width);
    const int valid = written(to, t.is_signed);

    const auto from = operand(s, a);
    release(s, a);

    if (takes(into, valid)) {
        insn(s, mnemonic, { from, reg(variable_register(into->variable), to) });
        return taken(s, *into);
    }

    const auto v = fresh(s, valid);
    insn(s, mnemonic, { from, reg(reg_of(s, v), to) });

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
// value, so &*p is p and no instruction, 6.5.4.2p3.  Handed a register
// variable, the leaq writes the variable's register, leaq buf(%rip),
// %rbx: it names its destination apart from its sources, so it reads
// every operand before it writes (chapter 4).
value_t
address_of(state_t &s, const address_t &a, into_t *into = nullptr)
{
    if (a.base.id >= 0 && a.index.id < 0 && a.displacement == 0)
        return a.base;

    const auto from = operand(s, a);
    release(s, a);

    if (takes(into, 64)) {
        insn(s, "leaq", { from, reg(variable_register(into->variable), 64) });
        return taken(s, *into);
    }

    const auto v = fresh(s, 64);
    insn(s, "leaq", { from, reg(reg_of(s, v), 64) });

    return v;
}

// The operand of an address by a name and a constant, Ritchie's &x+c,
// backend.md 6.3: the object's slot or label with the offset folded
// into its displacement, -20(%rbp) for a[3] on an int a[8] at
// -32(%rbp), table+12(%rip) for table[3], and no instruction forms the
// object's address first.  None when the displacement does not fit 32
// bits, or a label's offset lies outside its object, and the rows of
// *p and p + n reach the address.
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
    if (!fits_imm32(displacement) || (!slot && !within_object(*named)))
        return std::nullopt;

    if (slot)
        return slot_address(static_cast< int >(displacement));

    const auto label = is< string_t >(object) ?
                           string_label(s.strings, as< string_t >(object)) :
                           std::string(assembly_name(*global_of(object)));

    return address_t { .displacement = displacement, .label = label };
}

// The operand of a local array's element by a variable index,
// backend.md 6.3: the addressing form with rbp the base, the slot the
// displacement and the index scaled by the size, -24(%rbp,%rcx,4) for
// a[i] on an int a[6] at -24(%rbp); no instruction forms the array's
// address.  The array is folded()'s slot plus a constant, so m[1][i] on
// int m[2][3] at -24(%rbp) is -12(%rbp,%rcx,4).  The index is the one
// operand evaluated, so the order written holds (G13).  None for a
// label, since a PC-relative operand takes no index, for a constant
// index and for b - i, which the form does not express.
std::optional< address_t >
indexed(state_t &s, const binary_t &b, long size)
{
    const bool left = is< pointer_t >(b.lhs->type);
    const auto &index = left ? *b.rhs : *b.lhs;
    if (b.op != binary_op_t::add || is< literal_t >(index))
        return std::nullopt;

    const auto slot = folded(s, left ? *b.lhs : *b.rhs);
    if (!slot || !slot->label.empty())
        return std::nullopt;

    return address_t { .index = rvalue(s, index),
                       .scale = size,
                       .displacement = slot->displacement };
}

address_t address(state_t &, const expr_t &);

// The address of the lvalue under summed()'s decay or &: address()'s,
// but that a dereference of a pointer plus a constant index is the
// pointer's value with the index times the size as the displacement,
// whatever the size.  p[1] on int (*p)[3] is 12(%rax), where address()
// forms the row's address by the p + n row, addq $12, %rax, since the
// row's 12 bytes are no scale.  A pointer that is itself an lvalue's
// address plus a constant is address()'s, whose folds hold.
address_t
lvalue_address(state_t &s, const expr_t &lvalue)
{
    if (!is< unary_t >(lvalue) || as< unary_t >(lvalue).op != unary_op_t::deref)
        return address(s, lvalue);

    const auto &q = *as< unary_t >(lvalue).arg;
    if (!is< binary_t >(q) || as< binary_t >(q).op != binary_op_t::add ||
        lvalue_offset(q))
        return address(s, lvalue);

    const auto &b = as< binary_t >(q);
    const bool left = is< pointer_t >(b.lhs->type);
    const auto &index = left ? *b.rhs : *b.lhs;
    if (!is< literal_t >(index))
        return address(s, lvalue);

    const long size = size_of(*as< pointer_t >(q.type).pointee);
    const auto d = scaled(as< literal_t >(index).value, size);
    if (!fits_imm32(d))
        return address(s, lvalue);

    return { .base = rvalue(s, left ? *b.lhs : *b.rhs), .displacement = d };
}

// The operand of an address by any other lvalue and a constant,
// backend.md 6.3: the lvalue's own address, lvalue_address()'s, with
// the constant summed into its displacement, 2(%rax,%rcx,8) for
// m[i][2] on char (*m)[8], 20(%rax) for p[1][2] on int (*p)[3];
// nothing else is evaluated, and no instruction forms the intermediate
// address.  The lvalue is a dereference whose pointer is no name's
// address plus a constant, or a global the unit only declares, whose
// address the GOT gives.  None for a constant no 32-bit displacement
// holds, and for a name's address, folded()'s, whose bounds hold.  A
// sum that does not fit forms the lvalue's address, and the constant is
// the displacement off it.
std::optional< address_t >
summed(state_t &s, const expr_t &pointer)
{
    const auto at = lvalue_offset(pointer);
    if (!at || !fits_imm32(at->offset) || named_offset(pointer, s.defined))
        return std::nullopt;

    auto a = lvalue_address(s, *at->lvalue);

    const auto displacement = wrapped_add(a.displacement, at->offset);
    if (!fits_imm32(displacement))
        return address_t { .base = address_of(s, a),
                           .displacement = at->offset };

    a.displacement = displacement;

    return a;
}

// The address a dereference designates, 6.5.4.2p4, of an object of
// type pointee: the addressing row.  A name's address plus a constant,
// 0 for *a and *&x, is the one operand of folded(), and any other
// lvalue's address plus a constant is that address with the constant in
// its displacement, summed().  A subscript by a constant 0 is the
// bare base, the pointer's value with no displacement and no index,
// whatever the pointee's size: m[0] on int m[2][3] is m, as p[0] is p.
// Any other add of a pointer and an integer, the shape of a subscript
// (6.5.3.2p2), is the machine's addressing form when the size is a
// scale: the pointer the base, the integer the index, each generated in
// the order written (G13); a constant index is the displacement
// c * size when it fits 32 bits, and a variable index into a local
// array has rbp the base and the slot the displacement, indexed().  The
// operand written first is held outside the registers the other's top
// operator pins, p[x / y]'s p outside rax and rdx (chapter 6.5).  Any
// other pointer, an add of another size included, is computed into a
// register and is the base; nothing looks through a cast.
address_t
pointed(state_t &s, const expr_t &p, const type_t &pointee)
{
    if (const auto a = folded(s, p))
        return *a;

    // m[i][2], p[1][2]: 2(%rax,%rcx,8), 20(%rax)
    if (const auto a = summed(s, p))
        return *a;

    if (const auto *base = bare_base(p))
        return { .base = rvalue(s, *base) };

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

    // a[i], m[1][i] on a local array: -24(%rbp,%rcx,4)
    if (const auto a = indexed(s, b, size))
        return *a;

    const auto l = rvalue_avoiding(s, *b.lhs, pinned_by(*b.rhs));
    const auto r = rvalue(s, *b.rhs);

    return { .base = left ? l : r, .index = left ? r : l, .scale = size };
}

// The address of a function or of an object as a value, valid to 64,
// chapter 10.  One the unit defines is PC-relative, as a string
// literal's address is, leaq f(%rip), L.  One it only declares may be
// in a shared library, which a PC-relative reference does not reach
// from a position-independent executable, so its address is loaded from
// the GOT, movq f@GOTPCREL(%rip), L, the one the dynamic linker wrote,
// the address the library's own code takes of it.  Handed a register
// variable, either writes the variable's register, the GOT's as a load
// does (chapter 4).
value_t
symbol_address(state_t &s, const symbol_t &sym, into_t *into = nullptr)
{
    const auto name = assembly_name(sym);
    if (s.defined.contains(&sym))
        return address_of(s, { .label = std::string(name) }, into);

    const auto entry = std::format("{}@GOTPCREL(%rip)", name);
    if (takes(into, 64)) {
        insn(s, "movq", { entry, reg(variable_register(into->variable), 64) });
        return taken(s, *into);
    }

    const auto v = fresh(s, 64);
    insn(s, "movq", { entry, reg(reg_of(s, v), 64) });

    return v;
}

// The address of a global as an access's operand, chapter 10.  One the
// unit defines is its label off rip, a slot with another spelling,
// n(%rip), where a local's is -12(%rbp).  One it only declares has its
// address loaded from the GOT into a register, and the access is the
// indirection through it, (%rax), as *p's is with p loaded: each access
// the source writes loads the address once.
address_t
global_address(state_t &s, const symbol_t &g)
{
    if (s.defined.contains(&g))
        return { .label = std::string(assembly_name(g)) };

    return { .base = symbol_address(s, g) };
}

// The address of an lvalue: a local's or a parameter's slot, a global's
// label or its address from the GOT, a string literal's label, or what a
// dereference designates.  It serves &, array decay, loads and stores
// alike, so the addressing row is written once.
address_t
address(state_t &s, const expr_t &e)
{
    if (is< string_t >(e))
        return { .label = string_label(s.strings, as< string_t >(e)) };

    if (is< unary_t >(e) && as< unary_t >(e).op == unary_op_t::deref)
        return pointed(s, *as< unary_t >(e).arg, e.type);

    if (const auto *g = global_of(e))
        return global_address(s, *g);

    return slot_address(slot_of(s, object_of(e)));
}

// The address of an lvalue as a value, the row of & and of an array's
// decay: the leaq of address()'s operand, address_of()'s, and for a
// global symbol_address()'s, the load from the GOT for one the unit
// only declares.  A register variable handed down is the destination of
// either (chapter 4).
value_t
address_value(state_t &s, const expr_t &lvalue, into_t *into)
{
    if (const auto *g = global_of(lvalue))
        return symbol_address(s, *g, into);

    return address_of(s, address(s, lvalue), into);
}

// A function designator decayed to a pointer, 6.3.2.1p4: a function's
// name is its address, and *p through a pointer to a function is p's
// value, the * written as no instruction.  A register variable handed
// down goes on to the row of either.
value_t
function_pointer(state_t &s, const expr_t &designator, into_t *into)
{
    if (is< name_t >(designator))
        return symbol_address(s, *as< name_t >(designator).sym, into);

    return rvalue(s, *as< unary_t >(designator).arg, into);
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

// The object in memory that an instruction of width bits names as its
// own operand for e, backend.md 6.1, or nullptr: in_memory()'s read, of
// a type exactly that wide, so the instruction reads the bytes the load
// would, in one access at the object's width (G10).  The read may be
// under casts that keep its bits, (unsigned) i in u + i, since the
// bytes at the width are the value either way.  A char or a short
// promoted, and an int widened to 64 bits, are under a cast that
// changes the width, and keep their extending load.
const expr_t *
memory_operand(const expr_t &e, int width)
{
    const expr_t *top = &e;
    while (is< cast_t >(*top) && same_bits(as< cast_t >(*top), top->type))
        top = as< cast_t >(*top).arg.get();

    if (!in_memory(*top) || integer(top->type).width != width)
        return nullptr;

    return top;
}

// The register variable whose register an instruction of width bits
// names for e, or nullptr: a plain read of the variable, of a type
// exactly that wide, under casts that keep its bits, so its register at
// the width is the value (chapter 4).  A char or a short promoted and
// an int widened are under a cast that changes the width.
const symbol_t *
register_operand(const expr_t &e, int width)
{
    const expr_t *top = &e;
    while (is< cast_t >(*top) && same_bits(as< cast_t >(*top), top->type))
        top = as< cast_t >(*top).arg.get();

    const auto *sym = register_variable(*top);
    if (!sym || integer(top->type).width != width)
        return nullptr;

    return sym;
}

// A constant that must be held: mov $c, L, or movabsq for a 64-bit one
// no immediate holds.  A constant narrower than int is written as its
// promotion, so the register holds it extended.  Handed a register
// variable, the constant is the mov into its register, as
// constant_into() writes it (chapter 4).
value_t
literal(state_t &s, const literal_t &l, const type_t &t, into_t *into = nullptr)
{
    const auto &type = integer(t);
    const int width = std::max(32, type.width);
    const int valid = written(width, type.is_signed);
    const auto value =
        type.width < 32 ? widened(l.value, type) : normalized(l.value, width);

    if (takes(into, valid)) {
        constant(s, variable_register(into->variable), value, width);
        return taken(s, *into);
    }

    const auto v = fresh(s, valid);
    const reg_t r = reg_of(s, v);

    constant(s, r, value, width);

    return v;
}

// The store row of an initializer, chapter 6.6: e written into the
// object at to, at the object's width, its value not wanted.  A
// constant the immediate holds is the one mov $c, to, and no register
// is held; anything else is its value in a register and the store.  to
// is formed before e is evaluated, so it is a slot, or e is a constant.
void
initialize(state_t &s, const expr_t &e, std::string_view to, int width)
{
    if (immediate(e, width)) {
        insn(s, sized("mov", width), { immediate_of(e, width), to });
        return;
    }

    const auto v = rvalue(s, e);
    store(s, reg_of(s, v), to, width);
    release(s, v);
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

// The right operand of an operator's row, generated: the immediate the
// instruction takes, the value held, or the address of an object in
// memory that the instruction names in place of a register
// (binary_operand()), with the width of its type.  A binary operator
// generates it after its left operand, a compound assignment before it
// reads its object (G13); the row is the same function for both.  scale
// is the size the row still multiplies a held index by: the stride for
// the n of p + n, 1 otherwise.
struct right_t
{
    std::string immediate { };
    value_t value { };
    std::optional< address_t > place { };
    int width = 0;
    long scale = 1;
};

// The right operand is consumed: its value's register, or its address's
// registers, are free.
void
release(state_t &s, const right_t &r)
{
    release(s, r.value);
    if (r.place)
        release(s, *r.place);
}

// The address of e generated as rvalue_avoiding() generates a value:
// its registers taken with those in pinned left for last.
address_t
address_avoiding(state_t &s, const expr_t &e, unsigned pinned)
{
    const unsigned reserved = s.reserved;
    s.reserved |= pinned;
    const auto a = address(s, e);
    s.reserved = reserved;

    return a;
}

// A shift's variable count, generated with rax and avoid reserved, and
// with rcx taken out of what a node around the shift reserves for its
// own operand: the instruction consumes the count in cl, so rcx is the
// count's (chapter 6.5).
value_t
shift_count(state_t &s, const expr_t &rhs, unsigned avoid)
{
    const unsigned outer = std::exchange(s.reserved, s.reserved & ~bit(rcx));
    const auto v = rvalue_avoiding(s, rhs, avoid | bit(rax));
    s.reserved = outer;

    return v;
}

// The right operand of op in an operation of width bits, as its row
// takes it.  A constant is an immediate where the instruction takes
// one, and then nothing is evaluated: the arithmetic and bitwise
// operators' constant as it fits 32 bits, a shift's count as its low 8
// bits, which is what the instruction encodes.  A divisor is always
// held, since idiv takes no immediate.  A divisor is generated with
// rax and rdx reserved, the two registers its instruction pins, and a
// variable count by shift_count(), so that a dividend or a shifted
// value loaded after it finds rax and a divisor never stands where
// idiv writes; a count's load lands in rcx, the register the
// instruction wants.  avoid is reserved the same way for any other
// operand: the registers a left operand loaded after it is to take,
// rax for a compound assignment's object in memory, whose register is
// the result's.
right_t
right_operand(state_t &s,
              binary_op_t op,
              const expr_t &rhs,
              int width,
              unsigned avoid = 0)
{
    if (op == binary_op_t::div || op == binary_op_t::rem)
        return { .value = rvalue_avoiding(s, rhs, avoid | pins(op, rhs)),
                 .width = operation_width(rhs.type) };

    if (is_shift(op)) {
        if (is< literal_t >(rhs))
            return { .immediate = imm(as< literal_t >(rhs).value & 0xff) };

        return { .value = shift_count(s, rhs, avoid),
                 .width = operation_width(rhs.type) };
    }

    if (immediate(rhs, width))
        return { .immediate = immediate_of(rhs, width) };

    return { .value = rvalue_avoiding(s, rhs, avoid),
             .width = operation_width(rhs.type) };
}

// The right operand of a binary operator's instruction, backend.md 6.1:
// right_operand()'s, but that an object in memory of the operation's
// width is the instruction's own operand, addl -8(%rbp), %eax,
// idivl -8(%rbp), and no load writes it into a register.  Its address
// is generated where the load would be, after the left operand, and is
// held to the instruction, which reads the object, so the reads keep
// their order (G13).  A divisor's address is generated with rax and rdx
// reserved, as a divisor is.  A shift's count keeps its register, cl.
// The rule is the Seventh Edition compiler's: the row %n,aw of its
// table's cr40 writes the left operand into the register and the
// instruction with the right operand's address.
right_t
binary_operand(state_t &s, binary_op_t op, const expr_t &rhs, int width)
{
    const auto *object = is_shift(op) ? nullptr : memory_operand(rhs, width);
    if (!object)
        return right_operand(s, op, rhs, width);

    return { .place = address_avoiding(s, *object, pins(op, rhs)),
             .width = width };
}

// G4: the instruction the machine has, idiv or div, a fault on x86 for
// a zero divisor or INT_MIN / -1.  rax and rdx hold nothing but, it may
// be, the dividend in left: it goes to rax, rdx is cleared or
// sign-filled, and the division of the divisor's operand leaves the
// quotient in rax and the remainder in rdx.
void
idiv(state_t &s, reg_t left, std::string_view divisor, const int_t &t)
{
    const int width = t.width;

    if (left != rax)
        insn(s, sized("mov", width), { reg(left, width), reg(rax, width) });

    if (!t.is_signed)
        insn(s, "xorl", { "%edx", "%edx" });
    else
        insn(s, width == 32 ? "cltd" : "cqto");

    insn(s, sized(t.is_signed ? "idiv" : "div", width), { divisor });
}

// The register a held value is in, as a set for take() to avoid; none
// for a value spilled.
unsigned
held_in(const state_t &s, value_t v)
{
    const int r = s.values[v.id].reg;

    return r >= 0 ? bit(static_cast< reg_t >(r)) : 0;
}

// The register a division's dividend is in once rax and rdx are
// vacated, chapter 6.5: the one it is held in, or rax for a dividend
// spilled, which is reloaded there and not into another register to be
// moved again.  A dividend held in rdx, which the division overwrites,
// moves to rax for a quotient by the mov idiv() writes in any case, so
// that L is rax and the quotient stays where idiv leaves it.  A
// remainder's value is rdx whatever L is, so its dividend stays.
reg_t
dividend(state_t &s, value_t l, binary_op_t op, int width)
{
    const reg_t r = reg_of(s, l, ~bit(rax));
    if (r != rdx || op != binary_op_t::div)
        return r;

    move(s, rdx, rax, width);

    return rax;
}

// The pinned pair of a division by a held divisor, chapter 6.5: nothing
// looks at the divisor, which is in a register even when it is a
// constant.  The divisor moves out of rax or rdx first, then the values
// held there, and a dividend spilled is reloaded into rax.  Both values
// are still held after idiv(), the dividend in L, the register
// returned.  A register variable as the dividend is moved to rax and as
// the divisor is the operand of idiv.  op is the result the caller
// takes, / or %, which dividend() reads.
reg_t
pin_division(state_t &s, binary_op_t op, value_t l, value_t r, const int_t &t)
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

    const reg_t left = dividend(s, l, op, t.width);
    idiv(s, left, reg(right, t.width), t);

    return left;
}

// The pinned pair of a division by an object in memory, chapter 6.5:
// the object is idiv's operand, idivl -8(%rbp).  The registers of its
// address are held values as any other: one in rax or rdx moves out
// first, with the values held there, and the operand is formed after
// the moves, from the registers they went to.  A dividend spilled is
// reloaded into rax after them.  The dividend is held in L, the
// register returned, and the address is the caller's to free.
reg_t
pin_division(state_t &s,
             binary_op_t op,
             value_t l,
             const address_t &divisor,
             const int_t &t)
{
    const unsigned pinned = bit(rax) | bit(rdx);
    const unsigned in = held_in(s, l);

    for (const reg_t p : { rax, rdx })
        if (!(in & bit(p)))
            vacate(s, p, pinned | in | registers_of(s, divisor));

    const reg_t left = dividend(s, l, op, t.width);
    idiv(s, left, operand(s, divisor, pinned | bit(left)), t);

    return left;
}

// / and %: the division, the quotient back into L and the remainder
// where the instruction leaves it.  Both operands are generated by the
// caller in its order, the divisor held or, by binary_operand(), an
// object's address; right_operand() keeps a divisor and its address
// out of rax and rdx when it can, for a dividend loaded after it.  A
// remainder's value is the value in rdx, and L is freed (chapter 6.5),
// as the Seventh Edition compiler's row of a remainder returns the odd
// register of its pair, where div leaves it.  The quotient of a
// dividend that is a borrowed read is a new value in rax, since L is
// the variable.
value_t
divide(state_t &s, binary_op_t op, value_t l, const right_t &r, const int_t &t)
{
    const int width = t.width;

    const reg_t left = r.place ? pin_division(s, op, l, *r.place, t) :
                                 pin_division(s, op, l, r.value, t);

    if (op == binary_op_t::rem || borrowed(s, l)) {
        release(s, r);
        release(s, l);
        return hold(s, op == binary_op_t::rem ? rdx : rax,
                    written(width, t.is_signed));
    }

    if (left != rax)
        insn(s, sized("mov", width), { reg(rax, width), reg(left, width) });

    release(s, r);
    wrote(s, l, width, t.is_signed);

    return l;
}

// Puts a shift's count, held in right, into cl, chapter 6.5: a value
// held in rcx moves out first, to a register outside avoid, and the
// count is copied there if it is a borrowed read of a register
// variable and moved otherwise, at its own width.
void
count_into_cl(state_t &s, const right_t &r, reg_t right, unsigned avoid)
{
    if (right == rcx)
        return;

    vacate(s, rcx, avoid | bit(right));

    if (borrowed(s, r.value))
        copy(s, r.value, rcx);
    else
        move(s, right, rcx, r.width);
}

// G3: the instruction masks the count to the width, and nothing else
// does.  A constant count is the immediate; a variable one goes to cl,
// at the count's own width, which the parser leaves at its promoted
// type, a value held in rcx moving out first (chapter 6.5), a register
// variable copied there.  binary() generated the left operand with rcx
// reserved and right_operand() the count where cl is likely, and a
// left operand reloaded from its spill slot avoids rcx; the moves of
// chapter 6.5 remain for a value that is already there.
const char *
shift_mnemonic(binary_op_t op, bool is_signed)
{
    return op == binary_op_t::shl ? "shl" : is_signed ? "sar" : "shr";
}

value_t
shift(state_t &s, binary_op_t op, value_t l, const right_t &r, const int_t &t)
{
    const int width = t.width;
    const auto stem = shift_mnemonic(op, t.is_signed);

    if (!r.immediate.empty()) {
        insn(s, sized(stem, width),
             { r.immediate,
               reg(writable(s, l, written(width, t.is_signed)), width) });
        return l;
    }

    const int held = s.values[l.id].reg;
    const reg_t right =
        reg_of(s, r.value, held >= 0 ? bit(static_cast< reg_t >(held)) : 0);
    reg_t left =
        writable(s, l, written(width, t.is_signed), bit(right) | bit(rcx));

    if (left == rcx) {
        const reg_t to = take(s, bit(rcx) | bit(right));
        move(s, left, to, 64);
        left = to;
    }

    count_into_cl(s, r, right, bit(left));

    insn(s, sized(stem, width), { "%cl", reg(left, width) });

    release(s, r.value);

    return l;
}

// op obj, L: the instruction on the held left value l with the object
// at place as its source, the instruction's own operand, backend.md
// 6.1, read by it, addl -8(%rbp), %eax.  L is written, valid as valid
// says, and the address is consumed.
void
from_memory(state_t &s,
            std::string_view mnemonic,
            value_t l,
            const address_t &place,
            int valid,
            int width)
{
    const reg_t left = writable(s, l, valid, registers_of(s, place));
    insn(s, mnemonic, { operand(s, place, bit(left)), reg(left, width) });

    release(s, place);
}

// An arithmetic, bitwise or shift operator on the held left value and
// the generated right operand, in type t, chapter 6.1: op R, L, the
// result in L.  A constant right operand is an immediate where the
// instruction takes one; imul's is the three-operand form.  An object
// in memory of binary_operand()'s is the instruction's operand, op
// obj, L, read by it, imull -8(%rbp), %eax.  G1: add, sub, imul and shl
// wrap; imul is the same instruction for unsigned, the low bits agree.
// A register variable is R as it is; as L it is copied first, unless it
// is the variable a compound assignment writes in place.  add, sub,
// and, or and xor set ZF from L, and imul does not (7.1).
value_t
arithmetic(
    state_t &s, binary_op_t op, value_t l, const right_t &r, const type_t &t)
{
    const int width = operation_width(t);

    if (op == binary_op_t::div || op == binary_op_t::rem)
        return divide(s, op, l, r, integer(t));

    if (is_shift(op))
        return shift(s, op, l, r, integer(t));

    const auto mnemonic = sized(arithmetic_mnemonic(op), width);

    if (!r.immediate.empty()) {
        const auto left =
            reg(writable(s, l, written(width, integer(t).is_signed)), width);

        if (op == binary_op_t::mul) {
            insn(s, mnemonic, { r.immediate, left, left });
            return l;
        }

        insn(s, mnemonic, { r.immediate, left });
        flagged(s, width);

        return l;
    }

    if (r.place) {
        from_memory(s, mnemonic, l, *r.place,
                    written(width, integer(t).is_signed), width);
        if (op != binary_op_t::mul)
            flagged(s, width);

        return l;
    }

    const reg_t right = operands(s, l, r.value).second;
    const reg_t left =
        writable(s, l, written(width, integer(t).is_signed), bit(right));
    insn(s, mnemonic, { reg(right, width), reg(left, width) });
    if (op != binary_op_t::mul)
        flagged(s, width);

    release(s, r.value);

    return l;
}

// The arithmetic row of p + n, n + p and p - n, 6.5.7p9, both values
// held and the result in L.  G6: the address plus or minus n times
// the size, modulo 2^64.  A size of 1 is add or sub; + with another
// scale is one leaq (P,N,s); - with a scale, and either with any other
// size, multiplies the index by the size first.  L and R are as the
// source wrote the operands: n + p has the index in L.  A register
// variable handed down is the destination of the leaq (chapter 4).
value_t
offset(state_t &s,
       binary_op_t op,
       value_t l,
       value_t r,
       long size,
       bool pointer_first,
       into_t *into = nullptr)
{
    const auto mnemonic = op == binary_op_t::add ? "addq" : "subq";

    if (size != 1 && op == binary_op_t::add && is_scale(size))
        return address_of(s,
                          { .base = pointer_first ? l : r,
                            .index = pointer_first ? r : l,
                            .scale = size },
                          into);

    auto [left, right] = operands(s, l, r);

    // the multiply writes the index, and the add or sub writes L
    if (size != 1) {
        auto &index = pointer_first ? right : left;
        index = writable(s, pointer_first ? r : l, 64,
                         bit(pointer_first ? left : right));
        insn(s, "imulq", { imm(size), reg(index, 64), reg(index, 64) });
    }

    left = writable(s, l, 64, bit(right));
    insn(s, mnemonic, { reg(right, 64), reg(left, 64) });

    release(s, r);

    return l;
}

// Whether the row of p + n, n + p or p - n multiplies a held n by the
// size, 6.3: at any size but 1, except for + at a size of 2, 4 or 8,
// whose leaq (P,N,s) scales n.
bool
multiplies(binary_op_t op, long size)
{
    return size != 1 && !(op == binary_op_t::add && is_scale(size));
}

// The n of a row that multiplies() it, n times the size, when n is an
// object in memory of 64 bits: the three-operand imul with the object
// as its source, imulq $4, -16(%rbp), %rcx, where the load and the
// imulq on its register were, so the object is read once, where its
// load would read it (backend.md 6.1, G13).  The product takes a fresh
// register outside avoid, or, handed a register variable, the
// variable's, imulq $12, -24(%rbp), %rbx (chapter 4).  None for any
// other n: an int widened keeps its extending load, and the row
// multiplies n in its register.
std::optional< value_t >
multiplied(state_t &s,
           const expr_t &n,
           long size,
           unsigned avoid,
           into_t *into = nullptr)
{
    const auto *object = memory_operand(n, 64);
    if (!object)
        return std::nullopt;

    const auto place = address_avoiding(s, *object, avoid);
    const auto from = operand(s, place);
    release(s, place);

    if (takes(into, 64)) {
        insn(s, "imulq",
             { imm(size), from, reg(variable_register(into->variable), 64) });
        return taken(s, *into);
    }

    const auto v = fresh(s, 64, avoid);
    insn(s, "imulq", { imm(size), from, reg(reg_of(s, v), 64) });

    return v;
}

// The n of p op n, and of p op= n, generated.  A constant n is the
// immediate c * size, or, when no 32-bit immediate holds the product,
// the product held in a register; the parser retyped a constant index
// to 64 bits, so p += 2 on an int * is addq $8.  An n the row
// multiplies is its product where multiplied() writes it, already
// scaled.  Any other n is held, for displaced() to scale.  avoid is as
// for right_operand(): the registers a pointer loaded after n is to
// take.
right_t
displacement(
    state_t &s, binary_op_t op, const expr_t &n, long size, unsigned avoid = 0)
{
    if (!is< literal_t >(n)) {
        if (multiplies(op, size))
            if (const auto product = multiplied(s, n, size, avoid))
                return { .value = *product, .width = 64 };

        return { .value = rvalue_avoiding(s, n, avoid),
                 .width = 64,
                 .scale = size };
    }

    const auto d = scaled(as< literal_t >(n).value, size);
    if (fits_imm32(d))
        return { .immediate = imm(d) };

    const auto r = fresh(s, 64, avoid);
    constant(s, reg_of(s, r), d, 64);

    return { .value = r, .width = 64 };
}

// The n of p + n and p - n as their instruction takes it: at a size of
// 1, where add and sub take n unscaled, an object in memory of 64 bits
// is the instruction's own operand, addq -16(%rbp), %rax, its address
// generated where its load would be, after p (backend.md 6.1); any
// other n is displacement()'s.  At another size n is scaled in a
// register: the index of leaq keeps its load, and the imulq's product
// takes the object as its source.  A compound assignment's n is held
// in any case, since it is evaluated before the object is read (6.6).
right_t
addend(state_t &s, binary_op_t op, const expr_t &n, long size)
{
    if (size == 1)
        if (const auto *object = memory_operand(n, 64))
            return { .place = address(s, *object), .width = 64 };

    return displacement(s, op, n, size);
}

// p + n and p - n, p += n and p -= n, on the held pointer l and the
// generated n: addq $8, L for an immediate, addq -16(%rbp), L for an
// object in memory of addend()'s, else the offset row, which a
// register variable handed down goes on to.
value_t
displaced(state_t &s,
          binary_op_t op,
          value_t l,
          const right_t &n,
          into_t *into = nullptr)
{
    const auto mnemonic = op == binary_op_t::add ? "addq" : "subq";

    if (!n.immediate.empty()) {
        insn(s, mnemonic, { n.immediate, reg(writable(s, l, 64), 64) });
        return l;
    }

    if (n.place) {
        from_memory(s, mnemonic, l, *n.place, 64, 64);
        return l;
    }

    return offset(s, op, l, n.value, n.scale, true, into);
}

// n + p with p an object in memory of 64 bits, where the add takes p
// unscaled: at a size of 1, and at a size no addressing form scales by,
// after the index is multiplied in its register, imulq $12, %rax, %rax.
// p is the add's own operand, addq -16(%rbp), %rax (backend.md 6.1),
// its address generated where its load would be, after n, so the reads
// keep their order (G13).  None at a size of 2, 4 or 8, whose leaq
// takes p as its base, a register.
std::optional< value_t >
index_first(state_t &s, value_t n, const expr_t &p, long size)
{
    const auto *object = memory_operand(p, 64);
    if (!object || (size != 1 && is_scale(size)))
        return std::nullopt;

    const auto place = address(s, *object);

    if (size != 1) {
        const reg_t index = writable(s, n, 64, registers_of(s, place));
        insn(s, "imulq", { imm(size), reg(index, 64), reg(index, 64) });
    }

    from_memory(s, "addq", n, place, 64, 64);

    return n;
}

// Whether the row of a binary + or - of a pointer type, outside the
// forms of folded(), summed() and indexed(), ends in offset()'s leaq
// (P,N,s): an add at a size of 2, 4 or 8 whose index is held.  A
// constant on the pointer's right is not: it is the addq's immediate, or
// its product held, which the addq takes unscaled.
bool
scaled_sum(const operation_t &b, long size)
{
    const bool pointer_first = is< pointer_t >(b.lhs->type);

    return b.op == binary_op_t::add && size != 1 && is_scale(size) &&
           !(pointer_first && is< literal_t >(*b.rhs));
}

// The row of a binary + or - of a pointer type past the forms of
// folded(), summed() and indexed(), its operands in the order of
// ordered_sum(), on its left operand l, held: the right operand
// generated after it, an object in memory the add or the sub takes
// unscaled the instruction's own operand, addq -16(%rbp), %rax (6.1),
// by addend() and index_first(); then the instruction on l's register,
// or offset()'s leaq, which a register variable handed down is the
// destination of.  size is the stride, but 1 for n + p whose n is
// multiplied() already.
value_t
pointer_operation(state_t &s,
                  const operation_t &b,
                  long size,
                  value_t l,
                  into_t *into = nullptr)
{
    if (is< pointer_t >(b.lhs->type))
        return displaced(s, b.op, l, addend(s, b.op, *b.rhs, size), into);

    if (const auto sum = index_first(s, l, *b.rhs, size))
        return *sum;

    return offset(s, b.op, l, rvalue(s, *b.rhs), size, false, into);
}

// The subq of p - q on the held p, l, and q generated after it: an
// object in memory of 64 bits as q is the instruction's own operand,
// subq -16(%rbp), %rax (backend.md 6.1), whatever the size, since the
// difference of the addresses is taken before its division.  q, or the
// address of its object, is generated with the registers in pinned left
// for last, rax and rdx when a division follows, as a divisor is
// (chapter 6.5), so the copy of a register variable as p takes rax.
void
subtract_pointer(state_t &s, value_t l, const expr_t &q, unsigned pinned)
{
    if (const auto *object = memory_operand(q, 64)) {
        from_memory(s, "subq", l, address_avoiding(s, *object, pinned), 64, 64);
        return;
    }

    const auto r = rvalue_avoiding(s, q, pinned);
    const reg_t right = operands(s, l, r).second;
    const reg_t left = writable(s, l, 64, bit(right));
    insn(s, "subq", { reg(right, 64), reg(left, 64) });

    release(s, r);
}

// p - q, 6.5.7p10: the difference of the addresses divided by the size,
// a long.  G6: a division, idivq by the size held in a register, not a
// shift, since the two pointers are any two addresses; a size of 1 is
// the subq alone.  q and the size are taken with rax and rdx reserved,
// as any divisor is (chapter 6.5), so the difference is in rax when rax
// is free, and the quotient stays there.
value_t
difference(state_t &s, const binary_t &b)
{
    const long size = stride(b.lhs->type);
    const unsigned pinned = size != 1 ? bit(rax) | bit(rdx) : 0;

    const auto l = rvalue_avoiding(s, *b.lhs, pinned_by(*b.rhs));
    subtract_pointer(s, l, *b.rhs, pinned);

    if (size == 1)
        return l;

    const unsigned reserved = std::exchange(s.reserved, s.reserved | pinned);
    const auto d = fresh(s, 64);
    s.reserved = reserved;

    constant(s, reg_of(s, d), size, 64);

    return divide(s, binary_op_t::div, l, { .value = d, .width = 64 },
                  int_t { 64, true });
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

// cmp R, L of the held left value l and the right operand rhs, at
// width, a constant right operand as an immediate and an object in
// memory of the width as the instruction's operand, cmpl -8(%rbp),
// %eax, its address generated after l and the object read by the cmp
// (backend.md 6.1).  The flags are set and the right operand freed; l
// stays held.
void
compare_with(state_t &s, value_t l, const expr_t &rhs, int width)
{
    const auto mnemonic = sized("cmp", width);

    if (immediate(rhs, width)) {
        insn(s, mnemonic,
             { immediate_of(rhs, width), reg(reg_of(s, l), width) });
        return;
    }

    if (const auto *object = memory_operand(rhs, width)) {
        const auto place = address(s, *object);
        const reg_t left = reg_of(s, l, registers_of(s, place));
        insn(s, mnemonic, { operand(s, place, bit(left)), reg(left, width) });
        release(s, place);
        return;
    }

    const auto r = rvalue(s, rhs);
    const auto [left, right] = operands(s, l, r);
    insn(s, mnemonic, { reg(right, width), reg(left, width) });

    release(s, r);
}

// A comparison, or an operation of +, *, &, | or ^, in the order its
// row generates it, backend.md 6.1: a constant on the left that the
// instruction takes as an immediate is the immediate on the right, the
// comparison mirrored, so 5 < x is cmpl $5, -4(%rbp) and 0 == (c & 1)
// the flags of the and, and 4 * x is imull $4, -4(%rbp), %eax.  A
// constant no 32-bit immediate holds is held in a register in either
// order, so it keeps the order of the source: on the right, it would
// cost a load of an object in memory that is the instruction's own
// operand where it stands.  A pointer's sum keeps the rows of p + n.
operation_t
ordered(const binary_t &b)
{
    const int width = operation_width(b.lhs->type);

    if (constant_first(b) && immediate(*b.lhs, width))
        return mirrored(b);

    return as_written(b);
}

// A condition on an object in memory and an immediate, 7.1, and a
// comparison's value on the same operands, 6.3: cmp or test of the
// immediate source and the object itself, at the object's width,
// cmpl $5, -4(%rbp), cmpb $0, -1(%rbp), testl $1, -4(%rbp), and no
// register holds the object.  The instruction reads the object after
// its address, where its load would, and the constant has no
// evaluation, so nothing comes between and the reads keep their order
// (G13).  The Seventh Edition compiler's cctab has the rows: cc60's
// %a,z, tst _x, and %aw,aw, cmp $5,_x, and cc81's %a,a, bit $1,_x.
void
condition_on(state_t &s,
             std::string_view stem,
             const expr_t &object,
             std::string_view source,
             int width)
{
    const auto place = address(s, object);
    insn(s, sized(stem, width), { source, operand(s, place) });
    release(s, place);
}

// cmp R, L for a relational or equality operator, the left value
// generated outside the registers the right one's operator pins
// (chapter 6.5).  The left value stays held, for setcc or for the
// caller to free.
value_t
compare(state_t &s, const operation_t &b)
{
    const auto l = rvalue_avoiding(s, *b.lhs, pinned_by(*b.rhs));
    compare_with(s, l, *b.rhs, operation_width(b.lhs->type));

    return l;
}

// The cmp of a comparison, its operands in the order of ordered(): an
// object in memory against an immediate is the cmp of the object in
// place, condition_on()'s, cmpl $5, -4(%rbp), and against a register
// variable's plain read the same with the variable's register as the
// source, cmpl %ebx, -4(%rbp) for x < r: the object is read first, by
// the cmp, which reads the register too, and nothing is evaluated
// between (G13).  Nothing is held then, for a condition (7.1) and for
// a value (6.3) alike; any other is compare()'s cmp R, L, and L is
// held.
value_t
compared(state_t &s, const operation_t &b)
{
    const int width = operation_width(b.lhs->type);

    const auto *object = memory_operand(*b.lhs, width);
    if (object && immediate(*b.rhs, width)) {
        condition_on(s, "cmp", *object, immediate_of(*b.rhs, width), width);
        return { };
    }

    if (const auto *sym = object ? register_operand(*b.rhs, width) : nullptr) {
        condition_on(s, "cmp", *object, reg(variable_register(sym), width),
                     width);
        return { };
    }

    return compare(s, b);
}

// The jump to label on the flags a comparison set, when op holds: jcc
// of the condition, chapter 7.1.
void
jump_if(state_t &s, binary_op_t op, bool is_signed, int label)
{
    insn(s, std::format("j{}", condition(op, is_signed)),
         { label_name(s.function, label) });
}

// A comparison as a value: cmp, setcc into the low byte of L, movzbl
// to the int the operator yields, valid to 64 bits, truth_valid.  A
// register variable as L is only read by the cmp, and an object in
// memory compared in place is read by it (compared()); the setcc then
// takes a fresh register.  Handed a register variable, the setcc and
// the movzbl write its register, setl %bl; movzbl %bl, %ebx, an int's,
// a long's and an unsigned long's: the cmp has read both operands
// before either writes, the variable among them (chapter 4).
value_t
comparison(state_t &s, const operation_t &b, into_t *into = nullptr)
{
    const bool is_signed = integer(b.lhs->type).is_signed;
    const auto set = std::format("set{}", condition(b.op, is_signed));

    auto l = compared(s, b);
    if (takes(into, truth_valid)) {
        release(s, l);

        const reg_t r = variable_register(into->variable);
        insn(s, set, { reg(r, 8) });
        insn(s, "movzbl", { reg(r, 8), reg(r, 32) });

        return taken(s, *into);
    }

    if (l.id < 0 || borrowed(s, l)) {
        release(s, l);
        l = fresh(s, 32);
    }

    const reg_t r = reg_of(s, l);

    insn(s, set, { reg(r, 8) });
    insn(s, "movzbl", { reg(r, 8), reg(r, 32) });
    s.values[l.id].valid = truth_valid;

    return l;
}

// && and || as a value: jump code into one register on both paths,
// movl $1 and movl $0, the value valid to 64 bits, truth_valid.
// before_paths() first gives every held value one location on both, so
// the paths leave the register map as they found it whatever their
// operands do.
value_t
logical(state_t &s, const expr_t &e)
{
    const auto incoming = before_paths(s, e);

    const int no = new_label(s);
    const int end = new_label(s);

    cbranch(s, e, no, false);

    const auto v = fresh(s, truth_valid);
    const reg_t r = reg_of(s, v);

    constant(s, r, 1, 32);
    jump(s, end);

    place_label(s, no);
    constant(s, r, 0, 32);

    place_label(s, end);

    return v;
}

// A conditional whose value is not wanted, a void one or one in the
// effect context: the jump code of 6.5.16p5, and each arm for its
// effect, chapter 6.6, so a constant assigned in an arm is one store.
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
// never cmov.  Each arm's value is moved into the register of the
// first arm's, which is an expression register: a register variable as
// the first arm is copied into one.
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
        insn(s, sized("mov", width), { reg(from, width), reg(into, width) });
        other_valid = written(width, integer(t).is_signed);
    }
    release(s, other);

    place_label(s, end);

    // valid to what both arms left
    return hold(s, into, std::min(valid, other_valid));
}

// Whether a pointer reaches its object by one operand that evaluates
// nothing but, it may be, the load of one named pointer: the pointer a
// name, *p; a named object's address plus a constant, a[3] on an array
// and m[1][2], whose operand is the slot or the label with the offset
// folded in; or a named pointer, or such an address's dereference, plus
// a constant, p[3] and p[1][2], whose operand is the pointer's register
// with the constant as its displacement, 12(%rax), as *p's is (%rax).
bool
named_pointer(const expr_t &p)
{
    if (is< name_t >(p))
        return true;

    if (const auto at = lvalue_offset(p)) {
        const auto &lvalue = *at->lvalue;
        if (is< name_t >(lvalue))
            return true;

        return is< unary_t >(lvalue) &&
               as< unary_t >(lvalue).op == unary_op_t::deref &&
               named_pointer(*as< unary_t >(lvalue).arg);
    }

    if (!is< binary_t >(p) || !is< pointer_t >(p.type))
        return false;

    const auto &b = as< binary_t >(p);
    const bool left = is< pointer_t >(b.lhs->type);
    const auto &pointer = left ? *b.lhs : *b.rhs;
    const auto &n = left ? *b.rhs : *b.lhs;

    return b.op == binary_op_t::add && is< literal_t >(n) &&
           is< name_t >(pointer);
}

// Whether the right operand of the operation form is a leaf, the
// Seventh Edition compiler's operand of degree 12 or less (dcalc(), in
// c11.c): a constant, the read of a named object, a register variable,
// or the object a named_pointer() reaches, *p, a[3] or p[3], read
// through the parser's conversions, so that a char promoted is one.
bool
leaf(const expr_t &e)
{
    const expr_t *top = &e;
    while (is< cast_t >(*top))
        top = as< cast_t >(*top).arg.get();

    if (is< literal_t >(*top) || is< name_t >(*top))
        return true;

    return is< unary_t >(*top) && as< unary_t >(*top).op == unary_op_t::deref &&
           named_pointer(*as< unary_t >(*top).arg);
}

// The Seventh Edition compiler's two conditions on R of the operation
// form, which the instruction reads after L has written the variable's
// register (sreorder(), in c10.c): R is a leaf, and R does not name the
// variable, as itself or as the base of its address.
bool
leaf_apart_from(const expr_t &rhs, const symbol_t *sym)
{
    return leaf(rhs) && !names(rhs, sym);
}

bool operates_in(const operation_t &, const type_t &, const into_t &);
operation_t ordered(const binary_t &, const type_t &, const into_t *);

// Whether L of the operation form goes into the variable's register by
// an instruction it has in any case: its load, its constant, or again
// the operation form, each written into the register; the read of
// another register variable, which an instruction that writes is given
// a copy of (chapter 4), copied there; the variable itself, already
// there.  Any other L leaves the whole assignment to the mov of
// establish() after it: moved in before the instruction, L would leave
// the assignment's value in the variable's register, where the mov
// after leaves it in an expression register, and a use of the value in
// one would copy it back, an instruction more.  A row computing the
// value in another signedness, into's as, takes no L under a
// conversion, as through() hands it on through none.
bool
loads_into(const expr_t &lhs, const into_t &into)
{
    if (is< literal_t >(lhs) || is< name_t >(lhs) || in_memory(lhs))
        return true;

    if (is< cast_t >(lhs)) {
        const auto &arg = *as< cast_t >(lhs).arg;
        return !into.as &&
               (is< int_t >(arg.type) || is< pointer_t >(arg.type)) &&
               in_memory(arg) &&
               integer(lhs.type).width >= represented(into.variable);
    }

    if (!is< binary_t >(lhs))
        return false;

    const auto &b = as< binary_t >(lhs);

    return operates_in(ordered(b, lhs.type, &into), lhs.type, into);
}

// Whether r = L op R, of type t, its operands in the order of
// ordered(), is the operation on the variable's register, chapter 4:
// an operator of two addresses on a free register, +, -, &, |, ^, * or
// a shift, on the type the variable's value is computed in,
// computed_type(), no pointer; L that loads_into()
// the register; and the Seventh Edition compiler's two conditions on R,
// which the instruction reads after L has written the register
// (sreorder(), in c10.c): R is a leaf, and R does not name the
// variable, as itself or as the base of its address.
bool
operates_in(const operation_t &b, const type_t &t, const into_t &into)
{
    switch (b.op) {
    case binary_op_t::add:
    case binary_op_t::sub:
    case binary_op_t::mul:
    case binary_op_t::bit_and:
    case binary_op_t::bit_or:
    case binary_op_t::bit_xor:
    case binary_op_t::shl:
    case binary_op_t::shr: break;
    default: return false;
    }

    if (is< pointer_t >(t) || is< pointer_t >(b.lhs->type) ||
        !(t == computed_type(into)))
        return false;

    return leaf_apart_from(*b.rhs, into.variable) && loads_into(*b.lhs, into);
}

// An operation of type t in the order its row generates it, ordered()'s,
// when its value goes to the register variable into hands down, or to
// nothing when into is null.  A constant on the left that ordered()
// takes on the right stays on the left where that order alone is the
// operation on the variable's register (chapter 4): r = 5 + c on a
// register char c has the constant as L, which its mov puts in the
// register, where (int) c as L is no load into the register, and the
// sum would be computed elsewhere and moved, an instruction more.
operation_t
ordered(const binary_t &b, const type_t &t, const into_t *into)
{
    const auto o = ordered(b);
    if (!into || o.lhs == b.lhs.get() || operates_in(o, t, *into))
        return o;

    const auto source = as_written(b);

    return operates_in(source, t, *into) ? source : o;
}

// Whether r = P + N, N + P or P - N, of pointer type t, is the operation
// on the variable's register, chapter 4: a row whose last instruction is
// the addq or the subq on L, so not offset()'s leaq, which a register
// variable handed down is the destination of; on the variable's own
// type; L that loads_into() the register; and the two conditions on R.
// The index is multiplied by a size no addressing form scales by in its
// own register: R's, imulq $12, %rcx, %rcx, or the variable's for the
// index of N + P.
bool
offsets_in(const operation_t &b, const type_t &t, const into_t &into)
{
    if (scaled_sum(b, stride(t)) || !(t == computed_type(into)))
        return false;

    return leaf_apart_from(*b.rhs, into.variable) && loads_into(*b.lhs, into);
}

// L of the operation form into the variable's register, by the
// instruction loads_into() names: a register variable's read copied,
// movl %r12d, %ebx, the variable itself nothing, and anything else by
// its own row, handed the variable as the operation computes it.  The
// value is the variable's read.
value_t
left_in(state_t &s, const expr_t &lhs, const into_t &to, unsigned avoid)
{
    const auto *sym = to.variable;

    if (const auto *other = register_variable(lhs)) {
        if (other != sym)
            mov(s, variable_register(other), variable_register(sym),
                operation_width(lhs.type));
        return read(s, sym);
    }

    into_t into { sym, false, to.as };
    const auto v = rvalue_avoiding(s, lhs, avoid, &into);
    if (!into.taken)
        throw std::logic_error("c1: an operand missed its register variable");

    return v;
}

// r = L op R as the operation on the variable's register, chapter 4,
// the Seventh Edition compiler's sreorder(), which rewrites reg = x + y
// as reg = x; reg =+ y: L into the register, then R as 6.1 generates
// it, an immediate, an object in memory or a value held, and the
// instruction on the register in place, as a compound assignment's,
// movl -12(%rbp), %ebx; addl -16(%rbp), %ebx.  L is evaluated whole
// before R, as G13 has it.  The value is the variable's read.
value_t
operation_in(state_t &s, const operation_t &b, const type_t &t, into_t &into)
{
    const unsigned own = is_shift(b.op) ? pins(b.op, *b.rhs) : 0;

    const auto l = left_in(s, *b.lhs, into, own);
    const auto r = binary_operand(s, b.op, *b.rhs, operation_width(t));

    const int outer = std::exchange(s.in_place, l.id);
    arithmetic(s, b.op, l, r, t);
    s.in_place = outer;

    return taken(s, into, l);
}

// r = P + N, N + P or P - N as the operation on the variable's register,
// chapter 4: L into the register, then pointer_operation()'s row on it
// in place, movq -16(%rbp), %rbx; addq $4, %rbx, and p = p + 1 the addq
// alone, as p += 1 is.  N of N + P that the row multiplies is its
// product into the register, imulq $12, -24(%rbp), %rbx, by
// multiplied().  The value is the variable's read.
value_t
offset_in(state_t &s, const operation_t &b, long size, into_t &into)
{
    const bool scaled_first =
        !is< pointer_t >(b.lhs->type) && multiplies(b.op, size);

    into_t left { into.variable };
    const auto product =
        scaled_first ? multiplied(s, *b.lhs, size, 0, &left) : std::nullopt;
    const auto l = product ? *product : left_in(s, *b.lhs, into, 0);

    const int outer = std::exchange(s.in_place, l.id);
    pointer_operation(s, b, product ? 1 : size, l);
    s.in_place = outer;

    return taken(s, into, l);
}

// A binary + or - of a pointer type in the order its row generates it,
// backend.md 6.1 and 6.3: n + p with a constant n whose product with
// the size a 32-bit immediate holds is p + n, so 3 + p on an int * is
// addq $12, %rax after p's load, as p + 3 is.  The constant has no
// evaluation, so G13 has nothing to order.  A product no immediate
// holds keeps the order of the source, as a constant on the left of an
// integer operator does.
operation_t
ordered_sum(const binary_t &b, long size)
{
    if (constant_first(b) &&
        fits_imm32(scaled(as< literal_t >(*b.lhs).value, size)))
        return mirrored(b);

    return as_written(b);
}

// A binary + or - of a pointer type: the pointer and the integer, the
// integer widened to 64 bits by the parser, in the order written
// (G13).  A name's address plus a constant is the leaq of folded()'s
// one operand, leaq -20(%rbp), %rax for a + 3, 6.3; any other
// lvalue's address plus a constant the leaq of summed()'s,
// leaq 2(%rax,%rcx,8), %rax for m[i] + 2; and a local array plus a
// variable index of a size the form scales the leaq of indexed()'s,
// leaq -24(%rbp,%rcx,4), %rax for a + i.  Otherwise the operand written
// first is held outside the registers the other's top operator pins
// (chapter 6.5), and pointer_operation() writes the row, the operands
// in the order of ordered_sum(); n of n + p that the row multiplies is
// multiplied()'s product.  A register variable handed down is the
// destination of each leaq, and of the operation on its register where
// offsets_in() holds (chapter 4).
value_t
pointer_arithmetic(state_t &s, const expr_t &e, into_t *into)
{
    if (const auto a = folded(s, e))
        return address_of(s, *a, into);

    if (const auto a = summed(s, e))
        return address_of(s, *a, into);

    const auto &b = as< binary_t >(e);
    const long size = stride(e.type);

    if (is_scale(size))
        if (const auto a = indexed(s, b, size))
            return address_of(s, *a, into);

    const auto o = ordered_sum(b, size);

    if (into && offsets_in(o, e.type, *into))
        return offset_in(s, o, size, *into);

    const unsigned pinned = pinned_by(*o.rhs);

    if (!is< pointer_t >(o.lhs->type) && multiplies(o.op, size))
        if (const auto product = multiplied(s, *o.lhs, size, pinned))
            return pointer_operation(s, o, 1, *product, into);

    const auto l = rvalue_avoiding(s, *o.lhs, pinned);

    return pointer_operation(s, o, size, l, into);
}

// The object in memory x of x * c, c a constant the instruction takes
// as an immediate, or nullptr: memory_operand()'s at the operation's
// width.  c * x is x * c, the constant taken on the right by ordered()
// (backend.md 6.1).
const expr_t *
multiplicand(const operation_t &b, int width)
{
    if (b.op != binary_op_t::mul || !immediate(*b.rhs, width))
        return nullptr;

    return memory_operand(*b.lhs, width);
}

// x * c with x an object in memory: the three-operand imul with the
// object as its source, imull $4, -4(%rbp), %eax, one instruction and
// still a multiply (backend.md 6.1, 6.4).  The object is read once, by
// the imul, where its load would read it, and the constant has no
// evaluation (G13).  As load() does, the product takes the lowest free
// register once the address is consumed, or, handed a register
// variable, the variable's register, imull $4, -4(%rbp), %ebx
// (chapter 4).
value_t
product(state_t &s,
        const expr_t &object,
        const expr_t &constant,
        const int_t &t,
        into_t *into)
{
    const int width = t.width;
    const int valid = written(width, t.is_signed);
    const auto mnemonic = sized("imul", width);
    const auto factor = immediate_of(constant, width);

    const auto place = address(s, object);
    const auto from = operand(s, place);
    release(s, place);

    if (takes(into, valid)) {
        insn(s, mnemonic,
             { factor, from, reg(variable_register(into->variable), width) });
        return taken(s, *into);
    }

    const auto v = fresh(s, valid);
    insn(s, mnemonic, { factor, from, reg(reg_of(s, v), width) });

    return v;
}

value_t
binary(state_t &s, const expr_t &e, into_t *into = nullptr)
{
    const auto &b = as< binary_t >(e);

    switch (b.op) {
    case binary_op_t::log_and:
    case binary_op_t::log_or: return logical(s, e);
    // 6.5.18p2: the left operand is evaluated as a void expression,
    // the effect context of chapter 6.6
    case binary_op_t::comma: {
        effect(s, *b.lhs);
        return rvalue(s, *b.rhs, into);
    }
    default: break;
    }

    if (is_comparison(b.op))
        return comparison(s, ordered(b), into);

    if (is< pointer_t >(e.type))
        return pointer_arithmetic(s, e, into);

    if (is< pointer_t >(b.lhs->type))
        return difference(s, b);

    // a constant on the left of an operator that commutes is taken on
    // the right, 4 * x as x * 4 (6.1)
    const auto o = ordered(b, e.type, into);
    const int width = operation_width(e.type);

    if (const auto *object = multiplicand(o, width))
        return product(s, *object, *o.rhs, integer(e.type), into);

    if (into && operates_in(o, e.type, *into))
        return operation_in(s, o, e.type, *into);

    // G13: the left operand first, held while the right one is
    // generated, outside the registers the right one's operator pins and,
    // for a shift by a variable count, outside rcx, where the count goes
    // (chapter 6.5)
    const unsigned own = is_shift(o.op) ? pins(o.op, *o.rhs) : 0;
    const auto l = rvalue_avoiding(s, *o.lhs, own | pinned_by(*o.rhs));
    const auto r = binary_operand(s, o.op, *o.rhs, width);

    return arithmetic(s, o.op, l, r, e.type);
}

// ++ and -- of a register variable: add or sub of 1, or of the size for
// a pointer, on its register in place, then the re-extension of a narrow
// one, movsbl %bl, %ebx, which is the store row.  The postfix forms
// read the variable first, and preserve() copies the read into a fresh
// register before the write, movl %ebx, %eax: that copy is the value.
// The prefix forms yield a read of the variable after the write, as a
// compound assignment does.  The add or sub sets ZF from the register,
// and a re-extension after it withdraws that (7.1).
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

    const reg_t r = variable_register(sym);
    insn(s, sized(up ? "add" : "sub", width), { imm(amount), reg(r, width) });
    flagged(s, width);
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
        return step_register(s, u.op, sym);

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

// - and ~ on the held value v, of type t: neg or not on its register,
// an instruction of one address.  G1: neg of INT_MIN is INT_MIN.  A
// borrowed read is copied into a fresh register first, movl %ebx, %eax,
// by writable(), but the variable written in place.  neg sets ZF from
// its result and not sets no flag (7.1).
value_t
negated(state_t &s, unary_op_t op, value_t v, const type_t &t)
{
    const auto &type = integer(t);
    const int width = operation_width(t);
    const bool negates = op == unary_op_t::neg;

    insn(s, sized(negates ? "neg" : "not", width),
         { reg(writable(s, v, written(width, type.is_signed)), width) });
    if (negates)
        flagged(s, width);

    return v;
}

// Whether r = -x or r = ~x, of type t, is computed on the variable's
// register, chapter 4: on the type the variable's value is computed
// in, computed_type(), and x that loads_into() the register.  The
// instruction has one address and no right operand to put a condition
// on.
bool
negates_in(const unary_t &u, const type_t &t, const into_t &into)
{
    return t == computed_type(into) && loads_into(*u.arg, into);
}

// r = -x and r = ~x on the variable's register, chapter 4: x into the
// register as the operation form's L goes, then the neg or the not in
// place, movl -12(%rbp), %ebx; negl %ebx, and r = -r the negl alone.
// The value is the variable's read.
value_t
negated_in(state_t &s, const unary_t &u, const type_t &t, into_t &into)
{
    const auto l = left_in(s, *u.arg, into, 0);

    const int outer = std::exchange(s.in_place, l.id);
    negated(s, u.op, l, t);
    s.in_place = outer;

    return taken(s, into, l);
}

value_t
unary(state_t &s, const unary_t &u, const type_t &t, into_t *into = nullptr)
{
    switch (u.op) {
    case unary_op_t::neg:
    case unary_op_t::bit_not: {
        if (into && negates_in(u, t, *into))
            return negated_in(s, u, t, *into);

        return negated(s, u.op, rvalue(s, *u.arg), t);
    }

    // as a value; the operand is not promoted, so it is compared at its
    // own width.  A register variable is read by the cmp, and the sete
    // takes a fresh register, as for a comparison; the value is valid
    // to 64 bits, truth_valid.
    case unary_op_t::log_not: {
        const int width = integer(u.arg->type).width;
        auto v = rvalue(s, *u.arg);
        insn(s, sized("cmp", width), { imm(0), reg(reg_of(s, v), width) });

        if (borrowed(s, v)) {
            release(s, v);
            v = fresh(s, 32);
        }

        const reg_t r = reg_of(s, v);
        insn(s, "sete", { reg(r, 8) });
        insn(s, "movzbl", { reg(r, 8), reg(r, 32) });
        s.values[v.id].valid = truth_valid;
        return v;
    }

    case unary_op_t::pre_inc:
    case unary_op_t::pre_dec:
    case unary_op_t::post_inc:
    case unary_op_t::post_dec: return step(s, u);

    case unary_op_t::addr: return address_value(s, *u.arg, into);

    case unary_op_t::deref: {
        const auto &type = integer(t);
        return load(s, pointed(s, *u.arg, t), type, std::max(32, type.width),
                    into);
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
    if (!borrowed(s, v)) {
        establish(s, sym, reg_of(s, v));
        return;
    }

    const auto &t = integer(sym->type);
    if (t.width < 32 || width > register_width(sym))
        establish(s, sym, variable_register(sym));
}

// The value of e written into a register variable, chapter 4: by the
// row at the top of e, handed the variable, where the row can write
// its register, the load, movl -12(%rbp), %ebx, the address, leaq
// buf(%rip), %rbx, the operation form, movl -12(%rbp), %ebx; addl
// -16(%rbp), %ebx, - and ~ on the register, negl %ebx, and a
// comparison's setl %bl; movzbl %bl, %ebx, and the value is the
// variable's read; otherwise into L, then the mov of establish(),
// movl %eax, %ebx, or movl %r12d, %ebx from another register variable,
// and the value stays L, as for a slot.
value_t
computed_into(state_t &s, const symbol_t *sym, const expr_t &e)
{
    into_t into { sym };
    const auto v = rvalue(s, e, &into);
    if (!into.taken)
        establish(s, sym, reg_of(s, v));

    return v;
}

// Assignment to a register variable, after preserve() has copied every
// read of it an enclosing expression still holds, x + (x += e), before
// the first write of the variable, which in the operation form is the
// load of L.  Plain: a constant is the immediate into the variable's
// register and the value is a read of the variable; otherwise the right
// operand into the register by computed_into().  Compound: the right
// operand first, then the operation row with the variable's register as
// L, in place, addl %eax, %ebx, but for a division, whose quotient comes
// back from rax by the mov; p += n at a size of 2, 4 or 8, whose leaq
// is handed the variable and writes its register, leaq (%rbx,%rax,4),
// %rbx (chapter 4); the value is a read of the variable after the
// write.  The variable is read by the operation, after the right
// operand, as a slot's object is (G13), so no read of it is held across
// the right operand: r += (r = 2) is movl $2, %ebx and addl %ebx, %ebx.
// The read is extended into a fresh register first when the operation is
// wider than the register is valid to, movslq %ebx, %rax for int +=
// long.
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

    const auto r = stepped ? displacement(s, *a.op, *a.rhs, stride(sym->type)) :
                             right_operand(s, *a.op, *a.rhs, width);
    const auto l = extended(s, read(s, sym), integer(sym->type), width);

    into_t into { sym };
    const int outer = std::exchange(s.in_place, l.id);
    const auto v = stepped ? displaced(s, *a.op, l, r, &into) :
                             arithmetic(s, *a.op, l, r, a.common);
    s.in_place = outer;

    settle(s, sym, v, width);
    release(s, v);

    return read(s, sym);
}

// Plain or compound assignment; its value is the value stored, the low
// bytes of L.  The lvalue's address comes first (G13) and is held
// across the right operand.  A compound one then generates the right
// operand, and its operation follows as one piece, nothing evaluated
// between: the load of the object through the held address, extended
// into the common type the parser chose, the operation, and the store
// of the result's low bytes back through the same address (G2).  So the
// object is read after the right operand, as the memory form of chapter
// 6.6 reads it.  The place is generated outside the registers
// pinned_by() names, and the right operand with rax reserved, so that
// the object's load takes rax and the result is in the register a
// return or an enclosing expression takes first.  6.5.17.3p4: "with
// respect to an indeterminately sequenced function call, the operation
// of a compound assignment is a single evaluation."  p += n and p -= n
// step the pointer by n times the size.
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

    const auto place = address_avoiding(s, *a.lhs, pinned_by(a));

    if (!a.op) {
        const auto v = rvalue(s, *a.rhs);
        store(s, v, place, t.width);
        release(s, place);
        return v;
    }

    const int width = operation_width(a.common);
    const auto r = stepped ? displacement(s, *a.op, *a.rhs, stride(a.lhs->type),
                                          bit(rax)) :
                             right_operand(s, *a.op, *a.rhs, width, bit(rax));

    // the value of the expression has the lvalue's type: the bits
    // above its width are the operation's, not the value's extension
    const auto l = fetch(s, place, t, width);
    const auto v = stepped ? displaced(s, *a.op, l, r) :
                             arithmetic(s, *a.op, l, r, a.common);
    store(s, v, place, t.width);
    release(s, place);
    s.values[v.id].valid = std::min(s.values[v.id].valid, t.width);

    return v;
}

// The load of a signed object in memory extended to a cast's width,
// backend.md 6.3: movslq -4(%rbp), %rax for (long) i on an int, movsbq
// for a signed char.  It reads the bytes the load at 32 bits reads, at
// the object's width (G10), and the cast then finds the value valid to
// its width.  The rule is the Seventh Edition compiler's: match(), in
// c10.c, tries a row again with the conversion dropped, and its tables
// convert an operand in memory in the instruction that reads it.
value_t
extending_load(state_t &s,
               const expr_t &e,
               const int_t &from,
               int to,
               into_t *into = nullptr)
{
    return load(s, address(s, e), from, std::max(32, to), into);
}

// An integer conversion.  G2: narrowing is no instruction, the narrower
// register name is the value; widening extends by the operand's
// signedness, and only when the register is not already valid to the
// new width, as it is after the load of a char or a 32-bit write of an
// unsigned value.  A signed object in memory is loaded extended to the
// new width, extending_load(), so the load is the widening; an unsigned
// one is loaded as any read, valid to 64 bits by the load's own
// extension.  The object may be under casts that keep its bits,
// object_under()'s: (long) (int) u is movslq -4(%rbp), %rax, and
// (unsigned long) (unsigned) i the movl, valid to 64 bits, each
// reading the object at its own width (G10).  A change of signedness
// alone is nothing.  A pointer
// is its integer view, so pointer to pointer is nothing, an integer
// widens to a pointer by its own signedness (6.3.2.3p5), and a pointer
// narrows to an integer by its low bits (p6).  An array decays to its
// address, 6.3.2.1p3, and a function designator to the pointer to the
// function, p4.  A cast to void evaluates its operand for its
// effect alone, chapter 6.6.  A register variable widens into a fresh
// register, movslq %ebx, %rax, and narrows as its read: the read, not
// the variable, is valid to less.  A register variable handed down goes
// on to the operand's row through() a conversion that leaves its
// representation, and a load takes it: register int x = c is
// movsbl -1(%rbp), %ebx (chapter 4).  The decay and the function's
// address take it as & does, leaq buf(%rip), %rbx.
// The operand of a conversion that retypes(), into the variable's
// register, backend.md chapter 4.  An object in memory, under casts
// that keep its bits, is loaded at the extension of the conversion's
// type, movl -20(%rbp), %ebx for register unsigned u = i and
// movsbl -9(%rbp), %ebx for register char c = uc, the object read at
// its own width (G10).  Any other operand of 32 bits or more is its own
// row, handed the variable as computed in the operand's type: the row's
// last instruction writes the register at the variable's width, and a
// 32-bit write clears the upper half, so u = a + b on two ints is
// movl -12(%rbp), %ebx; addl -16(%rbp), %ebx.  A narrower operand is
// no row of its own, and is as any conversion's.
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

value_t
cast(state_t &s, const cast_t &c, const type_t &to, into_t *into = nullptr)
{
    if (is< void_t >(to)) {
        effect(s, *c.arg);
        return { };
    }

    if (is< array_t >(c.arg->type))
        return address_value(s, *c.arg, into);

    if (is< function_t >(c.arg->type))
        return function_pointer(s, *c.arg, into);

    if (retypes(c, to, into))
        return retyped(s, c, integer(to), *into);

    const auto &from = integer(c.arg->type);
    const auto &target = integer(to);
    const bool widens = target.width > from.width;
    const auto onward = through(into, target);

    const auto *object = widens ? object_under(c) : nullptr;
    const bool loads = object && (from.is_signed || object != c.arg.get());
    const int width = from.is_signed ? target.width : 32;

    auto v = loads ? extending_load(s, *object, from, width, onward) :
                     rvalue(s, *c.arg, onward);
    if (widens)
        v = extended(s, v, from, target.width);

    auto &valid = s.values[v.id].valid;
    valid = std::min(valid, target.width);

    return v;
}

// The callee of a call through a pointer, chapter 8: a hidden local of
// its call, as an argument is, an 8-byte slot taken before the callee
// is evaluated and written by the store row of 6.6.  Returns the slot's
// displacement.
int
callee_slot(state_t &s, const expr_t &callee)
{
    const int displacement = allocate(s, 8, 8);
    initialize(s, callee, at(displacement), 64);

    return displacement;
}

// Chapter 8, every argument a hidden local of its call.  Every held
// value is spilled; the callee of a call through a pointer is evaluated
// first, as the source writes it (G13), and stored to its slot; each
// argument, in order and complete before the next, is stored to its own
// slot at its width; what the arguments left held is spilled in its
// turn; then the stack arguments are pushed from their slots, last
// first, over a pad that keeps rsp a multiple of 16 at the call, and
// the first six are loaded into their registers, extended to 32 bits if
// narrower.  G16: the call is made with the call site's types and the
// convention's registers, call f@PLT for a named function and call
// *d(%rbp) through the callee's slot, the machine's call through
// memory, with no register.  The result is in rax, the lowest free
// register, valid to its type's width and no further, 6.3: the
// convention leaves the bits above a narrow one unspecified, and the
// width rule extends it where it is used wider.
value_t
call(state_t &s, const call_t &c, const type_t &t)
{
    const auto *named = named_callee(c);
    const auto &f = callee_type(c);

    spill_all(s);

    const int target = named ? 0 : callee_slot(s, *c.callee);

    std::vector< std::pair< int, int_t > > slots;
    for (std::size_t i = 0; i < c.args.size(); ++i) {
        const auto &arg = c.args[i];
        const auto &type = integer(arg.type);

        if (i < f.params.size() && !(arg.type == *f.params[i]))
            throw std::logic_error("c1: an argument unlike its parameter");

        const int displacement =
            allocate(s, size_of(arg.type), align_of(arg.type));
        initialize(s, arg, at(displacement), type.width);

        slots.emplace_back(displacement, type);
    }

    // the call destroys every expression register, and an argument's
    // write may have copied a borrowed read into one (6.7)
    spill_all(s);

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

    // every named call through the PLT: the executable is
    // position-independent
    if (named)
        insn(s, "call", { std::format("{}@PLT", assembly_name(*named)) });
    else
        insn(s, "call", { "*" + at(target) });

    const long popped = 8 * static_cast< long >(stacked) + (pad ? 8 : 0);
    if (popped > 0)
        insn(s, "addq", { imm(popped), "%rsp" });

    s.calls = true;

    if (is< void_t >(t))
        return { };

    return hold(s, rax, integer(t).width);
}

// Whether the instruction written for e has a pin on the register of
// its result: a call, whose result is in rax (chapter 8), or a
// division, chapter 6.5's pin on rax and rdx: / and %, /= and %=, and
// p - q for a size other than 1.  A shift's pin is on its count.
bool
pinned(const expr_t &e)
{
    return is< call_t >(e) || (pins(e) & bit(rax));
}

// The value of an expression in a register, its code written first.
// An array is never a value: the parser decays it, 6.3.2.1p3, and a
// string literal is an array.  A call or a division keeps its own pin:
// its operands are generated outside an outer reservation, since the
// pin, not the outer wish, decides where its result is written.  A
// register variable handed down, into, goes to the row at the top of
// e, which writes the variable's register where it can (chapter 4); a
// read of a register variable is no row, and the assignment moves it
// in, movl %r12d, %ebx.
value_t
rvalue(state_t &s, const expr_t &e, into_t *into)
{
    if (is< array_t >(e.type))
        throw std::logic_error("c1: an array as a value");

    if (s.reserved && pinned(e)) {
        const unsigned reserved = std::exchange(s.reserved, 0u);
        const auto v = rvalue(s, e, into);
        s.reserved = reserved;

        return v;
    }

    if (is< literal_t >(e))
        return literal(s, as< literal_t >(e), e.type, into);

    if (is< name_t >(e)) {
        if (const auto *sym = register_variable(e))
            return read(s, sym);

        const auto &t = integer(e.type);
        return load(s, address(s, e), t, std::max(32, t.width), into);
    }

    if (is< unary_t >(e))
        return unary(s, as< unary_t >(e), e.type, into);

    if (is< binary_t >(e))
        return binary(s, e, into);

    if (is< assign_t >(e))
        return assign(s, as< assign_t >(e));

    if (is< call_t >(e))
        return call(s, as< call_t >(e), e.type);

    if (is< cast_t >(e))
        return cast(s, as< cast_t >(e), e.type, into);

    return conditional(s, e);
}

// Whether the instruction of op sets ZF from the result it computes:
// add, sub, and, or and xor, by Intel's reference (7.1).
bool
flag_operator(binary_op_t op)
{
    switch (op) {
    case binary_op_t::add:
    case binary_op_t::sub:
    case binary_op_t::bit_and:
    case binary_op_t::bit_or:
    case binary_op_t::bit_xor: return true;
    default: return false;
    }
}

// Whether a condition's top operator is one whose own instruction sets
// ZF from the condition's value, the rows of 7.1 after the Seventh
// Edition compiler's cctab: +, -, |, ^ and unary - on integers, & as
// test, and +=, -=, &=, |=, ^= and a prefix ++ or -- by the form
// effect() writes.  A pointer keeps the row of any value whatever its
// size, since p + n, p - n and p - q are lea, imul or a division at
// every size but 1; so do *, /, %, the shifts, ~, a cast, *=, /=, %=,
// <<=, >>= and a postfix step, whose value is the old one.
bool
flag_condition(const expr_t &e)
{
    if (is< pointer_t >(e.type))
        return false;

    if (is< unary_t >(e)) {
        const auto op = as< unary_t >(e).op;
        return op == unary_op_t::neg || op == unary_op_t::pre_inc ||
               op == unary_op_t::pre_dec;
    }

    if (is< binary_t >(e)) {
        const auto &b = as< binary_t >(e);
        return !is< pointer_t >(b.lhs->type) && flag_operator(b.op);
    }

    if (is< assign_t >(e)) {
        const auto &op = as< assign_t >(e).op;
        return op && flag_operator(*op);
    }

    return false;
}

// The operand of e == 0 or e != 0 that is a flag condition, whose row
// then serves the comparison, the jump's sense by the operator; nullptr
// for any other comparison, e < 0 and the rest among them, which keep
// the cmp, since the jump of 7.1 reads ZF alone.  0 == e is e == 0 by
// ordered().  The Seventh Edition compiler's rcexpr() drops the 0 of
// any relational under cctab.
const expr_t *
zero_compared(const operation_t &b)
{
    if (b.op != binary_op_t::eq && b.op != binary_op_t::ne)
        return nullptr;

    if (!is< literal_t >(*b.rhs) || as< literal_t >(*b.rhs).value != 0)
        return nullptr;

    return flag_condition(*b.lhs) ? b.lhs : nullptr;
}

// & as a condition, 7.1: test R, L, which sets the flags and writes no
// result, so a register variable as L is read in its register and not
// copied, testl %eax, %ebx, and a constant R is the immediate,
// testl $1, %eax.  The operands are generated as the row of & generates
// them, in the order of ordered(), so 1 & c is c & 1; an object in
// memory as R the instruction's operand, testl -8(%rbp), %eax
// (binary_operand()), the left operand outside the registers the right
// one's operator pins (chapter 6.5), and all are consumed.  An object in
// memory and a constant are test $c, obj, condition_on()'s.  The
// Seventh Edition compiler's cctab has the PDP-11's bit for & (cc81).
void
test(state_t &s, const operation_t &b, int width)
{
    const auto mnemonic = sized("test", width);

    const auto *object = memory_operand(*b.lhs, width);
    if (object && immediate(*b.rhs, width)) {
        condition_on(s, "test", *object, immediate_of(*b.rhs, width), width);
        flagged(s, width);
        return;
    }

    const auto l = rvalue_avoiding(s, *b.lhs, pinned_by(*b.rhs));
    const auto r = binary_operand(s, b.op, *b.rhs, width);

    if (!r.immediate.empty()) {
        insn(s, mnemonic, { r.immediate, reg(reg_of(s, l), width) });
    }
    else if (r.place) {
        const reg_t left = reg_of(s, l, registers_of(s, *r.place));
        insn(s, mnemonic,
             { operand(s, *r.place, bit(left)), reg(left, width) });
    }
    else {
        const auto [left, right] = operands(s, l, r.value);
        insn(s, mnemonic, { reg(right, width), reg(left, width) });
    }
    flagged(s, width);

    release(s, r);
    release(s, l);
}

// A compound assignment or a prefix step as a condition, 7.1: the form
// effect() writes for it as a statement.  On an object in memory that
// is the memory form of 6.6, addl $1, -4(%rbp), whose instruction sets
// ZF from the object's new value at its width, and nothing is held.  On
// a register variable it is the operation in place, chapter 4, and the
// variable's read is returned for the cmp when the flags are not the
// condition's: a narrow variable is re-extended after its operation,
// and the result of a wider operation is moved back.
value_t
modified(state_t &s, const expr_t &e, int width)
{
    effect(s, e);
    if (s.zero_flag == width)
        return { };

    const auto &object =
        is< assign_t >(e) ? *as< assign_t >(e).lhs : *as< unary_t >(e).arg;
    const auto *sym = register_variable(object);
    if (!sym)
        throw std::logic_error("c1: a memory form as a condition set no flag");

    return read(s, sym);
}

// A flag condition, 7.1: its row, then je or jne on the flags of its
// operator's instruction, with no cmp.  The jump reads the flags of the
// last instruction written, so it is taken bare only where the record
// says that instruction set ZF at the condition's width; otherwise the
// value is compared with 0, as any other condition's.
void
flag_branch(state_t &s, const expr_t &e, int label, bool sense)
{
    const int width = integer(e.type).width;

    const bool steps = is< unary_t >(e) && is_step(as< unary_t >(e).op);

    value_t v;
    if (is< binary_t >(e) && as< binary_t >(e).op == binary_op_t::bit_and)
        test(s, ordered(as< binary_t >(e)), width);
    else if (is< assign_t >(e) || steps)
        v = modified(s, e, width);
    else
        v = rvalue(s, e);

    if (s.zero_flag != width)
        insn(s, sized("cmp", width), { imm(0), reg(reg_of(s, v), width) });
    release(s, v);

    jump_if(s, sense ? binary_op_t::ne : binary_op_t::eq, true, label);
}

// A comparison as a condition, 7.1, its operands in the order of
// ordered(): == 0 or != 0 of a flag condition jumps on its operator's
// own flags; any other is compared()'s cmp, of an object in memory in
// place against an immediate, cmpl $5, -4(%rbp), or cmp R, L.  Then jcc
// of the operator, or of its inverse by sense.
void
comparison_branch(state_t &s, const operation_t &b, int label, bool sense)
{
    if (const auto *tested = zero_compared(b)) {
        const bool equal = b.op == binary_op_t::eq;
        flag_branch(s, *tested, label, sense != equal);
        return;
    }

    const bool is_signed = integer(b.lhs->type).is_signed;

    release(s, compared(s, b));

    jump_if(s, sense ? b.op : inverse(b.op), is_signed, label);
}

// Jump code, chapter 7.1: a jump to label when e is true (sense) or
// false (!sense), a fall through otherwise, and no value.  A comparison
// is cmp and jcc, a constant on its left mirrored to the right by
// ordered(); ! flips the sense; && and || are the shape of the
// short circuit, 6.5.14p4, 6.5.15p4, the right operand not reached when
// the left decides.  A flag condition, and one compared with 0 by == or
// !=, jumps on its operator's own flags.  Any other scalar, a constant
// or a pointer included, is its value compared with 0 at its own width.
// An object in memory is compared in place, by condition_on(): alone,
// cmpl $0, -4(%rbp), and as a comparison's left operand when the right
// one is an immediate, cmpl $5, -4(%rbp); the value is not wanted, so
// no register holds it.
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
            comparison_branch(s, ordered(b), label, sense);
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

    if (flag_condition(e)) {
        flag_branch(s, e, label, sense);
        return;
    }

    const int width = integer(e.type).width;
    if (const auto *object = memory_operand(e, width)) {
        condition_on(s, "cmp", *object, imm(0), width);
    }
    else {
        const auto v = rvalue(s, e);
        insn(s, sized("cmp", width), { imm(0), reg(reg_of(s, v), width) });
        release(s, v);
    }

    insn(s, sense ? "jne" : "je", { label_name(s.function, label) });
}

// A plain assignment of a constant to an object in memory, for its
// effect alone: the lvalue's address, then the store row of an
// initializer through it, movb $122, (%rax).  G13 holds, since the
// address is evaluated first and the constant has no evaluation.  False
// for anything else, a register variable and a 64-bit constant no
// 32-bit immediate holds included: the operand is formed before the
// right operand would be evaluated, which could spill the address.
bool
constant_assignment(state_t &s, const assign_t &a)
{
    if (a.op || register_variable(*a.lhs))
        return false;

    const int width = integer(a.lhs->type).width;
    if (!immediate(*a.rhs, width))
        return false;

    const auto place = address(s, *a.lhs);
    initialize(s, *a.rhs, operand(s, place), width);
    release(s, place);

    return true;
}

// The memory forms, chapter 6.6: the operator on the object at the
// object's width, op R, obj or op $c, obj.  The address is held and
// the right operand evaluated before the operand is formed, since the
// right operand may spill the address's base or index (G13).

// +=, -=, &=, |= and ^=: addl $5, -8(%rbp), andb %cl, (%rax).  G1, G2:
// the low bits of the operation in int are the low bits of the
// operation at the object's width.  The instruction sets ZF from the
// object's new value at that width (7.1).
void
arithmetic_on(state_t &s,
              binary_op_t op,
              const address_t &place,
              const expr_t &rhs,
              int width)
{
    const auto mnemonic = sized(arithmetic_mnemonic(op), width);

    const auto r = right_operand(s, op, rhs, width);
    if (!r.immediate.empty()) {
        insn(s, mnemonic, { r.immediate, operand(s, place) });
        flagged(s, width);
        return;
    }

    const reg_t right = reg_of(s, r.value, registers_of(s, place));
    insn(s, mnemonic, { reg(right, width), operand(s, place, bit(right)) });
    flagged(s, width);
    release(s, r.value);
}

// <<= and >>=: shlb $3, (%rax), the count the immediate or in cl, as
// for shift(); sar for a signed object and shr for an unsigned one.
// G3: the instruction masks the count to 5 bits at 8, 16 and 32 bits,
// as the promoted operation does, and the low bits of its result are
// the narrow shift's.  modify() generated the place with rcx reserved
// and right_operand() the count where cl is likely, and the moves of
// chapter 6.5 remain for a value that is already there.
void
shift_on(state_t &s,
         binary_op_t op,
         const address_t &place,
         const expr_t &rhs,
         const int_t &t)
{
    const auto mnemonic = sized(shift_mnemonic(op, t.is_signed), t.width);

    const auto r = right_operand(s, op, rhs, t.width);
    if (!r.immediate.empty()) {
        insn(s, mnemonic, { r.immediate, operand(s, place) });
        return;
    }

    const unsigned held = registers_of(s, place);
    const reg_t right = reg_of(s, r.value, held);

    count_into_cl(s, r, right, held);

    insn(s, mnemonic, { "%cl", operand(s, place, bit(rcx)) });
    release(s, r.value);
}

// p += n and p -= n: addq $8, -8(%rbp) for a constant n the immediate
// holds times the size; otherwise n in a register, multiplied by the
// size at any size but 1, an object in memory of 64 bits by
// multiplied()'s imulq $4, -16(%rbp), %rax, then addq R, obj or
// subq R, obj.  G6.
void
displaced_on(state_t &s,
             binary_op_t op,
             const address_t &place,
             const expr_t &n,
             long size)
{
    const auto mnemonic = op == binary_op_t::add ? "addq" : "subq";
    const unsigned held = registers_of(s, place);

    value_t r;
    if (is< literal_t >(n)) {
        const auto d = scaled(as< literal_t >(n).value, size);
        if (fits_imm32(d)) {
            insn(s, mnemonic, { imm(d), operand(s, place) });
            return;
        }

        r = fresh(s, 64, held);
        constant(s, reg_of(s, r), d, 64);
    }
    else if (const auto product =
                 size != 1 ? multiplied(s, n, size, held) : std::nullopt) {
        r = *product;
    }
    else {
        r = rvalue(s, n);
        if (size != 1) {
            const reg_t index = writable(s, r, 64, held);
            insn(s, "imulq", { imm(size), reg(index, 64), reg(index, 64) });
        }
    }

    const reg_t right = reg_of(s, r, held);
    insn(s, mnemonic, { reg(right, 64), operand(s, place, bit(right)) });
    release(s, r);
}

// A compound assignment for its effect, on an object in memory: the
// address, outside the registers pinned_by() names, then the one
// instruction on the object.  False for a register variable, whose
// operation is in place already, and for *=, /= and %=, which keep the
// load, the operation and the store: imul has no memory destination,
// and idiv none at all.
bool
modify(state_t &s, const assign_t &a)
{
    if (!a.op || register_variable(*a.lhs))
        return false;

    const auto op = *a.op;
    if (op == binary_op_t::mul || op == binary_op_t::div ||
        op == binary_op_t::rem)
        return false;

    const auto &t = integer(a.lhs->type);
    const auto place = address_avoiding(s, *a.lhs, pinned_by(a));

    if (op == binary_op_t::shl || op == binary_op_t::shr)
        shift_on(s, op, place, *a.rhs, t);
    else if (is< pointer_t >(a.lhs->type))
        displaced_on(s, op, place, *a.rhs, stride(a.lhs->type));
    else
        arithmetic_on(s, op, place, *a.rhs, t.width);

    release(s, place);

    return true;
}

// ++ and -- for their effect: add $1 or sub $1 on the object at its
// width, addq $8 for a pointer to 8 bytes, and no old value copied,
// since nothing reads it; the instruction sets ZF from the object's new
// value at that width (7.1).  A register variable's step is its prefix
// form, which copies no read.
void
modify(state_t &s, const unary_t &u)
{
    const bool up = u.op == unary_op_t::pre_inc || u.op == unary_op_t::post_inc;

    if (const auto *sym = register_variable(*u.arg)) {
        const auto prefix = up ? unary_op_t::pre_inc : unary_op_t::pre_dec;
        release(s, step_register(s, prefix, sym));
        return;
    }

    const auto &t = integer(u.arg->type);
    const long amount = is< pointer_t >(u.arg->type) ? stride(u.arg->type) : 1;

    const auto place = address(s, *u.arg);
    insn(s, sized(up ? "add" : "sub", t.width),
         { imm(amount), operand(s, place) });
    flagged(s, t.width);
    release(s, place);
}

// The effect context, chapter 6.6, Ritchie's efftab beside regtab and
// cctab: an expression evaluated for its side effects, its value not
// wanted.  A constant assigned to memory is stored directly, and a
// compound assignment or a step on an object in memory is the memory
// form.  The context reaches through the operators that discard a
// value: a conditional's arms, both operands of a comma, whose left one
// is in this context wherever the comma stands (6.5.18p2), and, by
// cast(), the operand of a cast to void; anything else is its value,
// discarded.
void
effect(state_t &s, const expr_t &e)
{
    if (is< assign_t >(e)) {
        const auto &a = as< assign_t >(e);
        if (constant_assignment(s, a) || modify(s, a))
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

// A case's test in a switch's dispatch, backend.md 7.3: the comparison
// row of the held value and the case's constant, cmpl $5, %eax, then je
// to its label.  The Seventh Edition compiler tested a 0 where it
// compared another value; this machine's test of zero is the same row,
// cmpl $0, %eax (7.1).
void
equal_jump(state_t &s, value_t v, const expr_t &constant, int label)
{
    compare_with(s, v, constant, operation_width(constant.type));
    jump_if(s, binary_op_t::eq, true, label);
}

// The jump through a switch's table by a held index of width bits,
// backend.md 7.3: the index valid to 64 bits as an unsigned value, by
// the width rule, which writes movl L, L where no 32-bit write of an
// unsigned value left it so; the table's address PC-relative, leaq
// .Lf.8(%rip), R; and the jump through its entry, jmp *(R,L,8).
void
table_jump(state_t &s, value_t index, int width, int table)
{
    const auto i = extended(s, index, int_t { width, false }, 64);
    const auto base = address_of(s, { .label = label_name(s.function, table) });
    const address_t entry { .base = base, .index = i, .scale = 8 };

    insn(s, "jmp", { "*" + operand(s, entry) });
    release(s, entry);

    s.terminated = true;
}

// The division of a hash switch, backend.md 7.3: the unsigned division
// row by the table's size, held in a register as the divisor of x / 7
// is, kept out of rax and rdx, and both results kept, the quotient in
// rax and the remainder in rdx, each valid to 64 as an unsigned value
// the instruction wrote.  v and the divisor are consumed.
std::pair< value_t, value_t >
divided(state_t &s, value_t v, const expr_t &tabs, int width)
{
    const auto d = rvalue_avoiding(s, tabs, bit(rax) | bit(rdx));
    pin_division(s, binary_op_t::div, v, d, int_t { width, false });

    release(s, d);
    release(s, v);

    const int valid = written(width, false);

    return { hold(s, rax, valid), hold(s, rdx, valid) };
}

// statements

// A register variable is in the one of the five its index names.  A
// constant initializer is the constant row into the register, mov $c,
// %r12d, and any other is computed into the register, as an
// assignment's right operand is, by computed_into().
// G5: without one, nothing writes the value.  The register holds what
// the declaration finds in it: what the prologue left, where no variable
// of the function held the register before, and otherwise what the last
// variable to hold it left.  Every read before the first write reads
// that.  Where the representation claims more bits than the type has,
// for a char, a short or an unsigned, it is established on those
// contents by the mov from the register to itself, movsbl %bl, %ebx: the
// value of the type in the low bits is unchanged, and the register is
// the extension every read assumes.
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
// row of chapter 6.6, into the slot.  An array's slot is at the
// type's alignment, chapter 5, and its displacement is the array's
// lowest address, the elements upward from it.
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

    initialize(s, *d.init, at(displacement), integer(sym->type).width);
}

#include "c1-statements.hh"

// functions

// The parameters' half of the prologue, before the body, since the
// body's slots come after theirs.  Each of the first six takes the next
// slot and is stored there from its argument register with the mov of
// its width; the seventh and later are where the caller pushed them,
// 16(%rbp) and up, 8 bytes apart, and are read from there.  A register
// parameter is in the one of the five its index names, the next in
// parameter order, and is moved into it from its argument register or
// its stack slot by establish(), which extends a narrow one: the ABI
// leaves the upper bits of a char or a short argument undefined, and
// the argument register is caller-saved, so it would not survive the
// first call.  The moves and the stores are written in parameter order,
// after the pushes.
void
parameters(state_t &s, const function_decl_t &fd)
{
    const std::size_t registers = std::size(argument_registers);

    for (std::size_t i = 0; i < fd.params.size(); ++i) {
        const auto *p = fd.params[i];
        const auto &t = integer(p->type);
        const int above =
            16 + 8 * (static_cast< int >(i) - static_cast< int >(registers));

        if (in_register(*p)) {
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

// Chapter 5: rbp is the frame pointer in every function; the registers
// of the five the function uses are pushed below it, in order, each
// with its place in the unwind table, -16 - 8k off the canonical frame
// address for the k-th.  The frame is the slots below the pushes, sized
// so that rsp is a multiple of 16 at every call: after pushq %rbp it
// is, so the pushes and the subq together are.  A function that calls
// nothing keeps up to 128 bytes of slots in the red zone, measured from
// rsp after the pushes, and moves no rsp.
void
prologue(state_t &s, long size, bool sub)
{
    s.out << "\tpushq\t%rbp\n"
             "\t.cfi_def_cfa_offset 16\n"
             "\t.cfi_offset %rbp, -16\n"
             "\tmovq\t%rsp, %rbp\n"
             "\t.cfi_def_cfa_register %rbp\n";

    for (int k = 0; k < s.saved; ++k) {
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
    s.out << label_name(s.function, s.ret) << ":\n";

    if (sub && s.saved > 0)
        s.out << std::format("\tleaq\t{}, %rsp\n", at(-8 * s.saved));

    for (int k = s.saved; k-- > 0;)
        s.out << std::format("\tpopq\t{}\n", reg(variable_registers[k], 64));

    s.out << "\tleave\n"
             "\t.cfi_def_cfa %rsp, 8\n"
             "\tret\n";
}

// A definition.  The registers the prologue pushes are the parser's
// count, the registers the function's variables take, backend.md
// chapter 5, known before the body, since every slot lies below them;
// the body is generated next, into the buffer, and the prologue written
// after it, when the frame's size is known.  The one epilogue is reached with
// rax as it is, G5's some value, but for main, which returns 0 when it
// falls off its end, 5.1.2.3.4.
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
    s.saved = fd.registers;
    s.in_place = -1;
    s.reserved = 0;
    s.frame = 8 * s.saved;
    s.function = fd.sym->name;
    s.labels = 0;
    s.ret = new_label(s);
    s.terminated = false;
    s.jump_targets.clear();
    s.user_labels.clear();
    s.case_labels.clear();

    parameters(s, fd);
    block_stmt(s, *fd.body);

    if (!s.terminated && s.function == "main")
        insn(s, "movl", { imm(0), "%eax" });

    const int pushed = 8 * s.saved;
    const long size = (s.frame + 15) / 16 * 16 - pushed;
    const bool sub = size > 0 && (s.calls || s.frame - pushed > 128);

    const auto name = assembly_name(*fd.sym);
    binding(s.out, *fd.sym);
    s.out << std::format("\t.type\t{}, @function\n"
                         "{}:\n"
                         "\t.cfi_startproc\n",
                         name, name);

    prologue(s, size, sub);

    s.out << s.body.str();

    epilogue(s, sub);
    s.out << std::format("\t.cfi_endproc\n"
                         "\t.size\t{}, .-{}\n",
                         name, name);
}

} // namespace

// The functions the unit defines, in order; one it only declares
// writes nothing, since the assembler leaves its symbol to the link.
// The string literals follow the last function, and the objects the
// unit defines follow them.  The note section marks the stack not
// executable.
void
generate_x86_64(const unit_t &unit,
                const target_t &target,
                std::string_view file,
                std::ostream &out)
{
    state_t s { .out = out,
                .frame_limit = target.frame_limit,
                .defined = defined_symbols(unit) };

    out << std::format("\t.file\t{}\n\t.text\n", quoted_file(file));

    for (const auto &fd : unit.functions)
        if (fd.body)
            function(s, fd);

    data(s.out, s.strings, unit.objects, target);

    out << "\t.section\t.note.GNU-stack,\"\",@progbits\n";
}

} // namespace rcc
