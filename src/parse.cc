// -*- mode: c++; -*-

// The parser: recursive descent over the token vector, one function
// per production of the C grammar, named after it.  Each expression is
// typed as it is built; the type rules of C live in the section of
// conversion functions below, and every conversion they make is a cast
// node in the tree.  Paragraph numbers are C23 (N3220).

#include "parse.hh"

#include <algorithm>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rcc {

namespace {

// The state of one parse.  Scope 0 is file scope; the scopes die with
// the parser and leave only the symbols in the unit.
struct parser_t
{
    const std::vector< token_t > &tokens;
    std::size_t pos;

    std::vector< std::unordered_map< std::string_view, const symbol_t * > >
        scopes;

    unit_t unit;
    type_t result;
};

// A parameter as written: its type, and its name token when it has one.
struct parameter_t
{
    type_t type;
    const token_t *start;
    const token_t *name;
};

// A declarator in the subset: a name, and a parameter list when it
// declares a function.
struct declarator_t
{
    const token_t *name;
    std::optional< std::vector< parameter_t > > params;
    bool variadic;
};

//
// diagnostics
//

[[noreturn]] void
fail(const token_t &at, std::string what)
{
    throw error_t { at.where, std::move(what) };
}

// A construct of C outside the subset this parser takes.
[[noreturn]] void
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

// The token ahead of the position; past the end, the eof that closes
// the vector.
const token_t &
peek(const parser_t &p, std::size_t ahead = 0)
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

// Consumes the token ahead and returns it; the eof is never consumed.
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

// The message is the whole diagnostic, said at the token found instead.
const token_t &
expect(parser_t &p, tok_t kind, std::string_view message)
{
    if (peek(p).kind != kind)
        fail(peek(p), std::string(message));

    return advance(p);
}

//
// types and conversions
//

type_t
int_type()
{
    return make_int(32, true);
}

// The integer promotions, 6.3.1.1p2: every type narrower than int
// converts to int, which holds all of its values, signed or not.
type_t
promoted(const type_t &t)
{
    return as< int_t >(t).width < 32 ? int_type() : t;
}

// The common type of the usual arithmetic conversions, 6.3.1.8p1, on
// the promoted operands.  The rank of a type is its width here: the
// tree keeps no spelling, and on LP64 long and long long are one type.
type_t
common_type(const type_t &a, const type_t &b)
{
    const int_t x = as< int_t >(promoted(a));
    const int_t y = as< int_t >(promoted(b));

    if (x.is_signed == y.is_signed)
        return make_int(std::max(x.width, y.width), x.is_signed);

    const int_t &u = x.is_signed ? y : x;
    const int_t &s = x.is_signed ? x : y;

    // a signed type wider than the unsigned one holds all its values
    if (u.width >= s.width)
        return make_int(u.width, false);

    return make_int(s.width, true);
}

expr_ptr_t
box(expr_t e)
{
    return std::make_unique< expr_t >(std::move(e));
}

stmt_ptr_t
box(stmt_t s)
{
    return std::make_unique< stmt_t >(std::move(s));
}

// The one place a conversion enters the tree.  An expression already
// of the type passes through, so no cast converts a type to itself.
expr_t
convert_to(expr_t e, const type_t &t)
{
    if (e.type == t)
        return e;

    return expr_t { t, cast_t { box(std::move(e)) } };
}

expr_t
promote(expr_t e)
{
    const type_t t = promoted(e.type);

    return convert_to(std::move(e), t);
}

// Converts both operands to their common type and returns it.  Each
// operand goes there in one cast: its promotion keeps every value, so
// promoting first and converting after would compute the same thing.
type_t
arithmetic_conversions(expr_t &lhs, expr_t &rhs)
{
    const type_t t = common_type(lhs.type, rhs.type);

    lhs = convert_to(std::move(lhs), t);
    rhs = convert_to(std::move(rhs), t);

    return t;
}

//
// operand checks
//

// Every value in the subset is an integer: the checks below are the
// constraints that remain once pointers are out, a void expression and
// a function designator where a value is wanted.
void
require_not_function(const token_t &at, const expr_t &e)
{
    if (is< function_t >(e.type))
        not_yet(at, "function " + quoted(as< name_t >(e).sym->name) +
                        " used as a value");
}

void
require_value(const token_t &at, const expr_t &e)
{
    require_not_function(at, e);

    if (is< void_t >(e.type))
        fail(at, "void expression used as a value");
}

// 6.5.17.1p2, 6.5.3.5p1, 6.5.4.1p1: a name of an integer object; a
// function name is an lvalue to is_lvalue, but not a modifiable one.
void
require_modifiable_lvalue(const token_t &at, const expr_t &e)
{
    if (!is_lvalue(e) || !is< int_t >(e.type))
        fail(at, "expression is not assignable");
}

//
// typed operators
//

binary_op_t
binary_operator(tok_t kind)
{
    switch (kind) {
    case tok_t::star:
    case tok_t::mul_assign: return binary_op_t::mul;
    case tok_t::slash:
    case tok_t::div_assign: return binary_op_t::div;
    case tok_t::percent:
    case tok_t::rem_assign: return binary_op_t::rem;
    case tok_t::plus:
    case tok_t::add_assign: return binary_op_t::add;
    case tok_t::minus:
    case tok_t::sub_assign: return binary_op_t::sub;
    case tok_t::shl:
    case tok_t::shl_assign: return binary_op_t::shl;
    case tok_t::shr:
    case tok_t::shr_assign: return binary_op_t::shr;
    case tok_t::amp:
    case tok_t::and_assign: return binary_op_t::bit_and;
    case tok_t::caret:
    case tok_t::xor_assign: return binary_op_t::bit_xor;
    case tok_t::pipe:
    case tok_t::or_assign: return binary_op_t::bit_or;
    case tok_t::lt: return binary_op_t::lt;
    case tok_t::gt: return binary_op_t::gt;
    case tok_t::le: return binary_op_t::le;
    case tok_t::ge: return binary_op_t::ge;
    case tok_t::eq: return binary_op_t::eq;
    case tok_t::ne: return binary_op_t::ne;
    case tok_t::log_and: return binary_op_t::log_and;
    case tok_t::log_or: return binary_op_t::log_or;
    default: return binary_op_t::comma;
    }
}

bool
is_assignment_operator(tok_t kind)
{
    switch (kind) {
    case tok_t::assign:
    case tok_t::mul_assign:
    case tok_t::div_assign:
    case tok_t::rem_assign:
    case tok_t::add_assign:
    case tok_t::sub_assign:
    case tok_t::shl_assign:
    case tok_t::shr_assign:
    case tok_t::and_assign:
    case tok_t::xor_assign:
    case tok_t::or_assign: return true;
    default: return false;
    }
}

expr_t
make_binary(binary_op_t op, type_t t, expr_t lhs, expr_t rhs)
{
    return expr_t { std::move(t),
                    binary_t { op, box(std::move(lhs)), box(std::move(rhs)) } };
}

// * / % + - & ^ |: the usual arithmetic conversions, and the result in
// the common type (6.5.6p4, 6.5.7p5, 6.5.11p3, 6.5.12p3, 6.5.13p3).
expr_t
arithmetic(const token_t &op, expr_t lhs, expr_t rhs)
{
    require_value(op, lhs);
    require_value(op, rhs);

    const type_t t = arithmetic_conversions(lhs, rhs);

    return make_binary(binary_operator(op.kind), t, std::move(lhs),
                       std::move(rhs));
}

// < > <= >= == !=: the operands in their common type, the result an
// int (6.5.9p4, p7; 6.5.10p5).
expr_t
comparison(const token_t &op, expr_t lhs, expr_t rhs)
{
    require_value(op, lhs);
    require_value(op, rhs);

    arithmetic_conversions(lhs, rhs);

    return make_binary(binary_operator(op.kind), int_type(), std::move(lhs),
                       std::move(rhs));
}

// && ||: an int (6.5.14p3, 6.5.15p3).  The operands stay as they are;
// the emitter compares each against zero in its own type.
expr_t
logical(const token_t &op, expr_t lhs, expr_t rhs)
{
    require_value(op, lhs);
    require_value(op, rhs);

    return make_binary(binary_operator(op.kind), int_type(), std::move(lhs),
                       std::move(rhs));
}

// << >>: the type of the promoted left operand, 6.5.8p3.  The count is
// promoted on its own there, and then goes to the width of the shifted
// operand as well, because the emitter converts nothing and an IR shift
// takes one type.  The narrowing loses nothing G3 keeps: the count is
// taken modulo the width, which divides 2^32.
expr_t
shift(const token_t &op, expr_t lhs, expr_t rhs)
{
    require_value(op, lhs);
    require_value(op, rhs);

    lhs = promote(std::move(lhs));
    const type_t t = lhs.type;
    rhs = convert_to(std::move(rhs), t);

    return make_binary(binary_operator(op.kind), t, std::move(lhs),
                       std::move(rhs));
}

//
// scopes
//

const symbol_t *
lookup(const parser_t &p, std::string_view name)
{
    for (auto scope = p.scopes.rbegin(); scope != p.scopes.rend(); ++scope)
        if (const auto it = scope->find(name); it != scope->end())
            return it->second;

    return nullptr;
}

// A new object in the innermost scope.  Without linkage a name is
// declared once per scope (6.7.1p4); an inner scope may shadow it.  The
// map's key views the symbol's own name, which the deque keeps in
// place.
const symbol_t *
declare(parser_t &p, const token_t &name, type_t t, storage_t storage)
{
    auto &scope = p.scopes.back();
    if (scope.contains(name.text))
        fail(name, "redefinition of " + quoted(name.text));

    const symbol_t &sym = p.unit.symbols.emplace_back(
        symbol_t { std::string(name.text), std::move(t), storage });
    scope.emplace(sym.name, &sym);

    return &sym;
}

//
// integer constants
//

int
digit_value(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;

    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    return -1;
}

struct suffix_t
{
    bool is_unsigned;
    int longs;
};

// u or U and l, L, ll or LL in either order, 6.4.4.2p1; a mixed-case
// lL is not a suffix.
suffix_t
integer_suffix(const token_t &t, std::string_view s)
{
    suffix_t suffix { false, 0 };

    const auto take_unsigned = [&] {
        if (!s.empty() && (s[0] == 'u' || s[0] == 'U')) {
            suffix.is_unsigned = true;
            s.remove_prefix(1);
        }
    };

    const auto take_long = [&] {
        if (s.starts_with("ll") || s.starts_with("LL")) {
            suffix.longs = 2;
            s.remove_prefix(2);
        }
        else if (!s.empty() && (s[0] == 'l' || s[0] == 'L')) {
            suffix.longs = 1;
            s.remove_prefix(1);
        }
    };

    take_unsigned();
    take_long();
    if (!suffix.is_unsigned)
        take_unsigned();

    if (s.find_first_of("wW") != std::string_view::npos)
        not_yet(t, "bit-precise integer constant");

    if (!s.empty())
        fail(t, "invalid suffix " + quoted(s) + " on integer constant");

    return suffix;
}

// The type of a constant is the first of its list in 6.4.4.2p6 that
// holds the value.  A decimal constant without u stays signed; an
// octal or hexadecimal one may go unsigned.  long long is long on
// LP64, so the lists end at the 64-bit types.
std::optional< type_t >
integer_constant_type(std::uint64_t value, bool decimal, suffix_t suffix)
{
    const bool may_be_signed = !suffix.is_unsigned;
    const bool may_be_unsigned = suffix.is_unsigned || !decimal;

    if (may_be_signed && suffix.longs == 0 &&
        value <= std::uint64_t(std::numeric_limits< std::int32_t >::max()))
        return make_int(32, true);

    if (may_be_unsigned && suffix.longs == 0 &&
        value <= std::numeric_limits< std::uint32_t >::max())
        return make_int(32, false);

    if (may_be_signed &&
        value <= std::uint64_t(std::numeric_limits< std::int64_t >::max()))
        return make_int(64, true);

    if (may_be_unsigned)
        return make_int(64, false);

    return std::nullopt;
}

// The value from the spelling, 6.4.4.2p4, p5.  The lexer has checked
// the form; a value past 64 bits has no type (6.4.4.2p7).
expr_t
integer_constant(const token_t &t)
{
    std::string_view s = t.text;

    int base = 10;
    if (s.size() > 1 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        base = 16;
        s.remove_prefix(2);
    }
    else if (s.size() > 1 && s[0] == '0' && (s[1] == 'b' || s[1] == 'B'))
        not_yet(t, "binary constant");
    else if (s[0] == '0')
        base = 8;

    std::uint64_t value = 0;
    bool too_large = false;

    std::size_t i = 0;
    for (; i < s.size(); ++i) {
        const int digit = digit_value(s[i]);
        if (digit < 0 || digit >= base)
            break;

        if (value > (std::numeric_limits< std::uint64_t >::max() - digit) /
                        std::uint64_t(base))
            too_large = true;

        value = value * base + digit;
    }

    if (i < s.size() && s[i] == '\'')
        not_yet(t, "digit separator");

    const suffix_t suffix = integer_suffix(t, s.substr(i));

    const auto type = integer_constant_type(value, base == 10, suffix);
    if (too_large || !type)
        fail(t, "integer constant is too large for its type");

    return expr_t { *type, literal_t { static_cast< long long >(value) } };
}

//
// the productions
//

void external_declaration(parser_t &);
void function_definition(parser_t &, const type_t &, const declarator_t &);
type_t declaration_specifiers(parser_t &);
declarator_t declarator(parser_t &);
void parameter_type_list(parser_t &, declarator_t &);
parameter_t parameter_declaration(parser_t &);
std::vector< stmt_t > declaration(parser_t &);
stmt_t init_declarator(parser_t &, const type_t &);

stmt_t statement(parser_t &);
block_t compound_statement(parser_t &);
void block_item_list(parser_t &, block_t &);
stmt_t expression_statement(parser_t &);
stmt_t selection_statement(parser_t &);
stmt_t iteration_statement(parser_t &);
stmt_t while_statement(parser_t &);
stmt_t for_statement(parser_t &);
stmt_t jump_statement(parser_t &);

expr_t expression(parser_t &);
expr_t assignment_expression(parser_t &);
expr_t conditional_expression(parser_t &);
expr_t logical_or_expression(parser_t &);
expr_t logical_and_expression(parser_t &);
expr_t inclusive_or_expression(parser_t &);
expr_t exclusive_or_expression(parser_t &);
expr_t and_expression(parser_t &);
expr_t equality_expression(parser_t &);
expr_t relational_expression(parser_t &);
expr_t shift_expression(parser_t &);
expr_t additive_expression(parser_t &);
expr_t multiplicative_expression(parser_t &);
expr_t cast_expression(parser_t &);
expr_t unary_expression(parser_t &);
expr_t postfix_expression(parser_t &);
std::vector< expr_t > argument_expression_list(parser_t &, const symbol_t &);
expr_t primary_expression(parser_t &);

//
// declarations
//

// The keywords a declaration can start with, those outside the subset
// included, so that they are refused by name and not as an expression.
bool
is_declaration_start(tok_t kind)
{
    switch (kind) {
    case tok_t::kw_void:
    case tok_t::kw_char:
    case tok_t::kw_short:
    case tok_t::kw_int:
    case tok_t::kw_long:
    case tok_t::kw_signed:
    case tok_t::kw_unsigned:
    case tok_t::kw_float:
    case tok_t::kw_double:
    case tok_t::kw_const:
    case tok_t::kw_volatile:
    case tok_t::kw_restrict:
    case tok_t::kw_auto:
    case tok_t::kw_register:
    case tok_t::kw_static:
    case tok_t::kw_extern:
    case tok_t::kw_typedef:
    case tok_t::kw_struct:
    case tok_t::kw_union:
    case tok_t::kw_enum: return true;
    default: return false;
    }
}

void
translation_unit(parser_t &p)
{
    while (peek(p).kind != tok_t::eof)
        external_declaration(p);
}

// The function type a declarator names, with the base type as its
// result.
type_t
function_type(const type_t &result, const declarator_t &d)
{
    std::vector< type_t > params;
    for (const parameter_t &param : *d.params)
        params.push_back(param.type);

    return make_function(result, std::move(params), d.variadic);
}

// A prototype's parameters share one scope, 6.7.1p4, even when no
// body follows to use the names.
void
check_parameter_names(const declarator_t &d)
{
    std::unordered_map< std::string_view, bool > seen;

    for (const parameter_t &param : *d.params)
        if (param.name && !seen.emplace(param.name->text, true).second)
            fail(*param.name,
                 "redefinition of parameter " + quoted(param.name->text));
}

// Declares a function at file scope, or meets its earlier declaration,
// which must name the same type (6.2.7p2).  Returns the index of its
// entry in the unit's functions.
std::size_t
declare_function(parser_t &p, const type_t &result, const declarator_t &d)
{
    check_parameter_names(d);

    const type_t t = function_type(result, d);
    const std::string_view name = d.name->text;

    auto &file_scope = p.scopes.front();
    if (const auto it = file_scope.find(name); it != file_scope.end()) {
        if (!(it->second->type == t))
            fail(*d.name, "conflicting types for " + quoted(name));

        for (std::size_t i = 0; i < p.unit.functions.size(); ++i)
            if (p.unit.functions[i].sym == it->second)
                return i;
    }

    const symbol_t &sym = p.unit.symbols.emplace_back(
        symbol_t { std::string(name), t, storage_t::function });
    file_scope.emplace(sym.name, &sym);
    p.unit.functions.push_back(function_decl_t { &sym, { }, std::nullopt });

    return p.unit.functions.size() - 1;
}

// A function definition, or a declaration of one or more functions.
// File-scope objects are outside the subset.
void
external_declaration(parser_t &p)
{
    const type_t base = declaration_specifiers(p);

    declarator_t d = declarator(p);
    if (!d.params)
        not_yet(*d.name, "global variable");

    if (peek(p).kind == tok_t::lbrace) {
        function_definition(p, base, d);
        return;
    }

    declare_function(p, base, d);

    while (accept(p, tok_t::comma)) {
        d = declarator(p);
        if (!d.params)
            not_yet(*d.name, "global variable");

        declare_function(p, base, d);
    }

    expect(p, tok_t::semicolon, "expected ';' after top level declarator");
}

// The parameters and the outermost block of the body are one block,
// 6.8.3p2, so a local cannot redeclare a parameter there; the body's
// items are parsed into the parameter scope directly.
void
function_definition(parser_t &p, const type_t &result, const declarator_t &d)
{
    const std::size_t index = declare_function(p, result, d);
    if (p.unit.functions[index].body)
        fail(*d.name, "redefinition of " + quoted(d.name->text));

    p.result = result;
    p.scopes.emplace_back();

    std::vector< const symbol_t * > params;
    for (const parameter_t &param : *d.params) {
        if (!param.name)
            fail(*param.start, "parameter name omitted");

        params.push_back(
            declare(p, *param.name, param.type, storage_t::parameter));
    }

    block_t body;
    expect(p, tok_t::lbrace, "expected '{' before function body");
    block_item_list(p, body);
    expect(p, tok_t::rbrace, "expected '}' at end of function body");

    p.scopes.pop_back();

    function_decl_t &f = p.unit.functions[index];
    f.params = std::move(params);
    f.body = std::move(body);
}

// The type specifiers in any valid order and combination, 6.7.3.1p2.
// The tree keeps no spelling: char and signed char are one type, long
// and long long another.  Each keyword is checked against the ones
// before it, so an error names the keyword that breaks the set.
type_t
declaration_specifiers(parser_t &p)
{
    int n_void = 0, n_char = 0, n_short = 0, n_int = 0, n_long = 0;
    int n_signed = 0, n_unsigned = 0;

    const token_t &start = peek(p);

    for (;;) {
        const token_t &t = peek(p);

        bool bad = n_void > 0;
        switch (t.kind) {
        case tok_t::kw_void:
            bad = bad || n_char || n_short || n_int || n_long || n_signed ||
                  n_unsigned;
            ++n_void;
            break;
        case tok_t::kw_char:
            bad = bad || n_char || n_short || n_int || n_long;
            ++n_char;
            break;
        case tok_t::kw_short:
            bad = bad || n_char || n_short || n_long;
            ++n_short;
            break;
        case tok_t::kw_int:
            bad = bad || n_char || n_int;
            ++n_int;
            break;
        case tok_t::kw_long:
            bad = bad || n_char || n_short || n_long == 2;
            ++n_long;
            break;
        case tok_t::kw_signed:
        case tok_t::kw_unsigned:
            bad = bad || n_signed || n_unsigned;
            ++(t.kind == tok_t::kw_signed ? n_signed : n_unsigned);
            break;
        case tok_t::kw_const:
        case tok_t::kw_volatile:
        case tok_t::kw_restrict: not_yet(t, "type qualifier " + quoted(t.text));
        case tok_t::kw_auto:
        case tok_t::kw_register:
        case tok_t::kw_static:
        case tok_t::kw_extern:
        case tok_t::kw_typedef: not_yet(t, "storage class " + quoted(t.text));
        case tok_t::kw_struct:
        case tok_t::kw_union:
        case tok_t::kw_enum: not_yet(t, quoted(t.text));
        case tok_t::kw_float:
        case tok_t::kw_double: not_yet(t, "floating type " + quoted(t.text));
        default:
            if (&t == &start)
                fail(t, "expected a type specifier");

            if (n_void)
                return make_void();

            return make_int(n_char  ? 8 :
                            n_short ? 16 :
                            n_long  ? 64 :
                                      32,
                            n_unsigned == 0);
        }

        if (bad)
            fail(t, "cannot combine " + quoted(t.text) +
                        " with the type specifiers before it");

        advance(p);
    }
}

// An identifier, and a parameter list after it for a function.
declarator_t
declarator(parser_t &p)
{
    if (peek(p).kind == tok_t::star)
        not_yet(peek(p), "pointer declarator");

    if (peek(p).kind == tok_t::lparen)
        not_yet(peek(p), "parenthesized declarator");

    const token_t &name = expect(p, tok_t::identifier, "expected identifier");
    declarator_t d { &name, std::nullopt, false };

    if (peek(p).kind == tok_t::lbracket)
        not_yet(peek(p), "array declarator");

    if (accept(p, tok_t::lparen))
        parameter_type_list(p, d);

    return d;
}

// (void), (), or parameters with an optional trailing ", ...".  An
// empty list is (void) in C23, 6.7.7.4p13; before C23 it declared a
// function without a prototype, which this parser does not take.
void
parameter_type_list(parser_t &p, declarator_t &d)
{
    d.params.emplace();

    if (accept(p, tok_t::rparen))
        return;

    if (peek(p).kind == tok_t::kw_void && peek(p, 1).kind == tok_t::rparen) {
        advance(p);
        advance(p);
        return;
    }

    do {
        if (peek(p).kind == tok_t::ellipsis) {
            if (d.params->empty())
                not_yet(peek(p), "variadic function without named parameters");

            advance(p);
            d.variadic = true;
            break;
        }

        d.params->push_back(parameter_declaration(p));
    } while (accept(p, tok_t::comma));

    expect(p, tok_t::rparen, "expected ')' after parameter list");
}

// Specifiers and an optional name.  void stands only alone, as (void),
// 6.7.7.4p9.
parameter_t
parameter_declaration(parser_t &p)
{
    const token_t &start = peek(p);

    const type_t t = declaration_specifiers(p);
    if (is< void_t >(t))
        fail(start, "'void' must be the only parameter");

    if (peek(p).kind == tok_t::star)
        not_yet(peek(p), "pointer declarator");

    if (peek(p).kind == tok_t::lparen)
        not_yet(peek(p), "parenthesized declarator");

    const token_t *name = nullptr;
    if (peek(p).kind == tok_t::identifier)
        name = &advance(p);

    if (peek(p).kind == tok_t::lbracket)
        not_yet(peek(p), "array declarator");

    return parameter_t { t, &start, name };
}

// A block-scope declaration: one declaration statement per declarator,
// in order.
std::vector< stmt_t >
declaration(parser_t &p)
{
    const type_t base = declaration_specifiers(p);

    std::vector< stmt_t > decls;
    do
        decls.push_back(init_declarator(p, base));
    while (accept(p, tok_t::comma));

    expect(p, tok_t::semicolon, "expected ';' at end of declaration");

    return decls;
}

// The name is in scope from the end of its declarator, 6.2.1p7, so an
// initializer sees the object it initializes.  The initializer is
// converted as by assignment, 6.7.11p12.
stmt_t
init_declarator(parser_t &p, const type_t &base)
{
    const declarator_t d = declarator(p);
    if (d.params)
        not_yet(*d.name, "function declaration at block scope");

    if (is< void_t >(base))
        fail(*d.name, "variable " + quoted(d.name->text) + " has type void");

    decl_stmt_t decl { declare(p, *d.name, base, storage_t::local),
                       std::nullopt };

    if (peek(p).kind == tok_t::assign) {
        const token_t &op = advance(p);

        expr_t init = assignment_expression(p);
        require_value(op, init);

        decl.init = convert_to(std::move(init), base);
    }

    return stmt_t { std::move(decl) };
}

//
// statements
//

// A declaration is a block item, not a statement: it cannot be the
// body of an if, a while or a for.
stmt_t
statement(parser_t &p)
{
    const token_t &t = peek(p);

    switch (t.kind) {
    case tok_t::lbrace: return stmt_t { compound_statement(p) };
    case tok_t::kw_if:
    case tok_t::kw_switch: return selection_statement(p);
    case tok_t::kw_while:
    case tok_t::kw_do:
    case tok_t::kw_for: return iteration_statement(p);
    case tok_t::kw_return:
    case tok_t::kw_break:
    case tok_t::kw_continue:
    case tok_t::kw_goto: return jump_statement(p);
    case tok_t::kw_case: not_yet(t, "case label");
    case tok_t::kw_default: not_yet(t, "default label");
    default: break;
    }

    if (t.kind == tok_t::identifier && peek(p, 1).kind == tok_t::colon)
        not_yet(t, "label");

    if (is_declaration_start(t.kind))
        fail(t, "expected statement, found a declaration");

    return expression_statement(p);
}

// A new scope for the block's declarations.
block_t
compound_statement(parser_t &p)
{
    block_t block;

    expect(p, tok_t::lbrace, "expected '{'");
    p.scopes.emplace_back();
    block_item_list(p, block);
    p.scopes.pop_back();
    expect(p, tok_t::rbrace, "expected '}' at end of block");

    return block;
}

// The items up to the closing brace, which is left for the caller.
void
block_item_list(parser_t &p, block_t &block)
{
    while (!next_is(p, { tok_t::rbrace, tok_t::eof })) {
        if (is_declaration_start(peek(p).kind)) {
            for (stmt_t &decl : declaration(p))
                block.body.push_back(std::move(decl));
        }
        else
            block.body.push_back(statement(p));
    }
}

// An expression whose value is discarded may be void; an empty one is
// the null statement.
stmt_t
expression_statement(parser_t &p)
{
    if (accept(p, tok_t::semicolon))
        return stmt_t { expr_stmt_t { } };

    const token_t &start = peek(p);

    expr_t e = expression(p);
    require_not_function(start, e);

    expect(p, tok_t::semicolon, "expected ';' after expression");

    return stmt_t { expr_stmt_t { std::move(e) } };
}

// The condition stays in its own type; the emitter compares it against
// zero, 6.8.5.2p2.
stmt_t
selection_statement(parser_t &p)
{
    const token_t &keyword = advance(p);
    if (keyword.kind == tok_t::kw_switch)
        not_yet(keyword, "switch statement");

    expect(p, tok_t::lparen, "expected '(' after 'if'");

    const token_t &start = peek(p);
    expr_t cond = expression(p);
    require_value(start, cond);

    expect(p, tok_t::rparen, "expected ')' after condition");

    stmt_ptr_t yes = box(statement(p));

    stmt_ptr_t no;
    if (accept(p, tok_t::kw_else))
        no = box(statement(p));

    return stmt_t { if_t { std::move(cond), std::move(yes), std::move(no) } };
}

stmt_t
iteration_statement(parser_t &p)
{
    switch (peek(p).kind) {
    case tok_t::kw_while: return while_statement(p);
    case tok_t::kw_for: return for_statement(p);
    default: not_yet(peek(p), "do statement");
    }
}

stmt_t
while_statement(parser_t &p)
{
    advance(p);
    expect(p, tok_t::lparen, "expected '(' after 'while'");

    const token_t &start = peek(p);
    expr_t cond = expression(p);
    require_value(start, cond);

    expect(p, tok_t::rparen, "expected ')' after condition");

    stmt_ptr_t body = box(statement(p));

    return stmt_t { while_t { std::move(cond), std::move(body) } };
}

// A declaration in the init is in scope for the whole loop, 6.8.6.4p1,
// so the for opens a scope; a braced body nests inside it.  The init
// and the step are evaluated as void expressions there.  Several
// declarators in the init become one block of declaration statements,
// since the tree holds a single init statement.
stmt_t
for_statement(parser_t &p)
{
    advance(p);
    expect(p, tok_t::lparen, "expected '(' after 'for'");

    p.scopes.emplace_back();

    for_t loop;

    if (is_declaration_start(peek(p).kind)) {
        std::vector< stmt_t > decls = declaration(p);
        if (decls.size() == 1)
            loop.init = box(std::move(decls.front()));
        else
            loop.init = box(stmt_t { block_t { std::move(decls) } });
    }
    else if (!accept(p, tok_t::semicolon))
        loop.init = box(expression_statement(p));

    if (peek(p).kind != tok_t::semicolon) {
        const token_t &start = peek(p);
        loop.cond = expression(p);
        require_value(start, *loop.cond);
    }

    expect(p, tok_t::semicolon, "expected ';' after for condition");

    if (peek(p).kind != tok_t::rparen) {
        const token_t &start = peek(p);
        loop.step = expression(p);
        require_not_function(start, *loop.step);
    }

    expect(p, tok_t::rparen, "expected ')' after for clauses");

    loop.body = box(statement(p));

    p.scopes.pop_back();

    return stmt_t { std::move(loop) };
}

// A value is converted to the result type as if by assignment,
// 6.8.7.5p3; a void function takes no value and a non-void one
// requires one, 6.8.7.5p1.
stmt_t
jump_statement(parser_t &p)
{
    const token_t &keyword = advance(p);
    if (keyword.kind != tok_t::kw_return)
        not_yet(keyword, quoted(keyword.text) + " statement");

    return_t ret;

    if (peek(p).kind != tok_t::semicolon) {
        const token_t &start = peek(p);

        expr_t value = expression(p);
        if (is< void_t >(p.result))
            fail(keyword, "void function should not return a value");

        require_value(start, value);
        ret.value = convert_to(std::move(value), p.result);
    }
    else if (!is< void_t >(p.result))
        fail(keyword, "non-void function should return a value");

    expect(p, tok_t::semicolon, "expected ';' after return statement");

    return stmt_t { std::move(ret) };
}

//
// expressions, from the comma down
//

// The left operand is evaluated for its effect and may be void, 6.5.18p2;
// the result is the right operand's type and value.  A function name
// is refused on either side, which keeps every function-typed
// expression a name_t.
expr_t
expression(parser_t &p)
{
    expr_t e = assignment_expression(p);

    while (peek(p).kind == tok_t::comma) {
        const token_t &op = advance(p);
        require_not_function(op, e);

        expr_t rhs = assignment_expression(p);
        require_not_function(op, rhs);

        const type_t t = rhs.type;
        e = make_binary(binary_op_t::comma, t, std::move(e), std::move(rhs));
    }

    return e;
}

// The type of the whole is the type of the left operand, 6.5.17.1p3,
// 6.5.17.2p2, and a plain assignment converts the right operand to it.
// E1 op= E2 is E1 = E1 op (E2) with E1 evaluated once, 6.5.17.3p4: the
// operation is computed in the common type of the usual arithmetic
// conversions, or in the promoted left type for a shift, 6.5.8p3, so
// the right operand goes to that type here, a shift count included, as
// in the shift production.  The emitter takes the loaded left operand
// into it and the result back to the left type.
expr_t
assignment_expression(parser_t &p)
{
    expr_t lhs = conditional_expression(p);
    if (!is_assignment_operator(peek(p).kind))
        return lhs;

    const token_t &op = advance(p);
    require_modifiable_lvalue(op, lhs);

    expr_t rhs = assignment_expression(p);
    require_value(op, rhs);

    const type_t t = lhs.type;

    std::optional< binary_op_t > compound;
    type_t common = t;
    if (op.kind != tok_t::assign) {
        compound = binary_operator(op.kind);

        if (compound == binary_op_t::shl || compound == binary_op_t::shr)
            common = promoted(t);
        else
            common = common_type(t, rhs.type);
    }

    rhs = convert_to(std::move(rhs), common);

    return expr_t { t, assign_t { compound, common, box(std::move(lhs)),
                                  box(std::move(rhs)) } };
}

// The condition stays as it is; the arms go to their common type,
// 6.5.16p6.  Two void arms are valid C, outside the subset for now.
expr_t
conditional_expression(parser_t &p)
{
    expr_t cond = logical_or_expression(p);
    if (peek(p).kind != tok_t::question)
        return cond;

    const token_t &op = advance(p);
    require_value(op, cond);

    expr_t yes = expression(p);
    expect(p, tok_t::colon, "expected ':' in conditional expression");
    expr_t no = conditional_expression(p);

    if (is< void_t >(yes.type) && is< void_t >(no.type))
        not_yet(op, "conditional with void operands");

    require_value(op, yes);
    require_value(op, no);

    const type_t t = arithmetic_conversions(yes, no);

    return expr_t { t,
                    conditional_t { box(std::move(cond)), box(std::move(yes)),
                                    box(std::move(no)) } };
}

expr_t
logical_or_expression(parser_t &p)
{
    expr_t e = logical_and_expression(p);

    while (peek(p).kind == tok_t::log_or) {
        const token_t &op = advance(p);
        e = logical(op, std::move(e), logical_and_expression(p));
    }

    return e;
}

expr_t
logical_and_expression(parser_t &p)
{
    expr_t e = inclusive_or_expression(p);

    while (peek(p).kind == tok_t::log_and) {
        const token_t &op = advance(p);
        e = logical(op, std::move(e), inclusive_or_expression(p));
    }

    return e;
}

expr_t
inclusive_or_expression(parser_t &p)
{
    expr_t e = exclusive_or_expression(p);

    while (peek(p).kind == tok_t::pipe) {
        const token_t &op = advance(p);
        e = arithmetic(op, std::move(e), exclusive_or_expression(p));
    }

    return e;
}

expr_t
exclusive_or_expression(parser_t &p)
{
    expr_t e = and_expression(p);

    while (peek(p).kind == tok_t::caret) {
        const token_t &op = advance(p);
        e = arithmetic(op, std::move(e), and_expression(p));
    }

    return e;
}

expr_t
and_expression(parser_t &p)
{
    expr_t e = equality_expression(p);

    while (peek(p).kind == tok_t::amp) {
        const token_t &op = advance(p);
        e = arithmetic(op, std::move(e), equality_expression(p));
    }

    return e;
}

expr_t
equality_expression(parser_t &p)
{
    expr_t e = relational_expression(p);

    while (next_is(p, { tok_t::eq, tok_t::ne })) {
        const token_t &op = advance(p);
        e = comparison(op, std::move(e), relational_expression(p));
    }

    return e;
}

expr_t
relational_expression(parser_t &p)
{
    expr_t e = shift_expression(p);

    while (next_is(p, { tok_t::lt, tok_t::gt, tok_t::le, tok_t::ge })) {
        const token_t &op = advance(p);
        e = comparison(op, std::move(e), shift_expression(p));
    }

    return e;
}

expr_t
shift_expression(parser_t &p)
{
    expr_t e = additive_expression(p);

    while (next_is(p, { tok_t::shl, tok_t::shr })) {
        const token_t &op = advance(p);
        e = shift(op, std::move(e), additive_expression(p));
    }

    return e;
}

expr_t
additive_expression(parser_t &p)
{
    expr_t e = multiplicative_expression(p);

    while (next_is(p, { tok_t::plus, tok_t::minus })) {
        const token_t &op = advance(p);
        e = arithmetic(op, std::move(e), multiplicative_expression(p));
    }

    return e;
}

expr_t
multiplicative_expression(parser_t &p)
{
    expr_t e = cast_expression(p);

    while (next_is(p, { tok_t::star, tok_t::slash, tok_t::percent })) {
        const token_t &op = advance(p);
        e = arithmetic(op, std::move(e), cast_expression(p));
    }

    return e;
}

// A parenthesized type name starts a cast; anything else in
// parentheses is a primary expression.
expr_t
cast_expression(parser_t &p)
{
    if (peek(p).kind == tok_t::lparen && is_declaration_start(peek(p, 1).kind))
        not_yet(peek(p), "cast");

    return unary_expression(p);
}

// - and ~ promote their operand and have its promoted type, 6.5.4.3p3,
// p4; ! is an int, its operand left as it is, p5.  Prefix ++ and --
// have the type of their lvalue operand, 6.5.4.1.
expr_t
unary_expression(parser_t &p)
{
    const token_t &op = peek(p);

    switch (op.kind) {
    case tok_t::inc:
    case tok_t::dec: {
        advance(p);

        expr_t arg = unary_expression(p);
        require_modifiable_lvalue(op, arg);

        const type_t t = arg.type;
        const unary_op_t u =
            op.kind == tok_t::inc ? unary_op_t::pre_inc : unary_op_t::pre_dec;

        return expr_t { t, unary_t { u, box(std::move(arg)) } };
    }
    case tok_t::minus:
    case tok_t::tilde: {
        advance(p);

        expr_t arg = cast_expression(p);
        require_value(op, arg);

        arg = promote(std::move(arg));
        const type_t t = arg.type;
        const unary_op_t u =
            op.kind == tok_t::minus ? unary_op_t::neg : unary_op_t::bit_not;

        return expr_t { t, unary_t { u, box(std::move(arg)) } };
    }
    case tok_t::bang: {
        advance(p);

        expr_t arg = cast_expression(p);
        require_value(op, arg);

        return expr_t { int_type(),
                        unary_t { unary_op_t::log_not, box(std::move(arg)) } };
    }
    case tok_t::plus: not_yet(op, "unary +");
    case tok_t::amp: not_yet(op, "address-of operator");
    case tok_t::star: not_yet(op, "indirection operator");
    case tok_t::kw_sizeof: not_yet(op, "sizeof");
    default: return postfix_expression(p);
    }
}

// A call names its function directly, 6.5.3.3p5 gives its type; postfix
// ++ and -- have the type of their lvalue operand, 6.5.3.5.
expr_t
postfix_expression(parser_t &p)
{
    expr_t e = primary_expression(p);

    for (;;) {
        const token_t &op = peek(p);

        switch (op.kind) {
        case tok_t::lparen: {
            advance(p);

            if (!is< name_t >(e))
                not_yet(op, "call through an expression");

            const symbol_t &fn = *as< name_t >(e).sym;
            if (fn.storage != storage_t::function)
                fail(op,
                     "called object " + quoted(fn.name) + " is not a function");

            std::vector< expr_t > args = argument_expression_list(p, fn);
            expect(p, tok_t::rparen, "expected ')' after arguments");

            const type_t t = *as< function_t >(fn.type).result;
            e = expr_t { t, call_t { box(std::move(e)), std::move(args) } };
            break;
        }
        case tok_t::inc:
        case tok_t::dec: {
            advance(p);
            require_modifiable_lvalue(op, e);

            const type_t t = e.type;
            const unary_op_t u = op.kind == tok_t::inc ? unary_op_t::post_inc :
                                                         unary_op_t::post_dec;

            e = expr_t { t, unary_t { u, box(std::move(e)) } };
            break;
        }
        case tok_t::lbracket: not_yet(op, "array subscript");
        case tok_t::dot:
        case tok_t::arrow: not_yet(op, "member access");
        default: return e;
        }
    }
}

// Each argument is converted to its parameter's type as if by
// assignment; past the parameters of a variadic function, the default
// argument promotions apply, 6.5.3.3p6.  The count must agree, p2.
std::vector< expr_t >
argument_expression_list(parser_t &p, const symbol_t &fn)
{
    const function_t &type = as< function_t >(fn.type);

    std::vector< expr_t > args;

    if (peek(p).kind != tok_t::rparen) {
        do {
            const token_t &start = peek(p);

            expr_t arg = assignment_expression(p);
            require_value(start, arg);

            if (args.size() < type.params.size())
                arg = convert_to(std::move(arg), *type.params[args.size()]);
            else if (type.variadic)
                arg = promote(std::move(arg));
            else
                fail(start,
                     "too many arguments to function " + quoted(fn.name));

            args.push_back(std::move(arg));
        } while (accept(p, tok_t::comma));
    }

    if (args.size() < type.params.size())
        fail(peek(p), "too few arguments to function " + quoted(fn.name));

    return args;
}

// A name must be declared, 6.5.2p2; parentheses leave the expression
// as it is, an lvalue included, p6.
expr_t
primary_expression(parser_t &p)
{
    const token_t &t = peek(p);

    switch (t.kind) {
    case tok_t::identifier: {
        advance(p);

        const symbol_t *sym = lookup(p, t.text);
        if (!sym)
            fail(t, quoted(t.text) + " undeclared");

        return expr_t { sym->type, name_t { sym } };
    }
    case tok_t::integer: advance(p); return integer_constant(t);
    case tok_t::lparen: {
        advance(p);

        expr_t e = expression(p);
        expect(p, tok_t::rparen, "expected ')'");

        return e;
    }
    default: fail(t, "expected expression");
    }
}

} // namespace

unit_t
parse(const std::vector< token_t > &tokens)
{
    parser_t p { tokens, 0, { }, { }, make_void() };
    p.scopes.emplace_back();

    translation_unit(p);

    return std::move(p.unit);
}

} // namespace rcc
