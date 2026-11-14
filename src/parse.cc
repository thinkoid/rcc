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

// A tag: the type it names, the keyword that declared it, and whether
// a list has defined its content.
struct tag_t
{
    type_t type;
    tok_t keyword;
    bool defined;
};

// The names one scope declares.  The tags of structs, unions and enums
// have a name space of their own, 6.2.3p1, apart from the ordinary
// identifiers, with the same block structure.  A symbol is writable
// here, since a later declaration may add to what an earlier one said:
// a function's assembler name, its marking.
struct scope_t
{
    std::unordered_map< std::string_view, symbol_t * > names;
    std::unordered_map< std::string_view, tag_t > tags;
};

// The state of one parse.  Scope 0 is file scope; the scopes die with
// the parser and leave only the symbols in the unit.  result and
// registers are the function being defined: its result type, and the
// most register variables live at once in it so far.  identities is
// the identity the next struct, union or enum type takes.
struct parser_t
{
    const std::vector< token_t > &tokens;
    std::size_t pos;
    const target_t &target;

    std::vector< scope_t > scopes;

    unit_t unit;
    type_t result;
    int registers;
    int identities;
};

// The declaration specifiers: the type, and the storage-class specifier
// when one was written, register, typedef or extern, for the caller to
// accept or refuse in its context.  tagged is whether the type
// specifier is a struct, union or enum one, which may declare a tag or
// enumeration constants with no declarator after it.
// function_specifier is the first inline or _Noreturn written, or null,
// for the caller to drop or refuse.
struct specifiers_t
{
    type_t type;
    const token_t *storage_class;
    bool tagged;
    const token_t *function_specifier;
};

// A parameter as written: its type, its name token when it has one,
// and whether it was declared register.
struct parameter_t
{
    type_t type;
    const token_t *start;
    const token_t *name;
    bool in_register;
};

