// -*- mode: c++; -*-

// aarch64, docs/aarch64.md: the registers, the state of a function's
// generation, the rows of the zoo's types this target writes itself,
// and the unit's entry.  The rows are declared here for build.cc, whose
// models call them, and those of a template defined here too; the rest
// are defined in arm64.cc, which reaches the tree only through a
// handle's row.  Every width a row writes is its node's view, handed to
// the text as data: the w name of a register at 32 bits, the x name at
// 64.

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
#include <unordered_set>
#include <utility>
#include <vector>

namespace rcc::arm64 {

// The general registers by number, chapter 2.1: the fifteen expression
// registers of chapter 4, the caller-saved set without x8, x0 the
// convention's result register.
enum reg_t : int {
    x0,
    x1,
    x2,
    x3,
    x4,
    x5,
    x6,
    x7,
    x9 = 9,
    x10,
    x11,
    x12,
    x13,
    x14,
    x15
};

// A register as a set of one, rcc's bit(): the sets are the registers
// a value is taken outside of, and those the consumer of a value names.
constexpr unsigned
bit(reg_t r)
{
    return 1u << r;
}

// The state of one function's generation, which the function's row
// builds fresh.  defined is the names of the objects the unit defines,
// defined_symbols()'s, which a global's address reads.  The body is a
// buffer, which the frame is written around; the function's name makes
// its labels, which are counted per function, and ret is the one
// epilogue's.  terminated says that no path reaches what is written
// next: the last instruction written is an unconditional jump, which
// jump() sets and place_label() clears, so an if whose then part ends
// so writes no jump over its else part, and main writes no return of 0
// (backend.md 7.2).  breaks is the label each loop and switch
// generated so far took for the breaks of its body, by its number, and
// continues each loop's for its continues; cases is the label of each
// case and default label of those switches, by the label's number, and
// user_labels the label of each C label that the generation has met so
// far, by the number the parser gave its name, and frame is the bytes
// from x29 to the end of the last slot, the locals' and the spills' in
// the order they are taken, the frame record's alone before the first,
// and slots each local's displacement off x29, by its number.  reserved
// is the registers take() leaves for last while an assignment's place
// is formed, the one its consumer names, so that the value takes it
// (6.1).
struct state_t
{
    const std::unordered_set< std::string > &defined;
    std::ostringstream body { };
    std::string function;
    int labels = 0;
    int ret = 0;
    bool terminated = false;
    std::unordered_map< int, int > breaks { };
    std::unordered_map< int, int > continues { };
    std::unordered_map< int, int > cases { };
    std::unordered_map< int, int > user_labels { };

    long frame = 0;
    std::unordered_map< int, int > slots { };

