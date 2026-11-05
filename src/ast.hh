// -*- mode: c++; -*-

// The tree between the parser and the emitter.  Three sums:
// expressions, statements, and the unit that owns the symbols.  The
// parser builds it typed and the emitter reads it; nothing writes it
// after parsing.  Recursion edges are unique_ptr for a fixed arity
// and vector for a list.

#pragma once

#include "type.hh"

#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace rcc {

// A declared name.  The unit owns every symbol in a deque, so a
// pointer to one is stable for the life of the unit.  The IR name is
// the emitter's business and is not kept here.
enum class storage_t { local, parameter, global, function };

struct symbol_t
{
    std::string name;
    type_t type;
    storage_t storage;
};

// expressions

struct expr_t;

using expr_ptr_t = std::unique_ptr< expr_t >;

enum class unary_op_t {
    neg,
    bit_not,
    log_not,
    addr,
    deref,
    pre_inc,
    pre_dec,
    post_inc,
    post_dec
};

enum class binary_op_t {
    add,
    sub,
    mul,
    div,
    rem,
    shl,
    shr,
    lt,
    gt,
    le,
    ge,
    eq,
    ne,
    bit_and,
    bit_or,
    bit_xor,
    log_and,
    log_or,
    comma
};

// An integer constant; its type is the expression's.
struct literal_t
{
    long long value;
};

struct name_t
{
    const symbol_t *sym;
};

struct unary_t
{
    unary_op_t op;
    expr_ptr_t arg;
};

struct binary_t
{
    binary_op_t op;
    expr_ptr_t lhs, rhs;
};

// Plain assignment has no op, and common is the lhs type.  A compound
// assignment carries the operator and the type the operation is
// computed in, the usual arithmetic conversions of the two operands
// (6.5.17.3p4), or the promoted lhs type for a shift; the parser
// converts the rhs to it, and the emitter converts the loaded lhs
// into it and the result back to the lhs type.
struct assign_t
{
    std::optional< binary_op_t > op;
    type_t common;
    expr_ptr_t lhs, rhs;
};

struct call_t
{
    expr_ptr_t callee;
    std::vector< expr_t > args;
};

// A conversion, explicit or inserted by the parser; the target type
// is the expression's.  The emitter converts nothing on its own,
// except the two conversions a compound assignment names.
struct cast_t
{
    expr_ptr_t arg;
};

struct conditional_t
{
    expr_ptr_t cond, yes, no;
};

struct expr_t
{
    type_t type;
    std::variant< literal_t,
                  name_t,
                  unary_t,
                  binary_t,
                  assign_t,
                  call_t,
                  cast_t,
                  conditional_t >
        v;
};

template< typename T >
bool
is(const expr_t &e)
{
    return std::holds_alternative< T >(e.v);
}

template< typename T >
const T &
as(const expr_t &e)
{
    return std::get< T >(e.v);
}

// The name of an object or a dereference; the two the emitter can
// take the address of.  A function's name is not an object (6.3.2.1p1).
inline bool
is_lvalue(const expr_t &e)
{
    if (is< name_t >(e))
        return as< name_t >(e).sym->storage != storage_t::function;

    return is< unary_t >(e) && as< unary_t >(e).op == unary_op_t::deref;
}

// statements

struct stmt_t;

using stmt_ptr_t = std::unique_ptr< stmt_t >;

// An empty statement is an expr_stmt_t without an expression.
struct expr_stmt_t
{
    std::optional< expr_t > e;
};

struct decl_stmt_t
{
    const symbol_t *sym;
    std::optional< expr_t > init;
};

struct block_t
{
    std::vector< stmt_t > body;
};

// no is null without an else
struct if_t
{
    expr_t cond;
    stmt_ptr_t yes, no;
};

struct while_t
{
    expr_t cond;
    stmt_ptr_t body;
};

// init is null, an expression statement or a declaration; cond and
// step are absent when the source leaves them out.
struct for_t
{
    stmt_ptr_t init;
    std::optional< expr_t > cond, step;
    stmt_ptr_t body;
};

struct return_t
{
    std::optional< expr_t > value;
};

struct stmt_t
{
    std::variant< expr_stmt_t,
                  decl_stmt_t,
                  block_t,
                  if_t,
                  while_t,
                  for_t,
                  return_t >
        v;
};

template< typename T >
bool
is(const stmt_t &s)
{
    return std::holds_alternative< T >(s.v);
}

template< typename T >
const T &
as(const stmt_t &s)
{
    return std::get< T >(s.v);
}

// the unit

// One per function symbol.  A body is present when the unit defines
// the function; a declaration alone has none, and the emitter writes
// a declare for it.
struct function_decl_t
{
    const symbol_t *sym;
    std::vector< const symbol_t * > params;
    std::optional< block_t > body;
};

struct unit_t
{
    std::deque< symbol_t > symbols;
    std::vector< function_decl_t > functions;
};

} // namespace rcc