// A declarator in the subset: a name, absent only from a parameter,
// the type it gives the name, and the parameters as written when it
// declares a function.
struct declarator_t
{
    const token_t *name;
    type_t type;
    std::optional< std::vector< parameter_t > > params;
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

// Plain char is signed or unsigned as the target has it, 6.2.5p20;
// long is the 64-bit signed type, and unsigned long the type of
// sizeof, 6.5.4.4p5.
type_t
char_type(const parser_t &p)
{
    return make_int(8, p.target.char_is_signed);
}

type_t
long_type()
{
    return make_int(64, true);
}

type_t
size_type()
{
    return make_int(64, false);
}

// The not-yet types that are one type wherever they are written: the
// floating types and __builtin_va_list, each of a fixed identity.  A
// struct, a union or an enum takes the next identity after theirs.
type_t
float_type()
{
    return make_not_yet(0, "floating type 'float'");
}

type_t
double_type()
{
    return make_not_yet(1, "floating type 'double'");
}

type_t
long_double_type()
{
    return make_not_yet(2, "floating type 'long double'");
}

type_t
va_list_type()
{
    return make_not_yet(3, "'__builtin_va_list'");
}

constexpr int first_tag_identity = 4;

const type_t &
pointee_of(const type_t &t)
{
    return *as< pointer_t >(t).pointee;
}

bool
is_void_pointer(const type_t &t)
{
    return is< pointer_t >(t) && is< void_t >(pointee_of(t));
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

//
// constant values
//

// A literal holds the value of its type: in the type's range, but for
// a 64-bit unsigned value past LLONG_MAX, which the long long holds as
// its bits.  The evaluator reads it exactly, in a type wide enough for
// any value of any operand and any sum or product of two signed ones.
using exact_t = __int128;

bool
is_integer_constant(const expr_t &e)
{
    return is< literal_t >(e) && is< int_t >(e.type);
}

exact_t
exact_value(const expr_t &e)
{
    const long long v = as< literal_t >(e).value;
    const int_t &t = as< int_t >(e.type);

    if (t.width == 64 && !t.is_signed)
        return static_cast< unsigned long long >(v);

    return v;
}

// The bits of a constant of integer type, the operand of the bitwise
// operators and of every unsigned operation.
unsigned long long
bits_of(const expr_t &e)
{
    return static_cast< unsigned long long >(as< literal_t >(e).value);
}

// The low N bits of a value, read in a type of width N: the conversion
// of 6.3.1.3p2 to an unsigned type, and of 6.3.1.3p3 to a signed one,
// which rcc defines so (G2).  A value the type holds is unchanged, p1.
long long
wrapped(exact_t v, const int_t &t)
{
    auto bits = static_cast< unsigned long long >(v);

    if (t.width < 64) {
        const auto mask = (1ull << t.width) - 1;
        const auto sign = 1ull << (t.width - 1);

        bits &= mask;
        if (t.is_signed && (bits & sign))
            bits |= ~mask;
    }

    return static_cast< long long >(bits);
}

expr_t
constant_of(const type_t &t, exact_t v)
{
    return expr_t { t, literal_t { wrapped(v, as< int_t >(t)) } };
}

// Whether the conversion of e to t is a constant: an integer constant
// to an integer type, or to a pointer type, an address constant
// (6.6p11); a pointer constant to another pointer type.  A pointer
// constant to an integer is a conversion no integer constant expression
// contains, 6.6p8, and stays a cast.
bool
converts_to_constant(const expr_t &e, const type_t &t)
{
    if (!is< literal_t >(e))
        return false;

    if (is< int_t >(e.type))
        return is< int_t >(t) || is< pointer_t >(t);

    return is< pointer_t >(e.type) && is< pointer_t >(t);
}

// The constant a conversion yields: an integer by 6.3.1.3; a pointer
// from an integer widened to 64 bits in its own signedness, 6.3.2.3p5,
// as the platform's compiler converts, so 0 is the null pointer; a
// pointer from a pointer unchanged, a null one null, 6.3.2.3p4.  The
// long long of an integer literal is that widening already: a signed
// value sign-extended, an unsigned one zero-extended or its own bits.
expr_t
converted_constant(const expr_t &e, const type_t &t)
{
    if (is< pointer_t >(t))
        return expr_t { t, as< literal_t >(e) };

    return constant_of(t, exact_value(e));
}

// The one place a conversion enters the tree.  An expression already
// of the type passes through, so no inserted cast converts a type to
// itself; only a cast the source writes may, since it yields no lvalue.
// A constant converted is the constant of the new type: the source
// wrote a number, not a conversion, so a char constant of 300 does not
// exist and the tree holds 44, as the conversion would have computed.
expr_t
convert_to(expr_t e, const type_t &t)
{
    if (e.type == t)
        return e;

    if (converts_to_constant(e, t))
        return converted_constant(e, t);

    return expr_t { t, cast_t { box(std::move(e)) } };
}

expr_t
promote(expr_t e)
{
    const type_t t = promoted(e.type);

    return convert_to(std::move(e), t);
}

// An integer operand of pointer arithmetic: promoted and then widened
// to 64 bits in its own signedness, in one cast, as an operand of the
// usual arithmetic conversions goes; an unsigned index never reads as
// negative (G6).
expr_t
widened_index(expr_t e)
{
    const type_t t = make_int(64, as< int_t >(promoted(e.type)).is_signed);

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

// The not-yet type a type is, or holds as an array's element, or null.
const not_yet_t *
not_yet_in(const type_t &t)
{
    if (is< not_yet_t >(t))
        return &as< not_yet_t >(t);

    if (is< array_t >(t))
        return not_yet_in(*as< array_t >(t).element);

    return nullptr;
}

// A not-yet type stands wherever nothing needs its object: behind a
// pointer, as the type a typedef names, in a function declaration, as
// the type of an object defined elsewhere.  Where the object is needed,
// so are its size and its layout, which rcc does not know, and the use
// is refused with the type's own words.  The parser refuses every
// expression that would have such a type where it would be made, by
// indirection, by a call or by a cast, so no expression has one and c1
// meets one only as a pointee it does not look into.
void
require_honoured(const token_t &at, const type_t &t)
{
    if (const not_yet_t *n = not_yet_in(t))
        not_yet(at, n->what);
}

// restrict qualifies a pointer to an object type, or an array of such
// pointers, 6.7.4.1p2.  It promises what an optimizer may assume, and
// deleting it changes no program's meaning, p9, so rcc takes the
// promise and drops it.  Behind a '*' it always qualifies such a
// pointer; among the specifiers it qualifies the type they name, which
// is one only through a typedef name.  Whether a not-yet type is one is
// not known, so restrict on it is not yet.
void
require_restrictable(const token_t &q, const type_t &t)
{
    require_honoured(q, t);

    const type_t *element = &t;
    while (is< array_t >(*element))
        element = as< array_t >(*element).element.get();

    if (!is< pointer_t >(*element) || is< function_t >(pointee_of(*element)))
        fail(q, quoted(q.text) +
                    " qualifies a type that is not a pointer to an object");
}

// A value in the subset is an integer or a pointer to an object.  A
// function's name stands only as the callee of a call: function
// pointers are outside the subset, so a function designator where a
// value is wanted is not yet, and a void expression is an error.
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

// 6.5.17.1p2, 6.5.3.5p1, 6.5.4.1p1: an lvalue of integer or pointer
// type.  An array is an lvalue but not a modifiable one, 6.3.2.1p1.
void
require_modifiable_lvalue(const token_t &at, const expr_t &e)
{
    if (!is_lvalue(e) || !(is< int_t >(e.type) || is< pointer_t >(e.type)))
        fail(at, "expression is not assignable");
}

// Array decay, 6.3.2.1p3: but as the operand of sizeof or of unary &,
// an array becomes a pointer to its first element, a cast c1 reads as
// that address.  Lvalue conversion, p2, is c1's load and needs no
// node.
expr_t
decayed(expr_t e)
{
    if (!is< array_t >(e.type))
        return e;

    type_t t = make_pointer(*as< array_t >(e.type).element);

    return expr_t { std::move(t), cast_t { box(std::move(e)) } };
}

// An operand whose value is used: not void, not a function, and
// decayed, so it is a scalar.
expr_t
rvalue(const token_t &at, expr_t e)
{
    require_value(at, e);

    return decayed(std::move(e));
}

// The operands that must be integers: of the arithmetic operators but
// + and -, 6.5.6p2, 6.5.8p2, 6.5.11p2, 6.5.12p2, 6.5.13p2, and of unary
// - and ~, 6.5.4.3p1.
void
require_integer(const token_t &op, const expr_t &e)
{
    if (is< pointer_t >(e.type))
        fail(op, "operand of " + quoted(op.text) + " is a pointer");
}

// Pointer arithmetic, ++ and -- included, needs a pointer to a
// complete object type, 6.5.7p2, p3: not void, which GNU C would take
// as a byte, and not a function.  The step is the pointee's size, so a
// not-yet pointee is not yet.
void
require_object_pointee(const token_t &op, const type_t &t)
{
    if (is< void_t >(pointee_of(t)))
        fail(op, "arithmetic on a pointer to void");

    if (is< function_t >(pointee_of(t)))
        fail(op, "arithmetic on a pointer to a function");

    require_honoured(op, pointee_of(t));
}

// Prefix and postfix ++ and -- on a pointer step it by one element,
// 6.5.4.1p2, 6.5.3.5p2, so the pointee is an object type as for p + 1.
void
require_steppable(const token_t &op, const expr_t &e)
{
    require_modifiable_lvalue(op, e);

    if (is< pointer_t >(e.type))
        require_object_pointee(op, e.type);
}

//
// pointer conversions
//

// An integer constant expression of value 0, or one cast to void *,
// 6.3.2.3p3.  The parser folds every integer constant expression to a
// literal, and the cast of one to a pointer type to a literal of that
// type, so the two are an integer literal 0 and a void * literal 0.
// A literal 0 of another pointer type, (int *) 0, is a null pointer
// but not a null pointer constant.
bool
is_null_pointer_constant(const expr_t &e)
{
    if (!is< literal_t >(e) || as< literal_t >(e).value != 0)
        return false;

    return is< int_t >(e.type) || is_void_pointer(e.type);
}

// The conversion as if by assignment, of a value to the type of an
// object: the right operand of =, an initializer, an argument, a value
// returned.  The constraints are those of simple assignment,
// 6.5.17.2p1: integers convert to each other; a pointer takes a pointer
// of its own type, a pointer to void and an object pointer convert
// either way, and a null pointer constant converts to any pointer.  An
// integer and a pointer meet only through a cast, 6.5.5p3.  A value
// converted to a not-yet type, as an argument meets a parameter of one,
// is not yet.
expr_t
assignment_conversion(const token_t &at, expr_t e, const type_t &t)
{
    require_honoured(at, t);

    e = rvalue(at, std::move(e));

    const bool to_pointer = is< pointer_t >(t);
    const bool from_pointer = is< pointer_t >(e.type);

    if (to_pointer && !from_pointer && !is_null_pointer_constant(e))
        fail(at, "incompatible integer to pointer conversion");

    if (!to_pointer && from_pointer)
        fail(at, "incompatible pointer to integer conversion");

    if (to_pointer && from_pointer && !(e.type == t) && !is_void_pointer(t) &&
        !is_void_pointer(e.type))
        fail(at, "incompatible pointer types");

    return convert_to(std::move(e), t);
}

// Two values, at least one a pointer, brought to one pointer type, as
// == and != and the arms of ?: take them, 6.5.10p2, p6, 6.5.16p3, p7:
// pointers of one type stay as they are, a pointer to void and an
// object pointer meet at void *, and a null pointer constant goes to
// the other operand's type.  False when none of the three applies.
bool
common_pointer(expr_t &lhs, expr_t &rhs)
{
    const bool lp = is< pointer_t >(lhs.type);
    const bool rp = is< pointer_t >(rhs.type);

    type_t t = lp ? lhs.type : rhs.type;

    if (lp && rp && !(lhs.type == rhs.type)) {
        if (!is_void_pointer(lhs.type) && !is_void_pointer(rhs.type))
            return false;

        t = make_pointer(make_void());
    }
    else if (!lp && !is_null_pointer_constant(lhs))
        return false;
    else if (!rp && !is_null_pointer_constant(rhs))
        return false;

    lhs = convert_to(std::move(lhs), t);
    rhs = convert_to(std::move(rhs), t);

    return true;
}

//
// integer constant expressions
//

// The evaluator of backend.md 6.4.  A node built on integer constants
// alone, under an operator 6.6p3 does not exclude, is replaced by the
// constant of its type, the type C gives the node, holding the value C
// gives it.  The operands are those of 6.6p8: integer and character
// constants, sizeof and casts of constants, each a literal by the time
// its parent is built.  A node with any other operand is left as built,
// so x * 1 multiplies and x - x subtracts.  The constant is the value
// the instruction would compute, so folding changes no program: a
// signed result the type cannot hold wraps (G1), a shift count is
// masked (G3), and a division with no value is left to the instruction
// (G4).  6.6p4, which makes an out-of-range constant expression a
// constraint violation, applies where the grammar requires a constant
// expression, the array size, and the wrapped value is refused there.

template< typename T >
T
arithmetic_result(binary_op_t op, T a, T b)
{
    switch (op) {
    case binary_op_t::add: return a + b;
    case binary_op_t::sub: return a - b;
    case binary_op_t::mul: return a * b;
    case binary_op_t::div: return a / b;
    default: return a % b;
    }
}

// Whether a division has a value: not by zero, 6.5.6p6, and not with a
// quotient the type cannot hold, p7, which on the machine is the one
// case of the most negative value divided by -1; the remainder of that
// division has no value either.  Such a node is left as built and the
// instruction traps at run time (G4).
bool
divides(const int_t &t, const expr_t &lhs, const expr_t &rhs)
{
    if (exact_value(rhs) == 0)
        return false;

    const auto most_negative = -(exact_t(1) << (t.width - 1));

    return !(t.is_signed && exact_value(lhs) == most_negative &&
             exact_value(rhs) == -1);
}

// * / % + - in the operands' common type.  Unsigned arithmetic is
// modulo 2^N, 6.2.5p11; a signed sum, difference or product wraps as
// the instruction does (G1), where 6.5p5 leaves it undefined.
// Division truncates, 6.5.6p7.
std::optional< expr_t >
folded_arithmetic(binary_op_t op,
                  const type_t &t,
                  const expr_t &lhs,
                  const expr_t &rhs)
{
    const int_t &type = as< int_t >(t);

    const bool division = op == binary_op_t::div || op == binary_op_t::rem;
    if (division && !divides(type, lhs, rhs))
        return std::nullopt;

    if (!type.is_signed)
        return constant_of(t,
                           arithmetic_result(op, bits_of(lhs), bits_of(rhs)));

    return constant_of(
        t, arithmetic_result(op, exact_value(lhs), exact_value(rhs)));
}

// << >> by a count taken modulo the width of the promoted left operand,
// as the instruction masks it (G3): 6.5.8p3 leaves a count that is
// negative or not less than the width undefined, and the constant is
// what the instruction computes.  A left shift is of the bits and
// wraps, G3, where 6.5.8p4 leaves a signed result the type cannot hold
// undefined.  A signed right shift is arithmetic, G3, the
// implementation's choice for a negative value by 6.5.8p5.
expr_t
folded_shift(binary_op_t op,
             const type_t &t,
             const expr_t &lhs,
             const expr_t &rhs)
{
    const int_t &type = as< int_t >(t);
    const auto count = bits_of(rhs) & unsigned(type.width - 1);

    if (op == binary_op_t::shl)
        return constant_of(t, bits_of(lhs) << count);

    if (type.is_signed)
        return constant_of(t, as< literal_t >(lhs).value >> count);

    return constant_of(t, bits_of(lhs) >> count);
}

// & ^ | on the bits, which every value of the type has, 6.5.11p4,
// 6.5.12p4, 6.5.13p4.
expr_t
folded_bitwise(binary_op_t op,
               const type_t &t,
               const expr_t &lhs,
               const expr_t &rhs)
{
    const auto a = bits_of(lhs);
    const auto b = bits_of(rhs);

    switch (op) {
    case binary_op_t::bit_and: return constant_of(t, a & b);
    case binary_op_t::bit_xor: return constant_of(t, a ^ b);
    default: return constant_of(t, a | b);
    }
}

// < > <= >= == != on the values in their common type, && || on each
// operand against zero, 6.5.14p3, 6.5.15p3: 1 or 0, an int.
bool
truth_of(binary_op_t op, exact_t a, exact_t b)
{
    switch (op) {
    case binary_op_t::lt: return a < b;
    case binary_op_t::gt: return a > b;
    case binary_op_t::le: return a <= b;
    case binary_op_t::ge: return a >= b;
    case binary_op_t::eq: return a == b;
    case binary_op_t::ne: return a != b;
    case binary_op_t::log_and: return a != 0 && b != 0;
    default: return a != 0 || b != 0;
    }
}

// The constant a binary node of two integer constants, of type t, folds
// to, or nothing for a division with no value.  The comma is excluded
// by 6.6p3 and never reaches here.
std::optional< expr_t >
folded_binary(binary_op_t op,
              const type_t &t,
              const expr_t &lhs,
              const expr_t &rhs)
{
    switch (op) {
    case binary_op_t::add:
    case binary_op_t::sub:
    case binary_op_t::mul:
    case binary_op_t::div:
    case binary_op_t::rem: return folded_arithmetic(op, t, lhs, rhs);
    case binary_op_t::shl:
    case binary_op_t::shr: return folded_shift(op, t, lhs, rhs);
    case binary_op_t::bit_and:
    case binary_op_t::bit_xor:
    case binary_op_t::bit_or: return folded_bitwise(op, t, lhs, rhs);
    default:
        return constant_of(t, truth_of(op, exact_value(lhs), exact_value(rhs)));
    }
}

// Unary - and ~ on a promoted integer constant, 6.5.4.3p3, p4: the
// negative modulo 2^N, as the instruction computes it (G1 for the most
// negative value, 6.2.5p11 for an unsigned one); the complement of the
// bits.  ! is 1 for a constant equal to 0 and 0 otherwise, an int, p5.
expr_t
folded_unary(unary_op_t op, const type_t &t, const expr_t &arg)
{
    switch (op) {
    case unary_op_t::neg: return constant_of(t, -exact_value(arg));
    case unary_op_t::bit_not: return constant_of(t, ~bits_of(arg));
    default: return constant_of(t, exact_value(arg) == 0);
    }
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

// A binary node, or the constant it folds to when both operands are
// integer constants and the operation has a value.
expr_t
make_binary(binary_op_t op, type_t t, expr_t lhs, expr_t rhs)
{
    if (op != binary_op_t::comma && is_integer_constant(lhs) &&
        is_integer_constant(rhs))
        if (auto folded = folded_binary(op, t, lhs, rhs))
            return std::move(*folded);

    return expr_t { std::move(t),
                    binary_t { op, box(std::move(lhs)), box(std::move(rhs)) } };
}

// * / % & ^ |, and + - on two integers: the usual arithmetic
// conversions, and the result in the common type (6.5.6p4, 6.5.7p5,
// 6.5.11p3, 6.5.12p3, 6.5.13p3).
expr_t
arithmetic(const token_t &op, expr_t lhs, expr_t rhs)
{
    lhs = rvalue(op, std::move(lhs));
    rhs = rvalue(op, std::move(rhs));

    require_integer(op, lhs);
    require_integer(op, rhs);

    const type_t t = arithmetic_conversions(lhs, rhs);

    return make_binary(binary_operator(op.kind), t, std::move(lhs),
                       std::move(rhs));
}

// + with a pointer, 6.5.7p2, p9: a pointer and an integer in either
// order, the result of the pointer's type.  The tree keeps the order
// written: c1 evaluates left to right (G13), whichever side the
// pointer is on.
expr_t
addition(const token_t &op, expr_t lhs, expr_t rhs)
{
    lhs = rvalue(op, std::move(lhs));
    rhs = rvalue(op, std::move(rhs));

    const bool lp = is< pointer_t >(lhs.type);
    const bool rp = is< pointer_t >(rhs.type);

    if (!lp && !rp)
        return arithmetic(op, std::move(lhs), std::move(rhs));

    if (lp && rp)
        fail(op, "cannot add two pointers");

    const type_t t = lp ? lhs.type : rhs.type;
    require_object_pointee(op, t);

    if (lp)
        rhs = widened_index(std::move(rhs));
    else
        lhs = widened_index(std::move(lhs));

    return make_binary(binary_op_t::add, t, std::move(lhs), std::move(rhs));
}

// - with a pointer, 6.5.7p3, p9, p10: a pointer less an integer has
// the pointer's type, and two pointers to one complete object type
// subtract to long, ptrdiff_t on LP64.  The type is the machine's
// view, so no qualifier is left to tell two compatible pointee types
// apart.
expr_t
subtraction(const token_t &op, expr_t lhs, expr_t rhs)
{
    lhs = rvalue(op, std::move(lhs));
    rhs = rvalue(op, std::move(rhs));

    const bool lp = is< pointer_t >(lhs.type);
    const bool rp = is< pointer_t >(rhs.type);

    if (!lp && !rp)
        return arithmetic(op, std::move(lhs), std::move(rhs));

    if (!lp)
        fail(op, "cannot subtract a pointer from an integer");

    if (rp && !(lhs.type == rhs.type))
        fail(op, "subtraction of pointers to different types");

    require_object_pointee(op, lhs.type);

    if (rp)
        return make_binary(binary_op_t::sub, long_type(), std::move(lhs),
                           std::move(rhs));

    const type_t t = lhs.type;
    rhs = widened_index(std::move(rhs));

    return make_binary(binary_op_t::sub, t, std::move(lhs), std::move(rhs));
}

// < > <= >= == !=: the result an int (6.5.9p7, 6.5.10p4), integer
// operands in their common type (6.5.9p4, 6.5.10p5).  Pointers compare
// as addresses (G7): a relational operator takes two pointers of one
// type, 6.5.9p2, and == and != what common_pointer() brings to one,
// 6.5.10p2.  A pointer and any other integer meet only through a cast.
expr_t
comparison(const token_t &op, expr_t lhs, expr_t rhs)
{
    lhs = rvalue(op, std::move(lhs));
    rhs = rvalue(op, std::move(rhs));

    const bool lp = is< pointer_t >(lhs.type);
    const bool rp = is< pointer_t >(rhs.type);
    const bool equality = op.kind == tok_t::eq || op.kind == tok_t::ne;

    if (!lp && !rp)
        arithmetic_conversions(lhs, rhs);
    else if (equality && !common_pointer(lhs, rhs))
        fail(op, lp && rp ? "comparison of distinct pointer types" :
                            "comparison between pointer and integer");
    else if (!equality && !(lp && rp))
        fail(op, "comparison between pointer and integer");
    else if (!equality && !(lhs.type == rhs.type))
        fail(op, "comparison of distinct pointer types");

    return make_binary(binary_operator(op.kind), int_type(), std::move(lhs),
                       std::move(rhs));
}

// && ||: an int (6.5.14p3, 6.5.15p3).  The operands are scalars and
// stay as they are, 6.5.14p2, 6.5.15p2; c1 compares each against zero
// in its own type, a pointer against null.
expr_t
logical(const token_t &op, expr_t lhs, expr_t rhs)
{
    lhs = rvalue(op, std::move(lhs));
    rhs = rvalue(op, std::move(rhs));

    return make_binary(binary_operator(op.kind), int_type(), std::move(lhs),
                       std::move(rhs));
}

// << >>: the type of the promoted left operand, 6.5.8p3.  The integer
// promotions are performed on each operand on its own there, so the
// count keeps its promoted type; the instruction reads only its low
// bits, in cl (G3).
expr_t
shift(const token_t &op, expr_t lhs, expr_t rhs)
{
    lhs = rvalue(op, std::move(lhs));
    rhs = rvalue(op, std::move(rhs));

    require_integer(op, lhs);
    require_integer(op, rhs);

    lhs = promote(std::move(lhs));
    rhs = promote(std::move(rhs));
    const type_t t = lhs.type;

    return make_binary(binary_operator(op.kind), t, std::move(lhs),
                       std::move(rhs));
}

// The result is the right operand's type and value, an array decayed,
// 6.5.18p2.  A function name is refused on either side, which keeps
// every function-typed expression a name_t; the left operand comes
// refused and decayed.
expr_t
comma(const token_t &op, expr_t lhs, expr_t rhs)
{
    require_not_function(op, rhs);
    rhs = decayed(std::move(rhs));

    const type_t t = rhs.type;

    return make_binary(binary_op_t::comma, t, std::move(lhs), std::move(rhs));
}

// The type of the whole is the type of the left operand, 6.5.17.1p3,
// 6.5.17.2p2, and a plain assignment converts the right operand to it
// as by assignment.  E1 op= E2 is E1 = E1 op (E2) with E1 evaluated
// once, 6.5.17.3p4: the operation is computed in the common type of the
// usual arithmetic conversions, so the right operand goes to that type
// here, or in the promoted left type for a shift, 6.5.8p3, where the
// count is promoted on its own, as in the shift production.  c1 takes
// the loaded left operand into it and the result back to the left
// type.  A pointer takes += and -= only, 6.5.17.3p1, computed in the
// pointer's type with the right operand widened as for p + n.  The left
// operand comes checked as a modifiable lvalue.
expr_t
assignment(const token_t &op, expr_t lhs, expr_t rhs)
{
    const type_t t = lhs.type;

    if (op.kind == tok_t::assign) {
        rhs = assignment_conversion(op, std::move(rhs), t);

        return expr_t { t, assign_t { std::nullopt, t, box(std::move(lhs)),
                                      box(std::move(rhs)) } };
    }

    rhs = rvalue(op, std::move(rhs));
    require_integer(op, rhs);

    const binary_op_t compound = binary_operator(op.kind);
    const bool additive =
        compound == binary_op_t::add || compound == binary_op_t::sub;

    type_t common = t;
    if (is< pointer_t >(t)) {
        if (!additive)
            require_integer(op, lhs);

        require_object_pointee(op, t);
        rhs = widened_index(std::move(rhs));
    }
    else {
        if (compound == binary_op_t::shl || compound == binary_op_t::shr) {
            common = promoted(t);
            rhs = promote(std::move(rhs));
        }
        else {
            common = common_type(t, rhs.type);
            rhs = convert_to(std::move(rhs), common);
        }
    }

    return expr_t { t, assign_t { compound, common, box(std::move(lhs)),
                                  box(std::move(rhs)) } };
}

// The arms go to their common type, 6.5.16p6, or two pointers to the
// one common_pointer() finds, p7.  A pointer and an integer other than
// a null pointer constant do not meet, p3.  Two void arms are valid C,
// outside the subset for now.  On a constant condition and two
// constant arms, the node is the arm the condition selects, p5.  The
// condition comes as an rvalue.
expr_t
conditional(const token_t &op, expr_t cond, expr_t yes, expr_t no)
{
    if (is< void_t >(yes.type) && is< void_t >(no.type))
        not_yet(op, "conditional with void operands");

    yes = rvalue(op, std::move(yes));
    no = rvalue(op, std::move(no));

    const bool pointers = is< pointer_t >(yes.type) && is< pointer_t >(no.type);

    if (!is< pointer_t >(yes.type) && !is< pointer_t >(no.type))
        arithmetic_conversions(yes, no);
    else if (!common_pointer(yes, no))
        fail(op, pointers ?
                     "pointer type mismatch in conditional expression" :
                     "pointer/integer type mismatch in conditional expression");

    const type_t t = yes.type;

    if (is_integer_constant(cond) && is_integer_constant(yes) &&
        is_integer_constant(no))
        return exact_value(cond) != 0 ? std::move(yes) : std::move(no);

    return expr_t { t,
                    conditional_t { box(std::move(cond)), box(std::move(yes)),
                                    box(std::move(no)) } };
}

// The type name is void or a scalar, 6.5.5p2: to void the operand is
// discarded, whatever its type; to a scalar it converts from a scalar,
// integer and pointer either way, 6.3.2.3p5, p6, and pointer to
// pointer, p1, p7.  A cast yields no lvalue, 6.5.5p6 and its footnote,
// so a cast to the operand's own type is still a node.  A constant cast
// to an integer or a pointer type is the constant converted_constant()
// gives, which is not an lvalue either.  A cast to a not-yet type is
// not yet.
expr_t
cast(const token_t &open, const type_t &t, expr_t arg)
{
    require_honoured(open, t);

    if (is< void_t >(t)) {
        require_not_function(open, arg);
        return convert_to(decayed(std::move(arg)), t);
    }

    if (!is< int_t >(t) && !is< pointer_t >(t))
        fail(open, "cast to a type that is not a scalar");

    arg = rvalue(open, std::move(arg));

    if (converts_to_constant(arg, t))
        return converted_constant(arg, t);

    return expr_t { t, cast_t { box(std::move(arg)) } };
}

// Prefix ++ and -- have the type of their lvalue operand, 6.5.4.1.
expr_t
prefix_step(const token_t &op, expr_t arg)
{
    require_steppable(op, arg);

    const type_t t = arg.type;
    const unary_op_t u =
        op.kind == tok_t::inc ? unary_op_t::pre_inc : unary_op_t::pre_dec;

    return expr_t { t, unary_t { u, box(std::move(arg)) } };
}

// - and ~ promote their operand and have its promoted type, 6.5.4.3p3,
// p4.  - and ~ on an integer constant are a constant.
expr_t
unary_arithmetic(const token_t &op, expr_t arg)
{
    arg = rvalue(op, std::move(arg));
    require_integer(op, arg);

    arg = promote(std::move(arg));
    const type_t t = arg.type;
    const unary_op_t u =
        op.kind == tok_t::minus ? unary_op_t::neg : unary_op_t::bit_not;

    if (is_integer_constant(arg))
        return folded_unary(u, t, arg);

    return expr_t { t, unary_t { u, box(std::move(arg)) } };
}

// ! is an int, its scalar operand left as it is, 6.5.4.3p5.  ! on an
// integer constant is a constant.
expr_t
logical_not(const token_t &op, expr_t arg)
{
    arg = rvalue(op, std::move(arg));

    if (is_integer_constant(arg))
        return folded_unary(unary_op_t::log_not, int_type(), arg);

    return expr_t { int_type(),
                    unary_t { unary_op_t::log_not, box(std::move(arg)) } };
}

// & takes an lvalue, which does not decay, and yields a pointer to its
// type, 6.5.4.2p1, p3.
expr_t
address_of(const token_t &op, expr_t arg)
{
    require_not_function(op, arg);

    if (!is_lvalue(arg))
        fail(op, "cannot take the address of an rvalue");

    // 6.5.4.2p1: not an object declared register, whether or not it
    // got a register: 6.7.2p12's footnote says that addressable
    // storage does not make the address computable
    if (is< name_t >(arg) && as< name_t >(arg).sym->declared_register)
        fail(op, "cannot take the address of " +
                     quoted(as< name_t >(arg).sym->name) +
                     ", declared register");

    type_t t = make_pointer(arg.type);

    return expr_t { std::move(t),
                    unary_t { unary_op_t::addr, box(std::move(arg)) } };
}

// *E: the operand a pointer, after decay; the result the lvalue it
// points to, of the pointee type, 6.5.4.2p2, p4.  A void pointee
// designates nothing to read, a function pointer is not in the subset,
// and a not-yet pointee is not yet.
expr_t
indirection(const token_t &op, expr_t arg)
{
    arg = rvalue(op, std::move(arg));
    if (!is< pointer_t >(arg.type))
        fail(op, "indirection requires a pointer operand");

    const type_t t = pointee_of(arg.type);

    if (is< void_t >(t))
        fail(op, "indirection through a pointer to void");

    if (is< function_t >(t))
        not_yet(op, "indirection through a function pointer");

    require_honoured(op, t);

    return expr_t { t, unary_t { unary_op_t::deref, box(std::move(arg)) } };
}

// sizeof folds to a constant of type unsigned long, size_t on LP64,
// 6.5.4.4p2, p5.  A function, void, and an array of unknown size have
// no size, p1; a not-yet type has one rcc does not know yet.  The name
// is the operator's, with the underscore a name takes when it is a
// keyword of C++.
expr_t
sizeof_(const token_t &op, const type_t &t)
{
    if (is< void_t >(t))
        fail(op, "invalid application of 'sizeof' to a void type");

    if (is< function_t >(t))
        fail(op, "invalid application of 'sizeof' to a function type");

    if (is< array_t >(t) && as< array_t >(t).count < 0)
        fail(op, "invalid application of 'sizeof' to an array of unknown size");

    require_honoured(op, t);

    return expr_t { size_type(), literal_t { size_of(t) } };
}

// Postfix ++ and -- have the type of their lvalue operand, 6.5.3.5.
expr_t
postfix_step(const token_t &op, expr_t arg)
{
    require_steppable(op, arg);

    const type_t t = arg.type;
    const unary_op_t u =
        op.kind == tok_t::inc ? unary_op_t::post_inc : unary_op_t::post_dec;

    return expr_t { t, unary_t { u, box(std::move(arg)) } };
}

// E1[E2] is *((E1)+(E2)), 6.5.3.2p2, and is built as exactly that tree,
// so the pointer may be either operand.
expr_t
subscript(const token_t &op, expr_t e, expr_t index)
{
    e = rvalue(op, std::move(e));
    index = rvalue(op, std::move(index));
    if (!is< pointer_t >(e.type) && !is< pointer_t >(index.type))
        fail(op, "subscripted value is not an array or a pointer");

    return indirection(op, addition(op, std::move(e), std::move(index)));
}

//
// scopes
//

const symbol_t *
lookup(const parser_t &p, std::string_view name)
{
    for (auto scope = p.scopes.rbegin(); scope != p.scopes.rend(); ++scope)
        if (const auto it = scope->names.find(name); it != scope->names.end())
            return it->second;

    return nullptr;
}

// The type an identifier names where it stands: a typedef's, in scope
// and not hidden by an inner declaration of the name; or, while
// undeclared, __builtin_va_list's, clang's and gcc's name for the type
// behind va_list, which no header declares.  Nothing for any other
// token.
std::optional< type_t >
named_type(const parser_t &p, const token_t &t)
{
    if (t.kind != tok_t::identifier)
        return std::nullopt;

    const symbol_t *sym = lookup(p, t.text);
    if (sym && sym->storage == storage_t::type_name)
        return sym->type;

    if (!sym && t.text == "__builtin_va_list")
        return va_list_type();

    return std::nullopt;
}

bool
names_type(const parser_t &p, const token_t &t)
{
    return named_type(p, t).has_value();
}

// The register variables live at this point of the function: those of
// the scopes open, the parameters' among them.  A scope's variables
// leave the count when the scope is popped.
int
live_registers(const parser_t &p)
{
    int live = 0;
    for (const auto &scope : p.scopes)
        for (const auto &[name, sym] : scope.names)
            if (in_register(*sym))
                ++live;

    return live;
}

// A target's count of register variables as the warning spells it.
std::string
in_words(int n)
{
    constexpr std::string_view words[] = { "zero",  "one",  "two", "three",
                                           "four",  "five", "six", "seven",
                                           "eight", "nine", "ten" };
    if (n >= 0 && n < std::ssize(words))
        return std::string(words[n]);

    return std::to_string(n);
}

// A new object in the innermost scope.  Without linkage a name is
// declared once per scope (6.7.1p4); an inner scope may shadow it.  The
// map's key views the symbol's own name, which the deque keeps in
// place.  A register declaration met with the target's register
// variables all live declares an ordinary object, with a warning: the
// extent to which the suggestion is effective is implementation-defined,
// 6.7.2p12, so the program is conforming and compiles.  One that gets a
// register takes the next, the count of those live, backend.md chapter
// 4, and the function's count is the most taken at once.
const symbol_t *
declare(parser_t &p,
        const token_t &name,
        type_t t,
        storage_t storage,
        bool in_register)
{
    auto &scope = p.scopes.back().names;
    if (scope.contains(name.text))
        fail(name, "redefinition of " + quoted(name.text));

    const bool declared_register = in_register;
    const int count = p.target.register_variables;
    const int live = in_register ? live_registers(p) : 0;
    if (in_register && live == count) {
        p.unit.warnings.push_back(
            { name.where, "register variable " + quoted(name.text) +
                              " in a stack slot: the " + in_words(count) +
                              " registers are taken" });
        in_register = false;
    }

    const int register_index = in_register ? live : -1;
    p.registers = std::max(p.registers, register_index + 1);

    symbol_t &sym = p.unit.symbols.emplace_back(
        symbol_t { std::string(name.text), std::move(t), storage,
                   declared_register, register_index });
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
// what GNU C writes in a declaration
//

// A system header is written in GNU C, and rcc reads it as it is.  It
// conforms with what a correct program depends on, the size and the
// layout of a type and the symbol a name links to, and leaves hints and
// warnings for later.  An attribute is of one of two kinds.  One that
// only licenses an optimizer, asks for a diagnostic, or describes what
// a compiler that calls each function as written does not depend on,
// is dropped.  Any other, known or not, marks what it is said of: mode
// changes a type's size, aligned and packed a layout, weak, alias and
// visibility a symbol.  A marking attribute does not stop the
// declaration, but the use of what it marks is refused with its name,
// as the use of a not-yet type is.  The names are GCC's, each the same
// with and without its double underscores.

// The attributes dropped, by name.
constexpr std::string_view dropped_attributes[] = {
    // what an optimizer may assume of a call, or where to lay it out
    "alloc_align", "alloc_size", "cold", "const", "leaf", "malloc", "nonnull",
    "noreturn", "nothrow", "pure", "returns_nonnull",

    // requests for a diagnostic
    "access", "deprecated", "format", "format_arg", "nodiscard", "sentinel",
    "unused", "warn_unused_result"
};

// An attribute's name as GCC reads it: __nonnull__ is nonnull.
std::string_view
attribute_name(const token_t &t)
{
    const std::string_view name = t.text;
    if (name.size() > 4 && name.starts_with("__") && name.ends_with("__"))
        return name.substr(2, name.size() - 4);

    return name;
}

// Whether the attribute of the name is one of those dropped.
bool
is_dropped(std::string_view attribute)
{
    return std::ranges::find(dropped_attributes, attribute) !=
           std::ranges::end(dropped_attributes);
}

// The words a refusal of what an attribute marks prints.
std::string
attribute_words(const token_t &name)
{
    return "attribute " + quoted(attribute_name(name));
}

// An attribute's name is a word, an identifier or a keyword: const and
// __const__ are attributes as well as qualifiers.
bool
is_word(const token_t &t)
{
    const char c = t.text.empty() ? '\0' : t.text[0];

    return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// The arguments of an attribute, from the open parenthesis to its
// match, skipped: nothing reads them.  The closing parenthesis is
// consumed with them.
void
skip_attribute_arguments(parser_t &p)
{
    advance(p);

    for (int depth = 0; peek(p).kind != tok_t::eof; advance(p)) {
        const tok_t kind = peek(p).kind;
        if (kind == tok_t::rparen && depth == 0)
            break;

        if (kind == tok_t::lparen)
            ++depth;
        else if (kind == tok_t::rparen)
            --depth;
    }

    expect(p, tok_t::rparen, "expected ')' after attribute arguments");
}

// One attribute of a list: empty, a name, or a name and its arguments
// in parentheses.  The name's token when the attribute marks, or null.
const token_t *
attribute(parser_t &p)
{
    if (next_is(p, { tok_t::comma, tok_t::rparen }))
        return nullptr;

    const token_t &name = peek(p);
    if (!is_word(name))
        fail(name, "expected attribute name");

    advance(p);

    if (peek(p).kind == tok_t::lparen)
        skip_attribute_arguments(p);

    return is_dropped(attribute_name(name)) ? nullptr : &name;
}

// Each __attribute__ ((list)), also spelled __attribute, of the run at
// the position, none or more, its attributes separated by commas.  The
// name's token of the first marking attribute, or null.
const token_t *
attribute_specifier_list(parser_t &p)
{
    const token_t *marking = nullptr;

    while (peek(p).kind == tok_t::kw_attribute) {
        const token_t &keyword = advance(p);

        if (!accept(p, tok_t::lparen) || !accept(p, tok_t::lparen))
            fail(peek(p), "expected '((' after " + quoted(keyword.text));

        do {
            const token_t *name = attribute(p);
            if (!marking)
                marking = name;
        } while (accept(p, tok_t::comma));

        if (!accept(p, tok_t::rparen) || !accept(p, tok_t::rparen))
            fail(peek(p), "expected '))' after attribute list");
    }

    return marking;
}

// __asm__ ("name"), also spelled __asm: the string, adjacent literals
// joined as in an expression, is the symbol's name in the assembly.
// glibc declares scanf with __asm__ ("" "__isoc99_scanf"), and a call
// of scanf must call __isoc99_scanf.  An empty name names no symbol.
std::string
asm_label(parser_t &p)
{
    const token_t &keyword = advance(p);
    expect(p, tok_t::lparen, "expected '(' after " + quoted(keyword.text));

    const token_t &first = peek(p);
    if (first.kind != tok_t::string)
        fail(first, "expected string literal in " + quoted(keyword.text));

    std::string name;
    while (peek(p).kind == tok_t::string)
        name += string_bytes(advance(p));

    if (name.empty())
        fail(first, "empty assembler name");

    expect(p, tok_t::rparen, "expected ')' after assembler name");

    return name;
}

// What GNU C writes after a declarator, before the comma, the semicolon
// or the initializer that follow it: attributes, and an assembler name
// among them.  asm_keyword is the __asm__ written, or null, and marking
// the name's token of the first marking attribute, or null.
struct declarator_tail_t
{
    const token_t *asm_keyword;
    std::string asm_name;
    const token_t *marking;
};

declarator_tail_t
declarator_tail(parser_t &p)
{
    declarator_tail_t tail { nullptr, { }, attribute_specifier_list(p) };

    if (peek(p).kind == tok_t::kw_asm) {
        tail.asm_keyword = &peek(p);
        tail.asm_name = asm_label(p);
    }

    const token_t *marking = attribute_specifier_list(p);
    if (!tail.marking)
        tail.marking = marking;

    return tail;
}

// A function or an object declared extern takes the tail of each of
// its declarations.  The assembler name of one holds for the others,
// and two that differ name two symbols for one name, an error.  A
// marking attribute marks the symbol, and every use of it after the
// declaration is refused.
void
record_tail(symbol_t &sym, const declarator_tail_t &tail)
{
    if (tail.asm_keyword) {
        if (!sym.asm_name.empty() && sym.asm_name != tail.asm_name)
            fail(*tail.asm_keyword,
                 "conflicting assembler names for " + quoted(sym.name));

        sym.asm_name = tail.asm_name;
    }

    if (tail.marking && sym.marking.empty())
        sym.marking = attribute_words(*tail.marking);
}

// A function specifier stands only in the declaration of a function,
// 6.7.5p2.  On a declaration it is dropped: inline suggests that calls
// be fast, p6, and _Noreturn promises that the function does not
// return, p8, and rcc writes each call as it is written either way.
void
refuse_function_specifier(const specifiers_t &spec)
{
    if (const token_t *t = spec.function_specifier)
        fail(*t, quoted(t->text) + " can only appear on functions");
}

// __extension__ before a declaration keeps GNU C from warning of the
// extensions in it, and means nothing else.
void
skip_extension_keywords(parser_t &p)
{
    while (accept(p, tok_t::kw_extension)) {
    }
}

//
// the productions
//

void external_declaration(parser_t &);
void function_definition(parser_t &, const declarator_t &);
specifiers_t declaration_specifiers(parser_t &);
type_t struct_or_union_specifier(parser_t &);
type_t enum_specifier(parser_t &);
void enumerator_list(parser_t &, const type_t &);
declarator_t declarator(parser_t &, const type_t &, bool abstract = false);
type_t pointer(parser_t &, type_t);
type_t array_declarator(parser_t &, const type_t &);
bool parameter_type_list(parser_t &, std::vector< parameter_t > &);
parameter_t parameter_declaration(parser_t &);
type_t type_name(parser_t &);
std::vector< stmt_t > declaration(parser_t &);
std::vector< stmt_t > init_declarator_list(parser_t &, const specifiers_t &);
stmt_t init_declarator(parser_t &, const specifiers_t &);

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
expr_t sizeof_expression(parser_t &);
expr_t postfix_expression(parser_t &);
std::vector< expr_t > argument_expression_list(parser_t &, const symbol_t &);
expr_t primary_expression(parser_t &);

//
// declarations
//

// Whether a declaration starts at the token ahead: a typedef name in
// scope, or a keyword a declaration can start with, those outside the
// subset included, so that they are refused by name and not as an
// expression; either may follow __extension__.  A typedef name before a
// colon is a label, since labels have a name space of their own.
bool
is_declaration_start(const parser_t &p, std::size_t ahead = 0)
{
    const token_t &t = peek(p, ahead);
    if (t.kind == tok_t::identifier)
        return names_type(p, t) && peek(p, ahead + 1).kind != tok_t::colon;

    if (t.kind == tok_t::kw_extension)
        return is_declaration_start(p, ahead + 1);

    switch (t.kind) {
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
    case tok_t::kw_enum:
    case tok_t::kw_inline:
    case tok_t::kw_noreturn:
    case tok_t::kw_bool:
    case tok_t::kw_complex:
    case tok_t::kw_imaginary:
    case tok_t::kw_atomic:
    case tok_t::kw_alignas:
    case tok_t::kw_thread_local:
    case tok_t::kw_static_assert: return true;
    default: return false;
    }
}

bool
declared(const specifiers_t &spec, tok_t storage_class)
{
    return spec.storage_class && spec.storage_class->kind == storage_class;
}

void
translation_unit(parser_t &p)
{
    while (peek(p).kind != tok_t::eof)
        external_declaration(p);
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

// The declaration of the name earlier in the current scope, or null
// when there is none.  A name declared again there is the same kind of
// entity, of the same type: a function or an object of file scope by
// the rule of compatible types (6.2.7p2), and a typedef because C11
// lets a typedef be declared again only as the type it already names.
symbol_t *
earlier_declaration(const parser_t &p,
                    const token_t &name,
                    const type_t &t,
                    storage_t storage)
{
    const auto &scope = p.scopes.back().names;

    const auto it = scope.find(name.text);
    if (it == scope.end())
        return nullptr;

    symbol_t &earlier = *it->second;
    if (earlier.storage != storage)
        fail(name, "redefinition of " + quoted(name.text));

    if (earlier.type == t)
        return &earlier;

    if (storage == storage_t::type_name)
        fail(name, "redefinition of typedef " + quoted(name.text) +
                       " with another type");

    fail(name, "conflicting types for " + quoted(name.text));
}

// A new symbol in the current scope, in no register.  The map's key
// views the symbol's own name, which the deque keeps in place.
symbol_t &
added(parser_t &p, std::string_view name, type_t t, storage_t storage)
{
    symbol_t &sym = p.unit.symbols.emplace_back(
        symbol_t { std::string(name), std::move(t), storage, false, -1 });
    p.scopes.back().names.emplace(sym.name, &sym);

    return sym;
}

// A typedef declares its declarator's name as a name for the type the
// declarator gives, in the current scope.  The name has no object, so
// nothing is generated for it, and it takes no initializer and no
// assembler name.  The type may be a function type, whose parameters
// share one scope as a prototype's do.  Through a marking attribute the
// name stands for a not-yet type of its own, of the attribute's words:
// typedef int register_t __attribute__ ((__mode__ (__word__))) names an
// integer of a size rcc does not take, and its objects are refused.
void
type_definition(parser_t &p, const declarator_t &d)
{
    if (d.params)
        check_parameter_names(d);

    const declarator_tail_t tail = declarator_tail(p);
    if (tail.asm_keyword)
        fail(*tail.asm_keyword, quoted(tail.asm_keyword->text) +
                                    " on typedef " + quoted(d.name->text));

    if (peek(p).kind == tok_t::assign)
        fail(peek(p),
             "typedef " + quoted(d.name->text) + " cannot have an initializer");

    type_t t = d.type;
    if (tail.marking)
        t = make_not_yet(p.identities++, attribute_words(*tail.marking));

    if (!earlier_declaration(p, *d.name, t, storage_t::type_name))
        added(p, d.name->text, std::move(t), storage_t::type_name);
}

// Declares a function at file scope, or meets its earlier declaration,
// with what the declaration writes after its declarator.  Returns the
// index of its entry in the unit's functions.
std::size_t
declare_function(parser_t &p,
                 const declarator_t &d,
                 const declarator_tail_t &tail)
{
    check_parameter_names(d);

    symbol_t *earlier =
        earlier_declaration(p, *d.name, d.type, storage_t::function);

    if (earlier) {
        record_tail(*earlier, tail);

        for (std::size_t i = 0; i < p.unit.functions.size(); ++i)
            if (p.unit.functions[i].sym == earlier)
                return i;
    }

    symbol_t &sym = added(p, d.name->text, d.type, storage_t::function);
    record_tail(sym, tail);
    p.unit.functions.push_back(function_decl_t { &sym, { }, std::nullopt, 0 });

    return p.unit.functions.size() - 1;
}

// An object declared extern at file scope without an initializer is
// defined elsewhere: its symbol has storage global, and c1 writes
// nothing for it.  Any other object at file scope is a definition,
// which needs the object's storage, and is not yet.  A use of the
// object is refused where the object is named.
void
declare_external_object(parser_t &p,
                        const specifiers_t &spec,
                        const declarator_t &d)
{
    const declarator_tail_t tail = declarator_tail(p);

    if (!declared(spec, tok_t::kw_extern) || peek(p).kind == tok_t::assign)
        not_yet(*d.name, "global variable");

    symbol_t *sym = earlier_declaration(p, *d.name, d.type, storage_t::global);
    if (!sym)
        sym = &added(p, d.name->text, d.type, storage_t::global);

    record_tail(*sym, tail);
}

// One declarator of a file-scope declaration: a type name, a function,
// or an object.  No function specifier stands in a declaration of
// main, 6.7.5p4.
void
external_declarator(parser_t &p,
                    const specifiers_t &spec,
                    const declarator_t &d)
{
    const bool typedef_ = declared(spec, tok_t::kw_typedef);
    if (typedef_ || !d.params)
        refuse_function_specifier(spec);

    const token_t *specifier = spec.function_specifier;
    if (specifier && d.name->text == "main")
        fail(*specifier,
             quoted(specifier->text) + " in a declaration of 'main'");

    if (typedef_)
        type_definition(p, d);
    else if (d.params)
        declare_function(p, d, declarator_tail(p));
    else
        declare_external_object(p, spec, d);
}

// A function definition, or a declaration of functions, of objects
// defined elsewhere and of type names, or of a tag or enumeration
// constants alone.  extern on a function changes nothing: a function
// has external linkage without it.  A definition is a declarator and
// its body, with nothing between them, so no attribute and no
// assembler name of its own; it takes those of its declarations.
// inline on a definition is not yet: an inline definition is no
// external definition, 6.7.5p7, which rcc would have to honour.
// _Noreturn on one is dropped, as on a declaration: it promises that
// the function does not return, and rcc writes the function as it is
// written either way.  An __asm__ at file scope is not yet.
void
external_declaration(parser_t &p)
{
    if (peek(p).kind == tok_t::kw_asm)
        not_yet(peek(p), quoted(peek(p).text) + " at file scope");

    skip_extension_keywords(p);

    const specifiers_t spec = declaration_specifiers(p);

    // 6.9.1p2: register shall not appear in an external declaration
    if (declared(spec, tok_t::kw_register))
        fail(*spec.storage_class,
             quoted(spec.storage_class->text) + " in an external declaration");

    if (spec.tagged && accept(p, tok_t::semicolon))
        return;

    const type_t &base = spec.type;

    const declarator_t d = declarator(p, base);

    const bool defines_function = d.params && peek(p).kind == tok_t::lbrace;
    if (defines_function && !declared(spec, tok_t::kw_typedef)) {
        const token_t *t = spec.function_specifier;
        if (t && t->kind == tok_t::kw_inline)
            not_yet(*t, quoted(t->text) + " on a function definition");

        function_definition(p, d);
        return;
    }

    external_declarator(p, spec, d);

    while (accept(p, tok_t::comma))
        external_declarator(p, spec, declarator(p, base));

    expect(p, tok_t::semicolon, "expected ';' after top level declarator");
}

// The parameters and the outermost block of the body are one block,
// 6.8.3p2, so a local cannot redeclare a parameter there; the body's
// items are parsed into the parameter scope directly.  The result and
// the parameters are objects the definition makes, so none may be of a
// not-yet type.  A function a marking attribute marked is one rcc would
// have to define as the attribute says, which is not yet.
void
function_definition(parser_t &p, const declarator_t &d)
{
    const std::size_t index = declare_function(p, d, { });
    if (p.unit.functions[index].body)
        fail(*d.name, "redefinition of " + quoted(d.name->text));

    const symbol_t &sym = *p.unit.functions[index].sym;
    if (!sym.marking.empty())
        not_yet(*d.name, sym.marking);

    p.result = *as< function_t >(d.type).result;
    require_honoured(*d.name, p.result);

    p.registers = 0;
    p.scopes.emplace_back();

    std::vector< const symbol_t * > params;
    for (const parameter_t &param : *d.params) {
        if (!param.name)
            fail(*param.start, "parameter name omitted");

        require_honoured(*param.name, param.type);
        params.push_back(declare(p, *param.name, param.type,
                                 storage_t::parameter, param.in_register));
    }

    block_t body;
    expect(p, tok_t::lbrace, "expected '{' before function body");
    block_item_list(p, body);
    expect(p, tok_t::rbrace, "expected '}' at end of function body");

    p.scopes.pop_back();

    function_decl_t &f = p.unit.functions[index];
    f.params = std::move(params);
    f.body = std::move(body);
    f.registers = p.registers;
}

// How a struct, union or enum specifier uses its tag, 6.7.3.4: a list
// defines the tag's content (p10); struct S; declares the tag alone
// (p11); any other mention refers to the type the tag names (p12, p13).
enum class tag_use_t { defines, declares, refers };

// A new struct, union or enum type, of an identity of its own: an
// untagged specifier makes one each time, 6.7.3.4p9.
type_t
new_tag_type(parser_t &p, const token_t &keyword)
{
    const std::string_view what = keyword.kind == tok_t::kw_struct ? "struct" :
                                  keyword.kind == tok_t::kw_union  ? "union" :
                                                                     "enum";

    return make_not_yet(p.identities++, std::string(what));
}

// The tag the scope declares by the name, or null.
tag_t *
tag_in(scope_t &scope, std::string_view name)
{
    const auto it = scope.tags.find(name);

    return it == scope.tags.end() ? nullptr : &it->second;
}

// The type a tag names, 6.7.3.4.  A list and struct S; declare the tag
// in the current scope (p10, p11), and a tag declared there already is
// the same type, p4.  Any other mention is the type of the innermost
// declaration of the tag in scope, p13, or without one declares an
// incomplete struct or union in the current scope, p12.  The tag of
// another kind of type is an error, p1, and so is an enum without its
// list before the enum is declared, p2.  A second list for one tag in
// one scope is valid when it defines the same content, p1, which rcc
// cannot compare while it skips the lists, so it is not yet.  A tag
// declared in a parameter list is declared in the scope around the
// declarator, which is not the prototype scope of 6.2.1p4.
type_t
tagged_type(parser_t &p,
            const token_t &keyword,
            const token_t &tag,
            tag_use_t use)
{
    tag_t *found = tag_in(p.scopes.back(), tag.text);

    if (use == tag_use_t::refers)
        for (auto scope = p.scopes.rbegin(); !found && scope != p.scopes.rend();
             ++scope)
            found = tag_in(*scope, tag.text);

    if (found && found->keyword != keyword.kind)
        fail(tag,
             "use of " + quoted(tag.text) +
                 " with tag type that does not match previous declaration");

    if (!found && use == tag_use_t::refers && keyword.kind == tok_t::kw_enum)
        fail(tag, "enum " + quoted(tag.text) + " used before its list");

    if (found && use == tag_use_t::defines && found->defined)
        not_yet(tag, std::string(keyword.text) + " " + quoted(tag.text) +
                         " defined again");

    if (!found) {
        tag_t fresh { new_tag_type(p, keyword), keyword.kind, false };
        found = &p.scopes.back().tags.emplace(tag.text, fresh).first->second;
    }

    if (use == tag_use_t::defines)
        found->defined = true;

    return found->type;
}

// The tag after struct, union or enum, or null where the list follows
// at once; a specifier with neither is an error.
const token_t *
optional_tag(parser_t &p, const token_t &keyword)
{
    if (peek(p).kind == tok_t::lbrace)
        return nullptr;

    return &expect(p, tok_t::identifier,
                   "expected identifier or '{' after " + quoted(keyword.text));
}

// The member list of a struct or union, skipped up to its closing
// brace, which is left for the caller, with the braces nested in it
// counted.  The members are not yet, and nothing reads them.
void
skip_member_list(parser_t &p)
{
    for (int depth = 0; peek(p).kind != tok_t::eof; advance(p)) {
        const tok_t kind = peek(p).kind;
        if (kind == tok_t::rbrace && depth == 0)
            return;

        if (kind == tok_t::lbrace)
            ++depth;
        else if (kind == tok_t::rbrace)
            --depth;
    }
}

// The value of an enumeration constant, skipped up to the comma or the
// closing brace at the list's own depth, which is left for the caller:
// a comma inside parentheses, brackets or braces is the expression's.
// The value is not computed yet.
void
skip_enumerator_value(parser_t &p)
{
    for (int depth = 0; peek(p).kind != tok_t::eof; advance(p)) {
        switch (peek(p).kind) {
        case tok_t::lparen:
        case tok_t::lbracket:
        case tok_t::lbrace: ++depth; break;
        case tok_t::rparen:
        case tok_t::rbracket:
            if (depth > 0)
                --depth;
            break;
        case tok_t::rbrace:
            if (depth == 0)
                return;
            --depth;
            break;
        case tok_t::comma:
            if (depth == 0)
                return;
            break;
        default: break;
        }
    }
}

// The type specifiers in any valid order and combination, 6.7.3.1p2.
// The tree keeps no spelling: char and signed char are one type, long
// and long long another.  Each keyword is checked against the ones
// before it, so an error names the keyword that breaks the set.  const
// may stand anywhere among them, once or more, 6.7.4.1p6, and is
// dropped: the type is the machine's view.  A storage-class specifier,
// register, typedef or extern, may stand anywhere too, one of them,
// 6.7.2p2, and is reported to the caller, since what it may declare
// depends on where the declaration is.  A typedef name is the type
// specifier where no type specifier has been seen; after one, an
// identifier is the declarator's name, so unsigned T declares a T.  A
// typedef name, a struct, union or enum specifier, and float are each
// the whole type specifier; double joins one long.  restrict may stand
// among them too, on a type require_restrictable() takes, and is
// dropped.  So is a function specifier, inline or _Noreturn, once or
// more, 6.7.5p5, reported to the caller, which knows whether the
// declaration is of a function.  The keywords of C outside the subset
// are refused by name where they stand.
specifiers_t
declaration_specifiers(parser_t &p)
{
    int n_void = 0, n_char = 0, n_short = 0, n_int = 0, n_long = 0;
    int n_signed = 0, n_unsigned = 0, n_float = 0, n_double = 0;
    bool any = false;

    std::optional< type_t > whole;
    bool tagged = false;
    const token_t *storage_class = nullptr;
    const token_t *restrict_qualifier = nullptr;
    const token_t *function_specifier = nullptr;

    // the type the specifiers name, once they end
    const auto specified = [&]() -> type_t {
        if (whole)
            return *whole;

        if (n_void)
            return make_void();

        if (n_float)
            return float_type();

        if (n_double)
            return n_long ? long_double_type() : double_type();

        if (n_char && !n_signed && !n_unsigned)
            return char_type(p);

        return make_int(n_char  ? 8 :
                        n_short ? 16 :
                        n_long  ? 64 :
                                  32,
                        n_unsigned == 0);
    };

    for (;;) {
        const token_t &t = peek(p);

        if (accept(p, tok_t::kw_const))
            continue;

        if (t.kind == tok_t::kw_restrict) {
            restrict_qualifier = &advance(p);
            continue;
        }

        if (t.kind == tok_t::kw_inline || t.kind == tok_t::kw_noreturn) {
            if (!function_specifier)
                function_specifier = &t;

            advance(p);
            continue;
        }

        if (t.kind == tok_t::kw_register || t.kind == tok_t::kw_typedef ||
            t.kind == tok_t::kw_extern) {
            // 6.7.2p2: at most one storage-class specifier
            if (storage_class)
                fail(t, "cannot combine " + quoted(t.text) +
                            " with the storage class before it");

            storage_class = &t;
            advance(p);
            continue;
        }

        if (!any && names_type(p, t)) {
            whole = named_type(p, t);

            // a function declared through it, or a pointer to one, would
            // need function designators as values
            if (is< function_t >(*whole))
                not_yet(t,
                        "typedef name " + quoted(t.text) + " of function type");

            advance(p);
            any = true;
            continue;
        }

        bool bad = n_void > 0 || whole || n_float > 0;
        switch (t.kind) {
        case tok_t::kw_void:
            bad = bad || n_char || n_short || n_int || n_long || n_signed ||
                  n_unsigned || n_double;
            ++n_void;
            break;
        case tok_t::kw_char:
            bad = bad || n_char || n_short || n_int || n_long || n_double;
            ++n_char;
            break;
        case tok_t::kw_short:
            bad = bad || n_char || n_short || n_long || n_double;
            ++n_short;
            break;
        case tok_t::kw_int:
            bad = bad || n_char || n_int || n_double;
            ++n_int;
            break;
        case tok_t::kw_long:
            bad =
                bad || n_char || n_short || n_long == 2 || (n_long && n_double);
            ++n_long;
            break;
        case tok_t::kw_signed:
        case tok_t::kw_unsigned:
            bad = bad || n_signed || n_unsigned || n_double;
            ++(t.kind == tok_t::kw_signed ? n_signed : n_unsigned);
            break;
        case tok_t::kw_float:
            bad = any;
            ++n_float;
            break;
        case tok_t::kw_double:
            bad = bad || n_char || n_short || n_int || n_long == 2 ||
                  n_signed || n_unsigned || n_double;
            ++n_double;
            break;
        case tok_t::kw_struct:
        case tok_t::kw_union:
        case tok_t::kw_enum:
            if (any) {
                bad = true;
                break;
            }

            whole = t.kind == tok_t::kw_enum ? enum_specifier(p) :
                                               struct_or_union_specifier(p);
            tagged = true;
            any = true;
            continue;
        case tok_t::kw_bool:
        case tok_t::kw_complex:
        case tok_t::kw_imaginary:
            not_yet(t, "type specifier " + quoted(t.text));
        case tok_t::kw_volatile:
        case tok_t::kw_atomic: not_yet(t, "type qualifier " + quoted(t.text));
        case tok_t::kw_auto:
        case tok_t::kw_static:
        case tok_t::kw_thread_local:
            not_yet(t, "storage class " + quoted(t.text));
        case tok_t::kw_alignas:
            not_yet(t, "alignment specifier " + quoted(t.text));
        // a declaration of its own, refused where a declaration starts
        case tok_t::kw_static_assert:
            not_yet(t, quoted(t.text) + " declaration");
        default: {
            if (!any)
                fail(t, "expected a type specifier");

            const type_t type = specified();
            if (restrict_qualifier)
                require_restrictable(*restrict_qualifier, type);

            return { type, storage_class, tagged, function_specifier };
        }
        }

        if (bad)
            fail(t, "cannot combine " + quoted(t.text) +
                        " with the type specifiers before it");

        advance(p);
        any = true;
    }
}

// struct or union, then a tag, a member list in braces, or both,
// 6.7.3.2p1.  The type is a not-yet type: the members are skipped,
// and nothing that needs the object is taken.  A tag followed by ; is
// the declaration of the tag alone, 6.7.3.4p11.  Attributes after the
// keyword and after the list are dropped, of either kind, and so are
// those the skipped members carry: the type's size and layout are not
// yet, so packed and aligned change nothing rcc reads, and the slice
// that lays the type out will have to honour them.
type_t
struct_or_union_specifier(parser_t &p)
{
    const token_t &keyword = advance(p);
    attribute_specifier_list(p);

    const token_t *tag = optional_tag(p, keyword);

    if (peek(p).kind != tok_t::lbrace) {
        const tag_use_t use = peek(p).kind == tok_t::semicolon ?
                                  tag_use_t::declares :
                                  tag_use_t::refers;

        return tagged_type(p, keyword, *tag, use);
    }

    const type_t t = tag ? tagged_type(p, keyword, *tag, tag_use_t::defines) :
                           new_tag_type(p, keyword);

    advance(p);
    skip_member_list(p);
    expect(p, tok_t::rbrace, "expected '}' at end of member list");
    attribute_specifier_list(p);

    return t;
}

// enum, then a tag, an enumerator list in braces, or both, 6.7.3.3p1.
// The type is a not-yet type, and its enumeration constants are
// declared for what they are.  Attributes after the keyword and after
// the list are dropped, as a struct's are: packed, which would change
// the enum's size, waits for the size.
type_t
enum_specifier(parser_t &p)
{
    const token_t &keyword = advance(p);
    attribute_specifier_list(p);

    const token_t *tag = optional_tag(p, keyword);

    if (peek(p).kind != tok_t::lbrace)
        return tagged_type(p, keyword, *tag, tag_use_t::refers);

    const type_t t = tag ? tagged_type(p, keyword, *tag, tag_use_t::defines) :
                           new_tag_type(p, keyword);

    advance(p);
    enumerator_list(p, t);
    expect(p, tok_t::rbrace, "expected '}' at end of enumerator list");
    attribute_specifier_list(p);

    return t;
}

// One or more enumerators, the list perhaps ending in a comma,
// 6.7.3.3p1.  Each declares its enumeration constant as an ordinary
// identifier, 6.2.3p1, in the current scope, once there, from the end
// of its enumerator, 6.2.1p7.  The constant's type here is the
// enumeration's, and its value, the tokens after =, is skipped: its use
// is refused where it is named.
void
enumerator_list(parser_t &p, const type_t &t)
{
    do {
        const token_t &name =
            expect(p, tok_t::identifier, "expected identifier");
        if (p.scopes.back().names.contains(name.text))
            fail(name, "redefinition of " + quoted(name.text));

        if (accept(p, tok_t::assign)) {
            if (next_is(p, { tok_t::comma, tok_t::rbrace }))
                fail(peek(p), "expected expression");

            skip_enumerator_value(p);
        }

        added(p, name.text, t, storage_t::enumerator);
    } while (accept(p, tok_t::comma) && peek(p).kind != tok_t::rbrace);
}

// The pointers, then an identifier, then array suffixes or a parameter
// list; the name is left out only in a parameter, which may be
// abstract.  The pointers bind to the base type, so int *f(void) is a
// function returning a pointer.  An array of functions and a function
// returning an array or a function are errors, 6.7.7.3p1, 6.7.7.4p1;
// a parenthesized declarator, which would let the two nest the other
// way, is not yet.
declarator_t
declarator(parser_t &p, const type_t &base, bool abstract)
{
    const type_t t = pointer(p, base);

    if (peek(p).kind == tok_t::lparen)
        not_yet(peek(p), "parenthesized declarator");

    const token_t *name = nullptr;
    if (!abstract || peek(p).kind == tok_t::identifier)
        name = &expect(p, tok_t::identifier, "expected identifier");

    declarator_t d { name, t, std::nullopt };

    if (peek(p).kind == tok_t::lbracket) {
        d.type = array_declarator(p, t);

        if (peek(p).kind == tok_t::lparen)
            fail(peek(p), "array element type cannot be a function");
    }
    else if (peek(p).kind == tok_t::lparen) {
        const token_t &open = advance(p);

        std::vector< parameter_t > params;
        const bool variadic = parameter_type_list(p, params);

        if (peek(p).kind == tok_t::lbracket)
            fail(peek(p), "function cannot return an array");

        if (peek(p).kind == tok_t::lparen)
            fail(peek(p), "function cannot return a function");

        // an array result through a typedef name, which no suffix shows
        if (is< array_t >(t))
            fail(open, "function cannot return an array");

        std::vector< type_t > types;
        for (const parameter_t &param : params)
            types.push_back(param.type);

        d.type = make_function(t, std::move(types), variadic);
        d.params = std::move(params);
    }

    return d;
}

// Each '*' derives a pointer to the type so far, 6.7.7.2p1.  A const or
// a restrict after it qualifies the pointer and is dropped, as in the
// specifiers; the pointer is one restrict may qualify.
type_t
pointer(parser_t &p, type_t t)
{
    while (accept(p, tok_t::star)) {
        t = make_pointer(std::move(t));

        for (;;) {
            const token_t &q = peek(p);
            if (q.kind == tok_t::kw_volatile || q.kind == tok_t::kw_atomic)
                not_yet(q, "type qualifier " + quoted(q.text));

            if (!accept(p, tok_t::kw_const) && !accept(p, tok_t::kw_restrict))
                break;
        }
    }

    return t;
}

// The count of an array, from its size: an expression of integer type,
// 6.7.7.3p1, which for an array that is not a variable length array is
// an integer constant expression, p4, folded by now to a literal; any
// other size makes a variable length array.  A constant size is
// greater than zero, p1.  An unsigned size past LONG_MAX is LONG_MAX,
// which no element size lets through as an array's size.
long
array_count(const token_t &at, const expr_t &size)
{
    if (!is< int_t >(size.type))
        fail(at, "size of array has non-integer type");

    if (!is< literal_t >(size))
        not_yet(at, "variable length array");

    const exact_t value = exact_value(size);
    if (value <= 0)
        fail(at, "array size must be greater than zero");

    return long(std::min< exact_t >(value, std::numeric_limits< long >::max()));
}

// One or more [N], N an integer constant expression, or [] for an array
// of unknown size, 6.7.7.3p1, p4.  The size is an assignment-expression
// in the grammar, 6.7.7.1p1.  The suffixes nest from the right: int
// a[2][3] is an array of two arrays of three ints.  An element must be
// a complete type, so only the first suffix may leave its size out, and
// the caller decides whether it may.  An array of a not-yet type has no
// size rcc knows, so its size is not checked; its object is refused
// where it would be made.
type_t
array_declarator(parser_t &p, const type_t &element)
{
    std::vector< std::pair< const token_t *, long > > suffixes;

    while (peek(p).kind == tok_t::lbracket) {
        const token_t &open = advance(p);

        long count = -1;
        if (peek(p).kind != tok_t::rbracket) {
            const token_t &start = peek(p);

            const expr_t size = rvalue(start, assignment_expression(p));
            count = array_count(start, size);
        }

        expect(p, tok_t::rbracket, "expected ']'");
        suffixes.emplace_back(&open, count);
    }

    type_t t = element;

    for (auto it = suffixes.rbegin(); it != suffixes.rend(); ++it) {
        const auto [open, count] = *it;

        if (is< void_t >(t) || (is< array_t >(t) && as< array_t >(t).count < 0))
            fail(*open, "array has incomplete element type");

        if (count >= 0 && !not_yet_in(t) &&
            count > std::numeric_limits< long >::max() / size_of(t))
            fail(*open, "array is too large");

        t = make_array(std::move(t), count);
    }

    return t;
}

// (void), (), or parameters with an optional trailing ", ...", which
// makes the function variadic, the value returned.  An empty list is
// (void) in C23, 6.7.7.4p13; before C23 it declared a function without
// a prototype, which this parser does not take.
bool
parameter_type_list(parser_t &p, std::vector< parameter_t > &params)
{
    if (accept(p, tok_t::rparen))
        return false;

    if (peek(p).kind == tok_t::kw_void && peek(p, 1).kind == tok_t::rparen) {
        advance(p);
        advance(p);
        return false;
    }

    bool variadic = false;

    do {
        if (peek(p).kind == tok_t::ellipsis) {
            if (params.empty())
                not_yet(peek(p), "variadic function without named parameters");

            advance(p);
            variadic = true;
            break;
        }

        params.push_back(parameter_declaration(p));
    } while (accept(p, tok_t::comma));

    expect(p, tok_t::rparen, "expected ')' after parameter list");

    return variadic;
}

// Specifiers and a declarator, whose name is optional.  void stands
// only alone, as (void), 6.7.7.4p9.  A parameter of array type is
// adjusted to a pointer to the element, 6.7.7.4p6, so its size may be
// left out; one of function type would be adjusted to a function
// pointer, p7, which is not yet.
parameter_t
parameter_declaration(parser_t &p)
{
    const token_t &start = peek(p);

    const specifiers_t spec = declaration_specifiers(p);

    // register is the one storage class a parameter takes
    if (spec.storage_class && !declared(spec, tok_t::kw_register))
        fail(*spec.storage_class,
             quoted(spec.storage_class->text) + " in a parameter declaration");

    refuse_function_specifier(spec);

    const declarator_t d = declarator(p, spec.type, true);

    if (is< void_t >(d.type))
        fail(start, "'void' must be the only parameter");

    if (d.params)
        not_yet(d.name ? *d.name : start, "parameter of function type");

    // a register parameter of array type is a register pointer, since
    // the adjustment comes first
    type_t t = d.type;
    if (is< array_t >(t))
        t = make_pointer(*as< array_t >(t).element);

    return parameter_t { std::move(t), &start, d.name,
                         declared(spec, tok_t::kw_register) };
}

// Specifiers and an abstract declarator of '*'s, each perhaps const,
// 6.7.8p1; the array and function abstract declarators are not yet.
type_t
type_name(parser_t &p)
{
    const specifiers_t spec = declaration_specifiers(p);

    // 6.7.8p1: a specifier-qualifier list, no storage class
    if (spec.storage_class)
        fail(*spec.storage_class,
             quoted(spec.storage_class->text) + " in a type name");

    refuse_function_specifier(spec);

    const type_t t = pointer(p, spec.type);

    if (peek(p).kind == tok_t::lbracket)
        not_yet(peek(p), "array type name");

    if (peek(p).kind == tok_t::lparen)
        not_yet(peek(p), "parenthesized or function type name");

    return t;
}

// A block-scope declaration, perhaps of a tag or enumeration constants
// alone, which makes no statement.  extern in a block is not yet.
std::vector< stmt_t >
declaration(parser_t &p)
{
    skip_extension_keywords(p);

    const specifiers_t spec = declaration_specifiers(p);

    if (declared(spec, tok_t::kw_extern))
        not_yet(*spec.storage_class,
                "storage class " + quoted(spec.storage_class->text));

    if (spec.tagged && accept(p, tok_t::semicolon))
        return { };

    return init_declarator_list(p, spec);
}

// The declarators of a block-scope declaration and its ';': one
// declaration statement per object, in order.  A typedef's declarators
// declare type names and make no statement.
std::vector< stmt_t >
init_declarator_list(parser_t &p, const specifiers_t &spec)
{
    std::vector< stmt_t > decls;

    do {
        if (declared(spec, tok_t::kw_typedef)) {
            refuse_function_specifier(spec);
            type_definition(p, declarator(p, spec.type));
        }
        else
            decls.push_back(init_declarator(p, spec));
    } while (accept(p, tok_t::comma));

    expect(p, tok_t::semicolon, "expected ';' at end of declaration");

    return decls;
}

// The name is in scope from the end of its declarator, 6.2.1p7, so an
// initializer sees the object it initializes.  The initializer is
// converted as by assignment, 6.7.11p12.  An object's type must be
// complete, 6.7.1p8, so an array needs its size; any initializer of an
// array, and a brace initializer of anything, is not yet.  A register
// array is not yet either: its decay is undefined, 6.3.2.1p3, and it
// has no slot to decay to.  A local of 2 GiB or more is refused here,
// where its position is known, since no frame holds it, and so is one
// of a not-yet type, whose size rcc does not know.  A local is an
// object rcc makes, so an attribute that marks it is one rcc would have
// to honour, and is not yet; so is an assembler name, which names a
// register for a register variable.
stmt_t
init_declarator(parser_t &p, const specifiers_t &spec)
{
    const declarator_t d = declarator(p, spec.type);
    if (d.params)
        not_yet(*d.name, "function declaration at block scope");

    refuse_function_specifier(spec);

    const declarator_tail_t tail = declarator_tail(p);
    if (tail.asm_keyword)
        not_yet(*tail.asm_keyword,
                quoted(tail.asm_keyword->text) + " on a local variable");

    if (tail.marking)
        not_yet(*tail.marking, attribute_words(*tail.marking));

    if (is< void_t >(d.type))
        fail(*d.name, "variable " + quoted(d.name->text) + " has type void");

    require_honoured(*d.name, d.type);

    if (is< array_t >(d.type) && declared(spec, tok_t::kw_register))
        not_yet(*d.name, "register array " + quoted(d.name->text));

    if (is< array_t >(d.type) && peek(p).kind == tok_t::assign)
        not_yet(peek(p), "array initializer");

    if (is< array_t >(d.type) && as< array_t >(d.type).count < 0)
        fail(*d.name, "array size missing in " + quoted(d.name->text));

    if (size_of(d.type) > p.target.frame_limit)
        fail(*d.name,
             "variable " + quoted(d.name->text) + " is too large for a local");

    decl_stmt_t decl { declare(p, *d.name, d.type, storage_t::local,
                               declared(spec, tok_t::kw_register)),
                       std::nullopt };

    if (peek(p).kind == tok_t::assign) {
        const token_t &op = advance(p);

        if (peek(p).kind == tok_t::lbrace)
            not_yet(peek(p), "brace initializer");

        expr_t init = assignment_expression(p);
        decl.init = assignment_conversion(op, std::move(init), d.type);
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
    case tok_t::kw_asm: not_yet(t, quoted(t.text) + " statement");
    default: break;
    }

    if (t.kind == tok_t::identifier && peek(p, 1).kind == tok_t::colon)
        not_yet(t, "label");

    if (is_declaration_start(p))
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
        if (is_declaration_start(p)) {
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

    return stmt_t { expr_stmt_t { decayed(std::move(e)) } };
}

// The condition stays in its own type, an integer or a pointer; c1
// compares it against zero, 6.8.5.2p2.
stmt_t
selection_statement(parser_t &p)
{
    const token_t &keyword = advance(p);
    if (keyword.kind == tok_t::kw_switch)
        not_yet(keyword, "switch statement");

    expect(p, tok_t::lparen, "expected '(' after 'if'");

    const token_t &start = peek(p);
    expr_t cond = rvalue(start, expression(p));

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
    expr_t cond = rvalue(start, expression(p));

    expect(p, tok_t::rparen, "expected ')' after condition");

    stmt_ptr_t body = box(statement(p));

    return stmt_t { while_t { std::move(cond), std::move(body) } };
}

// A declaration in the init is in scope for the whole loop, 6.8.6.4p1,
// so the for opens a scope; a braced body nests inside it.  The init
// and the step are evaluated as void expressions there.  Several
// declarators in the init become one block of declaration statements,
// since the tree holds a single init statement.  The init declares
// objects only, so neither typedef nor extern is a storage class of it.
stmt_t
for_statement(parser_t &p)
{
    advance(p);
    expect(p, tok_t::lparen, "expected '(' after 'for'");

    p.scopes.emplace_back();

    for_t loop;

    if (is_declaration_start(p)) {
        skip_extension_keywords(p);

        const specifiers_t spec = declaration_specifiers(p);
        if (spec.storage_class && !declared(spec, tok_t::kw_register))
            fail(*spec.storage_class, quoted(spec.storage_class->text) +
                                          " in the declaration of a 'for'");

        std::vector< stmt_t > decls = init_declarator_list(p, spec);
        if (decls.size() == 1)
            loop.init = box(std::move(decls.front()));
        else
            loop.init = box(stmt_t { block_t { std::move(decls) } });
    }
    else if (!accept(p, tok_t::semicolon))
        loop.init = box(expression_statement(p));

    if (peek(p).kind != tok_t::semicolon) {
        const token_t &start = peek(p);
        loop.cond = rvalue(start, expression(p));
    }

    expect(p, tok_t::semicolon, "expected ';' after for condition");

    if (peek(p).kind != tok_t::rparen) {
        const token_t &start = peek(p);
        expr_t step = expression(p);
        require_not_function(start, step);

        loop.step = decayed(std::move(step));
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

        ret.value = assignment_conversion(start, std::move(value), p.result);
    }
    else if (!is< void_t >(p.result))
        fail(keyword, "non-void function should return a value");

    expect(p, tok_t::semicolon, "expected ';' after return statement");

    return stmt_t { std::move(ret) };
}

//
// expressions, from the comma down
//

// The left operand is evaluated for its effect and may be void, 6.5.18p2.
expr_t
expression(parser_t &p)
{
    expr_t e = assignment_expression(p);

    while (peek(p).kind == tok_t::comma) {
        const token_t &op = advance(p);
        // before the right operand is parsed, so that this error comes first
        require_not_function(op, e);
        e = decayed(std::move(e));

        e = comma(op, std::move(e), assignment_expression(p));
    }

    return e;
}

expr_t
assignment_expression(parser_t &p)
{
    expr_t lhs = conditional_expression(p);
    if (!is_assignment_operator(peek(p).kind))
        return lhs;

    const token_t &op = advance(p);
    // before the right operand is parsed, so that this error comes first
    require_modifiable_lvalue(op, lhs);

    return assignment(op, std::move(lhs), assignment_expression(p));
}

// The condition is a scalar and stays as it is, 6.5.16p2.
expr_t
conditional_expression(parser_t &p)
{
    expr_t cond = logical_or_expression(p);
    if (peek(p).kind != tok_t::question)
        return cond;

    const token_t &op = advance(p);
    // before the arms are parsed, so that this error comes first
    cond = rvalue(op, std::move(cond));

    expr_t yes = expression(p);
    expect(p, tok_t::colon, "expected ':' in conditional expression");
    expr_t no = conditional_expression(p);

    return conditional(op, std::move(cond), std::move(yes), std::move(no));
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

        expr_t rhs = multiplicative_expression(p);
        e = op.kind == tok_t::plus ?
                addition(op, std::move(e), std::move(rhs)) :
                subtraction(op, std::move(e), std::move(rhs));
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
// parentheses is a primary expression.  The token after the
// parenthesis decides: a keyword by its kind, an identifier by what it
// names in scope, so (T)(x) is a cast when T is a typedef name and a
// call when T names a function.
bool
starts_type_name(const parser_t &p)
{
    return peek(p).kind == tok_t::lparen && is_declaration_start(p, 1);
}

expr_t
cast_expression(parser_t &p)
{
    if (!starts_type_name(p))
        return unary_expression(p);

    const token_t &open = advance(p);
    const type_t t = type_name(p);
    expect(p, tok_t::rparen, "expected ')' after type name");

    return cast(open, t, cast_expression(p));
}

expr_t
unary_expression(parser_t &p)
{
    const token_t &op = peek(p);

    switch (op.kind) {
    case tok_t::inc:
    case tok_t::dec: advance(p); return prefix_step(op, unary_expression(p));
    case tok_t::minus:
    case tok_t::tilde:
        advance(p);
        return unary_arithmetic(op, cast_expression(p));
    case tok_t::bang: advance(p); return logical_not(op, cast_expression(p));
    case tok_t::amp: advance(p); return address_of(op, cast_expression(p));
    case tok_t::star: advance(p); return indirection(op, cast_expression(p));
    case tok_t::kw_sizeof: return sizeof_expression(p);
    case tok_t::kw_alignof: not_yet(op, "operator " + quoted(op.text));
    case tok_t::plus: not_yet(op, "unary +");
    default: return postfix_expression(p);
    }
}

// sizeof E or sizeof (T).  The operand expression is typed and dropped,
// never evaluated, and does not decay, so the size of an array is the
// whole array's, 6.5.4.4p4.
expr_t
sizeof_expression(parser_t &p)
{
    const token_t &op = advance(p);

    type_t t = make_void();
    if (starts_type_name(p)) {
        advance(p);
        t = type_name(p);
        expect(p, tok_t::rparen, "expected ')' after type name");
    }
    else
        t = unary_expression(p).type;

    return sizeof_(op, t);
}

// A call names its function directly, 6.5.3.3p5 gives its type.  A
// call of a function whose result is of a not-yet type is not yet, its
// value used or not: the call makes the object it returns.
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

            const type_t t = *as< function_t >(fn.type).result;
            require_honoured(op, t);

            std::vector< expr_t > args = argument_expression_list(p, fn);
            expect(p, tok_t::rparen, "expected ')' after arguments");

            e = expr_t { t, call_t { box(std::move(e)), std::move(args) } };
            break;
        }
        case tok_t::inc:
        case tok_t::dec:
            advance(p);
            e = postfix_step(op, std::move(e));
            break;
        case tok_t::lbracket: {
            advance(p);

            expr_t index = expression(p);
            expect(p, tok_t::rbracket, "expected ']' after subscript");

            e = subscript(op, std::move(e), std::move(index));
            break;
        }
        case tok_t::dot:
        case tok_t::arrow: not_yet(op, "member access");
        default: return e;
        }
    }
}

// Each argument is converted to its parameter's type as if by
// assignment; past the parameters of a variadic function, the default
// argument promotions apply to an integer, and a pointer passes as it
// is, 6.5.3.3p6.  The count must agree, p2.
std::vector< expr_t >
argument_expression_list(parser_t &p, const symbol_t &fn)
{
    const function_t &type = as< function_t >(fn.type);

    std::vector< expr_t > args;

    if (peek(p).kind != tok_t::rparen) {
        do {
            const token_t &start = peek(p);

            expr_t arg = rvalue(start, assignment_expression(p));

            if (args.size() < type.params.size())
                arg = assignment_conversion(start, std::move(arg),
                                            *type.params[args.size()]);
            else if (type.variadic && is< int_t >(arg.type))
                arg = promote(std::move(arg));
            else if (!type.variadic)
                fail(start,
                     "too many arguments to function " + quoted(fn.name));

            args.push_back(std::move(arg));
        } while (accept(p, tok_t::comma));
    }

    if (args.size() < type.params.size())
        fail(peek(p), "too few arguments to function " + quoted(fn.name));

    return args;
}

// A name must be declared, 6.5.2p2, as an object or a function: a
// typedef name names a type, and an object declared extern at file
// scope is not addressed by c1 yet, so its use is refused here, with
// the name in hand, as is an enumeration constant's, whose value is
// not computed yet, and a symbol's an attribute marked, by the
// attribute's words.  A generic selection is not yet.  Parentheses leave the expression as it is, an
// lvalue included, p6.  A character constant is an int,
// 6.4.4.5p11.  Adjacent string literals are one, 6.4.5p5, an array of
// char one longer than its bytes for the zero appended, p6.
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

        if (sym->storage == storage_t::type_name)
            fail(t, "unexpected type name " + quoted(t.text) +
                        ": expected expression");

        if (!sym->marking.empty())
            not_yet(t, sym->marking);

        if (sym->storage == storage_t::global)
            not_yet(t, "global variable");

        if (sym->storage == storage_t::enumerator)
            not_yet(t, "enum");

        return expr_t { sym->type, name_t { sym } };
    }
    case tok_t::integer: advance(p); return integer_constant(t);
    case tok_t::character: {
        advance(p);
        const int value = character_value(t, p.target.char_is_signed);

        return expr_t { int_type(), literal_t { value } };
    }
    case tok_t::string: {
        std::string bytes;
        while (peek(p).kind == tok_t::string)
            bytes += string_bytes(advance(p));

        type_t type = make_array(char_type(p), long(bytes.size()) + 1);

        return expr_t { std::move(type), string_t { std::move(bytes) } };
    }
    case tok_t::lparen: {
        advance(p);

        expr_t e = expression(p);
        expect(p, tok_t::rparen, "expected ')'");

        return e;
    }
    case tok_t::kw_generic: not_yet(t, quoted(t.text) + " selection");
    default: fail(t, "expected expression");
    }
}

} // namespace

unit_t
parse(const std::vector< token_t > &tokens, const target_t &target)
{
    parser_t p {
        tokens, 0, target, { }, { }, make_void(), 0, first_tag_identity
    };
    p.scopes.emplace_back();

    translation_unit(p);

    return std::move(p.unit);
}

} // namespace rcc
