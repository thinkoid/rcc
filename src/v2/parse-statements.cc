// -*- mode: c++; -*-

// 6.8: the statements, of which rcc-v2 takes the compound statement,
// the expression and null statements, the if and switch statements, the
// while, do and for statements, goto, break and continue, the return
// statement, the three kinds of label, and the block items of a block,
// a declaration among them.  An asm statement is refused at its first
// token by its name.

#include "parse-common.hh"

#include "build.hh"

#include <cstdint>
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

        expr_t converted =
            assignment_conversion(start, std::move(value), p.result);
        expect(p, tok_t::semicolon, "expected ';' after return statement");

        return make_return(std::move(converted), view_of(p.result));
    }

    if (!is< void_t >(p.result))
        fail(keyword, "non-void function should return a value");

    advance(p);

    return make_return_void();
}

// A continue stands only in a loop's body, 6.8.7.3p1, and is a jump to
// the innermost loop's continuation, p2, past any switch between: the
// loop is marked continued, which picks the labels its row places.
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

// The innermost of the loops and the switches whose bodies are being
// read, or null where there is none: the one of the larger number, since
// the two are numbered in one count as they are opened, and the open
// ones nest.
jump_target_t *
innermost_breakable(parser_t &p)
{
    jump_target_t *loop = p.loops.empty() ? nullptr : &p.loops.back();
    jump_target_t *sw =
        p.switches.empty() ? nullptr : &p.switches.back().target;
    if (!loop || (sw && sw->id > loop->id))
        return sw;

    return loop;
}

// A break stands only in a loop's body or a switch's, 6.8.7.4p1, and
// jumps past the innermost, p2, which is marked broken: that picks the
// labels its row places.
stmt_t
break_statement(parser_t &p)
{
    const token_t &keyword = advance(p);
    jump_target_t *target = innermost_breakable(p);
    if (!target)
        fail(keyword, "'break' statement not in a loop or a switch");

    target->broken = true;
    expect(p, tok_t::semicolon, "expected ';' after break statement");

    return make_break(target->id);
}

// What the parser holds of the C label the token names, made at the
// first meeting of the name in the function, a goto or its definition,
// with the next number among the function's C labels.
label_name_t &
label_named(parser_t &p, const token_t &name)
{
    const int next = static_cast< int >(p.labels.size());

    return p.labels.try_emplace(name.text, label_name_t { next }).first->second;
}

// A goto names a label of the function, defined before it or after it,
// 6.8.7.2p1, so its name is kept for the function's end to check,
// require_labels(), and the goto names the label by its number.  GNU
// C's computed goto, goto *p, is not yet, in rcc's words.
stmt_t
goto_statement(parser_t &p)
{
    const token_t &keyword = advance(p);
    if (peek(p).kind == tok_t::star)
        not_yet(keyword, "computed " + quoted(keyword.text));

    const token_t &name =
        expect(p, tok_t::identifier, "expected label name after 'goto'");
    p.gotos.push_back(&name);
    const int label = label_named(p, name).number;

    expect(p, tok_t::semicolon, "expected ';' after goto statement");

    return make_goto(label);
}

