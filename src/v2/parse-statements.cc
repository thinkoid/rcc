// -*- mode: c++; -*-

// 6.8: the statements, of which rcc-v2 takes the return statement with
// an expression, and the block items of a function's body.  Every
// other statement is refused at its first token by its name, as is a
// label and a declaration among the block items.

#include "parse-common.hh"

#include "build.hh"

#include <string>
#include <utility>
#include <vector>

namespace rcc {

namespace {

// Whether a label starts at the token ahead: case, default, or an
// identifier and a colon, 6.8.2p1.
bool
is_label_start(const parser_t &p)
{
    if (next_is(p, { tok_t::kw_case, tok_t::kw_default }))
        return true;

    return peek(p).kind == tok_t::identifier && peek(p, 1).kind == tok_t::colon;
}

// A return without an expression stands only in a function whose
// result type is void, and a function rcc-v2 defines returns int,
// 6.8.7.5p1.  A value of another type than the result type is
// converted as if by assignment, p3, which is not yet.
stmt_t
return_statement(parser_t &p)
{
    const token_t &keyword = advance(p);
    if (peek(p).kind == tok_t::semicolon)
        fail(keyword, "non-void function should return a value");

    const token_t &start = peek(p);
    operand_t value = expression(p);
    if (value.type != p.result)
        not_yet(start, "conversion to the result type");

    expect(p, tok_t::semicolon, "expected ';' after return statement");

    return make_return(std::move(value.node));
}

// 6.8.7.1: a goto, a continue, a break or a return.
stmt_t
jump_statement(parser_t &p)
{
    const token_t &t = peek(p);

    switch (t.kind) {
    case tok_t::kw_goto:
    case tok_t::kw_continue:
    case tok_t::kw_break: not_yet(t, quoted(t.text) + " statement");
    default: return return_statement(p);
    }
}

// 6.8.1p1: a statement, by its first token.  A block item that is no
// label and no declaration is one, so a token that starts no other
// statement starts an expression statement, the null statement among
// them, 6.8.4.
stmt_t
statement(parser_t &p)
{
    const token_t &t = peek(p);

    switch (t.kind) {
    case tok_t::lbrace: not_yet(t, "compound statement");
    case tok_t::kw_if:
    case tok_t::kw_switch:
    case tok_t::kw_while:
    case tok_t::kw_do:
    case tok_t::kw_for:
    case tok_t::kw_asm: not_yet(t, quoted(t.text) + " statement");
    case tok_t::kw_return:
    case tok_t::kw_break:
    case tok_t::kw_continue:
    case tok_t::kw_goto: return jump_statement(p);
    default: not_yet(t, "expression statement");
    }
}

// One block item, 6.8.3p1: a label, which is an item of its own, a
// declaration or a statement.
stmt_t
block_item(parser_t &p)
{
    if (is_label_start(p))
        not_yet(peek(p), "label");

    if (is_declaration_start(p))
        not_yet(peek(p), "declaration");

    return statement(p);
}

} // namespace

// The items up to the closing brace, which is left for the caller.
std::vector< stmt_t >
block_item_list(parser_t &p)
{
    std::vector< stmt_t > items;
    while (!next_is(p, { tok_t::rbrace, tok_t::eof }))
        items.push_back(block_item(p));

    return items;
}

} // namespace rcc
