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

// A tag: the type it names, the keyword that declared it, whether a
// list has defined its content, and the record of a struct or a union,
// which the parser completes in place, or null for an enum.
struct tag_t
{
    type_t type;
    tok_t keyword;
    bool defined;
    record_t *record = nullptr;
};

// The names one scope declares.  The tags of structs, unions and enums
// have a name space of their own, 6.2.3p1, apart from the ordinary
// identifiers, with the same block structure.  A symbol is writable
// here, since a later declaration may add to what an earlier one said:
// a function's assembler name, its marking.  registers is one above
// the highest register index taken in the scope or in a closed block
// nested in it: a later declaration of the scope's block takes a
// register above it, since its object is live from the entry into the
// block, 6.2.4p6, and so through the nested block.
struct scope_t
{
    std::unordered_map< std::string_view, symbol_t * > names;
    std::unordered_map< std::string_view, tag_t > tags;
    int registers = 0;
};

// A name of internal linkage where an expression uses it, 6.9.1p3: its
// symbol and the token that names it.
struct internal_use_t
{
    const symbol_t *sym;
    const token_t *at;
};

// A tentative definition at file scope of an object of a record
// incomplete where it is written, 6.9.3p2, or of internal linkage and
// of an array of unknown size, p3: its symbol and the token that names
// it.
struct tentative_t
{
    const symbol_t *sym;
    const token_t *at;
};

// A declaration at file scope that an alias attribute makes the alias
// of another symbol, checked at the end of the unit: the token that
// names the declared symbol, the attribute's name, and whether the
// symbol is a function.
struct pending_alias_t
{
    const token_t *name;
    const token_t *attribute;
    bool function;
};

// The marking attributes of a run of attributes, or of all a
// declaration writes: how many, and the first alias among them.
struct markings_t
{
    int count = 0;
    const token_t *alias = nullptr;
};

// A switch whose body the parse is in, 6.8.5.3: the promoted type of
// its controlling expression, which each case's constant is converted
// to, p5; its cases so far, as the tree takes them, and the token of
// each one's constant by its value, for the words of a duplicate, p3;
// and its default's number and keyword, once one is met.
struct open_switch_t
{
    type_t type;
    std::vector< case_t > cases = { };
    std::unordered_map< long long, const token_t * > values = { };
    std::optional< int > default_ = { };
    const token_t *default_at = nullptr;
};

// The state of one parse.  Scope 0 is file scope; the scopes die with
// the parser and leave only the symbols in the unit.  result,
// registers, function and statics are the function being defined: its
// result type, the highest register index taken in it so far plus one,
// its name, and the count of the static objects its blocks declared so
// far, by name.  labels and gotos are its labels too, which have
// function scope, 6.2.1p3: each label defined so far, by name, and each
// goto's label name, in the order met, checked against the labels at
// the function's end.  break_targets and continue_targets are the
// statements around the parse a break may leave and a continue go on
// with: the loops, for both, and a switch, for a break alone.
// switches are the switches around the parse, the innermost last, which
// a case or a default label belongs to, 6.8.5.3p4; case_labels is the
// number the function's next case or default label takes.
// identities is the identity the next not-yet type of its own takes:
// an enum in its own list or marked by an attribute, an atomic type, a
// typedef through a marking attribute or an array size rcc cannot
// compute.
// internal_uses are the uses of names of internal linkage, in the order
// met, checked against their definitions at the end of the unit.
// block_entities are the functions and the objects of external linkage
// a block declared first, by name, which no declaration at file scope
// has met yet: each is the entity a later declaration of the name of
// external linkage denotes, at file scope or in another block, 6.2.2p2.
// warnings is the driver's list, which the parse adds each warning to
// as it meets it, and which outlives an error the parse throws.
// incomplete_definitions are the tentative definitions at file scope
// of an object of a record incomplete where it is declared, or of
// internal linkage and of an array of unknown size, with the token that
// names it, checked against the record and the array at the end of the
// unit.  va_record is the record behind the target's va_list, made once
// for the unit.  variadic and named are the function being defined's
// too: whether it takes variable arguments, and the count of its named
// parameters; va_area is its save area, made at its first va_start.
// temporaries are the locals its builtins made, which no declaration
// names, while its body is parsed, and nothing at file scope.  aliases
// are the declarations an alias attribute makes aliases.
struct parser_t
{
    const std::vector< token_t > &tokens;
    std::size_t pos;
    const target_t &target;
    std::vector< warning_t > &warnings;

    std::vector< scope_t > scopes;

    unit_t unit;
    type_t result;
    int registers;
    int identities;

    std::string_view function = { };
    std::unordered_map< std::string, int > statics = { };
    std::unordered_map< std::string_view, const token_t * > labels = { };
    std::vector< const token_t * > gotos = { };
    int break_targets = 0;
    int continue_targets = 0;
    std::vector< open_switch_t > switches = { };
    int case_labels = 0;
    std::vector< internal_use_t > internal_uses = { };
    std::unordered_map< std::string_view, symbol_t * > block_entities = { };
    std::vector< tentative_t > incomplete_definitions = { };
    std::vector< pending_alias_t > aliases = { };
    const record_t *va_record = nullptr;
    bool variadic = false;
    std::size_t named = 0;
    const symbol_t *va_area = nullptr;
    std::optional< std::vector< const symbol_t * > > temporaries = { };
};

// The declaration specifiers: the type, and the storage-class specifier
// when one was written, register, typedef, extern or static, for the
// caller to accept or refuse in its context.  tagged is whether the
// type specifier is a struct, union or enum one, which may declare a
// tag or enumeration constants with no declarator after it.
// function_specifier is the first inline or _Noreturn written, or null,
// for the caller to drop or refuse, and inline_specifier the first
// inline, or null.  marking is the name's token of the first marking
// attribute among the specifiers, or null: it marks each declarator of
// the declaration, as one after the declarator does.  markings are all
// the marking attributes among the specifiers.
struct specifiers_t
{
    type_t type;
    const token_t *storage_class;
    bool tagged;
    const token_t *function_specifier;
    const token_t *inline_specifier;
    const token_t *marking;
    markings_t markings;
};

// A parameter as written: its type, its name token when it has one,
// whether it was declared register, the name's token of the first
// marking attribute said of it, or null, and the '*' that sizes the
// array it is adjusted from, or null.
struct parameter_t
{
    type_t type;
    const token_t *start;
    const token_t *name;
    bool in_register;
    const token_t *marking;
    const token_t *star;
};

// A declarator: a name, absent from a parameter that leaves it out and
// from a type name, the type it gives the name, and the parameters as
// written when it declares a function, the parameter list that derives
// the name's type.  A pointer to a function has a parameter list and
// declares no function, so it has none.  scope is that list's scope,
// closed at its ')', with the parameters and the tags the list
// declares, which a definition opens again as its body's; empty when
// there are no params.  star is the '*' that sizes the array a
// parameter is adjusted from, 6.7.7.3p4, or null.  zero is the size of
// zero of a member's own array, read as left out, or null.
struct declarator_t
{
    const token_t *name;
    type_t type;
    std::optional< std::vector< parameter_t > > params;
    scope_t scope;
    const token_t *star = nullptr;
    const token_t *zero = nullptr;
};

// Whether a declarator names what it declares: a declaration's does, a
// parameter's may, 6.7.7.1p1, and a type name's, an abstract
// declarator, does not, 6.7.8p1.  A member's names it as a
// declaration's does, and its own array may have a size of zero, GNU
// C's spelling of a flexible array member, which member_list() judges.
enum class naming_t { required, optional, none, member };

//
// diagnostics
//

[[noreturn]] void
fail(const token_t &at, std::string what)
{
    throw error_t { at.where, std::move(what) };
}

// The refusal of a construct of C outside the subset, apart from an
// error of the program, so that the one place where the rule for a
// system header turns a refusal into a not-yet type catches this kind
// alone; the driver catches it as any error_t.  construct is the words
// after "not yet: ".
struct not_yet_error_t: error_t
{
    std::string construct;
};

