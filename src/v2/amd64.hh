// -*- mode: c++; -*-

// x86-64, docs/backend.md: the registers, the state of a function's
// generation, the rows of the zoo's types this target writes itself,
// and the unit's entry.  The rows are declared here for build.cc, whose
// models call them, and those of a template defined here too; the rest
// are defined in amd64.cc, which reaches the tree only through a
// handle's row.  Every width a row writes is its node's view, handed to
// the text as data.

#pragma once

#include "register-stack.hh"
#include "targets.hh"
#include "unit.hh"
#include "zoo.hh"

#include <concepts>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rcc::amd64 {

// The registers, chapter 4: the nine expression registers, the
// caller-saved set, in the order a new value takes them, rax first.
enum reg_t : int { rax, rcx, rdx, rsi, rdi, r8, r9, r10, r11 };

// A register as a set of one, rcc's bit(): the sets are the registers
// a value is taken outside of, and those an instruction pins.
constexpr unsigned
bit(reg_t r)
{
    return 1u << r;
}

// The state of one function's generation, which the function's row
// builds fresh.  The body is a buffer, which the frame is written
// around; the function's name makes its labels, which are counted per
// function, and ret is the one epilogue's.  terminated says that no
// path reaches what is written next: the last instruction written is an
// unconditional jump, which jump() sets and place_label() clears, so an
// if whose then part ends so writes no jump over its else part, and
// main writes no return of 0 (backend.md 7.2).  zero_flag is the width
// at which the last instruction written set ZF from the value it
// computed, flagged()'s, or 0, until the next instruction or label: a
// jump on that value at that width then needs no cmp (7.1).  loops is
// the labels each loop generated so far took for its body's jumps, by
// the loop's number.  frame is the bytes the slots take below rbp, the
// locals' and the spills' in the order they are taken, and slots each
// local's displacement off rbp, by its number; calls says that the
// function calls, which moves rsp below the slots (chapter 5).
// reserved is the registers take() leaves for last while a value meant
// for a pinned register is generated, so it lands there (chapter 6.5).
// ahead is the registers a pinned instruction generated after the value
// now being generated would move a held value out of, which the
// instruction on a spilled value keeps its destination out of
// (right_written()).
struct state_t
{
    std::ostringstream body { };
    std::string function;
    int labels = 0;
    int ret = 0;
    bool terminated = false;
    int zero_flag = 0;
    std::unordered_map< int, loop_labels_t > loops { };

    long frame = 0;
    std::unordered_map< int, int > slots { };
    bool calls = false;

    registers_t< reg_t > registers { };
    unsigned reserved = 0;
    unsigned ahead = 0;
};

// A jump to the function's label n; no path reaches what follows it.
void jump(state_t &, int n);

// A register for a new value, register-stack.hh's, taken outside avoid;
// it spills a held value when every one is held.
reg_t take(state_t &, unsigned avoid = 0);

// The function's label n, where a jump may reach it from elsewhere with
// the flags its own path left: a path reaches what follows, and no
// instruction's ZF stands.
void place_label(state_t &, int n);

// The jump to label on the flags a cmp set, where op holds, rcc's
// jump_if(): jl, and je and jne for == and !=, which read ZF alone.
void jump_if(state_t &, binary_op_t op, int label);

// The jump on the held value v compared with 0 at its view, rcc's
// test_zero(): cmpl $0, %eax or cmpq $0, %rax, then jne where sense, je
// otherwise; v stays held.
void test_zero(state_t &, value_t v, view_t view, int label, bool sense);

// A mov of the immediate value into r, at 32 bits: movl $1, %eax.
void move_immediate(state_t &, reg_t r, int value);

// The directive before a loop's head, rcc's align_loop_head(): .p2align
// 5, the head on a 32-byte boundary, a convention of the target; with
// no fill byte named, the assembler fills the gap with nops.
void align_loop_head(state_t &);

// The next slot of the frame, of size bytes at align, a local's or a
// spill's: its displacement off rbp.
int allocate(state_t &, long size, int align);

// The store of r to a spill slot, and the reload of a spilled value
// into r from its slot, register-stack.hh's.
void store_spill(state_t &, reg_t r, int displacement);
void reload_spill(state_t &, reg_t r, int displacement);

// An immediate, as the assembler reads it: $5.
std::string imm(long long value);

// The constant row, a constant of the view in a register taken for it,
// movl $5, %eax, movq $5, %rax, or movabsq for a 64-bit one no 32-bit
// immediate holds.  Every value row of x86-64 is handed wanted, the
// registers the consumer of the value names, a set of at most one, rax
// for a return's (6.1): a hint, which a row is free not to honour
// (docs/typed-tree.md 3.4), none where the consumer names none.  A leaf
// takes the lowest free register whatever the consumer names, as rcc's
// literal() and load() do.
value_t constant_value(state_t &, long long value, view_t view);

template< view_t V >
value_t
value(state_t &s, const literal_t< V > &c, unsigned = 0)
{
    return constant_value(s, c.value, V);
}

template< view_t V >
value_t
value(state_t &s, const zero_t< V > &, unsigned = 0)
{
    return constant_value(s, 0, V);
}

inline value_t
value(state_t &s, const wide_t &c, unsigned = 0)
{
    return constant_value(s, c.value, view_t::i64);
}

