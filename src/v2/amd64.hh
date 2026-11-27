// -*- mode: c++; -*-

// x86-64, docs/backend.md: the registers, the state of a function's
// generation, the rows of the zoo's types this target writes itself,
// and the unit's entry.  The rows are declared here for build.cc, whose
// models call them, and defined in amd64.cc, which reaches the tree
// only through a handle's row.

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
// path reaches what is written next: the last instruction written is
// an unconditional jump, which jump() sets and place_label() clears,
// so an if whose then part ends so writes no jump over its else part,
// and main writes no return of 0 (backend.md 7.2).  zero_flag says
// that the last instruction written set ZF from the value it computed,
// flagged()'s, until the next instruction or label: a jump on that
// value then needs no cmp (7.1).  loops is the labels each loop
// generated so far took for its body's jumps, by the loop's number.
// frame is the bytes the slots take below rbp, the locals' and the
// spills' in the order they are taken, and slots each local's
// displacement off rbp, by its number.  reserved is the registers
// take() leaves for last while a value meant for a pinned register is
// generated, so it lands there (chapter 6.5).  ahead is the registers
// a pinned instruction generated after the value now being generated
// would move a held value out of, which the instruction on a spilled
// value keeps its destination out of (right_written()).
struct state_t
{
    std::ostringstream body { };
    std::string function;
    int labels = 0;
    int ret = 0;
    bool terminated = false;
    bool zero_flag = false;
    std::unordered_map< int, loop_labels_t > loops { };

