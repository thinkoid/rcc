// -*- mode: c++; -*-

// The typed builders, docs/typed-tree.md 3.3: the overloads on the
// node types that a builder of build.hh reaches through a model, where
// a part's type is complete.  The overload chosen makes the node, so a
// rule of C and a shape of the zoo are decided here: which node a
// place's value is, in which class a value is kept.  A model finds a
// typed builder as it finds a row, by overload where it is
// instantiated, so build.cc includes this header beside the rows.

#pragma once

#include "expr.hh"
#include "stmt.hh"
#include "zoo.hh"

namespace rcc {

// A node that is no place is a value already, and the lvalue conversion
// leaves it the node it is: its own handle.
template< typename T >
expr_t
rvalue(const T &, const expr_t &self)
{
    return self;
}

// A local becomes the value its slot holds, 6.3.2.1p2.
inline expr_t
rvalue(const slot_t &where, const expr_t &)
{
    return expr_t(load_t { where });
}

// The value in its class, classed()'s: init_t< literal_t >,
// init_t< zero_t > or the primary, init_t< expr_t >.
template< typename V >
stmt_t
make_init(slot_t where, const V &value)
{
    return stmt_t(init_t< V > { where, value });
}

} // namespace rcc
