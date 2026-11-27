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

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <utility>

namespace rcc {

// A constant on the right of a binary operator as it crosses the
// interface, the class as data (docs/typed-tree.md 3.3): its value and
// its view, which the left operand's model makes a constant again,
// typed(): of the operation's view, since the parser converted it to
// the type of the operation, but for an operand of && or ||, which
// keeps its own.  A shift's constant count is made one of the
// operation's view too, since its rows read its low bits alone.
struct constant_class_t
{
    long long value;
    view_t view;
};

// A load on the right as it crosses: its where, which the left
// operand's model makes a load of the operation's view again.
struct load_class_t
{
    slot_t where;
};

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

    // the summaries both targets read (principle 3): the width rule and
    // the view
    virtual int valid() const = 0;
    virtual view_t view() const = 0;

    // the builders: self is the handle that holds this node, which a
    // builder returns, or erases a part into, without a copy of the node
    virtual expr_t rvalue(const expr_t &self) const = 0;
    virtual expr_t unary(unary_op_t op, view_t, const expr_t &self) const = 0;

    // a conversion, by the operand's model, which knows the operand's
    // type and so whether it is a load (rcc's widened_read())
    virtual expr_t
    conversion(view_t to, view_t from, const expr_t &self) const = 0;

    // an assignment: the place's model, which knows the place's type,
    // makes the node
    virtual bool is_assignable() const = 0;
    virtual expr_t assign(const expr_t &value) const = 0;

