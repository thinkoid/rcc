// -*- mode: c++; -*-

// The handle of an expression, docs/typed-tree.md 3.1: an erased node
// of the zoo, held by value and shared, never written after it is
// built.  The parser holds expressions as handles and makes them
// through the builders, build.hh; the interface and the model are
// model.hh's, which build.cc alone includes, so a unit that includes
// this header cannot make a node.  An expression has three entry
// points (3.4): its value, held in a register its row takes; its
// effect, evaluated for what it does, its value dropped; and a jump
// on it, branch, to a label where its value is nonzero or where it is
// 0, by the sense.  x86-64 has a fourth, value_branch, the jump on
// the value compared with 0, and it hands up two summaries, the
// registers the instructions within it pin, and those that would move
// a value held across it, nothing below && and ||.  A builder that
// must know what the handle holds goes through the model (3.3), and
// is a friend.

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
    friend value_t value(arm64::state_t &, const expr_t &, unsigned wanted);
    friend void effect(amd64::state_t &, const expr_t &);
    friend void effect(arm64::state_t &, const expr_t &);
    friend void branch(amd64::state_t &, const expr_t &, int, bool);
    friend void branch(arm64::state_t &, const expr_t &, int, bool);
    friend void value_branch(amd64::state_t &, const expr_t &, int, bool);
    friend unsigned pinned_within(const amd64::state_t &, const expr_t &);
    friend unsigned moved_within(const amd64::state_t &, const expr_t &);

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
// or concrete.  The value's row is handed wanted, the registers its
// consumer names, a hint (docs/typed-tree.md 3.4), none where the
// consumer names none.
value_t value(amd64::state_t &, const expr_t &, unsigned wanted = 0);
value_t value(arm64::state_t &, const expr_t &, unsigned wanted = 0);
void effect(amd64::state_t &, const expr_t &);
void effect(arm64::state_t &, const expr_t &);

// A jump on an erased operand, through the handle: to label where its
// value is nonzero, sense, or where it is 0, !sense, and a fall through
// otherwise, with no value held after it.  x86-64's value_branch is the
// jump on the value compared with 0, the node's own test of zero, where
// branch is the node's best jump.
void branch(amd64::state_t &, const expr_t &, int label, bool sense);
void branch(arm64::state_t &, const expr_t &, int label, bool sense);
void value_branch(amd64::state_t &, const expr_t &, int label, bool sense);

// The registers the instructions within an erased operand pin on
// x86-64, and those a pinned instruction within it would move a held
// value out of, its summaries handed up (principle 3), forwarded to the
// node it holds.
unsigned pinned_within(const amd64::state_t &, const expr_t &);
unsigned moved_within(const amd64::state_t &, const expr_t &);

} // namespace rcc
