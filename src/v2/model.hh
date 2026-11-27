// -*- mode: c++; -*-

// The handles' interfaces and models, docs/typed-tree.md 3.1: the
// operations of each handle, one for each target in a line of its own
// (4.7), and the model that holds a node and finds its row by overload
// where the model is instantiated.  A node type with no row on a
// target does not build.  An expression's interface holds the builders
// that need the held node's type too, the hops of 3.3, which name no
// target; each finds its overload in build.cc as a row is found.
// build.cc alone includes this header, beside every row of both
// targets, so every model is instantiated in one unit and no two units
// see different rows.

#pragma once

#include "expr.hh"
#include "register-stack.hh"
#include "stmt.hh"
#include "zoo.hh"

#include <memory>
#include <stdexcept>
#include <utility>

namespace rcc {

struct expr_t::interface_t
{
    virtual ~interface_t() = default;

    virtual value_t value(amd64::state_t &, unsigned wanted) const = 0;
    virtual value_t value(arm64::state_t &, unsigned wanted) const = 0;
    virtual void effect(amd64::state_t &) const = 0;
    virtual void effect(arm64::state_t &) const = 0;
    virtual void initialize(amd64::state_t &, slot_t where) const = 0;
    virtual void initialize(arm64::state_t &, slot_t where) const = 0;
    virtual void branch(amd64::state_t &, int label, bool sense) const = 0;
    virtual void branch(arm64::state_t &, int label, bool sense) const = 0;

    // x86-64's jump on the value compared with 0, e == 0's on e
    virtual void
    value_branch(amd64::state_t &, int label, bool sense) const = 0;

    // the summaries of the subtree, x86-64's (principle 3)
    virtual unsigned pinned_within(const amd64::state_t &) const = 0;
    virtual unsigned moved_within(const amd64::state_t &) const = 0;

    // the builders: self is the handle that holds this node, which a
    // builder returns, or erases a part into, without a copy of the node
    virtual expr_t rvalue(const expr_t &self) const = 0;
    virtual expr_t unary(unary_op_t op, const expr_t &self) const = 0;

    // an assignment: the place's model, which knows the place's type,
    // makes the node
    virtual bool is_assignable() const = 0;
    virtual expr_t assign(const expr_t &value) const = 0;

