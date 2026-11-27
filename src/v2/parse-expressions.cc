// -*- mode: c++; -*-

// 6.5: the expressions, from the primary expression up to the comma,
// one function for each level of the grammar.  A level of operators
// rcc-v2 compiles reads them in rcc's loop; a level whose operator it
// does not compile refuses it at its token, as not yet, after the
// level's first operand.

#include "parse-common.hh"

#include "build.hh"

#include <cstddef>
#include <initializer_list>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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

// An identifier, 6.5.2p3: a local's operand, a copy of what its
// declaration made (docs/typed-tree.md 4.5); an object the file
// declares, a copy of the leaf its first declaration made, whether the
// unit defines the object or not, which generation asks; or a name the
// file declares as a function, a function designator of the function's
// type and no node; a name no scope declares is rcc's error.  Each use
// of a static function is recorded, for the end of the unit to require
// its definition.  rcc knows reserved names that no declaration in the
// unit makes, its builtins and the types the compiler declares, and
// reads some of them before it looks the name up, so a reserved name is
// not yet wherever it stands.
operand_t
identifier(parser_t &p)
{
    const token_t &t = advance(p);
    if (is_reserved(t.text))
        not_yet(t, "reserved identifier " + quoted(t.text));

    if (const operand_t *local = lookup(p, t.text))
        return { local->type, local->node, &t };

    if (const auto object = p.objects.find(t.text); object != p.objects.end())
        return { object->second.type, object->second.leaf, &t };

    const auto it = p.functions.find(t.text);
    if (it == p.functions.end())
        fail(t, quoted(t.text) + " undeclared");

    const function_name_t &f = it->second;
    if (!p.unit.functions[f.index].external)
        p.internal_uses.push_back({ f.index, &t });

    return { f.type, std::nullopt, &t };
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
// character constant, a parenthesized expression, and each of the
// others refused by its name, a floating constant in rcc's words.
operand_t
primary_expression(parser_t &p)
{
    const token_t &t = peek(p);

    switch (t.kind) {
    case tok_t::identifier: return identifier(p);
    case tok_t::integer: return integer_constant(p);
    case tok_t::floating: not_yet(t, "floating constant");
    case tok_t::character: return character_constant(p);
    case tok_t::string: not_yet(t, "string literal");
    case tok_t::lparen: return parenthesized_expression(p);
    case tok_t::kw_generic: not_yet(t, "generic selection");
    default: fail(t, "expected expression");
    }
}

// The callee of a call at the '(' op, 6.5.3.3p1, rcc's callee(): a
// function's name, in parentheses or not, the direct call, which names
// its function.  The name of anything else is refused by name.  Any
// other expression is read for its value, a void one refused, and is no
// function and no pointer to one, rcc-v2 having no pointer.  Returns
// the function's name in the file's scope.
const function_name_t &
callee(const parser_t &p, const token_t &op, const operand_t &e)
{
    if (is< function_t >(e.type))
        return p.functions.at(e.name->text);

    if (e.name)
        fail(op,
             "called object " + quoted(e.name->text) + " is not a function");

    require_value(op, e);
    fail(op, "called object is not a function or a pointer to one");
}

// The argument ahead, the call's index-th, rcc's argument(): converted
// to its parameter's type as if by assignment, 6.5.3.3p6, with the sign
// of that type, which the call's load of it into its register extends
// a narrow one by.  One past the parameters is an error at its first
// token, p2, said once it is read.
converted_argument_t
argument(parser_t &p,
         const function_t &type,
         std::size_t index,
         std::string_view function)
{
    const token_t &start = peek(p);
    operand_t arg = assignment_expression(p);

    if (index >= type.params.size())
        fail(start, "too many arguments to function " + quoted(function));

    const type_t &param = *type.params[index];

    return { assignment_conversion(start, std::move(arg), param),
             sign_of(param) };
}

// The arguments of a call, argument()'s each, as many as the callee's
// parameters, 6.5.3.3p2: fewer is an error at the token that ends them.
std::vector< converted_argument_t >
argument_expression_list(parser_t &p,
                         const function_t &type,
                         std::string_view function)
{
    std::vector< converted_argument_t > args;

    if (peek(p).kind != tok_t::rparen) {
        do
            args.push_back(argument(p, type, args.size(), function));
        while (accept(p, tok_t::comma));
    }

    if (args.size() < type.params.size())
        fail(peek(p), "too few arguments to function " + quoted(function));

    return args;
}

// A call, 6.5.3.3, its '(' op: the callee, the arguments and the ')'.
// The call is an expression of the function's result type, p5,
// converted where it is used as any expression of that type, or void.
// Each argument is stored in a hidden local of the call, rcc's argument
// slot (backend.md chapter 8), numbered among the function's locals
// after the numbers the calls within its arguments took.
operand_t
call(parser_t &p, const token_t &op, const operand_t &e)
{
    advance(p);

    const function_name_t &f = callee(p, op, e);
    const function_t &type = as< function_t >(f.type);
    const std::string &name = p.unit.functions[f.index].name;

    const type_t &result = *type.result;

    std::vector< converted_argument_t > args =
        argument_expression_list(p, type, name);
    expect(p, tok_t::rparen, "expected ')' after arguments");

    const int first = p.locals;
    p.locals += static_cast< int >(args.size());

    if (is< void_t >(result))
        return { result, make_call_void(name, first, std::move(args)) };

    return { result, make_call(name, view_of(result), first, std::move(args)) };
}

// The postfix operators, 6.5.3, left to right: a call, and a subscript,
// a member access and a step, which are not yet.
operand_t
postfix_expression(parser_t &p)
{
    operand_t e = primary_expression(p);

    for (;;) {
        const token_t &op = peek(p);

        switch (op.kind) {
        case tok_t::lparen: e = call(p, op, e); break;
        case tok_t::lbracket: not_yet(op, "array subscript");
        case tok_t::dot:
        case tok_t::arrow:
        case tok_t::inc:
        case tok_t::dec: not_yet(op, quoted(op.text) + " operator");
        default: return e;
        }
    }
}

// The value of e in its promoted type, rcc's promote().
expr_t
promote(const operand_t &e)
{
    return convert_to(e, promoted(e.type));
}

// The rank of an integer type, 6.3.1.1p1, by its kind, rcc's rank():
// int, then long, then long long, each with its unsigned version, and
// each of its own rank though long and long long are of one width on
// LP64.
int
rank(const int_t &t)
{
    switch (t.kind) {
    case int_kind_t::int_:
    case int_kind_t::unsigned_: return 3;
    case int_kind_t::long_:
    case int_kind_t::unsigned_long: return 4;
    case int_kind_t::long_long:
    case int_kind_t::unsigned_long_long: return 5;
    default: throw std::logic_error("v2: rank of an integer type of no kind");
    }
}

// The unsigned type of a signed type's rank, the last conversion of
// 6.3.1.8p1's list, rcc's unsigned_kind().
int_kind_t
unsigned_kind(int_kind_t kind)
{
    switch (kind) {
    case int_kind_t::int_: return int_kind_t::unsigned_;
    case int_kind_t::long_: return int_kind_t::unsigned_long;
    case int_kind_t::long_long: return int_kind_t::unsigned_long_long;
    default: return kind;
    }
}

// The common type of the usual arithmetic conversions, 6.3.1.8p1, on
// the promoted operands, by their ranks, rcc's common_type(): of one
// sign, the higher rank; of two, the unsigned one where its rank is at
// least the signed one's, else the signed one where it is wider, else
// the unsigned type of the signed one's rank.  On LP64 int and unsigned
// meet at unsigned, long and unsigned at long, and unsigned long and
// long long, of one width and two ranks, at unsigned long long.
type_t
common_type(const type_t &a, const type_t &b)
{
    const type_t x = promoted(a);
    const type_t y = promoted(b);
    const int_t &i = as< int_t >(x);
    const int_t &j = as< int_t >(y);

    if (i.is_signed == j.is_signed)
        return rank(i) >= rank(j) ? x : y;

    const type_t &u = i.is_signed ? y : x;
    const type_t &s = i.is_signed ? x : y;

    if (rank(as< int_t >(u)) >= rank(as< int_t >(s)))
        return u;

    // a signed type wider than the unsigned one holds all its values
    if (as< int_t >(s).width > as< int_t >(u).width)
        return s;

    return make_int(as< int_t >(s).width, false,
                    unsigned_kind(as< int_t >(s).kind));
}

// The operator of an operation of a type of the sign, LLVM's names: an
// operator read by its signed name, sdiv for /, is the unsigned one,
// udiv, where the type is unsigned, 6.5.6p6, 6.5.8p5, 6.5.9p6, and an
// operator of no sign is itself.
binary_op_t
by_sign(binary_op_t op, sign_t sign)
{
    if (sign == sign_t::signed_)
        return op;

    switch (op) {
    case binary_op_t::sdiv: return binary_op_t::udiv;
    case binary_op_t::srem: return binary_op_t::urem;
    case binary_op_t::ashr: return binary_op_t::lshr;
    case binary_op_t::slt: return binary_op_t::ult;
    case binary_op_t::sle: return binary_op_t::ule;
    case binary_op_t::sgt: return binary_op_t::ugt;
    case binary_op_t::sge: return binary_op_t::uge;
    default: return op;
    }
}

// The operand of a unary arithmetic operator, the value it reads, rcc's
// rvalue() and require_integer(): not void, require_value()'s, and not
// a function designator, whose value is a pointer, 6.3.2.1p4, which no
// operator of 6.5.4.3 takes, p1; each an error at the operator.
void
require_arithmetic(const token_t &op, const operand_t &e)
{
    require_value(op, e);

    if (is< function_t >(e.type))
        fail(op, "operand of " + quoted(op.text) + " is a pointer");
}

// 6.5.5, the operand of a unary operator, below.
operand_t cast_expression(parser_t &);

// +e, 6.5.4.3p2, rcc's unary_plus(): the operand's value in its promoted
// type, and no node of its own; but the value is no lvalue, so an
// operand that is a place, whose promotion made no node, stands under
// a conversion to its own type, which writes nothing: +x is (T) x, T
// the promoted type of x.  The operand's mark is the value's.
operand_t
unary_plus(parser_t &p)
{
    const token_t &op = advance(p);
    operand_t arg = cast_expression(p);
    require_arithmetic(op, arg);

    const type_t t = promoted(arg.type);
    if (arg.type == t && is_assignable(*arg.node))
        return { .type = t,
                 .node = conversion(arg, t),
                 .overflowed = arg.overflowed };

    return { .type = t, .node = promote(arg), .overflowed = arg.overflowed };
}

// !e, 6.5.4.3p5: an int, its operand's value tested as it is, at its
// own type, with no promotion; ! of a constant is a constant.  A void
// operand is an error at the !, require_value()'s, and a function
// designator, a scalar, which ! takes, p1, is not yet.
operand_t
logical_not(parser_t &p)
{
    const token_t &op = advance(p);
    operand_t arg = cast_expression(p);
    expr_t value = rvalue(op, arg);
    const bool marked = overflowed(unary_op_t::log_not, arg.type, arg, value);

    return { .type = int_type(),
             .node = make_unary(unary_op_t::log_not, view_of(arg.type),
                                std::move(value)),
             .overflowed = marked };
}

// -e and ~e, 6.5.4.3p3, p4: the operand promoted, and the result of the
// promoted type.
operand_t
unary_arithmetic(parser_t &p)
{
    const token_t &op = advance(p);
    const unary_op_t u =
        op.kind == tok_t::minus ? unary_op_t::neg : unary_op_t::bit_not;

    operand_t arg = cast_expression(p);
    require_arithmetic(op, arg);

    const type_t t = promoted(arg.type);
    expr_t value = promote(arg);
    const bool marked = overflowed(u, t, arg, value);

    return { .type = t,
             .node = make_unary(u, view_of(t), std::move(value)),
             .overflowed = marked };
}

// The unary operators, 6.5.4, of which rcc-v2 takes + - ~ !, 6.5.4.3,
// on a cast expression; GNU C's __extension__ before a cast expression,
// and its address of a label, &&next, which rcc refuses in these words.
operand_t
unary_expression(parser_t &p)
{
    const token_t &op = peek(p);

    switch (op.kind) {
    case tok_t::plus: return unary_plus(p);
    case tok_t::minus:
    case tok_t::tilde: return unary_arithmetic(p);
    case tok_t::bang: return logical_not(p);
    case tok_t::inc:
    case tok_t::dec:
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

// (T) e, 6.5.5, rcc's cast(): the operand's value converted to T, a
// node even where e is of type T, since a cast yields no lvalue, p6
// (its footnote); a constant folds to the constant of T, which keeps
// its mark.  A void operand is an error at the '(', require_value()'s,
// and a function designator is not yet.
operand_t
cast(const token_t &open, const type_t &t, const operand_t &arg)
{
    const operand_t value { arg.type, rvalue(open, arg) };

    return { .type = t,
             .node = conversion(value, t),
             .overflowed = arg.overflowed };
}

// The cast operator, 6.5.5: a parenthesized type name starts a cast,
// its operand a cast expression; anything else in parentheses is a
// primary expression.  A cast to void, which discards its operand's
// value, p2, is not yet, at the '('.
operand_t
cast_expression(parser_t &p)
{
    if (peek(p).kind != tok_t::lparen || !is_declaration_start(p, 1))
        return unary_expression(p);

    const token_t &open = advance(p);
    const type_t t = type_name(p);
    expect(p, tok_t::rparen, "expected ')' after type name");

    if (is< void_t >(t))
        not_yet(open, "cast to 'void'");

    return cast(open, t, cast_expression(p));
}

// The operands of a binary operator of int that are function
// designators, whose values are pointers to their functions, 6.3.2.1p4,
// each an error at the operator, rcc's: + and - take a pointer to an
// object type alone, 6.5.7p2, p3, and no pointer to a function, two
// pointers are no sum, and a pointer is subtracted from no integer, in
// rcc's words; the other operators take no pointer, 6.5.6p2, 6.5.8p2,
// 6.5.11p2, 6.5.12p2, 6.5.13p2.
void
refuse_function_operands(binary_op_t op,
                         const token_t &at,
                         const operand_t &lhs,
                         const operand_t &rhs)
{
    const bool left = is< function_t >(lhs.type);
    const bool right = is< function_t >(rhs.type);
    if (!left && !right)
        return;

    if (op == binary_op_t::add && left && right)
        fail(at, "cannot add two pointers");

    if (op == binary_op_t::sub && !left)
        fail(at, "cannot subtract a pointer from an integer");

    if (op == binary_op_t::add || op == binary_op_t::sub)
        fail(at, "arithmetic on a pointer to a function");

    fail(at, "operand of " + quoted(at.text) + " is a pointer");
}

// A binary operator of integers, 6.5.6, 6.5.7, 6.5.11 to 6.5.13, its
// token at, rcc's arithmetic(): the usual arithmetic conversions, each
// operand converted to the common type, and the result of that type,
// the operation at its view and by its sign, 6.5.6p4, 6.5.7p5,
// 6.5.11p3, 6.5.12p3, 6.5.13p3.  rcc reads both values before it judges
// their types, so a void one is the error first, require_value()'s, and
// a function designator the next, refuse_function_operands()'s.
operand_t
arithmetic(binary_op_t op, const token_t &at, operand_t lhs, operand_t rhs)
{
    require_value(at, lhs);
    require_value(at, rhs);
    refuse_function_operands(op, at, lhs, rhs);

    const type_t t = common_type(lhs.type, rhs.type);
    expr_t l = convert_to(lhs, t);
    expr_t r = convert_to(rhs, t);
    const bool marked = overflowed(op, t, lhs, l, rhs, r);

    return { .type = t,
             .node = make_binary(by_sign(op, sign_of(t)), view_of(t),
                                 std::move(l), std::move(r)),
             .overflowed = marked };
}

// << and >>, 6.5.8, its token at, rcc's shift(): each operand promoted
// on its own, so the count keeps its promoted type, p3, and the result
// of the promoted left operand's type, the operation at its view and by
// its sign.  The errors are arithmetic()'s, in its order.
operand_t
shift(binary_op_t op, const token_t &at, operand_t lhs, operand_t rhs)
{
    require_value(at, lhs);
    require_value(at, rhs);
    refuse_function_operands(op, at, lhs, rhs);

    const type_t t = promoted(lhs.type);
    expr_t l = promote(lhs);
    expr_t r = promote(rhs);
    const bool marked = overflowed(op, t, lhs, l, rhs, r);

    return { .type = t,
             .node = make_binary(by_sign(op, sign_of(t)), view_of(t),
                                 std::move(l), std::move(r)),
             .overflowed = marked };
}

// The operator of a multiplicative operator's token: * / %, a division
// by its signed name, by_sign()'s.
binary_op_t
multiplicative_operator(tok_t kind)
{
    switch (kind) {
    case tok_t::star: return binary_op_t::mul;
    case tok_t::slash: return binary_op_t::sdiv;
    default: return binary_op_t::srem;
    }
}

// The multiplicative operators, 6.5.6, left-associative, as rcc's loop
// reads them: a / b * c is (a / b) * c.
operand_t
multiplicative_expression(parser_t &p)
{
    operand_t e = cast_expression(p);

    while (next_is(p, { tok_t::star, tok_t::slash, tok_t::percent })) {
        const token_t &op = advance(p);
        const binary_op_t b = multiplicative_operator(op.kind);

        operand_t rhs = cast_expression(p);
        e = arithmetic(b, op, std::move(e), std::move(rhs));
    }

    return e;
}

// The additive operators, 6.5.7, left-associative, as rcc's loop reads
// them: a - b - c is (a - b) - c.
operand_t
additive_expression(parser_t &p)
{
    operand_t e = multiplicative_expression(p);

    while (next_is(p, { tok_t::plus, tok_t::minus })) {
        const token_t &op = advance(p);
        const binary_op_t b =
            op.kind == tok_t::plus ? binary_op_t::add : binary_op_t::sub;

        operand_t rhs = multiplicative_expression(p);
        e = arithmetic(b, op, std::move(e), std::move(rhs));
    }

    return e;
}

// The bitwise shift operators, 6.5.8, left-associative, as rcc's loop
// reads them: a << b << c is (a << b) << c.
operand_t
shift_expression(parser_t &p)
{
    operand_t e = additive_expression(p);

    while (next_is(p, { tok_t::shl, tok_t::shr })) {
        const token_t &op = advance(p);
        const binary_op_t b =
            op.kind == tok_t::shl ? binary_op_t::shl : binary_op_t::ashr;

        operand_t rhs = additive_expression(p);
        e = shift(b, op, std::move(e), std::move(rhs));
    }

    return e;
}

// The operands of a comparison that are function designators, whose
// values are pointers to their functions, 6.3.2.1p4, rcc's: a
// relational operator takes no pointer to a function, 6.5.9p2, and
// compares a pointer with no integer, each an error at the operator in
// rcc's words; an equality operator compares two of them, and one with
// a null pointer constant, 6.5.10p2, which rcc compiles, so it is not
// yet.
void
refuse_function_comparison(binary_op_t op,
                           const token_t &at,
                           const operand_t &lhs,
                           const operand_t &rhs)
{
    const bool left = is< function_t >(lhs.type);
    const bool right = is< function_t >(rhs.type);
    if (!left && !right)
        return;

    if (op == binary_op_t::eq || op == binary_op_t::ne)
        not_yet(at, "comparison of a function designator");

    if (left && right)
        fail(at, "ordered comparison of pointers to functions");

    fail(at, "comparison between pointer and integer");
}

// A relational or an equality operator, 6.5.9, 6.5.10, its token at,
// rcc's comparison(): each operand converted to the operands' common
// type, 6.5.9p4, 6.5.10p5, the cmp at its view and by its sign, so -1 <
// 0u is an unsigned comparison, and the result an int
// whatever the operands' type, 6.5.9p7, 6.5.10p4.  rcc reads both
// values before it judges their types, so a void one is the error
// first, require_value()'s, and a function designator the next,
// refuse_function_comparison()'s.
operand_t
comparison(binary_op_t op, const token_t &at, operand_t lhs, operand_t rhs)
{
    require_value(at, lhs);
    require_value(at, rhs);
    refuse_function_comparison(op, at, lhs, rhs);

    const type_t t = common_type(lhs.type, rhs.type);
    expr_t l = convert_to(lhs, t);
    expr_t r = convert_to(rhs, t);
    const bool marked = overflowed(op, t, lhs, l, rhs, r);

    return { .type = int_type(),
             .node = make_binary(by_sign(op, sign_of(t)), view_of(t),
                                 std::move(l), std::move(r)),
             .overflowed = marked };
}

// The operator of a relational operator's token: < > <= >=, by its
// signed name, by_sign()'s.
binary_op_t
relational_operator(tok_t kind)
{
    switch (kind) {
    case tok_t::lt: return binary_op_t::slt;
    case tok_t::gt: return binary_op_t::sgt;
    case tok_t::le: return binary_op_t::sle;
    default: return binary_op_t::sge;
    }
}

// The relational operators, 6.5.9, left-associative, as rcc's loop
// reads them: a < b < c is (a < b) < c, 1 or 0 compared with c.
operand_t
relational_expression(parser_t &p)
{
    operand_t e = shift_expression(p);

    while (next_is(p, { tok_t::lt, tok_t::gt, tok_t::le, tok_t::ge })) {
        const token_t &op = advance(p);
        const binary_op_t b = relational_operator(op.kind);

        operand_t rhs = shift_expression(p);
        e = comparison(b, op, std::move(e), std::move(rhs));
    }

    return e;
}

// The equality operators, 6.5.10, left-associative, as rcc's loop
// reads them: a == b == c is (a == b) == c.
operand_t
equality_expression(parser_t &p)
{
    operand_t e = relational_expression(p);

    while (next_is(p, { tok_t::eq, tok_t::ne })) {
        const token_t &op = advance(p);
        const binary_op_t b =
            op.kind == tok_t::eq ? binary_op_t::eq : binary_op_t::ne;

        operand_t rhs = relational_expression(p);
        e = comparison(b, op, std::move(e), std::move(rhs));
    }

    return e;
}

// The bitwise AND operator, 6.5.11, left-associative.
operand_t
and_expression(parser_t &p)
{
    operand_t e = equality_expression(p);

    while (peek(p).kind == tok_t::amp) {
        const token_t &op = advance(p);

        operand_t rhs = equality_expression(p);
        e = arithmetic(binary_op_t::bit_and, op, std::move(e), std::move(rhs));
    }

    return e;
}

// The bitwise exclusive OR operator, 6.5.12, left-associative.
operand_t
exclusive_or_expression(parser_t &p)
{
    operand_t e = and_expression(p);

    while (peek(p).kind == tok_t::caret) {
        const token_t &op = advance(p);

        operand_t rhs = and_expression(p);
        e = arithmetic(binary_op_t::bit_xor, op, std::move(e), std::move(rhs));
    }

    return e;
}

// The bitwise inclusive OR operator, 6.5.13, left-associative.
operand_t
inclusive_or_expression(parser_t &p)
{
    operand_t e = exclusive_or_expression(p);

    while (peek(p).kind == tok_t::pipe) {
        const token_t &op = advance(p);

        operand_t rhs = exclusive_or_expression(p);
        e = arithmetic(binary_op_t::bit_or, op, std::move(e), std::move(rhs));
    }

    return e;
}

// && or ||, 6.5.14, 6.5.15, its token at: each operand's value,
// compared with 0 in its own type, so nothing converts it, 6.5.14p2,
// 6.5.15p2; the result an int, the operator at an int's view, 6.5.14p3,
// 6.5.15p3.  rcc reads both values before it judges their types: a
// void one is an error at the operator, and a function designator, a
// scalar, which rcc compiles, is not yet.
operand_t
logical(binary_op_t op, const token_t &at, operand_t lhs, operand_t rhs)
{
    require_value(at, lhs);
    require_value(at, rhs);

    expr_t l = rvalue(at, lhs);
    expr_t r = rvalue(at, rhs);
    const bool marked = overflowed(op, int_type(), lhs, l, rhs, r);

    return { .type = int_type(),
             .node = make_binary(op, view_t::i32, std::move(l), std::move(r)),
             .overflowed = marked };
}

// The logical AND operator, 6.5.14, left-associative, as rcc's loop
// reads it: a && b && c is (a && b) && c.
operand_t
logical_and_expression(parser_t &p)
{
    operand_t e = inclusive_or_expression(p);

    while (peek(p).kind == tok_t::log_and) {
        const token_t &op = advance(p);

        operand_t rhs = inclusive_or_expression(p);
        e = logical(binary_op_t::log_and, op, std::move(e), std::move(rhs));
    }

    return e;
}

// The logical OR operator, 6.5.15, left-associative, as rcc's loop
// reads it, and looser than &&: a || b && c is a || (b && c).
operand_t
logical_or_expression(parser_t &p)
{
    operand_t e = logical_and_expression(p);

    while (peek(p).kind == tok_t::log_or) {
        const token_t &op = advance(p);

        operand_t rhs = logical_and_expression(p);
        e = logical(binary_op_t::log_or, op, std::move(e), std::move(rhs));
    }

    return e;
}

} // namespace

// The conditional operator, 6.5.16.
operand_t
conditional_expression(parser_t &p)
{
    operand_t e = logical_or_expression(p);
    refuse_operator(p, { tok_t::question });

    return e;
}

// The assignment operators, 6.5.17: the simple one, right-associative,
// and the compound ones, not yet.  The left operand is a modifiable
// lvalue, 6.5.17.1p2, which rcc asks at the operator before it reads
// the right operand, and a function designator is none.  The right
// operand's value is converted to the assignment's type, 6.5.17.2p2,
// the left operand's, 6.5.17.1p3, assignment_conversion()'s at the
// operator.
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
    if (!lhs.node || !is_assignable(*lhs.node))
        fail(op, "expression is not assignable");

    operand_t rhs = assignment_expression(p);
    expr_t value = assignment_conversion(op, std::move(rhs), lhs.type);

    return { lhs.type, make_assign(std::move(*lhs.node), std::move(value)) };
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
