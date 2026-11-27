// -*- mode: c++; -*-

// The one unit that sees the whole zoo, docs/typed-tree.md 3.1: the
// node types, every row of both targets and the models, so that every
// model is instantiated here against the same rows and the same typed
// builders, build-typed.hh's.  It defines the handles' own rows, which
// call through the interface, and the builders, declared in build.hh,
// which make the models.

#include "build.hh"

#include "amd64.hh"
#include "arm64.hh"
#include "build-typed.hh"
#include "model.hh"
#include "register-stack.hh"
#include "rows-expressions.hh"
#include "rows-statements.hh"
#include "zoo.hh"

#include <utility>
#include <vector>

namespace rcc {

value_t
value(amd64::state_t &s, const expr_t &e)
{
    return e.self->value(s);
}

value_t
value(arm64::state_t &s, const expr_t &e)
{
    return e.self->value(s);
}

void
effect(amd64::state_t &s, const expr_t &e)
{
    e.self->effect(s);
}

void
effect(arm64::state_t &s, const expr_t &e)
{
    e.self->effect(s);
}

// A statement holds no value past its end, c1-statements.hh's check.
void
generate(amd64::state_t &s, const stmt_t &statement)
{
    statement.self->generate(s);
    check_none_held(s);
}

void
generate(arm64::state_t &s, const stmt_t &statement)
{
    statement.self->generate(s);
    check_none_held(s);
}

// the builders

// The question of the constant's value is asked here, once: 0 is a
// zero_t (principle 2).
expr_t
make_literal(int value)
{
    if (value == 0)
        return expr_t(zero_t { });

    return expr_t(literal_t { value });
}

expr_t
make_local(int slot)
{
    return expr_t(slot_t { slot });
}

expr_t
rvalue(expr_t e)
{
    return e.self->rvalue(e);
}

bool
is_assignable(const expr_t &e)
{
    return e.self->is_assignable();
}

// Two hops, 3.3: through the value's model, which knows the value's
// class, then through the place's, which knows the place's type.
expr_t
make_assign(expr_t place, expr_t value)
{
    return value.self->assigned_to(place, value);
}

// One hop through the operand's model, which knows whether the operand
// is a constant (3.3).
expr_t
make_unary(unary_op_t op, expr_t arg)
{
    return arg.self->unary(op, arg);
}

// Two hops, 3.3: through the right operand's model, which knows its
// class, then through the left's, which knows the left's class.
expr_t
make_binary(binary_op_t op, expr_t lhs, expr_t rhs)
{
    return rhs.self->right_of(op, lhs, rhs);
}

stmt_t
make_expr_stmt(expr_t e)
{
    return stmt_t(expr_stmt_t { std::move(e) });
}

stmt_t
make_null()
{
    return stmt_t(null_t { });
}

stmt_t
make_decl(int slot)
{
    return stmt_t(decl_t { slot_t { slot } });
}

// One hop through the value's model, which knows the value's class
// (3.3).
stmt_t
make_init(int slot, expr_t value)
{
    return value.self->init(slot_t { slot }, value);
}

stmt_t
make_return(expr_t value)
{
    return stmt_t(return_t { std::move(value) });
}

stmt_t
make_block(std::vector< stmt_t > items)
{
    return stmt_t(block_t { std::move(items) });
}

} // namespace rcc