// The load row: the object a slot holds at its view, extended to 32
// bits by its sign where it is narrow, in a register taken for it,
// movsbl -1(%rbp), %eax, movl -4(%rbp), %eax or movq -8(%rbp), %rax.
value_t load(state_t &, slot_t where, view_t view, sign_t sign);

template< view_t V, sign_t S >
value_t
value(state_t &s, const load_t< V, S > &l, unsigned = 0)
{
    return load(s, l.where, V, S);
}

// - or ~ on r in place, at the view.
void operate(state_t &, unary_op_t op, reg_t r, view_t view);

// - and ~, 6.5.4.3p3, p4, rcc's unary(): the operand's value, handed the
// consumer's register, then the operation on its register in place,
// negl %eax, which leaves the node's value there.  ! is a row of its
// own, below.
template< unary_op_t op, view_t V, typename A >
    requires(op != unary_op_t::log_not)
value_t
value(state_t &s, const unary_t< op, V, A > &u, unsigned wanted = 0)
{
    const value_t v = value(s, u.arg, wanted);
    operate(s, op, reg_of(s, v), V);

    return v;
}

// The operation of op into r from an operand at the view: a register,
// the object in a slot, or an operand as its text, an immediate as
// immediate_of() writes it or a spilled value's slot.
void operate(state_t &, binary_op_t op, reg_t r, reg_t source, view_t view);
void operate(state_t &, binary_op_t op, reg_t r, slot_t source, view_t view);
void operate(
    state_t &, binary_op_t op, reg_t r, std::string_view source, view_t view);

// The operation of op on the held values l and r at the view, rcc's
// arithmetic(): its value is in l's register, or in r's where op
// commutes and that is the one wanted names, or where l is spilled and
// is read in its slot; the other value is consumed.
value_t arithmetic(state_t &,
                   binary_op_t op,
                   value_t l,
                   value_t r,
                   view_t view,
                   unsigned wanted);

// The three-operand imul at the view, rcc's object_product(): the
// source, a register or the object in a slot, times the immediate
// factor into r.
void multiply(
    state_t &, reg_t r, reg_t source, std::string_view factor, view_t view);
void multiply(
    state_t &, reg_t r, slot_t source, std::string_view factor, view_t view);

// The division of op, / or %, of the held dividend l by the divisor, a
// held value r, which is consumed, or the object in a slot, at the
// view: the quotient or the remainder, held where idiv leaves it or in
// l's register.
value_t divide(state_t &,
               binary_op_t op,
               value_t l,
               value_t r,
               view_t view,
               unsigned wanted);
value_t divide(state_t &,
               binary_op_t op,
               value_t l,
               slot_t divisor,
               view_t view,
               unsigned wanted);

// The store rows, an operand into a slot at the view: a register, or an
// immediate as immediate_of() writes it.
void store(state_t &, reg_t r, slot_t where, view_t view);
void store(state_t &, std::string_view immediate, slot_t where, view_t view);

// A constant as the immediate operand of an instruction, 32 bits,
// sign-extended in a 64-bit one: a literal_t and a zero_t, and no
// wide_t, which no immediate holds.  The value is written at its view,
// modulo 2^N and signed, so a narrow store of an unsigned char's 255 is
// movb $-1, -1(%rbp).
std::string immediate_at(long long value, view_t view);

template< view_t V >
std::string
immediate_of(const literal_t< V > &c)
{
    return immediate_at(c.value, V);
}

template< view_t V >
std::string
immediate_of(const zero_t< V > &)
{
    return imm(0);
}

// A shift's constant count as its instruction's immediate: the low 8
// bits of the count, which is what the instruction encodes.
std::string count_of(long long count);

// The instruction of a shift of the value l by the count held in r, at
// the view, shll %cl, %eax, with what is in the way moved first, the
// count into rcx at its own view, count; r is consumed.
void shift(
    state_t &, binary_op_t op, value_t l, value_t r, view_t view, view_t count);

// A value an instruction takes as its immediate: a type immediate_of()
// is written for (principle 7).
template< typename C >
concept immediate = requires(const C &c) { immediate_of(c); };

// The niche of initialize(), chapter 6.6: an immediate is stored by
// one mov into the slot at its view, and no register is held.
template< immediate C >
void
initialize(state_t &s, slot_t where, const C &c)
{
    store(s, immediate_of(c), where, view_of(c));
}

// The operators of rcc's arithmetic_mnemonic(), + - * & | ^, whose
// instruction is op R, L, R a register, an immediate or an operand in
// memory.  The divisions and the shifts are rows of their own, below:
// idiv takes no immediate, and a shift no operand in memory and an
// immediate of its own.
template< binary_op_t op >
concept arithmetic_operator =
    op == binary_op_t::add || op == binary_op_t::sub ||
    op == binary_op_t::mul || op == binary_op_t::bit_and ||
    op == binary_op_t::bit_or || op == binary_op_t::bit_xor;

template< binary_op_t op >
concept division_operator = is_division(op);

template< binary_op_t op >
concept shift_operator = is_shift(op);

// The registers the instruction of a family pins, chapter 6.5, rcc's
// pins(): rax and rdx for / and %, rcx for << and >> by a count that is
// no constant, which is the immediate.
constexpr unsigned
pins(family_t family)
{
    return family == family_t::division ? bit(rax) | bit(rdx) : bit(rcx);
}