    registers_t< reg_t > registers { };
    unsigned reserved = 0;
};

// A jump to the function's label n; no path reaches what follows it.
void jump(state_t &, int n);

// A register for a new value, register-stack.hh's, taken outside avoid
// and, where it can, outside s.reserved; it spills a held value when
// every one is held.
reg_t take(state_t &, unsigned avoid = 0);

// The function's label n, where a jump may reach it from elsewhere: a
// path reaches what follows.
void place_label(state_t &, int n);

// The branch to label on the flags a cmp set, where op holds, rcc's
// jump_if(): b.lt, b.eq.
void jump_if(state_t &, binary_op_t op, int label);

// The test of the held value v against 0 at its view, valid to valid
// bits, rcc's test_zero(): with a label, the jump to it where v is
// nonzero, sense, or zero, cbnz w0 or cbz x0, one instruction and no
// flags; without one, the flags for the caller's cset, cmp w0, #0, the
// row of !.  A value valid to less than 32 bits, the value of a
// narrowing, has stale bits above its width, and the machine compares
// whole registers only: either way it is tst of the low bits alone,
// tst w0, #0xff, then b.ne or b.eq to the label.  v stays held.
void test_zero(state_t &,
               value_t v,
               view_t view,
               int valid,
               std::optional< int > label,
               bool sense = false);

// A mov of the immediate value into r, at 32 bits: mov w0, #1.
void move_immediate(state_t &, reg_t r, int value);

// The directive before a loop's head, rcc's align_loop_head(): none on
// aarch64, whose loop_alignment is 0.
void align_loop_head(state_t &);

// The next slot of the frame, of size bytes at align, a local's or a
// spill's: its displacement off x29.
int allocate(state_t &, long size, int align);

// The store of r to a spill slot, and the reload of a spilled value
// into r from its slot, register-stack.hh's.
void store_spill(state_t &, reg_t r, int displacement);
void reload_spill(state_t &, reg_t r, int displacement);

// The constant row, a constant of the view in a register taken for it,
// mov w0, #5 or mov x0, #5, and the forms of a value no one mov writes.
// Every value row of aarch64 is handed wanted, the registers the
// consumer of the value names, a set of at most one, x0 for a return's
// (6.1): a hint, which a row is free not to honour (docs/typed-tree.md
// 3.4), none where the consumer names none.  A leaf takes the lowest
// free register whatever the consumer names, as rcc's literal() and
// load() do.
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

// How an operand form reads its index register, rcc's extend_t, 2.3:
// the x register whole, or the w register extended to 64 bits by the
// form, signed or unsigned; either then shifted by the scale.
enum class extend_t { lsl, sxtw, uxtw };

// An object in memory as an access names it, rcc's address_t (6.3,
// chapter 10): a slot's displacement off x29, a global's label, which
// its access reaches through x16, or a base held in a register, the
// address the GOT held or a pointer's value, with a displacement or an
// index read by its extend and shifted by the stride's log; and, a form
// x86-64 lacks, a constant index not yet in a register, which the
// store of an assignment writes after its right operand, held_index()'s.
// The base and the index are held values, spilled and reloaded as any;
// the access consumes them.
struct address_t
{
    std::optional< value_t > base = std::nullopt;
    std::optional< value_t > index = std::nullopt;
    extend_t extend = extend_t::lsl;
    int shift = 0;
    int displacement = 0;
    std::string label = { };
    std::optional< long long > constant = std::nullopt;
};

// What the caller generates between an address and its access, rcc's
// between: nothing for a load; a store's right operand, or one that
// holds a call, calls_within()'s, which spills every held value.  It
// decides how an access through a sum holds its parts.
enum class between_t : int { nothing, operand, call };

// A slot's address, rcc's slot_address(): its displacement, and no
// instruction, whatever comes between it and its access.
address_t address(state_t &, slot_t where);
address_t address(state_t &, slot_t where, between_t);

// A global's, rcc's global_address(): its label where the unit defines
// it; otherwise the GOT's entry loaded into a register, adrp x0, :got:g
// and ldr x0, [x0, :got_lo12:g], rcc's symbol_address(), the base of
// the access, [x0].  The unit's answer is asked here, and no row asks
// it.
address_t address(state_t &, const global_t &where);
address_t address(state_t &, const global_t &where, between_t);

// The address of the object through a pointer, rcc's pointed() of the
// pointer, its entry point, with what comes between it and its access.
address_t address(state_t &, const indirect_t &where, between_t = { });

// The address of where formed with the registers in reserved left for
// last, rcc's address_avoiding(): a GOT's load takes its register
// outside them where it can.
template< typename W >
address_t
address_avoiding(state_t &s,
                 const W &where,
                 unsigned reserved,
                 between_t between = between_t::nothing)
{
    const unsigned outer = std::exchange(s.reserved, s.reserved | reserved);
    const address_t a = address(s, where, between);
    s.reserved = outer;

    return a;
}

// The place of an assignment, held across its value, rcc's assign():
// the address of where, formed outside the consumer's register, wanted,
// so that the value takes it: return g = 7 on an object only declared
// takes x1 for the GOT's base, outside the result's x0, with the value
// between it and its access, which may hold a call, calls_within()'s.  A
// slot's address and a label take no register, so a local's and a
// defined object's place is no reservation.
template< typename W >
address_t
place_of(state_t &s, const W &where, const expr_t &value, unsigned wanted)
{
    const between_t between =
        calls_within(s, value) ? between_t::call : between_t::operand;

    return address_avoiding(s, where, wanted, between);
}

// The registers an address's values are in, rcc's registers_of(), for
// a take or a reload to stay clear of: the base's and the index's, none
// where either is spilled or there is none.
unsigned registers_of(const state_t &, const address_t &);

// The address as an access's operand, rcc's operand(): [x29, #16], [x0],
// [x0, #12], [x0, w1, sxtw #2], a spilled index, then a spilled base,
// reloaded first outside avoid and the other's register; or a global's
// label, adrp x16, g written here, at the access, and [x16, :lo12:g]
// returned, so every caller forms the operand in a statement of its
// own, after every register of the access is in hand and right before
// the access: no spill or reload stands between the adrp and its
// access.  A constant index is held first, held_index()'s.
std::string operand(state_t &, const address_t &, unsigned avoid = 0);

// The address is consumed: its registers are free, rcc's release().
void release(state_t &, const address_t &);

// An address's constant index written after the store's right operand,
// rcc's held_index(), 6.3: the constant row of 6.4 into a register
// outside the address's and avoid, the operand's, read by the form, bl
// g; mov x1, #5000; str w0, [x2, x1, lsl #2]; any other address as it
// is.
address_t held_index(state_t &, address_t a, unsigned avoid);

// The load row, rcc's load(): the object at an address at its view,
// extended to 32 bits by its sign where it is narrow, in a register
// taken for it, ldrsb w0, [x29, #16], ldr w0, [x29, #16] or ldr x0,
// [x29, #16], ldr w0, [x16, :lo12:g] after adrp x16, g, or ldr w0, [x0]
// after the GOT's pair.
value_t load(state_t &, const address_t &, view_t view, sign_t sign);

template< typename W, view_t V, sign_t S >
value_t
value(state_t &s, const load_t< W, V, S > &l, unsigned = 0)
{
    return load(s, address(s, l.where), V, S);
}

// The address of a slot as a value, valid to 64, rcc's address_of():
// x29 plus its displacement, add x0, x29, #16, in a register taken for
// it.
value_t address_of(state_t &, const address_t &);

// &x, 6.5.4.2p3, rcc's address_value() of a name: the add of its
// where's address.
template< typename W >
value_t
value(state_t &s, const address_of_t< W > &a, unsigned = 0)
{
    return address_of(s, address(s, a.where));
}

// &*E is E's value, 6.5.4.2p3, rcc's address_value() of a dereference,
// handed the consumer's register.
inline value_t
value(state_t &s, const address_of_t< indirect_t > &a, unsigned wanted = 0)
{
    return value(s, a.where.pointer, wanted);
}

// - or ~ of the held value v at the view, into the register the
// consumer names where it may be written, or into v's own.
value_t
negated(state_t &, unary_op_t op, value_t v, view_t view, unsigned wanted);

// The operation of op on the held value l and the right operand at the
// view, rcc's arithmetic(): a held value r, which is consumed, or a
// constant's value, the instruction's immediate where it encodes one.
// Its value is l, in the register the consumer names where the
// instruction may write it, or in l's own.
value_t arithmetic(state_t &,
                   binary_op_t op,
                   value_t l,
                   value_t r,
                   view_t view,
                   unsigned wanted);
value_t arithmetic(state_t &,
                   binary_op_t op,
                   value_t l,
                   long long constant,
                   view_t view,
                   unsigned wanted);

// - and ~, 6.5.4.3p3, p4, rcc's unary(): the operand's value, generated
// as for any consumer, then neg or mvn, whose destination is apart from
// its source: neg w0, w0, and neg w0, w1 for a return whose operand is
// in w1.  ! is a row of its own, below.
template< unary_op_t op, view_t V, typename A >
    requires(op != unary_op_t::log_not)
value_t
value(state_t &s, const unary_t< op, V, A > &u, unsigned wanted = 0)
{
    return negated(s, op, value(s, u.arg), V, wanted);
}

// The load of the object at an address held for a store through it
// after the load, rcc's fetch(): the value in a register taken outside
// the address's, then the address's operand outside it, the object of
// the view extended by the sign to a register of to bits, load_of()'s,
// ldr w0, [x29, #16], ldr w0, [x1], and ldrsw x0, [x29, #16] for an int
// under an operation of 64 bits.  The address stays held.
value_t fetch(state_t &, const address_t &at, view_t view, sign_t sign, int to);

// The value of op= on the object at an address held across it, of the
// view and of a type of the sign, at the view of the operation, rcc's
// assign(), 6.4, which gives the address back, its right operand a
// constant or the held value r: a constant the instruction of op
// encodes at the operation's view is its immediate, and anything else a
// value held before the object's load, a constant too, mov w0, #5; then
// the object fetched into a fresh register, extended to the operation's
// view, the operation into the register the consumer names where it may
// be written, and the store of the value at the object's view: ldr w0,
// [x29, #16], add w0, w0, #5 and str w0; ldrb w1, [x29, #16], add x1,
// x1, x0 and strb w1 for c += l.
value_t compound(state_t &,
                 binary_op_t op,
                 const address_t &at,
                 view_t view,
                 sign_t sign,
                 view_t operation,
                 long long constant,
                 unsigned wanted);
value_t compound(state_t &,
                 binary_op_t op,
                 const address_t &at,
                 view_t view,
                 sign_t sign,
                 view_t operation,
                 value_t r,
                 unsigned wanted);

// A compound assignment, its value or its effect, the value dropped:
// the place first, its address formed outside the register the consumer
// names, then the right operand, rcc's right_operand(), the operation
// one evaluation after it.
template< binary_op_t op, typename W, view_t V, view_t O, typename R >
value_t
value(state_t &s, const compound_t< op, W, V, O, R > &c, unsigned wanted = 0)
{
    const address_t at = address_avoiding(s, c.where, wanted);
    const value_t r = value(s, c.value);

    return compound(s, op, at, V, c.sign, O, r, wanted);
}

template< binary_op_t op, typename W, view_t V, view_t O, constant C >
value_t
value(state_t &s, const compound_t< op, W, V, O, C > &c, unsigned wanted = 0)
{
    const address_t at = address_avoiding(s, c.where, wanted);

    return compound(s, op, at, V, c.sign, O, value_of(c.value), wanted);
}

// The value of a step of the object at an address, of the view and of
// a type of the sign, rcc's step(), 6.4, which gives the address back:
// the object fetched into a fresh register, then the add or sub of 1
// into that register for a prefix step, add w0, w0, #1, or into a fresh
// one outside the address's for a postfix step, add w1, w0, #1, whose
// old value stays where the load left it, 6.1; the store of the new
// value; the value the prefix's register or the postfix's old one.  No
// destination is the consumer's: rcc's step() names none.
value_t
step(state_t &, step_op_t op, const address_t &at, view_t view, sign_t sign);

// The place first, its address formed outside the registers the
// consumer names, rcc's address_avoiding().
template< step_op_t op, typename W, view_t V, sign_t S >
value_t
value(state_t &s, const step_t< op, W, V, S > &t, unsigned wanted = 0)
{
    return step(s, op, address_avoiding(s, t.where, wanted), V, S);
}

// A step for its effect, rcc's modify(): the prefix form's value
// dropped, since nothing reads the old one, ldr w0, add w0, w0, #1 and
// str w0 for a++.
template< step_op_t op, typename W, view_t V, sign_t S >
void
effect(state_t &s, const step_t< op, W, V, S > &t)
{
    constexpr step_op_t prefix =
        is_increment(op) ? step_op_t::pre_inc : step_op_t::pre_dec;

    release(s, step(s, prefix, address(s, t.where), V, S));
}

// The store row, r into an object in memory, to, its operand's text, at
// the view: str w0 or str x0; and of the held value v at an address
// held, rcc's store(), its constant index held after the value, the
// address the caller's to give back.
void store(state_t &, reg_t r, std::string_view to, view_t view);
void store(state_t &, value_t v, const address_t &at, view_t view);

// The niche of initialize(), 6.6: 0 is the zero register's store at the
// address its caller formed, str wzr, [x29, #16], adrp x16, g and str
// wzr, [x16, :lo12:g], and no register is held.
void store_zero(state_t &, const address_t &at, view_t view);

template< view_t V >
void
initialize(state_t &s, const address_t &at, const zero_t< V > &)
{
    store_zero(s, at, V);
}

// The operators of rcc's arithmetic(), * / % + - << >> & | ^, whose
// row writes the operation into the left operand's register; a
// comparison's cmp writes none, and its rows are below.
template< binary_op_t op >
concept arithmetic_operator =
    op == binary_op_t::add || op == binary_op_t::sub ||
    op == binary_op_t::mul || op == binary_op_t::sdiv ||
    op == binary_op_t::udiv || op == binary_op_t::srem ||
    op == binary_op_t::urem || op == binary_op_t::shl ||
    op == binary_op_t::ashr || op == binary_op_t::lshr ||
    op == binary_op_t::bit_and || op == binary_op_t::bit_or ||
    op == binary_op_t::bit_xor;

// The primary of * / % + - << >> & | ^, 6.5.6 to 6.5.8, 6.5.11 to
// 6.5.13, rcc's binary(), chapter 6.1: the left operand's value first,
// G13, held while the right operand's is taken in a register of its
// own; then arithmetic(), the operation from the two registers at the
// operation's view, add w0, w0, w1, lsl x0, x0, x1, and the right
// released.  No instruction pins a register, so the operands are
// generated as for any consumer, and the register the node's consumer
// names goes to the last instruction, which names its destination apart
// from its sources: add w0, w1, w0 after a right operand that took x0.
template< binary_op_t op, view_t V, typename L, typename R >
    requires arithmetic_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs);
    const value_t r = value(s, b.rhs);

    return arithmetic(s, op, l, r, V, wanted);
}