// A construct of C outside the subset this parser takes.
[[noreturn]] void
not_yet(const token_t &at, std::string_view construct)
{
    throw not_yet_error_t { { at.where, "not yet: " + std::string(construct) },
                            std::string(construct) };
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

// The tokens after an opening brace, skipped up to the closing brace
// that matches it, which is left for the caller, with the braces nested
// in them counted.  Nothing reads them.
void
skip_to_closing_brace(parser_t &p)
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

// The type of an element of a literal of an encoding: char for none and
// for u8, the dialect rcc preprocesses in giving a UTF-8 string literal
// char elements, C17's 6.4.5p6, where C23 gives them char8_t; char16_t
// and char32_t, unsigned short and unsigned int, 6.4.4.5p13, p14; and
// wchar_t, p15, int on x86-64 and unsigned int on aarch64, as the target
// has it.
type_t
element_type(const parser_t &p, encoding_t encoding)
{
    switch (encoding) {
    case encoding_t::none:
    case encoding_t::utf8: return char_type(p);
    case encoding_t::utf16: return make_int(16, false);
    case encoding_t::utf32: return make_int(32, false);
    default: return make_int(32, p.target.wchar_is_signed);
    }
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
// real and the complex floating types, GNU C's __float128 and
// __int128, clang's SVE types, and _Bool, each of a fixed identity.  A
// not-yet type of its own takes the next identity after theirs.  The
// real floating types have clang's sizes, the same on both targets, so
// that a record lays them out as members.
type_t
float_type()
{
    return make_floating(0, "floating type 'float'", 4);
}

type_t
double_type()
{
    return make_floating(1, "floating type 'double'", 8);
}

type_t
long_double_type()
{
    return make_floating(2, "floating type 'long double'", 16);
}

// __builtin_va_list, the type behind va_list, 7.16p4, the convention's:
// on x86-64 an array of one of the unit's list records, so that a list
// decays to the record's address and a parameter of it is that pointer,
// 6.7.7.4p6; on aarch64 the record itself.
type_t
va_list_type(const parser_t &p)
{
    const type_t r = make_record(*p.va_record);
    if (p.target.va_list == va_list_t::psabi)
        return make_array(r, 1);

    return r;
}

type_t
complex_float_type()
{
    return make_not_yet(4, "complex type 'float _Complex'");
}

type_t
complex_double_type()
{
    return make_not_yet(5, "complex type 'double _Complex'");
}

type_t
complex_long_double_type()
{
    return make_not_yet(6, "complex type 'long double _Complex'");
}

type_t
float128_type()
{
    return make_not_yet(7, "floating type '__float128'");
}

// GNU C's 128-bit integer types: __int128 and signed __int128 are one
// type, as int and signed int are.
type_t
int128_type(bool is_signed)
{
    if (is_signed)
        return make_not_yet(8, "integer type '__int128'");

    return make_not_yet(9, "integer type 'unsigned __int128'");
}

// clang's names for three of the aarch64 vector types of SVE, those
// glibc's bits/math-vector.h names: scalable, of no size fixed when
// the program is compiled.
type_t
sve_float32_type()
{
    return make_not_yet(10, "'__SVFloat32_t'");
}

type_t
sve_float64_type()
{
    return make_not_yet(11, "'__SVFloat64_t'");
}

type_t
sve_bool_type()
{
    return make_not_yet(12, "'__SVBool_t'");
}

// C's boolean type, whose conversion from a scalar, false for a zero
// and true for any other value, 6.3.1.2p1, rcc does not make yet.
type_t
bool_type()
{
    return make_not_yet(13, "'_Bool'");
}

constexpr int first_tag_identity = 14;

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

bool
is_function_pointer(const type_t &t)
{
    return is< pointer_t >(t) && is< function_t >(pointee_of(t));
}

// A pointer to void and a pointer to an object type, in either order,
// which assignment converts to each other, 6.5.17.2p1, and which meet at
// void * in == and ?:, 6.5.10p2, 6.5.16p3.  A pointer to a function is
// not a pointer to an object, and meets void * only as a null pointer
// constant or through a cast.
bool
void_and_object_pointer(const type_t &a, const type_t &b)
{
    if (is_void_pointer(a))
        return !is_function_pointer(b);

    return is_void_pointer(b) && !is_function_pointer(a);
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

// Whether an integer type holds a value: two's complement over its
// width, a signed type's top bit its sign.
bool
holds(const type_t &t, exact_t v)
{
    const int_t &type = as< int_t >(t);
    const int bits = type.width - (type.is_signed ? 1 : 0);
    const exact_t least = type.is_signed ? -(exact_t(1) << bits) : 0;

    return v >= least && v < (exact_t(1) << bits);
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

// The words of what rcc does not use yet that a type is, or holds as
// an array's element: a not-yet type's, or those of a record whose list
// holds a construct rcc does not lay out yet; null for any other type.
const std::string *
not_yet_in(const type_t &t)
{
    if (is< not_yet_t >(t))
        return &as< not_yet_t >(t).what;

    if (is< record_type_t >(t) &&
        !as< record_type_t >(t).record->not_yet.empty())
        return &as< record_type_t >(t).record->not_yet;

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
// meets one only as a pointee it does not look into.  A record that
// holds a construct rcc does not lay out yet is refused the same way.
void
require_honoured(const token_t &at, const type_t &t)
{
    if (const std::string *words = not_yet_in(t))
        not_yet(at, *words);
}

// The record a type is, or holds as an array's element at any depth, or
// null.
const record_t *
record_in(const type_t &t)
{
    if (is< record_type_t >(t))
        return as< record_type_t >(t).record;

    if (is< array_t >(t))
        return record_in(*as< array_t >(t).element);

    return nullptr;
}

// Whether t is the aarch64 va_list, the one record a value of which
// passes, 7.16p4: the AAPCS64 passes a record of more than 16 bytes as
// the address of a copy the caller makes, and the list is one.  Every
// other record passes by value in no rule rcc has yet.
bool
is_list_by_reference(const parser_t &p, const type_t &t)
{
    return p.target.va_list == va_list_t::aapcs64 && is< record_type_t >(t) &&
           as< record_type_t >(t).record == p.va_record;
}

// The words a diagnostic names a record by: struct 'point', or untagged
// union for one declared without a tag.
std::string
record_words(const record_t &r)
{
    const std::string kind = r.is_union ? "union" : "struct";
    if (r.tag.empty())
        return "untagged " + kind;

    return kind + " " + quoted(r.tag);
}

// A record whose object is read or written whole: complete, 6.3.2.1p1,
// p2, an error otherwise, and laid out, not yet otherwise.
void
require_whole_record(const token_t &at, const type_t &t)
{
    const record_t &r = *as< record_type_t >(t).record;
    if (!r.complete)
        fail(at, "incomplete type " + record_words(r) +
                     " where a complete type is required");

    require_honoured(at, t);
}

// A record converted to the value stored in its object, 6.3.2.1p2,
// where a record is wanted or for its effect: c1 reads no record yet,
// so the use is not yet.  An incomplete record has no value to read,
// p2, and the use is an error.  A record is an expression only as an
// lvalue, by a name or by indirection, and is taken as the operand of
// sizeof and of &, which read no value.  Where a scalar is wanted, a
// record is refuse_record_operand()'s error.
void
refuse_record_value(const token_t &at, const type_t &t)
{
    if (!is< record_type_t >(t))
        return;

    require_whole_record(at, t);
    not_yet(at, "value of " + record_words(*as< record_type_t >(t).record));
}

// restrict qualifies a pointer to an object type, or an array of such
// pointers, 6.7.4.1p2.  It promises what an optimizer may assume, and
// deleting it changes no program's meaning, p9, so rcc takes the
// promise and drops it.  Behind a '*' it qualifies such a pointer
// unless the pointer is to a function; among the specifiers it
// qualifies the type they name, which is one only through a typedef
// name.  Whether a not-yet type is one is
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

// A void expression has no value, 6.3.2.2p1, and its use as one is an
// error.
void
require_value(const token_t &at, const expr_t &e)
{
    if (is< void_t >(e.type))
        fail(at, "void expression used as a value");
}

// Every operator that reads its operand's value asks a scalar of it, or
// an arithmetic or an integer type, and so does a condition: 6.5.4.3p1,
// 6.5.7p2, 6.5.14p2, 6.5.15p2, 6.5.16p2, 6.8.5.2p1, 6.8.6.1p2.  A record
// there is the error of the constraint, not a construct rcc lacks.
void
refuse_record_operand(const token_t &at, const type_t &t)
{
    if (is< record_type_t >(t))
        fail(at, "value of " + record_words(*as< record_type_t >(t).record) +
                     " where a scalar is required");
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
// that address.  A function designator decays the same way, p4: a
// function's name, or *E through a pointer to a function, becomes the
// pointer to the function, a cast c1 reads as the function's address,
// or as E's value with no instruction for the *.  Lvalue conversion,
// p2, is c1's load and needs no node.
expr_t
decayed(expr_t e)
{
    if (is< function_t >(e.type)) {
        type_t t = make_pointer(e.type);
        return expr_t { std::move(t), cast_t { box(std::move(e)) } };
    }

    if (!is< array_t >(e.type))
        return e;

    type_t t = make_pointer(*as< array_t >(e.type).element);

    return expr_t { std::move(t), cast_t { box(std::move(e)) } };
}

// An operand whose value is used: not void, no record, and decayed, so
// it is a scalar.
expr_t
rvalue(const token_t &at, expr_t e)
{
    require_value(at, e);
    refuse_record_operand(at, e.type);

    return decayed(std::move(e));
}

// Whether an expression is an assignment, or a comma whose right
// operand is one, at any depth: the comma's value is its right
// operand's, 6.5.18p2.
bool
ends_in_assignment(const expr_t &e)
{
    if (is< assign_t >(e))
        return true;

    if (!is< binary_t >(e) || as< binary_t >(e).op != binary_op_t::comma)
        return false;

    return ends_in_assignment(*as< binary_t >(e).rhs);
}

// An operand evaluated for its effect alone, 6.8.4p2, 6.5.18p2: it may
// be void, and it decays; a record's value it would read is
// refuse_record_value()'s.  A record's assignment reads no value of
// its own, and is taken for its effect, the copy, and so is a comma
// whose right operand is one: n++, a = b; reads no record.
expr_t
discarded(const token_t &at, expr_t e)
{
    if (!ends_in_assignment(e))
        refuse_record_value(at, e.type);

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
// as a byte, not a function, and neither an array of unknown size nor
// an incomplete record, which are incomplete, 6.2.5p27, 6.7.3.4p5, and
// have no size to step by.  The step is the pointee's size, so a
// not-yet pointee is not yet.
void
require_object_pointee(const token_t &op, const type_t &t)
{
    const type_t &pointee = pointee_of(t);

    if (is< void_t >(pointee))
        fail(op, "arithmetic on a pointer to void");

    if (is< function_t >(pointee))
        fail(op, "arithmetic on a pointer to a function");

    if (is< array_t >(pointee) && as< array_t >(pointee).count < 0)
        fail(op, "arithmetic on a pointer to an array of unknown size");

    if (is< record_type_t >(pointee) &&
        !as< record_type_t >(pointee).record->complete)
        fail(op, "arithmetic on a pointer to incomplete type " +
                     record_words(*as< record_type_t >(pointee).record));

    require_honoured(op, pointee);
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

// Two distinct complete records of one kind and one tag, which C23
// makes compatible when their members agree, 6.2.7p1, where the C of
// before made them two types: the first such pair met walking two
// types alike, through pointers, arrays and functions, or null.
const record_t *
tag_twin(const type_t &a, const type_t &b)
{
    if (is< record_type_t >(a) && is< record_type_t >(b)) {
        const record_t &x = *as< record_type_t >(a).record;
        const record_t &y = *as< record_type_t >(b).record;
        const bool twins = &x != &y && x.is_union == y.is_union && x.complete &&
                           y.complete && !x.tag.empty() && x.tag == y.tag;

        return twins ? &x : nullptr;
    }

    if (is< pointer_t >(a) && is< pointer_t >(b))
        return tag_twin(pointee_of(a), pointee_of(b));

    if (is< array_t >(a) && is< array_t >(b))
        return tag_twin(*as< array_t >(a).element, *as< array_t >(b).element);

    if (!is< function_t >(a) || !is< function_t >(b))
        return nullptr;

    const auto &f = as< function_t >(a);
    const auto &g = as< function_t >(b);
    if (const record_t *r = tag_twin(*f.result, *g.result))
        return r;

    for (std::size_t i = 0; i < std::min(f.params.size(), g.params.size()); ++i)
        if (const record_t *r = tag_twin(*f.params[i], *g.params[i]))
            return r;

    return nullptr;
}

// rcc compares no two records' members, so a mismatch of two types
// that holds such a pair is not yet: C23 may make the two compatible.
// Any other mismatch is the caller's error.
void
refuse_tag_twins(const token_t &at, const type_t &a, const type_t &b)
{
    if (const record_t *r = tag_twin(a, b))
        not_yet(at, "compatibility of two types of " + record_words(*r));
}

// The words a conversion's error names a type by: a record by its
// own, a scalar by its kind.
std::string
conversion_words(const type_t &t)
{
    if (is< record_type_t >(t))
        return record_words(*as< record_type_t >(t).record);

    return is< pointer_t >(t) ? "pointer" : "integer";
}

// No record converts as if by assignment to a scalar, and a record
// takes only a record compatible with it, 6.5.17.2p1, which rcc takes
// as the same record: either way round the conversion is the error of
// incompatible types.  Two records of one tag are not yet.
void
require_record_conversion(const token_t &at,
                          const type_t &from,
                          const type_t &to)
{
    if (!is< record_type_t >(from) && !is< record_type_t >(to))
        return;

    refuse_tag_twins(at, to, from);
    if (!(from == to))
        fail(at, "incompatible " + conversion_words(from) + " to " +
                     conversion_words(to) + " conversion");
}

// The conversion as if by assignment, of a value to the type of an
// object: the right operand of =, an initializer, 6.7.11p12, an
// argument, 6.5.3.3p2, a value returned, 6.8.7.5p3.  The constraints
// are those of simple assignment, 6.5.17.2p1: integers convert to each
// other; a pointer takes a pointer of its own type, a pointer to void
// and an object pointer convert either way, and a null pointer
// constant converts to any pointer, one to a function included.  An
// integer and a pointer meet only through a cast, 6.5.5p3, and so do a
// pointer to a function and a pointer to void; a record and a scalar
// never meet.  A value converted to a not-yet type, as an argument
// meets a parameter of one, is not yet, and so is a record converted
// to its own type, whose object c1 does not write yet; the callers
// that may meet one refuse it first in their own words.
expr_t
assignment_conversion(const token_t &at, expr_t e, const type_t &t)
{
    require_honoured(at, t);
    require_value(at, e);

    e = decayed(std::move(e));
    require_record_conversion(at, e.type, t);

    if (is< record_type_t >(t))
        not_yet(at, "value of " + record_words(*as< record_type_t >(t).record));

    const bool to_pointer = is< pointer_t >(t);
    const bool from_pointer = is< pointer_t >(e.type);

    if (to_pointer && !from_pointer && !is_null_pointer_constant(e))
        fail(at, "incompatible integer to pointer conversion");

    if (!to_pointer && from_pointer)
        fail(at, "incompatible pointer to integer conversion");

    if (to_pointer && from_pointer && !(e.type == t) &&
        !void_and_object_pointer(t, e.type) && !is_null_pointer_constant(e)) {
        refuse_tag_twins(at, t, e.type);
        fail(at, "incompatible pointer types");
    }

    return convert_to(std::move(e), t);
}

// Two values, at least one a pointer, brought to one pointer type, as
// == and != and the arms of ?: take them, 6.5.10p2, p6, 6.5.16p3, p7:
// pointers of one type stay as they are, a null pointer constant goes
// to the other operand's type, and a pointer to void and an object
// pointer meet at void *.  The null pointer constant comes first, as
// p7's "if one operand is a null pointer constant, the result has the
// type of the other operand" does before its pointer to void, so
// c ? (void *) 0 : p is an int * for an int *p, and a (void *) 0 met by
// a pointer to a function, which meets no other void *, goes to the
// function pointer's type.  False when none of the three applies.
bool
common_pointer(expr_t &lhs, expr_t &rhs)
{
    const bool lp = is< pointer_t >(lhs.type);
    const bool rp = is< pointer_t >(rhs.type);

    type_t t = lp ? lhs.type : rhs.type;

    if (lp && rp && !(lhs.type == rhs.type)) {
        if (is_null_pointer_constant(lhs))
            t = rhs.type;
        else if (is_null_pointer_constant(rhs))
            t = lhs.type;
        else if (void_and_object_pointer(lhs.type, rhs.type))
            t = make_pointer(make_void());
        else
            return false;
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
// identities
//

// Whether the constant c, the right operand of op in the type t, makes
// op the identity (backend.md 6.4): + - | ^ by 0, << >> by a multiple
// of t's width, * and / by 1, & by every bit of t's width.  c is the
// constant as folded and converted to the operation's type, or a shift
// count in its own, so u & -1 on an unsigned is one and l & 0xffffffff
// on a long is not.  A count is taken modulo the width of t, the
// promoted left operand, as the instruction masks it (G3): x << 32 on
// an int is x, and x << 33 is no identity.
bool
is_identity_by(binary_op_t op, const type_t &t, const expr_t &c)
{
    if (!is_integer_constant(c))
        return false;

    const long long v = as< literal_t >(c).value;

    switch (op) {
    case binary_op_t::add:
    case binary_op_t::sub:
    case binary_op_t::bit_or:
    case binary_op_t::bit_xor: return v == 0;
    case binary_op_t::shl:
    case binary_op_t::shr:
        return (bits_of(c) & unsigned(as< int_t >(t).width - 1)) == 0;
    case binary_op_t::mul:
    case binary_op_t::div: return v == 1;
    case binary_op_t::bit_and: return v == wrapped(-1, as< int_t >(t));
    default: return false;
    }
}

// Whether a constant on the left of op makes it the identity as one on
// the right does: + * & | ^, which commute.  0 - a, 0 << a and 1 / a
// are no identity.
bool
is_identity_after(binary_op_t op, const type_t &t, const expr_t &c)
{
    const bool commutes = op == binary_op_t::add || op == binary_op_t::mul ||
                          op == binary_op_t::bit_and ||
                          op == binary_op_t::bit_or ||
                          op == binary_op_t::bit_xor;

    return commutes && is_identity_by(op, t, c);
}

// The operand a binary node of type t is the value of, its other one
// a constant that makes the operator the identity, or null.
expr_t *
identity_operand(binary_t &b, const type_t &t)
{
    if (is_identity_by(b.op, t, *b.rhs))
        return b.lhs.get();

    if (is_identity_after(b.op, t, *b.lhs))
        return b.rhs.get();

    return nullptr;
}

void drop_identities(expr_t &);

// drop_identities() of each operand of a node.
void
drop_identities_under(expr_t &e)
{
    if (auto *u = std::get_if< unary_t >(&e.v))
        return drop_identities(*u->arg);

    if (auto *b = std::get_if< binary_t >(&e.v)) {
        drop_identities(*b->lhs);
        return drop_identities(*b->rhs);
    }

    if (auto *a = std::get_if< assign_t >(&e.v)) {
        drop_identities(*a->lhs);
        return drop_identities(*a->rhs);
    }

    if (auto *c = std::get_if< call_t >(&e.v)) {
        drop_identities(*c->callee);
        for (expr_t &arg : c->args)
            drop_identities(arg);
        return;
    }

    if (auto *c = std::get_if< cast_t >(&e.v))
        return drop_identities(*c->arg);

    if (auto *c = std::get_if< conditional_t >(&e.v)) {
        drop_identities(*c->cond);
        drop_identities(*c->yes);
        drop_identities(*c->no);
    }
}

// Each identity's node under e and e's own left out, its other operand
// in its place (backend.md 6.4).  The operand has the node's type, the
// conversions the operator asked of it included, so the tree's types
// stay: sizeof (c + 0) is 4, and c + 0 on a char is the promoted c.
// The node is built as written and dropped once the function's body is
// whole, since it is no lvalue: &(a + 0) is an error and &a is not.
void
drop_identities(expr_t &e)
{
    drop_identities_under(e);

    auto *b = std::get_if< binary_t >(&e.v);
    if (!b)
        return;

    expr_t *kept = identity_operand(*b, e.type);
    if (!kept)
        return;

    expr_t operand = std::move(*kept);
    e = std::move(operand);
}

void drop_identities(stmt_t &);

void
drop_identities(block_t &b)
{
    for (stmt_t &item : b.body)
        drop_identities(item);
}

void
drop_identities(std::optional< expr_t > &e)
{
    if (e)
        drop_identities(*e);
}

void
drop_identities(stmt_ptr_t &s)
{
    if (s)
        drop_identities(*s);
}

// drop_identities() of every expression of a statement and of the
// statements in it.
void
drop_identities(stmt_t &s)
{
    if (auto *x = std::get_if< expr_stmt_t >(&s.v))
        return drop_identities(x->e);

    if (auto *d = std::get_if< decl_stmt_t >(&s.v))
        return drop_identities(d->init);

    if (auto *b = std::get_if< block_t >(&s.v))
        return drop_identities(*b);

    if (auto *i = std::get_if< if_t >(&s.v)) {
        drop_identities(i->cond);
        drop_identities(i->yes);
        return drop_identities(i->no);
    }

    if (auto *w = std::get_if< switch_t >(&s.v)) {
        drop_identities(w->cond);
        return drop_identities(w->body);
    }

    if (auto *w = std::get_if< while_t >(&s.v)) {
        drop_identities(w->cond);
        return drop_identities(w->body);
    }

    if (auto *d = std::get_if< do_t >(&s.v)) {
        drop_identities(d->body);
        return drop_identities(d->cond);
    }

    if (auto *f = std::get_if< for_t >(&s.v)) {
        drop_identities(f->init);
        drop_identities(f->cond);
        drop_identities(f->step);
        return drop_identities(f->body);
    }

    if (auto *r = std::get_if< return_t >(&s.v))
        drop_identities(r->value);
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

    if (rp && !(lhs.type == rhs.type)) {
        refuse_tag_twins(op, lhs.type, rhs.type);
        fail(op, "subtraction of pointers to different types");
    }

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
// object type, 6.5.9p2, so not two pointers to a function, and == and
// != what common_pointer() brings to one, 6.5.10p2.  A pointer and any
// other integer meet only through a cast.
expr_t
comparison(const token_t &op, expr_t lhs, expr_t rhs)
{
    lhs = rvalue(op, std::move(lhs));
    rhs = rvalue(op, std::move(rhs));

    const bool lp = is< pointer_t >(lhs.type);
    const bool rp = is< pointer_t >(rhs.type);
    const bool equality = op.kind == tok_t::eq || op.kind == tok_t::ne;

    if (lp && rp && !(lhs.type == rhs.type))
        refuse_tag_twins(op, lhs.type, rhs.type);

    if (!lp && !rp)
        arithmetic_conversions(lhs, rhs);
    else if (equality && !common_pointer(lhs, rhs))
        fail(op, lp && rp ? "comparison of distinct pointer types" :
                            "comparison between pointer and integer");
    else if (!equality && !(lp && rp))
        fail(op, "comparison between pointer and integer");
    else if (!equality && !(lhs.type == rhs.type))
        fail(op, "comparison of distinct pointer types");
    else if (!equality && is_function_pointer(lhs.type))
        fail(op, "ordered comparison of pointers to functions");

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
// count keeps its promoted type; the target's instruction reads only
// its low bits (G3).
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

// The result is the right operand's type and value, an array or a
// function decayed, 6.5.18p2; the left operand comes discarded().
expr_t
comma(expr_t lhs, expr_t rhs)
{
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
// a null pointer constant do not meet, p3.  Two void arms make a void
// conditional, p3, p6, which stands where a value is dropped, as a
// call of a void function does, and c1 generates its arms for their
// effect; one void arm and one of a value meet in none of p3's pairs.
// Two arms of one record are a record's value, not yet; a record and
// any other arm meet in none of them either.  On a constant condition
// and two constant arms, the node is the arm the condition selects,
// p5.  The condition comes as an rvalue.
expr_t
conditional(const token_t &op, expr_t cond, expr_t yes, expr_t no)
{
    const bool void_yes = is< void_t >(yes.type);
    const bool void_no = is< void_t >(no.type);

    if (void_yes && void_no)
        return expr_t { make_void(), conditional_t { box(std::move(cond)),
                                                     box(std::move(yes)),
                                                     box(std::move(no)) } };

    if (void_yes || void_no)
        fail(op, "void and non-void operands in conditional expression");

    if (is< record_type_t >(yes.type) || is< record_type_t >(no.type)) {
        refuse_tag_twins(op, yes.type, no.type);
        if (yes.type == no.type)
            refuse_record_value(op, yes.type);

        fail(op, "incompatible operand types in conditional expression");
    }

    yes = rvalue(op, std::move(yes));
    no = rvalue(op, std::move(no));

    const bool pointers = is< pointer_t >(yes.type) && is< pointer_t >(no.type);

    if (pointers)
        refuse_tag_twins(op, yes.type, no.type);

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
// pointer, p1, p7, p8, a pointer to a function as any other: an
// address, which no instruction converts.  A cast yields no lvalue,
// 6.5.5p6 and its footnote, so a cast to the operand's own type is
// still a node.  A constant cast to an integer or a pointer type is the
// constant converted_constant() gives, which is not an lvalue either.
// A struct or a union is no scalar, as the type named and as the
// operand alike, whatever rcc lays out of it; a cast to a not-yet type
// is not yet.  A record's value discarded by a cast to void is
// discarded()'s.
expr_t
cast(const token_t &open, const type_t &t, expr_t arg)
{
    if (is< record_type_t >(t))
        fail(open, "cast to a type that is not a scalar");

    require_honoured(open, t);

    if (is< void_t >(t))
        return convert_to(discarded(open, std::move(arg)), t);

    if (!is< int_t >(t) && !is< pointer_t >(t))
        fail(open, "cast to a type that is not a scalar");

    if (is< record_type_t >(arg.type))
        fail(open, "operand of a cast is not a scalar");

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

// + takes an operand of arithmetic type, an integer here, 6.5.4.3p1,
// and its result is the operand's value in the promoted type, p2.  The
// source writes no operation of its own, so the tree holds no node for
// the +, only the promotion: the cast of a narrow operand, which c1
// writes as its extending load, and nothing for an int or a wider one.
// The result is a value and no lvalue, so an operand that is one stands
// under a cast to its own type, which c1 makes nothing of: +x is (T) x,
// T the promoted type of x.  + on an integer constant is the constant.
expr_t
unary_plus(const token_t &op, expr_t arg)
{
    arg = rvalue(op, std::move(arg));
    require_integer(op, arg);

    arg = promote(std::move(arg));
    if (!is_lvalue(arg))
        return arg;

    type_t t = arg.type;

    return expr_t { std::move(t), cast_t { box(std::move(arg)) } };
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
// type, 6.5.4.2p1, p3.  On a function designator it yields the pointer
// to the function, p3, which is the designator's decay, p4: &f and f
// where a value is wanted are one node, and &*p is p's value, as if
// both operators were omitted.
expr_t
address_of(const token_t &op, expr_t arg)
{
    if (is< function_t >(arg.type))
        return decayed(std::move(arg));

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

// The lvalue of type t at offset bytes from what pointer points to,
// *(T *) ((char *) pointer + offset), built of the tree's own nodes, as
// the Seventh Edition compiler's first pass wrote a->b as *(a + offset);
// at offset 0 it is *(T *) pointer, with no sum.  The tree of a member's
// access and of a local list's subobject, which c1 reads as one offset
// from the outermost pointer.
expr_t
lvalue_at(const parser_t &p, expr_t pointer, long offset, const type_t &t)
{
    if (offset != 0) {
        const type_t bytes = make_pointer(char_type(p));
        if (!(pointer.type == bytes))
            pointer = expr_t { bytes, cast_t { box(std::move(pointer)) } };

        expr_t constant { long_type(), literal_t { offset } };
        pointer = expr_t { bytes,
                           binary_t { binary_op_t::add, box(std::move(pointer)),
                                      box(std::move(constant)) } };
    }

    const type_t to = make_pointer(t);
    if (!(pointer.type == to))
        pointer = expr_t { to, cast_t { box(std::move(pointer)) } };

    return expr_t { t, unary_t { unary_op_t::deref, box(std::move(pointer)) } };
}

// *E: the operand a pointer, after decay; the result the lvalue it
// points to, of the pointee type, 6.5.4.2p2, p4.  A void pointee
// designates nothing to read, and a not-yet pointee is not yet.
// Through a pointer to a function the result is a function designator,
// p4, which decays back to the pointer where a value is wanted, so
// (*p)(x), (**p)(x) and p(x) are one call.  A record pointee is an
// lvalue whose value refuse_record_value() or refuse_record_operand()
// refuses, and whose address c1 forms by the record's size; an
// incomplete one has none, so its lvalue, which only & and sizeof could
// take, is not yet.
expr_t
indirection(const token_t &op, expr_t arg)
{
    arg = rvalue(op, std::move(arg));
    if (!is< pointer_t >(arg.type))
        fail(op, "indirection requires a pointer operand");

    const type_t t = pointee_of(arg.type);

    if (is< void_t >(t))
        fail(op, "indirection through a pointer to void");

    if (is< record_type_t >(t) && !as< record_type_t >(t).record->complete)
        not_yet(op, "indirection through a pointer to incomplete type " +
                        record_words(*as< record_type_t >(t).record));

    require_honoured(op, t);

    return expr_t { t, unary_t { unary_op_t::deref, box(std::move(arg)) } };
}

// sizeof folds to a constant of type unsigned long, size_t on LP64,
// 6.5.4.4p2, p5.  A function, void, an array of unknown size and an
// incomplete record have no size, p1; a not-yet type has one rcc does
// not know yet.  A record's size is its layout's, its padding included,
// p4.  The name is the operator's, with the underscore a name takes
// when it is a keyword of C++.
expr_t
sizeof_(const token_t &op, const type_t &t)
{
    if (is< void_t >(t))
        fail(op, "invalid application of 'sizeof' to a void type");

    if (is< function_t >(t))
        fail(op, "invalid application of 'sizeof' to a function type");

    if (is< array_t >(t) && as< array_t >(t).count < 0)
        fail(op, "invalid application of 'sizeof' to an array of unknown size");

    if (is< record_type_t >(t) && !as< record_type_t >(t).record->complete)
        fail(op, "invalid application of 'sizeof' to incomplete type " +
                     record_words(*as< record_type_t >(t).record));

    require_honoured(op, t);

    return expr_t { size_type(), literal_t { size_of(t) } };
}

// _Alignof ( type-name ) folds to the alignment of the type, a size_t
// constant, 6.5.4.4p3, p5; of an array, its element's.  A function and
// an incomplete type have none, p1: void, an array of unknown size and
// an incomplete record.  A not-yet type has one rcc does not know yet.
expr_t
alignof_(const token_t &op, const type_t &t)
{
    if (is< void_t >(t))
        fail(op,
             "invalid application of " + quoted(op.text) + " to a void type");

    if (is< function_t >(t))
        fail(op, "invalid application of " + quoted(op.text) +
                     " to a function type");

    if (is< array_t >(t) && as< array_t >(t).count < 0)
        fail(op, "invalid application of " + quoted(op.text) +
                     " to an array of unknown size");

    if (is< record_type_t >(t) && !as< record_type_t >(t).record->complete)
        fail(op, "invalid application of " + quoted(op.text) +
                     " to incomplete type " +
                     record_words(*as< record_type_t >(t).record));

    require_honoured(op, t);

    return expr_t { size_type(), literal_t { align_of(t) } };
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

// A subscript's pointer points to a complete object type, 6.5.3.2p1:
// not void, not a function, and neither an array of unknown size nor an
// incomplete record, which are incomplete, 6.2.5p27, 6.7.3.4p5.  Each is
// refused in the subscript's words, before the addition that would
// refuse it in its own.
void
require_subscripted_pointee(const token_t &op, const type_t &t)
{
    const type_t &pointee = pointee_of(t);

    if (is< void_t >(pointee))
        fail(op, "subscript of a pointer to void");

    if (is< function_t >(pointee))
        fail(op, "subscript of a pointer to a function");

    if (is< array_t >(pointee) && as< array_t >(pointee).count < 0)
        fail(op, "subscript of a pointer to an array of unknown size");

    if (is< record_type_t >(pointee) &&
        !as< record_type_t >(pointee).record->complete)
        fail(op, "subscript of a pointer to incomplete type " +
                     record_words(*as< record_type_t >(pointee).record));
}

// E1[E2] is *((E1)+(E2)), 6.5.3.2p2, and is built as exactly that tree,
// so the pointer may be either operand.  One operand is a pointer to a
// complete object type and the other an integer, p1, which the
// subscript checks itself, so that its errors name the subscript and
// not the addition: two pointers make a subscript that is no integer.
expr_t
subscript(const token_t &op, expr_t e, expr_t index)
{
    e = rvalue(op, std::move(e));
    index = rvalue(op, std::move(index));
    if (!is< pointer_t >(e.type) && !is< pointer_t >(index.type))
        fail(op, "subscripted value is not an array or a pointer");

    if (is< pointer_t >(e.type) && is< pointer_t >(index.type))
        fail(op, "array subscript is not an integer");

    require_subscripted_pointee(op,
                                is< pointer_t >(e.type) ? e.type : index.type);

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

// The type a name the compiler declares stands for, or nothing:
// __int128_t and __uint128_t, clang's and gcc's names of the 128-bit
// integer types, which glibc's sys/user.h writes in a member list; and
// clang's names of the SVE types.  No header declares them.
// __builtin_va_list, clang's and gcc's name for the type behind
// va_list, is the unit's, named_type()'s.
std::optional< type_t >
builtin_type(std::string_view name)
{
    if (name == "__int128_t")
        return int128_type(true);

    if (name == "__uint128_t")
        return int128_type(false);

    if (name == "__SVFloat32_t")
        return sve_float32_type();

    if (name == "__SVFloat64_t")
        return sve_float64_type();

    if (name == "__SVBool_t")
        return sve_bool_type();

    return std::nullopt;
}

// The type an identifier names where it stands: a typedef's, in scope
// and not hidden by an inner declaration of the name; or, while
// undeclared, the builtin type of the name.  Nothing for any other
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
        return va_list_type(p);

    if (!sym)
        return builtin_type(t.text);

    return std::nullopt;
}

bool
names_type(const parser_t &p, const token_t &t)
{
    return named_type(p, t).has_value();
}

// One above the highest register index of the register variables live
// at this point of the function: those of the scopes open, the
// parameters' among them.  A scope's variables leave when it closes.
int
above_live_registers(const parser_t &p)
{
    int above = 0;
    for (const auto &scope : p.scopes)
        for (const auto &[name, sym] : scope.names)
            if (in_register(*sym))
                above = std::max(above, sym->register_index + 1);

    return above;
}

// The register a register declaration takes: above those live, and
// above those its block's closed inner blocks took, since its object is
// live from the entry into its block, 6.2.4p6, through those blocks.  A
// block opened later starts from the registers live at its opening, so
// sibling blocks share.
int
next_register(const parser_t &p)
{
    return std::max(above_live_registers(p), p.scopes.back().registers);
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

// The register index of an object declared by name, register or not:
// -1 for none.  A register declaration takes the next register,
// backend.md chapter 4.  One whose next is past the target's last
// declares an ordinary object, with a warning: the extent to which the
// suggestion is effective is implementation-defined, 6.7.2p12, so the
// program is conforming and compiles.  The function's count and the
// scope's are each the highest index taken plus one.
int
register_index(parser_t &p, const token_t &name, bool in_register)
{
    const int count = p.target.register_variables;
    const int next = in_register ? next_register(p) : 0;
    if (in_register && next >= count) {
        p.warnings.push_back(
            { name.where, "register variable " + quoted(name.text) +
                              " in a stack slot: the " + in_words(count) +
                              " registers are taken" });
        in_register = false;
    }

    const int index = in_register ? next : -1;
    p.registers = std::max(p.registers, index + 1);
    p.scopes.back().registers = std::max(p.scopes.back().registers, index + 1);

    return index;
}

// A new object in the innermost scope.  Without linkage a name is
// declared once per scope (6.7.1p4); an inner scope may shadow it.  The
// map's key views the symbol's own name, which the deque keeps in
// place.
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

    const int index = register_index(p, name, in_register);

    symbol_t &sym = p.unit.symbols.emplace_back(symbol_t {
        std::string(name.text), std::move(t), storage, in_register, index });
    scope.emplace(sym.name, &sym);

    return &sym;
}

// The end of a block's scope, a compound statement's or a for's: its
// names leave, and the enclosing scope keeps the registers it took as
// taken, for the enclosing block's later declarations, whose objects
// are live through this block, 6.2.4p6.  A function's body ends with a
// pop alone, since file scope takes no register.
void
close_block(parser_t &p)
{
    const int registers = p.scopes.back().registers;
    p.scopes.pop_back();

    scope_t &enclosing = p.scopes.back();
    enclosing.registers = std::max(enclosing.registers, registers);
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
// octal, hexadecimal or binary one may go unsigned.  long long is long on
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
    else if (s.size() > 1 && s[0] == '0' && (s[1] == 'b' || s[1] == 'B')) {
        base = 2;
        s.remove_prefix(2);
    }
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

    // what an optimizer may assume of what an lvalue aliases
    "may_alias",

    // what rcc does to every call, which it writes as written, inlining
    // none
    "noinline",

    // requests for a diagnostic
    "access", "deprecated", "fallthrough", "format", "format_arg", "nodiscard",
    "sentinel", "unused", "warn_unused_result"
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

// A marking attribute counted among a declaration's.
void
count_marking(markings_t &markings, const token_t &name)
{
    ++markings.count;
    if (!markings.alias && attribute_name(name) == "alias")
        markings.alias = &name;
}

// Each __attribute__ ((list)), also spelled __attribute, of the run at
// the position, none or more, its attributes separated by commas.  The
// name's token of the first marking attribute, or null; each marking
// one is counted in seen, where the caller gives it.
const token_t *
attribute_specifier_list(parser_t &p, markings_t *seen = nullptr)
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

            if (name && seen)
                count_marking(*seen, *name);
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
// the name's token of the first marking attribute, or null; markings
// are all the declaration's, its specifiers' among them.
struct declarator_tail_t
{
    const token_t *asm_keyword;
    std::string asm_name;
    const token_t *marking;
    markings_t markings;
};

// The tail of a declarator of the declaration the specifiers begin.  A
// marking attribute among the specifiers is said of every declarator,
// and comes first.
declarator_tail_t
declarator_tail(parser_t &p, const specifiers_t &spec)
{
    declarator_tail_t tail { nullptr, { }, spec.marking, spec.markings };

    const token_t *marking = attribute_specifier_list(p, &tail.markings);
    if (!tail.marking)
        tail.marking = marking;

    if (peek(p).kind == tok_t::kw_asm) {
        tail.asm_keyword = &peek(p);
        tail.asm_name = asm_label(p);
    }

    marking = attribute_specifier_list(p, &tail.markings);
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
void
function_definition(parser_t &, const specifiers_t &, const declarator_t &);
void header_function_definition(parser_t &,
                                const specifiers_t &,
                                const declarator_t &);
specifiers_t declaration_specifiers(parser_t &);
type_t struct_or_union_specifier(parser_t &);
type_t enum_specifier(parser_t &);
type_t atomic_type_specifier(parser_t &);
declarator_t declarator(parser_t &,
                        const type_t &,
                        naming_t = naming_t::required,
                        bool typedef_ = false);
declarator_t
parenthesized_declarator(parser_t &, const type_t &, naming_t, bool typedef_);
declarator_t declarator_suffix(parser_t &,
                               const token_t *,
                               const type_t &,
                               bool typedef_,
                               bool parameter,
                               bool member);
type_t pointer(parser_t &, type_t);
type_t array_declarator(parser_t &,
                        const type_t &,
                        bool typedef_,
                        bool parameter,
                        bool member,
                        const token_t *&star,
                        const token_t *&zero);
bool parameter_type_list(parser_t &, std::vector< parameter_t > &);
parameter_t parameter_declaration(parser_t &);
type_t type_name(parser_t &);
std::vector< stmt_t > declaration(parser_t &);
std::vector< stmt_t > init_declarator_list(parser_t &, const specifiers_t &);
void init_declarator(parser_t &,
                     const specifiers_t &,
                     const declarator_t &,
                     std::vector< stmt_t > &);

stmt_t statement(parser_t &);
block_t compound_statement(parser_t &);
void block_item_list(parser_t &, block_t &);
stmt_t expression_statement(parser_t &);
stmt_t selection_statement(parser_t &);
stmt_t iteration_statement(parser_t &);
stmt_t while_statement(parser_t &);
stmt_t do_statement(parser_t &);
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
expr_t alignof_expression(parser_t &);
expr_t postfix_expression(parser_t &);
std::vector< expr_t >
argument_expression_list(parser_t &, const function_t &, std::string_view);
expr_t primary_expression(parser_t &);

//
// declarations
//

// The position past the attribute specifiers that start at ahead, GNU
// C's __attribute__ ((...)), each to the parenthesis that closes its
// first; ahead when none starts there.  A specifier without its
// parentheses ends the run, which the parse then refuses.
std::size_t
past_attributes(const parser_t &p, std::size_t ahead)
{
    while (peek(p, ahead).kind == tok_t::kw_attribute) {
        ++ahead;
        if (peek(p, ahead).kind != tok_t::lparen)
            return ahead;

        for (int depth = 0;; ++ahead) {
            const tok_t kind = peek(p, ahead).kind;
            if (kind == tok_t::eof)
                return ahead;

            depth += kind == tok_t::lparen ? 1 : kind == tok_t::rparen ? -1 : 0;
            if (depth == 0) {
                ++ahead;
                break;
            }
        }
    }

    return ahead;
}

// Whether a declaration starts at the token ahead: a typedef name in
// scope, or a keyword a declaration can start with, those outside the
// subset included, so that they are refused by name and not as an
// expression; any of them may follow __extension__, and attributes,
// which the specifiers then read as they read those among them.  A
// typedef name before a colon is a label, since labels have a name space
// of their own, 6.2.3p1.  Attributes before a ';' or an expression start
// no declaration: __attribute__ ((fallthrough)); is a statement.
bool
is_declaration_start(const parser_t &p, std::size_t ahead = 0)
{
    const token_t &t = peek(p, ahead);
    if (t.kind == tok_t::identifier)
        return names_type(p, t) && peek(p, ahead + 1).kind != tok_t::colon;

    if (t.kind == tok_t::kw_extension)
        return is_declaration_start(p, ahead + 1);

    if (t.kind == tok_t::kw_attribute)
        return is_declaration_start(p, past_attributes(p, ahead));

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
    case tok_t::kw_static_assert:
    case tok_t::kw_float128:
    case tok_t::kw_int128: return true;
    default: return false;
    }
}

bool
declared(const specifiers_t &spec, tok_t storage_class)
{
    return spec.storage_class && spec.storage_class->kind == storage_class;
}

void complete_tentative_arrays(parser_t &);
void require_completed_definitions(const parser_t &);
void require_internal_definitions(const parser_t &);
void resolve_aliases(parser_t &);

void
translation_unit(parser_t &p)
{
    while (peek(p).kind != tok_t::eof)
        external_declaration(p);

    require_completed_definitions(p);
    complete_tentative_arrays(p);
    require_internal_definitions(p);
    resolve_aliases(p);
}

// Whether two array types are one but for the size one of them leaves
// out: compatible, 6.7.7.3p6, since the elements are one type, and the
// composite type of the two is the one of known size, 6.2.7p3.  Only
// the outermost bound may be left out, so the elements are complete.
bool
sized_alike(const type_t &a, const type_t &b)
{
    if (!is< array_t >(a) || !is< array_t >(b))
        return false;

    const auto &x = as< array_t >(a);
    const auto &y = as< array_t >(b);

    return (x.count < 0 || y.count < 0) && *x.element == *y.element;
}

// An earlier declaration of the name, met again: the same kind of
// entity, of the same type, a function or an object of linkage by the
// rule of compatible types (6.2.7p2), and a typedef because a typedef
// name may be declared again only as the type it already denotes
// (6.7.1p4).  An
// object declared as an array of unknown size and as one of known size
// takes the composite type from the later declaration on, p5, which is
// the array of known size whichever came first.
symbol_t &
redeclared(const token_t &name,
           symbol_t &earlier,
           const type_t &t,
           storage_t storage)
{
    if (earlier.storage != storage)
        fail(name, "redefinition of " + quoted(name.text));

    if (earlier.type == t)
        return earlier;

    if (storage == storage_t::global && sized_alike(earlier.type, t)) {
        if (as< array_t >(earlier.type).count < 0)
            earlier.type = t;

        return earlier;
    }

    refuse_tag_twins(name, earlier.type, t);

    if (storage == storage_t::type_name)
        fail(name, "redefinition of typedef " + quoted(name.text) +
                       " with another type");

    fail(name, "conflicting types for " + quoted(name.text));
}

// The declaration of the name earlier in the current scope, met again
// by redeclared(), or null when there is none.
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

    return &redeclared(name, *it->second, t, storage);
}

// A function or an object a block declared first with external linkage
// is the entity a declaration of its name at file scope denotes, 6.2.2p2:
// the name joins the file's names as that entity, where
// earlier_declaration() meets it, unless the file declares the name
// already, as another entity.
void
adopt_block_entity(parser_t &p, std::string_view name)
{
    const auto it = p.block_entities.find(name);
    if (it == p.block_entities.end())
        return;

    if (p.scopes.front().names.emplace(it->first, it->second).second)
        p.block_entities.erase(it);
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

// A parameter's name, declared in the scope of its list from the end of
// its declarator, 6.2.1p7, so that a later parameter's declarator sees
// it: as an array's size, or as the object that hides a typedef name.
// The parameters of one list share that scope, 6.7.1p4, whether a body
// follows or not, and so does an enumeration constant a parameter's
// type declares, 6.2.1p4; so the message names a parameter only when
// the earlier declaration is one.  The parameter takes no register
// here: register is ignored but in a definition, 6.7.7.4p12, which
// gives the registers.
void
declare_parameter(parser_t &p, const parameter_t &param)
{
    if (!param.name)
        return;

    const auto &scope = p.scopes.back().names;

    const auto it = scope.find(param.name->text);
    if (it != scope.end())
        fail(*param.name,
             it->second->storage == storage_t::parameter ?
                 "redefinition of parameter " + quoted(param.name->text) :
                 "redefinition of " + quoted(param.name->text));

    added(p, param.name->text, param.type, storage_t::parameter);
}

// A typedef declares its declarator's name as a name for the type the
// declarator gives, in the current scope, 6.7.9p3.  The name has no
// object, so nothing is generated for it, and it takes no assembler
// name and no initializer, which gives the value stored in an object,
// 6.7.11p9.  A typedef of function type defines no function: a
// function definition's storage class is extern or static, 6.9.2p4, so
// a body after its declarator is an error of its own.  The type may be
// a function type, whose parameters share one scope as a prototype's
// do.  Through a marking attribute the name stands for a not-yet type
// of its own, of the attribute's words: typedef int register_t
// __attribute__ ((__mode__ (__word__))) names an integer of a size rcc
// does not take, and its objects are refused.
void
type_definition(parser_t &p, const specifiers_t &spec, const declarator_t &d)
{
    const declarator_tail_t tail = declarator_tail(p, spec);
    if (tail.asm_keyword)
        fail(*tail.asm_keyword, quoted(tail.asm_keyword->text) +
                                    " on typedef " + quoted(d.name->text));

    if (peek(p).kind == tok_t::assign)
        fail(peek(p),
             "typedef " + quoted(d.name->text) + " cannot have an initializer");

    if (d.params && peek(p).kind == tok_t::lbrace)
        fail(peek(p), "typedef " + quoted(d.name->text) +
                          " cannot have a function body");

    type_t t = d.type;
    if (tail.marking)
        t = make_not_yet(p.identities++, attribute_words(*tail.marking));

    if (!earlier_declaration(p, *d.name, t, storage_t::type_name))
        added(p, d.name->text, std::move(t), storage_t::type_name);
}

// The name's token of the first marking attribute said of a parameter
// of the function the declarator declares, or null.
const token_t *
marked_parameter(const declarator_t &d)
{
    for (const parameter_t &param : *d.params)
        if (param.marking)
            return param.marking;

    return nullptr;
}

// The linkage a declaration at file scope gives its name, 6.2.2: static
// gives internal linkage, p3; extern the linkage of the earlier
// declaration, p4, or else external, and a function declared without a
// storage class is as if declared extern, p5; an object declared
// without one has external linkage, p5.  So static int f(void); int
// f(void) { ... } defines a function of internal linkage, and static
// int n; extern int n; declares one object of internal linkage.
linkage_t
linkage_of(const specifiers_t &spec, const symbol_t *earlier, bool function)
{
    if (declared(spec, tok_t::kw_static))
        return linkage_t::internal;

    const bool as_extern = function || declared(spec, tok_t::kw_extern);
    if (earlier && as_extern)
        return earlier->linkage;

    return linkage_t::external;
}

// A name of internal and of external linkage in one unit is undefined,
// 6.2.2p7, and refused: int n; static int n; and static int n; int n;,
// whose second declaration gives the name the other linkage.
void
require_linkage(const token_t &name, const symbol_t &earlier, linkage_t linkage)
{
    if (earlier.linkage == linkage)
        return;

    if (linkage == linkage_t::internal)
        fail(name, "static declaration of " + quoted(name.text) +
                       " follows non-static declaration");

    fail(name, "non-static declaration of " + quoted(name.text) +
                   " follows static declaration");
}

// The target an alias attribute names, its one argument, ( "name" ),
// adjacent literals joined.  The arguments were skipped as the
// attribute was read, and are read here from the tokens after its name.
std::string
alias_target(const parser_t &p, const token_t &attribute)
{
    std::size_t i = std::size_t(&attribute - p.tokens.data()) + 1;
    if (p.tokens[i].kind != tok_t::lparen ||
        p.tokens[i + 1].kind != tok_t::string)
        fail(attribute, "expected a string literal as the argument of " +
                            attribute_words(attribute));

    std::string target;
    for (++i; p.tokens[i].kind == tok_t::string; ++i)
        target += string_bytes(p.tokens[i]);

    if (p.tokens[i].kind != tok_t::rparen)
        fail(p.tokens[i],
             "expected ')' after the target of " + attribute_words(attribute));

    return target;
}

// A declaration at file scope that alias ("target") makes an alias,
// GNU C's: the alias is a symbol of the target's address, which the
// assembler gives it, so the declaration does not mark its symbol, and
// a use of it is a use of a symbol.  rcc writes the alias where it is
// the declaration's one marking attribute, on a function none of whose
// parameters an attribute marks, or on an object declared extern, and
// the declarator has no assembler name; any other alias is not yet
// where it is declared.  A definition is no alias, an error, as GNU C
// has it: a function's body, an object's initializer or a tentative
// definition.  The symbol and the target are checked at the end of
// the unit, resolve_aliases().
void
declared_alias(parser_t &p,
               const specifiers_t &spec,
               const declarator_t &d,
               declarator_tail_t &tail,
               bool function)
{
    const token_t &attribute = *tail.markings.alias;
    const std::string words = attribute_words(attribute);

    if (tail.markings.count > 1)
        not_yet(attribute, words + " beside another attribute that marks");

    if (tail.asm_keyword)
        not_yet(attribute, words + " with an assembler name");

    if (const token_t *marked = function ? marked_parameter(d) : nullptr)
        not_yet(*marked, attribute_words(*marked));

    const bool defined = function ? peek(p).kind == tok_t::lbrace :
                                    !declared(spec, tok_t::kw_extern) ||
                                        peek(p).kind == tok_t::assign;
    if (defined)
        fail(*d.name, "definition of " + quoted(d.name->text) +
                          " cannot also be an alias");

    alias_target(p, attribute);
    p.aliases.push_back(pending_alias_t { d.name, &attribute, function });
    tail.marking = nullptr;
}

// Declares a function at file scope, or meets its earlier declaration,
// there or in a block, with what the declaration writes after its
// declarator, and gives it the declaration's linkage.  A marking
// attribute on a parameter marks the function, whose calls pass the
// parameter.  Returns the index of its entry in the unit's functions.
std::size_t
declare_function(parser_t &p,
                 const specifiers_t &spec,
                 const declarator_t &d,
                 declarator_tail_t tail)
{
    if (tail.markings.alias)
        declared_alias(p, spec, d, tail, true);

    if (!tail.marking)
        tail.marking = marked_parameter(d);

    adopt_block_entity(p, d.name->text);

    symbol_t *earlier =
        earlier_declaration(p, *d.name, d.type, storage_t::function);
    const linkage_t linkage = linkage_of(spec, earlier, true);

    if (earlier) {
        require_linkage(*d.name, *earlier, linkage);
        record_tail(*earlier, tail);

        for (std::size_t i = 0; i < p.unit.functions.size(); ++i)
            if (p.unit.functions[i].sym == earlier)
                return i;
    }

    symbol_t &sym = added(p, d.name->text, d.type, storage_t::function);
    sym.linkage = linkage;
    record_tail(sym, tail);
    p.unit.functions.push_back(function_decl_t { &sym, { }, std::nullopt, 0 });

    return p.unit.functions.size() - 1;
}

std::optional< initializer_t > address_constant(const expr_t &);

// The address of an lvalue that designates an object of static storage
// or a function, the operand an address constant is made from, 6.6p11:
// a global's name, whether the unit defines it or only declares it, a
// function's, a string literal, 6.4.5p6, or *E for an address constant
// E, as a subscript by a constant is, p12.  Nothing for any other
// lvalue: a local, or *p, which reads the value of the object p.
std::optional< initializer_t >
static_address(const expr_t &e)
{
    if (is< name_t >(e)) {
        const symbol_t *sym = as< name_t >(e).sym;
        if (sym->storage != storage_t::global &&
            sym->storage != storage_t::function)
            return std::nullopt;

        return address_t { sym, 0 };
    }

    if (is< string_t >(e))
        return address_t { as< string_t >(e), 0 };

    if (is< unary_t >(e) && as< unary_t >(e).op == unary_op_t::deref)
        return address_constant(*as< unary_t >(e).arg);

    return std::nullopt;
}

// An address constant moved by a number of bytes, modulo 2^64 (G6): the
// offset from its base, or the integer of one cast from an integer.
initializer_t
moved(initializer_t a, unsigned long long bytes)
{
    const auto sum = [bytes](long long v) {
        return static_cast< long long >(static_cast< unsigned long long >(v) +
                                        bytes);
    };

    if (auto *constant = std::get_if< literal_t >(&a))
        constant->value = sum(constant->value);
    else
        std::get< address_t >(a).offset = sum(std::get< address_t >(a).offset);

    return a;
}

// A pointer expression folded to the address constant it is, 6.6p9,
// p11, p12, or nothing when it is none: & of an lvalue static_address()
// takes, the decay of an array or of a function designator it takes, a
// pointer cast of an address constant, and one plus or minus an integer
// constant, whose bytes are the constant times the pointee's size, as
// the subscript of a constant is.  The parser made an integer constant
// cast to a pointer a literal of the pointer's type, which is one too.
// A cast of an address constant to an integer of a pointer's width is
// the address as an integer, which 6.6p14 lets an implementation take.
std::optional< initializer_t >
address_constant(const expr_t &e)
{
    if (is< literal_t >(e) && is< pointer_t >(e.type))
        return as< literal_t >(e);

    if (is< unary_t >(e) && as< unary_t >(e).op == unary_op_t::addr)
        return static_address(*as< unary_t >(e).arg);

    if (is< cast_t >(e)) {
        const expr_t &arg = *as< cast_t >(e).arg;
        if (is< array_t >(arg.type) || is< function_t >(arg.type))
            return static_address(arg);

        if (!is< pointer_t >(arg.type) || size_of(e.type) != 8)
            return std::nullopt;

        return address_constant(arg);
    }

    if (!is< binary_t >(e) || !is< pointer_t >(e.type))
        return std::nullopt;

    const auto &b = as< binary_t >(e);
    if (b.op != binary_op_t::add && b.op != binary_op_t::sub)
        return std::nullopt;

    const bool left = is< pointer_t >(b.lhs->type);
    const expr_t &index = left ? *b.rhs : *b.lhs;

    const std::optional< initializer_t > base =
        address_constant(left ? *b.lhs : *b.rhs);
    if (!base || !is< literal_t >(index))
        return std::nullopt;

    const auto bytes =
        static_cast< unsigned long long >(as< literal_t >(index).value) *
        static_cast< unsigned long long >(size_of(pointee_of(e.type)));

    return moved(*base, b.op == binary_op_t::sub ? 0 - bytes : bytes);
}

// Whether a scalar's initializer is a pointer cast to an integer
// narrower than a pointer, (int) &n: the address is known only when the
// program is loaded, and the loader fills in a pointer's 64 bits and no
// narrower field.  An integer constant cast to a pointer and back is
// such a cast too, which is no integer constant expression, 6.6p8.
bool
narrowed_address(const expr_t &init)
{
    if (!is< cast_t >(init) || !is< int_t >(init.type) ||
        size_of(init.type) == 8)
        return false;

    const expr_t &arg = *as< cast_t >(init).arg;

    return is< pointer_t >(arg.type) && address_constant(arg);
}

// Whether a type is an array of char, signed char or unsigned char,
// which a string literal may initialize, 6.7.11p15.
bool
is_char_array(const type_t &t)
{
    if (!is< array_t >(t))
        return false;

    const type_t &element = *as< array_t >(t).element;

    return is< int_t >(element) && as< int_t >(element).width == 8;
}

// The words of an array a string literal may initialize, in a message.
std::string_view
string_array_words(const type_t &t)
{
    return is_char_array(t) ? "char array" : "wide char array";
}

// Whether the tokens ahead are a string literal alone in its place in a
// list, "abc" or "ab" "c" before a ',' or the '}', that initializes the
// array t whole: an array of char takes one of no prefix or of u8,
// 6.7.11p15, and an array of a wide literal's element type takes that
// literal, p16, so on x86-64 an array of int takes L"..." and on aarch64
// one of unsigned int takes L"..." and U"...".  Any other expression
// there initializes the array's first element.  A wide literal ahead of
// an array of char is the literal string_value() refuses.
bool
string_ahead(const parser_t &p, const type_t &t)
{
    std::size_t ahead = 0;
    encoding_t encoding = encoding_t::none;
    for (; peek(p, ahead).kind == tok_t::string; ++ahead)
        if (encoding_of(peek(p, ahead)) != encoding_t::none)
            encoding = encoding_of(peek(p, ahead));

    const tok_t after = peek(p, ahead).kind;
    if (ahead == 0 || (after != tok_t::comma && after != tok_t::rbrace))
        return false;

    if (is_char_array(t))
        return true;

    const type_t element = element_type(p, encoding);

    return size_of(element) > 1 && *as< array_t >(t).element == element;
}

// A designation, [2] = or .x =, 6.7.11p1, is not yet.
void
refuse_designator(const parser_t &p)
{
    if (next_is(p, { tok_t::lbracket, tok_t::dot }))
        not_yet(peek(p), "designator");
}

// The value of a scalar at file scope, the expression ahead.  Every
// expression in an initializer of an object of static storage is a
// constant expression, 6.7.11p5, converted to the scalar's type as by
// simple assignment, p12, and folded: an integer constant expression,
// or a null pointer, which the evaluator made a literal; an address
// constant, 6.6p9, p11.  Any other expression is no constant
// expression, an error, and so is an address in an integer narrower
// than a pointer, which no load writes.  A record's value is none
// either, p14: in a scalar's place it initializes a record the scalar
// begins, or no object at all.  at is the token a conversion's error
// is said at; a void expression, which has no value, 6.3.2.2p1, is the
// error of the expression, said at its first token.
initializer_t
scalar_value(parser_t &p, const token_t &at, const type_t &t)
{
    const token_t &start = peek(p);
    expr_t e = assignment_expression(p);
    if (is< record_type_t >(e.type))
        fail(start, "initializer element is not a compile-time constant");

    require_value(start, e);

    expr_t init = assignment_conversion(at, std::move(e), t);

    if (is< literal_t >(init))
        return as< literal_t >(init);

    if (narrowed_address(init))
        fail(start, "initializer element is an address cast to an integer "
                    "narrower than a pointer");

    if (std::optional< initializer_t > address = address_constant(init))
        return std::move(*address);

    fail(start, "initializer element is not a compile-time constant");
}

// Where the walk over an initializer puts each value it finds,
// 6.7.11p18.  For an object of static storage it is the object's data:
// each subobject's constant at its offset in the object, which c1
// writes (shape.md chapter 4).  For a local it is the statements after
// the declaration of the object local, stores: each scalar's value
// assigned to its place in the object, in the list's order (G13).
struct destination_t
{
    std::vector< subobject_t > *data = nullptr;
    const symbol_t *local = nullptr;
    std::vector< stmt_t > *stores = nullptr;
};

// The store of the scalar t at offset in a local, the expression ahead
// converted to t as by simple assignment, 6.7.11p12, at the token a
// conversion's error is said at: the assignment of the value to the
// place *(T *) ((char *) &local + offset), lvalue_at()'s, appended to
// the local's statements.  The parser makes the node, since initialization
// is no assignment and p12 takes the scalar's unqualified type.  A
// record's value in a scalar's place, which may initialize a record the
// scalar begins, p14, is not yet; a void expression has no value,
// 6.3.2.2p1, the error of the expression said at its first token.
void
local_store(parser_t &p,
            const token_t &at,
            const type_t &t,
            long offset,
            destination_t &to)
{
    const token_t &start = peek(p);
    expr_t e = assignment_expression(p);
    refuse_record_value(start, e.type);
    require_value(start, e);

    expr_t value = assignment_conversion(at, std::move(e), t);

    expr_t object { to.local->type, name_t { to.local } };
    expr_t place = lvalue_at(p, address_of(at, std::move(object)), offset, t);

    expr_t store { t, assign_t { std::nullopt, t, box(std::move(place)),
                                 box(std::move(value)) } };
    to.stores->push_back(stmt_t { expr_stmt_t { std::move(store) } });
}

// The scalar t at offset, given the value of the expression ahead: a
// constant in the data of an object of static storage, a store in a
// local.
void
scalar_subobject(parser_t &p,
                 const token_t &at,
                 const type_t &t,
                 long offset,
                 destination_t &to)
{
    if (to.local) {
        local_store(p, at, t, offset, to);
        return;
    }

    initializer_t value = scalar_value(p, at, t);
    to.data->push_back(subobject_t { offset, t, std::move(value) });
}

// Whether the string literal e initializes the array t whole, as
// string_ahead() says of one in a list: an array of char takes any,
// which string_value() checks, and any other array one of its element
// type.
bool
initializes(const type_t &t, const expr_t &e)
{
    return is_char_array(t) ||
           *as< array_t >(t).element == *as< array_t >(e.type).element;
}

// A string literal that initializes the array t at offset, of char,
// 6.7.11p15, or of a wide literal's element type, p16: its elements, and
// its terminating zero when the array has room for it or is of unknown
// size, which takes the literal's size; the elements past the literal's
// are zero, p22.  A literal longer than the array is an initializer for
// an object not contained in it, p3.  A wide literal initializes no
// array of char, p15.  Returns the count of elements the literal gives
// an array of unknown size, its elements and the zero.
long
string_value(const token_t &start,
             const string_t &str,
             const type_t &t,
             long offset,
             std::vector< subobject_t > &values)
{
    const auto &array = as< array_t >(t);
    const long given = long(str.bytes.size()) / str.width + 1;

    if (is_char_array(t) && str.width > 1)
        fail(start, "initializing a char array with a wide string literal");

    if (array.count >= 0 && given - 1 > array.count)
        fail(start, "initializer-string for " +
                        std::string(string_array_words(t)) + " is too long");

    const type_t sized =
        array.count >= 0 ? t : make_array(*array.element, given);
    values.push_back(subobject_t { offset, sized, str });

    return given;
}

// The string literal ahead, which string_ahead() found, for the array t
// at offset.  An object of static storage holds the literal's bytes as
// the array's own; a local's array would take them at run time, which
// is not yet.
long
listed_string(parser_t &p, const type_t &t, long offset, destination_t &to)
{
    const token_t &start = peek(p);
    if (to.local)
        not_yet(start, "string initializer of an array in local " +
                           record_words(*record_in(to.local->type)));

    const expr_t e = assignment_expression(p);

    return string_value(start, as< string_t >(e), t, offset, *to.data);
}

// Takes the ',' before another initializer of the list, and says
// whether one follows; a ',' before the '}' ends the list, 6.7.11p1,
// and is left for it.
bool
accept_separator(parser_t &p)
{
    if (peek(p).kind != tok_t::comma || peek(p, 1).kind == tok_t::rbrace)
        return false;

    advance(p);

    return true;
}

long
subobject_initializer(parser_t &, const type_t &, long offset, destination_t &);

// The elements of an array at offset, from the initializers ahead in a
// list, in increasing subscript order, 6.7.11p18: each element from
// the next initializer, or from as many as it takes when that one does
// not begin with a brace, p21.  An initializer is ahead.  The elements
// end with the array's count, leaving the rest of the list to the
// enclosing aggregate or to the list's own check, or with the list;
// an array of unknown size ends only with the list.  Returns the count
// of elements given, which completes such an array, p23.
long
elements(parser_t &p, const array_t &array, long offset, destination_t &to)
{
    const type_t &element = *array.element;
    const long size = size_of(element);

    long count = 0;
    do {
        subobject_initializer(p, element, offset + count * size, to);
        ++count;
    } while (count != array.count && accept_separator(p));

    return count;
}

// The members of a record at offset, from the initializers ahead in a
// list, 6.7.11p18: a struct's in declaration order, each from the next
// initializer, or from as many as it takes when that one does not
// begin with a brace, p21; a union's first named member alone.  Every
// member of a record rcc lays out is named, so none is passed over,
// p10.  An initializer is ahead.  The members end with the last,
// leaving the rest of the list to the enclosing aggregate or to the
// list's own check, or with the list.  Returns whether every member the
// list may give was given.
bool
member_values(parser_t &p, const record_t &r, long offset, destination_t &to)
{
    const std::size_t count = r.is_union ? 1 : r.members.size();

    std::size_t given = 0;
    do {
        const member_t &m = r.members[given];
        subobject_initializer(p, m.type, offset + m.offset, to);
        ++given;
    } while (given != count && accept_separator(p));

    return given == count;
}

// The end of a brace-enclosed list after its initializers: a ',' may
// stand before the '}', 6.7.11p1.  A list that went on once its object
// held every value it gives would be an initializer for an object not
// contained in it, p3, said with what the object is.
void
close_list(parser_t &p, bool full, std::string_view what)
{
    accept(p, tok_t::comma);

    if (full && peek(p).kind != tok_t::rbrace)
        fail(peek(p),
             "excess elements in " + std::string(what) + " initializer");

    expect(p, tok_t::rbrace, "expected '}'");
}

// A brace-enclosed initializer of an object or a subobject of type t at
// offset, its '{' ahead.  The empty initializer, {}, leaves it zero,
// 6.7.11p11, but an array of unknown size may not take it, p4.  A
// scalar takes one expression, p12, and no braces of its own inside
// them; a record its members, p17, p18; an array of char a string
// literal alone, p15; any other array its elements, p17.  Returns the
// count of elements given to an array.
long
braced_initializer(parser_t &p, const type_t &t, long offset, destination_t &to)
{
    const token_t &brace = advance(p);

    if (peek(p).kind == tok_t::rbrace) {
        if (is< array_t >(t) && as< array_t >(t).count < 0)
            fail(brace, "empty initializer for an array of unknown size");

        advance(p);

        return 0;
    }

    refuse_designator(p);

    if (is< record_type_t >(t)) {
        const record_t &r = *as< record_type_t >(t).record;
        const bool full = member_values(p, r, offset, to);
        close_list(p, full, r.is_union ? "union" : "struct");

        return 0;
    }

    if (!is< array_t >(t)) {
        if (peek(p).kind == tok_t::lbrace)
            fail(peek(p), "too many braces around scalar initializer");

        scalar_subobject(p, peek(p), t, offset, to);
        close_list(p, true, "scalar");

        return 0;
    }

    if (string_ahead(p, t)) {
        const long count = listed_string(p, t, offset, to);
        close_list(p, true, string_array_words(t));

        return count;
    }

    const auto &array = as< array_t >(t);
    const long count = elements(p, array, offset, to);
    close_list(p, count == array.count, "array");

    return count;
}

// The initializer of a subobject of type t at offset from its place in
// a list, 6.7.11p21: one that begins with a brace is its own; a scalar
// takes the one expression ahead; an array of char a string literal
// alone; a record, and any other array, takes as many initializers
// from the list as its members or its elements do.  Returns the count
// of elements given to an array.
long
subobject_initializer(parser_t &p,
                      const type_t &t,
                      long offset,
                      destination_t &to)
{
    refuse_designator(p);

    if (peek(p).kind == tok_t::lbrace)
        return braced_initializer(p, t, offset, to);

    if (is< record_type_t >(t)) {
        member_values(p, *as< record_type_t >(t).record, offset, to);
        return 0;
    }

    if (!is< array_t >(t)) {
        scalar_subobject(p, peek(p), t, offset, to);
        return 0;
    }

    if (string_ahead(p, t))
        return listed_string(p, t, offset, to);

    return elements(p, as< array_t >(t), offset, to);
}

// The initializer without braces of an object of the record t, the
// expression ahead: one of a compatible record type, 6.7.11p14, whose
// value is no constant expression, p5, so that no object the unit
// defines takes it; any other initializes no record, p17.
[[noreturn]] void
refuse_record_expression(parser_t &p, const type_t &t)
{
    const token_t &start = peek(p);
    const expr_t e = assignment_expression(p);

    refuse_tag_twins(start, t, e.type);
    if (e.type == t)
        fail(start, "initializer element is not a compile-time constant");

    fail(start, "initializing " + record_words(*as< record_type_t >(t).record) +
                    " with an expression of incompatible type");
}

// The initializer of an object the unit defines, after its '=', into
// the subobjects it gives a value.  A brace-enclosed one is as
// braced_initializer() takes it.  Without braces, a scalar takes one
// expression, converted from the '=', 6.7.11p12, and an array a string
// literal alone, for an array of char, p15: any other array takes a
// brace-enclosed list, p17, and so does a record, which takes no
// expression of its own type here.  Returns the count of elements given
// to an array, which completes one of unknown size, p23.
long
object_initializer(parser_t &p,
                   const token_t &op,
                   const type_t &t,
                   std::vector< subobject_t > &values)
{
    destination_t to { &values };

    if (peek(p).kind == tok_t::lbrace)
        return braced_initializer(p, t, 0, to);

    if (is< record_type_t >(t))
        refuse_record_expression(p, t);

    if (!is< array_t >(t)) {
        scalar_subobject(p, op, t, 0, to);
        return 0;
    }

    const token_t &start = peek(p);
    const expr_t e = assignment_expression(p);

    if (!is< string_t >(e) || !initializes(t, e))
        fail(start, "array initializer must be an initializer list");

    return string_value(start, as< string_t >(e), t, 0, values);
}

// The entry of the unit's objects that defines sym, its index, or
// nothing when no declaration has defined the object yet.
std::optional< std::size_t >
definition_of(const parser_t &p, const symbol_t *sym)
{
    for (std::size_t i = 0; i < p.unit.objects.size(); ++i)
        if (p.unit.objects[i].sym == sym)
            return i;

    return std::nullopt;
}

// The record of an object's type, or of its array's element, when the
// record is incomplete, or null.
const record_t *
incomplete_record_in(const type_t &t)
{
    const record_t *r = record_in(t);

    return r && !r->complete ? r : nullptr;
}

// An object of no linkage is of a complete type by the end of its
// declarator, 6.7.1p8: an incomplete record is an error, an array of
// one among them.
void
require_complete_record(const token_t &name, const type_t &t)
{
    if (const record_t *r = incomplete_record_in(t))
        fail(name, "variable " + quoted(name.text) + " has incomplete type " +
                       record_words(*r));
}

// An object the unit defines is one rcc makes, and its type is
// complete, 6.7.1p8: not void, not a not-yet type, whose size rcc does
// not know.  An array may leave its size out: its initializer gives it,
// 6.7.11p23, or a later declaration, 6.2.7p5, and an array of unknown
// size still at the end of the unit is one element, 6.9.3p2, if its
// linkage is external; of internal linkage it is an error then, p3,
// which require_completed_definitions() checks.  An attribute that
// marks the object, said on any of its declarations, is one rcc would
// have to honour, and is not yet.  An object of a record incomplete
// where it is declared is an error with an initializer, 6.7.11p4, and
// in a tentative definition of internal linkage, 6.9.3p3; a tentative
// definition of external linkage takes the record as it is at the end
// of the unit, 6.9.3p2, which require_completed_definitions() checks
// then.
void
require_definable(parser_t &p,
                  const token_t &name,
                  const symbol_t &sym,
                  bool initialized)
{
    if (is< void_t >(sym.type))
        fail(name, "variable " + quoted(name.text) + " has type void");

    require_honoured(name, sym.type);

    if (incomplete_record_in(sym.type)) {
        if (initialized || sym.linkage == linkage_t::internal)
            require_complete_record(name, sym.type);

        p.incomplete_definitions.push_back({ &sym, &name });
    }

    const bool unknown_size =
        is< array_t >(sym.type) && as< array_t >(sym.type).count < 0;
    if (unknown_size && !initialized && sym.linkage == linkage_t::internal)
        p.incomplete_definitions.push_back({ &sym, &name });

    if (!sym.marking.empty())
        not_yet(name, sym.marking);
}

// 6.9.3p2: a tentative definition of an object of a record incomplete
// where it was written takes the record as it is at the end of the
// unit, which must be complete, and laid out for rcc to make the object.
// One of internal linkage is not of an incomplete type, p3.  p3 says
// the declared type; rcc takes the composite type at the end of the
// unit, so static int a[]; static int a[3]; declares the int [3] it
// does elsewhere, 6.2.7p5, and an array no declaration sizes is an
// error at the name of its first tentative definition, where one of
// external linkage is one element.  The check comes before
// complete_tentative_arrays() gives it that element.
void
require_completed_definitions(const parser_t &p)
{
    for (const auto &[sym, at] : p.incomplete_definitions) {
        if (const record_t *r = incomplete_record_in(sym->type))
            fail(*at, "tentative definition of " + quoted(sym->name) +
                          " has incomplete type " + record_words(*r));

        if (is< array_t >(sym->type) && as< array_t >(sym->type).count < 0)
            fail(*at, "static array " + quoted(sym->name) +
                          " is never given a size");

        require_honoured(*at, sym->type);
    }
}

// The type of an array of unknown size that an initializer completes,
// 6.7.11p23: count elements, the elements its list gives or a string's
// bytes and its terminating zero, p15.  Any other type is complete and
// stays as it is.
type_t
completed_type(const type_t &t, long count)
{
    if (!is< array_t >(t) || as< array_t >(t).count >= 0)
        return t;

    return make_array(*as< array_t >(t).element, count);
}

// 6.9.3p2: an object the unit defines whose type is still an array of
// unknown size at the end of the unit, when no declaration has given
// the size, is one element, which is zero; its definitions are all
// tentative, since an initializer gives the size.
void
complete_tentative_arrays(parser_t &p)
{
    for (symbol_t &sym : p.unit.symbols) {
        if (sym.storage != storage_t::global || !is< array_t >(sym.type))
            continue;

        const auto &array = as< array_t >(sym.type);
        if (array.count < 0 && definition_of(p, &sym))
            sym.type = make_array(*array.element, 1);
    }
}

// The initializer after the '=' ahead of the object the unit's objects
// hold at index: the subobjects it gives a value, and the size it gives
// an array of unknown size, 6.7.11p23.
void
initialize(parser_t &p, symbol_t &sym, std::size_t index)
{
    const token_t &op = advance(p);
    std::vector< subobject_t > values;
    const long count = object_initializer(p, op, sym.type, values);

    sym.type = completed_type(sym.type, count);
    p.unit.objects[index].init = std::move(values);
}

// Whether the unit defines sym: a function by its body, an object by
// its entry in the unit's objects.
bool
defines(const parser_t &p, const symbol_t *sym)
{
    for (const function_decl_t &f : p.unit.functions)
        if (f.sym == sym)
            return f.body.has_value();

    return definition_of(p, sym).has_value();
}

// 6.9.1p3: a name of internal linkage that an expression uses has its
// external definition in the unit.  Without one the assembler would
// leave its symbol undefined, and the linker would bind it to another
// file's symbol of the name, which is not the program.  Only a
// function can lack one, since every object of internal linkage has a
// tentative definition at least.  A use of a system header's static
// inline function before its definition, which rcc does not compile,
// is not yet, in the function's words.  The first use is the one
// refused.
void
require_internal_definitions(const parser_t &p)
{
    for (const auto &[sym, at] : p.internal_uses) {
        if (defines(p, sym))
            continue;

        if (!sym->marking.empty())
            not_yet(*at, sym->marking);

        fail(*at, "static function " + quoted(sym->name) +
                      " is used but never defined");
    }
}

// The symbol an alias names, at file scope: a function or an object the
// unit defines, of the alias's kind, with no assembler name, since
// GNU C reads the name as the target's assembly name.  A name the unit
// does not define, a system header's static inline function among
// them, whose body rcc skips, leaves the alias not yet; a target of the
// other kind is an error.
const symbol_t &
alias_of(const parser_t &p, const pending_alias_t &a, const std::string &target)
{
    const auto &names = p.scopes.front().names;
    const auto it = names.find(target);
    const symbol_t *t = it == names.end() ? nullptr : it->second;

    const std::string alias = "alias " + quoted(a.name->text);
    if (!t || !defines(p, t))
        not_yet(*a.name, alias + " of " + quoted(target) +
                             ", which the unit does not define");

    if (a.function != (t->storage == storage_t::function))
        fail(*a.name, alias + " of " +
                          (a.function ? "a function" : "an object") +
                          " names " + quoted(target) + ", which is not one");

    if (!t->asm_name.empty())
        not_yet(*a.name, alias + " of " + quoted(target) +
                             ", which has an assembler name");

    return *t;
}

// The aliases the unit declares, checked once its definitions are all
// read, and each added to the unit with its target: the alias is of
// external linkage, which a static alias is not yet, has no assembler
// name, and is defined by no declaration, as a second alias of it would
// define it again.
void
resolve_aliases(parser_t &p)
{
    for (std::size_t i = 0; i < p.aliases.size(); ++i) {
        const pending_alias_t &a = p.aliases[i];
        const symbol_t &sym = *p.scopes.front().names.at(a.name->text);
        const std::string alias = "alias " + quoted(a.name->text);

        if (sym.linkage != linkage_t::external)
            not_yet(*a.name, alias + " of internal linkage");

        if (!sym.asm_name.empty())
            not_yet(*a.name, alias + " with an assembler name");

        bool defined = defines(p, &sym);
        for (std::size_t k = 0; k < i; ++k)
            defined |= p.aliases[k].name->text == a.name->text;

        if (defined)
            fail(*a.name, "redefinition of " + quoted(a.name->text));

        const symbol_t &target = alias_of(p, a, alias_target(p, *a.attribute));
        p.unit.aliases.push_back(alias_t { &sym, &target });
    }
}

// An object at file scope, 6.9.3.  A declaration with an initializer
// defines it, p1, extern or not; one without an initializer and
// without extern is a tentative definition, p2; extern alone, neither
// of the two, declares an object defined elsewhere, and c1 writes
// nothing for it.  Every
// declaration of the name in the unit is of one object, of one linkage,
// linkage_of()'s, and of one type, 6.2.7p2: extern int n; int n; int n
// = 1; declare one object of external linkage, static int n; extern int
// n; one of internal linkage, which a tentative definition defines,
// and two tentative definitions define one.  An
// array declared without its size and with it is one object of the
// composite type, the array of known size, p5: extern int a[]; int
// a[3]; and int a[3]; extern int a[]; declare an int [3].  An
// object only tentatively defined is zero, 6.9.3p2.  A second
// initializer is a second external definition, of which a program has
// one, 6.9.1p5, an error.  The name is in scope from the end of its
// declarator, 6.2.1p7, so an initializer sees the object.  A use of an
// object an attribute marks is refused where the object is named.
void
declare_external_object(parser_t &p,
                        const specifiers_t &spec,
                        const declarator_t &d)
{
    declarator_tail_t tail = declarator_tail(p, spec);
    if (tail.markings.alias)
        declared_alias(p, spec, d, tail, false);

    adopt_block_entity(p, d.name->text);

    symbol_t *sym = earlier_declaration(p, *d.name, d.type, storage_t::global);
    const linkage_t linkage = linkage_of(spec, sym, false);

    if (!sym) {
        sym = &added(p, d.name->text, d.type, storage_t::global);
        sym->linkage = linkage;
    }

    require_linkage(*d.name, *sym, linkage);
    record_tail(*sym, tail);

    const bool initialized = peek(p).kind == tok_t::assign;
    if (!initialized && declared(spec, tok_t::kw_extern))
        return;

    require_definable(p, *d.name, *sym, initialized);

    std::optional< std::size_t > index = definition_of(p, sym);
    if (index && initialized && p.unit.objects[*index].init)
        fail(*d.name, "redefinition of " + quoted(d.name->text));

    if (!index) {
        index = p.unit.objects.size();
        p.unit.objects.push_back(object_decl_t { sym, std::nullopt });
    }

    if (initialized)
        initialize(p, *sym, *index);
}

// No function specifier stands in a declaration of main, 6.7.5p4.
void
refuse_specifier_on_main(const specifiers_t &spec, const declarator_t &d)
{
    const token_t *specifier = spec.function_specifier;
    if (specifier && d.name->text == "main")
        fail(*specifier,
             quoted(specifier->text) + " in a declaration of 'main'");
}

// The function called at program startup is named main, 5.1.2.3.2p1,
// and is found by that name from outside the unit, which static, giving
// it internal linkage, 6.2.2p3, would forbid: the link would find no
// main.  An object named main is not the function and is let be.
void
refuse_static_main(const specifiers_t &spec, const declarator_t &d)
{
    if (declared(spec, tok_t::kw_static) && d.name->text == "main" &&
        is< function_t >(d.type))
        fail(*spec.storage_class,
             quoted(spec.storage_class->text) + " in a declaration of 'main'");
}

// Whether a static declaration is a system header's, by the line
// marker in force at its static.
bool
in_system_header(const specifiers_t &spec)
{
    return spec.storage_class->where.system_header;
}

// One declarator of a file-scope declaration: a type name, a function,
// or an object.  static inline on a declaration without a body is a
// static function's declaration in the program's own source, inline
// dropped as on any declaration; in a system header it is not yet,
// since only the definition of a system header's static inline
// function is taken there.  A function declared through a typedef name
// of function type, typedef int fn_t(int); fn_t f;, is not yet: its
// declaration writes no parameter list, and the typedef keeps no
// marking attribute of its parameters.
void
external_declarator(parser_t &p,
                    const specifiers_t &spec,
                    const declarator_t &d)
{
    const bool typedef_ = declared(spec, tok_t::kw_typedef);
    if (!typedef_ && !d.params && is< function_t >(d.type))
        not_yet(*d.name, "function " + quoted(d.name->text) +
                             " declared through a typedef");

    if (typedef_ || !d.params)
        refuse_function_specifier(spec);

    refuse_specifier_on_main(spec, d);
    refuse_static_main(spec, d);

    if (declared(spec, tok_t::kw_static) && spec.inline_specifier &&
        in_system_header(spec))
        not_yet(*spec.storage_class,
                "storage class " + quoted(spec.storage_class->text));

    if (typedef_)
        type_definition(p, spec, d);
    else if (d.params)
        declare_function(p, spec, d, declarator_tail(p, spec));
    else
        declare_external_object(p, spec, d);
}

// The string of a static assertion, adjacent literals joined as in an
// expression, 6.4.5p5, or empty without one.
std::string
assertion_message(parser_t &p)
{
    if (!accept(p, tok_t::comma))
        return { };

    if (peek(p).kind != tok_t::string)
        fail(peek(p), "expected string literal in static assertion");

    std::string message;
    while (peek(p).kind == tok_t::string)
        message += string_bytes(advance(p));

    return message;
}

// _Static_assert ( constant-expression , string-literal ) ;, the string
// optional, 6.7.12p1; the keyword is C23's alternative spelling of
// static_assert, 6.4.1p3, and C17's own, and a program that includes
// assert.h writes static_assert, which the header's macro spells so.  A
// declaration of its own, at file scope, in a block or as a for's first
// clause, which declares nothing, so the tree keeps nothing of it and c1
// writes nothing.  The expression is an integer constant expression,
// p3, folded as an array's size is, and one that needs what rcc does
// not have yet is refused by its words; one that compares equal to 0
// violates p2, and the error says the string, p3.
void
static_assert_declaration(parser_t &p)
{
    const token_t &keyword = advance(p);
    expect(p, tok_t::lparen, "expected '(' after " + quoted(keyword.text));

    const token_t &start = peek(p);
    const expr_t e = rvalue(start, conditional_expression(p));
    if (!is_integer_constant(e))
        fail(start, "static assertion is not an integer constant expression");

    const std::string message = assertion_message(p);
    expect(p, tok_t::rparen, "expected ')' in static assertion");
    expect(p, tok_t::semicolon, "expected ';' after static assertion");

    if (exact_value(e) == 0)
        fail(start, "static assertion failed" +
                        (message.empty() ? "" : ": " + message));
}

// A function definition, or a declaration of functions, of objects
// and of type names, or of a tag or enumeration constants alone, or a
// static assertion.  extern on a function changes nothing: a function
// declared without a storage class has the linkage extern gives it,
// 6.2.2p5, and static gives it internal linkage, p3, on a declaration
// and on a definition alike.  A definition is a declarator and
// its body, with nothing between them, so no attribute and no
// assembler name of its own; it takes those of its declarations.
// inline on a definition without static is not yet, wherever it stands
// among the specifiers: an inline definition of a function of external
// linkage is no external definition, 6.7.5p7, which rcc would have to
// honour.  _Noreturn on one is dropped, as on a declaration: it
// promises that the function does not return, and rcc writes the
// function as it is written either way.  A static inline function may
// be an inline function, p7, and its definition in the program's own
// source is compiled as the static function it is, inline dropped;
// one a system header writes is accepted and not compiled.  A
// definition of main takes no function specifier, 6.7.5p4, and no
// static, as no declaration of it does.  An __asm__ at file scope is
// not yet.
void
external_declaration(parser_t &p)
{
    if (peek(p).kind == tok_t::kw_asm)
        not_yet(peek(p), quoted(peek(p).text) + " at file scope");

    skip_extension_keywords(p);

    if (peek(p).kind == tok_t::kw_static_assert) {
        static_assert_declaration(p);
        return;
    }

    const specifiers_t spec = declaration_specifiers(p);

    // 6.9.1p2: register shall not appear in an external declaration
    if (declared(spec, tok_t::kw_register))
        fail(*spec.storage_class,
             quoted(spec.storage_class->text) + " in an external declaration");

    if (spec.tagged && accept(p, tok_t::semicolon))
        return;

    const type_t &base = spec.type;
    const bool typedef_ = declared(spec, tok_t::kw_typedef);

    const declarator_t d = declarator(p, base, naming_t::required, typedef_);

    const bool defines_function = d.params && peek(p).kind == tok_t::lbrace;
    if (defines_function && !typedef_) {
        refuse_specifier_on_main(spec, d);
        refuse_static_main(spec, d);
    }

    if (defines_function && declared(spec, tok_t::kw_static) &&
        spec.inline_specifier) {
        if (in_system_header(spec))
            header_function_definition(p, spec, d);
        else
            function_definition(p, spec, d);

        return;
    }

    if (defines_function && !typedef_) {
        if (const token_t *t = spec.inline_specifier)
            not_yet(*t, quoted(t->text) + " on a function definition");

        function_definition(p, spec, d);
        return;
    }

    external_declarator(p, spec, d);

    while (accept(p, tok_t::comma))
        external_declarator(p, spec,
                            declarator(p, base, naming_t::required, typedef_));

    expect(p, tok_t::semicolon, "expected ';' after top level declarator");
}

// Each goto of the function names a label the function defines,
// 6.8.7.2p1, before the goto or after it, since a label has function
// scope, 6.2.1p3.  So the check waits for the function's end, and the
// first goto that names none is the error.
void
require_labels(const parser_t &p)
{
    for (const token_t *name : p.gotos)
        if (!p.labels.contains(name->text))
            fail(*name,
                 "label " + quoted(name->text) + " used but not defined");
}

// A parameter of a definition, declared already in the scope of its
// list, which is open again as the body's, takes its register in the
// order of the parameters, as a local does.  An aarch64 va_list arrives
// as the address of the caller's copy, is_list_by_reference(): the
// symbol is that pointer, and its name reads as the list it points to.
const symbol_t *
defined_parameter(parser_t &p, const parameter_t &param)
{
    symbol_t &sym = *p.scopes.back().names.at(param.name->text);

    sym.declared_register = param.in_register;
    sym.register_index = register_index(p, *param.name, param.in_register);

    if (is_list_by_reference(p, sym.type)) {
        sym.type = make_pointer(sym.type);
        sym.by_reference = true;
    }

    return &sym;
}

// The parameters of a definition, each named, defined_parameter()'s.
// Each is an object the definition makes, so none may be of a not-yet
// type, nor of a record, which passes by value in no rule rcc has yet
// but the aarch64 va_list's.  No array of unspecified size, [*], is
// part of the declaration of one, 6.9.2p6.
std::vector< const symbol_t * >
defined_parameters(parser_t &p, const declarator_t &d)
{
    std::vector< const symbol_t * > params;
    for (const parameter_t &param : *d.params) {
        if (!param.name)
            fail(*param.start, "parameter name omitted");

        if (param.star)
            fail(*param.star, "'*' as an array size in a function definition");

        require_honoured(*param.name, param.type);

        const record_t *r = record_in(param.type);
        if (r && !is_list_by_reference(p, param.type))
            not_yet(*param.name, record_words(*r) + " passed by value");

        params.push_back(defined_parameter(p, param));
    }

    return params;
}

// The temporaries the body's builtins made, list_temporary()'s, each
// declared at the head of the body without an initializer, so that
// each has a slot for the activation, G12, as a local the body declared
// there would; the body is parsed with them, and file scope without.
void
declare_temporaries(parser_t &p, block_t &body)
{
    std::vector< stmt_t > declarations;
    for (const symbol_t *t : *p.temporaries)
        declarations.push_back(stmt_t { decl_stmt_t { t, std::nullopt } });

    body.body.insert(body.body.begin(),
                     std::make_move_iterator(declarations.begin()),
                     std::make_move_iterator(declarations.end()));
    p.temporaries.reset();
}

// The words a system header's static inline definition leaves on its
// function, whose body is skipped: a call of it, which would need the
// body compiled, is refused by them.
std::string
skipped_body_words(std::string_view name)
{
    return "static function " + quoted(name);
}

// Whether the unit has defined the function: by a body, or by a system
// header's definition whose body was skipped and left its words.  A
// second definition is then a redefinition, 6.9.1p3.
bool
is_defined(const parser_t &p, std::size_t index)
{
    const function_decl_t &f = p.unit.functions[index];

    return f.body || f.sym->marking == skipped_body_words(f.sym->name);
}

// The parameter type list and the body are one block, 6.9.2p10,
// 6.8.3p2, so a local cannot redeclare a parameter there: the scope of
// the list that makes the name a function, with its parameters and its
// tags, is opened again and the body's items are parsed into it
// directly.  The result is an object the definition makes, so it may
// not be of a not-yet type, nor yet of a record, which returns by
// value; the parameters are defined_parameters()'s.  A function a
// marking attribute marked, on a declaration, among the definition's
// specifiers or on a parameter, is one rcc would have to define as the
// attribute says, which is not yet.  function and statics start over,
// for the labels of the static objects the body declares, and labels,
// gotos and case_labels, for the body's own labels, which no other
// function sees; and variadic, named and va_area, for the body's
// va_start.  The body's identities are dropped once it is whole.
void
function_definition(parser_t &p,
                    const specifiers_t &spec,
                    const declarator_t &d)
{
    const std::size_t index = declare_function(
        p, spec, d, { nullptr, { }, spec.marking, spec.markings });
    if (is_defined(p, index))
        fail(*d.name, "redefinition of " + quoted(d.name->text));

    const symbol_t &sym = *p.unit.functions[index].sym;
    if (!sym.marking.empty())
        not_yet(*d.name, sym.marking);

    p.result = *as< function_t >(d.type).result;
    require_honoured(*d.name, p.result);

    if (const record_t *r = record_in(p.result))
        not_yet(*d.name, record_words(*r) + " returned by value");

    p.registers = 0;
    p.function = d.name->text;
    p.variadic = as< function_t >(d.type).variadic;
    p.named = d.params->size();
    p.va_area = nullptr;
    p.statics.clear();
    p.labels.clear();
    p.gotos.clear();
    p.case_labels = 0;
    p.scopes.push_back(d.scope);

    std::vector< const symbol_t * > params = defined_parameters(p, d);

    block_t body;
    p.temporaries.emplace();
    expect(p, tok_t::lbrace, "expected '{' before function body");
    block_item_list(p, body);
    expect(p, tok_t::rbrace, "expected '}' at end of function body");
    declare_temporaries(p, body);
    drop_identities(body);

    p.scopes.pop_back();

    require_labels(p);

    function_decl_t &f = p.unit.functions[index];
    f.params = std::move(params);
    f.body = std::move(body);
    f.registers = p.registers;
    f.va_area = p.va_area;
    p.variadic = false;
}

// The definition of a static inline function a system header writes,
// as glibc's bits/byteswap.h defines __bswap_16.  The function has
// internal linkage, 6.2.2p3, so a call of it calls this definition,
// which rcc would have to compile, and that is not yet: a header's body
// need not be one rcc can compile, and a program that never calls the
// function does not pay for it.  The definition is accepted, its body
// skipped to the brace that closes it, and the function stays
// declared, so c1 writes nothing for it; an expression that names it is
// refused by name, and so is a use before the definition, at the end of
// the unit.  A function that carries the words was defined so before.
void
header_function_definition(parser_t &p,
                           const specifiers_t &spec,
                           const declarator_t &d)
{
    const std::size_t index = declare_function(
        p, spec, d, { nullptr, { }, spec.marking, spec.markings });

    if (is_defined(p, index))
        fail(*d.name, "redefinition of " + quoted(d.name->text));

    symbol_t &sym = *p.scopes.back().names.at(d.name->text);
    if (sym.marking.empty())
        sym.marking = skipped_body_words(d.name->text);

    expect(p, tok_t::lbrace, "expected '{' before function body");
    skip_to_closing_brace(p);
    expect(p, tok_t::rbrace, "expected '}' at end of function body");
}

// How a struct, union or enum specifier uses its tag, 6.7.3.4: a list
// defines the tag's content (p10); struct S; declares the tag alone
// (p11); any other mention refers to the type the tag names (p12, p13).
enum class tag_use_t { defines, declares, refers };

// A new record of the unit's, incomplete until a list completes it,
// 6.7.3.4p5, of the kind the keyword says and the tag, empty for none.
record_t &
new_record(parser_t &p, const token_t &keyword, std::string_view tag)
{
    p.unit.records.push_back(
        record_t { keyword.kind == tok_t::kw_union, std::string(tag) });

    return p.unit.records.back();
}

// A tag met first, declared in the current scope as a new type of an
// identity of its own, 6.7.3.4p9: a struct or a union a record,
// new_record()'s; an enum a not-yet type until its list makes it the
// integer type compatible with it.
tag_t &
new_tag(parser_t &p, const token_t &keyword, const token_t &tag)
{
    tag_t fresh { make_void(), keyword.kind, false };
    if (keyword.kind != tok_t::kw_enum)
        fresh.record = &new_record(p, keyword, tag.text);

    fresh.type = fresh.record ? make_record(*fresh.record) :
                                make_not_yet(p.identities++, "enum");

    return p.scopes.back().tags.emplace(tag.text, fresh).first->second;
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
// does not compare, as it compares no two records' members, nor any
// enum's list against the first.  So it is not yet.  A tag
// first declared in a parameter list is declared in the list's scope,
// 6.2.1p4, and ends with it, or with the body of a definition.
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

    if (!found)
        found = &new_tag(p, keyword, tag);

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

// _Atomic as a qualifier makes an atomic type, whose access, ++ on it
// among them, is one atomic instruction, which c1 does not write.
// After typedef the qualifier is taken, and the typedef names a not-yet
// type of its own; anywhere else it is not yet.
void
refuse_atomic_qualifier(const token_t &q, const token_t *storage_class)
{
    if (!storage_class || storage_class->kind != tok_t::kw_typedef)
        not_yet(q, "type qualifier " + quoted(q.text));
}

// The type specifiers in any valid order and combination, 6.7.3.1p2.
// The tree keeps no spelling: char and signed char are one type, long
// and long long another.  Each keyword is checked against the ones
// before it, so an error names the keyword that breaks the set.  const
// and volatile may stand anywhere among them, once or more, 6.7.4.1p6,
// and are dropped: the type is the machine's view.  volatile asks that
// each access the source writes be made, p8, and c1 writes every access
// of every object, an unused one included, at the object's width, so
// the qualifier asks for nothing c1 does not do.  _Atomic as a qualifier
// is taken after typedef alone.  A storage-class specifier, register,
// typedef, extern or static, may stand anywhere too, one of them,
// 6.7.2p2, and is reported to the caller, since what it may declare
// depends on where the declaration is.  A typedef name denotes its
// type, 6.7.9p3, and is the type specifier where no type specifier has
// been seen, one of 6.7.3.1p2's sets alone; after one, an
// identifier is the declarator's name, so unsigned T declares a T.  A
// typedef name, a struct, union or enum specifier, an atomic type
// specifier, float, _Bool and GNU C's __float128 are each the whole
// type specifier; double joins one long, __int128 signed or unsigned,
// and _Complex a floating type.  restrict may stand among them too, on
// a type require_restrictable() takes, and is dropped.  So is a
// function specifier, inline or _Noreturn, once or more, 6.7.5p5,
// reported to the caller, which knows whether the declaration is of a
// function.  An attribute may stand anywhere among them; a marking one
// is reported to the caller.  The keywords of C outside the subset are
// refused by name where they stand.
specifiers_t
declaration_specifiers(parser_t &p)
{
    int n_void = 0, n_char = 0, n_short = 0, n_int = 0, n_long = 0;
    int n_signed = 0, n_unsigned = 0, n_float = 0, n_double = 0;
    int n_int128 = 0, n_float128 = 0;
    bool any = false;

    std::optional< type_t > whole;
    bool tagged = false;
    const token_t *storage_class = nullptr;
    const token_t *atomic_qualifier = nullptr;
    const token_t *restrict_qualifier = nullptr;
    const token_t *function_specifier = nullptr;
    const token_t *inline_specifier = nullptr;
    const token_t *complex_specifier = nullptr;
    const token_t *marking = nullptr;
    markings_t markings;

    // the type the specifiers name, once they end
    const auto specified = [&]() -> type_t {
        const bool complex = complex_specifier != nullptr;

        if (whole)
            return *whole;

        if (n_void)
            return make_void();

        if (n_float)
            return complex ? complex_float_type() : float_type();

        if (n_double && n_long)
            return complex ? complex_long_double_type() : long_double_type();

        if (n_double)
            return complex ? complex_double_type() : double_type();

        if (n_float128)
            return float128_type();

        if (n_int128)
            return int128_type(n_unsigned == 0);

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

        if (accept(p, tok_t::kw_const) || accept(p, tok_t::kw_volatile))
            continue;

        if (t.kind == tok_t::kw_restrict) {
            restrict_qualifier = &advance(p);
            continue;
        }

        if (t.kind == tok_t::kw_inline || t.kind == tok_t::kw_noreturn) {
            if (!function_specifier)
                function_specifier = &t;

            if (t.kind == tok_t::kw_inline && !inline_specifier)
                inline_specifier = &t;

            advance(p);
            continue;
        }

        if (t.kind == tok_t::kw_register || t.kind == tok_t::kw_typedef ||
            t.kind == tok_t::kw_extern || t.kind == tok_t::kw_static) {
            // 6.7.2p2: at most one storage-class specifier
            if (storage_class)
                fail(t, "cannot combine " + quoted(t.text) +
                            " with the storage class before it");

            storage_class = &t;
            advance(p);
            continue;
        }

        if (t.kind == tok_t::kw_atomic && peek(p, 1).kind != tok_t::lparen) {
            refuse_atomic_qualifier(t, storage_class);

            if (!atomic_qualifier)
                atomic_qualifier = &t;

            advance(p);
            continue;
        }

        if (t.kind == tok_t::kw_attribute) {
            const token_t *name = attribute_specifier_list(p, &markings);
            if (!marking)
                marking = name;

            continue;
        }

        // _Complex joins the floating type before or after it, and is
        // left out of any, since float takes no other type specifier
        if (t.kind == tok_t::kw_complex) {
            if (complex_specifier || n_void || whole || n_float128)
                fail(t, "cannot combine " + quoted(t.text) +
                            " with the type specifiers before it");

            complex_specifier = &advance(p);
            continue;
        }

        if (!any && names_type(p, t)) {
            whole = named_type(p, t);

            advance(p);
            any = true;
            continue;
        }

        bool bad = n_void > 0 || whole || n_float > 0 || n_float128 > 0;
        switch (t.kind) {
        case tok_t::kw_void:
            bad = bad || n_char || n_short || n_int || n_long || n_signed ||
                  n_unsigned || n_double || n_int128 || complex_specifier;
            ++n_void;
            break;
        case tok_t::kw_char:
            bad = bad || n_char || n_short || n_int || n_long || n_double ||
                  n_int128;
            ++n_char;
            break;
        case tok_t::kw_short:
            bad = bad || n_char || n_short || n_long || n_double || n_int128;
            ++n_short;
            break;
        case tok_t::kw_int:
            bad = bad || n_char || n_int || n_double || n_int128;
            ++n_int;
            break;
        case tok_t::kw_long:
            bad = bad || n_char || n_short || n_long == 2 ||
                  (n_long && n_double) || n_int128;
            ++n_long;
            break;
        case tok_t::kw_int128:
            bad = bad || n_char || n_short || n_int || n_long || n_double ||
                  n_int128;
            ++n_int128;
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
                  n_signed || n_unsigned || n_double || n_int128;
            ++n_double;
            break;
        case tok_t::kw_float128:
            bad = any;
            ++n_float128;
            break;
        case tok_t::kw_struct:
        case tok_t::kw_union:
        case tok_t::kw_enum:
            if (any || complex_specifier) {
                bad = true;
                break;
            }

            whole = t.kind == tok_t::kw_enum ? enum_specifier(p) :
                                               struct_or_union_specifier(p);
            tagged = true;
            any = true;
            continue;
        case tok_t::kw_atomic:
            if (any || complex_specifier) {
                bad = true;
                break;
            }

            whole = atomic_type_specifier(p);
            any = true;
            continue;
        case tok_t::kw_bool:
            bad = any || complex_specifier;
            whole = bool_type();
            break;
        case tok_t::kw_imaginary:
            not_yet(t, "type specifier " + quoted(t.text));
        case tok_t::kw_auto:
        case tok_t::kw_thread_local:
            not_yet(t, "storage class " + quoted(t.text));
        case tok_t::kw_alignas:
            not_yet(t, "alignment specifier " + quoted(t.text));
        default: {
            if (complex_specifier && !n_float && !n_double)
                not_yet(*complex_specifier,
                        "type specifier " + quoted(complex_specifier->text));

            if (!any)
                fail(t, "expected a type specifier");

            type_t type = specified();
            if (restrict_qualifier)
                require_restrictable(*restrict_qualifier, type);

            if (atomic_qualifier)
                type = make_not_yet(p.identities++,
                                    "type qualifier " +
                                        quoted(atomic_qualifier->text));

            return { type,
                     storage_class,
                     tagged,
                     function_specifier,
                     inline_specifier,
                     marking,
                     markings };
        }
        }

        if (bad)
            fail(t, "cannot combine " + quoted(t.text) +
                        " with the type specifiers before it");

        advance(p);
        any = true;
    }
}

// Words that say a construct of a record's list rcc does not lay out
// yet, kept in the record: the first one met is the one its uses are
// refused by.
void
mark_not_yet(record_t &r, std::string words)
{
    if (r.not_yet.empty())
        r.not_yet = std::move(words);
}

// The record a member declaration of a list is read into; the token
// of the member of an array of unknown size it has met, a flexible
// array member, which only the last member may be, and the token of
// its size when it was written as zero; and the count of the named
// members met, bit-fields among them.
struct member_list_t
{
    record_t &record;
    const token_t *flexible;
    const token_t *zero;
    int named;
};

// The refusal of a flexible array member where none may stand; one
// written with a size of zero is refused as any array of that size,
// 6.7.7.3p1, at its size.
[[noreturn]] void
refuse_flexible(const member_list_t &list, std::string what)
{
    if (list.zero)
        fail(*list.zero, "array size must be greater than zero");

    fail(*list.flexible, std::move(what));
}

// The width of a bit-field after its ':', an integer constant
// expression, 6.7.3.2p4, of a bit-field whose type is an integer type,
// p5, or one rcc does not use yet, _Bool among them.  rcc lays out no
// bit-field yet, so its record is a not-yet record; attributes after
// the width are said of the member, and change nothing that is laid
// out.
void
bit_field(parser_t &p, member_list_t &list, const type_t &t)
{
    const token_t &colon = advance(p);
    if (!is< int_t >(t) && !is< not_yet_t >(t))
        fail(colon, "bit-field of a type that is not an integer type");

    const token_t &start = peek(p);
    const expr_t width = rvalue(start, conditional_expression(p));
    if (!is_integer_constant(width))
        fail(start, "bit-field width is not an integer constant expression");

    attribute_specifier_list(p);
    mark_not_yet(list.record, "bit-field in " + record_words(list.record));
}

// Whether a member's type has a size though rcc does not use its
// object yet: a real floating type, or an array of one, takes its place
// in the layout, and an access of it is refused where it stands.
bool
is_sized(const type_t &t)
{
    if (is< array_t >(t))
        return is_sized(*as< array_t >(t).element);

    return is< not_yet_t >(t) && as< not_yet_t >(t).size > 0;
}

// A member's type: a complete object type, 6.7.3.2p11, so neither a
// function, p3, nor void, nor an incomplete record, the record being
// defined among them, which holds no instance of itself, p3.  An array
// of unknown size is a flexible array member, p20, which rcc does not
// lay out yet, and stands last, p3, as member_list() checks; in a union
// it is an error, p3.  zero is the size of such an array written as
// zero, or null.  A type rcc does not use yet, a record that holds a
// construct rcc does not lay out among them, leaves the record without
// its layout, refused by the member type's words, but for one
// is_sized() says is laid out.
void
require_member_type(const token_t &name,
                    const type_t &t,
                    const token_t *zero,
                    member_list_t &list)
{
    record_t &r = list.record;

    if (is< function_t >(t))
        fail(name, "member " + quoted(name.text) + " declared as a function");

    if (is< void_t >(t))
        fail(name, "member " + quoted(name.text) + " has type void");

    if (is< record_type_t >(t) && !as< record_type_t >(t).record->complete)
        fail(name, "member " + quoted(name.text) + " has incomplete type " +
                       record_words(*as< record_type_t >(t).record));

    if (is< array_t >(t) && as< array_t >(t).count < 0) {
        list.flexible = &name;
        list.zero = zero;

        if (r.is_union)
            refuse_flexible(list, "flexible array member " + quoted(name.text) +
                                      " in " + record_words(r));

        mark_not_yet(r, "flexible array member in " + record_words(r));
    }

    if (const std::string *words = not_yet_in(t); words && !is_sized(t))
        mark_not_yet(r, *words);
}

// A named member, its type checked and its name one the record has not
// declared yet, 6.7.3.2p1 and 6.2.3p1: each record is a name space of
// its own.  A member after a flexible array member is an error, p3.
// zero is the member's own array's size written as zero, or null.
void
add_member(member_list_t &list,
           const token_t &name,
           const type_t &t,
           const token_t *zero)
{
    record_t &r = list.record;

    if (list.flexible)
        refuse_flexible(list, "flexible array member " +
                                  quoted(list.flexible->text) +
                                  " not at the end of " + record_words(r));

    require_member_type(name, t, zero, list);

    for (const member_t &m : r.members)
        if (m.name == name.text)
            fail(name, "duplicate member " + quoted(name.text));

    if (!list.flexible)
        r.members.push_back(member_t { std::string(name.text), t, 0 });
}

// One member declarator, 6.7.3.2p1: a declarator, which names the
// member, then a bit-field's width or none; or a width alone, an
// unnamed bit-field, p14.  An attribute after the declarator is said of
// the member, and one that marks, aligned and packed among them,
// changes its layout, which rcc does not honour yet.
void
member_declarator(parser_t &p, const specifiers_t &spec, member_list_t &list)
{
    if (peek(p).kind == tok_t::colon) {
        bit_field(p, list, spec.type);
        return;
    }

    const declarator_t d = declarator(p, spec.type, naming_t::member, false);
    ++list.named;

    if (const token_t *marking = attribute_specifier_list(p))
        mark_not_yet(list.record, attribute_words(*marking) + " in " +
                                      record_words(list.record));

    if (peek(p).kind == tok_t::colon) {
        bit_field(p, list, d.type);
        return;
    }

    add_member(list, *d.name, d.type, d.zero);
}

// A member declaration with no declarator, 6.7.3.2p2: a struct or a
// union specifier without a tag declares an anonymous member, p15,
// which rcc does not lay out yet.  Any other struct, union or enum
// specifier declares its tag or its constants alone, as at file scope;
// one of no specifier of the kind declares nothing, an error.
void
unnamed_member(const token_t &semicolon,
               const specifiers_t &spec,
               member_list_t &list)
{
    if (!spec.tagged)
        fail(semicolon, "declaration does not declare anything");

    const record_t *inner = is< record_type_t >(spec.type) ?
                                as< record_type_t >(spec.type).record :
                                nullptr;
    if (inner && inner->tag.empty())
        mark_not_yet(list.record,
                     "anonymous " +
                         std::string(inner->is_union ? "union" : "struct") +
                         " in " + record_words(list.record));
}

// A member declaration: its specifiers, which are a specifier-qualifier
// list, 6.7.3.2p1, with no storage class and no function specifier, and
// its member declarators.  A marking attribute among the specifiers is
// said of each member, as after a declarator.
void
member_declaration(parser_t &p, member_list_t &list)
{
    const specifiers_t spec = declaration_specifiers(p);

    if (const token_t *t = spec.storage_class)
        fail(*t,
             "storage class " + quoted(t->text) + " in a member declaration");

    refuse_function_specifier(spec);

    if (spec.marking)
        mark_not_yet(list.record, attribute_words(*spec.marking) + " in " +
                                      record_words(list.record));

    if (peek(p).kind == tok_t::semicolon) {
        unnamed_member(advance(p), spec, list);
        return;
    }

    do
        member_declarator(p, spec, list);
    while (accept(p, tok_t::comma));

    expect(p, tok_t::semicolon, "expected ';' at end of member declaration");
}

// The member declarations of a list, up to its '}', 6.7.3.2p1: each a
// static assertion, checked where it stands, 6.7.12, or a member
// declaration, __extension__ before it dropped.  The list opens no
// scope: a tag or an enumeration constant declared in it is declared
// where the record is, 6.2.1p4, and the members' names are the
// record's own, 6.2.3p1.  A flexible array member is not the only
// named member, 6.7.3.2p3, and a list of no member at all makes an empty
// record, whose behavior p10 leaves undefined and GNU C takes, which
// rcc does not lay out yet.  A flexible array member written with a
// size of zero, as GNU C writes one, is taken with a warning in the
// program's own text and with none in a system header's, which is read
// as it is.
void
member_list(parser_t &p, record_t &r)
{
    member_list_t list { r, nullptr, nullptr, 0 };

    while (!next_is(p, { tok_t::rbrace, tok_t::eof })) {
        skip_extension_keywords(p);

        if (peek(p).kind == tok_t::kw_static_assert)
            static_assert_declaration(p);
        else
            member_declaration(p, list);
    }

    if (list.flexible && list.named < 2)
        refuse_flexible(list, "flexible array member " +
                                  quoted(list.flexible->text) +
                                  " in a struct with no other named member");

    if (list.zero && !list.zero->where.system_header)
        p.warnings.push_back(
            { list.zero->where, "array " + quoted(list.flexible->text) +
                                    " of size zero read as a flexible array "
                                    "member" });

    if (r.members.empty())
        mark_not_yet(r, "empty " + record_words(r));
}

// The members of the list after the '{' at the position.  The rule for
// a system header holds here as for a typedef's array size: in a list
// a system header writes, a refusal of the not-yet kind makes the
// record a not-yet record of the refusal's words, and the parse skips
// to the list's closing brace; the scopes are cut back to those open
// at the brace, as a parameter list inside the list leaves one open.
// An error stays the error it is, and a list of the program's own
// source is refused where it stands.
void
members(parser_t &p, record_t &r)
{
    const token_t &brace = advance(p);
    if (!brace.where.system_header) {
        member_list(p, r);
        return;
    }

    const std::size_t start = p.pos;
    const std::size_t depth = p.scopes.size();

    try {
        member_list(p, r);
    }
    catch (const not_yet_error_t &e) {
        mark_not_yet(r, e.construct);
        p.pos = start;
        p.scopes.resize(depth);
        skip_to_closing_brace(p);
    }
}

// The layout of a record its list completed, by the rule clang has for
// both targets: each member of a struct at the offset so far rounded up
// to the member's alignment, in the order declared, 6.7.3.2p16, p17;
// each member of a union at 0, p18; the record's alignment its
// members' largest, and its size the end of its members rounded up to
// that alignment, p19, so that each element of an array of it is
// aligned.  close is the list's '}', where a record too large for a
// size is refused.
void
lay_out(const token_t &close, record_t &r)
{
    constexpr long largest = std::numeric_limits< long >::max();

    long end = 0;
    int alignment = 1;

    for (member_t &m : r.members) {
        const long size = size_of(m.type);
        const int align = align_of(m.type);

        if (end > largest - align)
            fail(close, record_words(r) + " is too large");

        m.offset = r.is_union ? 0 : (end + align - 1) / align * align;

        if (size > largest - m.offset)
            fail(close, record_words(r) + " is too large");

        end = std::max(end, m.offset + size);
        alignment = std::max(alignment, align);
    }

    if (end > largest - alignment)
        fail(close, record_words(r) + " is too large");

    r.size = (end + alignment - 1) / alignment * alignment;
    r.alignment = alignment;
}

// The members of the record behind the target's va_list, 7.16p4, as the
// convention has them for the C library, which reads the lists a
// program hands it: the x86-64 psABI's __va_list_tag, the offsets into
// the register save area of the next general and the next vector
// register, and the addresses of the next argument on the stack and of
// the save area; the AAPCS64's __va_list, the address of the next
// argument on the stack, the tops of the general and the vector
// registers' parts of the save area, and the negative offsets below
// them of the next register of each.  The names are clang's.
std::vector< member_t >
va_members(const parser_t &p)
{
    const type_t pointer = make_pointer(make_void());

    if (p.target.va_list == va_list_t::psabi)
        return { { "gp_offset", make_int(32, false), 0 },
                 { "fp_offset", make_int(32, false), 0 },
                 { "overflow_arg_area", pointer, 0 },
                 { "reg_save_area", pointer, 0 } };

    return { { "__stack", pointer, 0 },
             { "__gr_top", pointer, 0 },
             { "__vr_top", pointer, 0 },
             { "__gr_offs", int_type(), 0 },
             { "__vr_offs", int_type(), 0 } };
}

// The unit's list record, made once, under clang's tag and in no scope,
// so that every va_list of the unit is one type.  Laid out by the rule
// of every record, it has the convention's offsets: 0, 4, 8 and 16 in
// 24 bytes, and 0, 8, 16, 24 and 28 in 32.
const record_t *
va_record(parser_t &p)
{
    const bool psabi = p.target.va_list == va_list_t::psabi;

    record_t &r = p.unit.records.emplace_back(
        record_t { false, psabi ? "__va_list_tag" : "__va_list" });
    r.members = va_members(p);
    r.complete = true;
    lay_out(p.tokens.front(), r);

    return &r;
}

// struct or union, then a tag, a member list in braces, or both,
// 6.7.3.2p1.  A tag followed by ; is the declaration of the tag alone,
// 6.7.3.4p11; without a list a tag names the type tagged_type() finds.
// A list completes the record, 6.7.3.4p5: members() reads it, and the
// record is laid out unless it holds a construct rcc does not lay out
// yet.  An attribute after the keyword or after the list is said of the
// record: one of the kind dropped is dropped, and one that marks,
// packed and aligned among them, changes the layout, which rcc does
// not honour yet; so does a #pragma pack that sets a packing where the
// list opens (shape.md chapter 2).  An attribute where the specifier
// names a type without a list is dropped.
type_t
struct_or_union_specifier(parser_t &p)
{
    const token_t &keyword = advance(p);
    const token_t *before = attribute_specifier_list(p);

    const token_t *tag = optional_tag(p, keyword);

    if (peek(p).kind != tok_t::lbrace) {
        const tag_use_t use = peek(p).kind == tok_t::semicolon ?
                                  tag_use_t::declares :
                                  tag_use_t::refers;

        return tagged_type(p, keyword, *tag, use);
    }

    if (tag)
        tagged_type(p, keyword, *tag, tag_use_t::defines);

    record_t &r = tag ? *tag_in(p.scopes.back(), tag->text)->record :
                        new_record(p, keyword, { });

    const token_t &open = peek(p);
    members(p, r);
    const token_t &close =
        expect(p, tok_t::rbrace, "expected '}' at end of member list");

    const token_t *after = attribute_specifier_list(p);
    if (const token_t *marking = before ? before : after)
        mark_not_yet(r, attribute_words(*marking) + " on " + record_words(r));

    if (open.packed)
        mark_not_yet(r, "pragma 'pack' on " + record_words(r));

    if (r.not_yet.empty())
        lay_out(close, r);

    r.complete = true;

    return make_record(r);
}

// An enumeration constant as its list declares it: its symbol, and its
// value exactly, which the type the enumeration completes as must hold.
struct enumerator_t
{
    symbol_t *sym;
    exact_t value;
};

// The type and the value of an enumeration constant while its list is
// read, 6.7.3.3p10, p12, as a literal.  After = they are those of the
// integer constant expression, p4, but an int where int holds the
// value.  Without =, the first constant is the int 0, and each other the
// previous one's value plus one, in the previous one's type, or in the
// 64-bit type of its signedness where that type does not hold the sum.
// No type holds a sum past the 64-bit types, and the enumeration has
// then no type for its values, a constraint violation, p12 and its
// footnote.
expr_t
enumerator_value(parser_t &p, const token_t &name, const expr_t *previous)
{
    if (accept(p, tok_t::assign)) {
        const token_t &start = peek(p);

        expr_t e = rvalue(start, conditional_expression(p));
        if (!is_integer_constant(e))
            fail(start,
                 "enumerator value is not an integer constant expression");

        if (holds(int_type(), exact_value(e)))
            return constant_of(int_type(), exact_value(e));

        return e;
    }

    if (!previous)
        return constant_of(int_type(), 0);

    const exact_t next = exact_value(*previous) + 1;
    if (holds(previous->type, next))
        return constant_of(previous->type, next);

    const type_t wider = make_int(64, as< int_t >(previous->type).is_signed);
    if (!holds(wider, next))
        fail(name, "value of " + quoted(name.text) +
                       " is too large for any integer type");

    return constant_of(wider, next);
}

// One or more enumerators, the list perhaps ending in a comma,
// 6.7.3.3p1.  Each declares its enumeration constant as an ordinary
// identifier, 6.2.3p1, in the current scope, once there, of the type and
// the value enumerator_value() gives it.  Its scope begins after its
// enumerator, 6.2.1p7, so its own value cannot name it and the next
// one's can.
std::vector< enumerator_t >
enumerator_list(parser_t &p)
{
    std::vector< enumerator_t > list;
    std::optional< expr_t > previous;

    do {
        const token_t &name =
            expect(p, tok_t::identifier, "expected identifier");
        if (p.scopes.back().names.contains(name.text))
            fail(name, "redefinition of " + quoted(name.text));

        expr_t value =
            enumerator_value(p, name, previous ? &*previous : nullptr);

        symbol_t &sym = added(p, name.text, value.type, storage_t::enumerator);
        sym.value = as< literal_t >(value).value;
        list.push_back({ &sym, exact_value(value) });

        previous = std::move(value);
    } while (accept(p, tok_t::comma) && peek(p).kind != tok_t::rbrace);

    return list;
}

// The integer type compatible with an enumerated type, 6.7.3.3p13, the
// choice J.3.10 leaves to the implementation, as clang makes it on both
// targets: unsigned int when no value is negative and it holds every
// one, else unsigned long; int when a value is negative and int holds
// every one, else long.  No type holds a negative value and one past
// LONG_MAX together, a constraint violation, p4.  at is the tag, or the
// keyword of an enum without one.
type_t
compatible_type(const token_t &at, const std::vector< enumerator_t > &list)
{
    const auto [least, most] = std::ranges::minmax(
        list, { }, [](const enumerator_t &e) { return e.value; });
    const bool negative = least.value < 0;

    for (const int width : { 32, 64 }) {
        const type_t t = make_int(width, negative);
        if (holds(t, least.value) && holds(t, most.value))
            return t;
    }

    const std::string what =
        at.kind == tok_t::identifier ? "enum " + quoted(at.text) : "the enum";
    fail(at, "no integer type holds every value of " + what);
}

// The constants of an enumeration whose list is complete take its
// member type, 6.7.3.3p15: int when int holds every value, else the
// enumerated type, t.
void
complete_constants(const std::vector< enumerator_t > &list, const type_t &t)
{
    const bool ints = std::ranges::all_of(
        list, [](const enumerator_t &e) { return holds(int_type(), e.value); });
    const type_t member = ints ? int_type() : t;

    for (const enumerator_t &e : list) {
        e.sym->type = member;
        if (is< int_t >(member))
            e.sym->value = wrapped(e.value, as< int_t >(member));
    }
}

// C23's enum type specifier, a ':' and a type's specifiers after enum or
// its tag, 6.7.3.3p1, which fixes the enumerated type, p5, is not yet:
// no header and no program at hand writes it, and its constraints, p3
// and p6 to p8, each need a diagnostic of their own.
void
refuse_fixed_underlying_type(const parser_t &p)
{
    if (peek(p).kind == tok_t::colon)
        not_yet(peek(p), "enum with a fixed underlying type");
}

// The tag of an enum whose list follows, declared in the current scope
// from the list's '{' on, 6.2.1p7, as the incomplete type it is until
// the '}', 6.7.3.4p5: a not-yet type, whose pointer is taken and whose
// every other use the list makes is refused by its words.
void
open_enum_tag(parser_t &p, const token_t &keyword, const token_t &tag)
{
    tagged_type(p, keyword, tag, tag_use_t::defines);

    tag_in(p.scopes.back(), tag.text)->type = make_not_yet(
        p.identities++, "enum " + quoted(tag.text) + " in its own list");
}

// enum, then a tag, an enumerator list in braces, or both, 6.7.3.3p1.
// A list defines an enumerated type, and the tree keeps the integer type
// compatible with it, compatible_type()'s, as it keeps the type a
// typedef names: an object, a parameter, a result, a cast, sizeof, a
// switch and the arithmetic of one are its integer type's.  Its
// constants take the types and the values of p10, p12 and p15.  A tag
// without a list names the type its list defined, and is an error
// before the list, 6.7.3.4p2: GNU C's forward declaration, enum e;, is
// not taken, since a pointer made to the type before the list would
// not point to the integer type the list completes it as.  An
// attribute after the keyword or after the list is dropped when it is
// of the kind dropped; a marking one, packed among them, which changes
// the type's size, makes the type a not-yet type of the attribute's
// words, and the constants keep their values, an int constant its use.
type_t
enum_specifier(parser_t &p)
{
    const token_t &keyword = advance(p);
    const token_t *marking = attribute_specifier_list(p);

    refuse_fixed_underlying_type(p);
    const token_t *tag = optional_tag(p, keyword);
    refuse_fixed_underlying_type(p);

    if (peek(p).kind != tok_t::lbrace)
        return tagged_type(p, keyword, *tag, tag_use_t::refers);

    if (tag)
        open_enum_tag(p, keyword, *tag);

    advance(p);
    const std::vector< enumerator_t > list = enumerator_list(p);
    expect(p, tok_t::rbrace, "expected '}' at end of enumerator list");

    const token_t *after = attribute_specifier_list(p);
    if (!marking)
        marking = after;

    type_t t = compatible_type(tag ? *tag : keyword, list);
    if (marking)
        t = make_not_yet(p.identities++, attribute_words(*marking));

    complete_constants(list, t);

    if (tag)
        tag_in(p.scopes.back(), tag->text)->type = t;

    return t;
}

// _Atomic ( type-name ), the atomic type specifier, 6.7.3.5p1, which
// _Atomic is when a parenthesis follows it, p4, as clang's stdatomic.h
// declares atomic_int.  The type is atomic, as the qualifier makes it,
// so it is a not-yet type of its own wherever it is written, and the
// use of its object is refused.
type_t
atomic_type_specifier(parser_t &p)
{
    const token_t &keyword = advance(p);
    advance(p);

    type_name(p);
    expect(p, tok_t::rparen, "expected ')' after type name");

    return make_not_yet(p.identities++,
                        "type specifier " + quoted(keyword.text));
}

// Whether the parenthesis ahead opens a declarator in parentheses,
// 6.7.7.1p1, and not a parameter list.  Before a name that must be
// written, a parenthesis opens only a declarator.  Where the name may
// be left out, a '*', a '(' or a '[' opens a declarator, and so does
// an identifier where it may be the name and does not name a type,
// 6.7.7.4p10; anything else opens a parameter list, an empty one
// included, 6.7.8 footnote 162.
bool
starts_parenthesized_declarator(const parser_t &p, naming_t naming)
{
    if (peek(p).kind != tok_t::lparen)
        return false;

    if (naming == naming_t::required || naming == naming_t::member)
        return true;

    const token_t &t = peek(p, 1);

    switch (t.kind) {
    case tok_t::star:
    case tok_t::lparen:
    case tok_t::lbracket: return true;
    case tok_t::identifier:
        return naming == naming_t::optional && !names_type(p, t);
    default: return false;
    }
}

// The tokens of a declarator in parentheses, skipped up to the
// parenthesis that closes it, which is left for the caller, with the
// parentheses nested in them counted.  Outside those, a semicolon or a
// brace ends the declaration and no declarator holds one, so the skip
// stops there too, and an unclosed parenthesis is reported where the
// declaration ends.
void
skip_parenthesized_declarator(parser_t &p)
{
    for (int depth = 0; peek(p).kind != tok_t::eof; advance(p)) {
        const tok_t kind = peek(p).kind;
        if (depth == 0 && (kind == tok_t::rparen || kind == tok_t::semicolon ||
                           kind == tok_t::lbrace || kind == tok_t::rbrace))
            return;

        if (kind == tok_t::lparen)
            ++depth;
        else if (kind == tok_t::rparen)
            --depth;
    }
}

// The pointers, then the name or a declarator in parentheses, then
// array suffixes or a parameter list.  The name must be written in a
// declaration, may be left out in a parameter, and is not written in a
// type name.  The pointers bind to the base type, so int *f(void) is a
// function returning a pointer, and parentheses bind them first, so
// int (*f)(void) is a pointer to a function.  typedef_ says that the
// declarator is a typedef's, where an array size that needs what rcc
// does not have yet makes a not-yet type; the parameters of a parameter
// list in it are not the typedef's, and their sizes are refused.  The
// array suffix right after a parameter's name, or the place of it, is
// the parameter's outermost array type derivation, the one 6.7.7.4p6
// adjusts to a pointer; a parameter's is the declarator whose name is
// optional.  The array suffix right after a member's name is the
// member's own array, which may have a size of zero.
declarator_t
declarator(parser_t &p, const type_t &base, naming_t naming, bool typedef_)
{
    const type_t t = pointer(p, base);

    if (starts_parenthesized_declarator(p, naming))
        return parenthesized_declarator(p, t, naming, typedef_);

    const token_t *name = nullptr;
    if (naming == naming_t::required || naming == naming_t::member ||
        (naming == naming_t::optional && peek(p).kind == tok_t::identifier))
        name = &expect(p, tok_t::identifier, "expected identifier");

    return declarator_suffix(p, name, t, typedef_, naming == naming_t::optional,
                             naming == naming_t::member);
}

// Whether the tokens from inside to close, those of a declarator in
// parentheses, are a name alone, in parentheses or not, as in (a) or
// ((a)), so that the parentheses are redundant and derive nothing.  A
// typedef name there opens a parameter list, 6.7.7.4p10, so it is no
// such name.
bool
names_alone(const parser_t &p, std::size_t inside, std::size_t close)
{
    std::size_t open = inside;
    while (open < close && p.tokens[open].kind == tok_t::lparen)
        ++open;

    if (open == close || p.tokens[open].kind != tok_t::identifier ||
        names_type(p, p.tokens[open]))
        return false;

    for (std::size_t i = open + 1; i < close; ++i)
        if (p.tokens[i].kind != tok_t::rparen)
            return false;

    return close - (open + 1) == open - inside;
}

// ( D ) and the suffixes after it.  The declarator in parentheses
// derives from what the suffixes derive from the type so far, 6.7.7.1p6:
// in void (*f)(int) the '*' derives a pointer from void (int), the
// function.  So the suffixes are read first, past the parentheses, and
// then the declarator inside them, and the parse goes on after the
// suffixes.  A parameter list after the parentheses is the name's when
// the declarator inside derives nothing, as in int (f)(void), whose
// type is then the suffix's; otherwise it is the prototype of what the
// name's type points through, and the name declares no function.  Each
// parameter list is read once, the suffix's before the rewind and the
// inner one's after it, and the list that is the name's brings its
// scope.  An array suffix after the parentheses is read as an array's
// that no parameter is adjusted from, as it is in int (*a)[3], unless
// the parentheses are redundant: in int (a)[3] it is the parameter's
// own, and a member's own likewise.
declarator_t
parenthesized_declarator(parser_t &p,
                         const type_t &t,
                         naming_t naming,
                         bool typedef_)
{
    advance(p);

    const std::size_t inside = p.pos;
    skip_parenthesized_declarator(p);
    const bool alone = names_alone(p, inside, p.pos);
    const bool parameter = naming == naming_t::optional && alone;
    const bool member = naming == naming_t::member && alone;
    expect(p, tok_t::rparen, "expected ')'");

    declarator_t suffix =
        declarator_suffix(p, nullptr, t, typedef_, parameter, member);
    const std::size_t after = p.pos;

    p.pos = inside;
    declarator_t d = declarator(p, suffix.type, naming, typedef_);
    expect(p, tok_t::rparen, "expected ')'");
    p.pos = after;

    if (d.type == suffix.type) {
        d.params = std::move(suffix.params);
        d.scope = std::move(suffix.scope);
        d.star = suffix.star;
        d.zero = suffix.zero;
    }

    return d;
}

// Array suffixes or a parameter list after the name, or after a
// declarator in parentheses, each deriving from t, the type so far; or
// neither, and the type is t.  An array of functions and a function
// returning an array or a function are errors, 6.7.7.3p1, 6.7.7.4p1:
// a suffix that would make one after the other is refused where it
// stands, and the type so far, which a typedef name or the suffixes
// after parentheses made, is refused at the suffix that derives from
// it.  A parameter list opens a scope at its '(' and closes it at its
// ')', the function prototype scope of 6.2.1p4, whether the function is
// the name's or one a pointer points to; the declarator keeps it for a
// definition.  parameter says that the suffixes follow a parameter's
// name or the place of it, so that their first array is the
// parameter's own; member says the same of a member's name.
declarator_t
declarator_suffix(parser_t &p,
                  const token_t *name,
                  const type_t &t,
                  bool typedef_,
                  bool parameter,
                  bool member)
{
    declarator_t d { name, t, std::nullopt, { } };

    if (peek(p).kind == tok_t::lbracket) {
        d.type =
            array_declarator(p, t, typedef_, parameter, member, d.star, d.zero);

        if (peek(p).kind == tok_t::lparen)
            fail(peek(p), "array element type cannot be a function");
    }
    else if (peek(p).kind == tok_t::lparen) {
        const token_t &open = advance(p);

        std::vector< parameter_t > params;
        p.scopes.emplace_back();
        const bool variadic = parameter_type_list(p, params);
        d.scope = std::move(p.scopes.back());
        p.scopes.pop_back();

        if (peek(p).kind == tok_t::lbracket)
            fail(peek(p), "function cannot return an array");

        if (peek(p).kind == tok_t::lparen)
            fail(peek(p), "function cannot return a function");

        if (is< array_t >(t))
            fail(open, "function cannot return an array");

        if (is< function_t >(t))
            fail(open, "function cannot return a function");

        std::vector< type_t > types;
        for (const parameter_t &param : params)
            types.push_back(param.type);

        d.type = make_function(t, std::move(types), variadic);
        d.params = std::move(params);
    }

    return d;
}

// Each '*' derives a pointer to the type so far, 6.7.7.2p1.  A const, a
// volatile or a restrict after it qualifies the pointer and is dropped,
// as in the specifiers; the pointer is one restrict may qualify unless
// it points to a function, as in void (*restrict f)(void).  An _Atomic
// pointer is not yet.
type_t
pointer(parser_t &p, type_t t)
{
    while (accept(p, tok_t::star)) {
        t = make_pointer(std::move(t));

        for (;;) {
            const token_t &q = peek(p);
            if (q.kind == tok_t::kw_atomic)
                not_yet(q, "type qualifier " + quoted(q.text));

            if (q.kind == tok_t::kw_restrict)
                require_restrictable(q, t);

            if (!accept(p, tok_t::kw_const) && !accept(p, tok_t::kw_volatile) &&
                !accept(p, tok_t::kw_restrict))
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

// An array suffix as written: its '[', its count, -1 when the size is
// left out, and the words of what the size needs that rcc does not
// have yet, empty when it needs nothing.
struct array_suffix_t
{
    const token_t *open;
    long count;
    std::string missing;
};

// The size of an array suffix, read up to the ']' that closes it.
expr_t
array_size(parser_t &p)
{
    const token_t &start = peek(p);

    return rvalue(start, assignment_expression(p));
}

// The position of the ']' that closes the array suffix whose size
// starts at the position, the brackets and the parentheses in the size
// matched; or nothing when the size is not one the parser could read
// whole: a bracket that closes what it did not open, a brace, a
// semicolon or the end of the file comes first.
std::optional< std::size_t >
closing_bracket(const parser_t &p)
{
    std::vector< tok_t > open;

    for (std::size_t i = p.pos; i < p.tokens.size(); ++i) {
        const tok_t kind = p.tokens[i].kind;

        if (kind == tok_t::rbracket && open.empty())
            return i;

        switch (kind) {
        case tok_t::lbracket:
        case tok_t::lparen: open.push_back(kind); break;
        case tok_t::rbracket:
        case tok_t::rparen: {
            const tok_t opener =
                kind == tok_t::rbracket ? tok_t::lbracket : tok_t::lparen;
            if (open.empty() || open.back() != opener)
                return std::nullopt;

            open.pop_back();
            break;
        }
        case tok_t::lbrace:
        case tok_t::rbrace:
        case tok_t::semicolon:
        case tok_t::eof: return std::nullopt;
        default: break;
        }
    }

    return std::nullopt;
}

// The size of an array suffix of a typedef's declarator, or nothing
// when it needs what rcc does not have yet, whose words go to missing.
// glibc's bits/procfs.h sizes elf_gregset_t by the size of a struct,
// which rcc does not know; by the rule for a system header the typedef
// is accepted and names a not-yet type, and the use of its object is
// refused.  Only the refusal of the not-yet kind is caught, only while
// the size is read, and only when the ']' that closes the suffix is
// found first, where the parse resumes.  An error of the program in
// the size stays the error it is, and so does a size that reads whole
// and is no constant, or no positive one, which array_count() refuses.
// A refusal inside a parameter list of a type name in the size leaves
// that list's scope open, and the scopes are cut back to those open
// before the size.
std::optional< expr_t >
typedef_array_size(parser_t &p, std::string &missing)
{
    const std::optional< std::size_t > close = closing_bracket(p);
    if (!close)
        return array_size(p);

    const std::size_t depth = p.scopes.size();

    try {
        return array_size(p);
    }
    catch (const not_yet_error_t &e) {
        missing = e.construct;
        p.pos = *close;
        p.scopes.resize(depth);
    }

    return std::nullopt;
}

// The count of the array a parameter is adjusted from, 6.7.7.4p6: as
// any array's, but for a size that is no integer constant expression,
// as one that names an earlier parameter, which is taken as left out.
// At prototype scope such a size stands for '*', 6.7.7.3p5, and the
// parameter is the pointer either way.  In a definition the size is not
// evaluated on entry, as 6.9.2p11 asks, which only a size with a side
// effect shows.
long
parameter_array_count(const token_t &at, const expr_t &size)
{
    if (is< int_t >(size.type) && !is< literal_t >(size))
        return -1;

    return array_count(at, size);
}

// The count of a member's own array: as any array's, but for a
// constant size of zero, GNU C's spelling of a flexible array member,
// which is taken as left out and its token kept in zero.  Only the
// place of a flexible array member takes it, and member_list() refuses
// it anywhere else as any array of size zero, 6.7.7.3p1.
long
member_array_count(const token_t &at, const expr_t &size, const token_t *&zero)
{
    if (is< int_t >(size.type) && is< literal_t >(size) &&
        exact_value(size) == 0) {
        zero = &at;
        return -1;
    }

    return array_count(at, size);
}

// static and the type qualifiers inside a parameter's brackets, before
// the size, 6.7.7.3p3: static first or after the qualifiers, once.  The
// qualifiers are the pointer's the parameter is adjusted to, 6.7.7.4p6,
// and are dropped as after a '*'; restrict is returned in restrict_,
// for the caller to check on that pointer.  _Atomic is not yet, as on a
// pointer.  static promises that each argument points to the first of
// at least as many elements as the size says, p6, which no call rcc
// writes depends on, and is dropped; its token is returned, or null,
// since a size must follow it.
const token_t *
parameter_array_qualifiers(parser_t &p, const token_t *&restrict_)
{
    const token_t *static_ =
        peek(p).kind == tok_t::kw_static ? &advance(p) : nullptr;

    for (;;) {
        const token_t &q = peek(p);
        if (q.kind == tok_t::kw_atomic)
            not_yet(q, "type qualifier " + quoted(q.text));

        if (q.kind == tok_t::kw_restrict && !restrict_)
            restrict_ = &q;

        if (!accept(p, tok_t::kw_const) && !accept(p, tok_t::kw_volatile) &&
            !accept(p, tok_t::kw_restrict))
            break;
    }

    if (!static_ && peek(p).kind == tok_t::kw_static)
        static_ = &advance(p);

    return static_;
}

// The '*' in a parameter's brackets after the qualifiers, 6.7.7.3p3,
// which makes a variable length array of unspecified size, p4: the
// parameter is the pointer either way, so it is the size left out.
// Null when the brackets hold none.  static takes a size, so with the
// '*' or with nothing after it, it is refused.
const token_t *
unspecified_size(parser_t &p, const token_t *static_)
{
    const bool star =
        peek(p).kind == tok_t::star && peek(p, 1).kind == tok_t::rbracket;

    if (static_ && star)
        fail(*static_, "'static' with '*' for the array size");

    if (static_ && peek(p).kind == tok_t::rbracket)
        fail(*static_, "'static' without an array size");

    return star ? &advance(p) : nullptr;
}

// static and the type qualifiers stand only in the outermost array type
// derivation of a parameter, 6.7.7.3p1, and are refused in any other
// brackets.
void
refuse_array_qualifier(const token_t &t)
{
    switch (t.kind) {
    case tok_t::kw_static:
    case tok_t::kw_const:
    case tok_t::kw_volatile:
    case tok_t::kw_restrict:
    case tok_t::kw_atomic:
        fail(t, quoted(t.text) +
                    " outside a parameter's outermost array type derivation");
    default: break;
    }
}

// A size that is no integer constant expression at file scope, extern
// int n; int a[n]; or a typedef of such an array there, is an error of
// the program and no construct to come: an identifier of a variably
// modified type has block or function prototype scope, and an object of
// static storage duration has no variable length array type,
// 6.7.7.3p2.  A parameter's list is a scope of its own, so its arrays
// are not at file scope.
void
refuse_file_scope_variable_size(const parser_t &p,
                                const token_t &at,
                                const expr_t &size)
{
    if (p.scopes.size() == 1 && is< int_t >(size.type) &&
        !is< literal_t >(size))
        fail(at, "variable length array declaration not allowed at file "
                 "scope");
}

// Whether an object type is incomplete, 6.2.5p1: void, an array of
// unknown size, or a record before its list completes it, 6.7.3.4p5.
bool
is_incomplete(const type_t &t)
{
    if (is< void_t >(t))
        return true;

    if (is< array_t >(t))
        return as< array_t >(t).count < 0;

    return is< record_type_t >(t) && !as< record_type_t >(t).record->complete;
}

// One or more [N], N an integer constant expression, or [] for an array
// of unknown size, 6.7.7.3p1, p4.  The size is an assignment-expression
// in the grammar, 6.7.7.1p1.  The suffixes nest from the right: int
// a[2][3] is an array of two arrays of three ints.  An element is of a
// complete type, p1: so only the first suffix may leave its size out,
// and the caller decides whether it may, and no element is an
// incomplete record.  An array of a not-yet type has no size rcc knows,
// so its size is not checked; its object is refused where it would be
// made.  In a typedef's declarator a suffix whose size needs what rcc
// does not have yet makes a not-yet type of the words of what it needs,
// in place of the array.  When parameter says
// that the first suffix is the array a parameter is adjusted from, its
// brackets may hold static and type qualifiers, 6.7.7.3p1, and its size
// need not be an integer constant expression, or may be '*', returned
// in star; every other suffix keeps the rules of any array, so a
// variable length array behind the parameter's pointer is not yet.
// When member says that the first suffix is a member's own array, its
// size may be zero, returned in zero.
type_t
array_declarator(parser_t &p,
                 const type_t &element,
                 bool typedef_,
                 bool parameter,
                 bool member,
                 const token_t *&star,
                 const token_t *&zero)
{
    std::vector< array_suffix_t > suffixes;
    const token_t *restrict_ = nullptr;

    while (peek(p).kind == tok_t::lbracket) {
        array_suffix_t suffix { &advance(p), -1, { } };
        const bool adjusted = parameter && suffixes.empty();
        const bool own = member && suffixes.empty();

        if (adjusted) {
            const token_t *static_ = parameter_array_qualifiers(p, restrict_);
            star = unspecified_size(p, static_);
        }
        else
            refuse_array_qualifier(peek(p));

        if (!(adjusted && star) && peek(p).kind != tok_t::rbracket) {
            const token_t &start = peek(p);

            const std::optional< expr_t > size =
                typedef_ ? typedef_array_size(p, suffix.missing) :
                           array_size(p);
            if (size) {
                refuse_file_scope_variable_size(p, start, *size);
                suffix.count = adjusted ? parameter_array_count(start, *size) :
                               own ? member_array_count(start, *size, zero) :
                                     array_count(start, *size);
            }
        }

        expect(p, tok_t::rbracket, "expected ']'");
        suffixes.push_back(std::move(suffix));
    }

    type_t t = element;

    for (auto it = suffixes.rbegin(); it != suffixes.rend(); ++it) {
        const auto &[open, count, missing] = *it;

        if (is< function_t >(t))
            fail(*open, "array element type cannot be a function");

        if (is_incomplete(t))
            fail(*open, "array has incomplete element type");

        if (!missing.empty()) {
            t = make_not_yet(p.identities++, missing);
            continue;
        }

        if (count >= 0 && !not_yet_in(t) &&
            count > std::numeric_limits< long >::max() / size_of(t))
            fail(*open, "array is too large");

        t = make_array(std::move(t), count);
    }

    if (restrict_)
        require_restrictable(*restrict_,
                             make_pointer(*as< array_t >(t).element));

    return t;
}

// (void), (), or parameters with an optional trailing ", ...", which
// makes the function variadic, the value returned, read in the scope
// the caller opened for the list.  An empty list is (void) in C23,
// 6.7.7.4p13; before C23 it declared a function without a prototype,
// which this parser does not take.  An unnamed parameter of type void
// as the only one is no parameter, p9, whatever spells the type, a
// typedef name among them; void stands nowhere else.  A storage class
// on it leaves the behavior undefined, J.2 (75), so (register void) is
// refused.  Each parameter is declared as it is read, so that the next
// one sees it.
bool
parameter_type_list(parser_t &p, std::vector< parameter_t > &params)
{
    if (accept(p, tok_t::rparen))
        return false;

    bool variadic = false;

    do {
        if (peek(p).kind == tok_t::ellipsis) {
            if (params.empty())
                not_yet(peek(p), "variadic function without named parameters");

            advance(p);
            variadic = true;
            break;
        }

        parameter_t param = parameter_declaration(p);
        if (is< void_t >(param.type)) {
            if (params.empty() && !param.name && !param.in_register &&
                accept(p, tok_t::rparen))
                return false;

            fail(*param.start, "'void' must be the only parameter");
        }

        declare_parameter(p, param);
        params.push_back(std::move(param));
    } while (accept(p, tok_t::comma));

    expect(p, tok_t::rparen, "expected ')' after parameter list");

    return variadic;
}

// Specifiers and a declarator, whose name is optional, and attributes
// after it, as after any declarator; a marking one among the
// specifiers or after the declarator is the parameter's.  A parameter
// of array type is adjusted to a pointer to the element, 6.7.7.4p6, so
// its size may be left out, and one of function type to a pointer to
// the function, p7, so int atexit(void f(void)) and int atexit(void
// (*f)(void)) declare one function.  A parameter of type void is left
// for the list to take or refuse.
parameter_t
parameter_declaration(parser_t &p)
{
    const token_t &start = peek(p);

    const specifiers_t spec = declaration_specifiers(p);

    // 6.7.7.4p2: register is the one storage class a parameter takes
    if (spec.storage_class && !declared(spec, tok_t::kw_register))
        fail(*spec.storage_class,
             quoted(spec.storage_class->text) + " in a parameter declaration");

    refuse_function_specifier(spec);

    const declarator_t d = declarator(p, spec.type, naming_t::optional);

    const token_t *marking = attribute_specifier_list(p);
    if (spec.marking)
        marking = spec.marking;

    // a register parameter of array or function type is a register
    // pointer, since the adjustment comes first
    type_t t = d.type;
    if (is< array_t >(t))
        t = make_pointer(*as< array_t >(t).element);
    else if (is< function_t >(t))
        t = make_pointer(std::move(t));

    return parameter_t { std::move(t), &start,
                         d.name,       declared(spec, tok_t::kw_register),
                         marking,      d.star };
}

// Specifiers and an abstract declarator, the declarator of a
// declaration without its name, 6.7.8p1, p2: int *, int [3],
// void (*)(int).  A marking attribute would make the type one rcc does
// not use, which no name stands for here, so it is not yet.
type_t
type_name(parser_t &p)
{
    const specifiers_t spec = declaration_specifiers(p);

    // 6.7.8p1: a specifier-qualifier list, no storage class
    if (spec.storage_class)
        fail(*spec.storage_class,
             quoted(spec.storage_class->text) + " in a type name");

    refuse_function_specifier(spec);

    if (spec.marking)
        not_yet(*spec.marking, attribute_words(*spec.marking));

    return declarator(p, spec.type, naming_t::none).type;
}

// The declarator of a declaration in a block that is no typedef's.  A
// function declared in a block takes no storage class but extern,
// 6.7.2p13, so static and register on one are errors.
declarator_t
block_declarator(parser_t &p, const specifiers_t &spec)
{
    declarator_t d = declarator(p, spec.type);

    const token_t *storage_class = spec.storage_class;
    if (is< function_t >(d.type) && storage_class &&
        !declared(spec, tok_t::kw_extern))
        fail(*d.name, "function " + quoted(d.name->text) + " declared " +
                          std::string(storage_class->text) + " in a block");

    return d;
}

// What is checked of every object a block declares that has no
// linkage, static or not.  An object is one rcc makes, so an attribute
// that marks it is one rcc would have to honour, and is not yet; so is
// an assembler name, which names a register for a register variable.
// An object's type is complete, 6.7.1p8: not void, not a not-yet type,
// whose size rcc does not know, and not an incomplete record.
void
require_block_object(parser_t &p,
                     const specifiers_t &spec,
                     const declarator_t &d)
{
    refuse_function_specifier(spec);

    const declarator_tail_t tail = declarator_tail(p, spec);
    if (tail.asm_keyword)
        not_yet(*tail.asm_keyword,
                quoted(tail.asm_keyword->text) + " on a local variable");

    if (tail.marking)
        not_yet(*tail.marking, attribute_words(*tail.marking));

    if (is< void_t >(d.type))
        fail(*d.name, "variable " + quoted(d.name->text) + " has type void");

    require_honoured(*d.name, d.type);
    require_complete_record(*d.name, d.type);
}

// The label of a block's static object, backend.md chapter 10: the
// function's name and the object's, next.count, and from the second of
// the name in the function on a number after them, counting from 1,
// next.count.1.  No C identifier holds a '.', so the label is no
// symbol's of the file and no other static object's.
std::string
static_label(parser_t &p, const std::string &name)
{
    const int n = p.statics[name]++;

    std::string label = std::string(p.function) + "." + name;
    if (n > 0)
        label += "." + std::to_string(n);

    return label;
}

// An object a block declares static: of static storage duration,
// 6.2.4p3, and no linkage, 6.2.2p6, so a name of the block's, which
// may hide another and be hidden as a local's is, and is declared once
// in its block, 6.7.1p4.  It is an object the unit defines, as one at
// file scope is, with an entry in the unit's objects and a label of its
// own: its initializer is constant, read as a global's, and gives its
// value once, before the program starts, and without one it is zero,
// 6.7.11p11.  Its declaration makes no statement.  An array of unknown
// size takes its size from its initializer, and is an error without
// one, 6.7.1p8.  The name is in scope from the end of its declarator,
// 6.2.1p7, so the initializer sees the object.
void
static_local(parser_t &p, const specifiers_t &spec, const declarator_t &d)
{
    require_block_object(p, spec, d);

    const bool initialized = peek(p).kind == tok_t::assign;
    if (!initialized && is< array_t >(d.type) &&
        as< array_t >(d.type).count < 0)
        fail(*d.name, "array size missing in " + quoted(d.name->text));

    if (p.scopes.back().names.contains(d.name->text))
        fail(*d.name, "redefinition of " + quoted(d.name->text));

    symbol_t &sym = added(p, d.name->text, d.type, storage_t::global);
    sym.asm_name = static_label(p, sym.name);

    p.unit.objects.push_back(object_decl_t { &sym, std::nullopt });

    if (initialized)
        initialize(p, sym, p.unit.objects.size() - 1);
}

// The entity of linkage the unit has of a name, or null: the file's
// declaration of the name, or the one a block declared first.
symbol_t *
unit_entity(const parser_t &p, std::string_view name)
{
    const auto &file = p.scopes.front().names;
    if (const auto it = file.find(name);
        it != file.end() && it->second->linkage != linkage_t::none)
        return it->second;

    const auto it = p.block_entities.find(name);

    return it == p.block_entities.end() ? nullptr : it->second;
}

// The entity a declaration in a block of a name of linkage denotes:
// that of the visible prior declaration of the name, when it has
// linkage, and its linkage, 6.2.2p4; or else the unit's entity of the
// name, of external linkage, or none yet.  linkage is the linkage the
// declaration gives the name.
symbol_t *
linked_entity(parser_t &p, std::string_view name, linkage_t &linkage)
{
    linkage = linkage_t::external;

    for (auto scope = p.scopes.rbegin(); scope != p.scopes.rend(); ++scope) {
        const auto it = scope->names.find(name);
        if (it == scope->names.end())
            continue;

        if (it->second->linkage == linkage_t::none)
            break;

        linkage = it->second->linkage;
        return it->second;
    }

    return unit_entity(p, name);
}

// A function or an object a block declares extern, or a function it
// declares without a storage class, whose linkage is then as if extern,
// 6.2.2p5.  The name has block scope, 6.2.1p4, and denotes an entity of
// linkage: the one linked_entity() finds, declared before the block at
// file scope or in an enclosing block, after it at file scope, or in
// another block, since every declaration of a name of external linkage
// denotes one entity, 6.2.2p2.  Every declaration of the entity is of
// its kind, its type, 6.2.7p2, and its linkage, 6.2.2p7.  A block
// declares the name once, but for the same entity again, 6.7.1p4.  The
// declaration takes no initializer, 6.7.11p6, and makes no statement:
// the block makes no object for it, and c1 calls the function or reaches
// the object as one declared at file scope.  inline and _Noreturn on a
// function are dropped, as on a declaration at file scope; a function
// declared through a typedef of function type is not yet there as here.
// A body after a function's declarator is an error of its own: a
// function definition is an external declaration, 6.9.1p1, and a
// block holds declarations and statements, 6.8.3p1.
void
linked_declaration(parser_t &p, const specifiers_t &spec, const declarator_t &d)
{
    const bool function = is< function_t >(d.type);
    if (function && !d.params)
        not_yet(*d.name, "function " + quoted(d.name->text) +
                             " declared through a typedef");

    if (!function)
        refuse_function_specifier(spec);

    refuse_specifier_on_main(spec, d);

    declarator_tail_t tail = declarator_tail(p, spec);
    if (function && !tail.marking)
        tail.marking = marked_parameter(d);

    if (const token_t *alias = tail.markings.alias)
        not_yet(*alias, attribute_words(*alias) + " in a block");

    if (function && peek(p).kind == tok_t::lbrace)
        fail(peek(p),
             "function " + quoted(d.name->text) + " defined in a block");

    if (peek(p).kind == tok_t::assign)
        fail(peek(p), quoted(d.name->text) +
                          " declared with linkage in a block cannot have "
                          "an initializer");

    const storage_t storage =
        function ? storage_t::function : storage_t::global;

    linkage_t linkage;
    symbol_t *entity = linked_entity(p, d.name->text, linkage);

    auto &names = p.scopes.back().names;
    const auto here = names.find(d.name->text);
    if (here != names.end() && here->second != entity)
        fail(*d.name, "redefinition of " + quoted(d.name->text));

    if (entity) {
        redeclared(*d.name, *entity, d.type, storage);
        require_linkage(*d.name, *entity, linkage);
        names.emplace(entity->name, entity);
    }
    else {
        entity = &added(p, d.name->text, d.type, storage);
        entity->linkage = linkage;
        p.block_entities.emplace(entity->name, entity);

        if (function)
            p.unit.functions.push_back(
                function_decl_t { entity, { }, std::nullopt, 0 });
    }

    record_tail(*entity, tail);
}

// A block-scope declaration, perhaps of a tag or enumeration constants
// alone, or a static assertion, which make no statement.
std::vector< stmt_t >
declaration(parser_t &p)
{
    skip_extension_keywords(p);

    if (peek(p).kind == tok_t::kw_static_assert) {
        static_assert_declaration(p);
        return { };
    }

    const specifiers_t spec = declaration_specifiers(p);

    if (spec.tagged && accept(p, tok_t::semicolon))
        return { };

    return init_declarator_list(p, spec);
}

// The declarators of a block-scope declaration and its ';': one
// declaration statement per object, in order.  A typedef's declarators
// declare type names and make no statement, and so do a static
// declaration's, whose objects are no locals, and the declarators of
// names of linkage, which declare a function or an object the block
// does not make.
std::vector< stmt_t >
init_declarator_list(parser_t &p, const specifiers_t &spec)
{
    std::vector< stmt_t > decls;

    do {
        if (declared(spec, tok_t::kw_typedef)) {
            refuse_function_specifier(spec);
            type_definition(p, spec,
                            declarator(p, spec.type, naming_t::required, true));
        }
        else {
            const declarator_t d = block_declarator(p, spec);

            if (is< function_t >(d.type) || declared(spec, tok_t::kw_extern))
                linked_declaration(p, spec, d);
            else if (declared(spec, tok_t::kw_static))
                static_local(p, spec, d);
            else
                init_declarator(p, spec, d, decls);
        }
    } while (accept(p, tok_t::comma));

    expect(p, tok_t::semicolon, "expected ';' at end of declaration");

    return decls;
}

// The initializer of a local record that is an expression: a record
// compatible with the object, whose value is the object's initial
// value, 6.7.11p14, so the declaration is followed by the record's
// assignment of it to the new object, the copy's own row.  A value of
// another type is the error of incompatible types, said at the =, and
// a record no lvalue holds, a call's or an assignment's, is a record's
// value, not yet.
expr_t
record_initializer(parser_t &p, const token_t &op, const symbol_t &sym)
{
    const token_t &start = peek(p);
    expr_t init = assignment_expression(p);
    require_value(start, init);

    init = decayed(std::move(init));
    require_record_conversion(op, init.type, sym.type);

    if (!is_lvalue(init))
        refuse_record_value(start, init.type);

    const type_t t = sym.type;
    expr_t object { t, name_t { &sym } };

    return expr_t { t, assign_t { std::nullopt, t, box(std::move(object)),
                                  box(std::move(init)) } };
}

// The list that initializes the local record sym, its '{' ahead: the
// walk of an object of static storage's, 6.7.11p17 to p23, whose
// scalars are stores at run time, local_store()'s, appended to stores
// in the list's order after the declaration, which zeroes the whole
// object, so every member, element and byte of padding no value is
// listed for is zero, p20, p22 (shape.md chapter 5).
void
local_list(parser_t &p, const symbol_t &sym, std::vector< stmt_t > &stores)
{
    destination_t to { .local = &sym, .stores = &stores };
    braced_initializer(p, sym.type, 0, to);
}

// A local, an object of automatic storage duration, 6.2.4p5, appended
// to decls as its declaration, and for a record initialized by an
// expression the record_initializer() statement after it, or by a list
// the declaration zeroed and local_list()'s statements.  The name is in
// scope from the end of its declarator, 6.2.1p7, so an initializer sees
// the object it initializes.  The initializer is converted as by
// assignment, 6.7.11p12, an error of the conversion said at the =; an
// initializer that has no value, a void one, 6.3.2.2p1, is the error
// of the operand, said at its first token.  An object's type must be
// complete, 6.7.1p8, so an array needs its size; any initializer of an
// array, an array of records' among them, and a brace initializer of a
// scalar is not yet: a local's initializer is stores at run time, where
// an object of static storage takes its value as data.  A register array
// is not yet either: its decay is undefined, 6.3.2.1p3, and it has no
// slot to decay to; nor is a register record, which no register holds.
// A local larger than the target's largest frame, its frame_limit, is
// refused here, where its position is known, since no frame holds it.
void
init_declarator(parser_t &p,
                const specifiers_t &spec,
                const declarator_t &d,
                std::vector< stmt_t > &decls)
{
    require_block_object(p, spec, d);

    if (is< array_t >(d.type) && declared(spec, tok_t::kw_register))
        not_yet(*d.name, "register array " + quoted(d.name->text));

    const record_t *r = record_in(d.type);
    if (r && declared(spec, tok_t::kw_register))
        not_yet(*d.name, "register object " + quoted(d.name->text) + " of " +
                             record_words(*r));

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

    const bool record = is< record_type_t >(d.type);
    if (peek(p).kind == tok_t::assign && record &&
        peek(p, 1).kind == tok_t::lbrace) {
        advance(p);
        const symbol_t &sym = *decl.sym;
        decl.zeroed = true;

        decls.push_back(stmt_t { std::move(decl) });
        local_list(p, sym, decls);
        return;
    }

    if (peek(p).kind == tok_t::assign && record) {
        const token_t &op = advance(p);
        expr_t copy = record_initializer(p, op, *decl.sym);

        decls.push_back(stmt_t { std::move(decl) });
        decls.push_back(stmt_t { expr_stmt_t { std::move(copy) } });
        return;
    }

    if (peek(p).kind == tok_t::assign) {
        const token_t &op = advance(p);

        if (peek(p).kind == tok_t::lbrace)
            not_yet(peek(p), "brace initializer");

        const token_t &start = peek(p);
        expr_t init = assignment_expression(p);
        require_value(start, init);

        decl.init = assignment_conversion(op, std::move(init), d.type);
    }

    decls.push_back(stmt_t { std::move(decl) });
}

//
// statements
//

// Whether a label starts at the token ahead, 6.8.2p1: case, default,
// or an identifier and a colon, a typedef name's among them, since a
// label name has a name space of its own, 6.2.3p1.
bool
is_label_start(const parser_t &p)
{
    if (next_is(p, { tok_t::kw_case, tok_t::kw_default }))
        return true;

    return peek(p).kind == tok_t::identifier && peek(p, 1).kind == tok_t::colon;
}

// A label, identifier :, which defines its name in the function's
// labels, 6.2.1p3; a name is defined once in a function, 6.8.2p3.
stmt_t
named_label(parser_t &p)
{
    const token_t &name = advance(p);
    advance(p);

    if (!p.labels.emplace(name.text, &name).second)
        fail(name, "redefinition of label " + quoted(name.text));

    return stmt_t { label_t { std::string(name.text) } };
}

// A case's value in the words of a diagnostic: the value of the
// controlling expression's promoted type, an unsigned long's read from
// the bits its literal holds.
std::string
case_value(long long value, const type_t &t)
{
    if (as< int_t >(t).width == 64 && !as< int_t >(t).is_signed)
        return std::to_string(static_cast< unsigned long long >(value));

    return std::to_string(value);
}

// case, an integer constant expression and a colon, 6.8.2p1.  It stands
// only in a switch, p2, and belongs to the innermost, 6.8.5.3p4.  Its
// constant is converted to the promoted type of the controlling
// expression, p5, and no two of one switch have one value after the
// conversion, p3.  GNU C's range, case 1 ... 5:, is not yet.
stmt_t
case_label(parser_t &p)
{
    const token_t &keyword = advance(p);
    if (p.switches.empty())
        fail(keyword, "'case' label not in a switch");

    const token_t &start = peek(p);
    expr_t constant = rvalue(start, conditional_expression(p));
    if (!is_integer_constant(constant))
        fail(start, "case label is not an integer constant expression");

    if (peek(p).kind == tok_t::ellipsis)
        not_yet(peek(p), "case range");

    expect(p, tok_t::colon, "expected ':' after case label");

    open_switch_t &sw = p.switches.back();
    const long long value =
        as< literal_t >(convert_to(std::move(constant), sw.type)).value;

    const auto [first, added] = sw.values.emplace(value, &start);
    if (!added)
        fail(start, "duplicate case value " + case_value(value, sw.type) +
                        ", first at line " +
                        std::to_string(first->second->where.line));

    sw.cases.push_back({ value, p.case_labels });

    return stmt_t { case_label_t { p.case_labels++ } };
}

// default and a colon, 6.8.2p1: in a switch alone, p2, the innermost's,
// 6.8.5.3p4, and one to a switch, p3.
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

    return stmt_t { case_label_t { p.case_labels++ } };
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

// A label before a statement, where a statement is wanted: the body of
// an if, of a switch, of a loop, or a labeled statement's own.  It is a
// block of the two, which opens no scope.
stmt_t
labeled_statement(parser_t &p)
{
    block_t block;
    block.body.push_back(label(p));
    block.body.push_back(statement(p));

    return stmt_t { std::move(block) };
}

// A declaration is a block item, not a statement: it cannot be the
// body of an if or of a loop.
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
    case tok_t::kw_asm: not_yet(t, quoted(t.text) + " statement");
    default: break;
    }

    if (is_label_start(p))
        return labeled_statement(p);

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
    close_block(p);
    expect(p, tok_t::rbrace, "expected '}' at end of block");

    return block;
}

// The items up to the closing brace, which is left for the caller.  A
// label is an item of its own, 6.8.3p1, so it may stand before a
// declaration and before the closing brace.
void
block_item_list(parser_t &p, block_t &block)
{
    while (!next_is(p, { tok_t::rbrace, tok_t::eof })) {
        if (is_label_start(p))
            block.body.push_back(label(p));
        else if (is_declaration_start(p)) {
            for (stmt_t &decl : declaration(p))
                block.body.push_back(std::move(decl));
        }
        else
            block.body.push_back(statement(p));
    }
}

// An expression whose value is discarded may be void; an empty one is
// the null statement.  Attributes before an empty one's ';' stand on
// the null statement, as GNU C's __attribute__ ((fallthrough)); does: one
// dropped is dropped, so the statement writes nothing, and one that
// marks is not yet, by its name, as on anything else rcc would make.
// Attributes before an expression are no part of the grammar, and the
// expression refuses them.
stmt_t
expression_statement(parser_t &p)
{
    if (peek(p).kind == tok_t::kw_attribute &&
        peek(p, past_attributes(p, 0)).kind == tok_t::semicolon) {
        if (const token_t *marking = attribute_specifier_list(p))
            not_yet(*marking, attribute_words(*marking));
    }

    if (accept(p, tok_t::semicolon))
        return stmt_t { expr_stmt_t { } };

    const token_t &start = peek(p);
    expr_t e = expression(p);

    expect(p, tok_t::semicolon, "expected ';' after expression");

    return stmt_t { expr_stmt_t { discarded(start, std::move(e)) } };
}

// A switch, 6.8.5.3: the controlling expression, an integer, p1, and
// promoted, p5; then the body, in which a break leaves the switch,
// 6.8.7.4p2, a continue goes on with the loop around it, and each case
// and default label belongs to this switch unless a switch inside the
// body is closer, p4.
stmt_t
switch_statement(parser_t &p)
{
    advance(p);
    expect(p, tok_t::lparen, "expected '(' after 'switch'");

    const token_t &start = peek(p);
    expr_t cond = rvalue(start, expression(p));
    if (!is< int_t >(cond.type))
        fail(start, "controlling expression of 'switch' is not an integer");
    cond = promote(std::move(cond));

    expect(p, tok_t::rparen, "expected ')' after controlling expression");

    p.switches.push_back({ cond.type });
    ++p.break_targets;

    stmt_ptr_t body = box(statement(p));

    --p.break_targets;
    open_switch_t sw = std::move(p.switches.back());
    p.switches.pop_back();

    return stmt_t { switch_t { std::move(cond), std::move(body),
                               std::move(sw.cases), sw.default_ } };
}

// The condition stays in its own type, an integer or a pointer; c1
// compares it against zero, 6.8.5.2p2.
stmt_t
selection_statement(parser_t &p)
{
    if (peek(p).kind == tok_t::kw_switch)
        return switch_statement(p);

    advance(p);
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
    case tok_t::kw_do: return do_statement(p);
    default: return for_statement(p);
    }
}

// The body of a loop, where a break and a continue may stand,
// 6.8.7.3p1, 6.8.7.4p1.
stmt_ptr_t
loop_body(parser_t &p)
{
    ++p.break_targets;
    ++p.continue_targets;

    stmt_ptr_t body = box(statement(p));

    --p.break_targets;
    --p.continue_targets;

    return body;
}

stmt_t
while_statement(parser_t &p)
{
    advance(p);
    expect(p, tok_t::lparen, "expected '(' after 'while'");

    const token_t &start = peek(p);
    expr_t cond = rvalue(start, expression(p));

    expect(p, tok_t::rparen, "expected ')' after condition");

    stmt_ptr_t body = loop_body(p);

    return stmt_t { while_t { std::move(cond), std::move(body) } };
}

// The body, then the condition, evaluated after each execution of the
// body, 6.8.6.3p1, and the ';' that ends the statement.
stmt_t
do_statement(parser_t &p)
{
    advance(p);

    stmt_ptr_t body = loop_body(p);

    expect(p, tok_t::kw_while, "expected 'while' after the body of 'do'");
    expect(p, tok_t::lparen, "expected '(' after 'while'");

    const token_t &start = peek(p);
    expr_t cond = rvalue(start, expression(p));

    expect(p, tok_t::rparen, "expected ')' after condition");
    expect(p, tok_t::semicolon, "expected ';' after do statement");

    return stmt_t { do_t { std::move(body), std::move(cond) } };
}

// The declaration of a for's first clause, as the init statement:
// several declarators become one block of declaration statements, since
// the tree holds a single init statement.  A storage class but register
// is refused there, by C17's rule that the declaration declare objects
// of storage class auto or register only, its 6.8.5p3; N3220 has no
// such rule, and a function declared there without one is taken, as C23
// lets it.  A static assertion, a declaration too, 6.7.1p1, makes no
// statement, and the init is null.
stmt_ptr_t
for_declaration(parser_t &p)
{
    skip_extension_keywords(p);

    if (peek(p).kind == tok_t::kw_static_assert) {
        static_assert_declaration(p);
        return nullptr;
    }

    const specifiers_t spec = declaration_specifiers(p);
    if (spec.storage_class && !declared(spec, tok_t::kw_register))
        fail(*spec.storage_class, quoted(spec.storage_class->text) +
                                      " in the declaration of a 'for'");

    std::vector< stmt_t > decls = init_declarator_list(p, spec);
    if (decls.size() == 1)
        return box(std::move(decls.front()));

    return box(stmt_t { block_t { std::move(decls) } });
}

// A declaration in the init is in scope for the whole loop, 6.8.6.4p1,
// so the for opens a scope; a braced body nests inside it.  The init
// and the step are evaluated as void expressions there.
stmt_t
for_statement(parser_t &p)
{
    advance(p);
    expect(p, tok_t::lparen, "expected '(' after 'for'");

    p.scopes.emplace_back();

    for_t loop;

    if (is_declaration_start(p))
        loop.init = for_declaration(p);
    else if (!accept(p, tok_t::semicolon))
        loop.init = box(expression_statement(p));

    if (peek(p).kind != tok_t::semicolon) {
        const token_t &start = peek(p);
        loop.cond = rvalue(start, expression(p));
    }

    expect(p, tok_t::semicolon, "expected ';' after for condition");

    if (peek(p).kind != tok_t::rparen) {
        const token_t &start = peek(p);
        loop.step = discarded(start, expression(p));
    }

    expect(p, tok_t::rparen, "expected ')' after for clauses");

    loop.body = loop_body(p);

    close_block(p);

    return stmt_t { std::move(loop) };
}

// A goto names a label of the function, defined before it or after
// it, so the name is checked at the function's end, 6.8.7.2p1.
stmt_t
goto_statement(parser_t &p)
{
    advance(p);

    const token_t &name =
        expect(p, tok_t::identifier, "expected label name after 'goto'");
    p.gotos.push_back(&name);

    expect(p, tok_t::semicolon, "expected ';' after goto statement");

    return stmt_t { goto_t { std::string(name.text) } };
}

// A continue stands only in the body of a loop, 6.8.7.3p1.
stmt_t
continue_statement(parser_t &p)
{
    const token_t &keyword = advance(p);
    if (p.continue_targets == 0)
        fail(keyword, "'continue' statement not in a loop");

    expect(p, tok_t::semicolon, "expected ';' after continue statement");

    return stmt_t { continue_t { } };
}

// A break stands only in the body of a loop or of a switch, 6.8.7.4p1.
stmt_t
break_statement(parser_t &p)
{
    const token_t &keyword = advance(p);
    if (p.break_targets == 0)
        fail(keyword, "'break' statement not in a loop or a switch");

    expect(p, tok_t::semicolon, "expected ';' after break statement");

    return stmt_t { break_t { } };
}

// A value is converted to the result type as if by assignment,
// 6.8.7.5p3; a void function takes no value and a non-void one
// requires one, 6.8.7.5p1.
stmt_t
return_statement(parser_t &p)
{
    const token_t &keyword = advance(p);

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

//
// expressions, from the comma down
//

// The left operand is evaluated for its effect and may be void, 6.5.18p2.
expr_t
expression(parser_t &p)
{
    expr_t e = assignment_expression(p);

    while (peek(p).kind == tok_t::comma) {
        e = discarded(advance(p), std::move(e));
        e = comma(std::move(e), assignment_expression(p));
    }

    return e;
}

// A record's simple assignment, 6.5.17.2p1: its left operand a
// modifiable lvalue of a complete record, 6.3.2.1p1, and its right one
// of a compatible record, which rcc takes as the same record, so that
// two records of one tag are not yet.  The right operand is an lvalue
// too, since a record's value is no operand yet: the value of another
// assignment or of a comma is refused in refuse_record_value()'s words.
// The tree is an assign_t of the record's type without an operation,
// the two lvalues, for c1 to copy the right's object into the left's.
expr_t
record_assignment(parser_t &p, const token_t &op, expr_t lhs)
{
    const record_t &r = *as< record_type_t >(lhs.type).record;
    if (!is_lvalue(lhs))
        fail(op, "expression is not assignable");

    require_whole_record(op, lhs.type);

    const token_t &start = peek(p);
    expr_t rhs = assignment_expression(p);

    refuse_tag_twins(op, lhs.type, rhs.type);
    if (!(rhs.type == lhs.type))
        fail(op,
             "assigning to " + record_words(r) + " from an incompatible type");

    if (!is_lvalue(rhs))
        refuse_record_value(start, rhs.type);

    const type_t t = lhs.type;

    return expr_t { t, assign_t { std::nullopt, t, box(std::move(lhs)),
                                  box(std::move(rhs)) } };
}

expr_t
assignment_expression(parser_t &p)
{
    expr_t lhs = conditional_expression(p);
    if (!is_assignment_operator(peek(p).kind))
        return lhs;

    const token_t &op = advance(p);
    if (op.kind == tok_t::assign && is< record_type_t >(lhs.type))
        return record_assignment(p, op, std::move(lhs));

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
    case tok_t::plus: advance(p); return unary_plus(op, cast_expression(p));
    case tok_t::minus:
    case tok_t::tilde:
        advance(p);
        return unary_arithmetic(op, cast_expression(p));
    case tok_t::bang: advance(p); return logical_not(op, cast_expression(p));
    case tok_t::amp: advance(p); return address_of(op, cast_expression(p));
    case tok_t::star: advance(p); return indirection(op, cast_expression(p));
    case tok_t::kw_sizeof: return sizeof_expression(p);
    case tok_t::kw_alignof: return alignof_expression(p);
    default: return postfix_expression(p);
    }
}

// sizeof E or sizeof (T).  The operand expression is typed and dropped,
// never evaluated, and does not decay, so the size of an array is the
// whole array's, 6.5.4.4p4.  A name of internal linkage it uses needs no
// definition, 6.9.1p3, since the size is an integer constant, and its
// uses are dropped.
expr_t
sizeof_expression(parser_t &p)
{
    const token_t &op = advance(p);
    const auto uses = std::ssize(p.internal_uses);

    type_t t = make_void();
    if (starts_type_name(p)) {
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

// _Alignof ( T ), 6.5.4.4p1: the operand is a type name alone.  GNU
// C's alignment of an expression is not yet.
expr_t
alignof_expression(parser_t &p)
{
    const token_t &op = advance(p);
    if (!starts_type_name(p))
        not_yet(op, quoted(op.text) + " of an expression");

    advance(p);
    const type_t t = type_name(p);
    expect(p, tok_t::rparen, "expected ')' after type name");

    return alignof_(op, t);
}

// The callee of a call, 6.5.3.3p1.  A function's name, in parentheses
// or not, stays the name: the direct call, which names its function.
// Any other callee decays as a value does and must then be a pointer to
// a function: the call through it, (*f)(x) and (&f)(x) included, since
// the source wrote the address.  The name of anything else is refused
// by name.
expr_t
callee(const token_t &op, expr_t e)
{
    if (is< name_t >(e) && is< function_t >(e.type))
        return e;

    if (is< name_t >(e) && !is_function_pointer(e.type))
        fail(op, "called object " + quoted(as< name_t >(e).sym->name) +
                     " is not a function");

    e = rvalue(op, std::move(e));
    if (!is_function_pointer(e.type))
        fail(op, "called object is not a function or a pointer to one");

    return e;
}

// The type a call is made with: the named function's, or the pointee of
// the callee's pointer, which is the call site's type whatever the
// pointer was converted from (G16, 6.5.3.3p7).
const function_t &
callee_type(const expr_t &callee)
{
    if (is< function_t >(callee.type))
        return as< function_t >(callee.type);

    return as< function_t >(pointee_of(callee.type));
}

// The words a count error names the callee by: the function, or the
// pointer it is called through.
std::string
callee_words(const expr_t &callee)
{
    if (is< function_t >(callee.type))
        return "to function " + quoted(as< name_t >(callee).sym->name);

    return "in a call through a pointer";
}

// The member of a record by its name, 6.5.3.4p1, p2; a name the record
// has no member of is an error.
const member_t &
named_member(const record_t &r, const token_t &name)
{
    for (const member_t &m : r.members)
        if (m.name == name.text)
            return m;

    fail(name,
         "no member named " + quoted(name.text) + " in " + record_words(r));
}

// The record a member access reads a member of, 6.5.3.4p1, p2: the
// record E is, for E.m, and the one P points to, for P->m, P decayed.
// It is complete, since its members are known from its list's '}' on,
// and laid out, or the access is refused by the words of what rcc does
// not lay out.
const record_t &
accessed_record(const token_t &op, const type_t &t)
{
    const bool arrow = op.kind == tok_t::arrow;
    if (arrow && !is< pointer_t >(t))
        fail(op, "member reference type is not a pointer");

    const type_t &r = arrow ? pointee_of(t) : t;
    if (!is< record_type_t >(r))
        fail(op, arrow ? "member reference type is not a pointer to a "
                         "struct or a union" :
                         "member reference base type is not a struct or a "
                         "union");

    const record_t &record = *as< record_type_t >(r).record;
    if (!record.complete)
        fail(op, "member access into incomplete type " + record_words(record));

    require_honoured(op, r);

    return record;
}

// The pointer through which a member access reaches its record, and
// the constant in bytes added to it, record_pointer()'s.
struct record_pointer_t
{
    expr_t pointer;
    long offset;
};

// A pointer read through its casts between pointer types.
expr_t
uncast(expr_t e)
{
    while (is< cast_t >(e) && is< pointer_t >(as< cast_t >(e).arg->type)) {
        expr_t arg = std::move(*std::get< cast_t >(e.v).arg);
        e = std::move(arg);
    }

    return e;
}

// The sum of a pointer to bytes and an integer constant, whose
// constant a member access takes out of it.
bool
is_byte_sum(const expr_t &e)
{
    if (!is< binary_t >(e) || as< binary_t >(e).op != binary_op_t::add)
        return false;

    const type_t &t = pointee_of(e.type);

    return is< int_t >(t) && as< int_t >(t).width == 8 &&
           is< literal_t >(*as< binary_t >(e).rhs);
}

// The address of the record of E.m, (&E)->m, 6.5.3.4p4's footnote: E
// is an lvalue, as every record rcc makes an expression of is, and &*X
// is X, 6.5.4.2p3.
expr_t
record_address(const token_t &op, expr_t e)
{
    if (!is_lvalue(e))
        not_yet(op, "member of a value of " +
                        record_words(*as< record_type_t >(e.type).record));

    if (is< unary_t >(e) && as< unary_t >(e).op == unary_op_t::deref)
        return std::move(*std::get< unary_t >(e.v).arg);

    return address_of(op, std::move(e));
}

// The pointer of P->m, P's value, or of E.m, record_address()'s.  A
// cast between pointer types changes no address and is read through,
// and so is a sum of a pointer to bytes and a constant, whose constant
// is taken out, so that the members of a member, the tree a member
// access makes, are one offset from the outermost pointer.
record_pointer_t
record_pointer(const token_t &op, expr_t e)
{
    if (op.kind == tok_t::dot)
        e = record_address(op, std::move(e));

    e = uncast(std::move(e));
    if (!is_byte_sum(e))
        return { std::move(e), 0 };

    auto &sum = std::get< binary_t >(e.v);
    const long offset = long(as< literal_t >(*sum.rhs).value);

    return { uncast(std::move(*sum.lhs)), offset };
}

// The lvalue of the member m of the record E is, for E.m, or of the one
// P points to, for P->m, P an rvalue, op saying which, 6.5.3.4p3, p4:
// *(T *) ((char *) P + offset), lvalue_at()'s, of the member's type,
// the offset the member's in its record plus the one P carries.
expr_t
member_lvalue(const parser_t &p, const token_t &op, expr_t e, const member_t &m)
{
    auto [pointer, offset] = record_pointer(op, std::move(e));

    return lvalue_at(p, std::move(pointer), offset + m.offset, m.type);
}

// E.m and P->m, 6.5.3.4: the lvalue of the member, p3, p4, member_lvalue()'s.
// A member of an array type decays where its value is used, and one of
// a record type is an lvalue whose value is refused and whose members
// are reached.  A member of a floating type, or an array of one, which
// the record lays out and rcc does not use yet, is refused by the
// type's words, its address as its value.
expr_t
member_access(parser_t &p, const token_t &op, expr_t e)
{
    const token_t &name = expect(p, tok_t::identifier, "expected member name");

    if (op.kind == tok_t::arrow) {
        if (is< record_type_t >(e.type))
            fail(op, "member reference type is not a pointer");

        e = rvalue(op, std::move(e));
    }

    const member_t &m = named_member(accessed_record(op, e.type), name);
    if (is_sized(m.type))
        not_yet(name, *not_yet_in(m.type));

    return member_lvalue(p, op, std::move(e), m);
}

//
// the variable argument lists, 7.16
//

// A token for an operator of a tree the parser builds for a builtin of
// stdarg.h, at the builtin's place: the builders read the kind.
token_t
operator_at(const token_t &at, tok_t kind)
{
    token_t op = at;
    op.kind = kind;

    return op;
}

expr_t
int_constant(long long value)
{
    return expr_t { int_type(), literal_t { value } };
}

// Whether a unary operator steps its operand, ++ or --, prefix or
// postfix, an assignment, 6.5.3.5p2, 6.5.4.1p2.
bool
is_step_operator(unary_op_t op)
{
    return op == unary_op_t::pre_inc || op == unary_op_t::pre_dec ||
           op == unary_op_t::post_inc || op == unary_op_t::post_dec;
}

std::optional< expr_t > pure_copy(const expr_t &);

// pure_copy() of an operand, boxed, or null when it has a side effect.
expr_ptr_t
pure_operand(const expr_ptr_t &e)
{
    std::optional< expr_t > copy = pure_copy(*e);

    return copy ? box(std::move(*copy)) : nullptr;
}

// pure_copy() of a node of operands, each copied by pure_operand(), or
// nothing when one has a side effect.
std::optional< expr_t >
pure_node(const expr_t &e)
{
    if (is< unary_t >(e) && !is_step_operator(as< unary_t >(e).op)) {
        expr_ptr_t arg = pure_operand(as< unary_t >(e).arg);
        if (arg)
            return expr_t { e.type,
                            unary_t { as< unary_t >(e).op, std::move(arg) } };
    }

    if (is< binary_t >(e)) {
        const auto &b = as< binary_t >(e);
        expr_ptr_t lhs = pure_operand(b.lhs), rhs = pure_operand(b.rhs);
        if (lhs && rhs)
            return expr_t { e.type,
                            binary_t { b.op, std::move(lhs), std::move(rhs) } };
    }

    if (is< cast_t >(e))
        if (expr_ptr_t arg = pure_operand(as< cast_t >(e).arg))
            return expr_t { e.type, cast_t { std::move(arg) } };

    if (is< conditional_t >(e)) {
        const auto &c = as< conditional_t >(e);
        expr_ptr_t cond = pure_operand(c.cond), yes = pure_operand(c.yes),
                   no = pure_operand(c.no);
        if (cond && yes && no)
            return expr_t { e.type,
                            conditional_t { std::move(cond), std::move(yes),
                                            std::move(no) } };
    }

    return std::nullopt;
}

// A copy of an expression that has no side effect, 5.1.2.4p2: no
// assignment, no step and no call in it; nothing for any other.  A tree
// that reads a list more than once reads it through such copies, so
// that no effect repeats.
std::optional< expr_t >
pure_copy(const expr_t &e)
{
    if (is< literal_t >(e))
        return expr_t { e.type, as< literal_t >(e) };

    if (is< name_t >(e))
        return expr_t { e.type, as< name_t >(e) };

    if (is< string_t >(e))
        return expr_t { e.type, as< string_t >(e) };

    return pure_node(e);
}

// The address of the list record an operand of a builtin designates,
// which the trees read the members through.  On x86-64 the list is an
// array of one record, which decays to its address, or a parameter of
// it, which is that pointer, 6.7.7.4p6; on aarch64 an lvalue of the
// record, whose address is taken, &*P being P, the parameter's among
// them.  An aarch64 list that is no lvalue is a record's value, not
// yet; any other operand is no list.  words name the operand.
expr_t
list_pointer(const parser_t &p,
             const token_t &at,
             std::string_view words,
             expr_t e)
{
    const type_t r = make_record(*p.va_record);
    const bool psabi = p.target.va_list == va_list_t::psabi;

    if (psabi && is< array_t >(e.type) && *as< array_t >(e.type).element == r)
        return decayed(std::move(e));

    if (psabi && is< pointer_t >(e.type) && pointee_of(e.type) == r)
        return rvalue(at, std::move(e));

    if (!psabi && e.type == r && !is_lvalue(e))
        refuse_record_value(at, e.type);

    if (!psabi && e.type == r)
        return record_address(at, std::move(e));

    fail(at, std::string(words) + " is not a va_list");
}

// A list operand a tree reads more than once: the expression read
// through a copy each time, where it stands and its words, and, for a
// list with a side effect, the assignment of its address to a
// temporary, which the tree evaluates first and reads through after.
struct list_operand_t
{
    const token_t &at;
    std::string words;
    expr_t e;
    std::optional< expr_t > set = { };
};

// A temporary that no declaration names, of the type, for a builtin
// that holds a value across another evaluation.  In a body it is
// declared at the body's head, declare_temporaries(), so its slot is
// the activation's, G12.  At file scope, where no expression is
// generated, an initializer's being folded or refused, and sizeof's
// operand not evaluated, it is declared nowhere.
const symbol_t &
temporary(parser_t &p, std::string_view name, const type_t &type)
{
    symbol_t &t = p.unit.symbols.emplace_back(
        symbol_t { std::string(name), type, storage_t::local, false, -1 });

    if (p.temporaries)
        p.temporaries->push_back(&t);

    return t;
}

// The temporary of one builtin's list with a side effect, a pointer to
// the list's record.
const symbol_t &
list_temporary(parser_t &p)
{
    return temporary(p, "__va_list_address",
                     make_pointer(make_record(*p.va_record)));
}

// The list ahead, the first operand of the builtin of the macro named.
// One without a side effect is read through copies of its expression,
// pure_copy()'s, as often as the tree reads it.  One with a side effect
// is evaluated once, where the builtin stands, G13: its address is
// assigned to a temporary first, and the tree reads the list through
// the temporary, the pointer itself on x86-64, the record it points to
// on aarch64.
list_operand_t
list_operand(parser_t &p, std::string_view macro)
{
    const token_t &at = peek(p);
    expr_t e = assignment_expression(p);
    std::string words = "first argument to " + quoted(macro);

    if (pure_copy(e)) {
        list_pointer(p, at, words, std::move(*pure_copy(e)));
        return { at, std::move(words), std::move(e) };
    }

    expr_t address = list_pointer(p, at, words, std::move(e));
    const symbol_t &t = list_temporary(p);
    expr_t set =
        assignment(operator_at(at, tok_t::assign),
                   expr_t { t.type, name_t { &t } }, std::move(address));

    expr_t list { t.type, name_t { &t } };
    if (p.target.va_list == va_list_t::aapcs64)
        list = indirection(at, std::move(list));

    return { at, std::move(words), std::move(list), std::move(set) };
}

// A builtin's tree e, after the assignment of its list's address to
// the list's temporary, if the list has one: (temporary = &list, e).
expr_t
after_list(const token_t &at, list_operand_t &l, expr_t e)
{
    if (!l.set)
        return e;

    return comma(discarded(at, std::move(*l.set)), std::move(e));
}

// The member of index k of a list operand's record, an lvalue read
// through a copy of the operand.
expr_t
list_member(const parser_t &p, const list_operand_t &l, std::size_t k)
{
    expr_t pointer = list_pointer(p, l.at, l.words, std::move(*pure_copy(l.e)));

    return member_lvalue(p, operator_at(l.at, tok_t::arrow), std::move(pointer),
                         p.va_record->members[k]);
}

// What a list's tree reads of its record, the members by index: the
// offset of the next general register, the save area it is an offset
// into, and the next argument on the stack; and the test that a
// register is left, the offset against its end, as clang writes it.
// x86-64: gp_offset <= 40, of six registers of 8 bytes; aarch64:
// __gr_offs < 0, which counts up to 0 from below the top.
struct list_layout_t
{
    std::size_t offset, area, stack;
    tok_t test;
    long long end;
};

list_layout_t
list_layout(const parser_t &p)
{
    if (p.target.va_list == va_list_t::psabi)
        return { 0, 3, 2, tok_t::le, 40 };

    return { 3, 1, 0, tok_t::lt, 0 };
}

// A member of a list's record that holds an address, read as a pointer
// to bytes plus n, or less -n, n == 0 adding nothing.
expr_t
list_bytes(const parser_t &p, const list_operand_t &l, std::size_t k, long n)
{
    const type_t bytes = make_pointer(char_type(p));
    expr_t e = cast(l.at, bytes, list_member(p, l, k));
    if (n == 0)
        return e;

    if (n < 0)
        return subtraction(operator_at(l.at, tok_t::minus), std::move(e),
                           int_constant(-n));

    return addition(operator_at(l.at, tok_t::plus), std::move(e),
                    int_constant(n));
}

// The address of the next argument of a list, 7.16.1.1p2, as the
// convention finds it, each argument of rcc's types in one slot of 8
// bytes: while a register is left, the save area's slot at the list's
// offset, the offset stepped past it; then the one on the stack, the
// address stepped past it.  On x86-64 gp_offset <= 40 ? (gp_offset +=
// 8, (char *) reg_save_area + (gp_offset - 8)) : (overflow_arg_area =
// (char *) overflow_arg_area + 8, (char *) overflow_arg_area - 8); on
// aarch64 the same of __gr_offs < 0, __gr_top and __stack.
expr_t
next_argument(const parser_t &p, const list_operand_t &l)
{
    const list_layout_t m = list_layout(p);
    const token_t &at = l.at;

    expr_t test = comparison(operator_at(at, m.test),
                             list_member(p, l, m.offset), int_constant(m.end));

    expr_t step = assignment(operator_at(at, tok_t::add_assign),
                             list_member(p, l, m.offset), int_constant(8));
    expr_t slot =
        addition(operator_at(at, tok_t::plus), list_bytes(p, l, m.area, 0),
                 subtraction(operator_at(at, tok_t::minus),
                             list_member(p, l, m.offset), int_constant(8)));
    expr_t in_area = comma(discarded(at, std::move(step)), std::move(slot));

    expr_t past =
        assignment(operator_at(at, tok_t::assign), list_member(p, l, m.stack),
                   list_bytes(p, l, m.stack, 8));
    expr_t on_stack =
        comma(discarded(at, std::move(past)), list_bytes(p, l, m.stack, -8));

    return conditional(operator_at(at, tok_t::question),
                       rvalue(at, std::move(test)), std::move(in_area),
                       std::move(on_stack));
}

// The type va_arg reads, 7.16.1.1p2: an object type, complete.  One rcc
// has the value of, an integer or a pointer, is read; a char or a short,
// which no argument has after the promotions, reads the low bytes of the
// promoted one.  A record by value, and a not-yet type, floating ones
// among them, are not yet.
void
require_argument_type(const token_t &at, const type_t &t)
{
    if (is< void_t >(t))
        fail(at, "va_arg of type void");

    if (is< function_t >(t))
        fail(at, "va_arg of a function type");

    if (is< array_t >(t))
        fail(at, "va_arg of an array type");

    if (const record_t *r = record_in(t)) {
        if (!r->complete)
            fail(at, "va_arg of incomplete type " + record_words(*r));

        not_yet(at, "va_arg of " + record_words(*r));
    }

    require_honoured(at, t);
}

// __builtin_va_arg ( list , type-name ), va_arg, 7.16.1.1: the value of
// the next argument, read at the type from the slot next_argument()
// finds, (T) *(T *) slot, the cast making it no lvalue; for a list with
// a side effect, after_list()'s.
expr_t
va_arg_expression(parser_t &p, const token_t &keyword)
{
    expect(p, tok_t::lparen, "expected '(' after " + quoted(keyword.text));
    list_operand_t l = list_operand(p, "va_arg");
    expect(p, tok_t::comma, "expected ',' after the list");

    const token_t &at = peek(p);
    const type_t t = type_name(p);
    require_argument_type(at, t);
    expect(p, tok_t::rparen, "expected ')' after type name");

    expr_t next = after_list(keyword, l, next_argument(p, l));
    expr_t slot = cast(keyword, make_pointer(t), std::move(next));

    return cast(keyword, t, indirection(keyword, std::move(slot)));
}

// The function's save area, made at its first va_start, 7.16.1.4: a
// local of bytes no declaration names, in no scope, which the prologue
// fills.  x86-64: 184 bytes, rdi to r9 from 0, xmm0 to xmm7 from 48,
// and at 176 the address of the arguments the caller left on the stack
// past the named ones; aarch64: 200 bytes, x0 to x7 from 0, q0 to q7
// from 64, the address at 192.  docs/shape.md chapter 4.
const symbol_t &
va_area(parser_t &p)
{
    const long size = p.target.va_list == va_list_t::psabi ? 184 : 200;

    if (!p.va_area)
        p.va_area = &p.unit.symbols.emplace_back(
            symbol_t { "__va_area", make_array(make_int(8, false), size),
                       storage_t::local, false, -1 });

    return *p.va_area;
}

// The save area as a pointer to bytes plus n, or the address stored at
// n in it, read as a pointer to void.
expr_t
area_bytes(const parser_t &p, const token_t &at, const symbol_t &area, long n)
{
    expr_t name { area.type, name_t { &area } };
    expr_t e =
        cast(at, make_pointer(char_type(p)), address_of(at, std::move(name)));
    if (n == 0)
        return e;

    return addition(operator_at(at, tok_t::plus), std::move(e),
                    int_constant(n));
}

expr_t
area_address(const parser_t &p, const token_t &at, const symbol_t &area, long n)
{
    const type_t pointer = make_pointer(make_void());

    return indirection(
        at, cast(at, make_pointer(pointer), area_bytes(p, at, area, n)));
}

// What va_start writes into the members of a list's record, in their
// order.  x86-64: gp_offset 8 for each named parameter, of the six in
// registers; fp_offset 48, past the six, as no parameter rcc has is in
// a vector register; overflow_arg_area the address at 176 of the area;
// reg_save_area the area.  aarch64: __stack the address at 192;
// __gr_top the area plus 64, past x0 to x7, and __vr_top plus 192, past
// q0 to q7; __gr_offs -8 for each of x0 to x7 past the named
// parameters; __vr_offs -128, all eight q.
std::vector< expr_t >
start_values(parser_t &p, const token_t &at)
{
    const symbol_t &area = va_area(p);
    const long named = static_cast< long >(p.named);

    std::vector< expr_t > values;
    if (p.target.va_list == va_list_t::psabi) {
        values.push_back(int_constant(8 * std::min(named, 6L)));
        values.push_back(int_constant(48));
        values.push_back(area_address(p, at, area, 176));
        values.push_back(area_bytes(p, at, area, 0));
    }
    else {
        values.push_back(area_address(p, at, area, 192));
        values.push_back(area_bytes(p, at, area, 64));
        values.push_back(area_bytes(p, at, area, 192));
        values.push_back(int_constant(-8 * (8 - std::min(named, 8L))));
        values.push_back(int_constant(-128));
    }

    return values;
}

// __builtin_va_start ( list , parameter ), va_start, 7.16.1.4: in a
// variadic definition, each member of the list set to start_values()'s,
// in order, after_list()'s for a list with a side effect, the whole
// discarded, p6.  Only the list is evaluated, p4:
// the second operand, the last named parameter as the dialect rcc
// preprocesses in writes it, is neither evaluated nor checked.
expr_t
va_start_expression(parser_t &p, const token_t &keyword)
{
    if (!p.variadic)
        fail(keyword, "'va_start' used outside a variadic function");

    expect(p, tok_t::lparen, "expected '(' after " + quoted(keyword.text));
    list_operand_t l = list_operand(p, "va_start");
    expect(p, tok_t::comma, "expected ',' after the list");
    assignment_expression(p);
    expect(p, tok_t::rparen, "expected ')' after the parameter");

    std::vector< expr_t > values = start_values(p, keyword);
    const token_t assign = operator_at(keyword, tok_t::assign);

    expr_t e = assignment(assign, list_member(p, l, 0), std::move(values[0]));
    for (std::size_t k = 1; k < values.size(); ++k)
        e = comma(
            discarded(keyword, std::move(e)),
            assignment(assign, list_member(p, l, k), std::move(values[k])));

    return cast(keyword, make_void(), after_list(keyword, l, std::move(e)));
}

// __builtin_va_end ( list ), va_end, 7.16.1.3: nothing on either target
// but the list, evaluated for its address and discarded.
expr_t
va_end_expression(parser_t &p, const token_t &keyword)
{
    expect(p, tok_t::lparen, "expected '(' after " + quoted(keyword.text));
    const token_t &at = peek(p);
    expr_t list =
        list_pointer(p, at, "argument to 'va_end'", assignment_expression(p));
    expect(p, tok_t::rparen, "expected ')' after the list");

    return cast(keyword, make_void(), std::move(list));
}

// __builtin_va_copy ( dest , src ), va_copy, 7.16.1.2: src's record
// copied into dest's, a record's assignment, G18, its value discarded.
// Each operand is evaluated once.
expr_t
va_copy_expression(parser_t &p, const token_t &keyword)
{
    expect(p, tok_t::lparen, "expected '(' after " + quoted(keyword.text));
    const token_t &to_at = peek(p);
    expr_t to = list_pointer(p, to_at, "first argument to 'va_copy'",
                             assignment_expression(p));
    expect(p, tok_t::comma, "expected ',' after the list");
    const token_t &from_at = peek(p);
    expr_t from = list_pointer(p, from_at, "second argument to 'va_copy'",
                               assignment_expression(p));
    expect(p, tok_t::rparen, "expected ')' after the list");

    expr_t lhs = indirection(keyword, std::move(to));
    expr_t rhs = indirection(keyword, std::move(from));
    const type_t t = lhs.type;
    expr_t copy { t, assign_t { std::nullopt, t, box(std::move(lhs)),
                                box(std::move(rhs)) } };

    return cast(keyword, make_void(), std::move(copy));
}

// The builtins behind the macros of stdarg.h, 7.16.1, which clang's
// header defines va_start, va_arg, va_end and va_copy by in the
// dialect rcc preprocesses in: the expression of the one the keyword
// names, or nothing for any other name.
std::optional< expr_t >
va_builtin(parser_t &p, const token_t &keyword)
{
    if (keyword.text == "__builtin_va_start")
        return va_start_expression(p, keyword);

    if (keyword.text == "__builtin_va_arg")
        return va_arg_expression(p, keyword);

    if (keyword.text == "__builtin_va_end")
        return va_end_expression(p, keyword);

    if (keyword.text == "__builtin_va_copy")
        return va_copy_expression(p, keyword);

    return std::nullopt;
}

// 6.5.3.3p5 gives a call the result type of the function it calls, a
// named one or one through a pointer.  A call of a function whose result
// is of a not-yet type is not yet, its value used or not: the call
// makes the object it returns.
expr_t
postfix_expression(parser_t &p)
{
    expr_t e = primary_expression(p);

    for (;;) {
        const token_t &op = peek(p);

        switch (op.kind) {
        case tok_t::lparen: {
            advance(p);

            e = callee(op, std::move(e));
            const function_t &type = callee_type(e);

            const type_t t = *type.result;
            require_honoured(op, t);

            if (const record_t *r = record_in(t))
                not_yet(op, record_words(*r) + " returned by value");

            std::vector< expr_t > args =
                argument_expression_list(p, type, callee_words(e));
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
        case tok_t::arrow:
            advance(p);
            e = member_access(p, op, std::move(e));
            break;
        default: return e;
        }
    }
}

// An aarch64 va_list as an argument, is_list_by_reference(): an
// lvalue of the list's record, whose object c1 copies, passing the
// copy's address, as the convention has it.  An expression of another
// type is none, and a list that is no lvalue is a record's value, which
// is not yet.
expr_t
list_argument(parser_t &p, const token_t &start, const type_t &list)
{
    expr_t arg = assignment_expression(p);
    if (!(arg.type == list))
        fail(start, "incompatible type for a va_list argument");

    if (!is_lvalue(arg))
        refuse_record_value(start, arg.type);

    return arg;
}

// The argument ahead, the call's index-th: converted to its parameter's
// type as if by assignment, or, past the parameters of a variadic
// function, promoted by the default argument promotions if it is an
// integer, a pointer passing as it is, 6.5.3.3p6.  One past the
// parameters of a function that takes no more is an error, p2, which
// names the callee by words, callee_words()'s.  A record passes by value
// in no rule rcc has yet but the aarch64 va_list's; an argument of
// another type for a record parameter is the error of incompatible
// types, as a record for a scalar one is, p2.
expr_t
argument(parser_t &p,
         const function_t &type,
         std::size_t index,
         std::string_view words)
{
    const token_t &start = peek(p);
    const type_t *param =
        index < type.params.size() ? type.params[index].get() : nullptr;

    if (param && is_list_by_reference(p, *param))
        return list_argument(p, start, *param);

    expr_t arg = assignment_expression(p);

    if (const record_t *r = param ? record_in(*param) : nullptr) {
        require_value(start, arg);

        const expr_t value = decayed(std::move(arg));
        require_record_conversion(start, value.type, *param);

        not_yet(start, record_words(*r) + " passed by value");
    }

    if (param)
        return assignment_conversion(start, std::move(arg), *param);

    if (!type.variadic)
        fail(start, "too many arguments " + std::string(words));

    refuse_record_value(start, arg.type);
    arg = rvalue(start, std::move(arg));

    if (is< int_t >(arg.type))
        return promote(std::move(arg));

    return arg;
}

// The arguments of a call, argument()'s each, as many as the callee's
// parameters, p2, or more for a variadic one.
std::vector< expr_t >
argument_expression_list(parser_t &p,
                         const function_t &type,
                         std::string_view words)
{
    std::vector< expr_t > args;

    if (peek(p).kind != tok_t::rparen) {
        do
            args.push_back(argument(p, type, args.size(), words));
        while (accept(p, tok_t::comma));
    }

    if (args.size() < type.params.size())
        fail(peek(p), "too few arguments " + std::string(words));

    return args;
}

// A literal's encoding prefix as a message names it.
std::string
prefix_words(const token_t &t)
{
    return quoted(t.text.substr(0, t.text.find_first_of("'\"")));
}

// Adjacent string literals are one, 6.4.5p5, of the prefix of those of
// them that have one, which is one prefix, p2, or of none: "a" L"b" is
// L"ab".  Each literal's elements are read in that encoding, a
// literal's of no prefix among them, and such a literal's text is to be
// UTF-8 where the encoding is wide, as one of a prefix's is.  The array
// is one element longer than the elements, for the zero appended, p6,
// of the encoding's element type; its bytes are the elements', each at
// its width, little-endian.
expr_t
string_literal(parser_t &p)
{
    std::size_t count = 0;
    encoding_t encoding = encoding_t::none;
    const token_t *prefixed = nullptr;

    for (; peek(p, count).kind == tok_t::string; ++count) {
        const token_t &t = peek(p, count);
        const encoding_t own = encoding_of(t);
        if (own == encoding_t::none)
            continue;

        if (prefixed && own != encoding)
            fail(t, "string literal of prefix " + prefix_words(t) +
                        " concatenated with one of prefix " +
                        prefix_words(*prefixed));

        prefixed = &t;
        encoding = own;
    }

    const type_t element = element_type(p, encoding);
    const int width = int(size_of(element));
    const bool wide = width > 1;

    std::string bytes;
    for (std::size_t k = 0; k < count; ++k) {
        const token_t &t = advance(p);
        if (wide && encoding_of(t) == encoding_t::none && !is_utf8(t))
            fail(t, "invalid UTF-8 sequence in a string literal "
                    "concatenated with prefix " +
                        prefix_words(*prefixed));

        for (const std::uint32_t unit : string_units(t, encoding))
            for (int b = 0; b < width; ++b)
                bytes.push_back(static_cast< char >(unit >> (8 * b)));
    }

    const long elements = long(bytes.size()) / width;
    type_t type = make_array(element, elements + 1);

    return expr_t { std::move(type), string_t { std::move(bytes), width } };
}

// A character constant: of no prefix, an int, 6.4.4.5p11; of L, u or
// U, its one code unit, of wchar_t, char16_t or char32_t, p13 to p15.
expr_t
character_constant(const parser_t &p, const token_t &t)
{
    const encoding_t encoding = encoding_of(t);
    if (encoding == encoding_t::none)
        return expr_t { int_type(), literal_t { character_value(
                                        t, p.target.char_is_signed) } };

    return constant_of(element_type(p, encoding), wide_character_value(t));
}

// The member a designator of offsetof names in a record: the record
// laid out, 7.21p4, an incomplete one refused, 6.5.4.4p1's rule, and a
// not-yet one refused by its words.
const member_t &
designated_member(const token_t &at, const type_t &t, const token_t &name)
{
    if (!is< record_type_t >(t))
        fail(at, "offsetof of a member of a type that is not a struct or "
                 "a union");

    const record_t &r = *as< record_type_t >(t).record;
    if (!r.complete)
        fail(at, "offsetof of a member of incomplete type " + record_words(r));

    require_honoured(at, t);

    return named_member(r, name);
}

// The element of the array so far that a subscript of an offsetof
// designator names, by an integer constant expression: its offset is
// added to offset, and the element's type is the type so far.
void
designated_element(parser_t &p, type_t &t, long &offset)
{
    const token_t &open = advance(p);
    if (!is< array_t >(t))
        fail(open, "subscript in offsetof of a member that is not an array");

    const token_t &start = peek(p);
    const expr_t index = rvalue(start, expression(p));
    if (!is_integer_constant(index))
        fail(start,
             "subscript in offsetof is not an integer constant expression");

    expect(p, tok_t::rbracket, "expected ']'");

    t = *as< array_t >(t).element;
    offset += long(exact_value(index)) * size_of(t);
}

// __builtin_offsetof ( type-name , member-designator ), which the
// offsetof of the header defines, 7.21p4: the offset in bytes, from the
// start of an object of the type, of the subobject the designator
// names, an integer constant of type size_t.  The designator is a
// member's name, then any number of .name, a member of the record so
// far, and [constant], designated_element()'s, so that
// &(t.member-designator) on a static t is an address constant.  The
// keyword is the identifier ahead's.
expr_t
offsetof_expression(parser_t &p, const token_t &keyword)
{
    expect(p, tok_t::lparen, "expected '(' after " + quoted(keyword.text));
    type_t t = type_name(p);
    expect(p, tok_t::comma, "expected ',' after type name");

    long offset = 0;
    const token_t *at = &keyword;

    do {
        const token_t &name =
            expect(p, tok_t::identifier, "expected member name");
        const member_t &m = designated_member(*at, t, name);
        offset += m.offset;
        t = m.type;

        while (peek(p).kind == tok_t::lbracket)
            designated_element(p, t, offset);

        at = &peek(p);
    } while (accept(p, tok_t::dot));

    expect(p, tok_t::rparen, "expected ')' after member designator");

    return expr_t { size_type(), literal_t { offset } };
}

// __builtin_expect ( e , c ), which a program's likely() and
// unlikely() write: e, its value converted as if by assignment to
// long, as clang declares long __builtin_expect(long, long), and no
// lvalue.  It says only which value e is likely to have, so it writes
// nothing of its own.  c, checked as converted alike, is evaluated for
// its effect alone, after e, G13, and as clang has it at -O0 not at all
// where it has none; where it has one, e's value is held in a
// temporary across it: (t = e, c, t).
expr_t
expect_expression(parser_t &p, const token_t &keyword)
{
    expect(p, tok_t::lparen, "expected '(' after " + quoted(keyword.text));
    const token_t &e_at = peek(p);
    expr_t e =
        assignment_conversion(e_at, assignment_expression(p), long_type());
    expect(p, tok_t::comma, "expected ',' after the expression");
    const token_t &c_at = peek(p);
    expr_t c =
        assignment_conversion(c_at, assignment_expression(p), long_type());
    expect(p, tok_t::rparen, "expected ')' after the expected value");

    // the cast makes no lvalue of e where it is already a long; c's
    // value is discarded, so its conversion is checked and not made
    e = cast(keyword, long_type(), std::move(e));
    if (pure_copy(c))
        return e;

    if (is< cast_t >(c)) {
        expr_t unconverted = std::move(*as< cast_t >(c).arg);
        c = std::move(unconverted);
    }

    const symbol_t &t = temporary(p, "__expected", long_type());
    expr_t set = assignment(operator_at(keyword, tok_t::assign),
                            expr_t { t.type, name_t { &t } }, std::move(e));

    return comma(
        discarded(keyword, std::move(set)),
        comma(discarded(c_at, std::move(c)), expr_t { t.type, name_t { &t } }));
}

// A name must be declared, 6.5.2p2, as an object or a function: a
// typedef name names a type.  An enumeration constant is a constant,
// p2's footnote, of its type and its value, 6.4.4.4p2, 6.7.3.3p10: the
// literal, an integer constant thus.  The use of a symbol an attribute
// marked is refused here, with the name in hand, by the attribute's
// words, and so is a global's of a not-yet type, which an object
// declared elsewhere may have, and a constant's of one, an enum's an
// attribute marked, by the type's.  A use of a name of internal linkage
// is kept for the check of its definition at the end of the unit.  A
// generic selection is not yet.  Parentheses leave the expression as it
// is, an lvalue included, 6.5.2p6.  A floating constant is not yet: the
// lexer took its spelling, and its value is not computed.  A character
// constant is character_constant()'s, and adjacent string literals
// are one, string_literal()'s.
expr_t
primary_expression(parser_t &p)
{
    const token_t &t = peek(p);

    switch (t.kind) {
    case tok_t::identifier: {
        advance(p);

        if (t.text == "__builtin_offsetof")
            return offsetof_expression(p, t);

        if (t.text == "__builtin_expect")
            return expect_expression(p, t);

        if (std::optional< expr_t > e = va_builtin(p, t))
            return std::move(*e);

        const symbol_t *sym = lookup(p, t.text);
        if (!sym)
            fail(t, quoted(t.text) + " undeclared");

        if (sym->storage == storage_t::type_name)
            fail(t, "unexpected type name " + quoted(t.text) +
                        ": expected expression");

        if (!sym->marking.empty())
            not_yet(t, sym->marking);

        if (sym->storage == storage_t::global ||
            sym->storage == storage_t::enumerator)
            require_honoured(t, sym->type);

        if (sym->storage == storage_t::enumerator)
            return expr_t { sym->type, literal_t { sym->value } };

        if (sym->linkage == linkage_t::internal)
            p.internal_uses.push_back({ sym, &t });

        if (sym->by_reference)
            return indirection(t, expr_t { sym->type, name_t { sym } });

        return expr_t { sym->type, name_t { sym } };
    }
    case tok_t::integer: advance(p); return integer_constant(t);
    case tok_t::floating: not_yet(t, "floating constant");
    case tok_t::character: advance(p); return character_constant(p, t);
    case tok_t::string: return string_literal(p);
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
parse(const std::vector< token_t > &tokens,
      const target_t &target,
      std::vector< warning_t > &warnings)
{
    parser_t p { tokens, 0,           target, warnings,          { },
                 { },    make_void(), 0,      first_tag_identity };
    p.scopes.emplace_back();
    p.va_record = va_record(p);

    translation_unit(p);

    return std::move(p.unit);
}

} // namespace rcc