    long frame = 0;
    std::unordered_map< int, int > slots { };

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

// The jump on the held value v compared with 0, rcc's test_zero():
// cmpl $0, %eax, then jne where sense, je otherwise; v stays held.
void test_zero(state_t &, value_t v, int label, bool sense);

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

// The constant rows of an int: its 32 bits in a register taken for it.
// Every value row of x86-64 is handed wanted, the registers the
// consumer of the value names, a set of at most one, rax for a
// return's (6.1): a hint, which a row is free not to honour
// (docs/typed-tree.md 3.4), none where the consumer names none.  A
// leaf takes the lowest free register whatever the consumer names, as
// rcc's literal() and load() do.
value_t value(state_t &, const literal_t &, unsigned wanted = 0);
value_t value(state_t &, const zero_t &, unsigned wanted = 0);

// The load row: the int a slot holds, in a register taken for it.
value_t value(state_t &, const load_t &, unsigned wanted = 0);

// - or ~ on r in place, at 32 bits.
void operate(state_t &, unary_op_t op, reg_t r);

// - and ~, 6.5.4.3p3, p4, rcc's unary(): the operand's value, handed the
// consumer's register, then the operation on its register in place,
// negl %eax, which leaves the node's value there.  ! is a row of its
// own, below.
template< unary_op_t op, typename A >
    requires(op != unary_op_t::log_not)
value_t
value(state_t &s, const unary_t< op, A > &u, unsigned wanted = 0)
{
    const value_t v = value(s, u.arg, wanted);
    operate(s, op, reg_of(s, v));

    return v;
}

// The operation of op into r from an operand at 32 bits: a register,
// the int in a slot, or an operand as its text, an immediate as
// immediate_of() writes it or a spilled value's slot.
void operate(state_t &, binary_op_t op, reg_t r, reg_t source);
void operate(state_t &, binary_op_t op, reg_t r, slot_t source);
void operate(state_t &, binary_op_t op, reg_t r, std::string_view source);

// The operation of op on the held values l and r, rcc's arithmetic():
// its value is in l's register, or in r's where op commutes and that is
// the one wanted names, or where l is spilled and is read in its slot;
// the other value is consumed.
value_t
arithmetic(state_t &, binary_op_t op, value_t l, value_t r, unsigned wanted);

// The three-operand imul at 32 bits, rcc's object_product(): the
// source, a register or the int in a slot, times the immediate factor
// into r.
void multiply(state_t &, reg_t r, reg_t source, std::string_view factor);
void multiply(state_t &, reg_t r, slot_t source, std::string_view factor);

// The division of op, / or %, of the held dividend l by the divisor, a
// held value r, which is consumed, or the int in a slot, at 32 bits:
// the quotient or the remainder, held where idivl leaves it or in l's
// register.
value_t
divide(state_t &, binary_op_t op, value_t l, value_t r, unsigned wanted);
value_t
divide(state_t &, binary_op_t op, value_t l, slot_t divisor, unsigned wanted);

// The store rows, an operand into a slot at 32 bits: a register, or an
// immediate as immediate_of() writes it.
void store(state_t &, reg_t r, slot_t where);
void store(state_t &, std::string_view immediate, slot_t where);

// A constant as the immediate operand of an instruction, 32 bits.
std::string immediate_of(const literal_t &);
std::string immediate_of(const zero_t &);

// A shift's constant count as its instruction's immediate: the low 8
// bits of the count, which is what the instruction encodes.
std::string count_of(int count);

// The instruction of a shift of the value l by the count held in r, at
// 32 bits, shll %cl, %eax, with what is in the way moved first; r is
// consumed.
void shift(state_t &, binary_op_t op, value_t l, value_t r);

// A value an instruction takes as its immediate: a type immediate_of()
// is written for (principle 7).
template< typename V >
concept immediate = requires(const V &e) { immediate_of(e); };

// The niche of initialize(), chapter 6.6: an immediate is stored by
// one mov into the slot, and no register is held.
template< immediate V >
void
initialize(state_t &s, slot_t where, const V &e)
{
    store(s, immediate_of(e), where);
}

// The operators of rcc's arithmetic_mnemonic(), + - * & | ^, whose
// instruction is op R, L, R a register, an immediate or an operand in
// memory.  The divisions and the shifts are rows of their own, below:
// idivl takes no immediate, and a shift no operand in memory and an
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

// The registers the instruction of op pins, chapter 6.5, rcc's pins():
// rax and rdx for / and %, rcx for << and >> by a count that is no
// constant, which is the immediate; constant_right says whether the
// right operand is a constant.
constexpr unsigned
pins(binary_op_t op, bool constant_right)
{
    if (is_division(op))
        return bit(rax) | bit(rdx);

    return is_shift(op) && !constant_right ? bit(rcx) : 0;
}

// The registers a node holds out of the choice for its left operand's
// value while it generates its right operand, rcc's pinned_by(): those
// the right operand's top operator pins, a fact of its class, top_t.
// The value then does not stand where that instruction wants its
// operands.  Any other class pins nothing, an erased one too, since the
// builder keeps a node that pins as a top_t; a division or a shift
// deeper in the right operand changes nothing.
template< typename R >
constexpr unsigned
pinned_by(const R &)
{
    return 0;
}

template< binary_op_t op >
constexpr unsigned
pinned_by(const top_t< op > &)
{
    return pins(op, false);
}

// The registers the instruction of a node or of any operand within it
// pins, rcc's pinned_within(): pins() over the whole tree, a summary
// each node makes of its operands' and an erased one hands up through
// its handle (principle 3).  A value held while the node is generated
// may have to move out of one of them, as out of rcx for a shift's
// count, or out of rax and rdx for a division, deeper in a right
// operand.  A leaf pins nothing.
template< constant C >
constexpr unsigned
pinned_within(const state_t &, const C &)
{
    return 0;
}

constexpr unsigned
pinned_within(const state_t &, const load_t &)
{
    return 0;
}

constexpr unsigned
pinned_within(const state_t &, const slot_t &)
{
    return 0;
}

template< unary_op_t op, typename A >
unsigned
pinned_within(const state_t &s, const unary_t< op, A > &u)
{
    return pinned_within(s, u.arg);
}

template< binary_op_t op, typename L, typename R >
unsigned
pinned_within(const state_t &s, const binary_t< op, L, R > &b)
{
    return pins(op, constant< R >) | pinned_within(s, b.lhs) |
           pinned_within(s, b.rhs);
}

template< binary_op_t op >
unsigned
pinned_within(const state_t &s, const top_t< op > &t)
{
    return pinned_within(s, t.node);
}

template< typename V >
unsigned
pinned_within(const state_t &s, const assign_t< V > &a)
{
    return pinned_within(s, a.value);
}

// The registers a pinned instruction within a node would move a value
// held across the node out of, rcc's moved_within(), 6.5: pins() over
// the tree, as pinned_within(), but nothing below && and ||, which
// spill every held value before their paths, before_paths(), so that
// no value is held in a register where their operands' instructions
// pin one.  A summary of its own, each node's of its operands', which
// an erased one hands up through its handle.
template< constant C >
constexpr unsigned
moved_within(const state_t &, const C &)
{
    return 0;
}

constexpr unsigned
moved_within(const state_t &, const load_t &)
{
    return 0;
}

constexpr unsigned
moved_within(const state_t &, const slot_t &)
{
    return 0;
}

template< unary_op_t op, typename A >
unsigned
moved_within(const state_t &s, const unary_t< op, A > &u)
{
    return moved_within(s, u.arg);
}

template< binary_op_t op, typename L, typename R >
unsigned
moved_within(const state_t &s, const binary_t< op, L, R > &b)
{
    return pins(op, constant< R >) | moved_within(s, b.lhs) |
           moved_within(s, b.rhs);
}

template< binary_op_t op, typename L, typename R >
    requires logical_operator< op >
constexpr unsigned
moved_within(const state_t &, const binary_t< op, L, R > &)
{
    return 0;
}

template< binary_op_t op >
unsigned
moved_within(const state_t &s, const top_t< op > &t)
{
    return moved_within(s, t.node);
}

template< typename V >
unsigned
moved_within(const state_t &s, const assign_t< V > &a)
{
    return moved_within(s, a.value);
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
// value is held while rhs is generated, so a row within it that
// chooses its result's register, right_written(), chooses none of
// them.
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
// the ecx a count's consumer names.
template< binary_op_t op, typename L, typename R >
    requires arithmetic_operator< op >
value_t
value(state_t &s, const binary_t< op, L, R > &b, unsigned wanted = 0)
{
    const unsigned named = wanted_left(s, wanted, 0, b.rhs);

    const value_t l = left_operand(s, b.lhs, b.rhs, pinned_by(b.rhs), named);
    const value_t r = rvalue_avoiding(s, b.rhs, held_out(s, l, named));

    return arithmetic(s, op, l, r, wanted);
}

// Whether a node with the constant L on the left of op is written with
// its operands exchanged, rcc's constant_first() as ordered() asks it:
// op commutes, and the constant is an immediate, as every int is.
template< binary_op_t op, typename L >
concept constant_first = commutes(op) && immediate< L >;

// The niches of the primary, chapter 6.1.  A constant on the left of an
// operator that commutes is the row of the node with its operands
// exchanged, 5 + a as a + 5: the value of the right operand, which is
// no constant, then the constant as the immediate.
template< binary_op_t op, typename L, typename R >
    requires arithmetic_operator< op > && constant_first< op, L >
value_t
value(state_t &s, const binary_t< op, L, R > &b, unsigned wanted = 0)
{
    return value(s, binary_t< op, R, L > { b.rhs, b.lhs }, wanted);
}

// A load on the right is the instruction's operand in memory, addl
// -8(%rbp), %eax, and takes no register.  Its type is the more special,
// so the node the mirror takes, 5 + a, is excluded by name; 5 - a is
// this row, movl $5, %eax and subl -4(%rbp), %eax.  The right operand
// pins nothing, so the left operand is handed the consumer's register,
// as in every niche of an operand in memory or an immediate.
template< binary_op_t op, typename L >
    requires(arithmetic_operator< op > && !constant_first< op, L >)
value_t
value(state_t &s, const binary_t< op, L, load_t > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    operate(s, op, reg_of(s, l), b.rhs.where);

    return l;
}

// An immediate on the right is the instruction's, negative ones as
// written: addl $5, %eax, andl $-2, %eax.  Of the mirror's nodes it
// would take only a constant on both sides, which the builder folds.
template< binary_op_t op, typename L, immediate I >
    requires arithmetic_operator< op >
value_t
value(state_t &s, const binary_t< op, L, I > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    operate(s, op, reg_of(s, l), immediate_of(b.rhs));

    return l;
}

// imul by an immediate is the three-operand form, rcc's arithmetic():
// the left operand's value, its register the source and the
// destination, imull $5, %eax, %eax.
template< typename L, immediate I >
value_t
value(state_t &s,
      const binary_t< binary_op_t::mul, L, I > &b,
      unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    multiply(s, reg_of(s, l), reg_of(s, l), immediate_of(b.rhs));

    return l;
}

// A load on the left of imul by an immediate is the source in memory,
// rcc's product(): imull $5, -4(%rbp), %eax, no load, and the product
// in a register taken for it, the lowest free whatever the consumer
// names.  5 * a is this row, through the mirror.
template< immediate I >
value_t
value(state_t &s,
      const binary_t< binary_op_t::mul, load_t, I > &b,
      unsigned = 0)
{
    const value_t v = fresh(s);
    multiply(s, reg_of(s, v), b.lhs.where, immediate_of(b.rhs));

    return v;
}

// / and % by a divisor that is no local, 6.5.6, rcc's binary() and
// divide(), chapter 6.5: idivl takes its dividend in rax, and writes
// the quotient there and the remainder in rdx.  A division keeps its
// own pin, rcc's rvalue(): it is generated outside any reservation
// around it, since the pin, not the outer wish, decides where its
// result is written.  The dividend first, G13, outside what the
// divisor's top operator pins and handed the consumer's register,
// wanted_left(); then the divisor, held outside rax and rdx and
// held_out(), a constant too, since idivl takes no immediate, movl $5,
// %ecx; then divide().
template< binary_op_t op, typename L, typename R >
    requires division_operator< op >
value_t
value(state_t &s, const binary_t< op, L, R > &b, unsigned wanted = 0)
{
    const unsigned outer = std::exchange(s.reserved, 0u);
    const unsigned named = wanted_left(s, wanted, 0, b.rhs);

    const value_t l = left_operand(s, b.lhs, b.rhs, pinned_by(b.rhs), named);
    const value_t r = rvalue_avoiding(
        s, b.rhs, held_out(s, l, named) | pins(op, constant< R >));
    const value_t v = divide(s, op, l, r, wanted);

    s.reserved = outer;

    return v;
}

// A local as the divisor is idivl's operand in memory, idivl -8(%rbp),
// and takes no register; it pins nothing, so the dividend is handed the
// consumer's register.
template< binary_op_t op, typename L >
    requires division_operator< op >
value_t
value(state_t &s, const binary_t< op, L, load_t > &b, unsigned wanted = 0)
{
    const unsigned outer = std::exchange(s.reserved, 0u);

    const value_t l = value(s, b.lhs, wanted);
    const value_t v = divide(s, op, l, b.rhs.where, wanted);

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
// shift(), shll %cl, %eax, sarl for >>, which the machine masks to 5
// bits (G3).  The shifted value's register holds the node's value.
template< binary_op_t op, typename L, typename R >
    requires shift_operator< op >
value_t
value(state_t &s, const binary_t< op, L, R > &b, unsigned wanted = 0)
{
    const unsigned own = pins(op, constant< R >);
    const unsigned named = wanted_left(s, wanted, own, b.rhs);

    const value_t l =
        left_operand(s, b.lhs, b.rhs, own | pinned_by(b.rhs), named);
    const value_t r = shift_count(s, b.rhs, held_out(s, l, named));

    shift(s, op, l, r);

    return l;
}

// A constant count is the instruction's immediate, count_of(), and pins
// nothing: shll $33, %eax, and shll $255, %eax for a << -1.
template< binary_op_t op, typename L, constant C >
    requires shift_operator< op >
value_t
value(state_t &s, const binary_t< op, L, C > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    operate(s, op, reg_of(s, l), count_of(value_of(b.rhs)));

    return l;
}

// cmp of a source and the held value l at 32 bits, rcc's
// compare_with(): a held value r, cmpl %ecx, %eax, the registers of
// operands(), a spilled value reloaded, and r released; the int in a
// slot, cmpl -8(%rbp), %eax; or an immediate, cmpl $5, %eax.  It sets
// the flags and writes no register; l stays held.
void compare(state_t &, value_t l, value_t r);
void compare(state_t &, value_t l, slot_t source);
void compare(state_t &, value_t l, std::string_view immediate);

// cmp of an immediate and the int in a slot, rcc's condition_on(): the
// object compared in place, cmpl $5, -4(%rbp), and no register holds
// it.
void compare(state_t &, slot_t where, std::string_view immediate);

// The value of a comparison of op from the flags its cmp set, rcc's
// comparison() after compared(): setcc into the low byte of a register,
// then movzbl to the int.  l is the value the cmp held, none where it
// compared an object in place.
value_t
setcc(state_t &, binary_op_t op, std::optional< value_t > l, unsigned wanted);

// The cmp of the held left value l and the right operand of a
// comparison, rcc's compare_with(), by the right operand's class: its
// value, generated as for any consumer, held and freed, cmpl %ecx,
// %eax; a load, the instruction's operand in memory, cmpl -8(%rbp),
// %eax, which takes no register; an immediate, cmpl $5, %eax.
template< typename R >
void
compare_with(state_t &s, value_t l, const R &rhs)
{
    compare(s, l, value(s, rhs));
}

inline void
compare_with(state_t &s, value_t l, const load_t &rhs)
{
    compare(s, l, rhs.where);
}

template< immediate I >
void
compare_with(state_t &s, value_t l, const I &rhs)
{
    compare(s, l, immediate_of(rhs));
}

// The primary of < > <= >= == !=, 6.5.9, 6.5.10, rcc's comparison(),
// compared() and compare(), chapter 6.3: the left operand's value
// first, G13, generated outside the registers the right operand's top
// operator pins; then the cmp with the right operand, compare_with();
// then the value of the flags, setcc(), in the left operand's register,
// cmpl -8(%rbp), %eax, setl %al and movzbl %al, %eax.
template< binary_op_t op, typename L, typename R >
    requires comparison_operator< op >
value_t
value(state_t &s, const binary_t< op, L, R > &b, unsigned wanted = 0)
{
    const value_t l = rvalue_avoiding(s, b.lhs, pinned_by(b.rhs));
    compare_with(s, l, b.rhs);

    return setcc(s, op, l, wanted);
}

// A load compared with an immediate is the cmp of the object in place,
// cmpl $5, -4(%rbp), compare(); the object is read by the cmp and by
// nothing before it, G13, and no register holds it, so the setcc takes
// one.
template< binary_op_t op, immediate I >
    requires comparison_operator< op >
value_t
value(state_t &s, const binary_t< op, load_t, I > &b, unsigned wanted = 0)
{
    compare(s, b.lhs.where, immediate_of(b.rhs));

    return setcc(s, op, std::nullopt, wanted);
}

// A constant on the left is the row of the node mirrored, rcc's
// ordered(): 5 < a as a > 5, mirrored(), the constant the immediate on
// the right, so 5 < a is cmpl $5, -4(%rbp) and setg %al.
template< binary_op_t op, immediate I, typename R >
    requires comparison_operator< op >
value_t
value(state_t &s, const binary_t< op, I, R > &b, unsigned wanted = 0)
{
    return value(s, binary_t< mirrored(op), R, I > { b.rhs, b.lhs }, wanted);
}

// !e, 6.5.4.3p5, rcc's unary(): the operand's value, its test against
// 0, cmpl $0, %eax, then the value of the flags as e == 0 has it,
// sete and movzbl, setcc().  The operand is loaded, where e == 0
// compares a load in place.
value_t
value(state_t &, const unary_t< unary_op_t::log_not > &, unsigned wanted = 0);

// A comparison as a jump, rcc's comparison_branch(), 7.1: the cmp of
// the value row, compare_with(), then jcc of the operator where sense
// and of its inverse otherwise, jl or jge, and no value held.
template< binary_op_t op, typename L, typename R >
    requires comparison_operator< op >
void
branch(state_t &s, const binary_t< op, L, R > &b, int label, bool sense)
{
    const value_t l = rvalue_avoiding(s, b.lhs, pinned_by(b.rhs));
    compare_with(s, l, b.rhs);
    release(s, l);

    jump_if(s, sense ? op : inverse(op), label);
}

// e == 0 and e != 0, rcc's zero_compared(): the jump of 7.1 reads ZF
// alone, so e == 0 is e's own jump on its value compared with 0, the
// sense flipped, and e != 0 that jump as it is, value_branch(), where
// e's row may set ZF and write no cmp: (a + b) == 0 is addl and jne.
// It wins over the cmp of a load in place, below, whose text it writes
// for a == 0, cmpl $0, -4(%rbp).
template< binary_op_t op, typename R >
concept zero_compared = comparison_operator< op > &&
                        (op == binary_op_t::eq || op == binary_op_t::ne) &&
                        std::same_as< R, zero_t >;

template< binary_op_t op, typename L, typename R >
    requires zero_compared< op, R >
void
branch(state_t &s, const binary_t< op, L, R > &b, int label, bool sense)
{
    value_branch(s, b.lhs, label, sense != (op == binary_op_t::eq));
}

// A load against an immediate is the cmp of the object in place, as
// for its value, cmpl $5, -4(%rbp), and no register is held.
template< binary_op_t op, immediate I >
    requires comparison_operator< op > && (!zero_compared< op, I >)
void
branch(state_t &s, const binary_t< op, load_t, I > &b, int label, bool sense)
{
    compare(s, b.lhs.where, immediate_of(b.rhs));
    jump_if(s, sense ? op : inverse(op), label);
}

// A constant on the left is the jump of the node mirrored, ordered():
// if (5 < a) is cmpl $5, -4(%rbp) and jle.
template< binary_op_t op, immediate I, typename R >
    requires comparison_operator< op >
void
branch(state_t &s, const binary_t< op, I, R > &b, int label, bool sense)
{
    branch(s, binary_t< mirrored(op), R, I > { b.rhs, b.lhs }, label, sense);
}

// A load's value compared with 0 is the cmp of the object in place,
// rcc's condition_on(), cmpl $0, -4(%rbp), then jne or je: if (a) and
// a == 0 alike.
void value_branch(state_t &, const load_t &, int label, bool sense);

// Whether the instruction of op sets ZF from the value it computes,
// rcc's flag_operator(): add, sub, and, or and xor, by Intel's
// reference (7.1); imul, the shifts and idivl do not.
constexpr bool
flag_operator(binary_op_t op)
{
    return op == binary_op_t::add || op == binary_op_t::sub ||
           op == binary_op_t::bit_and || op == binary_op_t::bit_or ||
           op == binary_op_t::bit_xor;
}

// rcc's flag_branch() on the held value v of a flag condition: je or
// jne on the flags of the row's own instruction, with the cmp with 0
// first where the record says the last instruction set no ZF from v;
// v is released.
void flag_branch(state_t &, value_t v, int label, bool sense);

// The niches of a flag condition, rcc's flag_condition(), 7.1, after
// the Seventh Edition compiler's cctab: + - | ^ and unary -, whose
// row's instruction sets ZF from the value, its value's row, then the
// jump on those flags, flag_branch(): addl -8(%rbp), %eax and je,
// negl %eax and jne.  & is test, below.
template< binary_op_t op, typename L, typename R >
    requires(flag_operator(op))
void
value_branch(state_t &s, const binary_t< op, L, R > &b, int label, bool sense)
{
    flag_branch(s, value(s, b), label, sense);
}

template< typename A >
void
value_branch(state_t &s,
             const unary_t< unary_op_t::neg, A > &u,
             int label,
             bool sense)
{
    flag_branch(s, value(s, u), label, sense);
}

// test of a source and the held value l at 32 bits, rcc's test(), which
// sets ZF from their & and writes no register: a held value r, testl
// %ecx, %eax, the registers of operands(), a spilled value reloaded,
// and r released; the int in a slot, testl -8(%rbp), %eax; or an
// immediate, testl $1, %eax; and of an immediate and the int in a slot,
// testl $1, -4(%rbp), the object tested in place.  l stays held.
void test(state_t &, value_t l, value_t r);
void test(state_t &, value_t l, slot_t source);
void test(state_t &, value_t l, std::string_view immediate);
void test(state_t &, slot_t where, std::string_view immediate);

// The test of the held left value l and the right operand of &, by the
// right operand's class, as compare_with() takes a comparison's.
template< typename R >
void
test_with(state_t &s, value_t l, const R &rhs)
{
    test(s, l, value(s, rhs));
}

inline void
test_with(state_t &s, value_t l, const load_t &rhs)
{
    test(s, l, rhs.where);
}

template< immediate I >
void
test_with(state_t &s, value_t l, const I &rhs)
{
    test(s, l, immediate_of(rhs));
}

// & as a jump, rcc's test(), 7.1, the PDP-11's bit of cctab's cc81: the
// left operand's value, generated outside the registers the right
// operand's top operator pins, then test R, L, which sets ZF and writes
// no result, so nothing is held; then je or jne.
template< typename L, typename R >
void
value_branch(state_t &s,
             const binary_t< binary_op_t::bit_and, L, R > &b,
             int label,
             bool sense)
{
    const value_t l = rvalue_avoiding(s, b.lhs, pinned_by(b.rhs));
    test_with(s, l, b.rhs);
    release(s, l);

    jump_if(s, sense ? binary_op_t::ne : binary_op_t::eq, label);
}

// A load and an immediate are tested in place, testl $1, -4(%rbp).
template< immediate I >
void
value_branch(state_t &s,
             const binary_t< binary_op_t::bit_and, load_t, I > &b,
             int label,
             bool sense)
{
    test(s, b.lhs.where, immediate_of(b.rhs));
    jump_if(s, sense ? binary_op_t::ne : binary_op_t::eq, label);
}

// A constant on the left is the test of the node with its operands
// exchanged, ordered(): 1 & a is testl $1, -4(%rbp).
template< immediate I, typename R >
void
value_branch(state_t &s,
             const binary_t< binary_op_t::bit_and, I, R > &b,
             int label,
             bool sense)
{
    value_branch(s, binary_t< binary_op_t::bit_and, R, I > { b.rhs, b.lhs },
                 label, sense);
}

// The value, moved to the result register where its row left it
// elsewhere, then the jump to the epilogue.
void generate(state_t &, const return_t &);

// The unit's assembly; file is the source's name, for the .file
// directive.
void generate(const translation_unit_t &,
              std::string_view file,
              std::ostream &assembly);

} // namespace rcc::amd64