// Whether a node with the constant L on the left of op is written with
// its operands exchanged, rcc's constant_first() as ordered() asks it:
// op commutes, and L is a constant, whether the instruction encodes it
// or not.
template< binary_op_t op, typename L >
concept constant_first = commutes(op) && constant< L >;

// The niches of the primary, chapter 6.1.  A constant on the left of an
// operator that commutes is the row of the node with its operands
// exchanged, 5 + a as a + 5: the value of the right operand, which is
// no constant, then the constant as the right operand.
template< binary_op_t op, view_t V, typename L, typename R >
    requires arithmetic_operator< op > && constant_first< op, L >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    return value(s, binary_t< op, V, R, L > { b.rhs, b.lhs }, wanted);
}

// A constant on the right is the instruction's immediate where it
// encodes it at the view, add w0, w0, #5, lsl x0, x0, #5, and is held
// as the primary holds it otherwise: mul encodes none, mov w1, #5 and
// mul w0, w0, w1, and 0 is no immediate of these, a & 0 being mov w1,
// #0 and the and.  Of the mirror's nodes it would take only a constant
// on both sides, which the builder folds.
template< binary_op_t op, view_t V, typename L, constant C >
    requires arithmetic_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, C > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs);

    return arithmetic(s, op, l, value_of(b.rhs), V, wanted);
}

