// -*- mode: c++; -*-

// The handle of an expression, docs/typed-tree.md 3.1: an erased node
// of the zoo, held by value and shared, never written after it is
// built.  The parser holds expressions as handles and makes them
// through the builders, build.hh; the interface and the model are
// model.hh's, which build.cc alone includes, so a unit that includes
// this header cannot make a node.  An expression has two entry points
// (3.4): its value, held in a register its row takes, and its effect,
// the expression evaluated for what it does and its value dropped; and
// it hands up one summary, x86-64's, the registers the instructions
// within it pin.  A builder that must know what the handle holds goes
// through the model (3.3), and is a friend.

#pragma once

#include "operators.hh"
#include "stmt.hh"
#include "targets.hh"

#include <memory>

namespace rcc {

// A held value, register-stack.hh's: the handle's rows return one, and
// a unit that holds handles and generates none never sees its
// definition.
struct value_t;

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

    friend value_t value(amd64::state_t &, const expr_t &, unsigned wanted);
    friend value_t value(arm64::state_t &, const expr_t &);
    friend void effect(amd64::state_t &, const expr_t &);
    friend void effect(arm64::state_t &, const expr_t &);
    friend unsigned pinned_within(const amd64::state_t &, const expr_t &);

    friend expr_t rvalue(expr_t);
    friend bool is_assignable(const expr_t &);
    friend expr_t make_assign(expr_t place, expr_t value);
    friend expr_t make_unary(unary_op_t op, expr_t arg);
    friend expr_t make_binary(binary_op_t op, expr_t lhs, expr_t rhs);
    friend stmt_t make_init(int slot, expr_t value);
};

// An erased operand generates itself: the handle's own rows, the value
// of the node it holds, in the register its row took, or its effect.  A
// row calls a child by these names whether the child's type is erased
// or concrete.  On x86-64 the value's row is handed wanted, the
// registers its consumer names, a hint (docs/typed-tree.md 3.4), none
// where the consumer names none.
value_t value(amd64::state_t &, const expr_t &, unsigned wanted = 0);
value_t value(arm64::state_t &, const expr_t &);
void effect(amd64::state_t &, const expr_t &);
void effect(arm64::state_t &, const expr_t &);

// The registers the instructions within an erased operand pin on
// x86-64, its summary handed up (principle 3), forwarded to the node it
// holds.
unsigned pinned_within(const amd64::state_t &, const expr_t &);

} // namespace rcc
