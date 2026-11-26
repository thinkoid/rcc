// -*- mode: c++; -*-

// The handle of a statement, docs/typed-tree.md 4.4: an erased node of
// the zoo, by the technique of expr.hh, with one entry point, the
// statement generated.  Statements and expressions are two handles,
// since no operation is whole across both (principle 10).

#pragma once

#include "targets.hh"

#include <memory>

namespace rcc {

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
};

// An erased statement generates itself: the handle's own rows.
void generate(amd64::state_t &, const stmt_t &);
void generate(arm64::state_t &, const stmt_t &);

} // namespace rcc