// cmp of the held value l and a right operand at the view, rcc's
// compare_with(): a held value r, cmp w0, w1, the registers of
// operands(), a spilled value reloaded, and r released; or a constant's
// value, the immediate where cmp encodes it, add's and sub's of 2.4
// whatever the comparison, cmp w0, #5 and cmp x0, #-1, and otherwise a
// held value of its own, mov w1, #4097 and cmp w0, w1.  It sets the
// flags and writes no register; l stays held.
void compare(state_t &, value_t l, value_t r, view_t view);
void compare(state_t &, value_t l, long long constant, view_t view);

// The value of a comparison of op from the flags its cmp set, rcc's
// comparison(): cset of the condition, one instruction, into
// final_destination(), the register the consumer names where it may be
// written, cset w0, lt after cmp w1, w0, or v's own: an int, on the w
// name.
value_t cset(state_t &, binary_op_t op, value_t v, unsigned wanted);

// The cmp of l and the right operand of a comparison at the view V by
// its class: a constant as compare() takes its value, 0 the immediate
// #0, and any other right operand its value, generated as for any
// consumer.
template< view_t V, typename R >
void
compare_with(state_t &s, value_t l, const R &rhs)
{
    compare(s, l, value(s, rhs), V);
}

template< view_t V, constant C >
void
compare_with(state_t &s, value_t l, const C &rhs)
{
    compare(s, l, value_of(rhs), V);
}

// The primary of < > <= >= == !=, 6.5.9, 6.5.10, rcc's comparison() and
// compare(), 6.3: the left operand's value first, G13, generated as for
// any consumer, then the cmp with the right operand at the operation's
// view, compare_with(), then cset, cmp w0, w1 and cset w0, lt.
template< binary_op_t op, view_t V, typename L, typename R >
    requires comparison_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs);
    compare_with< V >(s, l, b.rhs);

    return cset(s, op, l, wanted);
}

// A constant on the left is the row of the node mirrored, rcc's
// ordered(): 5 < a as a > 5, mirrored(), whether cmp encodes the
// constant or not, so 5 < a is cmp w0, #5 after the load of a.
template< binary_op_t op, view_t V, constant C, typename R >
    requires comparison_operator< op >
value_t
value(state_t &s, const binary_t< op, V, C, R > &b, unsigned wanted = 0)
{
    return value(s, binary_t< mirrored(op), V, R, C > { b.rhs, b.lhs }, wanted);
}

// !e, 6.5.4.3p5, rcc's unary(): the operand's value, generated as for
// any consumer, its test against 0 at the operand's view, test_zero()'s
// without a label, cmp w0, #0, or tst w0, #0xff for a value valid to
// less than 32 bits, then cset of eq, as for e == 0.
template< view_t V, typename A >
value_t
value(state_t &s,
      const unary_t< unary_op_t::log_not, V, A > &u,
      unsigned wanted = 0)
{
    const value_t v = value(s, u.arg);
    test_zero(s, v, V, valid_of(u.arg, sign_t::signed_), std::nullopt);

    return cset(s, binary_op_t::eq, v, wanted);
}

// The jump on a value compared with 0, rcc's test_zero(), 7.1: the
// value held, then the jump on its register at the value's view, cbz
// w0, or tst and b.eq where the width rule says the value is valid to
// less than 32 bits, the same at either sign, and the value released.
// A constant is no special case: if (1) loads 1 and tests it.
template< typename T >
void
value_branch(state_t &s, const T &t, int label, bool sense)
{
    const value_t v = value(s, t);
    test_zero(s, v, view_of(t), valid_of(t, sign_t::signed_), label, sense);
    release(s, v);
}

// A comparison as a jump, rcc's cbranch(), 7.1: the cmp of the value
// row, then b.cc of the operator where sense and of its inverse
// otherwise, b.lt or b.ge, and no value held.  e == 0 has no niche
// here: a == 0 is cmp w0, #0 and b.ne, where if (a) is cbz.
template< binary_op_t op, view_t V, typename L, typename R >
    requires comparison_operator< op >
void
branch(state_t &s, const binary_t< op, V, L, R > &b, int label, bool sense)
{
    const value_t l = value(s, b.lhs);
    compare_with< V >(s, l, b.rhs);
    release(s, l);

    jump_if(s, sense ? op : inverse(op), label);
}

// A constant on the left is the jump of the node mirrored, ordered():
// if (5 < a) is cmp w0, #5 and b.le.
template< binary_op_t op, view_t V, constant C, typename R >
    requires comparison_operator< op >
void
branch(state_t &s, const binary_t< op, V, C, R > &b, int label, bool sense)
{
    branch(s, binary_t< mirrored(op), V, R, C > { b.rhs, b.lhs }, label, sense);
}

// Pointers, docs/typed-tree.md 4.2: the address a pointer's value
// designates, each access's form, rcc's pointed(), the summaries that
// read the chain under it, rcc's denoted() and based(), and the rows of
// pointer arithmetic, rcc's pointer_arithmetic() and difference(), 6.3.