// The registers the instruction of op pins, its family's; constant_right
// says whether the right operand is a constant, which a shift takes as
// its immediate.
constexpr unsigned
pins(binary_op_t op, bool constant_right)
{
    if (is_division(op))
        return pins(family_t::division);

    return is_shift(op) && !constant_right ? pins(family_t::shift) : 0;
}

// The registers a node holds out of the choice for its left operand's
// value while it generates its right operand, rcc's pinned_by(): those
// the right operand's top operator pins, a fact of its class, top_t,
// which keeps its family.  The value then does not stand where that
// instruction wants its operands.  Any other class pins nothing, an
// erased one too, since the builder keeps a node that pins as a top_t;
// a division or a shift deeper in the right operand changes nothing.
template< typename R >
constexpr unsigned
pinned_by(const R &)
{
    return 0;
}

template< family_t F >
constexpr unsigned
pinned_by(const top_t< F > &)
{
    return pins(F);
}

// The registers the instruction of a node or of any operand within it
// pins, rcc's pinned_within(): pins() over the whole tree, a summary
// each node makes of its operands' and an erased one hands up through
// its handle (principle 3).  A value held while the node is generated
// may have to move out of one of them, as out of rcx for a shift's
// count, or out of rax and rdx for a division, deeper in a right
// operand.  A leaf pins nothing, and a conversion what its operand
// pins.
template< constant C >
constexpr unsigned
pinned_within(const state_t &, const C &)
{
    return 0;
}

template< view_t V, sign_t S >
constexpr unsigned
pinned_within(const state_t &, const load_t< V, S > &)
{
    return 0;
}

template< view_t V, sign_t S >
constexpr unsigned
pinned_within(const state_t &, const place_t< V, S > &)
{
    return 0;
}

template< unary_op_t op, view_t V, typename A >
unsigned
pinned_within(const state_t &s, const unary_t< op, V, A > &u)
{
    return pinned_within(s, u.arg);
}

template< binary_op_t op, view_t V, typename L, typename R >
unsigned
pinned_within(const state_t &s, const binary_t< op, V, L, R > &b)
{
    return pins(op, constant< R >) | pinned_within(s, b.lhs) |
           pinned_within(s, b.rhs);
}

template< family_t F >
unsigned
pinned_within(const state_t &s, const top_t< F > &t)
{
    return pinned_within(s, t.node);
}

template< view_t V, view_t F, typename A >
unsigned
pinned_within(const state_t &s, const sext_t< V, F, A > &e)
{
    return pinned_within(s, e.arg);
}

template< view_t V, view_t F, typename A >
unsigned
pinned_within(const state_t &s, const zext_t< V, F, A > &e)
{
    return pinned_within(s, e.arg);
}

template< view_t V, typename A >
unsigned
pinned_within(const state_t &s, const trunc_t< V, A > &t)
{
    return pinned_within(s, t.arg);
}

template< view_t V >
unsigned
pinned_within(const state_t &s, const assign_t< V > &a)
{
    return pinned_within(s, a.value);
}

// A call pins rax, where the convention puts its result (chapter 8),
// and whatever its arguments pin, call_pins()'s.
unsigned call_pins(const state_t &, const std::vector< argument_t > &args);

template< view_t V >
unsigned
pinned_within(const state_t &s, const call_t< V > &c)
{
    return call_pins(s, c.args);
}

inline unsigned
pinned_within(const state_t &s, const call_void_t &c)
{
    return call_pins(s, c.args);
}

// The registers a pinned instruction within a node would move a value
// held across the node out of, rcc's moved_within(), 6.5: pins() over
// the tree, as pinned_within(), but nothing below && and || and nothing
// within a call, which spill every held value first, the first before
// their paths, before_paths(), and a call at its start, so that no
// value is held in a register where an instruction within them pins
// one.  A summary of its own, each node's of its operands', which an
// erased one hands up through its handle.
template< constant C >
constexpr unsigned
moved_within(const state_t &, const C &)
{
    return 0;
}

template< view_t V, sign_t S >
constexpr unsigned
moved_within(const state_t &, const load_t< V, S > &)
{
    return 0;
}

template< view_t V, sign_t S >
constexpr unsigned
moved_within(const state_t &, const place_t< V, S > &)
{
    return 0;
}

template< unary_op_t op, view_t V, typename A >
unsigned
moved_within(const state_t &s, const unary_t< op, V, A > &u)
{
    return moved_within(s, u.arg);
}

template< binary_op_t op, view_t V, typename L, typename R >
unsigned
moved_within(const state_t &s, const binary_t< op, V, L, R > &b)
{
    return pins(op, constant< R >) | moved_within(s, b.lhs) |
           moved_within(s, b.rhs);
}

template< binary_op_t op, view_t V, typename L, typename R >
    requires logical_operator< op >
constexpr unsigned
moved_within(const state_t &, const binary_t< op, V, L, R > &)
{
    return 0;
}

template< family_t F >
unsigned
moved_within(const state_t &s, const top_t< F > &t)
{
    return moved_within(s, t.node);
}

