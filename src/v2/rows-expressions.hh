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

namespace rcc {

// A place is read only through its load, the lvalue conversion the
// parser asks of every operand whose value it uses, rvalue(): a slot
// asked for its value is a tree the parser does not make.
template< typename S >
value_t
value(S &, const slot_t &, unsigned = 0)
{
    throw std::logic_error("v2: a place read without its lvalue conversion");
}

// A right operand kept as a top_t is the node it holds, whose own row
// writes it through the handle.
template< typename S, binary_op_t op >
value_t
value(S &s, const top_t< op > &t, unsigned wanted = 0)
{
    return value(s, t.node, wanted);
}

// The store row of an initializer, rcc's initialize(), chapter 6.6: the
// value in a register, stored into the slot, released.  The targets'
// niches store a constant with no register held.
template< typename S, typename V >
void
initialize(S &s, slot_t where, const V &e)
{
    const value_t v = value(s, e);
    store(s, reg_of(s, v), where);
    release(s, v);
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
template< typename S, typename V >
void
effect(S &s, const assign_t< V > &a)
{
    initialize(s, a.where, a.value);
}

// The jump on a value compared with 0, rcc's test_zero(), 7.1: the
// value held, then each target's jump on its register, cmpl $0, %eax
// and je, or cbz w0, and the value released.  A constant is no special
// case: if (1) loads 1 and tests it.
template< typename S, typename T >
void
value_branch(S &s, const T &t, int label, bool sense)
{
    const value_t v = value(s, t);
    test_zero(s, v, label, sense);
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
template< typename S, typename A >
void
branch(S &s, const unary_t< unary_op_t::log_not, A > &u, int label, bool sense)
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
template< typename S, binary_op_t op, typename L, typename R >
    requires logical_operator< op >
void
branch(S &s, const binary_t< op, L, R > &b, int label, bool sense)
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
// is held in a register across the paths, before_paths(); then the
// labels of the 0 path and of the join, taken before either operand;
// the jump on the whole to the 0 path; and on both paths one register,
// taken after the jumps, holding 1 or 0, movl $1, %eax and movl $0,
// %eax, mov w0, #1 and mov w0, #0, never set from the flags.  The join
// is the label past the 0 path.  The register is the lowest free,
// whatever the consumer names, as in rcc.
template< typename S, binary_op_t op, typename L, typename R >
    requires logical_operator< op >
value_t
value(S &s, const binary_t< op, L, R > &b, unsigned = 0)
{
    before_paths(s);

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
// register, which holds the place's value after the store, 6.5.17.1p3,
// the node's value.  So a constant goes through a register here, on
// x86-64 too, and 0 is no store of wzr.
template< typename S, typename V >
value_t
value(S &s, const assign_t< V > &a, unsigned wanted = 0)
{
    const value_t v = value(s, a.value, wanted);
    store(s, reg_of(s, v), a.where);

    return v;
}

} // namespace rcc
