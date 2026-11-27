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

#include <optional>
#include <utility>
#include <vector>

namespace rcc {

value_t
value(amd64::state_t &s, const expr_t &e, unsigned wanted)
{
    return e.self->value(s, wanted);
}

value_t
value(arm64::state_t &s, const expr_t &e, unsigned wanted)
{
    return e.self->value(s, wanted);
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

void
branch(amd64::state_t &s, const expr_t &e, int label, bool sense)
{
    e.self->branch(s, label, sense);
}

void
branch(arm64::state_t &s, const expr_t &e, int label, bool sense)
{
    e.self->branch(s, label, sense);
}

void
value_branch(amd64::state_t &s, const expr_t &e, int label, bool sense)
{
    e.self->value_branch(s, label, sense);
}

unsigned
pinned_within(const amd64::state_t &s, const expr_t &e)
{
    return e.self->pinned_within(s);
}

unsigned
moved_within(const amd64::state_t &s, const expr_t &e)
{
    return e.self->moved_within(s);
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

// One hop through the then part's model, which knows whether it is one
// jump (3.3), and a second through a block's one item.
stmt_t
make_if(expr_t cond, stmt_t then)
{
    return then.self->if_then(cond, std::nullopt, then);
}

stmt_t
make_if_else(expr_t cond, stmt_t then, stmt_t otherwise)
{
    return then.self->if_then(cond, otherwise, then);
}

stmt_t
if_on_item(const stmt_t &item,
           const expr_t &cond,
           const std::optional< stmt_t > &otherwise,
           const stmt_t &block)
{
    return item.self->if_item(cond, otherwise, block);
}

namespace {

template< bool broken, bool continued >
using tested_for_t = for_t< expr_t, broken, continued >;

template< bool broken, bool continued >
using untested_for_t = for_t< omitted_t, broken, continued >;

// The crossing from what the parser knows of a loop's body, data, to
// the parameters of the loop's node, L, made once (principle 6):
// whether a break and a continue of the loop stand in its body.
template< template< bool, bool > typename L, typename... P >
stmt_t
loop_node(const loop_t &loop, P... parts)
{
    if (loop.broken && loop.continued)
        return stmt_t(L< true, true > { std::move(parts)..., loop.id });

    if (loop.broken)
        return stmt_t(L< true, false > { std::move(parts)..., loop.id });

    if (loop.continued)
        return stmt_t(L< false, true > { std::move(parts)..., loop.id });

    return stmt_t(L< false, false > { std::move(parts)..., loop.id });
}

} // namespace

stmt_t
make_while(expr_t cond, stmt_t body, const loop_t &loop)
{
    return stmt_t(while_t { std::move(cond), std::move(body), loop.id });
}

stmt_t
make_do(stmt_t body, expr_t cond, const loop_t &loop)
{
    return loop_node< do_t >(loop, std::move(body), std::move(cond));
}

stmt_t
make_for(stmt_t init, expr_t cond, stmt_t step, stmt_t body, const loop_t &loop)
{
    return loop_node< tested_for_t >(loop, std::move(init), std::move(cond),
                                     std::move(step), std::move(body));
}

stmt_t
make_for(stmt_t init, stmt_t step, stmt_t body, const loop_t &loop)
{
    return loop_node< untested_for_t >(loop, std::move(init), omitted_t { },
                                       std::move(step), std::move(body));
}

stmt_t
make_break(int loop)
{
    return stmt_t(jump_t { loop, &loop_labels_t::break_ });
}

stmt_t
make_continue(int loop)
{
    return stmt_t(jump_t { loop, &loop_labels_t::continue_ });
}

} // namespace rcc
