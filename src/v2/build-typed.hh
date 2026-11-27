// -*- mode: c++; -*-

// The typed builders, docs/typed-tree.md 3.3: the overloads on the
// node types that a builder of build.hh reaches through a model, where
// a part's type is complete.  The overload chosen makes the node, so a
// rule of C and a shape of the zoo are decided here: which node a
// place's value is, what an assignment stores into, in which class a
// value is kept, what an operator on constants folds to, which operator
// is the identity and leaves its operand, whether an if's then part is
// one jump.  A model finds a typed builder as it finds a row, by
// overload where it is instantiated, so build.cc includes this header
// beside the rows.

#pragma once

#include "build.hh"
#include "expr.hh"
#include "stmt.hh"
#include "zoo.hh"

#include <cstdint>
#include <limits>
#include <optional>

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

// An assignment's builder exists for a place alone, a local's slot, and
// so defines what is assignable (principle 8).
inline expr_t
make_assign(const slot_t &where, const expr_t &value)
{
    return expr_t(assign_t { where, value });
}

// The value of an operation of int on constants, as rcc folds it: the
// exact value wrapped to 32 bits, as the instruction wraps it (G1),
// where 6.5.1p5 leaves a signed overflow undefined.
inline int
wrapped(long long exact)
{
    return static_cast< int >(static_cast< std::uint32_t >(exact));
}

// - ~ ! of a constant, 6.5.4.3p3 to p5: the negative of INT_MIN wraps
// to INT_MIN, and !c is 1 where c is 0 and 0 otherwise.
inline int
folded(unary_op_t op, int value)
{
    switch (op) {
    case unary_op_t::neg: return wrapped(-static_cast< long long >(value));
    case unary_op_t::bit_not: return ~value;
    case unary_op_t::log_not: return value == 0;
    }
}

// - ~ ! of anything but a constant: the node, its operand erased (3.3).
template< unary_op_t op, typename A >
expr_t
make_unary(const A &, const expr_t &arg)
{
    return expr_t(unary_t< op > { arg });
}

// - ~ ! of a constant fold where the node would be built, as rcc folds
// them: a constant, which make_literal() classes.
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

// Whether the division of lhs by rhs has a value, rcc's divides(): not
// by zero, 6.5.6p6, and not with a quotient int cannot hold, p7, which
// is the one case of the most negative int divided by -1; the remainder
// of that division has no value either.  The instruction traps on both
// at run time (G4).
inline bool
divides(int lhs, int rhs)
{
    return rhs != 0 && !(lhs == std::numeric_limits< int >::min() && rhs == -1);
}

// * / % + - << >> & | ^ of two constants, 6.5.6p5 to p7, 6.5.7p6, p7,
// 6.5.8p4, p5, 6.5.11p4, 6.5.12p4, 6.5.13p4.  / and % on a division
// that divides() says has a value: the quotient truncates toward zero
// and the remainder has the dividend's sign.  << shifts the bits and
// wraps, and >> of a negative value is arithmetic, G3, the
// implementation's choice by 6.5.8p5.  A comparison is 1 where its
// relation holds and 0 where it does not, 6.5.9p7, 6.5.10p4, and so are
// && and || of the operands compared with 0, 6.5.14p3, 6.5.15p3.
inline int
folded(binary_op_t op, int lhs, int rhs)
{
    const long long a = lhs;
    const long long b = rhs;

    switch (op) {
    case binary_op_t::add: return wrapped(a + b);
    case binary_op_t::sub: return wrapped(a - b);
    case binary_op_t::mul: return wrapped(a * b);
    case binary_op_t::div: return lhs / rhs;
    case binary_op_t::rem: return lhs % rhs;
    case binary_op_t::shl: return wrapped(a << shift_count(rhs));
    case binary_op_t::shr: return lhs >> shift_count(rhs);
    case binary_op_t::lt: return lhs < rhs;
    case binary_op_t::gt: return lhs > rhs;
    case binary_op_t::le: return lhs <= rhs;
    case binary_op_t::ge: return lhs >= rhs;
    case binary_op_t::eq: return lhs == rhs;
    case binary_op_t::ne: return lhs != rhs;
    case binary_op_t::bit_and: return lhs & rhs;
    case binary_op_t::bit_or: return lhs | rhs;
    case binary_op_t::bit_xor: return lhs ^ rhs;
    case binary_op_t::log_and: return lhs != 0 && rhs != 0;
    case binary_op_t::log_or: return lhs != 0 || rhs != 0;
    }
}

