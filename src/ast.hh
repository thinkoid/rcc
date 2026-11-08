// -*- mode: c++; -*-

// The tree between the parser and c1, the code generator.  Three
// sums: expressions, statements, and the unit that owns the symbols.
// The parser builds it typed and c1 reads it; nothing writes it after
// parsing.  Recursion edges are unique_ptr for a fixed arity and
// vector for a list.

#pragma once

#include "error.hh"
#include "type.hh"

#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace rcc {

// A declared name.  The unit owns every symbol in a deque, so a
// pointer to one is stable for the life of the unit.  Where an object
// lives, its slot or its register, is c1's business and is not kept
// here.  A local or parameter the parser gives a register, in_register
// below, has no slot: c1 keeps it in a callee-saved register.
// declared_register is the keyword as written: & on such an object is
// refused whether or not it got a register (6.5.4.2p1), and an array
// so declared is not yet (6.3.2.1p3).
// in_register is the parser's assignment: a register declaration met
// with five register variables live is an ordinary local, with a
// warning.
enum class storage_t { local, parameter, global, function };

struct symbol_t
{
    std::string name;
    type_t type;
    storage_t storage;
    bool declared_register;
    bool in_register;
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

// An integer or character constant; its type is the expression's.
struct literal_t
{
    long long value;
};

struct name_t
{
    const symbol_t *sym;
};

// A string literal: adjacent tokens concatenated and escapes applied,
// without the terminating zero, which the type counts; the type is
// char [N + 1].  An object of static storage, one per literal written,
// and writable (G17).
struct string_t
{
    std::string bytes;
};

// addr yields a pointer to its lvalue operand; deref is the lvalue
// its pointer operand names.  A subscript is a deref of an add,
// 6.5.3.2p2.
struct unary_t
{
    unary_op_t op;
    expr_ptr_t arg;
};

// A pointer and an integer add or subtract as a binary add or sub of
// the pointer's type, the integer operand promoted and then widened
// to 64 bits in its own signedness by the parser, so an unsigned
// index never reads as negative (G6).  Two pointers subtract to long.
// A comparison of two pointers compares addresses (G7).
struct binary_t
{
    binary_op_t op;
    expr_ptr_t lhs, rhs;
};

// Plain assignment has no op, and common is the lhs type.  A compound
// assignment carries the operator and the type the operation is
// computed in, the usual arithmetic conversions of the two operands
// (6.5.17.3p4), or the promoted lhs type for a shift.  The parser
// converts the rhs to it, but a shift count, which it promotes on its
// own (6.5.8p3); c1 converts the loaded lhs into it and the result
// back to the lhs type.  On a pointer, += and -= carry the pointer type
// as common and the rhs widened to 64 bits as for binary +, and c1
// steps the pointer by it.
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
// is the expression's.  c1 converts nothing on its own, except the
// two conversions a compound assignment names.  Pointer to pointer is
// no instruction; a pointer converts to an integer and back as an
// unsigned long, its integer view in c1.  Array decay is a cast too,
// to the pointer to the first element; a function decays nowhere yet.
// An explicit cast is a node even to the operand's own type, since a
// cast is never an lvalue (6.5.5p6); c1 makes nothing of it.
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
                  string_t,
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

// The name of an object, a string literal or a dereference; the three
// c1 takes the address of: a local's slot, a string's label, or what
// the dereferenced pointer designates; a global's is a later slice.  A
// function's name is not an object (6.3.2.1p1).
inline bool
is_lvalue(const expr_t &e)
{
    if (is< name_t >(e))
        return as< name_t >(e).sym->storage != storage_t::function;

    if (is< string_t >(e))
        return true;

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
// the function; a declaration alone has none, and c1 writes nothing
// for it: a call names the symbol, and the linker resolves it.
struct function_decl_t
{
    const symbol_t *sym;
    std::vector< const symbol_t * > params;
    std::optional< block_t > body;
};

// The warnings in the order the parser met them; the unit compiles
// whatever they say.
struct unit_t
{
    std::deque< symbol_t > symbols;
    std::vector< function_decl_t > functions;
    std::vector< warning_t > warnings;
};

} // namespace rcc
