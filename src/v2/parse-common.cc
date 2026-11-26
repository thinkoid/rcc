// -*- mode: c++; -*-

// The diagnostics, the token primitives and the types the parser's
// files share, declared in parse-common.hh, as rcc's parser has them.

#include "parse-common.hh"

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

std::string
quoted(std::string_view name)
{
    return "'" + std::string(name) + "'";
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

} // namespace rcc
