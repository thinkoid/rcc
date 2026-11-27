// -*- mode: c++; -*-

// The typed builders, docs/typed-tree.md 3.3: the overloads on the
// node types that a builder of build.hh reaches through a model, where
// a part's type is complete.  The overload chosen makes the node, so a
// rule of C and a shape of the zoo are decided here: which node a
// place's value is, what an assignment stores into, in which class a
// value is kept, what an operator on constants folds to, which operator
// is the identity and leaves its operand.  A model finds a typed
// builder as it finds a row, by overload where it is instantiated, so
// build.cc includes this header beside the rows.

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

// The count of a shift of an int, as rcc's shift_count() takes it: the
// count modulo 32, as the instruction masks it (G3), where 6.5.8p3
// leaves a count that is negative or not less than 32 undefined.
inline int
shift_count(int count)
{
    return count & 31;
}

// * + - << >> & | ^ of two constants, 6.5.6p5, 6.5.7p6, p7, 6.5.8p4,
// p5, 6.5.11p4, 6.5.12p4, 6.5.13p4.  << shifts the bits and wraps, and
// >> of a negative value is arithmetic, G3, the implementation's
// choice by 6.5.8p5.
inline int
folded(binary_op_t op, int lhs, int rhs)
{
    const long long a = lhs;
    const long long b = rhs;

    switch (op) {
    case binary_op_t::add: return wrapped(a + b);
    case binary_op_t::sub: return wrapped(a - b);
    case binary_op_t::mul: return wrapped(a * b);
    case binary_op_t::shl: return wrapped(a << shift_count(rhs));
    case binary_op_t::shr: return lhs >> shift_count(rhs);
    case binary_op_t::bit_and: return lhs & rhs;
    case binary_op_t::bit_or: return lhs | rhs;
    case binary_op_t::bit_xor: return lhs ^ rhs;
    }
}

// Whether the constant c on the right of op makes op the identity, as
// rcc's is_identity_by() says: + - | ^ by 0, * by 1, & by -1, every bit
// of an int, << >> by a count that is 0 modulo 32, so x << 32 is x and
// x << 33 is no identity.
inline bool
is_identity_by(binary_op_t op, int c)
{
    switch (op) {
    case binary_op_t::add:
    case binary_op_t::sub:
    case binary_op_t::bit_or:
    case binary_op_t::bit_xor: return c == 0;
    case binary_op_t::mul: return c == 1;
    case binary_op_t::shl:
    case binary_op_t::shr: return shift_count(c) == 0;
    case binary_op_t::bit_and: return c == -1;
    }
}

// The node an operand is, out of the class a builder kept it in: a
// top_t holds its node, and any other class is a node of its own.
template< typename T >
expr_t
node_of(const T &operand)
{
    return expr_t(operand);
}

template< binary_op_t op >
expr_t
node_of(const top_t< op > &operand)
{
    return operand.node;
}

// * + - << >> & | ^ of operands neither of which is a constant: the
// node, each operand in its class.
template< binary_op_t op, typename L, typename R >
expr_t
make_binary(const L &lhs, const R &rhs)
{
    return expr_t(binary_t< op, L, R > { lhs, rhs });
}

// A constant on the right that makes the operator the identity, x + 0,
// leaves the other operand, the node it is; rcc drops such a node in a
// pass after the body is parsed, drop_identities(), and rcc-v2 where it
// would be built.  Any other constant is the node's.
template< binary_op_t op, typename L, constant R >
expr_t
make_binary(const L &lhs, const R &rhs)
{
    if (is_identity_by(op, value_of(rhs)))
        return expr_t(lhs);

    return expr_t(binary_t< op, L, R > { lhs, rhs });
}

// A constant on the left makes the operator the identity as one on the
// right does, but for an operator that commutes alone: 0 + x is x, and
// 0 - x and 0 << x are the node.
template< binary_op_t op, constant L, typename R >
expr_t
make_binary(const L &lhs, const R &rhs)
{
    if (commutes(op) && is_identity_by(op, value_of(lhs)))
        return node_of(rhs);

    return expr_t(binary_t< op, L, R > { lhs, rhs });
}

// Two constants fold where the node would be built, as rcc folds them:
// a constant, whatever the operator.
template< binary_op_t op, constant L, constant R >
expr_t
make_binary(const L &lhs, const R &rhs)
{
    return make_literal(folded(op, value_of(lhs), value_of(rhs)));
}

} // namespace rcc