// Whether the constant c on the right of op makes op the identity, as
// rcc's is_identity_by() says: + - | ^ by 0, * / by 1, & by -1, every
// bit of an int, << >> by a count that is 0 modulo 32, so x << 32 is x
// and x << 33 is no identity; % by none, x % 1 being 0, and a
// comparison, && and || by none, their value being 0 or 1, so x && 1
// is the node, its constant loaded and tested.
inline bool
is_identity_by(binary_op_t op, int c)
{
    switch (op) {
    case binary_op_t::add:
    case binary_op_t::sub:
    case binary_op_t::bit_or:
    case binary_op_t::bit_xor: return c == 0;
    case binary_op_t::mul:
    case binary_op_t::div: return c == 1;
    case binary_op_t::rem:
    case binary_op_t::lt:
    case binary_op_t::gt:
    case binary_op_t::le:
    case binary_op_t::ge:
    case binary_op_t::eq:
    case binary_op_t::ne:
    case binary_op_t::log_and:
    case binary_op_t::log_or: return false;
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

// A binary operator of operands neither of which is a constant: the
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
// 0 - x, 0 << x and 1 / x are the node.
template< binary_op_t op, constant L, typename R >
expr_t
make_binary(const L &lhs, const R &rhs)
{
    if (commutes(op) && is_identity_by(op, value_of(lhs)))
        return node_of(rhs);

    return expr_t(binary_t< op, L, R > { lhs, rhs });
}

// Two constants fold where the node would be built, as rcc folds them:
// a constant, whatever the operator, but for a division that has no
// value, divides(), which is the node with its two constants, its
// instruction written: 1 / 0 is movl $1, %eax, movl $0, %ecx, cltd and
// idivl %ecx.
template< binary_op_t op, constant L, constant R >
expr_t
make_binary(const L &lhs, const R &rhs)
{
    if constexpr (is_division(op)) {
        if (!divides(value_of(lhs), value_of(rhs)))
            return expr_t(binary_t< op, L, R > { lhs, rhs });
    }

    return make_literal(folded(op, value_of(lhs), value_of(rhs)));
}

// An if of the then part then, a node's type S: with the else part
// otherwise where there is one.
template< typename S >
stmt_t
if_node(const expr_t &cond,
        const S &then,
        const std::optional< stmt_t > &otherwise)
{
    if (otherwise)
        return stmt_t(if_else_t< S > { cond, then, *otherwise });

    return stmt_t(if_t< S > { cond, then });
}

// An if whose then part is any statement but one jump: the node with
// the then part erased, the handle that holds it.
template< typename T >
stmt_t
make_if(const expr_t &cond,
        const T &,
        const stmt_t &then,
        const std::optional< stmt_t > &otherwise)
{
    return if_node(cond, then, otherwise);
}

// A then part that is one jump, one_jump's, is the node's own type,
// docs/backend.md 7.2: the condition's jump to its label, as the first
// pass of the Seventh Edition compiler writes it, c02.c.
template< one_jump J >
stmt_t
make_if(const expr_t &cond,
        const J &then,
        const stmt_t &,
        const std::optional< stmt_t > &otherwise)
{
    return if_node(cond, then, otherwise);
}

// The second hop of an if's builder, build.cc's: through the model of
// item, the one item of the block that is the then part.
stmt_t if_on_item(const stmt_t &item,
                  const expr_t &cond,
                  const std::optional< stmt_t > &otherwise,
                  const stmt_t &block);

// A block of one item is one jump where its item is one, the braces
// writing nothing: the answer is the item's, asked through its model,
// if_on_item(), and a block of any other count is no jump.
inline stmt_t
make_if(const expr_t &cond,
        const block_t &b,
        const stmt_t &then,
        const std::optional< stmt_t > &otherwise)
{
    if (b.items.size() != 1)
        return if_node(cond, then, otherwise);

    return if_on_item(b.items.front(), cond, otherwise, then);
}

// The one item of a block that is a then part, asked whether it is one
// jump: a type of one_jump is, and any other statement is not, a block
// among them, so { { break; } } is no jump and the then part is the
// block.
template< typename T >
stmt_t
make_if_of_item(const expr_t &cond,
                const T &,
                const stmt_t &block,
                const std::optional< stmt_t > &otherwise)
{
    return if_node(cond, block, otherwise);
}

template< one_jump J >
stmt_t
make_if_of_item(const expr_t &cond,
                const J &item,
                const stmt_t &,
                const std::optional< stmt_t > &otherwise)
{
    return if_node(cond, item, otherwise);
}

} // namespace rcc
