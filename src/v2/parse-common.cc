// -*- mode: c++; -*-

// The diagnostics, the token primitives, the types and the scopes the
// parser's files share, declared in parse-common.hh, as rcc's parser
// has them.

#include "parse-common.hh"

#include "build.hh"

#include <algorithm>
#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>

namespace rcc {

//
// diagnostics
//

void
fail(const token_t &at, std::string what)
{
    throw error_t { at.where, std::move(what) };
}

void
not_yet(const token_t &at, std::string_view construct)
{
    fail(at, "not yet: " + std::string(construct));
}

void
warn(parser_t &p, const token_t &at, std::string what)
{
    p.warnings.push_back({ at.where, std::move(what) });
}

std::string
quoted(std::string_view name)
{
    return "'" + std::string(name) + "'";
}

//
// identifiers
//

bool
is_reserved(std::string_view name)
{
    if (name.starts_with("__"))
        return true;

    return name.size() > 1 && name[0] == '_' && name[1] >= 'A' &&
           name[1] <= 'Z';
}

//
// token primitives
//

const token_t &
peek(const parser_t &p, std::size_t ahead)
{
    return p.tokens[std::min(p.pos + ahead, p.tokens.size() - 1)];
}

bool
next_is(const parser_t &p, std::initializer_list< tok_t > kinds)
{
    for (const tok_t kind : kinds)
        if (peek(p).kind == kind)
            return true;

    return false;
}

const token_t &
advance(parser_t &p)
{
    const token_t &t = peek(p);
    if (t.kind != tok_t::eof)
        ++p.pos;

    return t;
}

bool
accept(parser_t &p, tok_t kind)
{
    if (peek(p).kind != kind)
        return false;

    advance(p);

    return true;
}

const token_t &
expect(parser_t &p, tok_t kind, std::string_view message)
{
    if (peek(p).kind != kind)
        fail(peek(p), std::string(message));

    return advance(p);
}

//
// types
//

type_t
int_type()
{
    return make_int(32, true, int_kind_t::int_);
}

//
// values
//

void
require_value(const token_t &at, const operand_t &e)
{
    if (is< void_t >(e.type))
        fail(at, "void expression used as a value");
}

namespace {

// The not yet of a function designator is said at its name, wherever
// its value is read.
void
refuse_designator(const operand_t &e)
{
    if (!e.node)
        not_yet(*e.name, "function designator " + quoted(e.name->text));
}

} // namespace

expr_t
rvalue(const token_t &at, operand_t e)
{
    require_value(at, e);
    refuse_designator(e);

    return rvalue(std::move(*e.node));
}

expr_t
discarded(operand_t e)
{
    refuse_designator(e);

    return rvalue(std::move(*e.node));
}

expr_t
assignment_conversion(const token_t &at,
                      operand_t e,
                      const type_t &t,
                      std::string_view words)
{
    require_value(at, e);

    if (is< function_t >(e.type))
        fail(at, "incompatible pointer to integer conversion");

    if (e.type != t)
        not_yet(at, "conversion to " + std::string(words));

    return rvalue(std::move(*e.node));
}

//
// scopes
//

// The scopes from the innermost out, 6.2.1p4: an inner declaration
// hides an outer one.
const operand_t *
lookup(const parser_t &p, std::string_view name)
{
    for (auto scope = p.scopes.rbegin(); scope != p.scopes.rend(); ++scope)
        if (const auto it = scope->find(name); it != scope->end())
            return &it->second;

    return nullptr;
}

// One declaration of a name of no linkage in a scope, 6.7.1p4, which
// rcc says at the second.  The local's leaf is its slot, numbered in
// the order the function declares its locals.
int
declare(parser_t &p, const token_t &name, const type_t &type)
{
    if (p.scopes.back().contains(name.text))
        fail(name, "redefinition of " + quoted(name.text));

    const int slot = p.locals++;
    p.scopes.back().emplace(name.text, operand_t { type, make_local(slot) });

    return slot;
}

} // namespace rcc