// 6.8.7.1: a goto, a continue, a break or a return.
stmt_t
jump_statement(parser_t &p)
{
    switch (peek(p).kind) {
    case tok_t::kw_goto: return goto_statement(p);
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

// A switch, 6.8.5.3: the controlling expression, an integer, p1, a
// pointer an error at its first token in rcc's words, read as any
// operand whose value is read, at its first token, and promoted, p5,
// the switch of its promoted type; then the body, with the switch open
// and numbered in the count of the function's loops and switches, in
// which a break leaves the switch, 6.8.7.4p2, a continue goes on with
// the loop around it, and each case and default label belongs to this
// switch unless a switch inside the body is closer, 6.8.5.3p4.
stmt_t
switch_statement(parser_t &p)
{
    advance(p);
    expect(p, tok_t::lparen, "expected '(' after 'switch'");

    const token_t &start = peek(p);
    operand_t e = expression(p);
    e.node = rvalue(start, e);
    if (is< pointer_t >(e.type))
        fail(start, "controlling expression of 'switch' is not an integer");

    const type_t t = promoted(e.type);
    expr_t cond = convert_to(e, t);

    expect(p, tok_t::rparen, "expected ')' after controlling expression");

    p.switches.push_back({ .target = { .id = p.loop_count++ }, .type = t });
    stmt_t body = statement(p);

    open_switch_t sw = std::move(p.switches.back());
    p.switches.pop_back();

    return make_switch(std::move(cond), view_of(t), sign_of(t), std::move(body),
                       std::move(sw.cases), sw.default_, sw.target);
}

// The if statement, 6.8.5.2, with an else part or without one, or a
// switch; an else goes with the nearest if, 6.8.5.2p3, since it is read
// right after the then part.  The condition is converted as any operand
// whose value is read, and nothing else converts it, its value compared
// with 0, 6.8.5.2p2.
stmt_t
selection_statement(parser_t &p)
{
    if (peek(p).kind == tok_t::kw_switch)
        return switch_statement(p);

    advance(p);
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
    jump_target_t loop;
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

    const jump_target_t loop = p.loops.back();
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

// The for statement, 6.8.6.4: the init, a declaration, or an expression
// statement or the null statement, whose ';' is that statement's; the
// condition where the next token is no ';', and an omitted one
// otherwise, p2; the step, an expression evaluated as a void
// expression, p1, where the next token is no ')', and the null
// statement otherwise; then the body.  The scope of a name the init
// declares is the rest of the for, p1, so the for opens a scope of its
// own before the init and closes it after the body, as rcc's
// for_statement() does, and a braced body's nests in it.
stmt_t
for_statement(parser_t &p)
{
    advance(p);
    expect(p, tok_t::lparen, "expected '(' after 'for'");

    p.scopes.emplace_back();
    stmt_t init =
        is_declaration_start(p) ? for_declaration(p) : expression_statement(p);

    std::optional< expr_t > cond;
    if (peek(p).kind != tok_t::semicolon)
        cond = condition(p);

    expect(p, tok_t::semicolon, "expected ';' after for condition");

    stmt_t step = make_null();
    if (peek(p).kind != tok_t::rparen)
        step = make_expr_stmt(discarded(expression(p)));

    expect(p, tok_t::rparen, "expected ')' after for clauses");

    loop_body_t l = loop_body(p);
    p.scopes.pop_back();

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

// A named label, identifier :, 6.8.2p1, which defines its name among
// the function's C labels, the name's number made at its first meeting;
// a name is defined once in a function, p3, which rcc says at the
// second definition's name, in any block, since a label's name has
// function scope, 6.2.1p3.
stmt_t
named_label(parser_t &p)
{
    const token_t &name = advance(p);
    advance(p);

    label_name_t &label = label_named(p, name);
    if (label.defined)
        fail(name, "redefinition of label " + quoted(name.text));

    label.defined = true;

    return make_label(label.number);
}

// A case's value in the words of a diagnostic, rcc's case_value(): the
// value of the controlling expression's promoted type t, an unsigned
// type's read from its bits at its width, since the tree holds the
// value its register holds, read signed, an unsigned 4294967295 as -1.
std::string
case_value(long long value, const type_t &t)
{
    if (sign_of(t) == sign_t::signed_)
        return std::to_string(value);

    if (view_of(t) == view_t::i64)
        return std::to_string(static_cast< unsigned long long >(value));

    return std::to_string(static_cast< std::uint32_t >(value));
}

// case, an integer constant expression and a colon, 6.8.2p1.  It stands
// only in a switch, p2, and belongs to the innermost, 6.8.5.3p4.  Its
// constant is a conditional expression whose value is read, which the
// builders fold to a constant node, constant_of(), or it is no constant
// expression, an error at its first token, which warns there of a
// signed overflow in it, warn_overflowed().  It is converted to the
// promoted type of the controlling expression, p5, silently, case
// 0x100000001: on an int being the case 1, and no two cases of one
// switch have one value after the conversion, p3, which rcc says at the
// second's first token with the first's line and the value of that
// type, case_value()'s.  GNU C's range, case 1 ... 5:, is not yet, in
// rcc's words.
stmt_t
case_label(parser_t &p)
{
    const token_t &keyword = advance(p);
    if (p.switches.empty())
        fail(keyword, "'case' label not in a switch");

    const token_t &start = peek(p);
    const operand_t e = conditional_expression(p);
    if (!constant_of(rvalue(start, e)))
        fail(start, "case label is not an integer constant expression");

    warn_overflowed(p, start, e);

    if (peek(p).kind == tok_t::ellipsis)
        not_yet(peek(p), "case range");

    expect(p, tok_t::colon, "expected ':' after case label");

    open_switch_t &sw = p.switches.back();
    const long long value = *constant_of(convert_to(e, sw.type));

    const auto [first, added] = sw.values.emplace(value, &start);
    if (!added)
        fail(start, "duplicate case value " + case_value(value, sw.type) +
                        ", first at line " +
                        std::to_string(first->second->where.line));

    sw.cases.push_back({ value, p.case_labels });

    return make_case_label(p.case_labels++);
}

// default and a colon, 6.8.2p1: in a switch alone, p2, the innermost's,
// 6.8.5.3p4, and one to a switch, p3, which rcc says at the second's
// keyword with the first's line.
stmt_t
default_label(parser_t &p)
{
    const token_t &keyword = advance(p);
    if (p.switches.empty())
        fail(keyword, "'default' label not in a switch");

    open_switch_t &sw = p.switches.back();
    if (sw.default_)
        fail(keyword, "second 'default' label in a switch, first at line " +
                          std::to_string(sw.default_at->where.line));

    expect(p, tok_t::colon, "expected ':' after 'default'");

    sw.default_ = p.case_labels;
    sw.default_at = &keyword;

    return make_case_label(p.case_labels++);
}

// A label of any of the three kinds of 6.8.2p1.
stmt_t
label(parser_t &p)
{
    switch (peek(p).kind) {
    case tok_t::kw_case: return case_label(p);
    case tok_t::kw_default: return default_label(p);
    default: return named_label(p);
    }
}

// A label before a statement where a statement is wanted, 6.8.2p1: the
// then part of an if, the body of a loop or of a switch, a labeled
// statement's own.  It is a block of the two, rcc's labeled_statement(),
// which opens no scope and is no one jump, so an if whose then part it
// is takes the label past the then part.
stmt_t
labeled_statement(parser_t &p)
{
    std::vector< stmt_t > items;
    items.push_back(label(p));
    items.push_back(statement(p));

    return make_block(std::move(items));
}

// 6.8.1p1: a statement, by its first token.  A declaration is a block
// item and no statement, so it is no then part and no body, 6.8.3p1,
// which rcc says at its first token; a label starts a labeled
// statement; and a token that starts no other statement starts an
// expression statement, the null statement among them, 6.8.4.
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
        return labeled_statement(p);

    if (is_declaration_start(p))
        fail(t, "expected statement, found a declaration");

    return expression_statement(p);
}

// One block item, 6.8.3p1, appended to items: a label, which is an
// item of its own, so it may stand before a declaration and before the
// closing brace; a declaration, of the statements it makes; or a
// statement.
void
block_item(parser_t &p, std::vector< stmt_t > &items)
{
    if (is_label_start(p)) {
        items.push_back(label(p));
        return;
    }

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