    // a binary operator, by the same two hops: the right operand's
    // model, which knows the right operand's class, calls the left's
    // operation for that class, which knows the left's class and makes
    // the node
    virtual expr_t
    right_of(binary_op_t op, const expr_t &lhs, const expr_t &self) const = 0;
    virtual expr_t
    binary(binary_op_t op, const literal_t &rhs, const expr_t &self) const = 0;
    virtual expr_t
    binary(binary_op_t op, const zero_t &rhs, const expr_t &self) const = 0;
    virtual expr_t
    binary(binary_op_t op, const load_t &rhs, const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          const top_t< binary_op_t::div > &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          const top_t< binary_op_t::rem > &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          const top_t< binary_op_t::shl > &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          const top_t< binary_op_t::shr > &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t
    binary(binary_op_t op, const expr_t &rhs, const expr_t &self) const = 0;
};

// The rows of a held node, by overload and by the state's namespace: a
// member named value, effect, initialize, branch, value_branch,
// pinned_within or moved_within would hide them.  A value's row is
// handed the target's hint, the registers the consumer names.
template< typename S, typename T >
value_t
fwd_value(S &s, const T &t, unsigned wanted)
{
    return value(s, t, wanted);
}

template< typename S, typename T >
void
fwd_effect(S &s, const T &t)
{
    effect(s, t);
}

template< typename S, typename T >
void
fwd_branch(S &s, const T &t, int label, bool sense)
{
    branch(s, t, label, sense);
}

template< typename S, typename T >
void
fwd_value_branch(S &s, const T &t, int label, bool sense)
{
    value_branch(s, t, label, sense);
}

template< typename S, typename T >
void
fwd_initialize(S &s, slot_t where, const T &t)
{
    initialize(s, where, t);
}

template< typename S, typename T >
unsigned
fwd_pinned_within(const S &s, const T &t)
{
    return pinned_within(s, t);
}

template< typename S, typename T >
unsigned
fwd_moved_within(const S &s, const T &t)
{
    return moved_within(s, t);
}

// The lvalue conversion of a held node, as fwd_value() finds a row.
template< typename T >
expr_t
fwd_rvalue(const T &t, const expr_t &self)
{
    return rvalue(t, self);
}

// A unary operator on a held node: the crossing from the operator, a
// value in the parser, to the node's parameter (principle 6), the typed
// builder of each operator.
template< typename A >
expr_t
fwd_unary(unary_op_t op, const A &arg, const expr_t &self)
{
    switch (op) {
    case unary_op_t::neg: return make_unary< unary_op_t::neg >(arg, self);
    case unary_op_t::bit_not:
        return make_unary< unary_op_t::bit_not >(arg, self);
    case unary_op_t::log_not:
        return make_unary< unary_op_t::log_not >(arg, self);
    }
}

// A constant as a builder keeps it in the node it makes, by the classes
// a target has a niche for: a literal_t and a zero_t by their types,
// anything else erased, the handle that holds it.
template< typename T >
const expr_t &
classed(const T &, const expr_t &self)
{
    return self;
}

inline const literal_t &
classed(const literal_t &value, const expr_t &)
{
    return value;
}

inline const zero_t &
classed(const zero_t &value, const expr_t &)
{
    return value;
}

// An operand of a binary operator as a builder keeps it: classed()'s
// classes, and a load_t besides, which x86-64 reads as the
// instruction's operand in memory, addl -8(%rbp), %eax on the right and
// imull $5, -4(%rbp), %eax on the left.
template< typename T >
const auto &
classed_operand(const T &operand, const expr_t &self)
{
    return classed(operand, self);
}

inline const load_t &
classed_operand(const load_t &operand, const expr_t &)
{
    return operand;
}

// The right operand of a binary operator as a builder keeps it:
// classed_operand()'s classes, and a top_t besides for a node whose top
// operator x86-64 generates the left operand away from: a division,
// and a shift by a count that is no constant, the count's class read
// from the node's type.
template< typename T >
decltype(auto)
classed_right(const T &operand, const expr_t &self)
{
    return classed_operand(operand, self);
}

template< binary_op_t op, typename L, typename R >
auto
classed_right(const binary_t< op, L, R > &, const expr_t &self)
{
    if constexpr (is_division(op) || (!constant< R > && is_shift(op)))
        return top_t< op > { self };
    else
        return self;
}

// A binary operator on two operands in their classes: the crossing from
// the operator, a value in the parser, to the node's parameter
// (principle 6), made once for every class, the typed builder of each
// operator.
template< typename L, typename R >
expr_t
fwd_binary(binary_op_t op, const L &lhs, const R &rhs)
{
    switch (op) {
    case binary_op_t::add: return make_binary< binary_op_t::add >(lhs, rhs);
    case binary_op_t::sub: return make_binary< binary_op_t::sub >(lhs, rhs);
    case binary_op_t::mul: return make_binary< binary_op_t::mul >(lhs, rhs);
    case binary_op_t::div: return make_binary< binary_op_t::div >(lhs, rhs);
    case binary_op_t::rem: return make_binary< binary_op_t::rem >(lhs, rhs);
    case binary_op_t::shl: return make_binary< binary_op_t::shl >(lhs, rhs);
    case binary_op_t::shr: return make_binary< binary_op_t::shr >(lhs, rhs);
    case binary_op_t::lt: return make_binary< binary_op_t::lt >(lhs, rhs);
    case binary_op_t::gt: return make_binary< binary_op_t::gt >(lhs, rhs);
    case binary_op_t::le: return make_binary< binary_op_t::le >(lhs, rhs);
    case binary_op_t::ge: return make_binary< binary_op_t::ge >(lhs, rhs);
    case binary_op_t::eq: return make_binary< binary_op_t::eq >(lhs, rhs);
    case binary_op_t::ne: return make_binary< binary_op_t::ne >(lhs, rhs);
    case binary_op_t::bit_and:
        return make_binary< binary_op_t::bit_and >(lhs, rhs);
    case binary_op_t::bit_or:
        return make_binary< binary_op_t::bit_or >(lhs, rhs);
    case binary_op_t::bit_xor:
        return make_binary< binary_op_t::bit_xor >(lhs, rhs);
    case binary_op_t::log_and:
        return make_binary< binary_op_t::log_and >(lhs, rhs);
    case binary_op_t::log_or:
        return make_binary< binary_op_t::log_or >(lhs, rhs);
    }
}

// Whether a node is a place an assignment stores into: whether the
// builder of an assignment, make_assign(), is written for it (principle
// 8).
template< typename P >
concept assignable = requires(const P &place, const expr_t &value) {
    make_assign(place, value);
};

// The assignment to a held place, by its builder.
template< assignable P, typename V >
expr_t
fwd_assign(const P &place, const V &value)
{
    return make_assign(place, value);
}

// The parser asks is_assignable() before it builds an assignment, and
// says the error of one that is not, so no assignment to a node that is
// no place is built.
template< typename P, typename V >
expr_t
fwd_assign(const P &, const V &)
{
    throw std::logic_error("v2: an assignment to a node that is no place");
}

template< typename T >
struct expr_t::model_t final: interface_t
{
    explicit model_t(T t) : held(std::move(t))
    {
    }

    value_t value(amd64::state_t &s, unsigned wanted) const override
    {
        return fwd_value(s, held, wanted);
    }

    value_t value(arm64::state_t &s, unsigned wanted) const override
    {
        return fwd_value(s, held, wanted);
    }

    void effect(amd64::state_t &s) const override
    {
        fwd_effect(s, held);
    }

    void effect(arm64::state_t &s) const override
    {
        fwd_effect(s, held);
    }