    // a binary operator, by the same two hops: the right operand's
    // model, which knows the right operand's class, calls the left's
    // operation for that class, which knows the left's class and makes
    // the node; the view is the operation's, at which the left's model
    // makes the class a type again
    virtual expr_t right_of(binary_op_t op,
                            view_t,
                            const expr_t &lhs,
                            const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const constant_class_t &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const load_class_t &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const top_t< family_t::division > &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const top_t< family_t::shift > &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const expr_t &rhs,
                          const expr_t &self) const = 0;
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

// A unary operator on a held node: the crossing from the operator and
// the view, values in the parser, to the node's parameters (principle
// 6), the typed builder of each.
template< typename A >
expr_t
fwd_unary(unary_op_t op, view_t view, const A &arg, const expr_t &self)
{
    return cross(op, [&]< unary_op_t o >() {
        return cross(
            view, [&]< view_t V >() { return make_unary< o, V >(arg, self); });
    });
}

// The conversion of a held node from the view from to the view to: the
// two crossings, then the typed builder, by overload on the node's type.
template< typename T >
expr_t
fwd_conversion(view_t to, view_t from, const T &e, const expr_t &self)
{
    return cross(to, [&]< view_t V >() {
        return cross(from, [&]< view_t F >() {
            return make_conversion< V, F >(e, self);
        });
    });
}

// A constant of view V by its value, f handed the node: the one
// question asked of a constant's value (principle 2), whose answer is
// its type.  0 is a zero_t, a 64-bit value past int's range a wide_t,
// and any other a literal_t.
template< view_t V, typename F >
decltype(auto)
classed_constant(long long value, F &&f)
{
    if (value == 0)
        return f(zero_t< V > { });

    if constexpr (V == view_t::i64) {
        if (value < INT32_MIN || value > INT32_MAX)
            return f(wide_t { value });
    }

    return f(literal_t< V > { value });
}

// An operand of a binary operator as a builder keeps it on the left, in
// the classes a target has a niche for: a constant and a load of any
// view, since the operands of && and || keep their own types, and a
// trunc_t of a load's own view, which keeps the object's bits, as its
// load, rcc's memory_operand() under under_kept_bits(); anything else
// erased, the handle that holds it.
template< typename T >
const expr_t &
classed_operand(const T &, const expr_t &self)
{
    return self;
}

template< constant C >
const C &
classed_operand(const C &c, const expr_t &)
{
    return c;
}

template< view_t V >
const load_t< V > &
classed_operand(const load_t< V > &l, const expr_t &)
{
    return l;
}

template< view_t V >
const load_t< V > &
classed_operand(const trunc_t< V, load_t< V > > &t, const expr_t &)
{
    return t.arg;
}

// The right operand of a binary operator as it crosses the interface,
// its class as data: a constant's value and a load's where, a load
// under a trunc_t of its own view among them; a top_t for a node whose
// top operator, read through its conversions, x86-64 generates the left
// operand away from, a division and a shift by a count that is no
// constant, pinning()'s; anything else erased.
template< typename T >
decltype(auto)
crossing(const T &e, const expr_t &self)
{
    return pinning(e, self);
}

template< constant C >
constant_class_t
crossing(const C &c, const expr_t &)
{
    return { value_of(c), view_of(c) };
}

template< view_t V >
load_class_t
crossing(const load_t< V > &l, const expr_t &)
{
    return { l.where };
}

template< view_t V >
load_class_t
crossing(const trunc_t< V, load_t< V > > &t, const expr_t &)
{
    return { t.arg.where };
}

// Whether a right operand of a class crosses erased all the same, a
// question of the operator, data here, asked once (3.3): a shift's count
// and an operand of && or || keep their own types, so their load is no
// operand of the operation's view.  A constant crosses as one, so that
// two constants fold.
constexpr bool
crosses_erased(binary_op_t op, const load_class_t &)
{
    return is_shift(op) || is_logical(op);
}

template< typename C >
constexpr bool
crosses_erased(binary_op_t, const C &)
{
    return false;
}

// The right operand's class made a type again for op at the
// operation's view V, and handed to f: a constant by its value, at V,
// or at its own view for && and ||; a load at V; a top_t and an erased
// operand as they are.
template< binary_op_t op, view_t V, typename F >
expr_t
typed(const constant_class_t &c, F &&f)
{
    if constexpr (is_logical(op))
        return cross(c.view, [&]< view_t W >() {
            return classed_constant< W >(c.value, f);
        });
    else
        return classed_constant< V >(c.value, f);
}

template< binary_op_t op, view_t V, typename F >
expr_t
typed(const load_class_t &l, F &&f)
{
    return f(load_t< V > { l.where });
}

template< binary_op_t op, view_t V, typename R, typename F >
expr_t
typed(const R &rhs, F &&f)
{
    return f(rhs);
}

// A binary operator on two operands in their classes: the crossing from
// the operator and the view, values in the parser, to the node's
// parameters (principle 6), and the right operand's class made a type
// again at the view, made once for every pair of classes, the typed
// builder of each.
template< typename L, typename R >
expr_t
fwd_binary(binary_op_t op, view_t view, const L &lhs, const R &rhs)
{
    return cross(op, [&]< binary_op_t o >() {
        return cross(view, [&]< view_t V >() {
            return typed< o, V >(rhs, [&](const auto &r) {
                return make_binary< o, V >(lhs, r);
            });
        });
    });
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

    int valid() const override
    {
        return valid_of(held);
    }

    view_t view() const override
    {
        return view_of(held);
    }

    expr_t rvalue(const expr_t &self) const override
    {
        return fwd_rvalue(held, self);
    }

    expr_t unary(unary_op_t op, view_t view, const expr_t &self) const override
    {
        return fwd_unary(op, view, held, self);
    }

    expr_t conversion(view_t to, view_t from, const expr_t &self) const override
    {
        return fwd_conversion(to, from, held, self);
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
                    view_t view,
                    const expr_t &lhs,
                    const expr_t &self) const override
    {
        const auto &rhs = crossing(held, self);
        if (crosses_erased(op, rhs))
            return lhs.self->binary(op, view, self, lhs);

        return lhs.self->binary(op, view, rhs, lhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const constant_class_t &rhs,
                  const expr_t &self) const override
    {
        return fwd_binary(op, view, classed_operand(held, self), rhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const load_class_t &rhs,
                  const expr_t &self) const override
    {
        return fwd_binary(op, view, classed_operand(held, self), rhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const top_t< family_t::division > &rhs,
                  const expr_t &self) const override
    {
        return fwd_binary(op, view, classed_operand(held, self), rhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const top_t< family_t::shift > &rhs,
                  const expr_t &self) const override
    {
        return fwd_binary(op, view, classed_operand(held, self), rhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const expr_t &rhs,
                  const expr_t &self) const override
    {
        return fwd_binary(op, view, classed_operand(held, self), rhs);
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
