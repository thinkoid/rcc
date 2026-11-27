// -*- mode: c++; -*-

// The rows of the expressions that both targets write alike: templates
// over the target's state, which name no register and write no text of
// their own.  value(), the node's value held in a register its row
// takes, is the entry point with no primary (docs/typed-tree.md 3.4):
// every node type has a row that returns its value, and effect()'s
// primary is derived from it, a value dropped.  build.cc includes this
// header beside the targets' rows.

#pragma once

#include "register-stack.hh"
#include "zoo.hh"

#include <stdexcept>

namespace rcc {

// A place is read only through its load, the lvalue conversion the
// parser asks of every operand whose value it uses, rvalue(): a slot
// asked for its value is a tree the parser does not make.
template< typename S >
value_t
value(S &, const slot_t &)
{
    throw std::logic_error("v2: a place read without its lvalue conversion");
}

// - and ~, 6.5.4.3p3, p4: the operand's value, then the operation on
// its register in place, negl %eax, neg w0, w0, which leaves the node's
// value there.
template< typename S, unary_op_t op, typename A >
value_t
value(S &s, const unary_t< op, A > &u)
{
    const value_t v = value(s, u.arg);
    operate(s, op, reg_of(s, v));

    return v;
}

// The primary of + - & | ^, 6.5.7, 6.5.11 to 6.5.13: the left operand's
// value first, G13, held while the right operand's is taken in a
// register of its own; then the operation into the left's register from
// the right's, addl %ecx, %eax, add w0, w0, w1, and the right released.
// The left's register holds the node's value.
template< typename S, binary_op_t op, typename L, typename R >
value_t
value(S &s, const binary_t< op, L, R > &b)
{
    const value_t l = value(s, b.lhs);
    const value_t r = value(s, b.rhs);

    operate(s, op, reg_of(s, l), reg_of(s, r));
    release(s, r);

    return l;
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
template< typename S, typename V >
value_t
value(S &s, const assign_t< V > &a)
{
    const value_t v = value(s, a.value);
    store(s, reg_of(s, v), a.where);

    return v;
}

} // namespace rcc
