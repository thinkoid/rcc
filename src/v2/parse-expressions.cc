// -*- mode: c++; -*-

// 6.5: the expressions, from the primary expression up to the comma,
// one function for each level of the grammar, each level's operators
// read in rcc's loop.

#include "parse-common.hh"

#include "build.hh"

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace rcc {

namespace {

// An identifier, 6.5.2p3: a local's operand, a copy of what its
// declaration made (docs/typed-tree.md 4.5), or an enumeration
// constant's, the literal of its value, 6.4.4.4p3; an object the file
// declares, a copy of the leaf its first declaration made, whether the
// unit defines the object or not, which generation asks; or a name the
// file declares as a function, a function designator of the function's
// type and no node; a typedef name, which names a type, p2, and a name
// no scope declares are rcc's errors.  Each use of a static function is
// recorded, for the end of the unit to require its definition.  rcc
// knows reserved names that no declaration in the unit makes, its
// builtins and the types the compiler declares, and reads some of them
// before it looks the name up, so a reserved name is not yet wherever
// it stands.
operand_t
identifier(parser_t &p)
{
    const token_t &t = advance(p);
    if (is_reserved(t.text))
        not_yet(t, "reserved identifier " + quoted(t.text));

    if (const symbol_t *sym = lookup(p, t.text)) {
        if (sym->type_name)
            fail(t, "unexpected type name " + quoted(t.text) +
                        ": expected expression");

        return { sym->operand.type, sym->operand.node, &t };
    }

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
// function and no pointer to one, rcc-v2 having no pointer to a
// function.  Returns the function's name in the file's scope.
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

// ++E --E E++ E--, 6.5.4.1, 6.5.3.5, rcc's prefix_step() and
// postfix_step(): the operand a modifiable lvalue, p1 of each, rcc's
// require_steppable(), said at the operator, op; the result of the
// operand's type, the step's node, with no promotion and no conversion:
// the operation in int of a narrow object is the rows', as rcc's c1
// steps it.  A step is no constant expression, 6.6p3, and carries no
// mark of an overflow.  A pointer's step, by its pointee's size, is not
// yet.
operand_t
stepped(const token_t &op, step_op_t s, operand_t arg)
{
    if (!arg.node || !is_assignable(*arg.node))
        fail(op, "expression is not assignable");

    if (is< pointer_t >(arg.type))
        not_yet(op, quoted(op.text) + " with a pointer operand");

    return { arg.type, make_step(s, std::move(*arg.node)) };
}

// The postfix operators, 6.5.3, left to right: a call and a step, and a
// subscript and a member access, which are not yet.
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
        case tok_t::arrow: not_yet(op, quoted(op.text) + " operator");
        case tok_t::inc:
        case tok_t::dec: {
            const step_op_t s = op.kind == tok_t::inc ? step_op_t::post_inc :
                                                        step_op_t::post_dec;
            advance(p);
            e = stepped(op, s, std::move(e));
            break;
        }
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
// rvalue() and require_integer(): not void, require_value()'s, and no
// pointer, a function designator's value among them, 6.3.2.1p4, which
// no operator of 6.5.4.3 takes, p1; each an error at the operator.
void
require_arithmetic(const token_t &op, const operand_t &e)
{
    require_value(op, e);

    if (points(e))
        fail(op, "operand of " + quoted(op.text) + " is a pointer");
}

// 6.5.5, the operand of a unary operator, below, and 6.5.4, the
// operand of a prefix step.
operand_t cast_expression(parser_t &);
operand_t unary_expression(parser_t &);

// ++E and --E, 6.5.4.1, their operand a unary expression, so ++a++ is
// ++(a++), as rcc reads it.
operand_t
prefix_step(parser_t &p)
{
    const token_t &op = advance(p);
    const step_op_t s =
        op.kind == tok_t::inc ? step_op_t::pre_inc : step_op_t::pre_dec;

    return stepped(op, s, unary_expression(p));
}

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

// A pointer constant, (int *) 0, the value v of type t, an operand of
// an operator whose builder folds constants: rcc folds an operator on
// integer constants alone, is_integer_constant(), and writes the
// operator on a pointer constant, which rcc-v2 does not yet, refused at
// the operator.
void
refuse_pointer_constant(const token_t &at, const type_t &t, const expr_t &v)
{
    if (is< pointer_t >(t) && constant_of(v))
        not_yet(at, quoted(at.text) + " on a pointer constant");
}

// !e, 6.5.4.3p5: an int, its operand's value tested as it is, at its
// own type, with no promotion, a pointer at 64 bits; ! of a constant is
// a constant, refuse_pointer_constant()'s aside.  A void operand is an
// error at the !, require_value()'s, and a function designator, a
// scalar, which ! takes, p1, is not yet.
operand_t
logical_not(parser_t &p)
{
    const token_t &op = advance(p);
    operand_t arg = cast_expression(p);
    expr_t value = rvalue(op, arg);
    refuse_pointer_constant(op, arg.type, value);
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

// &E, 6.5.4.2, rcc's address_of(): its operand a cast expression, which
// does not decay; the result a pointer to the operand's type, p3, the
// node make_address_of() makes of a place, p1, whose builder exists for
// a place alone (principle 8).  An operand of no place is rcc's error at
// the &, but for a function designator, whose address is its decay,
// p3, and an object of static storage, a place of no & yet, each not
// yet.
operand_t
address_of(parser_t &p)
{
    const token_t &op = advance(p);
    const operand_t arg = cast_expression(p);
    if (!arg.node)
        not_yet(*arg.name, "function designator " + quoted(arg.name->text));

    if (!is_place(*arg.node) && is_assignable(*arg.node))
        not_yet(op, "address of an object of static storage");

    if (!is_place(*arg.node))
        fail(op, "cannot take the address of an rvalue");

    return { make_pointer(arg.type), make_address_of(*arg.node) };
}

// The size of the type t, 6.5.4.4p2, rcc's sizeof_(): a constant of
// type unsigned long, size_t on LP64, p5, which no overflow marks.  void
// and a function have none, p1, an error at the sizeof, op, and an enum
// in its own list, incomplete, is refuse_not_yet_type()'s there.
operand_t
sizeof_(const token_t &op, const type_t &t)
{
    if (is< void_t >(t))
        fail(op, "invalid application of 'sizeof' to a void type");

    if (is< function_t >(t))
        fail(op, "invalid application of 'sizeof' to a function type");

    refuse_not_yet_type(op, t);

    return { make_int(64, false, int_kind_t::unsigned_long),
             make_literal(size_of(t), view_t::i64) };
}

// sizeof E or sizeof (T), 6.5.4.4, rcc's sizeof_expression(): the
// operand expression is typed and dropped, never evaluated, p2, and
// does not decay, so its nodes are built and generate nothing.  A name
// of internal linkage it uses needs no definition, 6.9.1p3, since the
// size is an integer constant, and its uses are dropped.
operand_t
sizeof_expression(parser_t &p)
{
    const token_t &op = advance(p);
    const auto uses = std::ssize(p.internal_uses);

    type_t t = make_void();
    if (peek(p).kind == tok_t::lparen && is_declaration_start(p, 1)) {
        advance(p);
        t = type_name(p);
        expect(p, tok_t::rparen, "expected ')' after type name");
    }
    else
        t = unary_expression(p).type;

    p.internal_uses.erase(p.internal_uses.begin() + uses,
                          p.internal_uses.end());

    return sizeof_(op, t);
}

// The unary operators, 6.5.4, of which rcc-v2 takes the prefix steps,
// 6.5.4.1, &, 6.5.4.2, + - ~ !, 6.5.4.3, on a cast expression, and
// sizeof, 6.5.4.4; GNU C's __extension__ before a cast expression, and
// its address of a label, &&next, which rcc refuses in these words.
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
    case tok_t::dec: return prefix_step(p);
    case tok_t::amp: return address_of(p);
    case tok_t::kw_sizeof: return sizeof_expression(p);
    case tok_t::star:
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
// its mark.  A pointer and an integer convert both ways, a pointer
// being an unsigned long to the conversion, and a pointer to another
// pointer keeps the address, 6.3.2.3p7, and writes nothing: its node is
// the operand's value, which every row of rcc reads through such a
// cast.  A pointer constant converted to an integer is a cast in rcc,
// no constant, which rcc-v2 does not make yet.  A void operand is an
// error at the '(', require_value()'s, and a function designator is not
// yet.  A function type, which a typedef name may name, is no scalar
// type, p2, rcc's error at the '(' once the operand is read, and an enum
// in its own list is refuse_not_yet_type()'s there.
operand_t
cast(const token_t &open, const type_t &t, const operand_t &arg)
{
    if (is< function_t >(t))
        fail(open, "cast to a type that is not a scalar");

    refuse_not_yet_type(open, t);

    const operand_t value { arg.type, rvalue(open, arg) };

    if (is< pointer_t >(t) && is< pointer_t >(arg.type))
        return { .type = t, .node = value.node, .overflowed = arg.overflowed };

    if (is< pointer_t >(arg.type) && constant_of(*value.node))
        not_yet(open, "cast of a pointer constant to an integer");

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

// The operands of an operator of integers that are pointers, a function
// designator's value among them, 6.3.2.1p4, rcc's require_integer(): *
// / % << >> & | ^ take none, 6.5.6p2, 6.5.8p2, 6.5.11p2, 6.5.12p2,
// 6.5.13p2, an error at the operator.
void
require_integers(const token_t &at, const operand_t &lhs, const operand_t &rhs)
{
    if (points(lhs) || points(rhs))
        fail(at, "operand of " + quoted(at.text) + " is a pointer");
}

// A binary operator of integers, 6.5.6, 6.5.7, 6.5.11 to 6.5.13, its
// token at, rcc's arithmetic(): the usual arithmetic conversions, each
// operand converted to the common type, and the result of that type,
// the operation at its view and by its sign, 6.5.6p4, 6.5.7p5,
// 6.5.11p3, 6.5.12p3, 6.5.13p3.  rcc reads both values before it judges
// their types, so a void one is the error first, require_value()'s, and
// a pointer the next, require_integers()'s.
operand_t
arithmetic(binary_op_t op, const token_t &at, operand_t lhs, operand_t rhs)
{
    require_value(at, lhs);
    require_value(at, rhs);
    require_integers(at, lhs, rhs);

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
    require_integers(at, lhs, rhs);

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

// Pointer arithmetic needs a pointer to a complete object type, 6.5.7p2,
// p3, rcc's require_object_pointee(): no function, an error at the
// operator.
void
require_object_pointee(const token_t &at, const operand_t &pointer)
{
    if (is< function_t >(pointee(pointer)))
        fail(at, "arithmetic on a pointer to a function");
}

// +, 6.5.7, its token at, rcc's addition(): two integers are
// arithmetic()'s; with a pointer, a function designator's value among
// them, a pointer and an integer in either order, p2, two pointers an
// error at the operator, and the pointer's pointee an object type,
// require_object_pointee()'s.  The arithmetic is not yet.  rcc reads
// both values before it judges their types.
operand_t
addition(const token_t &at, operand_t lhs, operand_t rhs)
{
    require_value(at, lhs);
    require_value(at, rhs);
    if (!points(lhs) && !points(rhs))
        return arithmetic(binary_op_t::add, at, std::move(lhs), std::move(rhs));

    if (points(lhs) && points(rhs))
        fail(at, "cannot add two pointers");

    require_object_pointee(at, points(lhs) ? lhs : rhs);
    not_yet(at, "arithmetic on a pointer");
}

// -, 6.5.7, its token at, rcc's subtraction(): two integers are
// arithmetic()'s; with a pointer, a pointer less an integer, p3, or two
// pointers to compatible object types, each pointee an object type,
// require_object_pointee()'s, in rcc's order, a pointer to a function
// judged before the pointees; anything else an error at the operator.
// The arithmetic is not yet.
operand_t
subtraction(const token_t &at, operand_t lhs, operand_t rhs)
{
    require_value(at, lhs);
    require_value(at, rhs);
    if (!points(lhs) && !points(rhs))
        return arithmetic(binary_op_t::sub, at, std::move(lhs), std::move(rhs));

    if (!points(lhs))
        fail(at, "cannot subtract a pointer from an integer");

    require_object_pointee(at, lhs);
    if (points(rhs) && !compatible(pointee(lhs), pointee(rhs)))
        fail(at, "subtraction of pointers to different types");

    not_yet(at, "arithmetic on a pointer");
}

// The additive operators, 6.5.7, left-associative, as rcc's loop reads
// them: a - b - c is (a - b) - c.
operand_t
additive_expression(parser_t &p)
{
    operand_t e = multiplicative_expression(p);

    while (next_is(p, { tok_t::plus, tok_t::minus })) {
        const token_t &op = advance(p);

        operand_t rhs = multiplicative_expression(p);
        e = op.kind == tok_t::plus ?
                addition(op, std::move(e), std::move(rhs)) :
                subtraction(op, std::move(e), std::move(rhs));
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

// The type a comparison with a pointer brings its operands to, a
// function designator's value among them, 6.3.2.1p4, rcc's
// comparison() and common_pointer(): an equality operator compares two
// pointers to compatible types, and a pointer and a null pointer
// constant, which converts to the pointer's type, 6.5.10p2, p5; a
// relational operator two pointers to compatible object types, no
// function, 6.5.9p2; anything else is an error at the operator, in
// rcc's words, in its order.  Pointers compare as addresses (G7).  A
// function designator compared for equality, which rcc compiles, is not
// yet.  The pointees are int, an enumerated type compatible with int,
// and the pointers, so compatible ones are one type to the machine, the
// composite's, 6.2.7p3, and the operands convert to it as they are.
type_t
common_pointer(binary_op_t op,
               const token_t &at,
               const operand_t &lhs,
               const operand_t &rhs)
{
    const bool lp = points(lhs);
    const bool rp = points(rhs);

    if (op == binary_op_t::eq || op == binary_op_t::ne) {
        if (is< function_t >(lhs.type) || is< function_t >(rhs.type))
            not_yet(at, "comparison of a function designator");

        if (lp && rp && !compatible(pointee(lhs), pointee(rhs)))
            fail(at, "comparison of distinct pointer types");

        if (!(lp && rp) && !is_null_pointer_constant(lp ? rhs : lhs))
            fail(at, "comparison between pointer and integer");

        return lp ? lhs.type : rhs.type;
    }

    if (!(lp && rp))
        fail(at, "comparison between pointer and integer");

    if (is< function_t >(lhs.type))
        fail(at, "ordered comparison of pointers to functions");

    if (!compatible(pointee(lhs), pointee(rhs)))
        fail(at, "comparison of distinct pointer types");

    return lhs.type;
}

// A relational or an equality operator, 6.5.9, 6.5.10, its token at,
// rcc's comparison(): each operand converted to the operands' common
// type, 6.5.9p4, 6.5.10p5, or with a pointer to common_pointer()'s, the
// cmp at its view and by its sign, so -1 < 0u is an unsigned comparison
// and so is one of pointers, and the result an int whatever the
// operands' type, 6.5.9p7, 6.5.10p4.  Two pointer constants are
// refuse_pointer_constant()'s.  rcc reads both values before it judges
// their types, so a void one is the error first, require_value()'s.
operand_t
comparison(binary_op_t op, const token_t &at, operand_t lhs, operand_t rhs)
{
    require_value(at, lhs);
    require_value(at, rhs);

    const type_t t = points(lhs) || points(rhs) ?
                         common_pointer(op, at, lhs, rhs) :
                         common_type(lhs.type, rhs.type);
    expr_t l = convert_to(lhs, t);
    expr_t r = convert_to(rhs, t);
    if (constant_of(r))
        refuse_pointer_constant(at, t, l);

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
// 6.5.15p3, a pointer compared with null at 64 bits; a pointer
// constant beside a constant is refuse_pointer_constant()'s.  rcc reads
// both values before it judges their types: a void one is an error at
// the operator, and a function designator, a scalar, which rcc
// compiles, is not yet.
operand_t
logical(binary_op_t op, const token_t &at, operand_t lhs, operand_t rhs)
{
    require_value(at, lhs);
    require_value(at, rhs);

    expr_t l = rvalue(at, lhs);
    expr_t r = rvalue(at, rhs);
    if (constant_of(r))
        refuse_pointer_constant(at, lhs.type, l);

    if (constant_of(l))
        refuse_pointer_constant(at, rhs.type, r);

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

// The arms of a conditional, its '?' at, and the conditional, rcc's
// joined_arms() and conditional(): two void arms make a void
// conditional, 6.5.16p3, p6; one void arm and one of a value meet in
// none of p3's pairs, an error at the '?'; any other two are read for
// their values, each rvalue()'s at the '?', and each converted to the
// type of the usual arithmetic conversions of the two, p6, in one
// conversion, rcc's arithmetic_conversions(), so that two narrow arms
// make an int, and a constant arm is the constant of that type.  A
// constant condition and two constant arms fold to the arm the
// condition picks, p5, make_conditional()'s, marked overflowed where the
// condition or that arm was so reached.  A pointer arm, p3, is not yet.
operand_t
joined(const token_t &at,
       const operand_t &condition,
       operand_t yes,
       operand_t no)
{
    expr_t cond = *condition.node;

    if (is< void_t >(yes.type) && is< void_t >(no.type))
        return { yes.type,
                 make_conditional_void(std::move(cond), discarded(yes),
                                       discarded(no)) };

    if (is< void_t >(yes.type) || is< void_t >(no.type))
        fail(at, "void and non-void operands in conditional expression");

    const operand_t y { yes.type, rvalue(at, yes) };
    const operand_t n { no.type, rvalue(at, no) };
    if (points(y) || points(n))
        not_yet(at, "'?:' with a pointer arm");

    const type_t t = common_type(y.type, n.type);
    expr_t first = convert_to(y, t);
    expr_t second = convert_to(n, t);

    const auto c = constant_of(cond);
    const bool folded = c && constant_of(first) && constant_of(second);
    const bool marked =
        folded && (condition.overflowed || (*c != 0 ? yes : no).overflowed);

    return { .type = t,
             .node =
                 make_conditional(std::move(cond), std::move(first),
                                  std::move(second), view_of(t), sign_of(t)),
             .overflowed = marked };
}

} // namespace

// The conditional operator, 6.5.16, right-associative, as rcc reads it:
// the condition a logical OR expression, read for its value at the '?'
// before the arms, so that its error comes first, its value compared
// with 0, p2, a pointer constant refuse_pointer_constant()'s; the
// second operand an expression, a comma among them, and the third a
// conditional expression.  GNU C's a ?: b is not yet, at the ':'.
operand_t
conditional_expression(parser_t &p)
{
    operand_t e = logical_or_expression(p);
    if (peek(p).kind != tok_t::question)
        return e;

    const token_t &at = advance(p);
    const bool marked = e.overflowed;
    const operand_t condition { .type = e.type,
                                .node = rvalue(at, std::move(e)),
                                .overflowed = marked };
    refuse_pointer_constant(at, condition.type, *condition.node);
    if (peek(p).kind == tok_t::colon)
        not_yet(peek(p), "'?:' with no middle operand");

    operand_t yes = expression(p);
    expect(p, tok_t::colon, "expected ':' in conditional expression");
    operand_t no = conditional_expression(p);

    return joined(at, condition, std::move(yes), std::move(no));
}

namespace {

// The operator of a compound assignment's token, 6.5.17.3p1, a division
// and >> by their signed names, by_sign()'s, or none for a token that is
// no compound assignment's.
std::optional< binary_op_t >
compound_operator(tok_t kind)
{
    switch (kind) {
    case tok_t::mul_assign: return binary_op_t::mul;
    case tok_t::div_assign: return binary_op_t::sdiv;
    case tok_t::rem_assign: return binary_op_t::srem;
    case tok_t::add_assign: return binary_op_t::add;
    case tok_t::sub_assign: return binary_op_t::sub;
    case tok_t::shl_assign: return binary_op_t::shl;
    case tok_t::shr_assign: return binary_op_t::ashr;
    case tok_t::and_assign: return binary_op_t::bit_and;
    case tok_t::xor_assign: return binary_op_t::bit_xor;
    case tok_t::or_assign: return binary_op_t::bit_or;
    default: return std::nullopt;
    }
}

// E1 op= E2, 6.5.17.3, rcc's assignment(), its operator at op: the
// right operand's value read, a void one an error at the operator; the
// operation's type the common type of the two, 6.5.17.3p3, the usual
// arithmetic conversions, the right operand converted to it, or for <<=
// and >>= the left operand's promoted type, the count promoted on its
// own, 6.5.8p3; the result of the left operand's type, the operator by
// the operation's sign, at its view.  The read of the left operand into
// the operation's type and the result back to its own are no conversion
// of the tree, as in rcc: the node's rows extend the read and store the
// low bits.  A compound assignment is no constant expression, 6.6p3,
// and carries no mark of an overflow.
operand_t
compound_assignment(const token_t &op,
                    binary_op_t b,
                    operand_t lhs,
                    operand_t rhs)
{
    const operand_t value { rhs.type, rvalue(op, std::move(rhs)) };
    const type_t t =
        is_shift(b) ? promoted(lhs.type) : common_type(lhs.type, value.type);
    expr_t r = is_shift(b) ? promote(value) : convert_to(value, t);

    return { lhs.type, make_compound(by_sign(b, sign_of(t)), view_of(t),
                                     std::move(*lhs.node), std::move(r)) };
}

} // namespace

// The assignment operators, 6.5.17, right-associative: the simple one
// and the compound ones.  The left operand is a modifiable lvalue,
// 6.5.17.1p2, which rcc asks at the operator before it reads the right
// operand, and a function designator is none.  The right operand's
// value is converted to the simple assignment's type, 6.5.17.2p2, the
// left operand's, 6.5.17.1p3, assignment_conversion()'s at the
// operator.  A compound assignment of a pointer or by one is not yet, at
// its operator.
operand_t
assignment_expression(parser_t &p)
{
    operand_t lhs = conditional_expression(p);

    const auto compound = compound_operator(peek(p).kind);
    if (peek(p).kind != tok_t::assign && !compound)
        return lhs;

    const token_t &op = advance(p);
    if (!lhs.node || !is_assignable(*lhs.node))
        fail(op, "expression is not assignable");

    operand_t rhs = assignment_expression(p);
    if (compound && (points(lhs) || points(rhs)))
        not_yet(op, quoted(op.text) + " with a pointer operand");

    if (compound)
        return compound_assignment(op, *compound, std::move(lhs),
                                   std::move(rhs));

    expr_t value = assignment_conversion(op, std::move(rhs), lhs.type);

    return { lhs.type, make_assign(std::move(*lhs.node), std::move(value)) };
}

// The comma operator, 6.5.18, left-associative, as rcc's loop reads
// it: a, b, c is (a, b), c.  Each left operand is evaluated as a void
// expression, p2, discarded()'s, at the comma, before its right operand
// is read, so it may be void; the result is of the right operand's type
// and value, which may be void too and is converted as discarded()
// converts.  A comma is no constant expression, 6.6p3, so it carries no
// mark of an overflow.
operand_t
expression(parser_t &p)
{
    operand_t e = assignment_expression(p);

    while (accept(p, tok_t::comma)) {
        expr_t lhs = discarded(std::move(e));

        operand_t rhs = assignment_expression(p);
        const type_t t = rhs.type;
        e = { t, make_comma(std::move(lhs), discarded(std::move(rhs))) };
    }

    return e;
}

} // namespace rcc
