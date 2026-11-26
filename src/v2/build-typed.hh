// -*- mode: c++; -*-

// The typed builders, docs/typed-tree.md 3.3: the overloads on the
// node types that a builder of build.hh reaches through a model, where
// a part's type is complete.  The overload chosen makes the node, so a
// rule of C and a shape of the zoo are decided here: which node a
// place's value is, what an assignment stores into, in which class a
// value is kept, what an operator on constants folds to.  A model finds
// a typed builder as it finds a row, by overload where it is
// instantiated, so build.cc includes this header beside the rows.

#pragma once

#include "build.hh"
#include "expr.hh"
#include "stmt.hh"
#include "zoo.hh"

#include <cstdint>

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

// An assignment's builder exists for a place alone, a local's slot, and
// so defines what is assignable (principle 8); its value is in its
// class, as an initializer's.
template< typename V >
expr_t
make_assign(const slot_t &where, const V &value)
{
    return expr_t(assign_t< V > { where, value });
}

// The value of an operation of int on constants, as rcc folds it: the
// exact value wrapped to 32 bits, as the instruction wraps it (G1),
// where 6.5.1p5 leaves a signed overflow undefined.
inline int
wrapped(long long exact)
{
    return static_cast< int >(static_cast< std::uint32_t >(exact));
}

// - and ~ of a constant, 6.5.4.3p3, p4: the negative of INT_MIN wraps
// to INT_MIN.
inline int
folded(unary_op_t op, int value)
{
    switch (op) {
    case unary_op_t::neg: return wrapped(-static_cast< long long >(value));
    case unary_op_t::bit_not: return ~value;
    }
}

// - and ~ of anything but a constant: the node, its operand erased
// (3.3).
template< unary_op_t op, typename A >
expr_t
make_unary(const A &, const expr_t &arg)
{
    return expr_t(unary_t< op > { arg });
}

// - and ~ of a constant fold where the node would be built, as rcc
// folds them: a constant, which make_literal() classes.
template< unary_op_t op, constant C >
expr_t
make_unary(const C &arg, const expr_t &)
{
    return make_literal(folded(op, value_of(arg)));
}

} // namespace rcc
