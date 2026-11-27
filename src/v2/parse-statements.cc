// -*- mode: c++; -*-

// 6.8: the statements, of which rcc-v2 takes the compound statement,
// the expression and null statements, the if statement, the while, do
// and for statements, break and continue, the return statement, and the
// block items of a block, a declaration among them.  Every other
// statement is refused at its first token by its name, as is a label.

#include "parse-common.hh"

#include "build.hh"

#include <optional>
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

// return E; of a void E in a void function breaks 6.8.7.5p1, and GNU C
// takes it: rcc takes it with a warning, at the return, as the block of
// E for its effect and the return, a block of no scope, rcc's
// void_return().
stmt_t
void_return(parser_t &p, const token_t &keyword, operand_t value)
{
    warn(p, keyword,
         "void function returns a void expression, as GNU C allows");
    expect(p, tok_t::semicolon, "expected ';' after return statement");

    std::vector< stmt_t > items;
    items.push_back(make_expr_stmt(discarded(std::move(value))));
    items.push_back(make_return_void());

    return make_block(std::move(items));
}

// A void function takes no value and a non-void one requires one,
// 6.8.7.5p1, each said at the return; the value is read first, and is
// converted to the result type as if by assignment, p3, at its first
// token.
stmt_t
return_statement(parser_t &p)
{
    const token_t &keyword = advance(p);

    if (peek(p).kind != tok_t::semicolon) {
        const token_t &start = peek(p);

        operand_t value = expression(p);
        if (is< void_t >(p.result) && is< void_t >(value.type))
            return void_return(p, keyword, std::move(value));

        if (is< void_t >(p.result))
            fail(keyword, "void function should not return a value");

        expr_t converted = assignment_conversion(start, std::move(value),
                                                 p.result, "the result type");
        expect(p, tok_t::semicolon, "expected ';' after return statement");

        return make_return(std::move(converted));
    }

    if (!is< void_t >(p.result))
        fail(keyword, "non-void function should return a value");

    advance(p);

    return make_return_void();
}

// A continue stands only in a loop's body, 6.8.7.3p1, and is a jump to
// the innermost loop's continuation, p2: the loop is marked continued,
// which picks the labels its row places.
stmt_t
continue_statement(parser_t &p)
{
    const token_t &keyword = advance(p);
    if (p.loops.empty())
        fail(keyword, "'continue' statement not in a loop");

    p.loops.back().continued = true;
    expect(p, tok_t::semicolon, "expected ';' after continue statement");

    return make_continue(p.loops.back().id);
}

// A break stands only in a loop's body or a switch's, 6.8.7.4p1, and
// jumps past the innermost, p2; a switch is not yet, so it is the
// innermost loop's, marked broken.
stmt_t
break_statement(parser_t &p)
{
    const token_t &keyword = advance(p);
    if (p.loops.empty())
        fail(keyword, "'break' statement not in a loop or a switch");

    p.loops.back().broken = true;
    expect(p, tok_t::semicolon, "expected ';' after break statement");

    return make_break(p.loops.back().id);
}

// 6.8.7.1: a goto, a continue, a break or a return.
stmt_t
jump_statement(parser_t &p)
{
    const token_t &t = peek(p);

    switch (t.kind) {
    case tok_t::kw_goto: not_yet(t, quoted(t.text) + " statement");
    case tok_t::kw_continue: return continue_statement(p);
    case tok_t::kw_break: return break_statement(p);
    default: return return_statement(p);
    }
}

// An expression statement, its expression evaluated for its effect and
// its value dropped, 6.8.4p2, or the null statement, 6.8.4p3.  The
// expression may be void, a void call's, and is converted as any
// operand whose value is read, 6.3.2.1p2, so x; is the load of x, as in
// rcc; a function designator decays to its function's address there,
// 6.3.2.1p4, which is not yet.
stmt_t
expression_statement(parser_t &p)
{
    if (accept(p, tok_t::semicolon))
        return make_null();

    operand_t e = expression(p);
    expect(p, tok_t::semicolon, "expected ';' after expression");

    return make_expr_stmt(discarded(std::move(e)));
}

// 6.8.1, a then part and an else part, below.
stmt_t statement(parser_t &);

