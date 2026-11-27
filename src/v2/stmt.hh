// -*- mode: c++; -*-

// The handle of a statement, docs/typed-tree.md 4.4: an erased node of
// the zoo, by the technique of expr.hh, with one entry point, the
// statement generated.  Statements and expressions are two handles,
// since no operation is whole across both (principle 10).  The builder
// of an if goes through the model of its then part (3.3), and is a
// friend.

#pragma once

#include "targets.hh"

#include <memory>
#include <optional>

namespace rcc {

class expr_t;

class stmt_t
{
    struct interface_t;
    template< typename T >
    struct model_t;

    std::shared_ptr< const interface_t > self;

public:

    // explicit, as expr_t's is: a block_t never converts to the handle
    // and reaches the handle's row
    template< typename T >
    explicit stmt_t(T);

    friend void generate(amd64::state_t &, const stmt_t &);
    friend void generate(arm64::state_t &, const stmt_t &);

    friend stmt_t make_if(expr_t cond, stmt_t then);
    friend stmt_t make_if_else(expr_t cond, stmt_t then, stmt_t otherwise);
    friend stmt_t if_on_item(const stmt_t &item,
                             const expr_t &cond,
                             const std::optional< stmt_t > &otherwise,
                             const stmt_t &block);
};

// An erased statement generates itself: the handle's own rows.
void generate(amd64::state_t &, const stmt_t &);
void generate(arm64::state_t &, const stmt_t &);

} // namespace rcc