// An integer as aarch64's form reads it, rcc's index_t and index_of():
// its value and how the form reads its register.
struct index_t
{
    value_t value;
    extend_t extend = extend_t::lsl;
};

// The address a pointer denotes before any instruction forms it, rcc's
// denoted_t, 6.3: the pointer under the casts between pointer types and
// the sums by a constant met on the way, their constants summed in
// bytes, each scaled by its own sum's stride, their count, and the
// instructions the row of p + n writes for them, product_length()'s; at
// most one sum by a variable index, its index, its stride and whether
// it is written first; the first sum met, top.  A summary, each node's
// overload one case of rcc's walk, handed the walk so far: a
// dereference's address &*q steps to q, a cast steps through, a sum by a
// constant adds, the first sum by an index records it, and anything
// else, a second index and a difference by one among them, is the base.
struct denoted_t
{
    std::optional< expr_t > base = std::nullopt;
    long long bytes = 0;
    int constants = 0;
    int written = 0;
    std::optional< expr_t > index = std::nullopt;
    long index_stride = 0;
    bool index_first = false;
    std::optional< expr_t > top = std::nullopt;
};

// The instructions of p + n's row for a product d, rcc's
// product_length(): the add, and before it the constant row's where
// add's immediate does not encode d.
int product_length(long long d);

inline denoted_t
denoted(const address_of_t< indirect_t > &a, const expr_t &, denoted_t d)
{
    return denoted(a.where.pointer, std::move(d));
}

template< typename P >
denoted_t
denoted(const bitcast_t< P > &c, const expr_t &, denoted_t d)
{
    if constexpr (std::same_as< P, expr_t >)
        return denoted(c.pointer, std::move(d));
    else
        return denoted(c.pointer, expr_t(c.pointer), std::move(d));
}

template< binary_op_t op, typename N, bool index_first >
denoted_t
denoted(const gep_t< op, N, index_first > &g, const expr_t &self, denoted_t d)
{
    if (!d.top)
        d.top = self;

    if constexpr (constant< N >) {
        const long long bytes =
            scaled< binary_op_t::add >(value_of(g.index), g.stride);
        d.bytes =
            wrapped_add(d.bytes, scaled< op >(value_of(g.index), g.stride));
        ++d.constants;
        d.written += product_length(bytes);
    }
    else {
        if (op != binary_op_t::add || d.index) {
            d.base = self;
            return d;
        }

        d.index = node_of(g.index);
        d.index_stride = g.stride;
        d.index_first = index_first;
    }

    return denoted(g.pointer, std::move(d));
}

// The base and the displacement of an address by any lvalue that is no
// local's and a constant, rcc's based(), 6.3: the address of the place
// through q plus the constant, lvalue_offset()'s, down through each
// such address whose own q is one, and through one sum by a constant,
// p[1] on int **p being p and 8; the pointer at the bottom is the base.
// None for a local's address, folded()'s, and for any other pointer.
std::optional< based_t > based(const expr_t &pointer);

// rcc's based() past its tests, on the dereference's address &*q, q, and
// the offset.
std::optional< based_t >
based(const expr_t &address, const expr_t &q, long long offset);

// Whether an access of size bytes encodes the displacement off its
// base, rcc's in_reach(), 2.3: a multiple of the size from 0 to 4095
// times it, the scaled form, or -256 to 255, the unscaled one.
bool in_reach(long long displacement, int size);

// rcc's folded() and named_operand(): a local's address plus a constant,
// named_offset()'s, is the slot's operand with the offset folded into
// its displacement, [x29, #28] for (&x)[3]; none for a displacement past
// the frame's reach.
std::optional< address_t > folded(state_t &,
                                  const std::optional< named_offset_t > &);

// rcc's folded() where an access of size bytes takes it, named_access():
// the slot's operand in reach, [x29, #d], beyond which rcc reaches the
// slot through x16, which rcc-v2 does not yet.
std::optional< address_t >
folded_access(state_t &, const std::optional< named_offset_t > &, int size);

// rcc's summed(): an address by any other lvalue and a constant, based()'s
// base, its pointer's value, with the displacement, when the access of
// size bytes encodes it, [x0, #12] for (&p[1])[2], and no instruction
// forms the intermediate address; a row by a variable index, the row's
// sum and its constants past it, row_access()'s; past reach, with
// nothing evaluated between the address and its access, a displacement
// no immediate holds is held_offset()'s.  None otherwise.
std::optional< address_t > summed(state_t &,
                                  const std::optional< based_t > &plan,
                                  int size,
                                  bool nothing_between);

// A sum of a pointer and an integer as the access's form, rcc's
// sum_address(), read off the node: its pointer, its index as a handle,
// the index's value where it is a constant, the stride, whether the
// integer is written first, and the sum's own handle.
struct sum_t
{
    expr_t pointer;
    expr_t index;
    std::optional< long long > constant;
    long stride;
    bool index_first;
    expr_t self;
};

template< binary_op_t op, typename N, bool index_first >
sum_t
sum_of(const gep_t< op, N, index_first > &g, const expr_t &self)
{
    std::optional< long long > c;
    if constexpr (constant< N >)
        c = value_of(g.index);

    return { g.pointer, node_of(g.index), c, g.stride, index_first, self };
}

// The address of an access of size bytes through a sum, rcc's
// sum_address(), 6.3: the shape of a subscript is the machine's form,
// the pointer the base and the integer the index, generated in the order
// written (G13), the index by index_of(); a constant index whose bytes
// the access encodes the displacement, [x0, #12], one past its reach an
// index held, mov x1, #5000, or held back past a store's right operand,
// held_index()'s; the constants under the pointer, a variable index
// among them, the row's, row_access()'s, and a total of them past reach
// held once where that is shorter, holds_total()'s; a base and an index
// held across a call summed into one register, index_across()'s.  under
// is a cast between pointer types directly over the sum, whose own
// value pointer is: the index form under it only where it saves the
// add.
address_t sum_address(state_t &,
                      const sum_t &sum,
                      bool under,
                      const expr_t &pointer,
                      between_t between,
                      int size);

