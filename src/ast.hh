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
// pointer to one is stable for the life of the unit.  An object's slot
// and the machine register it is kept in are c1's business and are
// not kept here; which of the target's register variables it is, is
// kept.  A local or parameter the parser gives a register, in_register()
// below, has no slot: c1 keeps it in a callee-saved register.
// declared_register is the keyword as written: & on such an object is
// refused whether or not it got a register (6.5.4.2p1), and an array
// so declared is not yet (6.3.2.1p3).
// register_index is the parser's assignment, which of the target's
// register variables the object takes, 0 the first: each takes the next
// in declaration order and gives it back at its block's end, backend.md
// chapter 4, so the index is one above the highest of those live where
// the object is declared and of those the closed inner blocks of its
// block took, since the object is live from the entry into its block,
// 6.2.4p6, and so through those blocks.  It is -1 for an object in no
// register: a register declaration whose next index is past the
// target's register variables is an ordinary local, with a warning.
// A global is an object of static storage duration, 6.2.4p3: one
// declared at file scope, or one declared static in a block.  One the
// unit defines has an entry in the unit's objects, and one it only
// declares has none; a block's has one always.  A type_name is a
// typedef: its type is the type the name stands for, it has no object,
// and no expression names it.  An enumerator is an enumeration
// constant: its type is the constant's, 6.7.3.3p12, p15, and value is
// its value, as a literal of the type holds it.  The parser makes the
// literal where an expression names one, so no name in the tree is an
// enumerator's.
enum class storage_t {
    local,
    parameter,
    global,
    function,
    type_name,
    enumerator
};

// The linkage of a name, 6.2.2p1.  A function or an object at file
// scope has external linkage, or internal when declared static, p3;
// every other name has none, p6, a block's static object among them.
// c1 makes a symbol the unit defines global, .globl, for external
// linkage alone: the others are local to the file's object, which no
// other file's reference binds to.
enum class linkage_t { none, internal, external };

// A function or a global may carry two more facts from GNU C, said
// in its declarations.  asm_name is the name __asm__ gives the symbol
// in the assembly, empty when the symbol is named as it is in C; a
// block's static object takes one from the parser, the label of
// backend.md chapter 10, since two blocks may declare one name.
// marking is the words a use of the symbol is refused with, when an
// attribute changes or may change what the program depends on, as weak
// or alias change the symbol a name links to, or when the symbol is a
// static inline function, whose definition rcc does not compile; empty
// when none of these holds.  by_reference marks a parameter whose
// argument the convention passes as the address of a copy the caller
// makes, the aarch64 va_list: the symbol is that pointer, and the
// parser reads the parameter's name as the object it points to.
struct symbol_t
{
    std::string name;
    type_t type;
    storage_t storage;
    bool declared_register;
    int register_index;
    std::string asm_name = { };
    std::string marking = { };
    linkage_t linkage = linkage_t::none;
    long long value = 0;
    bool by_reference = false;
};

