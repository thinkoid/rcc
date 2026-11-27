// -*- mode: c++; -*-

// The builders, docs/typed-tree.md 3.3: all the parser sees of the
// zoo.  Each takes a construct's parts and returns its node under its
// handle; build.cc defines them, where the node types and the models
// are seen, so a builder is where a part's type becomes the node's.  A
// local is passed as its number among the function's, which the parser
// counts from 0 in the order it declares them, and so is a loop.

#pragma once

#include "expr.hh"
#include "stmt.hh"

#include <string_view>
#include <vector>

namespace rcc {

expr_t make_literal(int value);
expr_t make_local(int slot);

// The lvalue conversion, 6.3.2.1p2: a place becomes the value it holds;
// any other expression is a value already, and is returned as it is.
expr_t rvalue(expr_t e);

// Whether an expression is a place an assignment stores into, which
// make_assign() takes, 6.5.17.1p2.
bool is_assignable(const expr_t &e);
expr_t make_assign(expr_t place, expr_t value);

// -e ~e !e of an operand converted, 6.5.4.3.
expr_t make_unary(unary_op_t op, expr_t arg);

// lhs op rhs of operands converted, 6.5.6 to 6.5.13.
expr_t make_binary(binary_op_t op, expr_t lhs, expr_t rhs);

// A call of the function named, 6.5.3.3, whose result type is void
// where returns_void says so and int otherwise, of its arguments
// converted, each stored in a hidden local of the call, numbered from
// first.
expr_t make_call(std::string_view name,
                 bool returns_void,
                 int first,
                 std::vector< expr_t > args);

stmt_t make_expr_stmt(expr_t e);
stmt_t make_null();
stmt_t make_decl(int slot);
stmt_t make_init(int slot, expr_t value);
stmt_t make_return(expr_t value);
stmt_t make_return_void();
stmt_t make_block(std::vector< stmt_t > items);

// if (cond) then, and if (cond) then else otherwise, 6.8.5.2, of a
// condition converted.
stmt_t make_if(expr_t cond, stmt_t then);
stmt_t make_if_else(expr_t cond, stmt_t then, stmt_t otherwise);

// What the parser knows of a loop once it has read its body (4.4,
// 4.6): its number, and whether a break and a continue of it stand in
// the body, which picks the labels its row places.
struct loop_t
{
    int id;
    bool broken = false;
    bool continued = false;
};

// while (cond) body, 6.8.6.2, do body while (cond);, 6.8.6.3, and for
// (init; cond; step) body, 6.8.6.4, of a condition converted; a for
// that omits its condition is the overload without one.  The init and
// the step are statements, an expression statement or the null
// statement where the clause is omitted.
stmt_t make_while(expr_t cond, stmt_t body, const loop_t &loop);
stmt_t make_do(stmt_t body, expr_t cond, const loop_t &loop);
stmt_t make_for(
    stmt_t init, expr_t cond, stmt_t step, stmt_t body, const loop_t &loop);
stmt_t make_for(stmt_t init, stmt_t step, stmt_t body, const loop_t &loop);

// break; and continue; of the loop numbered loop, 6.8.7.4, 6.8.7.3.
stmt_t make_break(int loop);
stmt_t make_continue(int loop);

} // namespace rcc