template< view_t V, view_t F, typename A >
unsigned
moved_within(const state_t &s, const sext_t< V, F, A > &e)
{
    return moved_within(s, e.arg);
}

template< view_t V, view_t F, typename A >
unsigned
moved_within(const state_t &s, const zext_t< V, F, A > &e)
{
    return moved_within(s, e.arg);
}

template< view_t V, typename A >
unsigned
moved_within(const state_t &s, const trunc_t< V, A > &t)
{
    return moved_within(s, t.arg);
}

template< view_t V >
unsigned
moved_within(const state_t &s, const assign_t< V > &a)
{
    return moved_within(s, a.value);
}

template< view_t V >
constexpr unsigned
moved_within(const state_t &, const call_t< V > &)
{
    return 0;
}

constexpr unsigned
moved_within(const state_t &, const call_void_t &)
{
    return 0;
}

// The registers a node hands its left operand, of those its consumer
// names, wanted, rcc's wanted_left(): wanted, outside own, the
// registers the node's own instruction pins, where no instruction
// within its right operand pins one, which would move the left operand
// out of wanted or find the right operand's own values there.  None
// otherwise, and the left operand is as the lowest free register makes
// it.
template< typename R >
unsigned
wanted_left(const state_t &s, unsigned wanted, unsigned own, const R &rhs)
{
    return pinned_within(s, rhs) ? 0 : wanted & ~own;
}

// The registers a node generates its right operand with left for last,
// rcc's held_out(): named, those its left operand l was handed, where l
// holds one of them, so that a right operand that spills l leaves it
// free for l's reload; none where l holds another register, where the
// result stays.  l is the value its row has just returned, in a
// register, so rcc's case of a spilled l does not arise.
unsigned held_out(const state_t &, value_t l, unsigned named);

// The value of e generated while the registers in pinned are left for
// last, so that it does not land where an instruction generated after
// it wants its operands, rcc's rvalue_avoiding() (chapter 6.5).  The
// reservation is an avoidance, not a pin (take()).
template< typename T >
value_t
rvalue_avoiding(state_t &s, const T &e, unsigned pinned, unsigned wanted = 0)
{
    const unsigned reserved = std::exchange(s.reserved, s.reserved | pinned);
    const value_t v = value(s, e, wanted);
    s.reserved = reserved;

    return v;
}

// The left operand of a node whose right operand is rhs, generated
// outside pinned and handed named, rvalue_avoiding()'s, with the
// registers a pinned instruction within rhs would move it out of added
// to s.ahead, rcc's binary() and moved_within(): the left operand's
// value is held while rhs is generated, so a row within it that chooses
// its result's register, right_written(), chooses none of them.  A call
// within rhs spills the value first, so it adds none, (a + g(b)) * (b +
// g(c)) reading a in its slot where (a + g(b)) * (b + c / a) reloads
// it.
template< typename L, typename R >
value_t
left_operand(
    state_t &s, const L &lhs, const R &rhs, unsigned pinned, unsigned named)
{
    const unsigned ahead =
        std::exchange(s.ahead, s.ahead | moved_within(s, rhs));
    const value_t l = rvalue_avoiding(s, lhs, pinned, named);
    s.ahead = ahead;

    return l;
}

// The primary of + - * & | ^, 6.5.6, 6.5.7, 6.5.11 to 6.5.13, rcc's
// binary() and arithmetic(), chapter 6.1: the left operand's value
// first, G13, generated outside the registers the right operand's top
// operator pins, so that a + (b << a) loads a into eax and not into the
// ecx the count goes to, and handed the consumer's register,
// wanted_left(); held while the right operand's is taken in a register
// of its own, outside held_out(); then arithmetic(), addl %edx, %eax,
// or into the right operand's register where it is the one the
// consumer names: the count (0 << d) + (d = c) is addl %edx, %ecx, its
// left operand a shift, held outside ecx, and its right one loaded into
// the ecx a count's consumer names.  A wide_t is no immediate, so it is
// this row, movabsq into a register where the source has it.
template< binary_op_t op, view_t V, typename L, typename R >
    requires arithmetic_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    const unsigned named = wanted_left(s, wanted, 0, b.rhs);

    const value_t l = left_operand(s, b.lhs, b.rhs, pinned_by(b.rhs), named);
    const value_t r = rvalue_avoiding(s, b.rhs, held_out(s, l, named));

    return arithmetic(s, op, l, r, V, wanted);
}

// Whether a node with the constant L on the left of op is written with
// its operands exchanged, rcc's constant_first() as ordered() asks it:
// op commutes, and the constant is an immediate, which a wide_t is not.
template< binary_op_t op, typename L >
concept constant_first = commutes(op) && immediate< L >;

// The niches of the primary, chapter 6.1.  A constant on the left of an
// operator that commutes is the row of the node with its operands
// exchanged, 5 + a as a + 5: the value of the right operand, which is
// no constant, then the constant as the immediate.
template< binary_op_t op, view_t V, typename L, typename R >
    requires arithmetic_operator< op > && constant_first< op, L >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    return value(s, binary_t< op, V, R, L > { b.rhs, b.lhs }, wanted);
}

