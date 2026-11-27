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
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
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
// builds fresh.  The body is a buffer, which the frame is written
// around; the function's name makes its labels, which are counted per
// function, and ret is the one epilogue's.  terminated says that no
// path reaches what is written next: the last instruction written is
// an unconditional jump, which jump() sets and place_label() clears,
// so an if whose then part ends so writes no jump over its else part,
// and main writes no return of 0 (backend.md 7.2).  loops is the
// labels each loop generated so far took for its body's jumps, by the
// loop's number.  frame is the bytes from x29 to the end of the last
// slot, the locals' and the spills' in the order they are taken, the
// frame record's alone before the first, and slots each local's
// displacement off x29, by its number.
struct state_t
{
    std::ostringstream body { };
    std::string function;
    int labels = 0;
    int ret = 0;
    bool terminated = false;
    std::unordered_map< int, loop_labels_t > loops { };

    long frame = 0;
    std::unordered_map< int, int > slots { };

    registers_t< reg_t > registers { };
};

// A jump to the function's label n; no path reaches what follows it.
void jump(state_t &, int n);

// A register for a new value, register-stack.hh's, taken outside avoid;
// it spills a held value when every one is held.
reg_t take(state_t &, unsigned avoid = 0);

// The function's label n, where a jump may reach it from elsewhere: a
// path reaches what follows.
void place_label(state_t &, int n);

// The branch to label on the flags a cmp set, where op holds, rcc's
// jump_if(): b.lt, b.eq.
void jump_if(state_t &, binary_op_t op, int label);

// The jump on the held value v compared with 0 at its view, rcc's
// test_zero(): cbnz w0 where sense, cbz w0 or cbz x0 otherwise, one
// instruction and no flags; v stays held.
void test_zero(state_t &, value_t v, view_t view, int label, bool sense);

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

// The load row: the object a slot holds at its view, in a register
// taken for it, ldr w0, [x29, #16] or ldr x0, [x29, #16].
value_t load(state_t &, slot_t where, view_t view);

template< view_t V >
value_t
value(state_t &s, const load_t< V > &l, unsigned = 0)
{
    return load(s, l.where, V);
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

// The store row, r into a slot at the view: str w0 or str x0.
void store(state_t &, reg_t r, slot_t where, view_t view);

// The store of the zero register into a slot at the view, str wzr or
// str xzr.
void store_zero(state_t &, slot_t where, view_t view);

// The niche of initialize(), 6.6: 0 is the zero register's store, and
// no register is held.
template< view_t V >
void
initialize(state_t &s, slot_t where, const zero_t< V > &)
{
    store_zero(s, where, V);
}

// The operators of rcc's arithmetic(), * / % + - << >> & | ^, whose
// row writes the operation into the left operand's register; a
// comparison's cmp writes none, and its rows are below.
template< binary_op_t op >
concept arithmetic_operator =
    op == binary_op_t::add || op == binary_op_t::sub ||
    op == binary_op_t::mul || op == binary_op_t::sdiv ||
    op == binary_op_t::srem || op == binary_op_t::shl ||
    op == binary_op_t::ashr || op == binary_op_t::bit_and ||
    op == binary_op_t::bit_or || op == binary_op_t::bit_xor;

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
// any consumer, its test against 0 at the operand's view, cmp w0, #0,
// then cset of eq, as for e == 0.
template< view_t V, typename A >
value_t
value(state_t &s,
      const unary_t< unary_op_t::log_not, V, A > &u,
      unsigned wanted = 0)
{
    const value_t v = value(s, u.arg);
    compare(s, v, 0, V);

    return cset(s, binary_op_t::eq, v, wanted);
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

// The extension of the held value v of the view from to the view to by
// its sign, rcc's extended() past its test of the width rule, which the
// widening's row asks: sxtw x0, w0, written into final_destination(),
// the register the consumer names where it may be written, sxtw x0, w1
// for a return's (6.1), or v's own.
value_t extended(state_t &, value_t v, view_t from, view_t to, unsigned wanted);

// The extending load, rcc's extending_load(), 6.3: the object of the
// view from in its slot, loaded extended by its sign to 64 bits, ldrsw
// x0, [x29, #16].  It reads the bytes the load at its own view reads,
// at the object's width (G10), and leaves the value valid to 64.
value_t extending_load(state_t &, slot_t where, view_t from);

// A signed object widened to 64 bits is its extending load, the niche
// of the conversion over a load (docs/typed-tree.md 4.2), as on x86-64.
template< view_t F >
value_t
value(state_t &s, const sext_t< view_t::i64, F, load_t< F > > &e, unsigned = 0)
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
// call(), chapter 8: its value is held in x0, where the call leaves it.
template< view_t V >
value_t
value(state_t &s, const call_t< V > &c, unsigned = 0)
{
    call(s, c.name, c.args);

    return hold(s, x0);
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

} // namespace rcc::arm64
