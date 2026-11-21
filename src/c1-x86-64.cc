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

    // the values held, live's count, by the expression around the
    // statement expression whose statements are generated, which a
    // statement leaves as it finds them; 0 outside one
    int floor = 0;

    // the registers the prologue pushes, which the body's first slot
    // lies below
    int saved = 0;

    // the read of a compound assignment's register variable, which its
    // operation writes in place
    int in_place = -1;

    // the registers take() leaves for last while a value meant for a
    // pinned register is generated, so it lands there
    unsigned reserved = 0;

    // the registers a pinned instruction within the right operand of a
    // node whose left operand is being generated would move that left
    // operand's value out of, moved_within()'s: a row that chooses its
    // register writes none of them (6.1)
    unsigned ahead = 0;

    // the borrowed read whose copy preserve() writes into the register
    // its node's consumer names, held out of the right operand that
    // writes the read's variable, and that register, as a set of one
    int named_read = -1;
    unsigned named_copy = 0;

    // an address being formed is held across a right operand that
    // spills every held value, a call or a branch within it: an unscaled
    // sum then keeps to one register (backend.md 6.3)
    bool held_across = false;

    // an assignment's place is being formed across a right operand that
    // spills every held value: a label the place's index joins stays the
    // place's label, for formed() to write after the right operand (6.3)
    bool defer_label = false;

    // the width of the result from which the last instruction written
    // set ZF, or 0 when it set none: flagged() states it after such an
    // instruction, and the next instruction or label withdraws it
    int zero_flag = 0;

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
void cbranch(state_t &, const expr_t &, int label, bool sense);
void
arm_into(state_t &, const expr_t &, const into_t &, const type_t * = nullptr);

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

// the register a consumer names

// The register of wanted, the set of at most one expression register
// that the consumer of a value names, rax for a return's (backend.md
// 6.1), where a row may write it now: held by no value and left for
// last by no reservation, since a row never writes a register that
// holds a live value.  None for an empty set, and for a register that
// is held or reserved.
std::optional< reg_t >
wanted_register(const state_t &s, unsigned wanted)
{
    for (const reg_t r : expression_registers)
        if (wanted & bit(r))
            return s.owner[r] < 0 && !(s.reserved & bit(r)) ?
                       std::optional< reg_t >(r) :
                       std::nullopt;

    return std::nullopt;
}