// A load of the operation's view on the right is the instruction's
// operand in memory, addl -8(%rbp), %eax, addq -16(%rbp), %rax, and
// takes no register.  Its type is the more special, so the node the
// mirror takes, 5 + a, is excluded by name; 5 - a is this row, movl $5,
// %eax and subl -4(%rbp), %eax.  The right operand pins nothing, so the
// left operand is handed the consumer's register, as in every niche of
// an operand in memory or an immediate.
template< binary_op_t op, view_t V, typename L, sign_t S >
    requires(arithmetic_operator< op > && !constant_first< op, L >)
value_t
value(state_t &s,
      const binary_t< op, V, L, load_t< V, S > > &b,
      unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    operate(s, op, reg_of(s, l), b.rhs.where, V);

    return l;
}

// An immediate on the right is the instruction's, negative ones as
// written: addl $5, %eax, andq $-2, %rax.  Of the mirror's nodes it
// would take only a constant on both sides, which the builder folds.
template< binary_op_t op, view_t V, typename L, immediate I >
    requires arithmetic_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, I > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    operate(s, op, reg_of(s, l), immediate_of(b.rhs), V);

    return l;
}

// imul by an immediate is the three-operand form, rcc's arithmetic():
// the left operand's value, its register the source and the
// destination, imull $5, %eax, %eax.
template< view_t V, typename L, immediate I >
value_t
value(state_t &s,
      const binary_t< binary_op_t::mul, V, L, I > &b,
      unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    multiply(s, reg_of(s, l), reg_of(s, l), immediate_of(b.rhs), V);

    return l;
}

// A load of the operation's view on the left of imul by an immediate
// is the source in memory, rcc's product(): imull $5, -4(%rbp), %eax,
// no load, and the product in a register taken for it, the lowest free
// whatever the consumer names.  5 * a is this row, through the mirror.
template< view_t V, immediate I, sign_t S >
value_t
value(state_t &s,
      const binary_t< binary_op_t::mul, V, load_t< V, S >, I > &b,
      unsigned = 0)
{
    const value_t v = fresh(s);
    multiply(s, reg_of(s, v), b.lhs.where, immediate_of(b.rhs), V);

    return v;
}

// / and % by a divisor that is no local, 6.5.6, rcc's binary() and
// divide(), chapter 6.5: idiv takes its dividend in rax, and writes the
// quotient there and the remainder in rdx.  A division keeps its own
// pin, rcc's rvalue(): it is generated outside any reservation around
// it, since the pin, not the outer wish, decides where its result is
// written.  The dividend first, G13, outside what the divisor's top
// operator pins and handed the consumer's register, wanted_left();
// then the divisor, held outside rax and rdx and held_out(), a constant
// too, since idiv takes no immediate, movl $5, %ecx; then divide().
template< binary_op_t op, view_t V, typename L, typename R >
    requires division_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    const unsigned outer = std::exchange(s.reserved, 0u);
    const unsigned named = wanted_left(s, wanted, 0, b.rhs);

    const value_t l = left_operand(s, b.lhs, b.rhs, pinned_by(b.rhs), named);
    const value_t r = rvalue_avoiding(
        s, b.rhs, held_out(s, l, named) | pins(op, constant< R >));
    const value_t v = divide(s, op, l, r, V, wanted);

    s.reserved = outer;

    return v;
}

// A local of the operation's view as the divisor is idiv's operand in
// memory, idivl -8(%rbp), and takes no register; it pins nothing, so
// the dividend is handed the consumer's register.
template< binary_op_t op, view_t V, typename L, sign_t S >
    requires division_operator< op >
value_t
value(state_t &s,
      const binary_t< op, V, L, load_t< V, S > > &b,
      unsigned wanted = 0)
{
    const unsigned outer = std::exchange(s.reserved, 0u);

    const value_t l = value(s, b.lhs, wanted);
    const value_t v = divide(s, op, l, b.rhs.where, V, wanted);

    s.reserved = outer;

    return v;
}

// A shift's count that is no constant, rcc's shift_count(): generated
// with rax and avoid reserved and with rcx taken out of what a node
// around the shift reserves for its own operand, since the instruction
// consumes the count in cl (chapter 6.5), so a count's load lands in
// rcx; and rcx is the register the count's consumer names, as a return
// names rax (6.1).
template< typename R >
value_t
shift_count(state_t &s, const R &count, unsigned avoid)
{
    const unsigned outer = std::exchange(s.reserved, s.reserved & ~bit(rcx));
    const value_t v = rvalue_avoiding(s, count, avoid | bit(rax), bit(rcx));
    s.reserved = outer;

    return v;
}

// << and >> by a count that is no constant, 6.5.8, rcc's binary() and
// shift(), chapter 6.5: the shifted value first, G13, generated with
// rcx left for last, where the count goes, and outside what the count's
// top operator pins, and handed the consumer's register but rcx,
// wanted_left(); then the count, shift_count(), held, since the
// instruction takes no operand in memory, outside held_out(); then
// shift(), shll %cl, %eax, sarq for >> of a long, which the machine
// masks to the width (G3).  The count is of its own type, which its view
// says.  The shifted value's register holds the node's value.
template< binary_op_t op, view_t V, typename L, typename R >
    requires shift_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    const unsigned own = pins(op, constant< R >);
    const unsigned named = wanted_left(s, wanted, own, b.rhs);

    const value_t l =
        left_operand(s, b.lhs, b.rhs, own | pinned_by(b.rhs), named);
    const value_t r = shift_count(s, b.rhs, held_out(s, l, named));

    shift(s, op, l, r, V, view_of(b.rhs));

    return l;
}

