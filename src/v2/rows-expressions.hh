// -*- mode: c++; -*-

// The rows of the expressions that both targets write alike: templates
// over the target's state, which name no register and write no text of
// their own.  value(), the node's value held in a register its row
// takes, is the entry point with no primary (docs/typed-tree.md 3.4):
// every node type has a row that returns its value, and effect()'s
// primary is derived from it, a value dropped.  A target may hand a
// value's row a hint, x86-64 the registers the consumer names, and a
// row here hands the hints it is given on to the operand whose register
// the node's value is, as rcc's rows hand wanted on (3.4).  build.cc
// includes this header beside the targets' rows.

#pragma once

#include "register-stack.hh"
#include "zoo.hh"

#include <stdexcept>

namespace rcc {

// A place is read only through its load, the lvalue conversion the
// parser asks of every operand whose value it uses, rvalue(): a slot
// asked for its value is a tree the parser does not make.
template< typename S, typename... W >
value_t
value(S &, const slot_t &, W...)
{
    throw std::logic_error("v2: a place read without its lvalue conversion");
}

// - and ~, 6.5.4.3p3, p4: the operand's value, then the operation on
// its register in place, negl %eax, neg w0, w0, which leaves the node's
// value there.
template< typename S, unary_op_t op, typename A, typename... W >
value_t
value(S &s, const unary_t< op, A > &u, W... wanted)
{
    const value_t v = value(s, u.arg, wanted...);
    operate(s, op, reg_of(s, v));

    return v;
}

// A right operand kept as a top_t is the node it holds, whose own row
// writes it through the handle.
template< typename S, binary_op_t op, typename... W >
value_t
value(S &s, const top_t< op > &t, W... wanted)
{
    return value(s, t.node, wanted...);
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

// An assignment as a value: the value held, then stored from its
// register, which holds the place's value after the store, 6.5.17.1p3,
// the node's value.  So a constant goes through a register here, on
// x86-64 too, and 0 is no store of wzr.
template< typename S, typename V, typename... W >
value_t
value(S &s, const assign_t< V > &a, W... wanted)
{
    const value_t v = value(s, a.value, wanted...);
    store(s, reg_of(s, v), a.where);

    return v;
}

} // namespace rcc
