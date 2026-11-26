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
#include "stmt.hh"
#include "zoo.hh"

#include <memory>
#include <stdexcept>
#include <utility>

namespace rcc {

struct expr_t::interface_t
{
    virtual ~interface_t() = default;

    virtual void into(amd64::state_t &, amd64::reg_t) const = 0;
    virtual void into(arm64::state_t &, arm64::reg_t) const = 0;
    virtual void effect(amd64::state_t &) const = 0;
    virtual void effect(arm64::state_t &) const = 0;

    // the builders: self is the handle that holds this node, which a
    // builder returns, or erases a part into, without a copy of the node
    virtual expr_t rvalue(const expr_t &self) const = 0;
    virtual stmt_t init(slot_t where, const expr_t &self) const = 0;

    // an assignment, the second hop of 3.3: the value's model, which
    // knows the value's class, calls the place's operation for that
    // class, which knows the place's type and makes the node
    virtual bool is_assignable() const = 0;
    virtual expr_t assigned_to(const expr_t &place,
                               const expr_t &self) const = 0;
    virtual expr_t assign(const literal_t &value) const = 0;
    virtual expr_t assign(const zero_t &value) const = 0;
    virtual expr_t assign(const expr_t &value) const = 0;
};

// The rows of a held node, by overload and by the state's namespace: a
// member named into or effect would hide them.
template< typename S, typename T, typename R >
void
fwd_into(S &s, const T &t, R r)
{
    into(s, t, r);
}

template< typename S, typename T >
void
fwd_effect(S &s, const T &t)
{
    effect(s, t);
}

// The lvalue conversion of a held node, as fwd_into() finds a row.
template< typename T >
expr_t
fwd_rvalue(const T &t, const expr_t &self)
{
    return rvalue(t, self);
}

// A value as a builder keeps it in the node it makes, by the classes of
// value a target has a niche for: a literal_t and a zero_t by their
// types, anything else erased, the handle that holds it.
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

    void into(amd64::state_t &s, amd64::reg_t r) const override
    {
        fwd_into(s, held, r);
    }

    void into(arm64::state_t &s, arm64::reg_t r) const override
    {
        fwd_into(s, held, r);
    }

    void effect(amd64::state_t &s) const override
    {
        fwd_effect(s, held);
    }

    void effect(arm64::state_t &s) const override
    {
        fwd_effect(s, held);
    }

    expr_t rvalue(const expr_t &self) const override
    {
        return fwd_rvalue(held, self);
    }

    stmt_t init(slot_t where, const expr_t &self) const override
    {
        return make_init(where, classed(held, self));
    }

    bool is_assignable() const override
    {
        return assignable< T >;
    }

    expr_t assigned_to(const expr_t &place, const expr_t &self) const override
    {
        return place.self->assign(classed(held, self));
    }

    expr_t assign(const literal_t &value) const override
    {
        return fwd_assign(held, value);
    }

    expr_t assign(const zero_t &value) const override
    {
        return fwd_assign(held, value);
    }

    expr_t assign(const expr_t &value) const override
    {
        return fwd_assign(held, value);
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
};

// The row of a held statement, as fwd_into() finds an expression's.
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

    T held;
};

template< typename T >
stmt_t::stmt_t(T t) : self(std::make_shared< const model_t< T > >(std::move(t)))
{
}

} // namespace rcc