inline bool
in_register(const symbol_t &sym)
{
    return sym.register_index >= 0;
}

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
// char [N + 1], or for a wide literal an array of N + 1 of its element
// type, wchar_t, char16_t or char32_t's integer type.  bytes are the
// elements in memory order, each of width bytes, little-endian as both
// targets are: a char literal's width is 1.  An object of static
// storage, one per literal written, and writable (G17).
struct string_t
{
    std::string bytes;
    int width = 1;
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

// A call names its function, or goes through a pointer to one, and the
// callee's type tells the two apart.  The direct call's callee is the
// name of a function, of function type, the one place a function
// designator stands as an operand; any other callee is an expression of
// pointer-to-function type, the decay of a function designator among
// them, so (*f)(x) and (&f)(x) are calls through a pointer.
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
// to the pointer to the first element, and so is a function
// designator's, to the pointer to the function: over a function's name
// it is the function's address, over *E it is E's value.
// An explicit cast is a node even to the operand's own type, since a
// cast is never an lvalue (6.5.5p6); c1 makes nothing of it.
struct cast_t
{
    expr_ptr_t arg;
};

// GNU C's a ?: b has no yes: the condition, converted to the type of
// the whole, is the first arm's value too, and is evaluated once.
struct conditional_t
{
    expr_ptr_t cond, yes, no;
};

struct stmt_t;

// GNU C's statement expression, ({ ... }): a block whose statements run
// where the expression stands, G13, in a scope of their own.  body is
// the block's items but the last expression statement's expression,
// which is value, the expression's value, decayed as any operand
// discarded is; value is null where the block ends in anything else,
// and the expression is then void.  Its type is the value's, and it is
// no lvalue, the value's lvalue conversion being c1's load.
struct statement_expression_t
{
    std::vector< stmt_t > body;
    expr_ptr_t value;
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
                  conditional_t,
                  statement_expression_t >
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
// c1 takes the address of: a local's slot, a global's label or its
// address from the GOT, a string's label, or what the dereferenced
// pointer designates.  A function designator, a function's name or *p
// through a pointer to a function, designates no object (6.3.2.1p1,
// p4).
inline bool
is_lvalue(const expr_t &e)
{
    if (is< function_t >(e.type))
        return false;

    if (is< name_t >(e) || is< string_t >(e))
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

// A local's declaration: init is a scalar's initializer, converted to
// its type.  zeroed is a record's or an array's list, 6.7.11p17: the
// declaration zeroes the whole object, padding included, and the
// statements after it store the listed values, in the list's order.
struct decl_stmt_t
{
    const symbol_t *sym;
    std::optional< expr_t > init;
    bool zeroed = false;
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

// A case of a switch: its constant converted to the promoted type of
// the controlling expression, 6.8.5.3p5, held as a literal of that type
// holds its value, and the number of its label.  The parser numbers the
// case and default labels of a function from 0 in the order written.
struct case_t
{
    long long value;
    int label;
};

// A switch, 6.8.5.3: the controlling expression, promoted, p5; the
// body; the cases that belong to it, p4, in the order written, no two
// of one value, p3; and the number of its default label, absent without
// one.  The body marks where each label stands by a case_label_t.
struct switch_t
{
    expr_t cond;
    stmt_ptr_t body;
    std::vector< case_t > cases;
    std::optional< int > default_;
};

struct while_t
{
    expr_t cond;
    stmt_ptr_t body;
};

// The body before the condition, as written, 6.8.6.3p1.
struct do_t
{
    stmt_ptr_t body;
    expr_t cond;
};

// init is null, an expression statement or a declaration; cond and
// step are absent when the source leaves them out.
struct for_t
{
    stmt_ptr_t init;
    std::optional< expr_t > cond, step;
    stmt_ptr_t body;
};

// A break or a continue names no statement: c1 jumps out of the
// innermost one that takes it, or on with it, as it generates the
// statements around it.
struct break_t
{
};

struct continue_t
{
};

// A label and a goto name a label of the function's own name space,
// 6.2.3p1, by its identifier.  A label is a statement of its own, the
// null statement it is translated as followed by, 6.8.3p2, so it may
// stand before a declaration and before a block's closing brace; a
// labeled statement elsewhere is a block of the label and the
// statement, a block of no scope, as the init of a for is.
struct label_t
{
    std::string name;
};

// A case or a default label, by the number its switch_t gives it: a
// label as a named one is, 6.8.2p1, a statement of its own, which may
// stand before a declaration and before a block's closing brace.
struct case_label_t
{
    int label;
};

struct goto_t
{
    std::string name;
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
                  switch_t,
                  while_t,
                  do_t,
                  for_t,
                  break_t,
                  continue_t,
                  label_t,
                  case_label_t,
                  goto_t,
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
// registers is the count of register variables the definition uses,
// its parameters' among them, as the parser assigned them: the largest
// register_index + 1, and 0 for a declaration alone.  va_area is the
// save area of a variadic definition that calls va_start, 7.16.1.4: a
// local no declaration names, of bytes, into which the prologue stores
// every argument register as it arrives, and the address of the first
// argument the caller left on the stack past the named ones; null for
// any other function.  docs/shape.md chapter 4 gives its layout.
// uncompiled says that the unit defines the function by a definition
// whose head or body the parser stopped in at a construct rcc does not
// compile yet, or by a system header's static inline definition, whose
// body it skipped: there is no body, and c1 writes nothing for it.
// droppable says that the function is a static inline one of the
// program's own text whose body is whole and that no expression and no
// alias names: c1 writes nothing for it when it refuses the body, as
// the parser writes nothing for one it stops in.  statics_begin and
// statics_end bound the static objects its body declares in the unit's
// objects, which go with it, since nothing outside the body names them.
struct function_decl_t
{
    const symbol_t *sym;
    std::vector< const symbol_t * > params;
    std::optional< block_t > body;
    int registers;
    const symbol_t *va_area = nullptr;
    bool uncompiled = false;
    bool droppable = false;
    std::size_t statics_begin = 0;
    std::size_t statics_end = 0;
};

// An address constant, 6.6p9, p11, as the parser folds it: the address
// of a global, of a function or of a string literal, and a byte offset
// from it.  The literal is one more object of static storage, 6.4.5p6,
// apart from every other, as a literal a function names is.
struct address_t
{
    std::variant< const symbol_t *, string_t > base;
    long long offset;
};

// A value an initializer gives a scalar or an array of char at file
// scope, folded by the parser into the data c1 writes: a constant of the
// scalar's type, an integer or a pointer, the bits of a value past
// LLONG_MAX of an unsigned type; an address constant; or the bytes of a
// string literal that initialize an array of char, 6.7.11p15, the
// array's own bytes and no literal of its own.
using initializer_t = std::variant< literal_t, address_t, string_t >;

// One object or subobject an initializer gives a value, 6.7.11p18: the
// object itself when it is a scalar or an array of char a string
// initializes, else an element of an array, at any depth.  offset is
// its byte offset in the object, type its type, a scalar's or the array
// of char's, which gives its size, and value what it holds.
struct subobject_t
{
    long offset;
    type_t type;
    initializer_t value;
};

// One per object the unit defines, 6.9.3: a declaration at file scope
// with an initializer, or a tentative definition, one without an
// initializer and without extern; and a block's static object, whose
// declaration defines it, 6.7.1p6.  The entries are in the order of the
// definitions, each where the first declaration that defines its
// object stands; a later one adds its initializer.  An object the unit
// only declares has none, and c1 writes nothing for it.  init is the
// subobjects the initializer gives a value, each converted to its type,
// in the order of their offsets, which never overlap; every byte none of
// them holds is zero, 6.7.11p11, p20, p22, and {} holds none.  It is
// absent for an object whose definitions are all tentative, which is
// zero, 6.9.3p2.  The symbol's type is complete by the end of the unit:
// an array of unknown size takes its size from a later declaration, from
// its initializer, or else is one element, 6.9.3p2.
struct object_decl_t
{
    const symbol_t *sym;
    std::optional< std::vector< subobject_t > > init;
};

// One per alias the unit declares, GNU C's alias attribute: a function
// or an object of external linkage that the unit does not define, and
// the function or the object of the same kind the unit defines and the
// alias names.  c1 writes the alias as a symbol of the target's
// address.
struct alias_t
{
    const symbol_t *sym;
    const symbol_t *target;
};

// The unit owns every record in a deque, as it owns the symbols, so a
// record type's pointer to its record is stable for the life of the
// unit.  The aliases are in the order of their declarations.
struct unit_t
{
    std::deque< symbol_t > symbols;
    std::deque< record_t > records;
    std::vector< function_decl_t > functions;
    std::vector< object_decl_t > objects;
    std::vector< alias_t > aliases;
};

} // namespace rcc
