// -*- mode: c++; -*-

// The handles' interfaces and models, docs/typed-tree.md 3.1: the
// operations of each handle, one for each target in a line of its own
// (4.7), and the model that holds a node and finds its row by overload
// where the model is instantiated.  A node type with no row on a
// target does not build.  build.cc alone includes this header, beside
// every row of both targets, so every model is instantiated in one
// unit and no two units see different rows.

#pragma once

#include "expr.hh"
#include "stmt.hh"

#include <memory>
#include <utility>

namespace rcc {

struct expr_t::interface_t
{
    virtual ~interface_t() = default;

    virtual void into(amd64::state_t &, amd64::reg_t) const = 0;
    virtual void into(arm64::state_t &, arm64::reg_t) const = 0;
};

// The row of a held node, by overload and by the state's namespace: a
// member named into would hide it.
template< typename S, typename T, typename R >
void
fwd_into(S &s, const T &t, R r)
{
    into(s, t, r);
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