// The address a pointer's value designates for an access of size bytes,
// rcc's pointed(), 6.5.4.2p4, its entry point (docs/typed-tree.md 3.4),
// with between, what the caller generates between it and its access:
// the primary, erased_pointed(), rcc's tail, a local's address plus a
// constant folded_access(), any other address plus a constant summed(),
// any other pointer its value, the base, [x0].  self is the pointer's
// handle.  Each case asks the pointer through its handle, which answers
// as its node's own overload does, so the primary is one function, and
// not one for each node type: a model whose node has no niche of
// pointed() takes it, defaults_t's (model.hh).
address_t
erased_pointed(state_t &, const expr_t &self, between_t between, int size);

// A sum: folded_access(), summed(), then for + sum_address(), and for -,
// which no form subtracts, its value, the base, [x0], *(p - 3) being sub
// x0, x0, #12 and ldr w0, [x0].
template< binary_op_t op, typename N, bool index_first >
address_t
pointed(state_t &s,
        const gep_t< op, N, index_first > &g,
        between_t between,
        int size,
        const expr_t &self)
{
    if (const auto a = folded_access(s, named_offset(g), size))
        return *a;

    if (const auto a =
            summed(s, based(self), size, between == between_t::nothing))
        return *a;

    if constexpr (op == binary_op_t::add)
        return sum_address(s, sum_of(g, self), false, self, between, size);
    else
        return { .base = value(s, g) };
}

// A cast directly over a sum, rcc's under: the sum's address read through
// it, at the cast's access, where the sum's own pointer is no local's
// address plus a constant.
template< binary_op_t op, typename N, bool index_first >
address_t
pointed(state_t &s,
        const bitcast_t< gep_t< op, N, index_first > > &c,
        between_t between,
        int size,
        const expr_t &self)
{
    if (const auto a = folded_access(s, named_offset(c), size))
        return *a;

    if (const auto a =
            summed(s, based(self), size, between == between_t::nothing))
        return *a;

    if constexpr (op == binary_op_t::add)
        if (!named_offset(c.pointer))
            return sum_address(s, sum_of(c.pointer, expr_t(c.pointer)), true,
                               self, between, size);

    return { .base = value(s, c) };
}

// A cast's value is its operand's, the low bits of whose register are the
// address.
template< typename P >
value_t
value(state_t &s, const bitcast_t< P > &c, unsigned wanted = 0)
{
    return value(s, c.pointer, wanted);
}

// The widening's operand of the view F, extended by its sign S.
index_t
widened_index(state_t &, value_t v, int valid, view_t from, sign_t sign);

// An integer as aarch64's form reads it, rcc's index_of(), its entry
// point of integer nodes: the parser's widening looked through, its
// operand generated, extended to 32 bits where it is valid to less, and
// read by the form as the x register where valid to 64, lsl, else as the
// w register extended by the operand's sign, sxtw or uxtw; any other
// node its value, the x register, defaults_t's (model.hh).  rcc's
// third reader of the width rule, held_valid()'s.
template< view_t F, typename A >
index_t
index_of(state_t &s, const sext_t< view_t::i64, F, A > &e)
{
    const value_t v = value(s, e.arg);

    return widened_index(s, v, held_valid(s, v, e.arg, sign_t::signed_), F,
                         sign_t::signed_);
}

// A signed conditional under the widening: the widening is looked
// through before any row hands it to the arms, so the conditional is
// generated at its own view and the form extends its join, sxtw.
template< view_t F, bool jumps >
index_t
index_of(state_t &s,
         const sext_t< view_t::i64, F, widened_arms_t< F, jumps > > &e)
{
    const auto &c = e.arg.conditional;
    const value_t v = value(s, c);

    return widened_index(s, v, held_valid(s, v, c, sign_t::signed_), F,
                         sign_t::signed_);
}

template< view_t F, typename A >
index_t
index_of(state_t &s, const zext_t< view_t::i64, F, A > &e)
{
    const value_t v = value(s, e.arg);

    return widened_index(s, v, held_valid(s, v, e.arg, sign_t::unsigned_), F,
                         sign_t::unsigned_);
}

template< family_t F >
index_t
index_of(state_t &s, const top_t< F > &t)
{
    return index_of(s, t.node);
}

// The value of p + n, n + p and p - n, rcc's pointer_arithmetic() on the
// node read as sum_t, the operator op: a local's address plus a constant
// the add of folded()'s operand, add x0, x29, #28; a row on a local by an
// index named_row()'s; the constants under the pointer one add,
// joined()'s, add x0, x0, #12 for p + 1 + 2; otherwise the operands in
// the order written, a constant as its product with the stride, and the
// add or sub, add x0, x0, w1, sxtw #2, the result in the register the
// consumer names, wanted.
value_t pointer_arithmetic(state_t &,
                           binary_op_t op,
                           const sum_t &sum,
                           const std::optional< named_offset_t > &named,
                           unsigned wanted);

template< binary_op_t op, typename N, bool index_first >
value_t
value(state_t &s, const gep_t< op, N, index_first > &g, unsigned wanted = 0)
{
    return pointer_arithmetic(s, op, sum_of(g, expr_t(g)), named_offset(g),
                              wanted);
}

// p - q, 6.5.7p10, rcc's difference(): sub of the two values, then the
// division by the stride held, mov x1, #4; sdiv x0, x0, x1, a long, never
// a shift, since the two pointers are any two addresses (G6).
value_t difference(state_t &,
                   const expr_t &lhs,
                   const expr_t &rhs,
                   long stride,
                   unsigned wanted);

template< typename R >
value_t
value(state_t &s, const difference_t< R > &d, unsigned wanted = 0)
{
    return difference(s, d.lhs, node_of(d.rhs), d.stride, wanted);
}

// The extension of the held value v of the view from to the view to by
// the sign of its type, rcc's extended() past its test of the width
// rule, which the widening's row asks: sxtw x0, w0, or mov w0, w0 for an
// unsigned one, whose write of the w name clears the upper half,
// written into final_destination(), the register the consumer names
// where it may be written, sxtw x0, w1 for a return's (6.1), or v's
// own.
value_t extended(
    state_t &, value_t v, view_t from, sign_t sign, view_t to, unsigned wanted);

