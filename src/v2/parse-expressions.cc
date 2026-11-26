// -*- mode: c++; -*-

// 6.5: the expressions, from the primary expression up to the comma,
// one function for each level of the grammar.  A level whose operator
// rcc-v2 does not compile refuses it at its token, as not yet, so an
// operand stands alone at every level and an operator stops the parse
// where it is met.

#include "parse-common.hh"

#include "build.hh"

#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>

namespace rcc {

namespace {

// The operator of a level, refused at its token after the level's
// first operand.
void
refuse_operator(const parser_t &p, std::initializer_list< tok_t > operators)
{
    if (next_is(p, operators))
        not_yet(peek(p), quoted(peek(p).text) + " operator");
}

// Whether a name is reserved for any use, 6.4.2.1p7: it begins with
// two underscores, or with one and an uppercase letter.
bool
is_reserved(std::string_view name)
{
    if (name.starts_with("__"))
        return true;

    return name.size() > 1 && name[0] == '_' && name[1] >= 'A' &&
           name[1] <= 'Z';
}

// An identifier, 6.5.2p3: a local's operand, a copy of what its
// declaration made (docs/typed-tree.md 4.5).  A name the unit defines
// as a function is a function designator, not yet; a name no scope
// declares is rcc's error.  rcc knows reserved names that no
// declaration in the unit makes, its builtins and the types the
// compiler declares, and reads some of them before it looks the name
// up, so a reserved name is not yet wherever it stands.
operand_t
identifier(parser_t &p)
{
    const token_t &t = advance(p);
    if (is_reserved(t.text))
        not_yet(t, "reserved identifier " + quoted(t.text));

    if (const operand_t *local = lookup(p, t.text))
        return *local;

    if (p.defined.contains(t.text))
        not_yet(t, "function designator " + quoted(t.text));

    fail(t, quoted(t.text) + " undeclared");
}

// A parenthesized expression, 6.5.2p6: the operand inside, its handle
// as it is, so nothing marks it and (x) = 1; stores.  rcc reads '(' and
// '{' as GNU C's statement expression, which is not yet.
operand_t
parenthesized_expression(parser_t &p)
{
    const token_t &open = advance(p);
    if (peek(p).kind == tok_t::lbrace)
        not_yet(open, "statement expression");

    operand_t e = expression(p);
    expect(p, tok_t::rparen, "expected ')'");

    return e;
}

// The primary expressions, 6.5.2: an identifier, an integer constant, a
// parenthesized expression, and each of the others refused by its name,
// a floating constant in rcc's words.
operand_t
primary_expression(parser_t &p)
{
    const token_t &t = peek(p);

    switch (t.kind) {
    case tok_t::identifier: return identifier(p);
    case tok_t::integer: return integer_constant(p);
    case tok_t::floating: not_yet(t, "floating constant");
    case tok_t::character: not_yet(t, "character constant");
    case tok_t::string: not_yet(t, "string literal");
    case tok_t::lparen: return parenthesized_expression(p);
    case tok_t::kw_generic: not_yet(t, "generic selection");
    default: fail(t, "expected expression");
    }
}

// The postfix operators, 6.5.3: a call, a subscript, a member access
// and a step.
operand_t
postfix_expression(parser_t &p)
{
    operand_t e = primary_expression(p);

    const token_t &op = peek(p);

    switch (op.kind) {
    case tok_t::lparen: not_yet(op, "function call");
    case tok_t::lbracket: not_yet(op, "array subscript");
    case tok_t::dot:
    case tok_t::arrow:
    case tok_t::inc:
    case tok_t::dec: not_yet(op, quoted(op.text) + " operator");
    default: return e;
    }
}

// The operand of an arithmetic operator: its value, 6.3.2.1p2, of type
// int, which the integer promotions leave as it is, 6.5.4.3p2.  An
// operand of another type is not yet.
expr_t
arithmetic_operand(const token_t &op, operand_t e)
{
    if (e.type != int_type())
        not_yet(op, quoted(op.text) + " on an operand whose type is not 'int'");

    return rvalue(std::move(e.node));
}

// 6.5.5, the operand of a unary operator, below.
operand_t cast_expression(parser_t &);

// +e, 6.5.4.3p2: the operand's value, which is no lvalue, and no node
// of its own.
operand_t
unary_plus(parser_t &p)
{
    const token_t &op = advance(p);
    operand_t arg = cast_expression(p);

    return { int_type(), arithmetic_operand(op, std::move(arg)) };
}

// -e and ~e, 6.5.4.3p3, p4.
operand_t
unary_arithmetic(parser_t &p)
{
    const token_t &op = advance(p);
    const unary_op_t u =
        op.kind == tok_t::minus ? unary_op_t::neg : unary_op_t::bit_not;

    operand_t arg = cast_expression(p);

    return { int_type(),
             make_unary(u, arithmetic_operand(op, std::move(arg))) };
}

// The unary operators, 6.5.4, of which rcc-v2 takes + - ~, 6.5.4.3, on
// a cast expression; GNU C's __extension__ before a cast expression,
// and its address of a label, &&next, which rcc refuses in these words.
operand_t
unary_expression(parser_t &p)
{
    const token_t &op = peek(p);

    switch (op.kind) {
    case tok_t::plus: return unary_plus(p);
    case tok_t::minus:
    case tok_t::tilde: return unary_arithmetic(p);
    case tok_t::inc:
    case tok_t::dec:
    case tok_t::bang:
    case tok_t::amp:
    case tok_t::star:
    case tok_t::kw_sizeof:
    case tok_t::kw_alignof: not_yet(op, quoted(op.text) + " operator");
    case tok_t::kw_extension:
        not_yet(op, quoted(op.text) + " in an expression");
    case tok_t::log_and: not_yet(op, "address of a label");
    default: return postfix_expression(p);
    }
}

// The cast operator, 6.5.5: a parenthesized type name starts a cast;
// anything else in parentheses is a primary expression.
operand_t
cast_expression(parser_t &p)
{
    if (peek(p).kind == tok_t::lparen && is_declaration_start(p, 1))
        not_yet(peek(p), "cast");

    return unary_expression(p);
}

// The multiplicative operators, 6.5.6.
operand_t
multiplicative_expression(parser_t &p)
{
    operand_t e = cast_expression(p);
    refuse_operator(p, { tok_t::star, tok_t::slash, tok_t::percent });

    return e;
}

// The additive operators, 6.5.7.
operand_t
additive_expression(parser_t &p)
{
    operand_t e = multiplicative_expression(p);
    refuse_operator(p, { tok_t::plus, tok_t::minus });

    return e;
}

// The bitwise shift operators, 6.5.8.
operand_t
shift_expression(parser_t &p)
{
    operand_t e = additive_expression(p);
    refuse_operator(p, { tok_t::shl, tok_t::shr });

    return e;
}

// The relational operators, 6.5.9.
operand_t
relational_expression(parser_t &p)
{
    operand_t e = shift_expression(p);
    refuse_operator(p, { tok_t::lt, tok_t::gt, tok_t::le, tok_t::ge });

    return e;
}

// The equality operators, 6.5.10.
operand_t
equality_expression(parser_t &p)
{
    operand_t e = relational_expression(p);
    refuse_operator(p, { tok_t::eq, tok_t::ne });

    return e;
}

// The bitwise AND operator, 6.5.11.
operand_t
and_expression(parser_t &p)
{
    operand_t e = equality_expression(p);
    refuse_operator(p, { tok_t::amp });

    return e;
}

// The bitwise exclusive OR operator, 6.5.12.
operand_t
exclusive_or_expression(parser_t &p)
{
    operand_t e = and_expression(p);
    refuse_operator(p, { tok_t::caret });

    return e;
}

// The bitwise inclusive OR operator, 6.5.13.
operand_t
inclusive_or_expression(parser_t &p)
{
    operand_t e = exclusive_or_expression(p);
    refuse_operator(p, { tok_t::pipe });

    return e;
}

// The logical AND operator, 6.5.14.
operand_t
logical_and_expression(parser_t &p)
{
    operand_t e = inclusive_or_expression(p);
    refuse_operator(p, { tok_t::log_and });

    return e;
}

// The logical OR operator, 6.5.15.
operand_t
logical_or_expression(parser_t &p)
{
    operand_t e = logical_and_expression(p);
    refuse_operator(p, { tok_t::log_or });

    return e;
}

// The conditional operator, 6.5.16.
operand_t
conditional_expression(parser_t &p)
{
    operand_t e = logical_or_expression(p);
    refuse_operator(p, { tok_t::question });

    return e;
}

} // namespace

// The assignment operators, 6.5.17: the simple one, right-associative,
// and the compound ones, not yet.  The left operand is a modifiable
// lvalue, 6.5.17.1p2, which rcc asks at the operator before it reads
// the right operand.  The right operand's value is converted to the
// assignment's type, 6.5.17.2p2, the left operand's, 6.5.17.1p3: a
// value of another type is not yet.
operand_t
assignment_expression(parser_t &p)
{
    operand_t lhs = conditional_expression(p);
    refuse_operator(p,
                    { tok_t::mul_assign, tok_t::div_assign, tok_t::rem_assign,
                      tok_t::add_assign, tok_t::sub_assign, tok_t::shl_assign,
                      tok_t::shr_assign, tok_t::and_assign, tok_t::xor_assign,
                      tok_t::or_assign });

    if (peek(p).kind != tok_t::assign)
        return lhs;

    const token_t &op = advance(p);
    if (!is_assignable(lhs.node))
        fail(op, "expression is not assignable");

    const token_t &start = peek(p);
    operand_t rhs = assignment_expression(p);
    if (rhs.type != lhs.type)
        not_yet(start, "conversion to the type of the left operand");

    return { lhs.type,
             make_assign(std::move(lhs.node), rvalue(std::move(rhs.node))) };
}

// The comma operator, 6.5.18.
operand_t
expression(parser_t &p)
{
    operand_t e = assignment_expression(p);
    refuse_operator(p, { tok_t::comma });

    return e;
}

} // namespace rcc