// A constant count is the instruction's immediate, count_of(), and pins
// nothing: shll $33, %eax, shlq $137, %rax, and shll $255, %eax for a
// << -1.
template< binary_op_t op, view_t V, typename L, constant C >
    requires shift_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, C > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    operate(s, op, reg_of(s, l), count_of(value_of(b.rhs)), V);

    return l;
}

// cmp of a source and the held value l at the view, rcc's
// compare_with(): a held value r, cmpl %ecx, %eax, the registers of
// operands(), a spilled value reloaded, and r released; the object in a
// slot, cmpq -16(%rbp), %rax; or an immediate, cmpl $5, %eax.  It sets
// the flags and writes no register; l stays held.
void compare(state_t &, value_t l, value_t r, view_t view);
void compare(state_t &, value_t l, slot_t source, view_t view);
void compare(state_t &, value_t l, std::string_view immediate, view_t view);

// cmp of an immediate and the object in a slot at the view, rcc's
// condition_on(): the object compared in place, cmpl $5, -4(%rbp), and
// no register holds it.
void compare(state_t &, slot_t where, std::string_view immediate, view_t view);

// The value of a comparison of op from the flags its cmp set, rcc's
// comparison() after compared(): setcc into the low byte of a register,
// then movzbl to the int.  l is the value the cmp held, none where it
// compared an object in place.
value_t
setcc(state_t &, binary_op_t op, std::optional< value_t > l, unsigned wanted);

// The cmp of the held left value l and the right operand of a
// comparison at the view V, rcc's compare_with(), by the right
// operand's class: its value, generated as for any consumer, held and
// freed, cmpl %ecx, %eax, a wide_t's by movabsq; a load, the
// instruction's operand in memory, cmpl -8(%rbp), %eax, which takes no
// register; an immediate, cmpl $5, %eax.
template< view_t V, typename R >
void
compare_with(state_t &s, value_t l, const R &rhs)
{
    compare(s, l, value(s, rhs), V);
}

template< view_t V, sign_t S >
void
compare_with(state_t &s, value_t l, const load_t< V, S > &rhs)
{
    compare(s, l, rhs.where, V);
}

template< view_t V, immediate I >
void
compare_with(state_t &s, value_t l, const I &rhs)
{
    compare(s, l, immediate_of(rhs), V);
}

// The primary of < > <= >= == !=, 6.5.9, 6.5.10, rcc's comparison(),
// compared() and compare(), chapter 6.3: the left operand's value
// first, G13, generated outside the registers the right operand's top
// operator pins; then the cmp with the right operand at the operation's
// view, compare_with(); then the value of the flags, setcc(), in the
// left operand's register, cmpl -8(%rbp), %eax, setl %al and movzbl
// %al, %eax.
template< binary_op_t op, view_t V, typename L, typename R >
    requires comparison_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    const value_t l = rvalue_avoiding(s, b.lhs, pinned_by(b.rhs));
    compare_with< V >(s, l, b.rhs);

    return setcc(s, op, l, wanted);
}

// A load compared with an immediate is the cmp of the object in place,
// cmpl $5, -4(%rbp), compare(); the object is read by the cmp and by
// nothing before it, G13, and no register holds it, so the setcc takes
// one.
template< binary_op_t op, view_t V, immediate I, sign_t S >
    requires comparison_operator< op >
value_t
value(state_t &s,
      const binary_t< op, V, load_t< V, S >, I > &b,
      unsigned wanted = 0)
{
    compare(s, b.lhs.where, immediate_of(b.rhs), V);

    return setcc(s, op, std::nullopt, wanted);
}

// A constant on the left is the row of the node mirrored, rcc's
// ordered(): 5 < a as a > 5, mirrored(), the constant the immediate on
// the right, so 5 < a is cmpl $5, -4(%rbp) and setg %al.
template< binary_op_t op, view_t V, immediate I, typename R >
    requires comparison_operator< op >
value_t
value(state_t &s, const binary_t< op, V, I, R > &b, unsigned wanted = 0)
{
    return value(s, binary_t< mirrored(op), V, R, I > { b.rhs, b.lhs }, wanted);
}

// !e, 6.5.4.3p5, rcc's unary(): the operand's value, its test against
// 0 at the operand's view, cmpl $0, %eax, then the value of the flags
// as e == 0 has it, sete and movzbl, setcc().  The operand is loaded,
// where e == 0 compares a load in place.
template< view_t V, typename A >
value_t
value(state_t &s,
      const unary_t< unary_op_t::log_not, V, A > &u,
      unsigned wanted = 0)
{
    const value_t v = value(s, u.arg);
    compare(s, v, imm(0), V);

    return setcc(s, binary_op_t::eq, v, wanted);
}

// A comparison as a jump, rcc's comparison_branch(), 7.1: the cmp of
// the value row, compare_with(), then jcc of the operator where sense
// and of its inverse otherwise, jl or jge, and no value held.
template< binary_op_t op, view_t V, typename L, typename R >
    requires comparison_operator< op >