    void branch(amd64::state_t &s, int label, bool sense) const override
    {
        fwd_branch(s, held, label, sense);
    }

    void branch(arm64::state_t &s, int label, bool sense) const override
    {
        fwd_branch(s, held, label, sense);
    }

    void value_branch(amd64::state_t &s, int label, bool sense) const override
    {
        fwd_value_branch(s, held, label, sense);
    }

    void initialize(amd64::state_t &s, slot_t where) const override
    {
        fwd_initialize(s, where, held);
    }

    void initialize(arm64::state_t &s, slot_t where) const override
    {
        fwd_initialize(s, where, held);
    }

    unsigned pinned_within(const amd64::state_t &s) const override
    {
        return fwd_pinned_within(s, held);
    }

    unsigned moved_within(const amd64::state_t &s) const override
    {
        return fwd_moved_within(s, held);
    }

    expr_t rvalue(const expr_t &self) const override
    {
        return fwd_rvalue(held, self);
    }

    expr_t unary(unary_op_t op, const expr_t &self) const override
    {
        return fwd_unary(op, held, self);
    }

    bool is_assignable() const override
    {
        return assignable< T >;
    }

    expr_t assign(const expr_t &value) const override
    {
        return fwd_assign(held, value);
    }

    expr_t right_of(binary_op_t op,
                    const expr_t &lhs,
                    const expr_t &self) const override
    {
        return lhs.self->binary(op, classed_right(held, self), lhs);
    }

    expr_t binary(binary_op_t op,
                  const literal_t &rhs,
                  const expr_t &self) const override
    {
        return fwd_binary(op, classed_operand(held, self), rhs);
    }

    expr_t
    binary(binary_op_t op, const zero_t &rhs, const expr_t &self) const override
    {
        return fwd_binary(op, classed_operand(held, self), rhs);
    }

    expr_t
    binary(binary_op_t op, const load_t &rhs, const expr_t &self) const override
    {
        return fwd_binary(op, classed_operand(held, self), rhs);
    }

    expr_t binary(binary_op_t op,
                  const top_t< binary_op_t::div > &rhs,
                  const expr_t &self) const override
    {
        return fwd_binary(op, classed_operand(held, self), rhs);
    }

    expr_t binary(binary_op_t op,
                  const top_t< binary_op_t::rem > &rhs,
                  const expr_t &self) const override
    {
        return fwd_binary(op, classed_operand(held, self), rhs);
    }

    expr_t binary(binary_op_t op,
                  const top_t< binary_op_t::shl > &rhs,
                  const expr_t &self) const override
    {
        return fwd_binary(op, classed_operand(held, self), rhs);
    }

    expr_t binary(binary_op_t op,
                  const top_t< binary_op_t::shr > &rhs,
                  const expr_t &self) const override
    {
        return fwd_binary(op, classed_operand(held, self), rhs);
    }

    expr_t
    binary(binary_op_t op, const expr_t &rhs, const expr_t &self) const override
    {
        return fwd_binary(op, classed_operand(held, self), rhs);
    }

    T held;
};

template< typename T >
expr_t::expr_t(T t) : self(std::make_shared< const model_t< T > >(std::move(t)))
{
}

struct stmt_t::interface_t
{
    virtual ~interface_t() = default;

    virtual void generate(amd64::state_t &) const = 0;
    virtual void generate(arm64::state_t &) const = 0;

    // the builders of an if: whose then part is this statement, self,
    // with an else part or without one; and whose then part is a block
    // of which this statement is the one item, the second hop of 3.3
    virtual stmt_t if_then(const expr_t &cond,
                           const std::optional< stmt_t > &otherwise,
                           const stmt_t &self) const = 0;
    virtual stmt_t if_item(const expr_t &cond,
                           const std::optional< stmt_t > &otherwise,
                           const stmt_t &block) const = 0;
};

// The row of a held statement, as fwd_value() finds an expression's.
template< typename S, typename T >
void
fwd_generate(S &s, const T &t)
{
    generate(s, t);
}

template< typename T >
struct stmt_t::model_t final: interface_t
{
    explicit model_t(T t) : held(std::move(t))
    {
    }

    void generate(amd64::state_t &s) const override
    {
        fwd_generate(s, held);
    }

    void generate(arm64::state_t &s) const override
    {
        fwd_generate(s, held);
    }

    stmt_t if_then(const expr_t &cond,
                   const std::optional< stmt_t > &otherwise,
                   const stmt_t &self) const override
    {
        return make_if(cond, held, self, otherwise);
    }

    stmt_t if_item(const expr_t &cond,
                   const std::optional< stmt_t > &otherwise,
                   const stmt_t &block) const override
    {
        return make_if_of_item(cond, held, block, otherwise);
    }

    T held;
};

template< typename T >
stmt_t::stmt_t(T t) : self(std::make_shared< const model_t< T > >(std::move(t)))
{
}

} // namespace rcc