// The extending load, rcc's extending_load(), 6.3: the object of the
// view from at an address, loaded extended by its sign to 64 bits,
// ldrsw x0, [x29, #16], as load() takes the address.  It reads the
// bytes the load at its own view reads, at the object's width (G10),
// and leaves the value valid to 64.
value_t extending_load(state_t &, const address_t &, view_t from);

// A signed object widened to 64 bits is its extending load, the niche
// of the conversion over a load (docs/typed-tree.md 4.2), as on x86-64.
template< view_t F, typename W >
value_t
value(state_t &s,
      const sext_t< view_t::i64, F, load_t< W, F, sign_t::signed_ > > &e,
      unsigned = 0)
{
    return extending_load(s, address(s, e.arg.where), F);
}

// The arguments of a call moved from their slots to where the
// convention passes them, and the call of the function named, rcc's
// call() after the arguments' stores, chapter 8.
void transfer(state_t &,
              std::string_view name,
              const std::vector< argument_t > &args);

// A call of a function whose result is of the view V, 6.5.3.3, rcc's
// call(), chapter 8: its value is held in x0, where the call leaves it,
// valid to V's width and no further (valid_of()): the convention leaves
// the bits above a narrow one unspecified, and the widening's primary
// extends it where it is used wider, and test_zero() tests it with tst
// where it is narrow.
template< view_t V >
value_t
value(state_t &s, const call_t< V > &c, unsigned = 0)
{
    call(s, c.name, c.args);

    return hold(s, x0);
}

// Whether a call is within a node, whose result the convention puts in
// x0 (chapter 8), rcc's calls_within(), a summary of aarch64, each
// node's of its parts, which an erased one hands up through its handle
// (principle 3): a call's true, a leaf's false, a where's its
// pointer's.
template< constant C >
constexpr bool
calls_within(const state_t &, const C &)
{
    return false;
}

constexpr bool
calls_within(const state_t &, slot_t)
{
    return false;
}

inline bool
calls_within(const state_t &, const global_t &)
{
    return false;
}

inline bool
calls_within(const state_t &s, const indirect_t &w)
{
    return calls_within(s, w.pointer);
}

template< typename W, view_t V, sign_t S >
bool
calls_within(const state_t &s, const place_t< W, V, S > &p)
{
    return calls_within(s, p.where);
}

template< typename W, view_t V, sign_t S >
bool
calls_within(const state_t &s, const load_t< W, V, S > &l)
{
    return calls_within(s, l.where);
}

template< typename W >
bool
calls_within(const state_t &s, const address_of_t< W > &a)
{
    return calls_within(s, a.where);
}

template< unary_op_t op, view_t V, typename A >
bool
calls_within(const state_t &s, const unary_t< op, V, A > &u)
{
    return calls_within(s, u.arg);
}

template< binary_op_t op, view_t V, typename L, typename R >
bool
calls_within(const state_t &s, const binary_t< op, V, L, R > &b)
{
    return calls_within(s, b.lhs) || calls_within(s, b.rhs);
}

template< family_t F >
bool
calls_within(const state_t &s, const top_t< F > &t)
{
    return calls_within(s, t.node);
}

template< view_t V, view_t F, typename A >
bool
calls_within(const state_t &s, const sext_t< V, F, A > &e)
{
    return calls_within(s, e.arg);
}

template< view_t V, view_t F, typename A >
bool
calls_within(const state_t &s, const zext_t< V, F, A > &e)
{
    return calls_within(s, e.arg);
}

template< view_t V, typename A >
bool
calls_within(const state_t &s, const trunc_t< V, A > &t)
{
    return calls_within(s, t.arg);
}

template< typename W, view_t V >
bool
calls_within(const state_t &s, const assign_t< W, V > &a)
{
    return calls_within(s, a.where) || calls_within(s, a.value);
}

template< view_t V >
constexpr bool
calls_within(const state_t &, const call_t< V > &)
{
    return true;
}

constexpr bool
calls_within(const state_t &, const call_void_t &)
{
    return true;
}

template< binary_op_t op, typename W, view_t V, view_t O, typename R >
bool
calls_within(const state_t &s, const compound_t< op, W, V, O, R > &c)
{
    return calls_within(s, c.where) || calls_within(s, c.value);
}

template< step_op_t op, typename W, view_t V, sign_t S >
bool
calls_within(const state_t &s, const step_t< op, W, V, S > &t)
{
    return calls_within(s, t.where);
}

template< view_t V, bool jumps >
bool
calls_within(const state_t &s, const conditional_t< V, jumps > &c)
{
    return calls_within(s, c.cond) || calls_within(s, c.yes) ||
           calls_within(s, c.no);
}

inline bool
calls_within(const state_t &s, const conditional_void_t &c)
{
    return calls_within(s, c.cond) || calls_within(s, c.yes) ||
           calls_within(s, c.no);
}

// A conditional under a widening calls what the conditional calls: an
// arm's widening calls nothing of its own.
template< view_t F, bool jumps >
bool
calls_within(const state_t &s, const widened_arms_t< F, jumps > &w)
{
    return calls_within(s, w.conditional);
}

template< typename R >
bool
calls_within(const state_t &s, const comma_t< R > &c)
{
    return calls_within(s, c.lhs) || calls_within(s, c.rhs);
}

template< binary_op_t op, typename N, bool index_first >
bool
calls_within(const state_t &s, const gep_t< op, N, index_first > &g)
{
    return calls_within(s, g.pointer) || calls_within(s, g.index);
}

template< typename R >
bool
calls_within(const state_t &s, const difference_t< R > &d)
{
    return calls_within(s, d.lhs) || calls_within(s, d.rhs);
}

template< typename P >
bool
calls_within(const state_t &s, const bitcast_t< P > &c)
{
    return calls_within(s, c.pointer);
}

