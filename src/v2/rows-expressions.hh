// -*- mode: c++; -*-

// The rows of the expressions that both targets write alike: templates
// over the target's state, which name no register and write no text of
// their own.  into(), a value written in a register the caller owns, is
// the entry point with no primary (docs/typed-tree.md 3.4); value() is
// derived from it, a register taken and into(), and so is effect()'s
// primary, a value dropped.  build.cc includes this header beside the
// targets' rows.

#pragma once

#include "register-stack.hh"
#include "zoo.hh"

#include <stdexcept>

namespace rcc {

// A value is a register taken and into(), 3.4: the node's value written
// in a register taken for it, which the caller releases.  An erased
// node is served through the handle's into() row, as a concrete one is.
template< typename S, typename T >
value_t
value(S &s, const T &t)
{
    const value_t v = fresh(s);
    into(s, t, reg_of(s, v));

    return v;
}

// A place is read only through its load, the lvalue conversion the
// parser asks of every operand whose value it uses, rvalue(): a slot
// asked for its value is a tree the parser does not make.
template< typename S, typename R >
void
into(S &, const slot_t &, R)
{
    throw std::logic_error("v2: a place read without its lvalue conversion");
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
// in a register taken for it, which is released at once.
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

// An assignment as a value: the value into r, then stored from r, which
// holds the place's value after the store, 6.5.17.1p3.  So a constant
// goes through r here, on x86-64 too, and 0 is no store of wzr.
template< typename S, typename V, typename R >
void
into(S &s, const assign_t< V > &a, R r)
{
    into(s, a.value, r);
    store(s, r, a.where);
}

} // namespace rcc
