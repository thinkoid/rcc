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

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
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

void
initialize(amd64::state_t &s, const amd64::address_t &at, const expr_t &e)
{
    e.self->initialize(s, at);
}

void
initialize(arm64::state_t &s, const arm64::address_t &at, const expr_t &e)
{
    e.self->initialize(s, at);
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

int
valid_of(const expr_t &e, sign_t sign)
{
    return e.self->valid(sign);
}

view_t
view_of(const expr_t &e)
{
    return e.self->view();
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

// The crossing from the view, data in the parser, to the node's
// parameter (principle 6), and the question of the constant's value,
// classed_constant()'s.
expr_t
make_literal(long long value, view_t view)
{
    return cross(view, [&]< view_t V >() {
        return classed_constant< V >(value,
                                     [](const auto &c) { return expr_t(c); });
    });
}

expr_t
make_local(int slot, view_t view, sign_t sign)
{
    return cross(view, [&]< view_t V >() {
        return cross(sign, [&]< sign_t S >() {
            return expr_t(place_t< slot_t, V, S > { slot_t { slot } });
        });
    });
}

expr_t
make_global(std::string_view name, view_t view, sign_t sign)
{
    return cross(view, [&]< view_t V >() {
        return cross(sign, [&]< sign_t S >() {
            return expr_t(
                place_t< global_t, V, S > { global_t { std::string(name) } });
        });
    });
}

expr_t
rvalue(expr_t e)
{
    return e.self->rvalue(e);
}

std::optional< long long >
constant_of(const expr_t &e)
{
    return e.self->constant_of();
}

bool
is_assignable(const expr_t &e)
{
    return e.self->is_assignable();
}

// One hop through the place's model, which knows the place's type
// (3.3).
expr_t
make_assign(expr_t place, expr_t value)
{
    return place.self->assign(value);
}

// One hop through the operand's model, which knows the operand's type
// and so whether it is a load or a constant (3.3).
expr_t
make_conversion(
    view_t to, sign_t to_sign, view_t from, sign_t from_sign, expr_t e)
{
    return e.self->conversion(to, to_sign, from, from_sign, e);
}

// One hop through the operand's model, which knows whether the operand
// is a constant (3.3).
expr_t
make_unary(unary_op_t op, view_t view, expr_t arg)
{
    return arg.self->unary(op, view, arg);
}

// Two hops, 3.3: through the right operand's model, which knows its
// class, then through the left's, which knows the left's class.
expr_t
make_binary(binary_op_t op, view_t view, expr_t lhs, expr_t rhs)
{
    return rhs.self->right_of(op, view, lhs, rhs);
}

namespace {

// The arguments of a call, each in a hidden local of the call, numbered
// from first.
std::vector< argument_t >
arguments_from(int first, std::vector< converted_argument_t > args)
{
    std::vector< argument_t > arguments;
    for (converted_argument_t &a : args)
        arguments.push_back({ slot_t { first++ }, a.sign, std::move(a.value) });

    return arguments;
}

} // namespace

// The crossing from the result's view, data in the parser, to the
// node's parameter (principle 6), made once here.
expr_t
make_call(std::string_view name,
          view_t result,
          int first,
          std::vector< converted_argument_t > args)
{
    return cross(result, [&]< view_t V >() {
        return expr_t(call_t< V > { std::string(name),
                                    arguments_from(first, std::move(args)) });
    });
}

expr_t
make_call_void(std::string_view name,
               int first,
               std::vector< converted_argument_t > args)
{
    return expr_t(call_void_t { std::string(name),
                                arguments_from(first, std::move(args)) });
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
make_decl(int slot, view_t view)
{
    return cross(view, [&]< view_t V >() {
        return stmt_t(decl_t< V > { slot_t { slot } });
    });
}

stmt_t
make_init(int slot, view_t view, expr_t value)
{
    return cross(view, [&]< view_t V >() {
        return stmt_t(init_t< V > { slot_t { slot }, std::move(value) });
    });
}

stmt_t
make_return(expr_t value, view_t view)
{
    return cross(view, [&]< view_t V >() {
        return stmt_t(return_t< V > { std::move(value) });
    });
}

stmt_t
make_return_void()
{
    return stmt_t(return_void_t { });
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
loop_node(const jump_target_t &loop, P... parts)
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
make_while(expr_t cond, stmt_t body, const jump_target_t &loop)
{
    return stmt_t(while_t { std::move(cond), std::move(body), loop.id });
}

stmt_t
make_do(stmt_t body, expr_t cond, const jump_target_t &loop)
{
    return loop_node< do_t >(loop, std::move(body), std::move(cond));
}

stmt_t
make_for(stmt_t init,
         expr_t cond,
         stmt_t step,
         stmt_t body,
         const jump_target_t &loop)
{
    return loop_node< tested_for_t >(loop, std::move(init), std::move(cond),
                                     std::move(step), std::move(body));
}

stmt_t
make_for(stmt_t init, stmt_t step, stmt_t body, const jump_target_t &loop)
{
    return loop_node< untested_for_t >(loop, std::move(init), omitted_t { },
                                       std::move(step), std::move(body));
}

namespace {

// The form of a switch's dispatch on its sorted cases, rcc's
// dispatch_of(), backend.md 7.3, by the rule of the Seventh Edition
// compiler: none without a case; direct where the range is above 0 and
// at most three times the count of the cases; simple below ten cases;
// hash otherwise.  A question about constants' values, whose answer is
// the node's parameter (principle 2).
dispatch_t
dispatch_of(const std::vector< case_t > &sorted)
{
    if (sorted.empty())
        return dispatch_t::none;

    const unsigned long long count = sorted.size();
    const unsigned long long range = case_range(sorted);
    if (range > 0 && range <= 3 * count)
        return dispatch_t::direct;

    if (count < 10)
        return dispatch_t::simple;

    return dispatch_t::hash;
}

// The crossing of the form, principle 6, as operators.hh's are.
template< typename F >
decltype(auto)
cross(dispatch_t form, F &&f)
{
    switch (form) {
    case dispatch_t::none: return f.template operator()< dispatch_t::none >();
    case dispatch_t::direct:
        return f.template operator()< dispatch_t::direct >();
    case dispatch_t::simple:
        return f.template operator()< dispatch_t::simple >();
    case dispatch_t::hash: return f.template operator()< dispatch_t::hash >();
    }
}

// The crossing from what the parser knows of a switch, data, to the
// parameters of its node, made once (principle 6): the form of its
// dispatch, whether it has a default label, and whether a break of it
// stands in its body.
template< dispatch_t form >
stmt_t
switch_node(expr_t cond,
            stmt_t body,
            std::vector< case_t > cases,
            std::optional< int > default_,
            const jump_target_t &target)
{
    const auto make = [&]< bool defaulted, bool broken >() {
        return stmt_t(switch_t< form, defaulted, broken > {
            std::move(cond), std::move(body), std::move(cases), default_,
            target.id });
    };

    if (default_ && target.broken)
        return make.template operator()< true, true >();

    if (default_)
        return make.template operator()< true, false >();

    if (target.broken)
        return make.template operator()< false, true >();

    return make.template operator()< false, false >();
}

} // namespace

// The cases sorted by value, signed, the order of int, rcc's
// sorted_cases(), and the form asked of them once.
stmt_t
make_switch(expr_t cond,
            stmt_t body,
            std::vector< case_t > cases,
            std::optional< int > default_,
            const jump_target_t &target)
{
    std::ranges::sort(cases, { }, &case_t::value);

    return cross(dispatch_of(cases), [&]< dispatch_t form >() {
        return switch_node< form >(std::move(cond), std::move(body),
                                   std::move(cases), default_, target);
    });
}

stmt_t
make_case_label(int label)
{
    return stmt_t(case_label_t { label });
}

stmt_t
make_break(int target)
{
    return stmt_t(break_t { target });
}

stmt_t
make_continue(int loop)
{
    return stmt_t(continue_t { loop });
}

stmt_t
make_label(int label)
{
    return stmt_t(label_t { label });
}

stmt_t
make_goto(int label)
{
    return stmt_t(goto_t { label });
}

} // namespace rcc
