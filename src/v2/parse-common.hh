// -*- mode: c++; -*-

// What the parser's files share: the state of one parse, an operand,
// the diagnostics, the token primitives, the scopes, and the
// productions one file calls in another.  The parser is a recursive
// descent with one function for each production of the C grammar,
// named as rcc's parser names it (docs/typed-tree.md 4.1), and each
// file holds the productions of one clause.  A production's function
// is the accumulator of its parts, and ends in one builder call (4.4).
// Paragraph numbers are C23 (N3220).

#pragma once

#include "build.hh"
#include "expr.hh"
#include "lex.hh"
#include "stmt.hh"
#include "type.hh"
#include "unit.hh"

#include <cstddef>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rcc {

// What the parser holds of an expression: its C type, which the
// productions read; the node built for it, or none for a function
// designator, whose value, the function's address, 6.3.2.1p4, rcc-v2
// does not compile yet; and the identifier the expression is, through
// parentheses, 6.5.2p6, or null, for the diagnostics that name it and
// for the function a call names.
struct operand_t
{
    type_t type;
    std::optional< expr_t > node;
    const token_t *name = nullptr;
};

// A block's scope, or a parameter list's, 6.2.1p4: each name it
// declares, a view into the tokens, and what the parser holds of the
// name, its type and its leaf, which a use copies (docs/typed-tree.md
// 4.5).
using scope_t = std::unordered_map< std::string_view, operand_t >;

// What the parser holds of a function's name in the file's scope,
// 6.2.1p4: its type, a function_t, which a later declaration of the
// name must be compatible with, 6.7.1p5, and the index of its entry
// among the unit's functions, which holds its linkage and its body.
struct function_name_t
{
    type_t type;
    std::size_t index;
};

// The state of one parse: the tokens and the position in them; the
// warnings, a list the driver owns; the unit, filled as its functions
// are declared and defined; the names of the functions the file
// declares, views into the tokens; each use of a static function, by
// its index among the unit's functions, for the end of the unit to
// require its definition; the result type of the function being
// defined, for its return statements; the scopes open where the parse
// stands, innermost last, the function's blocks' and a parameter
// list's; the count of the locals the function has declared, which
// numbers the next one; and the loops whose bodies are being read,
// innermost last, with the count of the function's loops, which numbers
// the next one (docs/typed-tree.md 4.6).
struct parser_t
{
    const std::vector< token_t > &tokens;
    std::vector< warning_t > &warnings;
    std::size_t pos = 0;
    translation_unit_t unit = { };
    std::unordered_map< std::string_view, function_name_t > functions = { };
    std::vector< std::pair< std::size_t, const token_t * > >
        internal_uses = { };
    type_t result = { };
    std::vector< scope_t > scopes = { };
    int locals = 0;
    std::vector< loop_t > loops = { };
    int loop_count = 0;
};

//
// diagnostics
//

// An error of the program at the token, the whole of its message.
[[noreturn]] void fail(const token_t &at, std::string what);

// A construct of C that rcc-v2 does not compile yet, refused at the
// token by its name, as an error is.
[[noreturn]] void not_yet(const token_t &at, std::string_view construct);

// A warning at the token, which does not stop the translation: it joins
// the driver's list, which the driver prints after the parse, or before
// the error that ends it.
void warn(parser_t &p, const token_t &at, std::string what);

std::string quoted(std::string_view name);

//
// identifiers
//

// Whether a name is reserved for any use, 6.4.2.1p7: it begins with two
// underscores, or with one and an uppercase letter.
bool is_reserved(std::string_view name);

//
// token primitives
//

// The token ahead of the position; past the end, the eof that closes
// the vector.
const token_t &peek(const parser_t &p, std::size_t ahead = 0);

bool next_is(const parser_t &p, std::initializer_list< tok_t > kinds);

// Consumes the token ahead and returns it; the eof is never consumed.
const token_t &advance(parser_t &p);

// Consumes the token ahead if it is of the kind, and says whether it
// was.
bool accept(parser_t &p, tok_t kind);

// The message is the whole diagnostic, said at the token found instead.
const token_t &expect(parser_t &p, tok_t kind, std::string_view message);

//
// types
//

// int as the parser makes it: 32 bits, signed, of the kind int.
type_t int_type();

//
// values
//

// A void expression has no value, 6.3.2.2p1, and its use as one is an
// error at the token, rcc's require_value().
void require_value(const token_t &at, const operand_t &e);

// An operand whose value is read, rcc's rvalue(): a void one is none,
// require_value()'s, an error at the token; a function designator's
// value is its function's address, 6.3.2.1p4, which is not yet; any
// other is its node, the lvalue converted, 6.3.2.1p2.
expr_t rvalue(const token_t &at, operand_t e);

// An operand evaluated for its effect alone, 6.8.4p2, rcc's
// discarded(): it may be void, and a function designator, which would
// decay to its function's address, is not yet.
expr_t discarded(operand_t e);

// A value converted to the type t as if by assignment, 6.5.17.2p2,
// rcc's assignment_conversion(): a void value is none,
// require_value()'s; a function designator decays to a pointer,
// 6.3.2.1p4, which converts to no integer, 6.5.17.2p1, an error at the
// token; and a value of another type than t is not yet, its conversion
// to what words names.  Returns the value's node.
expr_t assignment_conversion(const token_t &at,
                             operand_t e,
                             const type_t &t,
                             std::string_view words);

//
// scopes
//

// What the innermost scope that declares the name holds of it, or
// null when none does.
const operand_t *lookup(const parser_t &p, std::string_view name);

// Declares the name in the innermost scope as a local of the type, and
// returns the local's number.
int declare(parser_t &p, const token_t &name, const type_t &type);

//
// the productions one file calls in another
//

// 6.4.4.2, parse-constants.cc
operand_t integer_constant(parser_t &);

// 6.5, parse-expressions.cc
operand_t assignment_expression(parser_t &);
operand_t expression(parser_t &);

// 6.7, parse-declarations.cc

// What the parser holds of the declaration specifiers: the storage
// class, static or none; the type specifier's token; and the type.
struct specifiers_t
{
    const token_t *storage_class;
    const token_t &specifier;
    type_t type;
};

// A parameter as written: its first token; its name's token, or null
// for a parameter its declaration does not name, 6.7.7.1p1; and its
// type.
struct parameter_t
{
    const token_t &start;
    const token_t *name;
    type_t type;
};

// What the parser holds of a declarator: its name; the type it
// declares, the specifiers' or a function returning it, 6.7.7.4p4; and
// for a function, its parameters as written and the scope of its
// parameter list, 6.2.1p4, which holds their names and which a
// definition's body opens again, 6.8.3p2.
struct declarator_t
{
    const token_t &name;
    type_t type;
    std::vector< parameter_t > params = { };
    scope_t scope = { };
};

bool is_declaration_start(const parser_t &, std::size_t ahead = 0);
specifiers_t declaration_specifiers(parser_t &);
declarator_t declarator(parser_t &, const type_t &base);
std::vector< stmt_t > declaration(parser_t &);

// 6.8, parse-statements.cc
std::vector< stmt_t > block_item_list(parser_t &);

} // namespace rcc