// The controlling expression of an if or of a loop, 6.8.5.2p2,
// 6.8.6.1p3: its value read, rvalue()'s at its first token, and
// compared with 0, so nothing else converts it.
expr_t
condition(parser_t &p)
{
    const token_t &start = peek(p);

    return rvalue(start, expression(p));
}

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
    expr_t cond = condition(p);
    expect(p, tok_t::rparen, "expected ')' after condition");

    stmt_t then = statement(p);
    if (!accept(p, tok_t::kw_else))
        return make_if(std::move(cond), std::move(then));

    stmt_t otherwise = statement(p);

    return make_if_else(std::move(cond), std::move(then), std::move(otherwise));
}

// A loop's body and what the parser knows of the loop once it is read.
struct loop_body_t
{
    stmt_t body;
    loop_t loop;
};

// The body of a loop, where a break and a continue of the loop may
// stand, 6.8.7.3p1, 6.8.7.4p1, but for those of a loop inside it: the
// loop, numbered among the function's, is the innermost while its body
// is read, and the parser's function for the loop is the accumulator
// of what the body held of it (docs/typed-tree.md 4.4, 4.6).
loop_body_t
loop_body(parser_t &p)
{
    p.loops.push_back({ .id = p.loop_count++ });
    stmt_t body = statement(p);

    const loop_t loop = p.loops.back();
    p.loops.pop_back();

    return { std::move(body), loop };
}

// The while statement, 6.8.6.2, whose condition is converted as an
// if's is, its value compared with 0, 6.8.6.1p3.
stmt_t
while_statement(parser_t &p)
{
    advance(p);
    expect(p, tok_t::lparen, "expected '(' after 'while'");
    expr_t cond = condition(p);
    expect(p, tok_t::rparen, "expected ')' after condition");

    loop_body_t l = loop_body(p);

    return make_while(std::move(cond), std::move(l.body), l.loop);
}

// The do statement, 6.8.6.3: the body, then the condition, evaluated
// after each execution of the body, p1, and the ';' that ends the
// statement.
stmt_t
do_statement(parser_t &p)
{
    advance(p);

    loop_body_t l = loop_body(p);

    expect(p, tok_t::kw_while, "expected 'while' after the body of 'do'");
    expect(p, tok_t::lparen, "expected '(' after 'while'");
    expr_t cond = condition(p);
    expect(p, tok_t::rparen, "expected ')' after condition");
    expect(p, tok_t::semicolon, "expected ';' after do statement");

    return make_do(std::move(l.body), std::move(cond), l.loop);
}

// The for statement, 6.8.6.4: the init, an expression statement or the
// null statement, whose ';' is that statement's; the condition where
// the next token is no ';', and an omitted one otherwise, p2; the step,
// an expression evaluated as a void expression, p1, where the next
// token is no ')', and the null statement otherwise; then the body.  A
// declaration as the init is not yet.
stmt_t
for_statement(parser_t &p)
{
    advance(p);
    expect(p, tok_t::lparen, "expected '(' after 'for'");

    if (is_declaration_start(p))
        not_yet(peek(p), "declaration in a 'for'");

    stmt_t init = expression_statement(p);

    std::optional< expr_t > cond;
    if (peek(p).kind != tok_t::semicolon)
        cond = condition(p);

    expect(p, tok_t::semicolon, "expected ';' after for condition");

    stmt_t step = make_null();
    if (peek(p).kind != tok_t::rparen)
        step = make_expr_stmt(discarded(expression(p)));

    expect(p, tok_t::rparen, "expected ')' after for clauses");

    loop_body_t l = loop_body(p);
    if (!cond)
        return make_for(std::move(init), std::move(step), std::move(l.body),
                        l.loop);

    return make_for(std::move(init), std::move(*cond), std::move(step),
                    std::move(l.body), l.loop);
}

// The iteration statements, 6.8.6, by their first token.
stmt_t
iteration_statement(parser_t &p)
{
    switch (peek(p).kind) {
    case tok_t::kw_while: return while_statement(p);
    case tok_t::kw_do: return do_statement(p);
    default: return for_statement(p);
    }
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
    case tok_t::kw_for: return iteration_statement(p);
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
