// -*- mode: c++; -*-

// The zoo, docs/typed-tree.md chapter 1: the node types under the
// handles.  A node is an aggregate of its parts, each concrete where a
// row's choice depends on it and erased where none does.  The builders
// make them, build.cc; the rows that write them are each target's,
// amd64.hh and arm64.hh, and rows-expressions.hh's and
// rows-statements.hh's for those both targets write alike.

#pragma once

#include "expr.hh"
#include "stmt.hh"

#include <vector>

namespace rcc {

// An integer constant of type int, 6.4.4.2: its row writes 32 bits.
struct literal_t
{
    int value;
};

// The constant 0 of type int, which make_literal() makes of a literal
// whose value is 0 (principle 2): a store of it names the zero register
// where the target has one.
struct zero_t
{
};

// The value of a constant, which a builder asks to fold an operator on
// it (4.3).
inline int
value_of(const literal_t &c)
{
    return c.value;
}

inline int
value_of(const zero_t &)
{
    return 0;
}

// A constant, a node whose value is known: a type value_of() is written
// for (principle 7).
template< typename C >
concept constant = requires(const C &c) { value_of(c); };

// A local, a place, 4.2: the where of an int in the function's frame,
// named by the local's number among the function's.  The displacement
// is the state's, since a slot is taken where its declaration is
// generated.
struct slot_t
{
    int id;
};

// What a place holds, 6.3.2.1p2: the value read from it, which is no
// place.
struct load_t
{
    slot_t where;
};

// -e ~e !e, 6.5.4.3p3 to p5, on an operand that is no constant, which
// the builder folds instead; the operand is erased.
template< unary_op_t op, typename A = expr_t >
struct unary_t
{
    A arg;
};

// L op R, 6.5.6 to 6.5.13, on operands that do not fold, each in the
// class a target's niches read: the builder keeps each a constant, a
// load or erased, the right one a top_t besides, and a row that
// exchanges the operands makes the node of the other order.
template< binary_op_t op, typename L = expr_t, typename R = expr_t >
struct binary_t
{
    L lhs;
    R rhs;
};

// The right operand of a binary operator whose top operator is op, a
// division or a shift by a count that is no constant: the node erased,
// with its own niches, and op kept as the class's parameter, since
// x86-64 generates the left operand away from the registers that
// instruction pins, a fact of the right operand's type (pinned_by()).
// It is a class of an operand and no node: an operator that is the
// identity leaves the node it holds, node_of().
template< binary_op_t op >
struct top_t
{
    expr_t node;
};

// x = e, 6.5.17.2: the value stored in the place, whose value after the
// store is the expression's, 6.5.17.1p3.  The value's type is a
// literal_t's or a zero_t's where a target has a niche for it, else
// erased.
template< typename V = expr_t >
struct assign_t
{
    slot_t where;
    V value;
};

// e;, 6.8.4p2: the expression evaluated for its effect, its value
// dropped.
struct expr_stmt_t
{
    expr_t e;
};

// ;, the null statement, 6.8.4p3, which writes nothing.
struct null_t
{
};

// return E;, 6.8.7.5: the value in the convention's result register,
// then the jump to the function's one epilogue.
struct return_t
{
    expr_t value;
};

// int x;, a declaration without an initializer: the local's slot taken,
// and nothing stored, 6.2.4p6.
struct decl_t
{
    slot_t where;
};

// int x = e;, a declaration with an initializer, 6.7.11p12: the slot
// taken, then the value stored in it.  An initialization, not an
// assignment (4.5); the value's type is a literal_t's or a zero_t's
// where a target has a niche for it, else erased.
template< typename V = expr_t >
struct init_t
{
    slot_t where;
    V value;
};

// A compound statement, 6.8.3: its items in order, erased, since their
// count is not bounded (4.4).
struct block_t
{
    std::vector< stmt_t > items;
};

} // namespace rcc