void
branch(state_t &s, const binary_t< op, V, L, R > &b, int label, bool sense)
{
    const value_t l = rvalue_avoiding(s, b.lhs, pinned_by(b.rhs));
    compare_with< V >(s, l, b.rhs);
    release(s, l);

    jump_if(s, sense ? op : inverse(op), label);
}

// e == 0 and e != 0, rcc's zero_compared(): the jump of 7.1 reads ZF
// alone, so e == 0 is e's own jump on its value compared with 0, the
// sense flipped, and e != 0 that jump as it is, value_branch(), where
// e's row may set ZF and write no cmp: (a + b) == 0 is addl and jne.
// It wins over the cmp of a load in place, below, whose text it writes
// for a == 0, cmpl $0, -4(%rbp).
template< binary_op_t op, view_t V, typename R >
concept zero_compared = comparison_operator< op > &&
                        (op == binary_op_t::eq || op == binary_op_t::ne) &&
                        std::same_as< R, zero_t< V > >;

template< binary_op_t op, view_t V, typename L, typename R >
    requires zero_compared< op, V, R >
void
branch(state_t &s, const binary_t< op, V, L, R > &b, int label, bool sense)
{
    value_branch(s, b.lhs, label, sense != (op == binary_op_t::eq));
}

// A load against an immediate is the cmp of the object in place, as
// for its value, cmpl $5, -4(%rbp), and no register is held.
template< binary_op_t op, view_t V, immediate I, sign_t S >
    requires comparison_operator< op > && (!zero_compared< op, V, I >)
void
branch(state_t &s,
       const binary_t< op, V, load_t< V, S >, I > &b,
       int label,
       bool sense)
{
    compare(s, b.lhs.where, immediate_of(b.rhs), V);
    jump_if(s, sense ? op : inverse(op), label);
}

// A constant on the left is the jump of the node mirrored, ordered():
// if (5 < a) is cmpl $5, -4(%rbp) and jle.
template< binary_op_t op, view_t V, immediate I, typename R >
    requires comparison_operator< op >
void
branch(state_t &s, const binary_t< op, V, I, R > &b, int label, bool sense)
{
    branch(s, binary_t< mirrored(op), V, R, I > { b.rhs, b.lhs }, label, sense);
}

// The jump on a value compared with 0, rcc's test_zero(), 7.1: the
// value held, then the jump on its register at the value's view, cmpl
// $0, %eax and je, and the value released.  A constant is no special
// case: if (1) loads 1 and tests it.
template< typename T >
void
value_branch(state_t &s, const T &t, int label, bool sense)
{
    const value_t v = value(s, t);
    test_zero(s, v, view_of(t), label, sense);
    release(s, v);
}

// A load's value compared with 0 is the cmp of the object in place at
// its view, rcc's condition_on(), cmpl $0, -4(%rbp), cmpq $0, -8(%rbp),
// then jne or je: if (a) and a == 0 alike.  A cast to its own type over
// a load is the object read as it is, so if ((int) i) is the same cmp.
void
in_place_branch(state_t &, slot_t where, view_t view, int label, bool sense);

template< view_t V, sign_t S >
void
value_branch(state_t &s, const load_t< V, S > &l, int label, bool sense)
{
    in_place_branch(s, l.where, V, label, sense);
}

template< view_t V, sign_t S >
void
value_branch(state_t &s,
             const trunc_t< V, load_t< V, S > > &t,
             int label,
             bool sense)
{
    in_place_branch(s, t.arg.where, V, label, sense);
}

// Whether the instruction of op sets ZF from the value it computes,
// rcc's flag_operator(): add, sub, and, or and xor, by Intel's
// reference (7.1); imul, the shifts and idiv do not.
constexpr bool
flag_operator(binary_op_t op)
{
    return op == binary_op_t::add || op == binary_op_t::sub ||
           op == binary_op_t::bit_and || op == binary_op_t::bit_or ||
           op == binary_op_t::bit_xor;
}

// rcc's flag_branch() on the held value v of a flag condition of the
// view: je or jne on the flags of the row's own instruction, with the
// cmp with 0 first where the record says the last instruction set no
// ZF from v at that width; v is released.
void flag_branch(state_t &, value_t v, view_t view, int label, bool sense);

// The niches of a flag condition, rcc's flag_condition(), 7.1, after
// the Seventh Edition compiler's cctab: + - | ^ and unary -, whose
// row's instruction sets ZF from the value, its value's row, then the
// jump on those flags, flag_branch(): addl -8(%rbp), %eax and je,
// negl %eax and jne.  & is test, below.
template< binary_op_t op, view_t V, typename L, typename R >
    requires(flag_operator(op))
void
value_branch(state_t &s,
             const binary_t< op, V, L, R > &b,
             int label,
             bool sense)
{
    flag_branch(s, value(s, b), V, label, sense);
}

template< view_t V, typename A >
void
value_branch(state_t &s,
             const unary_t< unary_op_t::neg, V, A > &u,
             int label,
             bool sense)
{
    flag_branch(s, value(s, u), V, label, sense);
}

