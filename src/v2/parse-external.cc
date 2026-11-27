// -*- mode: c++; -*-

// 6.9: the external definitions, of which rcc-v2 takes the function
// definition, and parse(), declared in parse.hh, last.

#include "parse.hh"

#include "build.hh"
#include "parse-common.hh"

#include <string>
#include <utility>
#include <vector>

namespace rcc {

namespace {

// The body, a compound statement, 6.9.2p1, which is a statement: the
// block of its items.  Its '{' is the token that made the declaration a
// definition.  The body and the parameter list are one block, 6.8.3p2,
// and with no parameter the body's scope is the function's.
stmt_t
function_body(parser_t &p)
{
    advance(p);

    p.scopes.emplace_back();
    std::vector< stmt_t > items = block_item_list(p);
    p.scopes.pop_back();
    expect(p, tok_t::rbrace, "expected '}' at end of function body");

    return make_block(std::move(items));
}

// A definition of the name, whose body is parsed with the function's
// result type, its locals numbered from 0.  A name is defined once,
// 6.9.1p5, and is in scope in its own body, 6.2.1p7.  The function has
// external linkage, 6.2.2p5, so a #pragma GCC visibility other than
// default would set its symbol's visibility, as clang has it: rcc
// refuses the name there, and before it asks whether the name is
// defined.  A system header's definition is compiled as any other,
// since rcc's rule for one is a static inline function's.
function_definition_t
function_definition(parser_t &p, const type_t &result, const token_t &name)
{
    if (name.nondefault_visibility)
        not_yet(name, "pragma 'GCC visibility'");

    if (!p.defined.insert(name.text).second)
        fail(name, "redefinition of " + quoted(name.text));

    p.result = result;
    p.locals = 0;
    p.loop_count = 0;

    return { std::string(name.text), function_body(p) };
}

// A function definition, 6.9.1p1; the other external declarations are
// not yet, an object among them.  rcc reads __asm__, __extension__ and
// a static assertion before the declaration specifiers, and refuses the
// first in these words.  A function's declarator that no body follows
// declares the function.
function_definition_t
external_declaration(parser_t &p)
{
    const token_t &t = peek(p);
    if (t.kind == tok_t::kw_asm)
        not_yet(t, quoted(t.text) + " at file scope");

    if (t.kind == tok_t::kw_extension)
        not_yet(t, quoted(t.text) + " before a declaration");

    if (t.kind == tok_t::kw_static_assert)
        not_yet(t, quoted(t.text) + " declaration");

    const type_t result = declaration_specifiers(p);
    const declarator_t d = declarator(p);
    if (!d.function)
        not_yet(d.name, "object " + quoted(d.name.text));

    if (peek(p).kind != tok_t::lbrace)
        not_yet(d.name, "declaration of function " + quoted(d.name.text));

    return function_definition(p, result, d.name);
}

// The external declarations up to the end of the text, 6.9.1p1.  An
// empty text, which the grammar does not have, is a unit of no
// function, as rcc takes it.
translation_unit_t
translation_unit(parser_t &p)
{
    translation_unit_t unit;
    while (peek(p).kind != tok_t::eof)
        unit.functions.push_back(external_declaration(p));

    return unit;
}

} // namespace

translation_unit_t
parse(const std::vector< token_t > &tokens)
{
    parser_t p { tokens };

    return translation_unit(p);
}

} // namespace rcc
