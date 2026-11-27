// -*- mode: c++; -*-

// The rows of the expressions that both targets write alike: templates
// over the target's state, which name no register and write no text of
// their own.  value(), the node's value held in a register its row
// takes, is the entry point with no primary (docs/typed-tree.md 3.4):
// every node type has a row that returns its value, and the primaries
// of effect() and branch() are derived from it, a value dropped and a
// value compared with 0.  Each target hands a value's row a hint,
// wanted, the registers the consumer names, and a row here hands it on
// to the operand whose register the node's value is, as rcc's rows hand
// wanted on (3.4).  build.cc includes this header beside the targets'
// rows.

#pragma once

#include "assembly.hh"
#include "register-stack.hh"
#include "zoo.hh"

#include <stdexcept>
#include <string_view>
#include <vector>

namespace rcc {

// A place is read only through its load, the lvalue conversion the
// parser asks of every operand whose value it uses, rvalue(): a place
// asked for its value is a tree the parser does not make.
template< typename S, view_t V >
value_t
value(S &, const place_t< V > &, unsigned = 0)
{
    throw std::logic_error("v2: a place read without its lvalue conversion");
}

// A right operand kept as a top_t is the node it holds, whose own row
// writes it through the handle.
template< typename S, family_t F >
value_t
value(S &s, const top_t< F > &t, unsigned wanted = 0)
{
    return value(s, t.node, wanted);
}

// A widening, rcc's converted() for one: the operand's value, handed
// the consumer's register, then its extension by sign where the width
// rule says that the register is not valid to V's width yet, each
// target's extended(), which writes it into the register the consumer
// names where it can.  (long) (a + b) on two ints is movslq %eax, %rax,
// and (long) (a < b) writes no extension, a truth value being valid to
// 64 bits.  The row asks its operand's summary, as x86-64's rows ask
// pinned_within(); a niche of each target reads a load extended.
template< typename S, view_t V, view_t F, typename A >
value_t
value(S &s, const sext_t< V, F, A > &e, unsigned wanted = 0)
{
    const value_t v = value(s, e.arg, wanted);
    if (valid_of(e.arg) >= width(V))
        return v;

    return extended(s, v, F, V, wanted);
}

// A conversion that writes nothing is its operand's value, the low bits
// of whose register are the value at V (G2).
template< typename S, view_t V, typename A >
value_t
value(S &s, const trunc_t< V, A > &t, unsigned wanted = 0)
{
    return value(s, t.arg, wanted);
}

// The store row of an initializer, rcc's initialize(), chapter 6.6: the
// value in a register, stored into the slot at the value's view, which
// is the place's, since the parser converted the value to the place's
// type (docs/typed-tree.md 3.4), and released.  The targets' niches
// store a constant with no register held, found by overload on the
// value's type, which the handle's row, the entry point, knows.
template< typename S, typename T >
void
initialize(S &s, slot_t where, const T &e)
{
    const value_t v = value(s, e);
    store(s, reg_of(s, v), where, view_of(e));
    release(s, v);
}

// A local's slot, of its view's bytes at as many bytes' alignment, from
// the frame's allocator, which a spill's slot comes from too, so the
// two interleave in the order they are taken; its displacement is
// recorded under the local's number, where a use reads it
// (docs/typed-tree.md 4.5).
template< typename S >
void
allocate(S &s, slot_t where, view_t view)
{
    const int bytes = width(view) / 8;

    s.slots.emplace(where.id, allocate(s, bytes, bytes));
}

// An expression for its effect, 6.8.4p2, its value dropped: the value
// held, and released at once.
template< typename S, typename T >
void
effect(S &s, const T &t)
{
    release(s, value(s, t));
}

// An assignment for its effect is the store of an initializer, so
// x = 7; writes what int x = 7; writes, and each target's niche of
// initialize() serves both.
template< typename S, view_t V >
void
effect(S &s, const assign_t< V > &a)
{
    initialize(s, a.where, a.value);
}

// The jump on a value compared with 0, rcc's test_zero(), 7.1: the
// value held, then each target's jump on its register at the value's
// view, cmpl $0, %eax and je, or cbz w0, and the value released.  A
// constant is no special case: if (1) loads 1 and tests it.
template< typename S, typename T >
void
value_branch(S &s, const T &t, int label, bool sense)
{
    const value_t v = value(s, t);
    test_zero(s, v, view_of(t), label, sense);
    release(s, v);
}

// The primary of branch, rcc's cbranch() for a node that is no
// comparison, no ! and no && or ||: a jump on a value is the value
// compared with 0 (3.4), value_branch(), where x86-64 has niches of its
// own.
template< typename S, typename T >
void
branch(S &s, const T &t, int label, bool sense)
{
    value_branch(s, t, label, sense);
}

// !e as a jump writes nothing of its own: the jump on e, the sense
// flipped.
template< typename S, view_t V, typename A >
void
branch(S &s,
       const unary_t< unary_op_t::log_not, V, A > &u,
       int label,
       bool sense)
{
    branch(s, u.arg, label, !sense);
}

// && and || as a jump, rcc's cbranch(), 6.5.14p4, 6.5.15p4: where
// either operand decides the jump, && jumping on 0 and || on nonzero,
// each operand jumps to label in the same sense, with no label of its
// own; otherwise the left operand jumps the other way past the right
// one, to a skip label taken before it and placed after the right
// one's jump.  The right operand is reached only where the left one
// leaves the value open.
template< typename S, binary_op_t op, view_t V, typename L, typename R >
    requires logical_operator< op >
void
branch(S &s, const binary_t< op, V, L, R > &b, int label, bool sense)
{
    if (sense != (op == binary_op_t::log_and)) {
        branch(s, b.lhs, label, sense);
        branch(s, b.rhs, label, sense);
        return;
    }

    const int skip = new_label(s);
    branch(s, b.lhs, skip, !sense);
    branch(s, b.rhs, label, sense);
    place_label(s, skip);
}

// && and || as a value, rcc's logical(), 6.5.14p3, 6.5.15p3: no value
// is held in a register across the paths, before_paths(), which spills
// every one and marks each held value until the join; then the labels
// of the 0 path and of the join, taken before either operand; the jump
// on the whole to the 0 path; and on both paths one register, taken
// after the jumps, holding 1 or 0, movl $1, %eax and movl $0, %eax,
// mov w0, #1 and mov w0, #0, never set from the flags.  The join is the
// label past the 0 path.  The register is the lowest free outside the
// reservations, whatever the consumer names, as in rcc.
template< typename S, binary_op_t op, view_t V, typename L, typename R >
    requires logical_operator< op >
value_t
value(S &s, const binary_t< op, V, L, R > &b, unsigned = 0)
{
    const auto incoming = before_paths(s);

    const int no = new_label(s);
    const int end = new_label(s);

    branch(s, b, no, false);

    const value_t v = fresh(s);
    move_immediate(s, reg_of(s, v), 1);
    jump(s, end);

    place_label(s, no);
    move_immediate(s, reg_of(s, v), 0);

    place_label(s, end);

    return v;
}

// An assignment as a value: the value held, then stored from its
// register at the place's view, V, since the value is erased; the
// register holds the place's value after the store, 6.5.17.1p3, the
// node's value.  So a constant goes through a register here, on x86-64
// too, and 0 is no store of wzr.
template< typename S, view_t V >
value_t
value(S &s, const assign_t< V > &a, unsigned wanted = 0)
{
    const value_t v = value(s, a.value, wanted);
    store(s, reg_of(s, v), a.where, V);

    return v;
}

// A call, rcc's call(), backend.md chapter 8, as both targets write it
// up to the transfer: every held value spilled first, in the order of
// the registers, since the call destroys every expression register, so
// the arguments find every register free; then each argument, in order
// and complete before the next, G13, stored to its hidden local, whose
// slot is taken at the value's view before its value is generated;
// then the target's transfer(), the arguments moved to where the
// convention passes them and the call made.  Nothing is held when the
// arguments are stored, so rcc's second spill, before the transfer,
// spills nothing.
template< typename S >
void
call(S &s, std::string_view name, const std::vector< argument_t > &args)
{
    spill_all(s);

    for (const argument_t &a : args) {
        allocate(s, a.where, view_of(a.value));
        initialize(s, a.where, a.value);
    }

    transfer(s, name, args);
}

// A call of a void function has no value, 6.3.2.2p1, and the parser
// asks none of it, require_value(): a void call asked for its value is
// a tree the parser does not make.
template< typename S >
value_t
value(S &, const call_void_t &, unsigned = 0)
{
    throw std::logic_error("v2: the value of a void call");
}

// A call of a void function for its effect is the call, and leaves
// nothing held.
template< typename S >
void
effect(S &s, const call_void_t &c)
{
    call(s, c.name, c.args);
}

} // namespace rcc