// Whether a node's rows are certain to write the register a consumer
// names, rcc's takes_name(), a summary of aarch64 that its ?: asks of
// both arms before it hands them the name: every row of the tree does,
// but a ?:'s, which hands its own arms the name only where they take
// it, an address and pointer arithmetic, the nodes that are their
// operands' value, a conversion and a comma, whose operand's answer is
// theirs, and an assignment, whose place holding a call's result holds
// x0 at the store.
namespace primary {

template< typename T >
constexpr bool
takes_name(const state_t &, const T &)
{
    return true;
}

} // namespace primary

// The primary of this target's line whose default a model writes once,
// as zoo.hh's are (model.hh).
using namespace primary;

template< typename W >
constexpr bool
takes_name(const state_t &, const address_of_t< W > &)
{
    return false;
}

// Pointer arithmetic is no operation on integers, rcc's takes_name() of a
// binary operator of a pointer type or of a pointer operand.
template< binary_op_t op, typename N, bool index_first >
constexpr bool
takes_name(const state_t &, const gep_t< op, N, index_first > &)
{
    return false;
}

template< typename R >
constexpr bool
takes_name(const state_t &, const difference_t< R > &)
{
    return false;
}

template< typename P >
bool
takes_name(const state_t &s, const bitcast_t< P > &c)
{
    return takes_name(s, c.pointer);
}

template< view_t V, bool jumps >
constexpr bool
takes_name(const state_t &, const conditional_t< V, jumps > &)
{
    return false;
}

template< family_t F >
bool
takes_name(const state_t &s, const top_t< F > &t)
{
    return takes_name(s, t.node);
}

template< view_t V, view_t F, typename A >
bool
takes_name(const state_t &s, const sext_t< V, F, A > &e)
{
    return takes_name(s, e.arg);
}

template< view_t V, view_t F, typename A >
bool
takes_name(const state_t &s, const zext_t< V, F, A > &e)
{
    return takes_name(s, e.arg);
}

template< view_t V, typename A >
bool
takes_name(const state_t &s, const trunc_t< V, A > &t)
{
    return takes_name(s, t.arg);
}

template< typename R >
bool
takes_name(const state_t &s, const comma_t< R > &c)
{
    return takes_name(s, c.rhs);
}

template< typename W, view_t V >
bool
takes_name(const state_t &s, const assign_t< W, V > &a)
{
    return !calls_within(s, a.where) && takes_name(s, a.value);
}

template< view_t V, bool jumps >
constexpr bool
takes_name(const state_t &, const widened_arms_t< V, jumps > &)
{
    return false;
}

// Whether a signed widening of the conditional of the arms yes and no
// is handed to them, rcc's widens_arms() past what the node's type
// says: where an arm widens free, its arm_widening().  A remainder is
// no such arm here, since msub writes the register it is handed, and
// neither the state nor the register the consumer names is read.
bool widens_arms(const state_t &,
                 const expr_t &yes,
                 const expr_t &no,
                 unsigned wanted);

// The value of c ? y : n of the view, rcc's conditional(), aarch64.md
// 6.1: as x86-64's, but that each arm is handed the register the
// consumer names, wanted, where it is free, and never the join, so a
// second arm that lands elsewhere moves into the join, mov w1, w0.
value_t conditional(state_t &,
                    const expr_t &cond,
                    const expr_t &yes,
                    const expr_t &no,
                    view_t view,
                    sign_t sign,
                    unsigned wanted);

// A conditional's value, rcc's rvalue(): the consumer's register handed
// on where both arms take it, takes_name(), and none otherwise.
template< view_t V, bool jumps >
value_t
value(state_t &s, const conditional_t< V, jumps > &c, unsigned wanted = 0)
{
    const bool arms = takes_name(s, c.yes) && takes_name(s, c.no);

    return conditional(s, c.cond, c.yes, c.no, V, c.sign, arms ? wanted : 0);
}

// The value at a return's top, rcc's rvalue_in(): any node's value
// handed the result register, defaults_t's (model.hh), and a
// conditional's joined in it whatever its arms take.
template< view_t V, bool jumps >
value_t
value_in(state_t &s, const conditional_t< V, jumps > &c, unsigned wanted)
{
    return conditional(s, c.cond, c.yes, c.no, V, c.sign, wanted);
}

// The pieces of a switch's dispatch, backend.md 7.3, which the
// dispatches of rows-statements.hh are written over, as rcc's
// direct_switch(), simple_switch() and hash_switch() are, each at the
// view of the controlling expression's promoted type: the w names of
// the registers at 32 bits, the x names at 64.

// A case's test, rcc's equal_jump(): the comparison row of the held
// value v and the case's value at the view, cmp w0, #5, or mov w1,
// #4097 and cmp w0, w1 where cmp does not encode it, then b.eq to its
// label; v stays held.  The Seventh Edition compiler tested a 0 where it
// compared another value, tst for cmp; this machine's test of zero is
// cbz, one instruction, 7.1.
void equal_jump(state_t &, value_t v, long long value, view_t view, int label);

// The first value of a direct switch subtracted from the held value v
// at the view, rcc's arithmetic() of the sub row, sub w0, w0, #1, or mov
// x1, #4294967296 and sub x0, x0, x1 where sub does not encode it; the
// value is v's.
value_t subtracted(state_t &, value_t v, long long first, view_t view);

// The jump through a switch's table by a held index of the view, rcc's
// table_jump(): the table's page and offset into x16, adrp and add, as a
// literal's address is formed; the entry loaded through it by the form
// that scales the index, and extends a 32-bit one unsigned, ldr x16,
// [x16, w0, uxtw #3], or reads a 64-bit one whole, [x16, x0, lsl #3], so
// the width its register is valid to is not read; then br x16, after
// which no path reaches what is written next.  The index is consumed.
void table_jump(state_t &, value_t index, int valid, view_t view, int table);

// The division of a hash switch at the view, rcc's divided(): the
// table's size held in a register, mov w1, #3, as the divisor of x / 7
// is, since udiv takes no immediate; the quotient by udiv into a
// register taken outside both, udiv w2, w0, w1, and the remainder by
// msub into v's, msub w0, w2, w1, w0.  The pair is the quotient and the
// remainder, v.
std::pair< value_t, value_t >
divided(state_t &, value_t v, long long size, view_t view);

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

} // namespace rcc::arm64