// test of a source and the held value l at the view, rcc's test(),
// which sets ZF from their & and writes no register: a held value r,
// testl %ecx, %eax, the registers of operands(), a spilled value
// reloaded, and r released; the object in a slot, testl -8(%rbp), %eax;
// or an immediate, testl $1, %eax; and of an immediate and the object in
// a slot, testl $1, -4(%rbp), the object tested in place.  l stays
// held.
void test(state_t &, value_t l, value_t r, view_t view);
void test(state_t &, value_t l, slot_t source, view_t view);
void test(state_t &, value_t l, std::string_view immediate, view_t view);
void test(state_t &, slot_t where, std::string_view immediate, view_t view);

// The test of the held left value l and the right operand of & at the
// view V, by the right operand's class, as compare_with() takes a
// comparison's.
template< view_t V, typename R >
void
test_with(state_t &s, value_t l, const R &rhs)
{
    test(s, l, value(s, rhs), V);
}

template< view_t V, sign_t S >
void
test_with(state_t &s, value_t l, const load_t< V, S > &rhs)
{
    test(s, l, rhs.where, V);
}

template< view_t V, immediate I >
void
test_with(state_t &s, value_t l, const I &rhs)
{
    test(s, l, immediate_of(rhs), V);
}

// & as a jump, rcc's test(), 7.1, the PDP-11's bit of cctab's cc81: the
// left operand's value, generated outside the registers the right
// operand's top operator pins, then test R, L, which sets ZF and writes
// no result, so nothing is held; then je or jne.
template< view_t V, typename L, typename R >
void
value_branch(state_t &s,
             const binary_t< binary_op_t::bit_and, V, L, R > &b,
             int label,
             bool sense)
{
    const value_t l = rvalue_avoiding(s, b.lhs, pinned_by(b.rhs));
    test_with< V >(s, l, b.rhs);
    release(s, l);

    jump_if(s, sense ? binary_op_t::ne : binary_op_t::eq, label);
}

// A load of the operation's view and an immediate are tested in place,
// testl $1, -4(%rbp).
template< view_t V, immediate I, sign_t S >
void
value_branch(state_t &s,
             const binary_t< binary_op_t::bit_and, V, load_t< V, S >, I > &b,
             int label,
             bool sense)
{
    test(s, b.lhs.where, immediate_of(b.rhs), V);
    jump_if(s, sense ? binary_op_t::ne : binary_op_t::eq, label);
}

// A constant on the left is the test of the node with its operands
// exchanged, ordered(): 1 & a is testl $1, -4(%rbp).
template< view_t V, immediate I, typename R >
void
value_branch(state_t &s,
             const binary_t< binary_op_t::bit_and, V, I, R > &b,
             int label,
             bool sense)
{
    value_branch(s, binary_t< binary_op_t::bit_and, V, R, I > { b.rhs, b.lhs },
                 label, sense);
}

// The extension of the held value v of the view from to the view to by
// the sign of its type, rcc's extended() past its test of the width
// rule, which the widening's row asks: movslq %eax, %rax or movl %eax,
// %eax on v's own register, or into the register the consumer names,
// wanted, where it is free and v is elsewhere, movslq %edx, %rax for an
// int remainder returned as a long (6.1).
value_t extended(
    state_t &, value_t v, view_t from, sign_t sign, view_t to, unsigned wanted);

// The extending load, rcc's extending_load(), backend.md 6.3: the object
// of the view from in its slot, loaded extended by its sign to 64 bits,
// movslq -4(%rbp), %rax.  It reads the bytes the load at its own view
// reads, at the object's width (G10), and leaves the value valid to 64.
value_t extending_load(state_t &, slot_t where, view_t from);

// A signed object widened to 64 bits is its extending load, the niche
// of the conversion over a load (docs/typed-tree.md 4.2).  The rule is
// the Seventh Edition compiler's: match(), in c10.c, tries a row again
// with the conversion dropped, and its tables convert an operand in
// memory in the instruction that reads it.
template< view_t F >
value_t
value(state_t &s,
      const sext_t< view_t::i64, F, load_t< F, sign_t::signed_ > > &e,
      unsigned = 0)
{
    return extending_load(s, e.arg.where, F);
}

// The arguments of a call moved from their slots to where the
// convention passes them, and the call of the function named, rcc's
// call() after the arguments' stores, chapter 8.
void transfer(state_t &,
              std::string_view name,
              const std::vector< argument_t > &args);

// A call of a function whose result is of the view V, 6.5.3.3, rcc's
// call() and rvalue(), chapter 8: the call keeps its own pin, since its
// result is in rax by the convention, so it is generated outside any
// reservation around it, as a division is, and its arguments find no
// register left for last; its value is held in rax, where the call
// leaves it.
template< view_t V >
value_t
value(state_t &s, const call_t< V > &c, unsigned = 0)
{
    const unsigned outer = std::exchange(s.reserved, 0u);

    call(s, c.name, c.args);
    const value_t v = hold(s, rax);

    s.reserved = outer;

    return v;
}

// The value of a return, of the result's view, moved to the result
// register where its row left it elsewhere, then the jump to the
// epilogue, rcc's return_stmt().
void return_stmt(state_t &, const expr_t &e, view_t view);

template< view_t V >
void
generate(state_t &s, const return_t< V > &r)
{
    return_stmt(s, r.value, V);
}

// The unit's assembly; file is the source's name, for the .file
// directive.
void generate(const translation_unit_t &,
              std::string_view file,
              std::ostream &assembly);

} // namespace rcc::amd64
