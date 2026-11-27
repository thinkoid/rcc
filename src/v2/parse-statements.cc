// -*- mode: c++; -*-

// 6.8: the statements, of which rcc-v2 takes the compound statement,
// the expression and null statements, the if statement, the return
// statement with an expression, and the block items of a block, a
// declaration among them.  Every other statement is refused at its
// first token by its name, as is a label.

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

    return make_return(rvalue(std::move(value.node)));
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

// An expression statement, its expression evaluated for its effect and
// its value dropped, 6.8.4p2, or the null statement, 6.8.4p3.  The
// expression is converted as any operand whose value is read,
// 6.3.2.1p2, so x; is the load of x, as in rcc.
stmt_t
expression_statement(parser_t &p)
{
    if (accept(p, tok_t::semicolon))
        return make_null();

    operand_t e = expression(p);
    expect(p, tok_t::semicolon, "expected ';' after expression");

    return make_expr_stmt(rvalue(std::move(e.node)));
}

// 6.8.1, a then part and an else part, below.
stmt_t statement(parser_t &);

// The if statement, 6.8.5.2, with an else part or without one; an else
// goes with the nearest if, 6.8.5.2p3, since it is read right after the
// then part.  The condition is converted as any operand whose value is
// read, and nothing else converts it, its value compared with 0,
// 6.8.5.2p2.  A switch is not yet.
stmt_t
selection_statement(parser_t &p)
{
    const token_t &t = advance(p);
    if (t.kind == tok_t::kw_switch)
        not_yet(t, quoted(t.text) + " statement");

    expect(p, tok_t::lparen, "expected '(' after 'if'");
    operand_t cond = expression(p);
    expect(p, tok_t::rparen, "expected ')' after condition");

    stmt_t then = statement(p);
    if (!accept(p, tok_t::kw_else))
        return make_if(rvalue(std::move(cond.node)), std::move(then));

    stmt_t otherwise = statement(p);

    return make_if_else(rvalue(std::move(cond.node)), std::move(then),
                        std::move(otherwise));
}

// A compound statement, 6.8.3: a block, which opens a scope of its own
// for its declarations, 6.2.1p4, closed at its end.  Its '{' is the
// token that made it one.
stmt_t
compound_statement(parser_t &p)
{
    advance(p);

    p.scopes.emplace_back();
    std::vector< stmt_t > items = block_item_list(p);
    p.scopes.pop_back();
    expect(p, tok_t::rbrace, "expected '}' at end of block");

    return make_block(std::move(items));
}

// 6.8.1p1: a statement, by its first token.  A declaration is a block
// item and no statement, so it is no then part and no body, 6.8.3p1,
// which rcc says at its first token; a label is not yet; and a token
// that starts no other statement starts an expression statement, the
// null statement among them, 6.8.4.
stmt_t
statement(parser_t &p)
{
    const token_t &t = peek(p);

    switch (t.kind) {
    case tok_t::lbrace: return compound_statement(p);
    case tok_t::kw_if:
    case tok_t::kw_switch: return selection_statement(p);
    case tok_t::kw_while:
    case tok_t::kw_do:
    case tok_t::kw_for:
    case tok_t::kw_asm: not_yet(t, quoted(t.text) + " statement");
    case tok_t::kw_return:
    case tok_t::kw_break:
    case tok_t::kw_continue:
    case tok_t::kw_goto: return jump_statement(p);
    default: break;
    }

    if (is_label_start(p))
        not_yet(t, "label");

    if (is_declaration_start(p))
        fail(t, "expected statement, found a declaration");

    return expression_statement(p);
}

// One block item, 6.8.3p1, appended to items: a label, which is an
// item of its own; a declaration, of the statements it makes; or a
// statement.
void
block_item(parser_t &p, std::vector< stmt_t > &items)
{
    if (is_label_start(p))
        not_yet(peek(p), "label");

    if (is_declaration_start(p)) {
        for (stmt_t &decl : declaration(p))
            items.push_back(std::move(decl));
        return;
    }

    items.push_back(statement(p));
}

} // namespace

// The items up to the closing brace, which is left for the caller.
std::vector< stmt_t >
block_item_list(parser_t &p)
{
    std::vector< stmt_t > items;
    while (!next_is(p, { tok_t::rbrace, tok_t::eof }))
        block_item(p, items);

    return items;
}

} // namespace rcc
