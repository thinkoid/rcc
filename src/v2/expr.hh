// -*- mode: c++; -*-

// The handle of an expression, docs/typed-tree.md 3.1: an erased node
// of the zoo, held by value and shared, never written after it is
// built.  The parser holds expressions as handles and makes them
// through the builders, build.hh; the interface and the model are
// model.hh's, which build.cc alone includes, so a unit that includes
// this header cannot make a node.  An expression has one entry point,
// its value into a register the caller owns (3.4); a builder that must
// know what the handle holds goes through the model (3.3), and is a
// friend.

#pragma once

#include "stmt.hh"
#include "targets.hh"

#include <memory>

namespace rcc {

class expr_t
{
    struct interface_t;
    template< typename T >
    struct model_t;

    std::shared_ptr< const interface_t > self;

public:

    // explicit, so that a node of no row of its own never converts to
    // the handle and reaches the handle's row (3.1)
    template< typename T >
    explicit expr_t(T);

    friend void into(amd64::state_t &, const expr_t &, amd64::reg_t);
    friend void into(arm64::state_t &, const expr_t &, arm64::reg_t);

    friend expr_t rvalue(expr_t);
    friend stmt_t make_init(int slot, expr_t value);
};

// An erased operand generates itself: the handle's own rows, the value
// of the node it holds written into r, which the caller owns.  A row
// calls a child by this name whether the child's type is erased or
// concrete.
void into(amd64::state_t &, const expr_t &, amd64::reg_t r);
void into(arm64::state_t &, const expr_t &, arm64::reg_t r);

} // namespace rcc