// The registers a node generates its right operand rhs with left for
// last, so that its result, written into its left operand l's register,
// can take the register its consumer names, wanted (backend.md 6.1):
// wanted, where l is a borrowed read, which the instruction copies
// after rhs into the lowest free register, but for a rhs that writes
// l's variable, whose preserve() copies the read inside rhs, which
// copied_into() hands the name; and where
// l holds wanted or is spilled, so that a rhs that spills l has it
// reloaded there.  None where l holds another register, where the
// result stays.
unsigned
held_out(const state_t &s, value_t l, const expr_t &rhs, unsigned wanted)
{
    if (!wanted || l.id < 0)
        return 0;

    const auto &h = s.values[l.id];
    if (h.variable)
        return writes(rhs, h.variable) ? 0 : wanted;

    if (h.reg >= 0 && !(bit(static_cast< reg_t >(h.reg)) & wanted))
        return 0;

    return wanted;
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

// The registers the instruction of e or of any operand within it pins,
// pins()'s over the whole tree, and rax for a call, whose result the
// convention puts there (chapter 8): a value held while e is generated
// may have to move out of one of them, as 6.5's boundary has it for a
// division deeper in a right operand, and a value of e generated
// outside one may have to move into it, as ?:'s second arm into a
// first arm's call result.  A statement expression is taken as a call:
// its statements may call, and it spills every held value first, as a
// call does, where they may move one, and holds no pin elsewhere (6.7).
unsigned
pinned_within(const expr_t &e)
{
    const bool calls = is< call_t >(e) || is< statement_expression_t >(e);
    const unsigned own = calls ? bit(rax) : pins(e);

    if (is< unary_t >(e))
        return own | pinned_within(*as< unary_t >(e).arg);

    if (is< binary_t >(e)) {
        const auto &b = as< binary_t >(e);
        return own | pinned_within(*b.lhs) | pinned_within(*b.rhs);
    }

    if (is< assign_t >(e)) {
        const auto &a = as< assign_t >(e);
        return own | pinned_within(*a.lhs) | pinned_within(*a.rhs);
    }

    if (is< call_t >(e)) {
        const auto &c = as< call_t >(e);

        unsigned pinned = own | pinned_within(*c.callee);
        for (const auto &arg : c.args)
            pinned |= pinned_within(arg);

        return pinned;
    }

    if (is< cast_t >(e))
        return own | pinned_within(*as< cast_t >(e).arg);

    if (is< conditional_t >(e)) {
        const auto &c = as< conditional_t >(e);
        return own | pinned_within(*c.cond) |
               (c.yes ? pinned_within(*c.yes) : 0) | pinned_within(*c.no);
    }

    return own;
}

// The registers a pinned instruction within e would move a value held
// across e out of, 6.5: pins() over the tree, but not within a call, a
// branch of &&, || or ?:, or a statement expression, which spill every
// held value first, wherever it is, the last but where its statements
// hold no pin (6.7).
unsigned
moved_within(const expr_t &e)
{
    if (is< call_t >(e) || is< conditional_t >(e) ||
        is< statement_expression_t >(e))
        return 0;

    if (is< unary_t >(e))
        return pins(e) | moved_within(*as< unary_t >(e).arg);

    if (is< binary_t >(e)) {
        const auto &b = as< binary_t >(e);
        if (b.op == binary_op_t::log_and || b.op == binary_op_t::log_or)
            return 0;

        return pins(e) | moved_within(*b.lhs) | moved_within(*b.rhs);
    }

    if (is< assign_t >(e)) {
        const auto &a = as< assign_t >(e);
        return pins(e) | moved_within(*a.lhs) | moved_within(*a.rhs);
    }

    if (is< cast_t >(e))
        return pins(e) | moved_within(*as< cast_t >(e).arg);

    return pins(e);
}

// The register a node hands its left operand, of those its consumer
// names, wanted (backend.md 6.1): wanted, outside own, the registers the
// node's own instruction pins, where no instruction within its right
// operand rhs pins one.  The left operand then holds wanted while rhs
// is generated, or takes it after rhs, a borrowed read's copy or a
// spilled value's reload, and rhs is generated with wanted left for
// last (held_out()): a pin within rhs would move the left operand out
// of wanted, or find rhs's own values in the registers it pins.  None
// otherwise, and the node is as the lowest free register makes it.
unsigned
wanted_left(unsigned wanted, unsigned own, const expr_t &rhs)
{
    return pinned_within(rhs) ? 0 : wanted & ~own;
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

value_t
rvalue(state_t &, const expr_t &, into_t * = nullptr, unsigned wanted = 0);
void effect(state_t &, const expr_t &);

// GNU C's statement expression's rows, c1-statements.hh's, which
// generate its statements and so come after them.
value_t statement_expression(state_t &, const expr_t &, into_t *, unsigned);
void statement_expression_effect(state_t &, const expr_t &);
void statement_expression_branch(state_t &, const expr_t &, int, bool);
const expr_t &operand_of(state_t &, const expr_t &);

// The value of e generated while the registers in pinned are left for
// last, so that it does not land where an instruction generated after
// it wants its operands (chapter 6.5).  The reservation is an
// avoidance, not a pin (take()), and a call or a division inside e
// keeps its own pin (rvalue()).  wanted is the register e's consumer
// names, rvalue()'s.
value_t
rvalue_avoiding(state_t &s,
                const expr_t &e,
                unsigned pinned,
                into_t *into = nullptr,
                unsigned wanted = 0)
{
    const unsigned reserved = s.reserved;
    s.reserved |= pinned;
    const auto v = rvalue(s, e, into, wanted);
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

// A value no row wrote into the register variable sym's register moved
// in, establish()'s mov, but for a read of sym itself, under
// conversions that write nothing too: the register holds the variable's
// representation already, which the mov would write again, so r = c ?
// x : r on a register r writes no movl %ebx, %ebx on the second path,
// and r = r nothing at all (chapter 4).
void
move_in(state_t &s, const symbol_t *sym, value_t v)
{
    if (s.values[v.id].variable == sym)
        return;

    establish(s, sym, reg_of(s, v));
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

// The register preserve() copies the held read id into: the one its
// node's consumer names, s.named_copy, for the read s.named_read, where
// it is free (backend.md 6.1), and otherwise the lowest free register.
reg_t
copy_register(state_t &s, int id)
{
    if (id == s.named_read)
        for (const reg_t r : expression_registers)
            if ((s.named_copy & bit(r)) && s.owner[r] < 0)
                return r;

    return take(s, 0);
}

// Before a register variable is written, each of its reads still held
// is copied into an expression register, so it keeps the value it
// read: G13, an operand's side effects complete before the next operand
// starts, and i = i++ + ++i has one value.  The read a node's consumer
// names a register for is copied there, copy_register()'s.
void
preserve(state_t &s, const symbol_t *sym)
{
    for (std::size_t id = 0; id < s.values.size(); ++id) {
        const auto &h = s.values[id];
        if (!h.live || h.variable != sym)
            continue;

        const int read = static_cast< int >(id);
        copy(s, { read }, copy_register(s, read));
    }
}

// The value extended by from's signedness to a width of 32 or 64, where
// it is not valid to it already, the width rule.  A borrowed read is
// extended into a fresh register, and the value is then that one.  The
// register the value's consumer names, wanted, takes the extension
// where it is free and the value is elsewhere, movslq %edx, %rax for a
// remainder returned as a long (6.1); otherwise an owned value is
// extended in its own register.  A register variable handed down that
// no row has written takes the extension, the last instruction of the
// value and the first write of its register, movslq %eax, %rbx for r =
// a + b into a register long from two ints (chapter 4): the operand,
// generated before it, reads the variable where it must.
value_t
extended(state_t &s,
         value_t v,
         const int_t &from,
         int to,
         unsigned wanted = 0,
         into_t *into = nullptr)
{
    if (s.values[v.id].valid >= to)
        return v;

    const auto [mnemonic, width] = extension(from, std::max(32, to));
    const reg_t source = reg_of(s, v);
    const auto named = wanted_register(s, wanted);
    const int valid = written(width, from.is_signed);

    if (takes(into, valid)) {
        release(s, v);
        insn(s, mnemonic,
             { reg(source, from.width),
               reg(variable_register(into->variable), width) });
        return taken(s, *into);
    }

    if (!borrowed(s, v) && !named) {
        insn(s, mnemonic, { reg(source, from.width), reg(source, width) });
        s.values[v.id].valid = valid;
        return v;
    }

    release(s, v);

    const auto e = named ? hold(s, *named, valid) : fresh(s, valid);
    insn(s, mnemonic, { reg(source, from.width), reg(reg_of(s, e), width) });

    return e;
}

// addresses

// An lvalue's address as the operand of the instruction that reads or
// writes it: a slot off rbp, a label off rip with a displacement of
// its own, table+12(%rip), or the machine's addressing form, a held
// base with a displacement or with a held index scaled by 1, 2, 4 or 8.
// A slot with a held index is the form with rbp the base,
// -24(%rbp,%rcx,4).  The access consumes the values.  sum marks the
// index as the product of a sum at a size no addressing form scales by,
// whose value with no displacement is the p + n row's addq (6.3).
struct address_t
{
    value_t base { };
    value_t index { };
    long scale = 1;
    long long displacement = 0;
    std::string label { };
    bool sum = false;
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
// the other's register.  A label with an index has no form: formed()
// writes it first.
std::string
operand(state_t &s, const address_t &a, unsigned avoid = 0)
{
    if (!a.label.empty() && a.index.id >= 0)
        throw std::logic_error("c1: a label with an index has no operand");

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

// An assignment's place as an operand holds it, backend.md 6.3: a label
// beside an index, which place_of() keeps across a right operand that
// spills, written now, after that right operand, where the place is
// first wanted: the index reloaded, then the label's leaq into a
// register outside avoid and the index's, the base, leaq g(%rip), %rdx
// for movl %eax, (%rdx,%rcx,4).  Any other place is as it is.  The
// caller consumes the place returned.
address_t
formed(state_t &s, address_t place, unsigned avoid = 0)
{
    if (place.label.empty() || place.index.id < 0)
        return place;

    const reg_t index = reg_of(s, place.index, avoid | registers_of(s, place));
    const auto base = fresh(s, 64, avoid | bit(index));
    insn(s, "leaq", { place.label + "(%rip)", reg(reg_of(s, base), 64) });

    place.base = base;
    place.label.clear();

    return place;
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
// evaluated once, 6.5.17.3p4.  Handed a register variable, the load
// writes the variable's register where that leaves its
// representation, and the value is the variable's read (chapter 4); not
// where the address reads the variable's register, r = a[r]++, since
// the store after the load goes through the same address.  Otherwise
// the value takes a fresh register outside the address's and avoid.
value_t
fetch(state_t &s,
      const address_t &a,
      const int_t &t,
      int width,
      into_t *into = nullptr,
      unsigned avoid = 0)
{
    const auto [mnemonic, to] = extension(t, width);
    const int valid = written(to, t.is_signed);

    const auto from = operand(s, a);
    if (takes(into, valid) &&
        !(registers_of(s, a) & bit(variable_register(into->variable)))) {
        insn(s, mnemonic, { from, reg(variable_register(into->variable), to) });
        return taken(s, *into);
    }

    const auto v = fresh(s, valid, avoid | registers_of(s, a));
    insn(s, mnemonic, { from, reg(reg_of(s, v), to) });

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

// The value of a sum at a size no addressing form scales by, with no
// displacement, backend.md 6.3: the p + n row's addq of the index's
// product into the base, addq %rcx, %rax after imulq $12, %rcx, %rcx,
// as offset() writes it, where the leaq of the address would only
// replace it.
value_t
summed_base(state_t &s, value_t base, value_t index)
{
    const reg_t right = operands(s, base, index).second;
    const reg_t left = writable(s, base, 64, bit(right));
    insn(s, "addq", { reg(right, 64), reg(left, 64) });

    release(s, index);

    return base;
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

    if (a.sum && a.base.id >= 0 && a.displacement == 0)
        return summed_base(s, a.base, a.index);

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

// The register variable an address's base is generated into, where a
// displacement follows it, chapter 4: a copy of the one handed down, so
// that the base's own row may write the variable's register and the
// leaq of the address, address_of()'s, still writes it after, movq
// -16(%rbp), %rbx; leaq 12(%rbx), %rbx.  A row that cannot write it
// leaves the copy untaken, and the leaq writes the variable as before.
// None where none is handed down or a row has written the variable.
std::optional< into_t >
base_into(const into_t *into)
{
    if (!into || into->taken)
        return std::nullopt;

    return into_t { into->variable, false, into->as };
}

const expr_t *memory_operand(const expr_t &, int);
const expr_t *cast_sum(const expr_t &, const into_t *);
address_t pointed(state_t &, const expr_t &, const type_t &, into_t *);

// Whether the pointer p, read through casts between pointer types, is a
// sum at a stride of 1 whose index is an object of 64 bits in memory,
// which its addq takes as its operand, addq -24(%rbp), %rax, where the
// addressing form would load it (backend.md 6.1).
bool
memory_indexed(const expr_t &p)
{
    const expr_t *e = &p;
    while (is< cast_t >(*e) && is< pointer_t >(as< cast_t >(*e).arg->type))
        e = as< cast_t >(*e).arg.get();

    if (!is< binary_t >(*e) || !is< pointer_t >(e->type) ||
        stride(e->type) != 1)
        return false;

    const auto &b = as< binary_t >(*e);
    const auto &n = is< pointer_t >(b.lhs->type) ? *b.rhs : *b.lhs;

    return !is< literal_t >(n) && memory_operand(n, 64);
}

// The address the value of the pointer p denotes, as an operand that no
// instruction has formed yet, backend.md 6.3: pointed()'s for p's own
// pointee, the base, the displacement and the index p's rows form,
// through a cast between pointer types too, cast_sum()'s, so that a
// constant or an index over p joins them where the form holds it,
// held across a pinned instruction too, the constants the access's
// displacement, which holds no register: ((long (*)[2]) (p +
// 800))[i][1] = f() is 808(%rcx) after the call, and place_of() sums
// a base and an index that the right operand would spill.  None, and
// p's value formed as before, for a sum at a stride of 1 by an index
// in memory, memory_indexed()'s.
std::optional< address_t >
denoted(state_t &s, const expr_t &p)
{
    if (memory_indexed(p))
        return std::nullopt;

    return pointed(s, p, *as< pointer_t >(p.type).pointee, nullptr);
}

// The operand of an address by a pointer and a constant index,
// backend.md 6.3: the pointer's value the base and the index times the
// size the displacement, whatever the size, 12(%rax) for p[3] on an
// int *p and for the row p[1] on int (*p)[3], whose 12 bytes are no
// scale, so that the row as a pointer is leaq 12(%rax), %rax.  Only the
// pointer is evaluated.  None for any other pointer, and for a product
// no 32-bit displacement holds, which the rows of p + n and of an index
// reach.  A register variable handed down is the destination of the
// pointer's row, base_into()'s, so that q = p[1] into a register int *q
// is movq -16(%rbp), %rbx; leaq 12(%rbx), %rbx (chapter 4); not where
// the product wraps to 0, since no leaq follows to write the variable.
// A constant subtracted is the displacement negated, -16(%rcx,%rdx,1)
// for (q - 1)[i] on long (*q)[2].  Any other pointer is denoted()'s
// address, the constant joining its
// displacement, ((int *) (p + 1))[3] on a char *p being 13(%rax) and
// *(short *) ((char *) p + i * 4 + 2) 2(%rax,%rcx,1); a label's, whose
// offset must stay within its object, and a sum no 32-bit displacement
// holds are that address formed by its leaq, the constant the
// displacement off it.
std::optional< address_t >
constant_indexed(state_t &s, const expr_t &p, long size, into_t *into = nullptr)
{
    if (!is< binary_t >(p))
        return std::nullopt;

    const auto op = as< binary_t >(p).op;
    if (op != binary_op_t::add && op != binary_op_t::sub)
        return std::nullopt;

    const auto &b = as< binary_t >(p);
    const bool left = is< pointer_t >(b.lhs->type);
    const auto &index = left ? *b.rhs : *b.lhs;
    if (!is< literal_t >(index))
        return std::nullopt;

    const auto d = scaled(as< literal_t >(index).value,
                          op == binary_op_t::sub ? -size : size);
    if (!fits_imm32(d))
        return std::nullopt;

    const auto &pointer = left ? *b.lhs : *b.rhs;
    auto onto = d != 0 ? base_into(into) : std::nullopt;

    if (!onto)
        if (auto a = denoted(s, pointer)) {
            const auto sum = wrapped_add(a->displacement, d);
            if (a->label.empty() && fits_imm32(sum)) {
                a->displacement = sum;
                return *a;
            }

            return address_t { .base = address_of(s, *a), .displacement = d };
        }

    const auto base = rvalue(s, pointer, onto ? &*onto : nullptr);

    return address_t { .base = base, .displacement = d };
}

address_t address(state_t &, const expr_t &, into_t * = nullptr);

// Whether an lvalue's address is a base the row that forms it can
// write into a register variable, chapter 4: the GOT's load of a global
// the unit only declares, or the value of the pointer a dereference
// reads, where that pointer is no sum by a variable index, whose index
// joins the address rather than its base (6.3).
bool
whole_base(const state_t &s, const expr_t &lvalue)
{
    if (const auto *g = global_of(lvalue))
        return !s.defined.contains(g);

    if (!is< unary_t >(lvalue) || as< unary_t >(lvalue).op != unary_op_t::deref)
        return false;

    const auto &p = *as< unary_t >(lvalue).arg;
    if (!is< binary_t >(p) || !is< pointer_t >(p.type))
        return true;

    const auto &b = as< binary_t >(p);

    return is< literal_t >(is< pointer_t >(b.lhs->type) ? *b.rhs : *b.lhs);
}

// The operand of an address by any other lvalue and a constant,
// backend.md 6.3: the lvalue's own address, address()'s, with the
// constant summed into its displacement, 2(%rax,%rcx,8) for
// m[i][2] on char (*m)[8], 20(%rax) for p[1][2] on int (*p)[3];
// nothing else is evaluated, and no instruction forms the intermediate
// address.  The lvalue is a dereference whose pointer is no name's
// address plus a constant, or a global the unit only declares, whose
// address the GOT gives.  None for a constant no 32-bit displacement
// holds, and for a name's address, folded()'s, whose bounds hold.  A
// sum that does not fit forms the lvalue's address, and the constant is
// the displacement off it.  A register variable handed down goes on to
// the lvalue's address where the constant is 0, so that the address is
// the lvalue's, the base of &a[i][0] on int a[4][6] (chapter 4).  Under
// any other constant it is the destination of a whole_base()'s row,
// base_into()'s, so that q = &lga[3] on an array the unit only
// declares is movq lga@GOTPCREL(%rip), %rbx; leaq 12(%rbx), %rbx, and
// a sum that comes to 0, &p[1][-3], leaves the pointer's value in the
// variable, which the assignment moves nowhere.
std::optional< address_t >
summed(state_t &s, const expr_t &pointer, into_t *into = nullptr)
{
    const auto at = lvalue_offset(pointer);
    if (!at || !fits_imm32(at->offset) || named_offset(pointer, s.defined))
        return std::nullopt;

    into_t *to = at->offset == 0 ? into : nullptr;
    auto onto = at->offset != 0 && whole_base(s, *at->lvalue) ?
                    base_into(into) :
                    std::nullopt;
    if (onto)
        to = &*onto;

    auto a = address(s, *at->lvalue, to);

    const auto displacement = wrapped_add(a.displacement, at->offset);
    if (!fits_imm32(displacement))
        return address_t { .base = address_of(s, a),
                           .displacement = at->offset };

    a.displacement = displacement;

    return a;
}

// The pointer of a subscript or of a sum by a variable index as the
// address the index joins, backend.md 6.3.  An address of a base and a
// displacement and no index, summed()'s or constant_indexed()'s, is
// held whole, so that p[1][i] on int (*p)[3] is 12(%rax,%rcx,4) and no
// instruction forms the row's address.  Any other address, one with an index or
// with no register for its base, is formed by address_of()'s leaq, and
// any other pointer, a name's address plus a constant among them, whose
// label takes no index, is its value: the base alone, computed in the
// register its consumer names, wanted, as rvalue() computes it.  The
// registers are taken with those in pinned left for last, as
// rvalue_avoiding() takes them.  An assignment's place on a label held
// across a right operand that spills, defer_label, keeps the label,
// with no register for it, for formed() to write after the right
// operand: g[i] = f() holds the index alone across the call, then leaq
// g(%rip), %rdx and movl %eax, (%rdx,%rcx,4).  A label's address reads
// nothing, so the order of evaluation has no say in where it is formed.
address_t
index_base(state_t &s,
           const expr_t &pointer,
           unsigned pinned = 0,
           unsigned wanted = 0)
{
    if (std::exchange(s.defer_label, false))
        if (const auto f = folded(s, pointer); f && !f->label.empty())
            return *f;

    const unsigned reserved = std::exchange(s.reserved, s.reserved | pinned);

    auto a = summed(s, pointer);
    if (!a && !named_offset(pointer, s.defined))
        a = constant_indexed(s, pointer,
                             size_of(*as< pointer_t >(pointer.type).pointee));

    // ((int *) (p + 1))[i] on a char *p: 1(%rax,%rcx,4)
    if (!a && cast_sum(pointer, nullptr))
        a = denoted(s, pointer);

    if (!a)
        a = address_t { .base = rvalue(s, pointer, nullptr, wanted) };
    else if (a->base.id < 0 || a->index.id >= 0)
        a = address_t { .base = address_of(s, *a) };

    s.reserved = reserved;

    return *a;
}

// Whether p + n is a sum at a size no addressing form scales by, size,
// whose product the addressing form takes as its index at scale 1,
// backend.md 6.3: an add, the pointer written first, and n no constant.
bool
unscaled_sum(const operation_t &o, long size)
{
    return !is_scale(size) && o.op == binary_op_t::add &&
           is< pointer_t >(o.lhs->type) && !is< literal_t >(*o.rhs);
}

// The address of the pointer of an unscaled_sum(), which its index's
// product joins, backend.md 6.3: a local array's slot plus a constant,
// folded()'s, with rbp the base and no instruction, and any other
// pointer index_base()'s, an address of a base and a displacement or
// the base alone, generated outside pinned and in the register the
// consumer names, named.  An address held_across a right operand that
// spills takes the slot's leaq too, since it keeps to one register.
address_t
unscaled_pointer(state_t &s,
                 const expr_t &pointer,
                 unsigned pinned,
                 unsigned named)
{
    if (const auto slot = folded(s, pointer);
        slot && slot->label.empty() && !s.held_across)
        return *slot;

    return index_base(s, pointer, pinned, named);
}

address_t unscaled(state_t &,
                   const operation_t &,
                   long,
                   unsigned,
                   std::optional< unsigned > = std::nullopt);
bool unscaled_first(const binary_t &, long);
address_t index_unscaled(state_t &, const binary_t &, long);

// The sum under a cast of an access's pointer between pointer types,
// backend.md 6.3, whose rows form the access's operand as the sum's own
// pointer's: its stride scales the index and its bytes are the
// displacement, whatever the access's size, since the operand holds any
// displacement and scales by 1, 2, 4 or 8, so *(int *) (p + 24) on a
// char *p is 24(%rax) and *(long *) (s + i) on a short *s (%rax,%rcx,2).
// The access is at the cast's type all the same (G10), at the address
// the source computes (G11).  None for any other pointer, an array's
// decay among them, for an address handed a register variable, whose
// rows are the value's, and for an index of 64 bits in memory at a
// stride of 1, memory_indexed()'s.  An address held across a right
// operand that spills it is summed into one register by place_of().
const expr_t *
cast_sum(const expr_t &p, const into_t *into)
{
    if (into || !is< cast_t >(p) || !is< pointer_t >(p.type))
        return nullptr;

    const auto &sum = *as< cast_t >(p).arg;
    if (!is< pointer_t >(sum.type) || !is< binary_t >(sum))
        return nullptr;

    const auto &b = as< binary_t >(sum);
    if (b.op != binary_op_t::add && b.op != binary_op_t::sub)
        return nullptr;

    if (memory_indexed(p))
        return nullptr;

    return &sum;
}

// The address a dereference designates, 6.5.4.2p4, of an object of
// type pointee: the addressing row.  A name's address plus a constant,
// 0 for *a and *&x, is the one operand of folded(), and any other
// lvalue's address plus a constant is that address with the constant in
// its displacement, summed().  A subscript by a constant 0 is the
// bare base, the pointer's value with no displacement and no index,
// whatever the pointee's size: m[0] on int m[2][3] is m, as p[0] is p.
// Any other constant index is the displacement c * size, whatever the
// size, when it fits 32 bits, constant_indexed()'s.  Any other add of a
// pointer and an integer, the shape of a subscript (6.5.3.2p2), is the
// machine's addressing form when the size is a scale: the pointer the
// base, the integer the index, each generated in the order written
// (G13); a variable index into a local array has rbp the base and the
// slot the displacement, indexed(), and one into a pointer that is a
// base and a displacement joins that address, index_base()'s, p[1][i]
// on int (*p)[3] being 12(%rax,%rcx,4).  The operand written first is
// held outside the registers the other's top operator pins, p[x / y]'s
// p outside rax and rdx (chapter 6.5).  At a size no addressing form
// scales by, the index's product is the form's index at scale 1, the
// pointer's address its base and displacement, unscaled()'s, so that
// m[i][4] on a local char m[4][5] is -23(%rbp,%rcx) after imulq $5,
// %rcx, %rcx, and the index written first the same, index_unscaled()'s.  A cast of the pointer between pointer types is looked
// through to the sum under it, cast_sum()'s, so *(int *) (p + 24) on a
// char *p is 24(%rax).  Any other pointer, a sum of another size
// included, is computed into a register and is the base.  A register
// variable handed down goes
// to the row of a base that is the whole address, p of &*p and &p[0],
// or the sum of &a[i] at a size no addressing form scales by, to
// summed()'s, and to constant_indexed()'s pointer (chapter 4).
address_t
pointed(state_t &s,
        const expr_t &p,
        const type_t &pointee,
        into_t *into = nullptr)
{
    // *(int *) (p + 24) on a char *p: 24(%rax)
    if (const auto *sum = cast_sum(p, into))
        return pointed(s, *sum, *as< pointer_t >(sum->type).pointee);

    if (const auto a = folded(s, p))
        return *a;

    // m[i][2], p[1][2]: 2(%rax,%rcx,8), 20(%rax)
    if (const auto a = summed(s, p, into))
        return *a;

    const long size = size_of(pointee);

    // p[3], and the row p[1] on int (*p)[3]: 12(%rax)
    if (const auto a = constant_indexed(s, p, size, into))
        return *a;

    // m[i][4] on char m[4][5], p[1][i][0] on int (*p)[3][3]:
    // -23(%rbp,%rcx), 36(%rax,%rcx) after imulq
    if (!into && is< binary_t >(p) &&
        unscaled_sum(as_written(as< binary_t >(p)), size))
        return unscaled(s, as_written(as< binary_t >(p)), size, 0);

    // i[q + 1][1] on long (*q)[2]: 24(%rcx,%rax,1) after imulq
    if (!into && !s.held_across && is< binary_t >(p) &&
        unscaled_first(as< binary_t >(p), size))
        return index_unscaled(s, as< binary_t >(p), size);

    if (!is_scale(size) || !is< binary_t >(p) ||
        as< binary_t >(p).op != binary_op_t::add)
        return { .base = rvalue(s, p, into) };

    const auto &b = as< binary_t >(p);
    const bool left = is< pointer_t >(b.lhs->type);

    // a[i], m[1][i] on a local array: -24(%rbp,%rcx,4)
    if (const auto a = indexed(s, b, size))
        return *a;

    // p[i], and p[1][i] on int (*p)[3]: 12(%rax,%rcx,4)
    if (left) {
        auto a = index_base(s, *b.lhs, pinned_by(*b.rhs));
        a.index = rvalue(s, *b.rhs);
        a.scale = size;

        return a;
    }

    // the index first: a label it reads is not the place's (place_of())
    const bool defer = std::exchange(s.defer_label, false);
    const auto index = rvalue_avoiding(s, *b.lhs, pinned_by(*b.rhs));
    s.defer_label = defer;

    auto a = index_base(s, *b.rhs);
    a.index = index;
    a.scale = size;

    return a;
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
// the source writes loads the address once.  A register variable
// handed down is the destination of the GOT's load, symbol_address()'s
// (chapter 4).
address_t
global_address(state_t &s, const symbol_t &g, into_t *into = nullptr)
{
    if (s.defined.contains(&g))
        return { .label = std::string(assembly_name(g)) };

    return { .base = symbol_address(s, g, into) };
}

// The address of an lvalue: a local's or a parameter's slot, a global's
// label or its address from the GOT, a string literal's label, or what a
// dereference designates.  It serves &, array decay, loads and stores
// alike, so the addressing row is written once.  A register variable
// handed down goes on to pointed()'s rows.
address_t
address(state_t &s, const expr_t &e, into_t *into)
{
    if (is< string_t >(e))
        return { .label = string_label(s.strings, as< string_t >(e)) };

    if (is< unary_t >(e) && as< unary_t >(e).op == unary_op_t::deref)
        return pointed(s, *as< unary_t >(e).arg, e.type, into);

    if (const auto *g = global_of(e))
        return global_address(s, *g, into);

    return slot_address(slot_of(s, object_of(e)));
}

// The address of an lvalue as a value, the row of & and of an array's
// decay: the leaq of address()'s operand, address_of()'s, and for a
// global symbol_address()'s, the load from the GOT for one the unit
// only declares.  A register variable handed down is the destination of
// either, and of the pointer's own row where the address of *p is p's
// value, p = &*q being the load of q into the register (chapter 4).
value_t
address_value(state_t &s, const expr_t &lvalue, into_t *into)
{
    if (const auto *g = global_of(lvalue))
        return symbol_address(s, *g, into);

    if (is< unary_t >(lvalue) && as< unary_t >(lvalue).op == unary_op_t::deref)
        return address_of(
            s, pointed(s, *as< unary_t >(lvalue).arg, lvalue.type, into), into);

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
    const expr_t &top = under_kept_bits(e);
    if (!in_memory(top) || integer(top.type).width != width)
        return nullptr;

    return &top;
}

// The register variable whose register an instruction of width bits
// names for e, or nullptr: a plain read of the variable, of a type
// exactly that wide, under casts that keep its bits, so its register at
// the width is the value (chapter 4).  A char or a short promoted and
// an int widened are under a cast that changes the width.
const symbol_t *
register_operand(const expr_t &e, int width)
{
    const expr_t &top = under_kept_bits(e);

    const auto *sym = register_variable(top);
    if (!sym || integer(top.type).width != width)
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

// Whether op is an operator of two addresses on a free register, the
// operation form's +, -, *, &, |, ^ and the shifts (backend.md chapter
// 4).
bool
two_address(binary_op_t op)
{
    switch (op) {
    case binary_op_t::add:
    case binary_op_t::sub:
    case binary_op_t::mul:
    case binary_op_t::bit_and:
    case binary_op_t::bit_or:
    case binary_op_t::bit_xor:
    case binary_op_t::shl:
    case binary_op_t::shr: return true;
    default: return false;
    }
}

// Whether e is an operation whose row writes its own register at its
// own width: an operator of two addresses, and - and ~ (chapter 4).
bool
operation(const expr_t &e)
{
    if (is< unary_t >(e))
        return as< unary_t >(e).op == unary_op_t::neg ||
               as< unary_t >(e).op == unary_op_t::bit_not;

    return is< binary_t >(e) && two_address(as< binary_t >(e).op);
}

// What a conversion from from to target hands on to its operand's row
// of a register variable into hands down, backend.md chapter 4:
// through()'s, and, where into's row computes in another signedness,
// into itself through a conversion that changes the signedness alone,
// whose operand's row writes the same bits at the same width, v = 5 +
// (int) u into a register unsigned v being movl -12(%rbp), %ebx; addl
// $5, %ebx.
into_t *
onward(into_t *into, const int_t &from, const int_t &target)
{
    if (into && !into->taken && into->as && from.width == target.width)
        return into;

    return through(into, target, target.width > from.width);
}

// The integer conversion of the operand e to the type to.  G2:
// narrowing is no instruction, the narrower register name is the
// value; widening extends by the operand's signedness, and only when
// the register is not already valid to the new width, as it is after
// the load of a char or a 32-bit write of an unsigned value.  A signed
// object in memory is loaded extended to the new width,
// extending_load(), so the load is the widening; an unsigned one is
// loaded as any read, valid to 64 bits by the load's own extension.
// The object may be under casts that keep its bits, object_under()'s:
// (long) (int) u is movslq -4(%rbp), %rax, and (unsigned long)
// (unsigned) i the movl, valid to 64 bits, each reading the object at
// its own width (G10); and under a promotion, widened_read()'s, (long)
// (int) c on a char c the one movsbq.  A signed object widened to an
// unsigned of 32 bits is its load into the 32-bit name, which clears
// the upper half, so the value is valid to 64 bits as the unsigned it
// is: (long) (unsigned) c is the one movsbl, and no movl clears it
// again; not a register variable's read, whose upper half is its own
// type's (chapter 4).  A signed constant widened is the constant of
// the new type, its row's mov, movq $-1, %rbx, where an unsigned one's
// movl leaves it valid to 64 bits already.  A change of signedness
// alone is nothing.  A pointer is its integer view, so
// pointer to pointer is nothing, an integer widens to a pointer by its
// own signedness (6.3.2.3p5), and a pointer narrows to an integer by
// its low bits (p6).  A register variable widens into a fresh register,
// movslq %ebx, %rax, and narrows as its read: the read, not the
// variable, is valid to less.  A register variable handed down goes on
// to the operand's row onward() through a conversion that leaves its
// representation, and a load takes it: register int x = c is movsbl
// -1(%rbp), %ebx (chapter 4); an unsigned operation of 32 bits widened
// to 64 is handed it as computed in its own type, the operation form on
// the register, since its 32-bit write clears the upper half.  The
// register the consumer of the value names, wanted, goes on to the
// operand, and a widening writes its extension there, extended()'s
// (6.1).
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

    if (widens && from.is_signed && is< literal_t >(e))
        return literal(s, { widened(as< literal_t >(e).value, from) }, to,
                       into);

    // r = a * b into a register unsigned long from two unsigneds: the
    // operation's 32-bit write is the widening, movl -12(%rbp), %ebx;
    // imull -16(%rbp), %ebx
    const bool zero = widens && !from.is_signed && from.width == 32 &&
                      operation(e) && into && !into->taken && !into->as;
    into_t computed { zero ? into->variable : nullptr, false, &e.type };
    const auto next = zero ? &computed : onward(into, from, target);

    const auto read = widens ? widened_read(e) : widened_read_t { };
    const bool loads =
        read.object && (read.type->is_signed || read.object != &e);
    const int width = loads && read.type->is_signed ? target.width : 32;

    auto v = loads ? extending_load(s, *read.object, *read.type, width, next) :
                     rvalue(s, e, next, wanted);
    if (computed.taken)
        into->taken = true;
    if (widens)
        v = extended(s, v, from, target.width, wanted, into);

    auto &valid = s.values[v.id].valid;
    valid = std::min(valid, target.width);

    // (long) (unsigned) c on a char c: movsbl -1(%rbp), %eax, its
    // 32-bit write the zero extension of the unsigned
    if (loads && read.type->is_signed && !target.is_signed && width == 32 &&
        !borrowed(s, v))
        valid = written(32, false);

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

// Whether generating e spills every value held across it, 6.7: a call
// within it, a branch of &&, || or ?:, or a statement expression, taken
// as one that does whatever its statements.
bool
spills_within(const expr_t &e)
{
    if (is< call_t >(e) || is< conditional_t >(e) ||
        is< statement_expression_t >(e))
        return true;

    if (is< unary_t >(e))
        return spills_within(*as< unary_t >(e).arg);

    if (is< binary_t >(e)) {
        const auto &b = as< binary_t >(e);
        return b.op == binary_op_t::log_and || b.op == binary_op_t::log_or ||
               spills_within(*b.lhs) || spills_within(*b.rhs);
    }

    if (is< assign_t >(e)) {
        const auto &a = as< assign_t >(e);
        return spills_within(*a.lhs) || spills_within(*a.rhs);
    }

    if (is< cast_t >(e))
        return spills_within(*as< cast_t >(e).arg);

    return false;
}

// Whether the lvalue e is reached from a label, backend.md 6.3: its
// pointer, read through casts between pointer types, the decay of a
// row and the pointer operand of each sum, a global's or a string's
// address, whose leaq is
// the base an index cannot join, so a place there is left as it is.
bool
label_rooted(const expr_t &e,
             const std::unordered_set< const symbol_t * > &defined)
{
    if (!is< unary_t >(e) || as< unary_t >(e).op != unary_op_t::deref)
        return false;

    const expr_t *p = as< unary_t >(e).arg.get();
    while (is< cast_t >(*p) || is< binary_t >(*p)) {
        if (is< cast_t >(*p) && is< pointer_t >(as< cast_t >(*p).arg->type)) {
            p = as< cast_t >(*p).arg.get();
            continue;
        }

        // a row's decay, m[i] of m[i][1]: the row's own pointer
        if (is< cast_t >(*p)) {
            const auto &row = *as< cast_t >(*p).arg;
            if (!is< array_t >(row.type) || !is< unary_t >(row) ||
                as< unary_t >(row).op != unary_op_t::deref)
                break;

            p = as< unary_t >(row).arg.get();
            continue;
        }

        if (!is< binary_t >(*p) || !is< pointer_t >(p->type))
            break;

        const auto &b = as< binary_t >(*p);
        p = is< pointer_t >(b.lhs->type) ? b.lhs.get() : b.rhs.get();
    }

    const auto named = named_offset(*p, defined);
    if (!named)
        return false;

    const auto &object = *named->object;

    return !is< name_t >(object) || global_of(object);
}

// The place of an assignment, held across its right operand: the
// lvalue's address, formed outside the registers an instruction within
// the right operand pins, moved_within()'s and the caller's pinned, so
// that a division or a shift by a variable count moves nothing out of
// them, and held_across the right operand where it spills every held
// value, spills_within()'s, a call or a branch within it, so that an
// unscaled sum keeps to one register and a label beside an index stays
// the place's label, with no register (backend.md 6.3): each consumer,
// assign(), modify()'s rows and copy_record(), forms the place by
// formed() after the right operand.  There a base and an index that
// would each be spilled are summed into one register first, the addq of
// a sum at scale 1 and a leaq at any other scale, the displacement
// apart: ((int (*)[2]) (c + 400))[i][1] = f() is leaq (%rax,%rcx,8),
// %rax before the call, one spill and one reload, and movl %eax,
// 404(%rcx) after it.  Not where either is a register variable's read,
// which no call spills, or is spilled already, across a call of the
// index, p[id(i)] = id(v), whose slot holds it across the next; nor on
// a label's leaq, label_rooted()'s; and not where nothing is spilled, a
// division's or a shift's registers chosen outside the address.
address_t
place_of(state_t &s, const assign_t &a, unsigned pinned)
{
    const bool spills = spills_within(*a.rhs);
    const bool outer = std::exchange(s.held_across, spills);
    const unsigned reserved =
        std::exchange(s.reserved, s.reserved | pinned | moved_within(*a.rhs));
    s.defer_label = spills;
    auto place = address(s, *a.lhs);
    s.defer_label = false;
    s.held_across = outer;

    const bool apart = place.base.id >= 0 && place.index.id >= 0 &&
                       !borrowed(s, place.base) && !borrowed(s, place.index) &&
                       s.values[place.base.id].reg >= 0 &&
                       s.values[place.index.id].reg >= 0;
    if (apart && spills_within(*a.rhs) && !label_rooted(*a.lhs, s.defined)) {
        const auto displacement = std::exchange(place.displacement, 0);
        place.sum = place.scale == 1;
        place = { .base = address_of(s, place), .displacement = displacement };
    }

    s.reserved = reserved;

    return place;
}

// A shift's variable count, generated with rax and avoid reserved, and
// with rcx taken out of what a node around the shift reserves for its
// own operand: the instruction consumes the count in cl, so rcx is the
// count's (chapter 6.5), and the register the shift names, as a return
// names rax (6.1).  The count's row takes the name where rcx is free,
// so that the extension of a call's narrow result is movsbl %al, %ecx.
value_t
shift_count(state_t &s, const expr_t &rhs, unsigned avoid)
{
    const unsigned outer = std::exchange(s.reserved, s.reserved & ~bit(rcx));
    const auto v = rvalue_avoiding(s, rhs, avoid | bit(rax), nullptr, bit(rcx));
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
// A statement expression whose statements leave the values held beside
// it where they are is its statements and then its value as the
// operand, operand_of()'s (6.7).
// avoid is reserved for the operand or its address, held_out()'s.  The
// rule is the Seventh Edition compiler's: the row %n,aw of its table's
// cr40 writes the left operand into the register and the instruction
// with the right operand's address.
right_t
binary_operand(state_t &s,
               binary_op_t op,
               const expr_t &rhs,
               int width,
               unsigned avoid = 0)
{
    const auto &e = operand_of(s, rhs);
    const auto *object = is_shift(op) ? nullptr : memory_operand(e, width);
    if (!object)
        return right_operand(s, op, e, width, avoid);

    return { .place = address_avoiding(s, *object, pins(op, e) | avoid),
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
// the variable, and so is one whose consumer names rax, wanted, where
// L is another register (6.1).
value_t
divide(state_t &s,
       binary_op_t op,
       value_t l,
       const right_t &r,
       const int_t &t,
       unsigned wanted = 0)
{
    const int width = t.width;

    const reg_t left = r.place ? pin_division(s, op, l, *r.place, t) :
                                 pin_division(s, op, l, r.value, t);
    const bool stays = left != rax && wanted_register(s, wanted) == rax;

    if (op == binary_op_t::rem || borrowed(s, l) || stays) {
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

// The operand by which an instruction reads the held value v at width:
// its register, or for a spilled value its slot, read in place at the
// instruction's width, -8(%rbp), and not reloaded (6.2).
std::string
source_of(state_t &s, value_t v, int width)
{
    const auto &h = s.values[v.id];
    if (h.reg < 0 && !h.variable)
        return at(h.spill);

    return reg(reg_of(s, v), width);
}

// Whether the row of op on the held values l and r writes r's register,
// backend.md 6.1: an operator that commutes, +, *, &, | or ^, whose
// right operand is an owned value in a register when the instruction is
// written, so that the instruction on the source's two values names
// that register its destination and L its source.  The register is the
// one the consumer names, wanted, addl %ecx, %eax for return g +
// bump(), where L's register would be written and moved; or L is a
// register variable's read, which the row would copy, addl %ebx, %eax
// for s = r + f(x), or a spilled value, which it would reload, where no
// free register is named for the copy or the reload and R's register
// is left for last by no pinned instruction ahead.  A spilled L is read
// in its slot, the instruction's source, addl -8(%rbp), %eax (6.2).
// Not where L is the variable a compound assignment writes in place,
// nor where R is a register variable's read, whose register is the
// variable.  copied says that L would be copied or loaded, right_written()
// the rest, for L an object in memory as well (assign()).
bool
right_written(
    const state_t &s, binary_op_t op, value_t r, unsigned wanted, bool copied)
{
    if (!commutes(op) || borrowed(s, r))
        return false;

    const int reg = s.values[r.id].reg;
    if (reg < 0)
        return false;

    const unsigned own = bit(static_cast< reg_t >(reg));
    if (wanted & own)
        return true;

    return copied && !wanted_register(s, wanted) &&
           !((s.reserved | s.ahead) & own);
}

bool
written_in_right(
    const state_t &s, binary_op_t op, value_t l, value_t r, unsigned wanted)
{
    if (l.id == s.in_place)
        return false;

    const bool copied = borrowed(s, l) || s.values[l.id].reg < 0;

    return right_written(s, op, r, wanted, copied);
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
// and, or and xor set ZF from L, and imul does not (7.1).  wanted is
// the register the result's consumer names, which a quotient stays in
// (divide()), and which an operator that commutes writes where R is
// in it; such an operator writes R's register too where L is a register
// variable's read or a spilled value and no free register is named,
// written_in_right()'s, a spilled L read in its slot.
value_t
arithmetic(state_t &s,
           binary_op_t op,
           value_t l,
           const right_t &r,
           const type_t &t,
           unsigned wanted = 0)
{
    const int width = operation_width(t);

    if (op == binary_op_t::div || op == binary_op_t::rem)
        return divide(s, op, l, r, integer(t), wanted);

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

    // return g + bump(): addl -8(%rbp), %eax, g's value read in its slot
    // and the call's result written; s = r + f(x) on a register r: addl
    // %ebx, %eax, r not copied
    if (written_in_right(s, op, l, r.value, wanted)) {
        const reg_t right = reg_of(s, r.value);
        insn(s, mnemonic, { source_of(s, l, width), reg(right, width) });
        if (op != binary_op_t::mul)
            flagged(s, width);
        wrote(s, r.value, width, integer(t).is_signed);

        release(s, l);

        return r.value;
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
// takes the object as its source.  A compound assignment's n into an
// object in memory is held in any case, since it is evaluated before
// the object is read (6.6); a register pointer's own step takes it,
// p += n on a register char *p being addq -24(%rbp), %rbx.
// avoid is reserved for n or its address, held_out()'s.
right_t
addend(
    state_t &s, binary_op_t op, const expr_t &n, long size, unsigned avoid = 0)
{
    if (size == 1)
        if (const auto *object = memory_operand(n, 64))
            return { .place = address_avoiding(s, *object, avoid),
                     .width = 64 };

    return displacement(s, op, n, size, avoid);
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

// Whether the sum of an unscaled_sum() or a byte_sum() as a value, the
// pointer's address a and the index's product n, is unscaled_value()'s
// addq with a spilled pointer its source, read in its slot (6.2): the
// base alone, spilled, and the row written_in_right()'s, the consumer
// naming wanted, so that return p + f() on a char *p is addq
// -16(%rbp), %rax and no reload.
bool
summed_in_slot(const state_t &s, const address_t &a, value_t n, unsigned wanted)
{
    if (a.base.id < 0 || a.displacement != 0)
        return false;

    const auto &h = s.values[a.base.id];
    if (h.reg >= 0 || h.variable)
        return false;

    return written_in_right(s, binary_op_t::add, a.base, n, wanted);
}

// The index of an unscaled_sum() joined to the pointer's address a,
// backend.md 6.3: n's product, the imulq the subscript asks for, at
// scale 1, so that no addq forms the sum.  n is addend()'s, its product
// already where multiplied() wrote it.  A base and an index spilled
// across n are reloaded first, as offset() reloads them, but a base
// the sum reads in its slot, in_slot.
address_t
unscaled_index(
    state_t &s, address_t a, const right_t &n, long size, bool in_slot = false)
{
    if (a.base.id >= 0 && !in_slot)
        operands(s, a.base, n.value);

    if (n.scale != 1) {
        const reg_t index = writable(s, n.value, 64, registers_of(s, a));
        insn(s, "imulq", { imm(size), reg(index, 64), reg(index, 64) });
    }

    a.index = n.value;
    a.scale = 1;
    a.sum = true;

    return a;
}

// Whether n + p is a sum at a size no addressing form scales by, size,
// the index written first, whose product the addressing form takes as
// its index at scale 1, backend.md 6.3: an add, n no constant, and p
// no object in memory of 64 bits, which index_first()'s addq takes as
// its operand, addq -8(%rbp), %rax, as many instructions as its load.
bool
unscaled_first(const binary_t &b, long size)
{
    return !is_scale(size) && b.op == binary_op_t::add &&
           !is< pointer_t >(b.lhs->type) && !is< literal_t >(*b.lhs) &&
           !memory_operand(*b.rhs, 64);
}

// The address of an unscaled_first() sum as an access's operand,
// backend.md 6.3: n's product first, in the order written (G13), held
// outside the registers p's top operator pins, then p's address,
// unscaled_pointer()'s, which the product joins at scale 1, so that
// i[(long (*)[2]) (p + 8)][1] is 16(%rcx,%rax,1) after imulq $16, %rax,
// %rax and i[m][1] on a local long m[4][2] the slot plus 8,
// -56(%rbp,%rax,1).
address_t
index_unscaled(state_t &s, const binary_t &b, long size)
{
    const auto n =
        displacement(s, binary_op_t::add, *b.lhs, size, pinned_by(*b.rhs));
    const auto a = unscaled_pointer(s, *b.rhs, 0, 0);

    return unscaled_index(s, a, n, size);
}

unsigned copied_into(const state_t &, value_t, const expr_t &, unsigned);

// The copy preserve() makes of a borrowed read held across a right
// operand that writes its variable, as binary() hands it (backend.md
// 6.1): the read and the register named for its copy, copied_into()'s,
// set for the right operand's generation; the state's own before, for
// the restore after it.
struct named_copy_t
{
    int read;
    unsigned copy;
};

named_copy_t
hand_copy(state_t &s, value_t l, unsigned copied)
{
    const named_copy_t outer { s.named_read, s.named_copy };
    if (copied) {
        s.named_read = l.id;
        s.named_copy = copied;
    }

    return outer;
}

void
restore_copy(state_t &s, const named_copy_t &outer)
{
    s.named_read = outer.read;
    s.named_copy = outer.copy;
}

// The address of an unscaled_sum() as an access's operand, backend.md
// 6.3: the pointer's address, unscaled_pointer()'s, then the index, in
// the order written (G13), its product the form's index at scale 1,
// 36(%rax,%rcx) for p[1][i][0] on int (*p)[3][3] and -23(%rbp,%rcx)
// for m[i][4] on a local char m[4][5], where the sum was the addq of
// the p + n row.  As a value it is address_of()'s leaq, leaq
// -48(%rbp,%rcx), %rax for m + i on a local int m[4][3], but that a
// pointer that is its base alone keeps the row's addq.  named is the
// register a value's consumer names, handed to the pointer and held out
// of the index as held_out() says.  An address held_across a right
// operand that spills keeps to one register, spilled once: the
// addq of the p + n row forms the sum, and the pointer's displacement
// is the access's, 36(%rax) for p[1][i][0].  A right operand that
// writes the register variable the pointer reads has the read's copy
// in the named register, copied_into()'s, which the addq writes, return
// p + (i + (p = t, 0)) on a register char *p being addq %rcx, %rax.
// The sum as a value whose consumer names wanted leaves a spilled
// pointer in its slot where unscaled_value()'s addq reads it there,
// summed_in_slot()'s.
address_t
unscaled(state_t &s,
         const operation_t &o,
         long size,
         unsigned named,
         std::optional< unsigned > wanted)
{
    const auto a = unscaled_pointer(s, *o.lhs, pinned_by(*o.rhs), named);

    const unsigned copied = copied_into(s, a.base, *o.rhs, named);
    const auto outer = hand_copy(s, a.base, copied);
    const auto n = addend(s, o.op, *o.rhs, size,
                          held_out(s, a.base, *o.rhs, named) | copied);
    restore_copy(s, outer);

    if (s.held_across && a.label.empty())
        return { .base = displaced(s, o.op, a.base, n),
                 .displacement = a.displacement };

    return unscaled_index(s, a, n, size,
                          wanted && summed_in_slot(s, a, n.value, *wanted));
}

// An unscaled_sum() or a byte_sum() as a value, backend.md 6.3:
// address_of()'s leaq, or the p + n row's addq into the pointer, but
// that where an operation that commutes writes its right operand's
// register (written_in_right(), 6.1), the addq writes the register of
// the index's product, an owned value, the pointer its source: return
// p + f() on a char *p is addq -16(%rbp), %rax, the pointer spilled
// across the call read in its slot (6.2), where the sum was written
// into rcx after p's reload and moved, s = p + f() on a char *p and a
// local s the same addq, where no free register is named for the
// reload, and s = p + f() on a register char *p is addq %rbx, %rax, p
// not copied.  Not where a register variable is handed down, whose
// register the leaq writes.
value_t
unscaled_value(state_t &s, const address_t &a, into_t *into, unsigned wanted)
{
    if (into || !a.sum || a.displacement != 0 || a.base.id < 0 ||
        !written_in_right(s, binary_op_t::add, a.base, a.index, wanted))
        return address_of(s, a, into);

    const reg_t index = reg_of(s, a.index);
    insn(s, "addq", { source_of(s, a.base, 64), reg(index, 64) });
    wrote(s, a.index, 64, false);

    release(s, a.base);

    return a.index;
}

// n + p with p an object in memory of 64 bits, where the add takes p
// unscaled: at a size of 1, and at a size no addressing form scales by,
// after the index is multiplied in its register, imulq $12, %rax, %rax.
// p is the add's own operand, addq -16(%rbp), %rax (backend.md 6.1),
// its address generated where its load would be, after n, so the reads
// keep their order (G13).  None at a size of 2, 4 or 8, whose leaq
// takes p as its base, a register.  avoid is reserved for p's address,
// held_out()'s.
std::optional< value_t >
index_first(
    state_t &s, value_t n, const expr_t &p, long size, unsigned avoid = 0)
{
    const auto *object = memory_operand(p, 64);
    if (!object || (size != 1 && is_scale(size)))
        return std::nullopt;

    const auto place = address_avoiding(s, *object, avoid);

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
// multiplied() already.  avoid is reserved for the right operand,
// held_out()'s.
value_t
pointer_operation(state_t &s,
                  const operation_t &b,
                  long size,
                  value_t l,
                  into_t *into = nullptr,
                  unsigned avoid = 0)
{
    if (is< pointer_t >(b.lhs->type))
        return displaced(s, b.op, l, addend(s, b.op, *b.rhs, size, avoid),
                         into);

    if (const auto sum = index_first(s, l, *b.rhs, size, avoid))
        return *sum;

    return offset(s, b.op, l, rvalue_avoiding(s, *b.rhs, avoid), size, false,
                  into);
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
// is free, and the quotient stays there.  At a size of 1, p is handed
// the register the consumer names, wanted, and q is taken outside it
// where held_out() says, as an operation's right operand is (6.1).  At
// any other size the quotient stays in rax where wanted names it,
// divide()'s, though p was not handed it for a call within q, return p
// - h(q) ending in the idivq.  Where q writes the register variable p
// reads, p's copy takes the register its difference is wanted in,
// copied_into()'s: at a size of 1 the one the consumer names, return p
// - (q + (p = t, 0)) being subq %rcx, %rax, and at any other the
// dividend's rax, so that the idivq follows the subq with no mov; not
// for a call or a division within q, which pins rax itself.
value_t
difference(state_t &s, const binary_t &b, unsigned wanted = 0)
{
    const long size = stride(b.lhs->type);
    const unsigned pinned = size != 1 ? bit(rax) | bit(rdx) : 0;
    const unsigned named = size == 1 ? wanted_left(wanted, 0, *b.rhs) : 0;
    const unsigned dividend =
        size != 1 && !pinned_within(*b.rhs) ? bit(rax) : named;

    const auto l =
        rvalue_avoiding(s, *b.lhs, pinned_by(*b.rhs), nullptr, named);

    const unsigned copied = copied_into(s, l, *b.rhs, dividend);
    const auto outer = hand_copy(s, l, copied);
    subtract_pointer(s, l, *b.rhs,
                     pinned | held_out(s, l, *b.rhs, named) | copied);
    restore_copy(s, outer);

    if (size == 1)
        return l;

    const unsigned reserved = std::exchange(s.reserved, s.reserved | pinned);
    const auto d = fresh(s, 64);
    s.reserved = reserved;

    constant(s, reg_of(s, d), size, 64);

    return divide(s, binary_op_t::div, l, { .value = d, .width = 64 },
                  int_t { 64, true }, wanted);
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
// before either writes, the variable among them (chapter 4).  The
// register the consumer names, wanted, takes them where the cmp leaves
// it free and L is elsewhere, setl %al after cmpl %eax, %ecx (6.1).
value_t
comparison(state_t &s,
           const operation_t &b,
           into_t *into = nullptr,
           unsigned wanted = 0)
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

    const auto named = wanted_register(s, wanted);
    if (l.id < 0 || borrowed(s, l) || named) {
        release(s, l);
        l = named ? hold(s, *named, 32) : fresh(s, 32);
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
// operands do.  Handed a register variable, each path writes the
// variable's register, movl $1, %ebx and movl $0, %ebx, after the jump
// code has read every operand, and the value is the variable's read
// (chapter 4); the assignment's preserve() has copied every read of
// the variable before the first divergence.
value_t
logical(state_t &s, const expr_t &e, into_t *into = nullptr)
{
    const auto incoming = before_paths(s, e);

    const int no = new_label(s);
    const int end = new_label(s);

    cbranch(s, e, no, false);

    if (takes(into, truth_valid)) {
        const reg_t r = variable_register(into->variable);

        constant(s, r, 1, 32);
        jump(s, end);

        place_label(s, no);
        constant(s, r, 0, 32);

        place_label(s, end);

        return taken(s, *into);
    }

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
// GNU C's a ?: b has no first arm, and its condition jumps past the
// second when it holds: f() ?: g(); is call f@PLT, cmpl $0, %eax, jne
// and call g@PLT.
void
conditional_effect(state_t &s, const expr_t &e)
{
    const auto &c = as< conditional_t >(e);

    const auto incoming = before_paths(s, e);

    if (!c.yes) {
        const int end = new_label(s);

        cbranch(s, *c.cond, end, true);
        effect(s, *c.no);

        place_label(s, end);
        return;
    }

    const int no = new_label(s);
    const int end = new_label(s);

    cbranch(s, *c.cond, no, false);

    effect(s, *c.yes);
    jump(s, end);

    place_label(s, no);
    effect(s, *c.no);

    place_label(s, end);
}

// An arm of ?: generated for the join register j, backend.md 6.1:
// handed j as the register its consumer names, so that its last
// instruction writes j where it can, and, where no instruction within
// it pins a register, with the registers below j left for last, so
// that j is its lowest free register and a row that writes a fresh one
// writes j, movl -12(%rbp), %edx beside a first arm's remainder.  A
// pin within the arm decides its registers, as in any operand (6.5).
// Under a conversion that widens the conditional, to, the arm is
// converted()'s, its extension written into j, movslq %edx, %rax.
value_t
arm(state_t &s, const expr_t &e, reg_t j, const type_t *to = nullptr)
{
    unsigned below = 0;
    for (const reg_t r : expression_registers) {
        if (r == j)
            break;
        below |= bit(r);
    }

    const unsigned avoid = pinned_within(e) ? 0 : below;
    if (!to)
        return rvalue_avoiding(s, e, avoid, nullptr, bit(j));

    const unsigned reserved = std::exchange(s.reserved, s.reserved | avoid);
    const auto v = converted(s, e, *to, nullptr, bit(j));
    s.reserved = reserved;

    return v;
}

// One path of a branch into a register variable, chapter 4: the arm's
// value written into the variable's register as an assignment's right
// operand is, computed_into()'s, by the arm's row where it takes the
// variable and otherwise by establish()'s mov after it, movl
// -12(%rbp), %ebx or addl $1, %ebx for an arm r + 1, which reads the
// variable before the path writes it.  Under a conversion that widens
// the conditional, to, the arm is converted()'s, movslq -16(%rbp), %rbx
// for an int in memory.  The value is consumed.
void
arm_into(state_t &s, const expr_t &arm, const into_t &into, const type_t *to)
{
    into_t path { into.variable, false, into.as };
    const auto v = to ? converted(s, arm, *to, &path) : rvalue(s, arm, &path);
    if (!path.taken)
        move_in(s, into.variable, v);

    release(s, v);
}

// Whether the row of an arm of ?: handed a register variable writes the
// variable's register itself, its last instruction the one that writes
// it, chapter 4: a constant, its mov; an object in memory, its load; a
// truth value, the setcc and the movzbl or the movl $1 and the movl $0;
// and, the arm unconverted, the value of a step of an object in memory
// whose address does not read the variable, its load.  Under a
// conversion that widens the arm, widened, the object may be under
// casts that keep its bits, and its load is the extending one (6.3).
bool
lands_in(const expr_t &arm, const symbol_t *variable, bool widened)
{
    if (is< literal_t >(arm))
        return true;

    if (widened ? widened_read(arm).object != nullptr : in_memory(arm))
        return true;

    if (is< binary_t >(arm)) {
        const auto op = as< binary_t >(arm).op;
        return is_comparison(op) || op == binary_op_t::log_and ||
               op == binary_op_t::log_or;
    }

    if (!is< unary_t >(arm))
        return false;

    const auto &u = as< unary_t >(arm);
    if (u.op == unary_op_t::log_not)
        return true;

    return !widened && is_step(u.op) && in_memory(*u.arg) &&
           !names(*u.arg, variable);
}

// Whether a conditional handed a register variable may have each path
// write the variable's register, chapter 4: one of the variable's own
// type, or of the type into's row computes in, computed_type(), so that
// each arm is the right operand of an assignment to it; for a pointer
// variable, one of any pointer type, whose conversion to the variable's
// writes nothing (6.3), q = &(c ? m : n)[1][2] on two pointers to rows
// being movq -24(%rbp), %rbx on each path and leaq 20(%rbx), %rbx.
bool
joins_into(const expr_t &e, const into_t *into)
{
    if (!into || into->taken)
        return false;

    const auto &type = computed_type(*into);

    return e.type == type || (is< pointer_t >(e.type) && is< pointer_t >(type));
}

// The second path of a conditional each of whose paths writes the
// register variable, chapter 4: the jump past it, the second arm into
// the variable, arm_into()'s, converted to to where it is given, and
// the join, after which the value is the variable's read.
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

// GNU C's a ?: b, a ? a : b with a evaluated once (shape.md chapter
// 5): the condition's value is the first arm's, so it is computed into
// the join, compared with 0 there, and taken by a jne past the second
// arm, which is written into the same register, return f() ?: g()
// being call f@PLT, cmpl $0, %eax, jne and call g@PLT, with no slot
// and no second read of a.  The join is the register the consumer
// names, wanted, where it names one, and otherwise the condition's, a
// register variable's read copied into one, as conditional() joins its
// first arm.  The cmp is left out where the condition's own
// instruction set ZF at its width, as flag_branch() leaves it out.
// Handed a register variable that joins_into(), the condition is
// computed into the variable, arm_into()'s, the test reads it there,
// and the second path writes it: r = a ?: b is movl -12(%rbp), %ebx,
// cmpl $0, %ebx, jne and movl -16(%rbp), %ebx, and the value is the
// variable's read.  Not where the second arm reads the variable, which
// would find the condition's value in it; that one joins as for any
// consumer, and the assignment moves the join in (chapter 4).
value_t
omitted_middle(state_t &s, const expr_t &e, into_t *into, unsigned wanted)
{
    const auto &c = as< conditional_t >(e);
    const auto &t = integer(e.type);

    const auto incoming = before_paths(s, e);

    const int end = new_label(s);

    if (joins_into(e, into) && !names(*c.no, into->variable)) {
        arm_into(s, *c.cond, *into, nullptr);
        if (s.zero_flag != t.width)
            insn(s, sized("cmp", t.width),
                 { imm(0), reg(variable_register(into->variable), t.width) });
        jump_if(s, binary_op_t::ne, true, end);

        arm_into(s, *c.no, *into, nullptr);

        place_label(s, end);

        return taken(s, *into);
    }

    const auto named = wanted_register(s, wanted);
    const auto value = named ? arm(s, *c.cond, *named) : rvalue(s, *c.cond);
    if (borrowed(s, value))
        copy(s, value, named ? *named : take(s, 0));

    reg_t join = reg_of(s, value);
    int valid = s.values[value.id].valid;
    if (named && join != *named) {
        mov(s, join, *named, t.width);
        join = *named;
        valid = written(t.width, t.is_signed);
    }
    release(s, value);

    if (s.zero_flag != t.width)
        insn(s, sized("cmp", t.width), { imm(0), reg(join, t.width) });
    jump_if(s, binary_op_t::ne, true, end);

    const auto other = arm(s, *c.no, join);
    const reg_t from = reg_of(s, other);
    int other_valid = s.values[other.id].valid;
    if (from != join) {
        mov(s, from, join, t.width);
        other_valid = written(t.width, t.is_signed);
    }
    release(s, other);

    place_label(s, end);

    // valid to what both arms left
    return hold(s, join, std::min(valid, other_valid));
}

// 6.5.16p5: the arm not chosen is not evaluated, and the jump says so;
// never cmov.  The arms join in one expression register: the one the
// conditional's consumer names, wanted, where it names one, and
// otherwise the first arm's, a register variable as the first arm
// copied into one.  Each arm is generated for the join, arm()'s, and
// moved there where its value is elsewhere: return c ? x % y : y is
// the remainder's movl %edx, %eax and the second arm's movl -12(%rbp),
// %eax, and no move after the join (backend.md 6.1).  Handed a register
// variable that joins_into(), the first arm is computed into it where
// its row can, and then each path writes the variable and no register
// joins them: r = c ? x : y is movl -16(%rbp), %ebx on the first path
// and the second arm's arm_into(), movl -20(%rbp), %ebx, and the value
// is the variable's read.  Where the first arm's row does not write the
// variable, the first path moves its value in, and the second arm's
// row writes it where lands_in() says, r = c ? f(x) : p[i]++ being
// movl %eax, %ebx after the call and the step's movl (%rax,%rcx,4),
// %ebx; where an arm is the variable's own read, which writes nothing,
// the condition jumps past the other path alone, one_path()'s.  Under
// a conversion that widens the conditional, to, which
// cast() hands down where widens_arms(), each arm is converted: into
// the variable on each path, or into the join, return a ? b % c : c as
// a long being movslq %edx, %rax and movslq -12(%rbp), %rax, with no
// extension after the join.  An arm reads the variable on its own path
// before the path writes it, and the assignment's preserve() has copied
// every read of it held across the branch.  Otherwise the arms join as
// for any consumer and the assignment moves the join in after it
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

    if (!c.yes)
        return omitted_middle(s, e, into, wanted);

    const auto incoming = before_paths(s, e);

    const int no = new_label(s);
    const int end = new_label(s);

    // r = c ? r : x on a register r: jne past the second path alone
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

    into_t path { joins ? into->variable : nullptr, false,
                  joins ? into->as : nullptr };
    const auto named = wanted_register(s, wanted);
    const auto yes = joins ? rvalue(s, *c.yes, &path) :
                     named ? arm(s, *c.yes, *named, to) :
                     to    ? converted(s, *c.yes, *to) :
                             rvalue(s, *c.yes);

    if (path.taken || (joins && lands_in(*c.no, into->variable, false))) {
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
    const auto other = arm(s, *c.no, join, to);
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

// Whether a pointer reaches its object by one operand that evaluates
// nothing but, it may be, the load of one named pointer: the pointer a
// name, *p; a named object's address plus a constant, a[3] on an array
// and m[1][2], whose operand is the slot or the label with the offset
// folded in; or a named pointer, or such an address's dereference, plus
// a constant, p[3] and p[1][2], whose operand is the pointer's register
// with the constant as its displacement, 12(%rax), as *p's is (%rax).
// The pointer and the named pointer of a sum are seen through casts
// that keep their bits, which write nothing: ((int *) q)[0] is *q's
// operand, and ((int *) q)[3] is q[3]'s.
bool
named_pointer(const expr_t &e)
{
    const expr_t &p = under_kept_bits(e);
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
           is< name_t >(under_kept_bits(pointer));
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

// Whether a cast as L of the operation form goes into the variable's
// register by its own row: an array's decay, the leaq or the GOT's load
// of a named array or a string literal; a conversion that changes the
// signedness alone of an object in memory, retyped()'s load at the
// conversion's extension; a conversion of an object in memory, under
// casts that keep its bits, whose width holds the representation, its
// load, which a row computing the value in another signedness, into's
// as, takes through a conversion that does not narrow, as onward()
// hands it on; and a widening of a
// register variable's read that is not valid to the new width, its
// extension, movslq %ebx, %r12 for r = i + 5L into a register long
// from a register int i.
bool
cast_loads_into(const cast_t &c, const type_t &to, const into_t &into)
{
    const auto &arg = *c.arg;

    if (is< array_t >(arg.type))
        return (is< name_t >(arg) && !register_variable(arg)) ||
               is< string_t >(arg);

    if (retypes(c, to, &into))
        return object_under(c) != nullptr;

    if (!is< int_t >(arg.type) && !is< pointer_t >(arg.type))
        return false;

    const int from = integer(arg.type).width;
    const int width = integer(to).width;

    if (width > from && register_read(arg).variable)
        return width >= needed(into);

    return (!into.as || width >= from) && object_under(c) &&
           width >= needed(into);
}

bool offsets_in(const operation_t &, const type_t &, const into_t &);
operation_t ordered_sum(const binary_t &, long size);

// Whether L of the operation form goes into the variable's register by
// an instruction it has in any case: its load, its constant, or again
// the operation form, each written into the register, pointer
// arithmetic's among them, flag + i + i; the address of a name, its
// leaq or the GOT's load, and a cast that cast_loads_into(); the read
// of another register variable, under conversions that write nothing,
// register_read()'s, (int) c on a register char among them, which an
// instruction that writes is given a copy of (chapter 4), copied there;
// the variable itself, already there.  Any other L leaves the whole
// assignment to the mov of establish() after it: moved in before the
// instruction, L would leave the assignment's value in the variable's
// register, where the mov after leaves it in an expression register,
// and a use of the value in one would copy it back, an instruction
// more.  A sum whose integer is a constant is no L here, since folded()
// and summed() may form it with no instruction of its own.
bool
loads_into(const expr_t &lhs, const into_t &into)
{
    if (is< literal_t >(lhs) || is< name_t >(lhs) || in_memory(lhs))
        return true;

    if (is< unary_t >(lhs))
        return as< unary_t >(lhs).op == unary_op_t::addr &&
               is< name_t >(*as< unary_t >(lhs).arg);

    if (is< cast_t >(lhs))
        return register_read(lhs).variable ||
               cast_loads_into(as< cast_t >(lhs), lhs.type, into);

    if (!is< binary_t >(lhs))
        return false;

    const auto &b = as< binary_t >(lhs);

    if (is< pointer_t >(lhs.type)) {
        const auto &n = is< pointer_t >(b.lhs->type) ? *b.rhs : *b.lhs;
        return !is< literal_t >(n) &&
               offsets_in(ordered_sum(b, stride(lhs.type)), lhs.type, into);
    }

    return operates_in(ordered(b), lhs.type, into);
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
    if (!two_address(b.op))
        return false;

    if (is< pointer_t >(t) || is< pointer_t >(b.lhs->type) ||
        !(t == computed_type(into)))
        return false;

    return leaf_apart_from(*b.rhs, into.variable) && loads_into(*b.lhs, into);
}

// Whether P is a local array's slot plus a constant, the base of the
// addressing form with rbp and no instruction (backend.md 6.3): a name's
// address plus a constant whose root is a local's or a parameter's
// name, which named_offset() reaches with no global defined.
bool
slot_rooted(const expr_t &p)
{
    static const std::unordered_set< const symbol_t * > none;

    const auto named = named_offset(p, none);

    return named && is< name_t >(*named->object);
}

// Whether r = P + N, N + P or P - N, of pointer type t, is the operation
// on the variable's register, chapter 4: a row whose last instruction is
// the addq or the subq on L, so not offset()'s leaq, which a register
// variable handed down is the destination of; on the variable's own
// type; L that loads_into() the register; and the two conditions on R.
// The index is multiplied by a size no addressing form scales by in its
// own register: R's, imulq $12, %rcx, %rcx, or the variable's for the
// index of N + P.  Not a local array's row by a variable index at such
// a size, whose product joins the slot in the one leaq, unscaled()'s,
// leaq -60(%rbp,%rax,1), %rbx for q = m[i] (6.3).
bool
offsets_in(const operation_t &b, const type_t &t, const into_t &into)
{
    const auto &v = computed_type(into);
    if (scaled_sum(b, stride(t)) ||
        (unscaled_sum(b, stride(t)) && slot_rooted(*b.lhs)) ||
        !(t == v || (is< pointer_t >(t) && is< pointer_t >(v))))
        return false;

    return leaf_apart_from(*b.rhs, into.variable) && loads_into(*b.lhs, into);
}

// L of the operation form into the variable's register, by the
// instruction loads_into() names: a register variable's read copied,
// movl %r12d, %ebx, under conversions that write nothing too, movl
// %ebx, %r12d for (int) c on a register char, the variable itself
// nothing, and anything else by its own row, handed the variable as the
// operation computes it.  The value is the variable's read.
value_t
left_in(state_t &s, const expr_t &lhs, const into_t &to, unsigned avoid)
{
    const auto *sym = to.variable;

    if (const auto *other = register_read(lhs).variable) {
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

// p + n and n + p at a size of 2, 4 or 8, scaled_sum()'s, as one leaq
// of the addressing form, backend.md 6.3: the pointer the address the
// index joins, index_base()'s, so that p[1] + i on int (*p)[3] is
// leaq 12(%rax,%rcx,4), %rax and no instruction forms the row's
// address, and any other pointer the base, leaq (%rax,%rcx,4), %rax.
// The operands in the order of o, as pointer_arithmetic() generates
// them: the one written first outside pinned and in the register the
// consumer names, named, the other outside it where held_out() says.
// A register variable handed down is the destination of the leaq
// (chapter 4).
value_t
joined_sum(state_t &s,
           const operation_t &o,
           long size,
           into_t *into,
           unsigned pinned,
           unsigned named)
{
    if (is< pointer_t >(o.lhs->type)) {
        auto a = index_base(s, *o.lhs, pinned, named);
        a.index =
            rvalue_avoiding(s, *o.rhs, held_out(s, a.base, *o.rhs, named));
        a.scale = size;

        return address_of(s, a, into);
    }

    const auto index = rvalue_avoiding(s, *o.lhs, pinned, nullptr, named);

    auto a = index_base(s, *o.rhs, held_out(s, index, *o.rhs, named));
    a.index = index;
    a.scale = size;

    return address_of(s, a, into);
}

// Whether p + n at a size of 1 takes its index into the addressing
// form, backend.md 6.3: an add, the pointer written first, and n held
// in a register, no constant and no object of 64 bits in memory, which
// the p + n row's addq takes as its operand, addq -16(%rbp), %rax.
// The index then joins a pointer that is a base and a displacement,
// leaq 8(%rax,%rcx,1), %rax for p[1] + i on char (*p)[8].
bool
byte_sum(const operation_t &o, long size)
{
    return size == 1 && o.op == binary_op_t::add &&
           is< pointer_t >(o.lhs->type) && !is< literal_t >(*o.rhs) &&
           !memory_operand(*o.rhs, 64);
}

// A binary + or - of a pointer type: the pointer and the integer, the
// integer widened to 64 bits by the parser, in the order written
// (G13).  A name's address plus a constant is the leaq of folded()'s
// one operand, leaq -20(%rbp), %rax for a + 3, 6.3; any other
// lvalue's address plus a constant the leaq of summed()'s,
// leaq 2(%rax,%rcx,8), %rax for m[i] + 2; and a local array plus a
// variable index of a size the form scales the leaq of indexed()'s,
// leaq -24(%rbp,%rcx,4), %rax for a + i.  Any other + whose row is a
// leaq, scaled_sum()'s, is joined_sum()'s, leaq 12(%rax,%rcx,4), %rax
// for p[1] + i on int (*p)[3].  Otherwise the operand written first is held
// outside the registers the other's top operator pins (chapter 6.5),
// and pointer_operation() writes the row, the operands in the order of
// ordered_sum(); n of n + p that the row multiplies is
// multiplied()'s product.  A register variable handed down is the
// destination of each leaq, and of the operation on its register where
// offsets_in() holds (chapter 4).  The register the consumer names,
// wanted, is handed to the operand written first, and the other is
// taken outside it where held_out() says (6.1).
value_t
pointer_arithmetic(state_t &s,
                   const expr_t &e,
                   into_t *into,
                   unsigned wanted = 0)
{
    if (const auto a = folded(s, e))
        return address_of(s, *a, into);

    if (const auto a = summed(s, e, into))
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
    const unsigned named = wanted_left(wanted, 0, *o.rhs);

    // p[1] + i on int (*p)[3]: leaq 12(%rax,%rcx,4), %rax
    if (scaled_sum(o, size))
        return joined_sum(s, o, size, into, pinned, named);

    // m + i on a local int m[4][3]: leaq -48(%rbp,%rcx), %rax; and
    // p[1] + i on char (*p)[8]: leaq 8(%rax,%rcx,1), %rax
    if (unscaled_sum(o, size) || byte_sum(o, size)) {
        const auto value = into ? std::nullopt : std::optional(wanted);
        return unscaled_value(s, unscaled(s, o, size, named, value), into,
                              wanted);
    }

    if (!is< pointer_t >(o.lhs->type) && multiplies(o.op, size))
        if (const auto product = multiplied(s, *o.lhs, size, pinned))
            return pointer_operation(s, o, 1, *product, into,
                                     held_out(s, *product, *o.rhs, named));

    const auto l = rvalue_avoiding(s, *o.lhs, pinned, nullptr, named);

    return pointer_operation(s, o, size, l, into,
                             held_out(s, l, *o.rhs, named));
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

// The three-operand imul of the object in memory at the operand from by
// the immediate factor, in type t, backend.md 6.1 and 6.4: into the
// register variable handed down, into, where its row takes it, else
// into the register the consumer names, wanted, where it is free, else
// into the lowest free register outside avoid.
value_t
object_product(state_t &s,
               std::string_view from,
               std::string_view factor,
               const int_t &t,
               into_t *into,
               unsigned wanted = 0,
               unsigned avoid = 0)
{
    const int width = t.width;
    const int valid = written(width, t.is_signed);
    const auto mnemonic = sized("imul", width);

    if (takes(into, valid)) {
        insn(s, mnemonic,
             { factor, from, reg(variable_register(into->variable), width) });
        return taken(s, *into);
    }

    const auto named = wanted_register(s, wanted);
    const auto v = named ? hold(s, *named, valid) : fresh(s, valid, avoid);
    insn(s, mnemonic, { factor, from, reg(reg_of(s, v), width) });

    return v;
}

// x * c with x an object in memory: the three-operand imul with the
// object as its source, imull $4, -4(%rbp), %eax, one instruction and
// still a multiply (backend.md 6.1, 6.4), object_product()'s.  The
// object is read once, by the imul, where its load would read it, and
// the constant has no evaluation (G13).  As load() does, the product
// takes the lowest free register once the address is consumed, or,
// handed a register variable, the variable's register, imull $4,
// -4(%rbp), %ebx (chapter 4).
value_t
product(state_t &s,
        const expr_t &object,
        const expr_t &constant,
        const int_t &t,
        into_t *into)
{
    const auto place = address(s, object);
    const auto from = operand(s, place);
    release(s, place);

    return object_product(s, from, immediate_of(constant, t.width), t, into);
}

// x * c with x a register variable's read, register_read()'s, c a
// constant the instruction takes as an immediate: the three-operand imul
// with the variable's register as its source, imull $4, %ebx, %ecx, one
// instruction, and the variable read and not copied (backend.md 6.1,
// chapter 4).  The product takes the register a consumer names, wanted,
// where it is free, imull $4, %ebx, %eax for a return's, or else the
// lowest free one, and handed a register variable the variable's
// register, imull $4, %ebx, %r12d.
value_t
register_product(state_t &s,
                 const symbol_t *x,
                 const expr_t &constant,
                 const int_t &t,
                 into_t *into,
                 unsigned wanted)
{
    const int width = t.width;
    const int valid = written(width, t.is_signed);
    const auto mnemonic = sized("imul", width);
    const auto factor = immediate_of(constant, width);
    const auto source = reg(variable_register(x), width);

    if (takes(into, valid)) {
        insn(s, mnemonic,
             { factor, source, reg(variable_register(into->variable), width) });
        return taken(s, *into);
    }

    const auto named = wanted_register(s, wanted);
    const auto v = named ? hold(s, *named, valid) : fresh(s, valid);
    insn(s, mnemonic, { factor, source, reg(reg_of(s, v), width) });

    return v;
}

// The register a node's borrowed left operand l is copied into where
// its right operand rhs writes l's variable, of those its consumer
// names, named (backend.md 6.1): preserve() copies the read inside rhs,
// so the copy is handed the name, and rhs is generated with it left for
// last, so that nothing of rhs takes it before the copy, and the copy,
// a held value, keeps it to the node's instruction.  None for any other
// l, and where named is none, as where an instruction within rhs pins a
// register, a call or a division.
unsigned
copied_into(const state_t &s, value_t l, const expr_t &rhs, unsigned named)
{
    if (!named || l.id < 0)
        return 0;

    const auto *variable = s.values[l.id].variable;

    return variable && writes(rhs, variable) ? named : 0;
}

// A binary operator's row.  The register the consumer of its value
// names, wanted, is handed to the left operand, whose register the
// result is, where the node does not hold it out for its right
// operand's pins; the right operand, or its address, is then taken
// outside it where held_out() says, so that the copy of a borrowed read
// or the reload of a spilled left operand finds it free: return r -
// p[i] is movl %ebx, %eax; subl (%rcx,%rdx,4), %eax (6.1).  A right
// operand that writes the variable of a borrowed left operand is taken
// outside it too, and the read's copy takes it, copied_into()'s.  A
// quotient stays in the register the consumer names, rax, divide()'s,
// though the left operand was not handed it for a call or a division
// within the right one, return x / (y / z) ending in the idivl.
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
        return difference(s, b, wanted);

    // a constant on the left of an operator that commutes is taken on
    // the right, 4 * x as x * 4 (6.1)
    const auto o = ordered(b);
    const int width = operation_width(e.type);

    if (const auto *object = multiplicand(o, width))
        return product(s, *object, *o.rhs, integer(e.type), into);

    // x * 4 on a register variable: imull $4, %ebx, %ecx
    if (o.op == binary_op_t::mul && immediate(*o.rhs, width))
        if (const auto *x = register_read(*o.lhs).variable)
            return register_product(s, x, *o.rhs, integer(e.type), into,
                                    wanted);

    if (into && operates_in(o, e.type, *into))
        return operation_in(s, o, e.type, *into);

    // G13: the left operand first, held while the right one is
    // generated, outside the registers the right one's operator pins and,
    // for a shift by a variable count, outside rcx, where the count goes
    // (chapter 6.5)
    const unsigned own = is_shift(o.op) ? pins(o.op, *o.rhs) : 0;
    const unsigned named = wanted_left(wanted, own, *o.rhs);

    const unsigned ahead =
        std::exchange(s.ahead, s.ahead | moved_within(*o.rhs));
    const auto l =
        rvalue_avoiding(s, *o.lhs, own | pinned_by(*o.rhs), nullptr, named);
    s.ahead = ahead;

    // return x + (c ? x++ : 2) on a register x: x's copy in rax
    const unsigned copied = copied_into(s, l, *o.rhs, named);
    const int outer_read = s.named_read;
    const unsigned outer_copy = s.named_copy;
    if (copied) {
        s.named_read = l.id;
        s.named_copy = copied;
    }

    const auto r = binary_operand(s, o.op, *o.rhs, width,
                                  held_out(s, l, *o.rhs, named) | copied);
    s.named_read = outer_read;
    s.named_copy = outer_copy;

    return arithmetic(s, o.op, l, r, e.type, wanted);
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
    // copy, movl %ebx, %r12d (chapter 4)
    const bool copies =
        post && takes(into, represented(sym)) && into->variable != sym;
    if (copies)
        mov(s, variable_register(sym), variable_register(into->variable),
            width);

    const auto old = post && !copies ? read(s, sym) : value_t { };
    preserve(s, sym);

    const reg_t r = variable_register(sym);
    insn(s, sized(up ? "add" : "sub", width), { imm(amount), reg(r, width) });
    flagged(s, width);
    if (t.width < 32)
        establish(s, sym, r);

    if (copies)
        return taken(s, *into);

    return post ? old : read(s, sym);
}

// ++ and --, either side: the address once, the load, add or sub of 1,
// or of the size for a pointer (6.5.4.1p2, 6.5.3.5p2), the store
// through the same address; G1, the machine wraps.  A char or a short
// is loaded extended to 32 bits and its low bytes stored, so no
// operator sees 8 or 16 bits.  The postfix forms copy the old value to
// another register first and yield it.  Handed a register variable,
// the load writes its register, fetch()'s, for a postfix form the old
// value and for a prefix form, on an object of 32 bits or more, the
// value the add steps in place and the store takes from there, movl
// -12(%rbp), %ebx; addl $1, %ebx; movl %ebx, -12(%rbp) for r = ++x
// (chapter 4); a narrower object's sum is no value of its type.  The
// register the consumer names, wanted, is left out of the address, and
// of the load for a postfix form, so that the value takes it, where no
// instruction within the address pins a register, as wanted_left() has
// it (6.1).
value_t
step(state_t &s, const unary_t &u, into_t *into = nullptr, unsigned wanted = 0)
{
    if (const auto *sym = register_variable(*u.arg))
        return step_register(s, u.op, sym, into);

    const auto &t = integer(u.arg->type);
    const int width = std::max(32, t.width);
    const long amount = is< pointer_t >(u.arg->type) ? stride(u.arg->type) : 1;

    const bool up = u.op == unary_op_t::pre_inc || u.op == unary_op_t::post_inc;
    const bool post =
        u.op == unary_op_t::post_inc || u.op == unary_op_t::post_dec;
    const unsigned named = pinned_within(*u.arg) ? 0 : wanted;

    const auto place = address_avoiding(s, *u.arg, named);
    const bool into_load = post || t.width >= 32;
    const auto l =
        fetch(s, place, t, width, into_load ? into : nullptr, post ? named : 0);
    const reg_t r = reg_of(s, l);

    // r = ++x: the load wrote r's register, which the add steps in place
    // and the store takes the value from (chapter 4)
    if (borrowed(s, l) && !post) {
        insn(s, sized(up ? "add" : "sub", width),
             { imm(amount), reg(r, width) });
        store(s, l, place, t.width);
        release(s, place);

        return l;
    }

    // r = x++: the load wrote r's register, the old value; the new one
    // is a copy of it, stepped and stored (chapter 4)
    if (borrowed(s, l)) {
        const auto n =
            fresh(s, s.values[l.id].valid, bit(r) | registers_of(s, place));
        mov(s, r, reg_of(s, n), width);
        insn(s, sized(up ? "add" : "sub", width),
             { imm(amount), reg(reg_of(s, n), width) });
        store(s, n, place, t.width);
        release(s, place);
        release(s, n);

        return l;
    }

    value_t old;
    if (post) {
        old = fresh(s, s.values[l.id].valid, bit(r) | registers_of(s, place));
        insn(s, sized("mov", width),
             { reg(r, width), reg(reg_of(s, old), width) });
    }

    insn(s, sized(up ? "add" : "sub", width), { imm(amount), reg(r, width) });
    store(s, l, place, t.width);
    release(s, place);
    s.values[l.id].valid = stored_valid(written(width, t.is_signed), width, t);

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

// A unary operator's row.  The register the consumer of its value
// names, wanted, is handed to the operand of - and ~, which work on its
// register in place; ! writes it where the cmp leaves it free, as a
// comparison does; and a step generates its place outside it, so that
// its value takes it (6.1).
value_t
unary(state_t &s,
      const unary_t &u,
      const type_t &t,
      into_t *into = nullptr,
      unsigned wanted = 0)
{
    switch (u.op) {
    case unary_op_t::neg:
    case unary_op_t::bit_not: {
        if (into && negates_in(u, t, *into))
            return negated_in(s, u, t, *into);

        return negated(s, u.op, rvalue(s, *u.arg, nullptr, wanted), t);
    }

    // as a value; the operand is not promoted, so it is compared at its
    // own width.  A register variable is read by the cmp, and the sete
    // takes a fresh register, as for a comparison; the value is valid
    // to 64 bits, truth_valid.  Handed a register variable, the sete and
    // the movzbl write its register, sete %bl; movzbl %bl, %ebx, after
    // the cmp has read the operand (chapter 4).
    case unary_op_t::log_not: {
        const int width = integer(u.arg->type).width;
        auto v = rvalue(s, *u.arg);
        insn(s, sized("cmp", width), { imm(0), reg(reg_of(s, v), width) });

        if (takes(into, truth_valid)) {
            release(s, v);

            const reg_t r = variable_register(into->variable);
            insn(s, "sete", { reg(r, 8) });
            insn(s, "movzbl", { reg(r, 8), reg(r, 32) });

            return taken(s, *into);
        }

        const auto named = wanted_register(s, wanted);
        if (borrowed(s, v) || named) {
            release(s, v);
            v = named ? hold(s, *named, 32) : fresh(s, 32);
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
    case unary_op_t::post_dec: return step(s, u, into, wanted);

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
        move_in(s, sym, v);

    return v;
}

// Assignment to a register variable, after preserve() has copied every
// read of it an enclosing expression still holds, x + (x += e), before
// the first write of the variable, which in the operation form is the
// load of L.  Plain: a constant is the immediate into the variable's
// register and the value is a read of the variable; otherwise the right
// operand into the register by computed_into().  Compound: the right
// operand first, binary_operand()'s, an object in memory of the
// operation's width the instruction's own operand, andq -24(%rbp), %rbx
// (6.1), then the operation row with the variable's register as L, in
// place, addl %eax, %ebx, but for a division, whose quotient comes
// back from rax by the mov; p += n at a size of 2, 4 or 8, whose leaq
// is handed the variable and writes its register, leaq (%rbx,%rax,4),
// %rbx, and at a size of 1 n of 64 bits in memory addend()'s operand,
// addq -24(%rbp), %rbx (chapter 4); the value is a read of the variable
// after the write.  The variable is read by the operation, after the right
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

    const auto r = stepped ? addend(s, *a.op, *a.rhs, stride(sym->type)) :
                             binary_operand(s, *a.op, *a.rhs, width);
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
// step the pointer by n times the size.  The register the consumer of
// the value names, wanted, is left out of the place too, so that the
// value takes it, a plain assignment's right operand, handed it, or a
// compound one's load, where no instruction within the place or the
// right operand pins a register, as wanted_left() has it (6.1); a
// compound division's quotient stays in rax where the consumer names
// it, divide()'s, and is stored from there, return x /= f(y).
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

    const unsigned named =
        pinned_within(*a.lhs) || pinned_within(*a.rhs) ? 0 : wanted;
    const auto place = place_of(s, a, pinned_by(a) | named);

    if (!a.op) {
        const auto v = rvalue(s, *a.rhs, nullptr, wanted);
        const reg_t r = reg_of(s, v, registers_of(s, place));
        const auto at = formed(s, place, bit(r));
        store(s, v, at, t.width);
        release(s, at);
        return v;
    }

    const int width = operation_width(a.common);
    const auto r = stepped ? displacement(s, *a.op, *a.rhs, stride(a.lhs->type),
                                          bit(rax)) :
                             right_operand(s, *a.op, *a.rhs, width, bit(rax));

    // a += f(b) on a long a: addq -8(%rbp), %rax, the object the row's
    // source, and the store
    if (!stepped && t.width == width && r.immediate.empty() && !r.place &&
        right_written(s, *a.op, r.value, wanted, true)) {
        const reg_t right = reg_of(s, r.value);
        const auto at = formed(s, place, bit(right));
        insn(s, sized(arithmetic_mnemonic(*a.op), width),
             { operand(s, at, bit(right)), reg(right, width) });
        if (*a.op != binary_op_t::mul)
            flagged(s, width);
        wrote(s, r.value, width, t.is_signed);

        store(s, r.value, at, t.width);
        release(s, at);
        auto &valid = s.values[r.value.id].valid;
        valid = stored_valid(valid, width, t);

        return r.value;
    }

    // x *= 12 on an int in memory: imull $12, -4(%rbp), %eax, the object
    // the imul's source, and the store
    if (!stepped && *a.op == binary_op_t::mul && !r.immediate.empty() &&
        t.width == width) {
        const auto at = formed(s, place);
        const auto v =
            object_product(s, operand(s, at), r.immediate, integer(a.common),
                           nullptr, wanted, registers_of(s, at));
        store(s, v, at, t.width);
        release(s, at);
        auto &valid = s.values[v.id].valid;
        valid = stored_valid(valid, width, t);

        return v;
    }

    // the value of the expression has the lvalue's type: the bits
    // above its width are the operation's, not the value's extension
    const auto at = formed(s, place);
    const auto l = fetch(s, at, t, width);
    const auto v = stepped ? displaced(s, *a.op, l, r) :
                             arithmetic(s, *a.op, l, r, a.common, wanted);
    store(s, v, at, t.width);
    release(s, at);
    s.values[v.id].valid = stored_valid(s.values[v.id].valid, width, t);

    return v;
}

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

// Whether the conversion that widens an arm of ?: costs the arm no
// instruction of its own, backend.md 6.1: its row writes the value
// extended, lands_in()'s; or its value is moved into the join in any
// case, and the extension is written where the move would be, a
// register variable's read, which the join copies, and a remainder,
// which stays in rdx, for a consumer that names another register,
// named, movslq %edx, %rax for a return.  variable is the register
// variable the paths write, or null.
bool
widens_free(const expr_t &arm,
            const symbol_t *variable,
            std::optional< reg_t > named)
{
    if (lands_in(arm, variable, true) || register_read(arm).variable)
        return true;

    return named && *named != rdx && is< binary_t >(arm) &&
           as< binary_t >(arm).op == binary_op_t::rem;
}

// Whether a conditional under a conversion that widens it has the
// conversion handed to each arm, backend.md 6.1 and chapter 4: where
// one arm at least widens free, widens_free(), so that the other's
// extension, if it needs one, is the one the join would have had, and
// no path writes more than the join and the extension after it.  For a
// register variable handed down, of the type its row computes in, each
// path writes the variable; for any other consumer, whose register is
// wanted, the arms join as before, each converted, and nothing follows
// the join, where the widening is signed: an unsigned value written at
// 32 bits is valid to 64 already, and its join needs no extension.  Not
// for a register variable of another type, whose extension after the
// join is its write, nor for GNU C's a ?: b, whose first arm is the
// condition's value, tested before any extension.
bool
widens_arms(const state_t &s,
            const cast_t &c,
            const type_t &to,
            const into_t *into,
            unsigned wanted)
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

    const auto *variable = paths ? into->variable : nullptr;
    const auto named = paths ? std::nullopt : wanted_register(s, wanted);
    const auto &k = as< conditional_t >(*c.arg);

    return k.yes && (widens_free(*k.yes, variable, named) ||
                     widens_free(*k.no, variable, named));
}

// A cast: a change of signedness alone that retypes(), retyped()'s; a
// conditional under a conversion that widens it, each arm widened where
// widens_arms() says, into a register variable's register on each path
// (chapter 4) or into the join (6.1); any other integer conversion,
// converted()'s.  An array decays to its address, 6.3.2.1p3, and a
// function designator to the pointer to the function, p4.  A cast to
// void evaluates its operand for its effect alone, chapter 6.6.  The
// decay and the function's address take a register variable handed down
// as & does, leaq buf(%rip), %rbx.
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

    if (is< array_t >(c.arg->type))
        return address_value(s, *c.arg, into);

    if (is< function_t >(c.arg->type))
        return function_pointer(s, *c.arg, into);

    if (retypes(c, to, into))
        return retyped(s, c, integer(to), *into);

    // l = c ? x : y into a register long, movslq -16(%rbp), %rbx on each
    // path; return a ? b % c : c as a long, movslq %edx, %rax
    if (widens_arms(s, c, to, into, wanted))
        return conditional(s, *c.arg, into && !into->taken ? into : nullptr,
                           wanted, &to);

    return converted(s, *c.arg, to, into, wanted);
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
// in, movl %r12d, %ebx.  wanted is the register the consumer of e's
// value names, rax for a return's, as a set of one, or none (6.1): the
// rows that write a fresh register take it as the lowest free one,
// unary(), binary(), assign() and cast() hand it on, and a conditional
// joins its arms there, and a statement expression hands both to its
// value.
value_t
rvalue(state_t &s, const expr_t &e, into_t *into, unsigned wanted)
{
    if (is< array_t >(e.type))
        throw std::logic_error("c1: an array as a value");

    if (s.reserved && pinned(e)) {
        const unsigned reserved = std::exchange(s.reserved, 0u);
        const auto v = rvalue(s, e, into, wanted);
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
        return unary(s, as< unary_t >(e), e.type, into, wanted);

    if (is< binary_t >(e))
        return binary(s, e, into, wanted);

    if (is< assign_t >(e))
        return assign(s, as< assign_t >(e), wanted);

    if (is< call_t >(e))
        return call(s, as< call_t >(e), e.type);

    if (is< cast_t >(e))
        return cast(s, as< cast_t >(e), e.type, into, wanted);

    if (is< statement_expression_t >(e))
        return statement_expression(s, e, into, wanted);

    // a stack allocation, a variable length array's storage, is not yet
    if (is< alloca_t >(e))
        not_yet("stack allocation");

    return conditional(s, e, into, wanted);
}

// The value of e for a consumer that names the register r, a return's
// for the result register (backend.md 6.1, 7.2): e's rows compute it
// in r where the last instruction can write it, and the consumer moves
// it where it is elsewhere.
value_t
rvalue_in(state_t &s, const expr_t &e, reg_t r)
{
    return rvalue(s, e, nullptr, bit(r));
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
// the left decides, and so is a conditional that jumps_on_arms(),
// conditional_branch()'s.  A flag condition, and one compared with 0 by == or
// !=, jumps on its operator's own flags.  Any other scalar, a constant
// or a pointer included, is its value compared with 0 at its own width.
// An object in memory is compared in place, by condition_on(): alone,
// cmpl $0, -4(%rbp), and as a comparison's left operand when the right
// one is an immediate, cmpl $5, -4(%rbp); the value is not wanted, so
// no register holds it.
void
cbranch(state_t &s, const expr_t &e, int label, bool sense)
{
    if (is< statement_expression_t >(e)) {
        statement_expression_branch(s, e, label, sense);
        return;
    }

    if (is< conditional_t >(e) &&
        jumps_on_arms(as< conditional_t >(e), truth_value)) {
        conditional_branch(s, e, label, sense);
        return;
    }

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
arithmetic_on(
    state_t &s, binary_op_t op, address_t &place, const expr_t &rhs, int width)
{
    const auto mnemonic = sized(arithmetic_mnemonic(op), width);

    const auto r = right_operand(s, op, rhs, width);
    if (!r.immediate.empty()) {
        place = formed(s, place);
        insn(s, mnemonic, { r.immediate, operand(s, place) });
        flagged(s, width);
        return;
    }

    const reg_t right = reg_of(s, r.value, registers_of(s, place));
    place = formed(s, place, bit(right));
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
         address_t &place,
         const expr_t &rhs,
         const int_t &t)
{
    const auto mnemonic = sized(shift_mnemonic(op, t.is_signed), t.width);

    const auto r = right_operand(s, op, rhs, t.width);
    if (!r.immediate.empty()) {
        place = formed(s, place);
        insn(s, mnemonic, { r.immediate, operand(s, place) });
        return;
    }

    const unsigned held = registers_of(s, place);
    const reg_t right = reg_of(s, r.value, held);

    count_into_cl(s, r, right, held);

    place = formed(s, place, bit(rcx));
    insn(s, mnemonic, { "%cl", operand(s, place, bit(rcx)) });
    release(s, r.value);
}

// p += n and p -= n: addq $8, -8(%rbp) for a constant n the immediate
// holds times the size; otherwise n in a register, multiplied by the
// size at any size but 1, an object in memory of 64 bits by
// multiplied()'s imulq $4, -16(%rbp), %rax, then addq R, obj or
// subq R, obj.  G6.
void
displaced_on(
    state_t &s, binary_op_t op, address_t &place, const expr_t &n, long size)
{
    const auto mnemonic = op == binary_op_t::add ? "addq" : "subq";
    const unsigned held = registers_of(s, place);

    value_t r;
    if (is< literal_t >(n)) {
        const auto d = scaled(as< literal_t >(n).value, size);
        if (fits_imm32(d)) {
            place = formed(s, place);
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
    place = formed(s, place, bit(right));
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
    auto place = place_of(s, a, pinned_by(a));

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

// record copies

// One unit of a record's copy, backend.md 6.6: width bits at offset off
// the right's place loaded into scratch, then stored at the same offset
// off the left's, movzwl 2(%rcx), %edx; movw %dx, 2(%rax).  The load is
// the unsigned type's, which writes the whole register, so no unit
// reads the register's old content (G18).
void
copy_unit(state_t &s,
          const address_t &to,
          const address_t &from,
          long offset,
          int width,
          reg_t scratch)
{
    auto source = from;
    auto target = to;
    source.displacement += offset;
    target.displacement += offset;

    const auto [load, loaded] = extension(int_t { width, false }, 32);
    insn(s, load, { operand(s, source), reg(scratch, loaded) });
    insn(s, sized("mov", width), { reg(scratch, width), operand(s, target) });
}

// A record's place as the copy reaches each of its size bytes from the
// place's displacement, backend.md 6.6: the place itself where every
// displacement up to size fits 32 bits, or for a loop, indexed, where
// it also takes the loop's index, a slot or a base and a displacement;
// otherwise the address formed by its leaq, the bytes off its register:
// a label, which takes no index, and an address with an index of its
// own.  Its registers reloaded, so that no unit reloads one.
address_t
reached(state_t &s, const address_t &a, long size, bool indexed)
{
    const bool joins = !indexed || (a.label.empty() && a.index.id < 0);
    const bool near =
        !a.label.empty() || fits_imm32(wrapped_add(a.displacement, size));

    auto reach = joins && near ? a : address_t { .base = address_of(s, a) };
    operand(s, reach);

    return reach;
}

// A record's copy of more than eight units, backend.md 6.6: one index
// from -size up to 0 by the unit, which both places join, so the unit's
// two accesses address size bytes above it, and the add that steps the
// index sets the flag the loop's jne tests: movq $-72, %rcx; then
// movq -64(%rbp,%rcx,1), %rdx; movq %rdx, (%rax,%rcx,1); addq $8,
// %rcx; jne back, five instructions whatever the size.  The copy is
// still lowest address first (G18).
void
copy_loop(state_t &s,
          const address_t &to,
          const address_t &from,
          long size,
          int width,
          reg_t scratch)
{
    const auto index = fresh(s, 64, bit(scratch));
    const reg_t r = reg_of(s, index);
    insn(s, "movq", { imm(-size), reg(r, 64) });

    const int top = new_label(s);
    place_label(s, top);

    auto source = from;
    auto target = to;
    source.index = target.index = index;
    copy_unit(s, target, source, size, width, scratch);
    insn(s, "addq", { imm(width / 8), reg(r, 64) });
    jump_if(s, binary_op_t::ne, false, top);

    release(s, index);
}

// A record's assignment, backend.md 6.6 and G18, and the copy of a
// string literal into a local array, G22: an inline copy of the left's
// type in units of its alignment, at most 8, of which its size is a
// multiple, an array's its element's, lowest address first, each unit
// loaded and then stored, padding included, and no call.  The left's
// place is evaluated first, held across the right's as an assignment's
// is (place_of()), then the right's (G13), and the left's formed()
// after it.  Up to eight units are written out; above that, a loop,
// copy_loop()'s.  Each place keeps its own form, a slot, a label, a
// base with a displacement and an index, the unit's offset in the
// displacement.
void
copy_record(state_t &s, const assign_t &a)
{
    const long size = size_of(a.lhs->type);
    const auto units = units_of(size, align_of(a.lhs->type));

    const auto left = place_of(s, a, 0);
    const auto right = address(s, *a.rhs);

    const auto to = reached(s, formed(s, left), size, units.loop);
    const auto from = reached(s, right, size, units.loop);
    const auto scratch = fresh(s, 64);
    const reg_t r = reg_of(s, scratch);

    if (units.loop)
        copy_loop(s, to, from, size, units.unit * 8, r);
    else
        for (long k = 0; k < units.count; ++k)
            copy_unit(s, to, from, k * units.unit, units.unit * 8, r);

    release(s, scratch);
    release(s, to);
    release(s, from);
}

// The zeroing of more than eight units, backend.md 6.6: copy_loop()'s
// index from -size up to 0 by the unit, which the slot joins with the
// size in its displacement, and at each unit the store of the
// immediate 0: movq $-200, %rax; then movq $0, (%rbp,%rax,1); addq $8,
// %rax; jne back, four instructions whatever the size.
void
zero_loop(state_t &s, int displacement, long size, int width)
{
    const auto index = fresh(s, 64);
    const reg_t r = reg_of(s, index);
    insn(s, "movq", { imm(-size), reg(r, 64) });

    const int top = new_label(s);
    place_label(s, top);

    auto unit = slot_address(displacement);
    unit.displacement += size;
    unit.index = index;
    insn(s, sized("mov", width), { imm(0), operand(s, unit) });
    insn(s, "addq", { imm(width / 8), reg(r, 64) });
    jump_if(s, binary_op_t::ne, false, top);

    release(s, index);
}

// The zeroing of size bytes at displacement of a local's slot, in
// units_of() the object's alignment, from the lowest address up: a
// record or an array a list initializes zeroed whole, padding included,
// before the first listed value is evaluated, G20, and an array a
// string literal initializes zeroed past the copy, after it, G22.  Each
// unit is the store of the immediate 0 that initialize() writes for a
// constant, movl $0, -8(%rbp), and takes no register; more than eight
// are zero_loop()'s.
void
zero_slot(state_t &s, int displacement, long size, int alignment)
{
    const auto units = units_of(size, alignment);
    const int width = units.unit * 8;

    if (units.loop) {
        zero_loop(s, displacement, size, width);
        return;
    }

    for (long k = 0; k < units.count; ++k) {
        const long offset = k * units.unit;
        insn(s, sized("mov", width),
             { imm(0), at(displacement + static_cast< int >(offset)) });
    }
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
    if (copies_object(e)) {
        copy_record(s, as< assign_t >(e));
        return;
    }

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

    if (is< statement_expression_t >(e)) {
        statement_expression_effect(s, e);
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
// lowest address, the elements upward from it.  A record or an array
// its list initializes is zeroed whole, zero_slot(), and the statements
// after the declaration store the listed values (G20).  An array a
// string literal initializes is the copy of the literal, effect()'s
// row of a record's assignment, then the zeroing of the bytes past the
// copy, zero_slot() over them (G22).
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

    if (d.init && copies_object(*d.init))
        effect(s, *d.init);
    else if (d.init)
        initialize(s, *d.init, at(displacement), integer(sym->type).width);

    if (!d.zeroed)
        return;

    const long from = zeroed_from(d);
    zero_slot(s, displacement + static_cast< int >(from),
              size_of(sym->type) - from, align_of(sym->type));
}

// The context the rows of an expression leave for the rows below them:
// the registers left for last, reserved and ahead, the copy of a
// borrowed read a consumer names, the read a compound assignment writes
// in place, and an address formed across a right operand that spills.
// A statement expression's statements are generated with none of it,
// so each is the statement it would be at a boundary of statements, and
// its value with all of it, as the operand it is.
struct context_t
{
    unsigned reserved;
    unsigned ahead;
    int named_read;
    unsigned named_copy;
    int in_place;
    bool held_across;
    bool defer_label;
};

context_t
set_aside(state_t &s)
{
    return { .reserved = std::exchange(s.reserved, 0u),
             .ahead = std::exchange(s.ahead, 0u),
             .named_read = std::exchange(s.named_read, -1),
             .named_copy = std::exchange(s.named_copy, 0u),
             .in_place = std::exchange(s.in_place, -1),
             .held_across = std::exchange(s.held_across, false),
             .defer_label = std::exchange(s.defer_label, false) };
}

void
take_back(state_t &s, const context_t &c)
{
    s.reserved = c.reserved;
    s.ahead = c.ahead;
    s.named_read = c.named_read;
    s.named_copy = c.named_copy;
    s.in_place = c.in_place;
    s.held_across = c.held_across;
    s.defer_label = c.defer_label;
}

#include "c1-statements.hh"

// The operand a row takes for e, backend.md 6.1: e, but for a comma,
// whose left operand is generated for its effect, 6.5.18p2, and whose
// right operand's operand the row then takes in its own form, addl
// -12(%rbp), %eax for a + (t = b, t); and for a statement expression
// that leaves the values held beside it where they are,
// saves_beside()'s, and whose value is no read of a register variable
// its block declares: its statements, marked as for its value, then its
// value's operand, which the row takes in its own form, an object in
// memory as the instruction's operand, addl -8(%rbp), %eax for a + ({
// b; }), as for a + (b).
const expr_t &
operand_of(state_t &s, const expr_t &e)
{
    if (is< binary_t >(e) && as< binary_t >(e).op == binary_op_t::comma) {
        effect(s, *as< binary_t >(e).lhs);
        return operand_of(s, *as< binary_t >(e).rhs);
    }

    if (!is< statement_expression_t >(e))
        return e;

    const auto &x = as< statement_expression_t >(e);
    if (!x.value || saves_beside(s, e))
        return e;

    const auto *read = register_read(*x.value).variable;
    if (read && declares(x, read))
        return e;

    {
        const incoming_t incoming { s };
        statement_expression_body(s, x);
    }

    return operand_of(s, *x.value);
}

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

// The save area of a variadic definition that calls va_start, chapter
// 5 and shape.md chapter 4: a slot of its 184 bytes at an alignment of
// 16, which movaps asks of its vector half.  The six general argument
// registers are stored at 0 to 40 and the eight vector ones at 48 to
// 160, every one whatever al says, G19, and at 176 the address of the
// first argument the caller left on the stack past the named ones: the
// seventh argument's place, 16(%rbp), past the saved rbp and the return
// address, 8 bytes on for each named parameter past the sixth.  It
// follows the named parameters' own stores and moves, which read the
// argument registers and write none, so the area holds them as they
// arrived; the address is formed last, in a register of the stack's,
// once every argument register is stored.
void
save_area(state_t &s, const function_decl_t &fd)
{
    const int area = allocate(s, size_of(fd.va_area->type), 16);
    s.slots.emplace(fd.va_area, area);

    const int registers = static_cast< int >(std::size(argument_registers));
    for (int k = 0; k < registers; ++k)
        store(s, argument_registers[k], at(area + 8 * k), 64);

    for (int k = 0; k < 8; ++k)
        insn(s, "movaps", { std::format("%xmm{}", k), at(area + 48 + 16 * k) });

    const int named = static_cast< int >(fd.params.size());
    const auto first = fresh(s, 64);
    const reg_t r = reg_of(s, first);
    insn(s, "leaq",
         { at(16 + 8 * std::max(0, named - registers)), reg(r, 64) });
    store(s, r, at(area + 176), 64);
    release(s, first);
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
// falls off its end, 5.1.2.3.4.  Each field a function's generation
// changes starts over, but the literals, so that a refusal caught
// midway through the function before leaves nothing behind.
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
    s.floor = 0;
    s.saved = fd.registers;
    s.in_place = -1;
    s.reserved = 0;
    s.ahead = 0;
    s.named_read = -1;
    s.named_copy = 0;
    s.held_across = false;
    s.defer_label = false;
    s.zero_flag = 0;
    s.frame = 8 * s.saved;
    s.function = fd.sym->name;
    s.labels = 0;
    s.ret = new_label(s);
    s.terminated = false;
    s.jump_targets.clear();
    s.user_labels.clear();
    s.case_labels.clear();

    parameters(s, fd);
    if (fd.va_area)
        save_area(s, fd);

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
// The string literals follow the last function, the objects the unit
// defines follow them, and the aliases it declares follow those.  The
// note section marks the stack not executable.  A droppable function
// whose body c1 refuses writes nothing either, the string literals it
// met and the static objects its body declares among them, and the
// next function() starts its state over; any other refusal is thrown
// again with the function's name.
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

    std::vector< const function_decl_t * > dropped;
    for (const auto &fd : unit.functions) {
        if (!fd.body)
            continue;

        const std::size_t strings = s.strings.size();
        try {
            function(s, fd);
        }
        catch (const refusal_t &e) {
            if (!fd.droppable)
                refused_in(fd.sym->name, e);

            s.strings.resize(strings);
            dropped.push_back(&fd);
        }
    }

    data(s.out, s.strings, written_objects(unit, dropped), target);
    aliases(s.out, unit.aliases);

    out << "\t.section\t.note.GNU-stack,\"\",@progbits\n";
}

} // namespace rcc
