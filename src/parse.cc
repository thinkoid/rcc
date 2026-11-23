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
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
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
// block, 6.2.4p6, and so through the nested block.  variable_arrays
// are the variable length arrays the scope declares so far, by their
// names' tokens, in order: each is in scope from its declarator to the
// scope's end, which a jump into is checked against.
struct scope_t
{
    std::unordered_map< std::string_view, symbol_t * > names;
    std::unordered_map< std::string_view, tag_t > tags;
    int registers = 0;
    std::vector< const token_t * > variable_arrays = { };
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
// names the declared symbol, the attribute's name, whether the symbol
// is a function, and whether weak makes it a weak one.
struct pending_alias_t
{
    const token_t *name;
    const token_t *attribute;
    bool function;
    bool weak;
};

// A not-yet type of an identity of its own, as made: the means that
// made it, in words, the type it was made from, and its identity.
struct made_type_t
{
    std::string means;
    type_t from;
    int identity;
};

// The marking attributes of a run of attributes, or of all a
// declaration writes: how many, the first alias and the first weak
// among them, and the name's token of each, in their order.
struct markings_t
{
    int count = 0;
    const token_t *alias = nullptr;
    const token_t *weak = nullptr;
    std::vector< const token_t * > names = { };
};

// A switch whose body the parse is in, 6.8.5.3: the promoted type of
// its controlling expression, which each case's constant is converted
// to, p5; its cases so far, as the tree takes them, and the token of
// each one's constant by its value, for the words of a duplicate, p3;
// and its default's number and keyword, once one is met.
// statement_expressions is the count of statement expressions around
// the switch, which a label of it inside one more is a jump into, and
// variable_arrays the count of variable length arrays in scope at the
// switch, which a label of it in the scope of one more is a jump into.
struct open_switch_t
{
    type_t type;
    int statement_expressions;
    std::size_t variable_arrays;
    std::vector< case_t > cases = { };
    std::unordered_map< long long, const token_t * > values = { };
    std::optional< int > default_ = { };
    const token_t *default_at = nullptr;
};

// A member access of a bit-field: the positions of the first token of
// its postfix expression and of the token past the member's name, the
// field's member, and whether the field fills its access unit.  A
// store, a compound assignment and a step of a field that does not fill
// its unit are the parser's trees, and & and sizeof of any field are
// errors, each found by the positions (shape.md chapter 5).
struct field_access_t
{
    std::size_t first;
    std::size_t end;
    member_t member;
    bool fills;
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
// jump_scopes are the variable length arrays in scope at each label and
// each goto of the function that stands in the scope of one, by its
// name's token, checked at the function's end and counted at each label
// for c1, label_arrays().
// switches are the switches around the parse, the innermost last, which
// a case or a default label belongs to, 6.8.5.3p4; case_labels is the
// number the function's next case or default label takes.
// statement_expressions is the count of GNU C's statement expressions
// around the parse; statement_expression_spans are the function's, each
// by its '(' and its ')', which a label inside one is checked against
// at the function's end.
// identities is the identity the next not-yet type of its own takes:
// an enum in its own list or marked by an attribute, an atomic type, a
// typedef through a marking attribute or an array size rcc cannot
// compute; and the next enumerated type's enumeration.  made are the
// not-yet types made from another type so far, identity_of()'s.
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
// are the declarations an alias attribute makes aliases.  used are the
// symbols of linkage an expression has named so far, which a marking
// attribute on a later declaration comes too late for.  field is the
// last access of a bit-field a member access made, field_access_t's,
// and field_store the positions of the first token of the last store,
// compound assignment or step of a bit-field that does not fill its
// unit and of the token past it, which stored_effect() reads.
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
    std::unordered_map< const token_t *, std::vector< const token_t * > >
        jump_scopes = { };
    std::vector< open_switch_t > switches = { };
    int case_labels = 0;
    int statement_expressions = 0;
    std::vector< std::pair< const token_t *, const token_t * > >
        statement_expression_spans = { };
    std::vector< internal_use_t > internal_uses = { };
    std::unordered_map< std::string_view, symbol_t * > block_entities = { };
    std::vector< tentative_t > incomplete_definitions = { };
    std::vector< pending_alias_t > aliases = { };
    const record_t *va_record = nullptr;
    bool variadic = false;
    std::size_t named = 0;
    const symbol_t *va_area = nullptr;
    std::optional< std::vector< const symbol_t * > > temporaries = { };
    std::unordered_set< const symbol_t * > used = { };
    std::vector< made_type_t > made = { };
    std::optional< field_access_t > field = { };
    std::optional< std::pair< std::size_t, std::size_t > > field_store = { };
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
// the marking attributes among the specifiers.  qualifiers are the const
// and the volatile among them, joined with a typedef name's, 6.7.4.1p6,
// which the type does not carry.
struct specifiers_t
{
    type_t type;
    const token_t *storage_class;
    bool tagged;
    const token_t *function_specifier;
    const token_t *inline_specifier;
    const token_t *marking;
    markings_t markings;
    qualifiers_t qualifiers;
};

// A parameter as written: its type, its name token when it has one,
// whether it was declared register, the name's token of the first
// marking attribute said of it, or null, the '*' that sizes the
// array it is adjusted from, or null, the first '*' that sizes an
// array in its other brackets, or null, and the size of the array it
// is adjusted from when that is no constant, which a definition
// evaluates on entry, or nothing.  qualifiers are the parameter's own,
// for its symbol, which the function's type leaves out, 6.7.7.4p14:
// those its declaration says of it, or for one adjusted from an array
// those in the array's brackets, the pointer's, p6.
struct parameter_t
{
    type_t type;
    const token_t *start;
    const token_t *name;
    bool in_register;
    const token_t *marking;
    const token_t *star;
    const token_t *inner_star;
    std::optional< expr_t > size;
    qualifiers_t qualifiers = { };
};

// A declarator: a name, absent from a parameter that leaves it out and
// from a type name, the type it gives the name, and the parameters as
// written when it declares a function, the parameter list that derives
// the name's type.  A pointer to a function has a parameter list and
// declares no function, so it has none.  scope is that list's scope,
// closed at its ')', with the parameters and the tags the list
// declares, which a definition opens again as its body's; empty when
// there are no params.  star is the '*' that sizes the array a
// parameter is adjusted from, 6.7.7.3p4, or null, and inner_star the
// first '*' that sizes an array in a parameter's other brackets, or
// null.  zero is the size of zero of a member's own array, read as
// left out, or null.  variable_size is the size as written of the
// variable length array the declarator's type is, for its declaration
// to evaluate, or of the array a parameter is adjusted from when that
// is no constant, for a definition to evaluate on entry; absent for
// any other type.  qualifiers are those said of what the name declares,
// at the top of its type, which the type does not carry: an array's
// element's, which are the array's, 6.7.4.1p10, and none for a function,
// whose result is unqualified, 6.7.7.4p4.  bracket_qualifiers are those
// in the brackets of the array a parameter is adjusted from, the
// pointer's, p6.
struct declarator_t
{
    const token_t *name;
    type_t type;
    qualifiers_t qualifiers;
    std::optional< std::vector< parameter_t > > params;
    scope_t scope;
    const token_t *star = nullptr;
    const token_t *zero = nullptr;
    const token_t *inner_star = nullptr;
    qualifiers_t bracket_qualifiers = { };
    std::optional< expr_t > variable_size = { };
};

// A type and the qualifiers at its top, which the type does not carry,
// as a type name gives them: a generic association compares both.
struct qualified_type_t
{
    type_t type;
    qualifiers_t qualifiers;
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

// A warning at the token, added to the driver's list (shape.md chapter
// 8); the parse goes on.
void
warn(parser_t &p, const token_t &at, std::string what)
{
    p.warnings.push_back({ at.where, std::move(what) });
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
    return make_int(32, true, int_kind_t::int_);
}

// C's boolean type, 6.2.5p2: to the machine an unsigned char, a byte,
// of a kind of its own, so that c1 reads and writes it as the byte it
// is (G21) and no generator sees a _Bool; the parser converts to it,
// converted_to_bool().
type_t
bool_type()
{
    return make_int(8, false, int_kind_t::bool_);
}

// Plain char is signed or unsigned as the target has it, and a type of
// its own, 6.2.5p20; long is the 64-bit signed type, and unsigned long
// the type of sizeof, 6.5.4.4p5.
type_t
char_type(const parser_t &p)
{
    return make_int(8, p.target.char_is_signed, int_kind_t::char_);
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
    case encoding_t::utf16:
        return make_int(16, false, int_kind_t::unsigned_short);
    case encoding_t::utf32: return make_int(32, false, int_kind_t::unsigned_);
    default:
        return make_int(32, p.target.wchar_is_signed,
                        p.target.wchar_is_signed ? int_kind_t::int_ :
                                                   int_kind_t::unsigned_);
    }
}

type_t
long_type()
{
    return make_int(64, true, int_kind_t::long_);
}

type_t
size_type()
{
    return make_int(64, false, int_kind_t::unsigned_long);
}

// The not-yet types that are one type wherever they are written: the
// real and the complex floating types, GNU C's __float128 and
// __int128, and clang's SVE types, each of a fixed identity.  A
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

constexpr int first_tag_identity = 13;

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
// converts to int, which holds all of its values, signed or not.  An
// enumerated type converts to its compatible integer type, as clang
// converts it, so one of unsigned int is unsigned int and one of long
// is long.
type_t
promoted(const type_t &t)
{
    const int_t &i = as< int_t >(t);
    if (i.width < 32)
        return int_type();

    if (i.enumeration != 0)
        return make_int(i.width, i.is_signed, i.kind);

    return t;
}

// The rank of an integer type, 6.3.1.1p1, by its kind: _Bool, below
// all, then the three char types, short, int, long and long long, each
// with its unsigned version.  The parser makes no integer type without
// a kind, and asking a type c1 made throws.
int
rank(const int_t &t)
{
    switch (t.kind) {
    case int_kind_t::bool_: return 0;
    case int_kind_t::char_:
    case int_kind_t::signed_char:
    case int_kind_t::unsigned_char: return 1;
    case int_kind_t::short_:
    case int_kind_t::unsigned_short: return 2;
    case int_kind_t::int_:
    case int_kind_t::unsigned_: return 3;
    case int_kind_t::long_:
    case int_kind_t::unsigned_long: return 4;
    case int_kind_t::long_long:
    case int_kind_t::unsigned_long_long: return 5;
    default: throw std::logic_error("rank: an integer type of no kind");
    }
}

// The unsigned type of a signed type's rank, the last conversion of
// 6.3.1.8p1's list.
int_kind_t
unsigned_kind(int_kind_t kind)
{
    switch (kind) {
    case int_kind_t::signed_char: return int_kind_t::unsigned_char;
    case int_kind_t::short_: return int_kind_t::unsigned_short;
    case int_kind_t::int_: return int_kind_t::unsigned_;
    case int_kind_t::long_: return int_kind_t::unsigned_long;
    case int_kind_t::long_long: return int_kind_t::unsigned_long_long;
    default: return kind;
    }
}

// The common type of the usual arithmetic conversions, 6.3.1.8p1, on
// the promoted operands, by their ranks: on LP64 long and long long are
// of one width and two ranks, so unsigned long and long long meet at
// unsigned long long.
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

// Whether a statement expression stands anywhere in e.
bool
holds_statement_expression(const expr_t &e)
{
    if (is< statement_expression_t >(e))
        return true;

    if (is< unary_t >(e))
        return holds_statement_expression(*as< unary_t >(e).arg);

    if (is< binary_t >(e))
        return holds_statement_expression(*as< binary_t >(e).lhs) ||
               holds_statement_expression(*as< binary_t >(e).rhs);

    if (is< assign_t >(e))
        return holds_statement_expression(*as< assign_t >(e).lhs) ||
               holds_statement_expression(*as< assign_t >(e).rhs);

    if (is< call_t >(e)) {
        const auto &c = as< call_t >(e);
        return holds_statement_expression(*c.callee) ||
               std::ranges::any_of(c.args, holds_statement_expression);
    }

    if (is< cast_t >(e))
        return holds_statement_expression(*as< cast_t >(e).arg);

    if (is< conditional_t >(e)) {
        const auto &c = as< conditional_t >(e);
        return holds_statement_expression(*c.cond) ||
               (c.yes && holds_statement_expression(*c.yes)) ||
               holds_statement_expression(*c.no);
    }

    return false;
}

// The refusal of e where a constant expression is wanted and e folded
// to none: the error words say, but for a statement expression in it,
// which clang folds to its value as GNU C's extension, and rcc does not
// fold yet.
[[noreturn]] void
refuse_unfolded(const token_t &at, const expr_t &e, std::string words)
{
    if (holds_statement_expression(e))
        not_yet(at, "statement expression in a constant expression");

    fail(at, std::move(words));
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

// The place a conversion the language implies enters the tree, every
// one but a decay.  The other casts are made by their construct:
// decayed() an array's or a function's decay, cast() the cast the
// source writes, unary_plus() the cast that makes +x a value,
// lvalue_at() the pointer casts of an access at an offset, and
// pure_node() the copy of a cast it rebuilds.  An expression already
// of the type in the machine's view passes through, taking the C type
// t remembers with no node, as long does to long long, so no inserted
// cast converts a type to itself; only cast() and unary_plus() make
// one, since each yields no lvalue.  A constant converted is the
// constant of the new type: the source wrote a number, not a
// conversion, so a char constant of 300 does not exist and the tree
// holds 44, as the conversion would have computed.
expr_t
convert_to(expr_t e, const type_t &t)
{
    if (e.type == t) {
        e.type = t;
        return e;
    }

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
    const bool is_signed = as< int_t >(promoted(e.type)).is_signed;
    const type_t t =
        make_int(64, is_signed,
                 is_signed ? int_kind_t::long_ : int_kind_t::unsigned_long);

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

// Whether a type has a size though rcc does not use its object yet: a
// real floating type, or an array of one, carries clang's size and
// alignment (shape.md chapter 3), which a record lays out as a member's
// and sizeof and _Alignof fold, and an access of it is refused where it
// stands.
bool
is_sized(const type_t &t)
{
    if (is< array_t >(t))
        return is_sized(*as< array_t >(t).element);

    return is< not_yet_t >(t) && as< not_yet_t >(t).size > 0;
}

// Whether a type is a variable length array, 6.7.7.3p4: an array whose
// size its declaration evaluates at run time into the type's length.
bool
is_variable_array(const type_t &t)
{
    return is< array_t >(t) && as< array_t >(t).length;
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

// The record a type is when it holds a flexible array member, as
// record_t's flexible says, or null.
const record_t *
flexible_record(const type_t &t)
{
    if (!is< record_type_t >(t) || !as< record_type_t >(t).record->flexible)
        return nullptr;

    return as< record_type_t >(t).record;
}

// The words of a record that holds a flexible array member, for the
// refusal of one where 6.7.3.2p3 keeps it out.
std::string
flexible_words(const record_t &r)
{
    return record_words(r) + ", which holds a flexible array member";
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

// The qualifiers of an lvalue's type, a function of the tree, since no
// type carries them at its top: a name's, its symbol's; *E's, those of
// the pointee of E's type, a member's access among them (lvalue_at()).
// A string literal has none, as any other expression: a value is of the
// unqualified type, 6.3.2.1p2.
qualifiers_t
qualifiers_of(const expr_t &e)
{
    if (is< name_t >(e))
        return as< name_t >(e).sym->qualifiers;

    if (is< unary_t >(e) && as< unary_t >(e).op == unary_op_t::deref)
        return as< pointer_t >(as< unary_t >(e).arg->type).qualifiers;

    return { };
}

// A const lvalue is no modifiable lvalue, 6.3.2.1p1: the name of an
// object declared const, said by its name, or any other lvalue of a
// const-qualified type, *p through a pointer to const and a member
// declared const or of a const record among them.
void
refuse_const_lvalue(const token_t &at, const expr_t &e)
{
    if (!qualifiers_of(e).const_)
        return;

    if (is< name_t >(e))
        fail(at, "cannot assign to " + quoted(as< name_t >(e).sym->name) +
                     ", declared const");

    fail(at, "cannot assign to a const-qualified lvalue");
}

// Whether a record holds a member declared const, its own or one of a
// record or an array of records it holds, at any depth, which makes an
// lvalue of it no modifiable one, 6.3.2.1p1.
bool
holds_const_member(const record_t &r)
{
    for (const member_t &m : r.members) {
        if (m.qualifiers.const_)
            return true;

        const record_t *inner = record_in(m.type);
        if (inner && holds_const_member(*inner))
            return true;
    }

    return false;
}

// 6.5.17.1p2, 6.5.3.5p1, 6.5.4.1p1: a modifiable lvalue of integer or
// pointer type.  An array is an lvalue but not a modifiable one,
// 6.3.2.1p1, and neither is a const lvalue.
void
require_modifiable_lvalue(const token_t &at, const expr_t &e)
{
    if (!is_lvalue(e) || !(is< int_t >(e.type) || is< pointer_t >(e.type)))
        fail(at, "expression is not assignable");

    refuse_const_lvalue(at, e);
}

// Array decay, 6.3.2.1p3: but as the operand of sizeof or of unary &,
// an array becomes a pointer to its first element, a cast c1 reads as
// that address, which points to the element qualified as the array is.
// A function designator decays the same way, p4: a function's name, or
// *E through a pointer to a function, becomes the pointer to the
// function, a cast c1 reads as the function's address, or as E's value
// with no instruction for the *.  A variable length array is *p, p the
// pointer its declaration initialized, and decays to p's value, a cast
// of p with no * (shape.md chapter 5): c1 sees no type without a size.
// Lvalue conversion, p2, is c1's load and needs no node.
expr_t
decayed(expr_t e)
{
    if (is< function_t >(e.type)) {
        type_t t = make_pointer(e.type);
        return expr_t { std::move(t), cast_t { box(std::move(e)) } };
    }

    if (!is< array_t >(e.type))
        return e;

    type_t t = make_pointer(*as< array_t >(e.type).element, qualifiers_of(e));

    if (is_variable_array(e.type))
        return expr_t { std::move(t),
                        cast_t { std::move(std::get< unary_t >(e.v).arg) } };

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
// complete object type, 6.5.7p2, p3: not a function, and neither an
// array of unknown size nor an incomplete record, which are
// incomplete, 6.2.5p27, 6.7.3.4p5, and have no size to step by.  void
// is taken as GNU C takes it, a byte, and the production warns of it
// (shape.md chapter 5).  The step is the pointee's size, so a not-yet
// pointee is not yet.
void
require_object_pointee(const token_t &op, const type_t &t)
{
    const type_t &pointee = pointee_of(t);

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

// Arithmetic on a pointer to void breaks the constraint of 6.5.7p2,
// p3, and GNU C takes it, a step of a byte: rcc takes it so, with a
// warning once the operation is built (shape.md chapter 5).  pointer
// is the type of the operand that points, or of either.
void
warn_void_arithmetic(parser_t &p, const token_t &op, const type_t &pointer)
{
    if (is_void_pointer(pointer))
        warn(p, op,
             "arithmetic on a pointer to void counts bytes, as GNU C allows");
}

//
// pointer conversions
//

// An integer constant expression of value 0, or one cast to void *,
// 6.3.2.3p3.  The parser folds every integer constant expression to a
// literal, and the cast of one to a pointer type to a literal of that
// type, so the two are an integer literal 0 and a void * literal 0.
// A literal 0 of another pointer type, (int *) 0, is a null pointer
// but not a null pointer constant, and so is one of a pointer to a
// qualified void, (const void *) 0: p3 names void * alone.
bool
is_null_pointer_constant(const expr_t &e)
{
    if (!is< literal_t >(e) || as< literal_t >(e).value != 0)
        return false;

    if (is_void_pointer(e.type))
        return as< pointer_t >(e.type).qualifiers == qualifiers_t { };

    return is< int_t >(e.type);
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

// Whether a type may be compatible with a not-yet type made from
// another, which rcc cannot tell: any type may be for a typedef
// through a marking attribute, and an array, or another such type, for
// a typedef of an array of a size rcc cannot compute, 6.2.7p1.
bool
may_be_compatible(const not_yet_t &made, const type_t &t)
{
    if (!made.element)
        return true;

    return is< array_t >(t) || (is< not_yet_t >(t) && as< not_yet_t >(t).made);
}

// The first not-yet type made from another, by a typedef through a
// marking attribute or an array size rcc cannot compute, that two
// pointers of two types point to where the other pointee may be
// compatible with it, may_be_compatible()'s; met walking the two types
// alike, through pointers, arrays and functions, as tag_twin() walks
// them; or null.
const not_yet_t *
made_pointee(const type_t &a, const type_t &b)
{
    if (is< pointer_t >(a) && is< pointer_t >(b)) {
        const type_t &x = pointee_of(a);
        const type_t &y = pointee_of(b);

        for (const auto &[made, other] : { std::pair { &x, &y }, { &y, &x } })
            if (!(x == y) && is< not_yet_t >(*made) &&
                as< not_yet_t >(*made).made &&
                may_be_compatible(as< not_yet_t >(*made), *other))
                return &as< not_yet_t >(*made);

        return made_pointee(x, y);
    }

    if (is< array_t >(a) && is< array_t >(b))
        return made_pointee(*as< array_t >(a).element,
                            *as< array_t >(b).element);

    if (!is< function_t >(a) || !is< function_t >(b))
        return nullptr;

    const auto &f = as< function_t >(a);
    const auto &g = as< function_t >(b);
    if (const not_yet_t *made = made_pointee(*f.result, *g.result))
        return made;

    for (std::size_t i = 0; i < std::min(f.params.size(), g.params.size()); ++i)
        if (const not_yet_t *made = made_pointee(*f.params[i], *g.params[i]))
            return made;

    return nullptr;
}

// The words of refuse_made_pointees() for a comparison, a conditional's
// arms and a redeclaration, which convert nothing.
constexpr std::string_view compatibility_words =
    "compatibility of a pointer to a type of ";

// rcc does not know what a typedef through a marking attribute or an
// array size it cannot compute makes of the type, so two types that
// differ by a pointer to one and a pointer to another type that may be
// compatible with it, made_pointee()'s, are not yet, by the construct's
// words and the typedef's: a conversion between them, their comparison,
// a conditional's arms and a redeclaration.  Any other mismatch is the
// caller's error.
void
refuse_made_pointees(const token_t &at,
                     const type_t &a,
                     const type_t &b,
                     std::string_view construct)
{
    if (const not_yet_t *made = made_pointee(a, b))
        not_yet(at, std::string(construct) + made->what);
}

// The words of the marking attribute on a parameter of one of two
// functions met walking two types alike, through pointers, arrays and
// functions, as tag_twin() walks them, where the other's marks differ,
// or null.  Equality does not read the mark, so the two types may be
// equal.
const std::string *
marked_twin(const type_t &a, const type_t &b)
{
    if (is< pointer_t >(a) && is< pointer_t >(b))
        return marked_twin(pointee_of(a), pointee_of(b));

    if (is< array_t >(a) && is< array_t >(b))
        return marked_twin(*as< array_t >(a).element,
                           *as< array_t >(b).element);

    if (!is< function_t >(a) || !is< function_t >(b))
        return nullptr;

    const auto &f = as< function_t >(a);
    const auto &g = as< function_t >(b);
    if (f.marking != g.marking)
        return f.marking.empty() ? &g.marking : &f.marking;

    if (const std::string *words = marked_twin(*f.result, *g.result))
        return words;

    for (std::size_t i = 0; i < std::min(f.params.size(), g.params.size()); ++i)
        if (const std::string *words = marked_twin(*f.params[i], *g.params[i]))
            return words;

    return nullptr;
}

// A function type carries the words of a marking attribute on its
// parameter, and a call through a pointer to it is refused by them.  A
// pointer converted to a pointer to a function of other marks, or met
// by one in a conditional's arms, would lose them, so the two are not
// yet, by the attribute's words, marked_twin()'s.
void
refuse_marked_twins(const token_t &at, const type_t &a, const type_t &b)
{
    if (const std::string *words = marked_twin(a, b))
        not_yet(at, *words);
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

// A pointer to void and a pointer to a function, in either order, which
// 6.5.17.2p1 converts only through a cast, and GNU C as if by
// assignment too.
bool
void_and_function_pointer(const type_t &a, const type_t &b)
{
    return (is_void_pointer(a) && is_function_pointer(b)) ||
           (is_function_pointer(a) && is_void_pointer(b));
}

// Whether a type is _Bool.
bool
is_bool(const type_t &t)
{
    return is< int_t >(t) && as< int_t >(t).kind == int_kind_t::bool_;
}

// Whether an expression's value is 0 or 1 already, as C gives the
// result of a comparison, 6.5.9p7, 6.5.10p4, of !, 6.5.4.3p5, and of &&
// and ||, 6.5.14p3, 6.5.15p3.
bool
is_truth_value(const expr_t &e)
{
    if (is< unary_t >(e))
        return as< unary_t >(e).op == unary_op_t::log_not;

    if (!is< binary_t >(e))
        return false;

    switch (as< binary_t >(e).op) {
    case binary_op_t::lt:
    case binary_op_t::gt:
    case binary_op_t::le:
    case binary_op_t::ge:
    case binary_op_t::eq:
    case binary_op_t::ne:
    case binary_op_t::log_and:
    case binary_op_t::log_or: return true;
    default: return false;
    }
}

// E != 0, an int, as the source would write it: an integer promoted,
// as the comparison's usual arithmetic conversions promote it,
// 6.5.10p5, and a pointer against its null pointer, p6.
expr_t
nonzero(expr_t e)
{
    if (is< int_t >(e.type))
        e = promote(std::move(e));

    expr_t zero { e.type, literal_t { 0 } };

    return expr_t { int_type(), binary_t { binary_op_t::ne, box(std::move(e)),
                                           box(std::move(zero)) } };
}

// The conversion of a scalar to _Bool, 6.3.1.2p1: 0 for a zero or a
// null pointer, 1 for any other value.  It is the parser's tree,
// (_Bool) (E != 0): a comparison, and the narrowing of its 0 or 1 to
// the byte, which c1 writes as any other, so no generator sees a _Bool
// (shape.md chapter 3).  A constant converts to its constant; a value
// that is 0 or 1 already is narrowed alone, is_truth_value()'s; and a
// _Bool is no conversion, its byte taken as it is (G21).
expr_t
converted_to_bool(expr_t e)
{
    const type_t b = bool_type();

    if (is< literal_t >(e))
        return expr_t { b, literal_t { as< literal_t >(e).value != 0 } };

    if (is_bool(e.type)) {
        e.type = b;
        return e;
    }

    if (!is_truth_value(e))
        e = nonzero(std::move(e));

    return expr_t { b, cast_t { box(std::move(e)) } };
}

// The words of qualifiers, 'const', 'volatile', or both joined.
std::string
qualifier_words(qualifiers_t q)
{
    if (q.const_ && q.volatile_)
        return "'const' and 'volatile'";

    return q.const_ ? "'const'" : "'volatile'";
}

// A pointer converted as if by assignment keeps each qualifier of its
// pointee, 6.5.17.2p1: one that would lose a const or a volatile, from
// to to, breaks the constraint and is converted with a warning, the
// diagnostic 5.1.1.3 asks for, as the cast would convert it.  A system
// header's text is read as it is, and draws none.
void
warn_discarded(parser_t &p,
               const token_t &at,
               qualifiers_t from,
               qualifiers_t to)
{
    const qualifiers_t lost { from.const_ && !to.const_,
                              from.volatile_ && !to.volatile_ };
    if (lost == qualifiers_t { } || at.where.system_header)
        return;

    warn(p, at,
         "conversion discards the " + qualifier_words(lost) +
             " of the pointed-to type");
}

// The conversion as if by assignment, of a value to the type of an
// object: the right operand of =, an initializer, 6.7.11p12, an
// argument, 6.5.3.3p2, a value returned, 6.8.7.5p3.  The constraints
// are those of simple assignment, 6.5.17.2p1: integers convert to each
// other, and a scalar to _Bool, a pointer among them, by
// converted_to_bool(); a pointer takes a pointer to a qualified or
// unqualified version of a compatible type, so a long * takes no
// long long *, an enum a * no enum b * of one integer type and a
// const char ** no char **, a pointer to void and an object pointer
// convert either way, and a null pointer constant converts to any
// pointer, one to a function included.  A pointer whose pointee would
// lose a qualifier converts with a warning, warn_discarded()'s.  Any
// other integer and pointer meet only through a cast, 6.5.5p3; a record
// and a scalar never meet.  A pointer to a function and a pointer to void meet only
// through a cast too, and GNU C converts them either way: rcc does so,
// with a warning, the conversion the cast would make (shape.md chapter
// 5).  A value converted to a not-yet type, as an argument meets a
// parameter of one, is not yet, and so is a record converted to its
// own type, whose object c1 does not write yet; the callers that may
// meet one refuse it first in their own words.
expr_t
assignment_conversion(parser_t &p, const token_t &at, expr_t e, const type_t &t)
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

    if (!to_pointer && from_pointer && !is_bool(t))
        fail(at, "incompatible pointer to integer conversion");

    if (to_pointer && from_pointer &&
        !compatible(pointee_of(t), pointee_of(e.type)) &&
        !void_and_object_pointer(t, e.type) &&
        !void_and_function_pointer(t, e.type) && !is_null_pointer_constant(e)) {
        refuse_tag_twins(at, t, e.type);
        refuse_made_pointees(at, t, e.type,
                             "conversion of a pointer to a type of ");
        fail(at, "incompatible pointer types");
    }

    if (to_pointer && from_pointer)
        refuse_marked_twins(at, t, e.type);

    if (to_pointer && from_pointer && !is_null_pointer_constant(e))
        warn_discarded(p, at, as< pointer_t >(e.type).qualifiers,
                       as< pointer_t >(t).qualifiers);

    if (void_and_function_pointer(t, e.type) && !is_null_pointer_constant(e))
        warn(p, at,
             "conversion between a pointer to void and a pointer to a "
             "function, as GNU C allows");

    if (is_bool(t))
        return converted_to_bool(std::move(e));

    return convert_to(std::move(e), t);
}

// Two values, at least one a pointer, brought to one pointer type, as
// == and != and the arms of ?: take them, 6.5.10p2, p6, 6.5.16p3, p7:
// pointers to compatible types meet at the pointer to their composite
// type, a null pointer constant goes to the other operand's type, and
// a pointer to void and an object pointer meet at void *.  The null
// pointer constant comes first, as p7's "if one operand is a null
// pointer constant, the result has the type of the other operand" does
// before its pointer to void, so c ? (void *) 0 : p is an int * for an
// int *p, and a (void *) 0 met by a pointer to a function, which meets
// no other void *, goes to the function pointer's type.  Two pointers
// of no null pointer constant meet at a pointee qualified with the
// qualifiers of both, p7, so c ? cp : p is a const int * for a
// const int *cp.  False when none of the three applies.
bool
common_pointer(expr_t &lhs, expr_t &rhs)
{
    const bool lp = is< pointer_t >(lhs.type);
    const bool rp = is< pointer_t >(rhs.type);

    type_t t = lp ? lhs.type : rhs.type;

    if (lp && rp) {
        const qualifiers_t both = as< pointer_t >(lhs.type).qualifiers |
                                  as< pointer_t >(rhs.type).qualifiers;

        if (is_null_pointer_constant(lhs))
            t = rhs.type;
        else if (is_null_pointer_constant(rhs))
            t = lhs.type;
        else if (compatible(pointee_of(lhs.type), pointee_of(rhs.type)))
            t = make_pointer(
                composite(pointee_of(lhs.type), pointee_of(rhs.type)), both);
        else if (void_and_object_pointer(lhs.type, rhs.type))
            t = make_pointer(make_void(), both);
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
void drop_identities(stmt_t &);

// drop_identities() of each operand of a node, and of each statement
// of a statement expression's block.
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
        if (c->yes)
            drop_identities(*c->yes);
        drop_identities(*c->no);
    }

    if (auto *x = std::get_if< statement_expression_t >(&e.v)) {
        for (stmt_t &item : x->body)
            drop_identities(item);
        if (x->value)
            drop_identities(*x->value);
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
// the pointer's type, and two pointers to compatible complete object
// types, qualified or not, subtract to long, ptrdiff_t on LP64.  Both
// pointees are complete, since an array of unknown size is compatible
// with one of a size.
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

    if (rp && !compatible(pointee_of(lhs.type), pointee_of(rhs.type))) {
        refuse_tag_twins(op, lhs.type, rhs.type);
        fail(op, "subtraction of pointers to different types");
    }

    require_object_pointee(op, lhs.type);

    if (rp) {
        require_object_pointee(op, rhs.type);
        return make_binary(binary_op_t::sub, long_type(), std::move(lhs),
                           std::move(rhs));
    }

    const type_t t = lhs.type;
    rhs = widened_index(std::move(rhs));

    return make_binary(binary_op_t::sub, t, std::move(lhs), std::move(rhs));
}

// < > <= >= == !=: the result an int (6.5.9p7, 6.5.10p4), integer
// operands in their common type (6.5.9p4, 6.5.10p5).  Pointers compare
// as addresses (G7): a relational operator takes two pointers to
// compatible object types, qualified or not, 6.5.9p2, so not two
// pointers to a function, and == and != what common_pointer() brings
// to one, 6.5.10p2.  A pointer and any other integer meet only through
// a cast.
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
    else if (equality && !common_pointer(lhs, rhs)) {
        refuse_made_pointees(op, lhs.type, rhs.type, compatibility_words);
        fail(op, lp && rp ? "comparison of distinct pointer types" :
                            "comparison between pointer and integer");
    }
    else if (!equality && !(lp && rp))
        fail(op, "comparison between pointer and integer");
    else if (!equality &&
             !compatible(pointee_of(lhs.type), pointee_of(rhs.type))) {
        refuse_made_pointees(op, lhs.type, rhs.type, compatibility_words);
        fail(op, "comparison of distinct pointer types");
    }
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

std::optional< expr_t > pure_copy(const expr_t &);
const symbol_t &temporary(parser_t &, std::string_view, const type_t &);

// The operation of E1 op= E2 on its operands, as op's own production
// builds it: a shift's, whose operands are promoted apart, 6.5.8p3, or
// the usual arithmetic conversions' of any other.
expr_t
compound_operation(const token_t &op, expr_t lhs, expr_t rhs)
{
    const binary_op_t b = binary_operator(op.kind);
    if (b == binary_op_t::shl || b == binary_op_t::shr)
        return shift(op, std::move(lhs), std::move(rhs));

    return arithmetic(op, std::move(lhs), std::move(rhs));
}

// The copy that reads a _Bool lvalue that a rewrite of op also stores
// into: the lvalue is evaluated once, 6.5.17.3p4, so one with a side
// effect, *p++ in *p++ |= x, has no second copy, and is not yet.
expr_t
bool_read(const token_t &op, const expr_t &lvalue)
{
    std::optional< expr_t > copy = pure_copy(lvalue);
    if (!copy)
        not_yet(op, quoted(op.text) +
                        " on a '_Bool' whose lvalue has a side effect");

    return std::move(*copy);
}

// The store of value into a _Bool lvalue, converted_to_bool()'s 0 or 1.
expr_t
bool_store(expr_t lvalue, expr_t value)
{
    const type_t t = lvalue.type;

    return expr_t { t, assign_t { std::nullopt, t, box(std::move(lvalue)),
                                  box(converted_to_bool(std::move(value))) } };
}

// E1 op= E2 on a _Bool E1, built as C defines it, E1 = (_Bool) (E1 op
// (E2)), 6.5.17.3p4, so that the byte stored is 0 or 1 (G21): op is
// its production's on E1 promoted, an identity of op among its rules,
// as for any E1 op E2, and the conversion converted_to_bool()'s.
expr_t
bool_compound(const token_t &op, expr_t lhs, expr_t rhs)
{
    expr_t value = compound_operation(op, bool_read(op, lhs), std::move(rhs));

    return bool_store(std::move(lhs), std::move(value));
}

// The compound assignment a step on a _Bool is, ++E E += 1 and --E
// E -= 1, 6.5.4.1p2, p3, at the step's place and of its spelling, so
// that a diagnostic names the step.
token_t
step_assignment(const token_t &op)
{
    token_t compound = op;
    compound.kind =
        op.kind == tok_t::inc ? tok_t::add_assign : tok_t::sub_assign;

    return compound;
}

// E++ and E-- on a _Bool E: the value is E's before the store,
// 6.5.3.5p2, held in a temporary, (t = E, E = (_Bool) (t +- 1), t), so
// that E is read once, and stored as the prefix step would store it.
expr_t
bool_postfix_step(parser_t &p, const token_t &op, expr_t arg)
{
    expr_t read = bool_read(op, arg);

    const symbol_t &before = temporary(p, "__bool_before", arg.type);
    const auto held = [&before] {
        return expr_t { before.type, name_t { &before } };
    };
    expr_t hold { before.type, assign_t { std::nullopt, before.type,
                                          box(held()), box(std::move(read)) } };

    expr_t one { int_type(), literal_t { 1 } };
    expr_t store =
        bool_store(std::move(arg), compound_operation(step_assignment(op),
                                                      held(), std::move(one)));

    return comma(
        discarded(op, comma(discarded(op, std::move(hold)), std::move(store))),
        held());
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
// pointer's type with the right operand widened as for p + n.  A _Bool
// is bool_compound()'s, so that c1 meets no compound assignment of one.
// The left operand comes checked as a modifiable lvalue.
expr_t
assignment(parser_t &p, const token_t &op, expr_t lhs, expr_t rhs)
{
    const type_t t = lhs.type;

    if (op.kind == tok_t::assign) {
        rhs = assignment_conversion(p, op, std::move(rhs), t);

        return expr_t { t, assign_t { std::nullopt, t, box(std::move(lhs)),
                                      box(std::move(rhs)) } };
    }

    rhs = rvalue(op, std::move(rhs));
    require_integer(op, rhs);

    if (is_bool(t))
        return bool_compound(op, std::move(lhs), std::move(rhs));

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
// one common_pointer() finds, p7, which is returned.  A pointer and an
// integer other than a null pointer constant do not meet, p3.  Two void
// arms make a void conditional, p3, p6, which stands where a value is
// dropped, as a call of a void function does, and c1 generates its
// arms for their effect; one void arm and one of a value meet in none
// of p3's pairs.  Two arms of one record are a record's value, not
// yet; a record and any other arm meet in none of them either.
type_t
joined_arms(const token_t &op, expr_t &yes, expr_t &no)
{
    const bool void_yes = is< void_t >(yes.type);
    const bool void_no = is< void_t >(no.type);

    if (void_yes && void_no)
        return make_void();

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

    if (pointers) {
        refuse_tag_twins(op, yes.type, no.type);
        refuse_marked_twins(op, yes.type, no.type);
    }

    if (!is< pointer_t >(yes.type) && !is< pointer_t >(no.type))
        arithmetic_conversions(yes, no);
    else if (!common_pointer(yes, no)) {
        refuse_made_pointees(op, yes.type, no.type, compatibility_words);
        fail(op, pointers ?
                     "pointer type mismatch in conditional expression" :
                     "pointer/integer type mismatch in conditional expression");
    }

    return yes.type;
}

// The conditional of two arms, joined_arms() at its type.  On a
// constant condition and two constant arms, the node is the arm the
// condition selects, 6.5.16p5.  The condition comes as an rvalue.
expr_t
conditional(const token_t &op, expr_t cond, expr_t yes, expr_t no)
{
    const type_t t = joined_arms(op, yes, no);

    if (is_integer_constant(cond) && is_integer_constant(yes) &&
        is_integer_constant(no))
        return exact_value(cond) != 0 ? std::move(yes) : std::move(no);

    return expr_t { t,
                    conditional_t { box(std::move(cond)), box(std::move(yes)),
                                    box(std::move(no)) } };
}

// GNU C's a ?: b, which is a ? a : b with a evaluated once: the
// condition is the first arm as well, so joined_arms() converts it to
// the type of the whole as it would the first arm, and the node has no
// first arm.  c1 tests the converted value and takes it as the first
// arm's.  It is zero where a is: an integer converted keeps 0 and
// takes no other value to 0, 6.3.1.3p1, p2; a null pointer constant
// and a null pointer convert to a null pointer, 6.3.2.3p3, p4, and no
// conversion of one pointer to another writes a bit.  On a constant
// condition and a constant second arm, the node is the arm the
// condition selects, 6.5.16p5.  The condition comes as an rvalue.
expr_t
omitted_middle(const token_t &op, expr_t cond, expr_t no)
{
    const type_t t = joined_arms(op, cond, no);

    if (is_integer_constant(cond) && is_integer_constant(no))
        return exact_value(cond) != 0 ? std::move(cond) : std::move(no);

    return expr_t { t, conditional_t { box(std::move(cond)), nullptr,
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
// discarded()'s.  A cast to _Bool is converted_to_bool()'s, under a
// cast of its own where that leaves an lvalue, a _Bool's.
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

    if (is_bool(t))
        arg = converted_to_bool(std::move(arg));

    if (is_bool(t) && !is_lvalue(arg))
        return arg;

    if (converts_to_constant(arg, t))
        return converted_constant(arg, t);

    return expr_t { t, cast_t { box(std::move(arg)) } };
}

// Prefix ++ and -- have the type of their lvalue operand, 6.5.4.1.  On
// a _Bool each is the compound assignment it is, p2, p3,
// bool_compound()'s.
expr_t
prefix_step(const token_t &op, expr_t arg)
{
    require_steppable(op, arg);

    if (is_bool(arg.type)) {
        expr_t one { int_type(), literal_t { 1 } };
        return bool_compound(step_assignment(op), std::move(arg),
                             std::move(one));
    }

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
// both operators were omitted.  The pointer points to the lvalue's type
// qualified as the lvalue is.  The address of a variable length array
// is a pointer to one, which is not yet; the array's lvalue is its
// name, which reads as *symbol (shape.md chapter 5).
expr_t
address_of(const token_t &op, expr_t arg)
{
    if (is< function_t >(arg.type))
        return decayed(std::move(arg));

    if (!is_lvalue(arg))
        fail(op, "cannot take the address of an rvalue");

    if (is_variable_array(arg.type)) {
        const symbol_t &array = *as< name_t >(*as< unary_t >(arg).arg).sym;
        not_yet(op, "address of variable length array " + quoted(array.name));
    }

    // 6.5.4.2p1: not an object declared register, whether or not it
    // got a register: 6.7.2p12's footnote says that addressable
    // storage does not make the address computable
    if (is< name_t >(arg) && as< name_t >(arg).sym->declared_register)
        fail(op, "cannot take the address of " +
                     quoted(as< name_t >(arg).sym->name) +
                     ", declared register");

    type_t t = make_pointer(arg.type, qualifiers_of(arg));

    return expr_t { std::move(t),
                    unary_t { unary_op_t::addr, box(std::move(arg)) } };
}

// The lvalue of type t at offset bytes from what pointer points to,
// *(T *) ((char *) pointer + offset), built of the tree's own nodes, as
// the Seventh Edition compiler's first pass wrote a->b as *(a + offset);
// at offset 0 it is *(T *) pointer, with no sum.  The tree of a member's
// access and of a local list's subobject, which c1 reads as one offset
// from the outermost pointer.  The lvalue is qualified as qualifiers
// say: the T * points to them, and a pointer already a T * in the
// machine's view takes them with no cast.
expr_t
lvalue_at(const parser_t &p,
          expr_t pointer,
          long offset,
          const type_t &t,
          qualifiers_t qualifiers)
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

    const type_t to = make_pointer(t, qualifiers);
    if (!(pointer.type == to))
        pointer = expr_t { to, cast_t { box(std::move(pointer)) } };
    else
        pointer.type = to;

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
// not know yet, but for one is_sized() says has its size.  A record's
// size is its layout's, its padding included, p4.  The name is the operator's, with the underscore a name takes
// when it is a keyword of C++.  A variable length array's size is no
// constant: it is the value of the type's length, which the declaration
// evaluated, p2, in a cast, which makes no lvalue of it.  The operand,
// which p2 evaluates, is the array's name, whose evaluation has no
// effect, so nothing else is evaluated.
expr_t
sizeof_(const token_t &op, const type_t &t)
{
    if (is_variable_array(t)) {
        const symbol_t &length = *as< array_t >(t).length;
        expr_t size { length.type, name_t { &length } };

        return expr_t { size_type(), cast_t { box(std::move(size)) } };
    }

    if (is< void_t >(t))
        fail(op, "invalid application of 'sizeof' to a void type");

    if (is< function_t >(t))
        fail(op, "invalid application of 'sizeof' to a function type");

    if (is< array_t >(t) && as< array_t >(t).count < 0)
        fail(op, "invalid application of 'sizeof' to an array of unknown size");

    if (is< record_type_t >(t) && !as< record_type_t >(t).record->complete)
        fail(op, "invalid application of 'sizeof' to incomplete type " +
                     record_words(*as< record_type_t >(t).record));

    if (!is_sized(t))
        require_honoured(op, t);

    return expr_t { size_type(), literal_t { size_of(t) } };
}

// _Alignof ( type-name ) folds to the alignment of the type, a size_t
// constant, 6.5.4.4p3, p5; of an array, its element's.  A function and
// an incomplete type have none, p1: void, an array of unknown size and
// an incomplete record.  A not-yet type has one rcc does not know yet,
// but for one is_sized() says has its alignment.
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

    if (!is_sized(t))
        require_honoured(op, t);

    return expr_t { size_type(), literal_t { align_of(t) } };
}

// Postfix ++ and -- have the type of their lvalue operand, 6.5.3.5; on
// a _Bool each is bool_postfix_step()'s.
expr_t
postfix_step(parser_t &p, const token_t &op, expr_t arg)
{
    require_steppable(op, arg);

    if (is_bool(arg.type))
        return bool_postfix_step(p, op, std::move(arg));

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

// The qualifiers said of the type a typedef name names, its symbol's,
// which its use joins to those written beside it, 6.7.4.1p6: none for
// a builtin type's name.
qualifiers_t
named_qualifiers(const parser_t &p, const token_t &t)
{
    const symbol_t *sym = lookup(p, t.text);
    if (sym && sym->storage == storage_t::type_name)
        return sym->qualifiers;

    return { };
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
symbol_t *
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
// octal, hexadecimal or binary one may go unsigned.  long long is as
// wide as long on LP64, so the lists end at the 64-bit types: long and
// unsigned long, or after ll long long and unsigned long long.
std::optional< type_t >
integer_constant_type(std::uint64_t value, bool decimal, suffix_t suffix)
{
    const bool may_be_signed = !suffix.is_unsigned;
    const bool may_be_unsigned = suffix.is_unsigned || !decimal;
    const bool long_long = suffix.longs == 2;

    if (may_be_signed && suffix.longs == 0 &&
        value <= std::uint64_t(std::numeric_limits< std::int32_t >::max()))
        return make_int(32, true, int_kind_t::int_);

    if (may_be_unsigned && suffix.longs == 0 &&
        value <= std::numeric_limits< std::uint32_t >::max())
        return make_int(32, false, int_kind_t::unsigned_);

    if (may_be_signed &&
        value <= std::uint64_t(std::numeric_limits< std::int64_t >::max()))
        return make_int(64, true,
                        long_long ? int_kind_t::long_long : int_kind_t::long_);

    if (may_be_unsigned)
        return make_int(64, false,
                        long_long ? int_kind_t::unsigned_long_long :
                                    int_kind_t::unsigned_long);

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

// always_inline asks that each call of the function be inlined, and
// gcc and clang refuse a call they cannot inline.  rcc inlines none
// and writes the call, as a compiler that does not inline does, so the
// attribute is dropped, with a warning that says so in the program's
// own text; a system header's is read as it is (shape.md chapter 5).
bool
is_always_inline(std::string_view attribute)
{
    return attribute == "always_inline";
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

    if (is_always_inline(attribute_name(name))) {
        if (!name.where.system_header)
            warn(p, name,
                 attribute_words(name) + " dropped: rcc inlines nothing");

        return nullptr;
    }

    return is_dropped(attribute_name(name)) ? nullptr : &name;
}

// A marking attribute counted among a declaration's.
void
count_marking(markings_t &markings, const token_t &name)
{
    ++markings.count;
    if (!markings.alias && attribute_name(name) == "alias")
        markings.alias = &name;

    if (!markings.weak && attribute_name(name) == "weak")
        markings.weak = &name;

    markings.names.push_back(&name);
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

// A string of the __asm__ the keyword begins, adjacent literals joined
// as in an expression, 6.4.5p5: an assembler name, the template of an
// __asm__ statement, or one of its clobbers.
std::string
asm_string(parser_t &p, const token_t &keyword)
{
    if (peek(p).kind != tok_t::string)
        fail(peek(p), "expected string literal in " + quoted(keyword.text));

    std::string bytes;
    while (peek(p).kind == tok_t::string)
        bytes += string_bytes(advance(p));

    return bytes;
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
    const std::string name = asm_string(p, keyword);
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
// declaration is refused.  GNU C lets a later declaration add the
// attribute to the uses before it too, and to a definition before it,
// which rcc has taken unmarked: a function's body, written strong, and
// an object's entry, initialized or tentative, which c1 writes strong
// after the parse.  So the attribute is refused where it is written
// then.
bool defines(const parser_t &, const symbol_t *);

void
record_tail(const parser_t &p, symbol_t &sym, const declarator_tail_t &tail)
{
    if (tail.marking && (p.used.contains(&sym) || defines(p, &sym)))
        not_yet(*tail.marking, attribute_words(*tail.marking));

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
void function_definition(parser_t &, const specifiers_t &, declarator_t &);
void header_function_definition(parser_t &,
                                const specifiers_t &,
                                const declarator_t &);
specifiers_t declaration_specifiers(parser_t &);
type_t struct_or_union_specifier(parser_t &);
type_t enum_specifier(parser_t &);
type_t atomic_type_specifier(parser_t &);
declarator_t declarator(parser_t &,
                        const type_t &,
                        qualifiers_t,
                        naming_t = naming_t::required,
                        bool typedef_ = false);
declarator_t parenthesized_declarator(
    parser_t &, const type_t &, qualifiers_t, naming_t, bool typedef_);
declarator_t declarator_suffix(parser_t &,
                               const token_t *,
                               const type_t &,
                               qualifiers_t,
                               bool typedef_,
                               bool parameter,
                               bool member,
                               bool in_parameter);
type_t pointer(parser_t &, type_t, qualifiers_t &);
type_t array_declarator(parser_t &,
                        const type_t &,
                        bool typedef_,
                        bool parameter,
                        bool member,
                        bool in_parameter,
                        const token_t *&star,
                        const token_t *&zero,
                        const token_t *&inner_star,
                        qualifiers_t &bracket_qualifiers,
                        std::optional< expr_t > &variable_size);
bool parameter_type_list(parser_t &, std::vector< parameter_t > &);
parameter_t parameter_declaration(parser_t &);
type_t type_name(parser_t &);
std::vector< stmt_t > declaration(parser_t &);
std::vector< stmt_t > init_declarator_list(parser_t &,
                                           const specifiers_t &,
                                           bool objects_only = false);
void init_declarator(parser_t &,
                     const specifiers_t &,
                     declarator_t &,
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
void keep_named_functions(parser_t &);

void
translation_unit(parser_t &p)
{
    while (peek(p).kind != tok_t::eof)
        external_declaration(p);

    require_completed_definitions(p);
    complete_tentative_arrays(p);
    require_internal_definitions(p);
    resolve_aliases(p);
    keep_named_functions(p);
}

// An earlier declaration of the name, met again: the same kind of
// entity, a function or an object of linkage of a compatible type
// (6.7.1p5), and a typedef of the same type, since a typedef name may
// be declared again only as the type it already denotes (p4).  The
// entity takes the composite type from the later declaration on,
// 6.2.7p5: an object declared as an array of unknown size and as one
// of known size is the array of known size whichever came first, and
// so is a pointer to one in a parameter, as in int f(int (*)[]); int
// f(int (*)[3]);, and an enumerated type met by its integer type is
// the integer type.  qualifiers are those the declaration says of the
// object or of the typedef's type, which are the same as the earlier
// one's in either case, 6.7.4.1p11.
symbol_t &
redeclared(const token_t &name,
           symbol_t &earlier,
           const type_t &t,
           qualifiers_t qualifiers,
           storage_t storage)
{
    if (earlier.storage != storage)
        fail(name, "redefinition of " + quoted(name.text));

    const bool alike = storage == storage_t::type_name ?
                           same_type(earlier.type, t) :
                           compatible(earlier.type, t);
    if (alike && earlier.qualifiers == qualifiers) {
        earlier.type = composite(earlier.type, t);
        return earlier;
    }

    refuse_tag_twins(name, earlier.type, t);
    refuse_made_pointees(name, earlier.type, t, compatibility_words);

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
                    qualifiers_t qualifiers,
                    storage_t storage)
{
    const auto &scope = p.scopes.back().names;

    const auto it = scope.find(name.text);
    if (it == scope.end())
        return nullptr;

    return &redeclared(name, *it->second, t, qualifiers, storage);
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
// gives the registers.  The symbol keeps the parameter's qualifiers,
// for its use in a definition's body.
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

    added(p, param.name->text, param.type, storage_t::parameter).qualifiers =
        param.qualifiers;
}

// The identity of a not-yet type that a means makes from a type: the
// earlier one's where the same means made one from the same type, so
// that the type written twice is one type, as a typedef declared again
// must be, 6.7.1p4, and a function declared again, p5; a new one
// otherwise.
int
identity_of(parser_t &p, std::string means, const type_t &from)
{
    for (const made_type_t &m : p.made)
        if (m.means == means && m.from == from)
            return m.identity;

    p.made.push_back({ std::move(means), from, p.identities });

    return p.identities++;
}

// The words of a run of tokens, an identifier spelled __x__ read as x,
// as an attribute's name is, so that two spellings of one attribute or
// one argument are one means.
std::string
spelling(const parser_t &p, std::size_t first, std::size_t end)
{
    std::string words;
    for (std::size_t i = first; i < end; ++i)
        words += std::string(attribute_name(p.tokens[i])) + " ";

    return words;
}

// The words of a marking attribute and its arguments, from its name to
// the ')' that closes them, the means a typedef through it is made by.
std::string
attribute_spelling(const parser_t &p, const token_t &attribute)
{
    const std::size_t first = std::size_t(&attribute - p.tokens.data());
    std::size_t end = first + 1;

    if (p.tokens[end].kind == tok_t::lparen)
        for (int depth = 0; end < p.tokens.size(); ++end) {
            depth += p.tokens[end].kind == tok_t::lparen;
            depth -= p.tokens[end].kind == tok_t::rparen;
            if (depth == 0) {
                ++end;
                break;
            }
        }

    return spelling(p, first, end);
}

// The words of every marking attribute of a declaration and their
// arguments, attribute_spelling()'s, in their order: the means a
// typedef through them is made by.
std::string
markings_spelling(const parser_t &p, const markings_t &markings)
{
    std::string words;
    for (const token_t *name : markings.names)
        words += attribute_spelling(p, *name);

    return words;
}

// A typedef declares its declarator's name as a name for the type the
// declarator gives, in the current scope, 6.7.9p3, and its symbol keeps
// the qualifiers the declarator says of the type, which a use of the
// name joins to its own, declaration_specifiers().  The name has no
// object, so nothing is generated for it, and it takes no assembler
// name and no initializer, which gives the value stored in an object,
// 6.7.11p9.  A typedef of function type defines no function: a
// function definition's storage class is extern or static, 6.9.2p4, so
// a body after its declarator is an error of its own.  The type may be
// a function type, whose parameters share one scope as a prototype's
// do.  Through a marking attribute the name stands for a not-yet type
// of its own, of the attribute's words: typedef int register_t
// __attribute__ ((__mode__ (__word__))) names an integer of a size rcc
// does not take, and its objects are refused.  The attributes, their
// arguments included, made from the same type are the same type again,
// markings_spelling()'s.  A typedef of a
// variable length array, whose size the typedef fixes, 6.7.9p8, is not
// yet.
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

    if (is_variable_array(d.type))
        not_yet(*d.name,
                "typedef of variable length array " + quoted(d.name->text));

    type_t t = d.type;
    if (tail.marking) {
        const int identity =
            identity_of(p, markings_spelling(p, tail.markings), t);
        t = make_not_yet(identity, attribute_words(*tail.marking), true);
    }

    if (!earlier_declaration(p, *d.name, t, d.qualifiers, storage_t::type_name))
        added(p, d.name->text, std::move(t), storage_t::type_name).qualifiers =
            d.qualifiers;
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

// A declaration under a #pragma GCC visibility other than default
// gives that visibility to the symbol it names where the symbol has
// external linkage, as clang has it: a definition and a declaration
// alike, of a function or an object, at file scope or in a block.  rcc
// writes every symbol visible, so such a declaration is not yet,
// judged at the declarator's name; one of a name of internal linkage,
// a typedef and a local are what they are without the pragma
// (shape.md chapter 2).
void
refuse_visibility(const token_t &name, linkage_t linkage)
{
    if (name.nondefault_visibility && linkage == linkage_t::external)
        not_yet(name, "pragma 'GCC visibility'");
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
// the declaration's one marking attribute, or one of two beside weak,
// which makes the alias a weak symbol, on a function none of whose
// parameters an attribute marks, or on an object declared extern or
// static, and the declarator has no assembler name; any other alias is
// not yet where it is declared.  A definition is no alias, an error, as
// GNU C has it: a function's body, an object's initializer or a
// tentative definition of external linkage; a static object's
// declaration that the alias marks is no tentative definition, and
// defines nothing.  The symbol and the target are checked at the end
// of the unit, resolve_aliases().
void
declared_alias(parser_t &p,
               const specifiers_t &spec,
               const declarator_t &d,
               declarator_tail_t &tail,
               bool function)
{
    const token_t &attribute = *tail.markings.alias;
    const std::string words = attribute_words(attribute);
    const bool weak = tail.markings.weak != nullptr;

    if (tail.markings.count > (weak ? 2 : 1))
        not_yet(attribute, words + " beside another attribute that marks");

    if (tail.asm_keyword)
        not_yet(attribute, words + " with an assembler name");

    if (const token_t *marked = function ? marked_parameter(d) : nullptr)
        not_yet(*marked, attribute_words(*marked));

    const bool tentative =
        !declared(spec, tok_t::kw_extern) && !declared(spec, tok_t::kw_static);
    const bool defined = function ? peek(p).kind == tok_t::lbrace :
                                    tentative || peek(p).kind == tok_t::assign;
    if (defined)
        fail(*d.name, "definition of " + quoted(d.name->text) +
                          " cannot also be an alias");

    alias_target(p, attribute);
    p.aliases.push_back(pending_alias_t { d.name, &attribute, function, weak });
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
        earlier_declaration(p, *d.name, d.type, { }, storage_t::function);
    const linkage_t linkage = linkage_of(spec, earlier, true);
    refuse_visibility(*d.name, linkage);

    if (earlier) {
        require_linkage(*d.name, *earlier, linkage);
        record_tail(p, *earlier, tail);

        for (std::size_t i = 0; i < p.unit.functions.size(); ++i)
            if (p.unit.functions[i].sym == earlier)
                return i;
    }

    symbol_t &sym = added(p, d.name->text, d.type, storage_t::function);
    sym.linkage = linkage;
    record_tail(p, sym, tail);
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
// one of unsigned int takes L"..." and U"...".  The literal may stand in
// parentheses, ("abc"), which is the literal, 6.5.2p6, as it is outside
// a list.  Any other expression there initializes the array's first
// element.  A wide literal ahead of an array of char is the literal
// string_value() refuses.  The tokens are looked at from the from'th
// ahead, past a declaration's = and { for its own list.
bool
string_ahead(const parser_t &p, const type_t &t, std::size_t from = 0)
{
    std::size_t opened = 0;
    while (peek(p, from + opened).kind == tok_t::lparen)
        ++opened;

    std::size_t ahead = from + opened;
    encoding_t encoding = encoding_t::none;
    for (; peek(p, ahead).kind == tok_t::string; ++ahead)
        if (encoding_of(peek(p, ahead)) != encoding_t::none)
            encoding = encoding_of(peek(p, ahead));

    std::size_t closed = 0;
    while (closed < opened && peek(p, ahead + closed).kind == tok_t::rparen)
        ++closed;

    const tok_t after = peek(p, ahead + closed).kind;
    if (ahead == from + opened || closed != opened ||
        (after != tok_t::comma && after != tok_t::rbrace))
        return false;

    if (is_char_array(t))
        return true;

    const type_t element = element_type(p, encoding);

    return size_of(element) > 1 && *as< array_t >(t).element == element;
}

// GNU C's obsolete designation, y: 2 for .y = 2, at the token ahead
// past the given count, is not yet: an identifier before a colon starts
// no expression in a list, so one where a designation may stand is
// this one.
void
refuse_old_designation(const parser_t &p, std::size_t ahead)
{
    const token_t &name = peek(p, ahead);
    if (name.kind == tok_t::identifier &&
        peek(p, ahead + 1).kind == tok_t::colon)
        not_yet(name,
                "old-style designator " + quoted(std::string(name.text) + ":"));
}

// Whether a designation, [2] = or .x =, 6.7.11p1, is ahead: its first
// designator.
bool
designation_ahead(const parser_t &p)
{
    refuse_old_designation(p, 0);

    return next_is(p, { tok_t::lbracket, tok_t::dot });
}

// Whether the ',' ahead is followed by a designation, which begins at
// the current object of the closest brace pair, 6.7.11p19.
bool
designation_after_separator(const parser_t &p)
{
    if (peek(p).kind == tok_t::comma)
        refuse_old_designation(p, 1);

    const tok_t after = peek(p, 1).kind;

    return peek(p).kind == tok_t::comma &&
           (after == tok_t::lbracket || after == tok_t::dot);
}

// A designator ahead for an object of a kind it names no subobject of,
// 6.7.11p7, p8: [2] for one that is no array, .x for one that is no
// struct or union.
[[noreturn]] void
refuse_designator(const parser_t &p)
{
    if (peek(p).kind == tok_t::lbracket)
        fail(peek(p), "array designator cannot initialize non-array type");

    fail(peek(p),
         "field designator cannot initialize a non-struct, non-union type");
}

// An array designator, [ constant-expression ], 6.7.11p1, of an element
// of array: the index, an integer constant expression, p7, of an
// element the array holds, p3, or any nonnegative one for an array of
// unknown size, p7.  GNU C's range of elements, [1 ... 3], is not yet.
long
array_designator(parser_t &p, const array_t &array)
{
    if (peek(p).kind != tok_t::lbracket)
        refuse_designator(p);

    advance(p);

    const token_t &start = peek(p);
    const expr_t index = rvalue(start, conditional_expression(p));
    if (!is_integer_constant(index))
        fail(start, "expression is not an integer constant expression");

    if (peek(p).kind == tok_t::ellipsis)
        not_yet(peek(p), "range designator");

    expect(p, tok_t::rbracket, "expected ']'");

    const exact_t value = exact_value(index);
    if (value < 0)
        fail(start, "array designator value '" +
                        std::to_string(static_cast< long long >(value)) +
                        "' is negative");

    if (array.count >= 0 && value >= array.count)
        fail(start,
             "array designator index (" +
                 std::to_string(static_cast< unsigned long long >(value)) +
                 ") exceeds array bounds (" + std::to_string(array.count) +
                 ")");

    return long(value);
}

member_t named_member(const record_t &, const token_t &);
std::optional< member_t > member_named(const record_t &, std::string_view);

// Whether the member m is the member of the name, or an anonymous
// member that holds it, at any depth, 6.7.3.2p15; an unnamed bit-field
// is neither.
bool
holds_member(const member_t &m, std::string_view name)
{
    if (!m.name.empty())
        return m.name == name;

    if (!is_anonymous(m))
        return false;

    return member_named(*as< record_type_t >(m.type).record, name).has_value();
}

// A member designator, . identifier, 6.7.11p1, of a member of r: the
// index of the member it names, p8.  A member of an anonymous member is
// the record's too, 6.7.3.2p15: its designator gives the anonymous
// member that holds it, at any depth, and is left ahead, so that the
// walk enters the anonymous member as it enters s at .s.l, p19, and
// goes on there, p18.  A name r has no member of is named_member()'s
// error.
std::size_t
member_designator(parser_t &p, const record_t &r)
{
    if (peek(p).kind != tok_t::dot)
        refuse_designator(p);

    const token_t &name = peek(p, 1);
    if (name.kind != tok_t::identifier)
        fail(name, "expected member name");

    named_member(r, name);

    std::size_t i = 0;
    while (!holds_member(r.members[i], name.text))
        ++i;

    if (!r.members[i].name.empty()) {
        advance(p);
        advance(p);
    }

    return i;
}

// The '=' that ends a designation, 6.7.11p1.  GNU C leaves it out after
// an array designator, [2] 5, which is not yet.
void
designation_end(parser_t &p)
{
    if (accept(p, tok_t::assign))
        return;

    if (p.tokens[p.pos - 1].kind == tok_t::rbracket)
        not_yet(peek(p), "designator without '='");

    fail(peek(p), "expected '=' or another designator");
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

    expr_t init = assignment_conversion(p, at, std::move(e), t);

    if (is< literal_t >(init))
        return as< literal_t >(init);

    if (narrowed_address(init))
        fail(start, "initializer element is an address cast to an integer "
                    "narrower than a pointer");

    if (std::optional< initializer_t > address = address_constant(init))
        return std::move(*address);

    refuse_unfolded(start, init,
                    "initializer element is not a compile-time constant");
}

// Where the walk over an initializer puts each value it finds,
// 6.7.11p18.  For an object of static storage it is the object's data:
// each subobject's constant at its offset in the object, which c1
// writes (shape.md chapter 4).  For a local it is the statements after
// the declaration of the object local, stores: each scalar's value
// assigned to its place in the object, and each string literal copied
// to its array's, in the list's order (G13).
// reached is the end of the highest byte a value has gone to, so a
// value at or past it overrides none, p20, as every value of a list
// without designators is; members is the member each union holds a
// value of, by the union's offset in the object and its record; stored
// is the bytes of a local each store has gone to, its offset and its
// size.
struct destination_t
{
    std::vector< subobject_t > *data = nullptr;
    const symbol_t *local = nullptr;
    std::vector< stmt_t > *stores = nullptr;
    long reached = 0;
    std::map< std::pair< long, const record_t * >, std::size_t > members = { };
    std::vector< std::pair< long, long > > stored = { };
};

// The elements of the array that the string of sub initializes, each a
// constant of the element's type at its offset: the literal's, as far
// as the array holds them.  Its terminating zero and the elements past
// it are zero, 6.7.11p22, and take none.
std::vector< subobject_t >
string_elements(const subobject_t &sub)
{
    const auto &str = std::get< string_t >(sub.value);
    const auto &array = as< array_t >(sub.type);
    const long count =
        std::min(array.count, long(str.bytes.size()) / str.width);

    std::vector< subobject_t > elements;
    for (long i = 0; i < count; ++i) {
        exact_t bits = 0;
        for (int b = str.width - 1; b >= 0; --b)
            bits = bits << 8 |
                   static_cast< unsigned char >(str.bytes[i * str.width + b]);

        const literal_t value { wrapped(bits, as< int_t >(*array.element)) };
        elements.push_back(
            subobject_t { sub.offset + i * str.width, *array.element, value });
    }

    return elements;
}

// The values the data of an object of static storage holds in the size
// bytes at offset, which a later initializer overrides, 6.7.11p20,
// dropped.  A string whose array the bytes cover in part, { .s = "abc",
// .s[1] = 'x' }, is its elements first, and those outside the bytes
// stay.
void
dropped(destination_t &to, long offset, long size)
{
    const auto inside = [offset, size](const subobject_t &sub) {
        return sub.offset < offset + size &&
               offset < sub.offset + size_of(sub.type);
    };

    std::vector< subobject_t > kept;
    for (subobject_t &sub : *to.data) {
        if (!inside(sub)) {
            kept.push_back(std::move(sub));
            continue;
        }

        if (!std::holds_alternative< string_t >(sub.value))
            continue;

        for (subobject_t &element : string_elements(sub))
            if (!inside(element))
                kept.push_back(std::move(element));
    }

    *to.data = std::move(kept);
}

// The store of value into the local at offset, of the scalar type t:
// the assignment of the value to the place *(T *) ((char *) &local +
// offset), lvalue_at()'s, at the token at, appended to the local's
// statements.  The parser makes the node, since initialization is no
// assignment, 6.7.11p12, and the place is of the unqualified type, as
// p12 takes it.
void
local_assignment(parser_t &p,
                 const token_t &at,
                 const type_t &t,
                 long offset,
                 expr_t value,
                 destination_t &to)
{
    expr_t object { to.local->type, name_t { to.local } };
    expr_t place =
        lvalue_at(p, address_of(at, std::move(object)), offset, t, { });

    expr_t store { t, assign_t { std::nullopt, t, box(std::move(place)),
                                 box(std::move(value)) } };
    to.stores->push_back(stmt_t { expr_stmt_t { std::move(store) } });
}

// The size bytes at offset of a local, of a subobject a later
// initializer overrides, 6.7.11p20, zeroed again when a store of the
// list has gone to them: the stores of 0 in units of the subobject's
// alignment, at most 8 bytes, lowest address first, after the stores
// before and before the later initializer's, as the declaration zeroes
// the whole object (G20).  The bytes then hold what the bytes of an
// object of static storage of the same list hold.
void
zeroed_again(parser_t &p,
             const token_t &at,
             destination_t &to,
             long offset,
             long size,
             int alignment)
{
    const auto inside = [offset, size](const std::pair< long, long > &s) {
        return s.first < offset + size && offset < s.first + s.second;
    };

    if (std::erase_if(to.stored, inside) == 0)
        return;

    const int unit = std::min(alignment, 8);
    const int_kind_t kind = unit == 1 ? int_kind_t::unsigned_char :
                            unit == 2 ? int_kind_t::unsigned_short :
                            unit == 4 ? int_kind_t::unsigned_ :
                                        int_kind_t::unsigned_long;
    const type_t t = make_int(unit * 8, false, kind);
    for (long k = 0; k < size; k += unit)
        local_assignment(p, at, t, offset + k, expr_t { t, literal_t { 0 } },
                         to);
}

// A later initializer of a subobject overrides an earlier one, 6.7.11p20:
// in an object of static storage the values in the size bytes at offset
// are dropped, and a local's bytes are zeroed again.  at is the token
// of the later initializer.  No value has gone to the bytes at or past
// the highest one reached.
void
overridden(parser_t &p,
           const token_t &at,
           destination_t &to,
           long offset,
           long size,
           int alignment)
{
    if (offset >= to.reached)
        return;

    if (to.local)
        zeroed_again(p, at, to, offset, size, alignment);
    else
        dropped(to, offset, size);
}

// A value of size bytes at offset has gone to the object.
void
reach(destination_t &to, long offset, long size)
{
    to.reached = std::max(to.reached, offset + size);
}

// The member of the union r at offset that the walk initializes,
// 6.7.11p18, p19.  A union holds the value of one member, so an
// initializer of a member other than the one an earlier initializer
// gave a value overrides the whole union, p20: { .i = 1, .c = 2 } holds
// c alone, the rest of the union zero, as clang holds it.
void
union_member(parser_t &p,
             destination_t &to,
             const record_t &r,
             long offset,
             std::size_t member)
{
    const auto [held, added] = to.members.try_emplace({ offset, &r }, member);
    if (added || held->second == member)
        return;

    held->second = member;
    overridden(p, peek(p), to, offset, r.size, r.alignment);
}

// The store of the scalar t at offset in a local, the expression ahead
// converted to t as by simple assignment, 6.7.11p12, at the token a
// conversion's error is said at, local_assignment()'s; p12 takes the
// scalar's unqualified type.  A record's value in a scalar's place,
// which may initialize a record the scalar begins, p14, is not yet; a
// void expression has no value, 6.3.2.2p1, the error of the expression
// said at its first token.
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

    expr_t value = assignment_conversion(p, at, std::move(e), t);
    local_assignment(p, at, t, offset, std::move(value), to);

    to.stored.emplace_back(offset, size_of(t));
    reach(to, offset, size_of(t));
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
    overridden(p, at, to, offset, size_of(t), align_of(t));
    to.data->push_back(subobject_t { offset, t, std::move(value) });
    reach(to, offset, size_of(t));
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

// The copy of the string literal str into the array of type t at offset
// in local, 6.7.11p15, p16: a record's assignment's node on an array,
// which each generator writes by the copy's row (G18, G22).  Its left
// operand is the array's first elements, as many as the literal has
// with its terminating zero, or as the array holds where that is fewer,
// the lvalue at offset in the local, lvalue_at()'s; its right is the
// literal itself, an object of its own (G17), of which the copy reads
// as many.  The parser makes the node, since initialization is no
// assignment, 6.7.11p12, and the place is of the unqualified type, as
// local_assignment()'s is.
expr_t
string_copy(const parser_t &p,
            const token_t &at,
            const symbol_t &local,
            const type_t &t,
            long offset,
            expr_t str)
{
    const auto &array = as< array_t >(t);
    const long given = size_of(str.type) / size_of(*array.element);
    const type_t copied =
        make_array(*array.element, std::min(array.count, given));

    expr_t object { local.type, name_t { &local } };
    expr_t place =
        lvalue_at(p, address_of(at, std::move(object)), offset, copied, { });

    return expr_t { copied,
                    assign_t { std::nullopt, copied, box(std::move(place)),
                               box(std::move(str)) } };
}

// The string literal ahead, which string_ahead() found, for the array t
// at offset.  An object of static storage holds the literal's bytes as
// the array's own; a local's array takes them by their copy,
// string_copy()'s, a store of the list's after the declaration's
// zeroing (G20), which goes to the bytes copied.
long
listed_string(parser_t &p, const type_t &t, long offset, destination_t &to)
{
    const token_t &start = peek(p);
    expr_t e = assignment_expression(p);

    if (offset < to.reached)
        overridden(p, start, to, offset, size_of(t), align_of(t));

    std::vector< subobject_t > checked;
    auto &values = to.local ? checked : *to.data;
    const long count =
        string_value(start, as< string_t >(e), t, offset, values);
    const type_t sized = values.back().type;
    reach(to, offset, size_of(sized));

    if (to.local) {
        expr_t copy =
            string_copy(p, start, *to.local, sized, offset, std::move(e));
        to.stored.emplace_back(offset, size_of(copy.type));
        to.stores->push_back(stmt_t { expr_stmt_t { std::move(copy) } });
    }

    return count;
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

// How a walk over the subobjects of an aggregate meets a designation,
// 6.7.11p19, whose designators begin at the current object of the
// closest brace pair: the walk of a brace-enclosed list takes each
// designation in it; a walk a designator entered takes the rest of that
// designation first and ends at the next one; the walk of a
// subaggregate without braces, p21, ends at one.  A walk that ends at a
// designation leaves it, and its ',', to the walk of the list.
enum class walk_t { braced, designated, elided };

// Whether a walk takes a designation before its first initializer: one
// a designator entered does, and a list's walk when one is ahead.
bool
starts_designated(const parser_t &p, walk_t walk)
{
    return walk == walk_t::designated ||
           (walk == walk_t::braced && designation_ahead(p));
}

// Whether the walk takes the designation after the ',' ahead, which it
// takes: the walk of a list does.
bool
takes_designation(parser_t &p, walk_t walk)
{
    if (walk != walk_t::braced || !designation_after_separator(p))
        return false;

    advance(p);

    return true;
}

// Whether the walk goes on to another initializer after one, and takes
// the ',' before it: not past its last subobject, at_end, nor at a
// designation, which another walk takes, nor at the list's end.
bool
goes_on(parser_t &p, bool at_end)
{
    return !at_end && !designation_after_separator(p) && accept_separator(p);
}

void
designated_subobject(parser_t &, const type_t &, long offset, destination_t &);

// The elements of an array at offset, from the initializers ahead in a
// list, in increasing subscript order, 6.7.11p18: each element from
// the next initializer, or from as many as it takes when that one does
// not begin with a brace, p21.  A designation moves the walk to the
// element it names, p18, p19: the walk goes on from the next one, and a
// later initializer of an element overrides an earlier one, p20.  An
// initializer is ahead.  The elements end with the array's count,
// leaving the rest of the list to the enclosing aggregate or to the
// list's own check, or with the list; an array of unknown size ends
// only with the list.  Returns the count of elements given, past the
// highest index initialized, which completes such an array, p23.
long
elements(parser_t &p,
         const array_t &array,
         long offset,
         destination_t &to,
         walk_t walk)
{
    const type_t &element = *array.element;
    const long size = size_of(element);

    long next = 0;
    long count = 0;
    bool designated = starts_designated(p, walk);
    for (;;) {
        if (designated) {
            next = array_designator(p, array);
            designated_subobject(p, element, offset + next * size, to);
        }
        else
            subobject_initializer(p, element, offset + next * size, to);

        count = std::max(count, ++next);

        designated = takes_designation(p, walk);
        if (!designated && !goes_on(p, next == array.count))
            return count;
    }
}

// The integer type of size bytes, of the signedness and of the kind of
// that width: the type a bit-field's access unit is read as.
type_t
unit_type(int size, bool is_signed)
{
    const auto of = [size, is_signed](int_kind_t s, int_kind_t u) {
        return make_int(size * 8, is_signed, is_signed ? s : u);
    };

    switch (size) {
    case 1: return of(int_kind_t::signed_char, int_kind_t::unsigned_char);
    case 2: return of(int_kind_t::short_, int_kind_t::unsigned_short);
    case 4: return of(int_kind_t::int_, int_kind_t::unsigned_);
    default: return of(int_kind_t::long_, int_kind_t::unsigned_long);
    }
}

// The type of a bit-field's value, its promotion, 6.3.1.1p2: int, which
// holds every value of a field narrower than 32 bits; at 32 bits int or
// unsigned int by the field's signedness; and past them its declared
// type promoted, as clang has it for the types p2 leaves to the
// implementation (shape.md chapter 3).
type_t
field_promoted(const member_t &m)
{
    const int_t &declared = as< int_t >(m.type);

    if (m.width < 32)
        return int_type();

    if (m.width == 32)
        return unit_type(4, declared.is_signed);

    return promoted(m.type);
}

// The read of a bit-field m whose unit's lvalue is unit, of the unit's
// size and the field's signedness, shape.md chapter 5: the unit loaded
// once, as R bits, 32 for a unit of 1, 2 or 4 bytes and 64 for one of 8,
// and the field's bits moved to the bottom by shifts alone, no mask.
// Where the field reaches the unit's top, the load's extension has done
// a left shift's work, and one right shift by the field's bit is left;
// otherwise a left shift by R less the bit and the width puts the
// field's top at R's, and a right shift by R less the width brings it
// down.  The right shift is arithmetic for a signed field and logical
// for an unsigned one, by the type of R bits of the field's signedness.
// No shift is by 0.  The value is of the field's promoted type.
expr_t
field_read(expr_t unit, const member_t &m)
{
    const int r = m.unit == 8 ? 64 : 32;
    const type_t wide = unit_type(r / 8, as< int_t >(m.type).is_signed);
    const auto count = [](int n) {
        return expr_t { int_type(), literal_t { n } };
    };

    expr_t e = convert_to(std::move(unit), wide);
    if (m.bit + m.width == m.unit * 8) {
        e = make_binary(binary_op_t::shr, wide, std::move(e), count(m.bit));
        return convert_to(std::move(e), field_promoted(m));
    }

    e = make_binary(binary_op_t::shl, wide, std::move(e),
                    count(r - m.bit - m.width));
    e = make_binary(binary_op_t::shr, wide, std::move(e), count(r - m.width));

    return convert_to(std::move(e), field_promoted(m));
}

// Whether e is the node a field's read may be of: the access unit's
// lvalue, a store's assignment, or the comma of a compound assignment
// or a step, whose right operand is its store, field_update()'s.
bool
is_read_operand(const expr_t &e)
{
    if (is< assign_t >(e))
        return true;

    if (is< unary_t >(e))
        return as< unary_t >(e).op == unary_op_t::deref;

    return is< binary_t >(e) && as< binary_t >(e).op == binary_op_t::comma &&
           is< assign_t >(*as< binary_t >(e).rhs);
}

// The node a field's read is of, is_read_operand()'s: field_read()'s
// tree undone, its conversions and shifts taken off, each of which is
// of its first operand; and a postfix step's value, the comma of the
// update and the read of the temporary, taken off to its update.
expr_t
read_operand(expr_t read)
{
    while (!is_read_operand(read)) {
        expr_t inner = is< cast_t >(read) ?
                           std::move(*std::get< cast_t >(read.v).arg) :
                           std::move(*std::get< binary_t >(read.v).lhs);
        read = std::move(inner);
    }

    return read;
}

token_t operator_at(const token_t &, tok_t);
std::optional< expr_t > pure_copy(const expr_t &);

// The type of R bits that a store into the bit-field m computes in, 32
// for a unit of 1, 2 or 4 bytes and 64 for one of 8 (shape.md chapter
// 5).
type_t
wide_type(const member_t &m)
{
    return unit_type(m.unit == 8 ? 8 : 4, true);
}

// The mask M of the bit-field m in its unit.
exact_t
field_mask(const member_t &m)
{
    return ((exact_t(1) << m.width) - 1) << m.bit;
}

// The field's bits of a value of R bits for a store into the bit-field
// m, (v << s) & M, s the field's bit and M its mask in the unit
// (shape.md chapter 5).  No shift is by 0.
expr_t
placed_bits(expr_t value, const member_t &m)
{
    const type_t wide = wide_type(m);
    const exact_t mask = field_mask(m);

    if (m.bit > 0)
        value = make_binary(binary_op_t::shl, wide, std::move(value),
                            expr_t { int_type(), literal_t { m.bit } });

    return make_binary(
        binary_op_t::bit_and, wide, std::move(value),
        expr_t { wide, literal_t { wrapped(mask, as< int_t >(wide)) } });
}

// The unit a store into the bit-field m leaves, of the type t: bits,
// the field's bits placed, on the left of the |, and the rest of the
// unit's value unit kept, bits | (unit & ~M), by R bits (shape.md
// chapter 5).
expr_t
merged_unit(expr_t bits, expr_t unit, const member_t &m, const type_t &t)
{
    const type_t wide = wide_type(m);
    const exact_t mask = field_mask(m);

    expr_t kept = make_binary(
        binary_op_t::bit_and, wide, convert_to(std::move(unit), wide),
        expr_t { wide, literal_t { wrapped(~mask, as< int_t >(wide)) } });
    expr_t merged = make_binary(binary_op_t::bit_or, wide, std::move(bits),
                                std::move(kept));

    return convert_to(std::move(merged), t);
}

// The store of rhs into the bit-field m whose access unit's lvalue is
// place, at the token op, the parser's tree of the tree's own nodes, by
// R bits, 32 for a unit of 1, 2 or 4 bytes and 64 for one of 8
// (shape.md chapter 5).  The value keeps the field's low bits, of either
// signedness, 6.7.3.2p12 (G2), so an integer is taken in R bits as it
// is, and anything else is converted as if by assignment, 6.5.17.2p2, a
// _Bool field's value its comparison with 0, 6.3.1.2, the 0 or 1 of the
// comparison taken as it is, with no narrowing to the byte.  A constant that
// is 0 in the field's bits is one compound assignment on the unit, u &=
// ~M, M the field's mask in the unit, and one that is all ones is u |=
// M, so the place is written once.  Any other value is u = ((v << s) &
// M) | (u & ~M), s the field's bit, the value on the left of the |, so
// that it is computed before the unit is read, 6.5.1p3 (G13); a
// constant's placed bits are one constant, and no shift is by 0.  The
// place stands twice, the second a copy of its expression, so one with
// a side effect is not yet.  The store is of the unit's type.
expr_t
field_store(
    parser_t &p, const token_t &op, expr_t place, const member_t &m, expr_t rhs)
{
    const type_t wide = wide_type(m);
    const exact_t low = (exact_t(1) << m.width) - 1;
    const exact_t mask = field_mask(m);
    const auto constant = [&wide](exact_t v) {
        return expr_t { wide, literal_t { wrapped(v, as< int_t >(wide)) } };
    };

    if (is_bool(m.type) || !is< int_t >(rhs.type))
        rhs = assignment_conversion(p, op, std::move(rhs), m.type);
    else
        rhs = rvalue(op, std::move(rhs));

    if (is_bool(m.type) && is< cast_t >(rhs) && is_bool(rhs.type))
        rhs = expr_t { std::move(*std::get< cast_t >(rhs.v).arg) };

    expr_t value = convert_to(std::move(rhs), wide);
    if (is< literal_t >(value)) {
        const exact_t bits = exact_t(bits_of(value)) & low;
        if (bits == 0)
            return assignment(p, operator_at(op, tok_t::and_assign),
                              std::move(place), constant(~mask));

        if (bits == low)
            return assignment(p, operator_at(op, tok_t::or_assign),
                              std::move(place), constant(mask));

        value = constant(bits << m.bit);
    }
    else
        value = placed_bits(std::move(value), m);

    std::optional< expr_t > copy = pure_copy(place);
    if (!copy)
        not_yet(op, "store into bit-field " + quoted(m.name) +
                        " through a place with a side effect");

    const type_t t = place.type;
    expr_t unit = merged_unit(std::move(value), std::move(*copy), m, t);

    return expr_t { t, assign_t { std::nullopt, t, box(std::move(place)),
                                  box(std::move(unit)) } };
}

// The value of a compound assignment op of rhs, or of a step, to the
// bit-field m, from the unit's value held in the temporary t: op's
// production on the field's read of t, field_read()'s, and rhs, in
// their common type, or in the promoted field's for a shift,
// 6.5.17.3p4, compound_operation()'s; for a _Bool field the result
// compared with 0, 6.3.1.2, as field_store() takes a value, the 0 or 1
// of the comparison as it is.
expr_t
field_operation(const token_t &op,
                const symbol_t &t,
                const member_t &m,
                expr_t rhs)
{
    expr_t read = field_read(expr_t { t.type, name_t { &t } }, m);
    expr_t value = compound_operation(op, std::move(read), std::move(rhs));
    if (!is_bool(m.type))
        return value;

    value = converted_to_bool(std::move(value));
    if (is< cast_t >(value))
        return expr_t { std::move(*std::get< cast_t >(value.v).arg) };

    return value;
}

// A compound assignment op of rhs, or a step, to the bit-field m whose
// access unit's lvalue is place, at the token op, words naming it, the
// parser's tree of one temporary (shape.md chapter 5): (t = u, u =
// (((F (t) op v) << s) & M) | (t & ~M)), t the temporary of the unit's
// type, which the caller made, F (t) the field's read of it, v the
// right operand.  The unit is loaded once and stored once.  A right
// operand with a side effect, h () in f += h (), is held in a
// temporary of its own first, so that it runs before the unit is
// loaded, 6.5.1p3 (G13), and a store it makes into the unit stays; any
// other is v as it is.  The result keeps the field's low bits, of
// either signedness, 6.7.3.2p12 (G2), by the mask alone.  The place
// stands twice, the second a copy of its expression, so one with a
// side effect is not yet.  The tree is the comma whose right operand
// is the store, of the unit's type.
expr_t
field_update(parser_t &p,
             const token_t &op,
             std::string_view words,
             expr_t place,
             const member_t &m,
             expr_t rhs,
             const symbol_t &t)
{
    std::optional< expr_t > copy = pure_copy(place);
    if (!copy)
        not_yet(op, std::string(words) + " bit-field " + quoted(m.name) +
                        " through a place with a side effect");

    const auto held = [](const symbol_t &s) {
        return expr_t { s.type, name_t { &s } };
    };
    const auto set = [&held](const symbol_t &s, expr_t value) {
        return expr_t { s.type, assign_t { std::nullopt, s.type, box(held(s)),
                                           box(std::move(value)) } };
    };

    expr_t first = set(t, std::move(*copy));
    if (!pure_copy(rhs)) {
        const symbol_t &operand = temporary(p, "__field_operand", rhs.type);
        first = comma(discarded(op, set(operand, std::move(rhs))),
                      std::move(first));
        rhs = held(operand);
    }

    expr_t value = field_operation(op, t, m, std::move(rhs));
    expr_t bits = placed_bits(convert_to(std::move(value), wide_type(m)), m);

    const type_t u = place.type;
    expr_t unit = merged_unit(std::move(bits), held(t), m, u);
    expr_t store { u, assign_t { std::nullopt, u, box(std::move(place)),
                                 box(std::move(unit)) } };

    return comma(discarded(op, std::move(first)), std::move(store));
}

// E1 op= E2 on a bit-field m that does not fill its unit, whose
// unit's lvalue is place: field_update()'s, the right operand an
// integer as for any compound assignment, 6.5.17.3p1, p2.  Its value,
// 6.5.17.1p3, is the field's read of the unit the update stores,
// field_read()'s: the new value converted to the field and promoted,
// with no second load of the unit.
expr_t
field_compound(
    parser_t &p, const token_t &op, expr_t place, const member_t &m, expr_t rhs)
{
    rhs = rvalue(op, std::move(rhs));
    require_integer(op, rhs);

    const symbol_t &t = temporary(p, "__field_unit", place.type);
    expr_t update = field_update(p, op, "compound assignment of",
                                 std::move(place), m, std::move(rhs), t);

    return field_read(std::move(update), m);
}

// ++E, --E, E++ and E-- on a bit-field m that does not fill its unit,
// whose unit's lvalue is place: E += 1 and E -= 1, 6.5.4.1p2, p3,
// 6.5.3.5p2, p3, field_update()'s.  A prefix step's value is the
// field's read of the unit it stores, as a compound assignment's; a
// postfix step's is the field's value before, its read of the
// temporary.
expr_t
field_step(parser_t &p,
           const token_t &op,
           expr_t place,
           const member_t &m,
           bool postfix)
{
    const symbol_t &t = temporary(p, "__field_unit", place.type);
    expr_t one { int_type(), literal_t { 1 } };
    expr_t update = field_update(p, step_assignment(op), "step of",
                                 std::move(place), m, std::move(one), t);
    if (!postfix)
        return field_read(std::move(update), m);

    expr_t before = field_read(expr_t { t.type, name_t { &t } }, m);

    return comma(discarded(op, std::move(update)), std::move(before));
}

// The constant of a named bit-field in the data of an object of static
// storage, the field's record at offset in the object: the value of its
// declared type cut to its width, 6.7.3.2p12 (G2), in the bytes the
// field's bits lie in, from the lowest bit up (shape.md chapter 3).
// Each such byte is an unsigned char subobject, which the bits of the
// other fields in it share, so a later value of the field clears the
// field's own bits first, 6.7.11p20, and no other: a neighbour's value
// stays.  No other subobject holds a byte of a field's bits.
void
field_data(destination_t &to, const member_t &m, long offset, long long value)
{
    const long first = (offset + m.offset) * 8 + m.bit;
    const exact_t mask = (exact_t(1) << m.width) - 1;
    const exact_t cut =
        exact_t(static_cast< unsigned long long >(value)) & mask;
    const exact_t bits = cut << (first % 8);
    const exact_t clear = mask << (first % 8);
    const type_t byte = make_int(8, false, int_kind_t::unsigned_char);

    for (long k = first / 8; k <= (first + m.width - 1) / 8; ++k) {
        const int shift = int(k - first / 8) * 8;
        const long long in = static_cast< long long >((bits >> shift) & 0xff);
        const long long out = static_cast< long long >((clear >> shift) & 0xff);

        const auto held = std::ranges::find_if(
            *to.data, [k](const subobject_t &sub) { return sub.offset == k; });
        if (held == to.data->end()) {
            to.data->push_back(subobject_t { k, byte, literal_t { in } });
            reach(to, k, 1);
            continue;
        }

        if (size_of(held->type) != 1 ||
            !std::holds_alternative< literal_t >(held->value))
            throw std::logic_error("field_data: a byte held by another value");

        auto &held_value = std::get< literal_t >(held->value).value;
        held_value = (held_value & ~out) | in;
    }
}

void close_list(parser_t &, bool, std::string_view);

// The store of value into the named bit-field m of the record at
// offset in a local, at the token at: field_store()'s on the place of
// the field's unit in the local, lvalue_at()'s, appended to the
// local's statements, its bytes among those stored, as a scalar's are
// (G20).  Initialization is no assignment, 6.7.11p12, so a const field
// takes its value.
void
field_local_store(parser_t &p,
                  const token_t &at,
                  const member_t &m,
                  long offset,
                  expr_t value,
                  destination_t &to)
{
    expr_t object { to.local->type, name_t { to.local } };
    const type_t t = unit_type(m.unit, as< int_t >(m.type).is_signed);
    expr_t place = lvalue_at(p, address_of(at, std::move(object)),
                             offset + m.offset, t, { });

    expr_t store = field_store(p, at, std::move(place), m, std::move(value));
    to.stores->push_back(stmt_t { expr_stmt_t { std::move(store) } });

    to.stored.emplace_back(offset + m.offset, m.unit);
    reach(to, offset + m.offset, m.unit);
}

// The initializer of a named bit-field m of the record at offset, its
// designation's '=' ahead when it was designated: alone or in one pair
// of braces, or {}, which is 0, p11, as for a scalar.  In an object of
// static storage it is a constant of the field's declared type,
// converted as by assignment, 6.7.11p12, field_data()'s; an address
// constant is no value of a bit-field's bits, the loader fills in no
// part of a byte, so it is an error.  In a local it is any value, the
// store field_local_store() makes, in the list's order (G13).
void
field_initializer(parser_t &p,
                  const member_t &m,
                  long offset,
                  destination_t &to,
                  bool designated)
{
    if (designated) {
        if (designation_ahead(p))
            refuse_designator(p);

        designation_end(p);
    }

    const token_t &brace = peek(p);
    const bool braced = accept(p, tok_t::lbrace);
    if (braced && accept(p, tok_t::rbrace)) {
        if (to.local)
            field_local_store(p, brace, m, offset,
                              expr_t { int_type(), literal_t { 0 } }, to);
        else
            field_data(to, m, offset, 0);

        return;
    }

    if (braced && peek(p).kind == tok_t::lbrace)
        fail(peek(p), "too many braces around scalar initializer");

    if (braced && designation_ahead(p))
        fail(peek(p), "initialization of non-aggregate type with a "
                      "designated initializer list");

    const token_t &start = peek(p);
    if (to.local) {
        expr_t e = assignment_expression(p);
        refuse_record_value(start, e.type);
        require_value(start, e);

        field_local_store(p, start, m, offset, std::move(e), to);
    }
    else {
        const initializer_t value = scalar_value(p, start, m.type);
        if (!std::holds_alternative< literal_t >(value))
            fail(start, "initializer element is not a compile-time constant");

        field_data(to, m, offset, std::get< literal_t >(value).value);
    }

    if (braced)
        close_list(p, true, "scalar");
}

// The index of the first member of r at i or after it that takes an
// initializer, no unnamed bit-field, 6.7.11p10, or the count of the
// members when none does.
std::size_t
initialized_member(const record_t &r, std::size_t i)
{
    while (i < r.members.size() && is_bit_field(r.members[i]) &&
           r.members[i].name.empty())
        ++i;

    return i;
}

// The members of a record at offset, from the initializers ahead in a
// list, 6.7.11p18: a struct's in declaration order, each from the next
// initializer, or from as many as it takes when that one does not
// begin with a brace, p21; a union's first member alone.  A
// designation moves the walk to the member it names, p18, p19: a
// struct's walk goes on from the next member, a union's ends, and a
// later initializer of a member overrides an earlier one, p20.  An
// anonymous member is given as a member of its record type is, its
// members being the record's, 6.7.3.2p15, as clang gives it; an
// unnamed bit-field takes no initializer, p10, and the walk passes over
// it.  An initializer is ahead.  The members end with the last,
// leaving the rest of the list to the enclosing aggregate or to the
// list's own check, or with the list.  Returns whether the walk ended
// past the last member the list may give.  A flexible array member
// takes an initializer only in GNU C, which rcc does not take yet; a
// named bit-field takes its value by field_initializer().
bool
member_values(
    parser_t &p, const record_t &r, long offset, destination_t &to, walk_t walk)
{
    const std::size_t end = r.members.size();

    std::size_t next = initialized_member(r, 0);
    bool designated = starts_designated(p, walk);
    if (!designated && next == end)
        return true;

    for (;;) {
        if (designated)
            next = member_designator(p, r);

        const member_t &m = r.members[next];

        if (is< array_t >(m.type) && as< array_t >(m.type).count < 0)
            not_yet(peek(p),
                    "initializer of flexible array member " + quoted(m.name));

        if (r.is_union)
            union_member(p, to, r, offset, next);

        if (is_bit_field(m))
            field_initializer(p, m, offset, to, designated);
        else if (designated)
            designated_subobject(p, m.type, offset + m.offset, to);
        else
            subobject_initializer(p, m.type, offset + m.offset, to);

        next = r.is_union ? end : initialized_member(r, next + 1);

        designated = takes_designation(p, walk);
        if (!designated && !goes_on(p, next == end))
            return next == end;
    }
}

// The subobject of type t at offset that a designator named, 6.7.11p19:
// a designator after it names a subobject of it, and a walk entered
// there takes the rest of the designation and goes on in t, p18; else
// the designation's '=' and the subobject's initializer.  A designator
// names no subobject of a scalar.
void
designated_subobject(parser_t &p,
                     const type_t &t,
                     long offset,
                     destination_t &to)
{
    if (designation_ahead(p) && is< array_t >(t)) {
        elements(p, as< array_t >(t), offset, to, walk_t::designated);
        return;
    }

    if (designation_ahead(p) && is< record_type_t >(t)) {
        member_values(p, *as< record_type_t >(t).record, offset, to,
                      walk_t::designated);
        return;
    }

    if (designation_ahead(p))
        refuse_designator(p);

    designation_end(p);
    subobject_initializer(p, t, offset, to);
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

    if (offset < to.reached)
        overridden(p, brace, to, offset, size_of(t), align_of(t));

    if (peek(p).kind == tok_t::rbrace) {
        if (is< array_t >(t) && as< array_t >(t).count < 0)
            fail(brace, "empty initializer for an array of unknown size");

        advance(p);

        return 0;
    }

    if (is< record_type_t >(t)) {
        const record_t &r = *as< record_type_t >(t).record;
        const bool full = member_values(p, r, offset, to, walk_t::braced);
        close_list(p, full, r.is_union ? "union" : "struct");

        return 0;
    }

    if (!is< array_t >(t)) {
        if (peek(p).kind == tok_t::lbrace)
            fail(peek(p), "too many braces around scalar initializer");

        if (designation_ahead(p))
            fail(peek(p), "initialization of non-aggregate type with a "
                          "designated initializer list");

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
    const long count = elements(p, array, offset, to, walk_t::braced);
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
    if (peek(p).kind == tok_t::lbrace)
        return braced_initializer(p, t, offset, to);

    if (is< record_type_t >(t)) {
        member_values(p, *as< record_type_t >(t).record, offset, to,
                      walk_t::elided);
        return 0;
    }

    if (!is< array_t >(t)) {
        scalar_subobject(p, peek(p), t, offset, to);
        return 0;
    }

    if (string_ahead(p, t))
        return listed_string(p, t, offset, to);

    return elements(p, as< array_t >(t), offset, to, walk_t::elided);
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

// The subobjects a list gave values, in the order of their offsets,
// which a designation may have given out of it, 6.7.11p19; none
// overlaps another, since a later value overrides any earlier one in
// its bytes, p20.
void
in_offset_order(std::vector< subobject_t > &values)
{
    std::stable_sort(values.begin(), values.end(),
                     [](const subobject_t &a, const subobject_t &b) {
                         return a.offset < b.offset;
                     });
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

    if (peek(p).kind == tok_t::lbrace) {
        const long count = braced_initializer(p, t, 0, to);
        in_offset_order(values);

        return count;
    }

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
// where it is declared is an error with an initializer, 6.7.11p4; a
// tentative definition takes the record as it is at the end of the
// unit, 6.9.3p2, which require_completed_definitions() checks then, of
// internal linkage as of external, as an array of unknown size is.
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
        if (initialized)
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
// unit, so static struct s x; struct s { int i; }; defines an object of
// the struct, and static int a[]; static int a[3]; declares the int [3]
// it does elsewhere, 6.2.7p5, and an array no declaration sizes is an
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

// Whether a declaration of sym at file scope made it an alias.
bool
is_alias(const parser_t &p, const symbol_t *sym)
{
    const auto &names = p.scopes.front().names;
    for (const pending_alias_t &a : p.aliases)
        if (names.at(a.name->text) == sym)
            return true;

    return false;
}

// 6.9.1p3: a name of internal linkage that an expression uses has its
// external definition in the unit.  Without one the assembler would
// leave its symbol undefined, and the linker would bind it to another
// file's symbol of the name, which is not the program.  Only a
// function can lack one, since every object of internal linkage but an
// alias has a tentative definition at least; a static alias, whose
// symbol the assembler gives the target's address, is its definition.  A use of a system header's static
// inline function before its definition, which rcc does not compile,
// is not yet, in the function's words.  The first use is the one
// refused.
void
require_internal_definitions(const parser_t &p)
{
    for (const auto &[sym, at] : p.internal_uses) {
        if (defines(p, sym) || is_alias(p, sym))
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
// read, and each added to the unit with its target: the alias has no
// assembler name, is defined by no declaration, as a second alias of it
// would define it again, and is of external linkage where it is weak,
// as GNU C has it, since a weak symbol of internal linkage is one no
// other file could stand in for.
void
resolve_aliases(parser_t &p)
{
    for (std::size_t i = 0; i < p.aliases.size(); ++i) {
        const pending_alias_t &a = p.aliases[i];
        const symbol_t &sym = *p.scopes.front().names.at(a.name->text);
        const std::string alias = "alias " + quoted(a.name->text);

        if (a.weak && sym.linkage != linkage_t::external)
            fail(*a.name, "weak " + alias + " of internal linkage");

        if (!sym.asm_name.empty())
            not_yet(*a.name, alias + " with an assembler name");

        bool defined = defines(p, &sym);
        for (std::size_t k = 0; k < i; ++k)
            defined |= p.aliases[k].name->text == a.name->text;

        if (defined)
            fail(*a.name, "redefinition of " + quoted(a.name->text));

        const symbol_t &target = alias_of(p, a, alias_target(p, *a.attribute));
        p.unit.aliases.push_back(alias_t { &sym, &target, a.weak });
    }
}

// A static inline function that an expression or an alias names, before
// its definition or after it, is not droppable: a use of it would name
// a symbol c1 did not write, so c1's refusal of its body stops the file
// as any other's does.  A name in a body the parser stopped in counts,
// as does a call of the function in its own body.
void
keep_named_functions(parser_t &p)
{
    for (function_decl_t &f : p.unit.functions) {
        const bool aliased =
            std::ranges::any_of(p.unit.aliases, [&f](const alias_t &a) {
                return a.target == f.sym;
            });

        if (p.used.contains(f.sym) || aliased)
            f.droppable = false;
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

    symbol_t *sym = earlier_declaration(p, *d.name, d.type, d.qualifiers,
                                        storage_t::global);
    const linkage_t linkage = linkage_of(spec, sym, false);
    refuse_visibility(*d.name, linkage);

    if (!sym) {
        sym = &added(p, d.name->text, d.type, storage_t::global);
        sym->linkage = linkage;
        sym->qualifiers = d.qualifiers;
    }

    require_linkage(*d.name, *sym, linkage);
    record_tail(p, *sym, tail);

    const bool initialized = peek(p).kind == tok_t::assign;
    if (!initialized &&
        (declared(spec, tok_t::kw_extern) || tail.markings.alias))
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

// A '*' in a parameter's brackets other than those it is adjusted from
// makes a variable length array of unspecified size behind its
// pointer, 6.7.7.3p4, a pointer to a variable length array, which is
// not yet, as one of a size is there.  In a parameter of a function
// definition it is an error, 6.9.2p6, so a list that may be a
// definition's, the list after a name at file scope, keeps its stars
// for the declaration to refuse, external_declarator(), or the
// definition, which errs (require_definition_parameters()); every other
// list refuses its own at its ')'.
void
refuse_inner_stars(const std::vector< parameter_t > &params)
{
    for (const parameter_t &param : params)
        if (param.inner_star)
            not_yet(*param.inner_star, "pointer to variable length array");
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
    if (d.params)
        refuse_inner_stars(*d.params);

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
        refuse_unfolded(
            start, e, "static assertion is not an integer constant expression");

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

    declarator_t d =
        declarator(p, base, spec.qualifiers, naming_t::required, typedef_);

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
        external_declarator(
            p, spec,
            declarator(p, base, spec.qualifiers, naming_t::required, typedef_));

    expect(p, tok_t::semicolon, "expected ';' after top level declarator");
}

// Whether a goto at the token from would jump into a statement
// expression of the function: one the label at the token to stands in
// and the goto outside it.
bool
jumps_into(const parser_t &p, const token_t &to, const token_t &from)
{
    const auto inside = [](const auto &span, const token_t &t) {
        return span.first < &t && &t < span.second;
    };

    for (const auto &span : p.statement_expression_spans)
        if (inside(span, to) && !inside(span, from))
            return true;

    return false;
}

// The first variable length array in scope at the label to and not at
// the goto from, which the goto would jump into the scope of, 6.8.7.2p1,
// or null.
const token_t *
jumped_array(const parser_t &p, const token_t &to, const token_t &from)
{
    const auto label = p.jump_scopes.find(&to);
    if (label == p.jump_scopes.end())
        return nullptr;

    const auto jump = p.jump_scopes.find(&from);
    for (const token_t *array : label->second)
        if (jump == p.jump_scopes.end() ||
            std::ranges::find(jump->second, array) == jump->second.end())
            return array;

    return nullptr;
}

// Each goto of the function names a label the function defines,
// 6.8.7.2p1, before the goto or after it, since a label has function
// scope, 6.2.1p3, and stands inside each statement expression its label
// stands in: GNU C takes no jump into one, as clang has it.  Nor does
// it stand outside the scope of a variable length array the label is
// in, p1.  So the check waits for the function's end, and the first
// goto that names none, or jumps in, is the error.
void
require_labels(const parser_t &p)
{
    for (const token_t *name : p.gotos) {
        const auto label = p.labels.find(name->text);
        if (label == p.labels.end())
            fail(*name,
                 "label " + quoted(name->text) + " used but not defined");

        if (jumps_into(p, *label->second, *name))
            fail(*name, "goto " + quoted(name->text) +
                            " from outside the label's statement expression");

        if (const token_t *array = jumped_array(p, *label->second, *name))
            fail(*name, "goto " + quoted(name->text) +
                            " into the scope of variable length array " +
                            quoted(array->text));
    }
}

// The count of the variable length arrays in scope at each label of the
// function that stands in the scope of one, by its name, for c1: a goto
// leaves the scope of the arrays past the count, since no goto jumps
// into the scope of one, require_labels(), so those at its label are
// the first of those at the goto (shape.md chapter 5).
std::unordered_map< std::string, std::size_t >
label_arrays(const parser_t &p)
{
    std::unordered_map< std::string, std::size_t > counts;
    for (const auto &[name, label] : p.labels)
        if (const auto at = p.jump_scopes.find(label);
            at != p.jump_scopes.end())
            counts.emplace(name, at->second.size());

    return counts;
}

// A parameter of a definition, declared already in the scope of its
// list, which is open again as the body's, takes its register in the
// order of the parameters, as a local does.  An aarch64 va_list arrives
// as the address of the caller's copy, is_list_by_reference(): the
// symbol is that pointer, and its name reads as the list it points to,
// qualified as the parameter is.
const symbol_t *
defined_parameter(parser_t &p, const parameter_t &param)
{
    symbol_t &sym = *p.scopes.back().names.at(param.name->text);

    sym.declared_register = param.in_register;
    sym.register_index = register_index(p, *param.name, param.in_register);

    if (is_list_by_reference(p, sym.type)) {
        sym.type = make_pointer(sym.type, sym.qualifiers);
        sym.qualifiers = { };
        sym.by_reference = true;
    }

    return &sym;
}

// The parameters of a definition, defined_parameter()'s, which
// require_definition_parameters() and require_definable_head() checked.
std::vector< const symbol_t * >
defined_parameters(parser_t &p, const declarator_t &d)
{
    std::vector< const symbol_t * > params;
    for (const parameter_t &param : *d.params)
        params.push_back(defined_parameter(p, param));

    return params;
}

// The errors of a definition's parameters: each is named, and no array
// of unspecified size, [*], is part of the declaration of one, 6.9.2p6,
// in the brackets it is adjusted from or in any other.  They are
// checked before any refusal of the not-yet kind in the head, so that
// an error stays the error it is.
void
require_definition_parameters(const declarator_t &d)
{
    for (const parameter_t &param : *d.params) {
        if (!param.name)
            fail(*param.start, "parameter name omitted");

        if (const token_t *star = param.star ? param.star : param.inner_star)
            fail(*star, "'*' as an array size in a function definition");
    }
}

// The refusals of the not-yet kind in a definition's head.  A function
// a marking attribute marked, on a declaration, among the definition's
// specifiers or on a parameter, is one rcc would have to define as the
// attribute says.  The result and each parameter are objects the
// definition makes, so none may be of a not-yet type, nor of a record,
// which returns and passes by value in no rule rcc has yet but the
// aarch64 va_list's.
void
require_definable_head(const parser_t &p,
                       const symbol_t &sym,
                       const declarator_t &d)
{
    if (!sym.marking.empty())
        not_yet(*d.name, sym.marking);

    const type_t &result = *as< function_t >(d.type).result;
    require_honoured(*d.name, result);

    if (const record_t *r = record_in(result))
        not_yet(*d.name, record_words(*r) + " returned by value");

    for (const parameter_t &param : *d.params) {
        require_honoured(*param.name, param.type);

        const record_t *r = record_in(param.type);
        if (r && !is_list_by_reference(p, param.type))
            not_yet(*param.name, record_words(*r) + " passed by value");
    }
}

// The words of the first refusal of the not-yet kind in the head of a
// static inline definition of the program's own text, or nothing when
// the head has none: such a function is kept uncompiled, as for its
// body, inline_body()'s.  The checks change nothing of the parse, so
// nothing is undone.
std::optional< std::string >
inline_head(const parser_t &p, const symbol_t &sym, const declarator_t &d)
{
    try {
        require_definable_head(p, sym, d);
    }
    catch (const not_yet_error_t &e) {
        return e.construct;
    }

    return std::nullopt;
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

// A function's body, its braces included, skipped unread, for a
// definition that is not compiled.
void
skip_body(parser_t &p)
{
    expect(p, tok_t::lbrace, "expected '{' before function body");
    skip_to_closing_brace(p);
    expect(p, tok_t::rbrace, "expected '}' at end of function body");
}

// Whether the unit has defined the function: by a body, or by one it
// stopped in or skipped, which the flag uncompiled records whatever
// words the symbol keeps.  A second definition is then a redefinition,
// 6.9.1p3.
bool
is_defined(const parser_t &p, std::size_t index)
{
    const function_decl_t &f = p.unit.functions[index];

    return f.body || f.uncompiled;
}

// A function's body, its braces included, its items parsed into the
// scope of the parameter list, which the caller opened again, after the
// statements entry holds, parameter_sizes()'s.
block_t
function_body(parser_t &p, std::vector< stmt_t > entry)
{
    block_t body { std::move(entry) };
    p.temporaries.emplace();
    expect(p, tok_t::lbrace, "expected '{' before function body");
    block_item_list(p, body);
    expect(p, tok_t::rbrace, "expected '}' at end of function body");
    declare_temporaries(p, body);
    drop_identities(body);

    return body;
}

// The body of a static inline definition of the program's own text, or
// nothing when it stops at a refusal of the not-yet kind, whose words
// go to missing.  A header defines such a function for each file that
// includes it, and a file that never calls it does not pay for its
// body, as for a system header's.  Only the refusal of the not-yet
// kind is caught, so an error before it stays the error it is; the
// parse resumes at the body's closing brace, and the text after the
// refusal is not read.  What the body changed of the parse is undone:
// the scopes and the statements it opened, the switches, loops and
// temporaries, and the static objects it declared, which nothing
// outside it names.
std::optional< block_t >
inline_body(parser_t &p, std::string &missing, std::vector< stmt_t > entry)
{
    const std::size_t start = p.pos;
    const std::size_t depth = p.scopes.size();
    const std::size_t objects = p.unit.objects.size();
    const std::size_t switches = p.switches.size();
    const int breaks = p.break_targets;
    const int continues = p.continue_targets;

    try {
        return function_body(p, std::move(entry));
    }
    catch (const not_yet_error_t &e) {
        missing = e.construct;
        p.pos = start + 1;
        skip_to_closing_brace(p);
        expect(p, tok_t::rbrace, "expected '}' at end of function body");
        p.scopes.resize(depth);
        p.unit.objects.resize(objects);
        p.switches.erase(p.switches.begin() + switches, p.switches.end());
        p.break_targets = breaks;
        p.continue_targets = continues;
        p.temporaries.reset();
    }

    return std::nullopt;
}

// The sizes of the arrays a definition's parameters are adjusted from
// that are no constants, each an expression statement for the head of
// the body, in the order of the parameters.  The parameters have block
// scope, 6.2.1p4, so the sizes are evaluated on entry, 6.9.2p11, for
// their effects alone, since each parameter is the pointer; a constant
// size has none and writes nothing.
std::vector< stmt_t >
parameter_sizes(declarator_t &d)
{
    std::vector< stmt_t > sizes;
    for (parameter_t &param : *d.params)
        if (param.size)
            sizes.push_back(stmt_t { expr_stmt_t { std::move(param.size) } });

    return sizes;
}

// A static inline definition whose head needs what rcc does not
// compile yet, inline_head()'s: its body is skipped and the function
// kept uncompiled, and an expression that names it, before the
// definition or after it, is refused by the head's words, which the
// symbol keeps, or by a marking attribute's, which it keeps already.
void
uncompiled_head(parser_t &p,
                std::size_t index,
                const declarator_t &d,
                const std::string &missing)
{
    skip_body(p);

    std::string &marking = p.scopes.back().names.at(d.name->text)->marking;
    if (marking.empty())
        marking = missing;

    p.unit.functions[index].uncompiled = true;
}

// The parameter type list and the body are one block, 6.9.2p10,
// 6.8.3p2, so a local cannot redeclare a parameter there: the scope of
// the list that makes the name a function, with its parameters and its
// tags, is opened again and the body's items are parsed into it
// directly.  The head is checked first, its errors and then its
// refusals of the not-yet kind, require_definable_head()'s; the
// parameters are defined_parameters()'s.  function and statics start
// over, for the labels of the static objects the body declares, and
// labels, gotos and case_labels, for the body's own labels, which no
// other function sees; and variadic, named and va_area, for the body's
// va_start.  The body's identities are dropped once it is whole.  A
// static inline function whose head or body needs a construct rcc does
// not compile yet is kept uncompiled, uncompiled_head()'s or
// inline_body()'s: an expression that names it, before the definition
// or after it, is refused by the words of what it needs, which the
// symbol keeps.  One whose body is whole is droppable, for c1 to write
// nothing for it if it refuses the body, with the static objects the
// body declares, unless keep_named_functions() finds it named.
void
function_definition(parser_t &p, const specifiers_t &spec, declarator_t &d)
{
    const std::size_t index = declare_function(
        p, spec, d, { nullptr, { }, spec.marking, spec.markings });
    if (is_defined(p, index))
        fail(*d.name, "redefinition of " + quoted(d.name->text));

    require_definition_parameters(d);

    const symbol_t &sym = *p.unit.functions[index].sym;
    const bool inline_ =
        declared(spec, tok_t::kw_static) && spec.inline_specifier;
    if (!inline_)
        require_definable_head(p, sym, d);
    else if (const std::optional< std::string > head = inline_head(p, sym, d)) {
        uncompiled_head(p, index, d, *head);
        return;
    }

    p.result = *as< function_t >(d.type).result;
    p.registers = 0;
    p.function = d.name->text;
    p.variadic = as< function_t >(d.type).variadic;
    p.named = d.params->size();
    p.va_area = nullptr;
    p.statics.clear();
    p.labels.clear();
    p.gotos.clear();
    p.jump_scopes.clear();
    p.statement_expression_spans.clear();
    p.case_labels = 0;
    p.scopes.push_back(d.scope);

    std::vector< const symbol_t * > params = defined_parameters(p, d);

    const std::size_t statics = p.unit.objects.size();
    std::string missing;
    std::vector< stmt_t > sizes = parameter_sizes(d);
    std::optional< block_t > body =
        inline_ ? inline_body(p, missing, std::move(sizes)) :
                  function_body(p, std::move(sizes));

    p.scopes.pop_back();

    function_decl_t &f = p.unit.functions[index];
    p.variadic = false;

    if (!body) {
        p.scopes.back().names.at(d.name->text)->marking =
            missing + " in the body of static function " + quoted(d.name->text);
        f.uncompiled = true;
        return;
    }

    require_labels(p);

    f.params = std::move(params);
    f.body = std::move(body);
    f.registers = p.registers;
    f.va_area = p.va_area;
    f.droppable = inline_;
    f.statics_begin = statics;
    f.statics_end = p.unit.objects.size();
    f.label_arrays = label_arrays(p);
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
// the unit.  The function is uncompiled, which is_defined() reads, so a
// second definition is a redefinition whatever words the symbol keeps,
// a marking attribute's among them.  Its parameters are a definition's
// and keep their errors, require_definition_parameters()'s.
void
header_function_definition(parser_t &p,
                           const specifiers_t &spec,
                           const declarator_t &d)
{
    const std::size_t index = declare_function(
        p, spec, d, { nullptr, { }, spec.marking, spec.markings });

    if (is_defined(p, index))
        fail(*d.name, "redefinition of " + quoted(d.name->text));

    require_definition_parameters(d);

    p.unit.functions[index].uncompiled = true;

    symbol_t &sym = *p.scopes.back().names.at(d.name->text);
    if (sym.marking.empty())
        sym.marking = skipped_body_words(d.name->text);

    skip_body(p);
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

// A const or a volatile ahead, taken into the qualifiers, once or more,
// 6.7.4.1p6; false for any other token.
bool
accept_qualifier(parser_t &p, qualifiers_t &qualifiers)
{
    if (accept(p, tok_t::kw_const)) {
        qualifiers.const_ = true;
        return true;
    }

    if (accept(p, tok_t::kw_volatile)) {
        qualifiers.volatile_ = true;
        return true;
    }

    return false;
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
// The type keeps its kind: char and signed char are two kinds, long
// and long long two more.  Each keyword is checked against the ones
// before it, so an error names the keyword that breaks the set.  const
// and volatile may stand anywhere among them, once or more, 6.7.4.1p6,
// and are kept beside the type, which is the machine's view, joined
// with those of a typedef name's type, p6.  volatile asks that each
// access the source writes be made, p8, and c1 writes every access of
// every object, an unused one included, at the object's width, so the
// qualifier asks for nothing c1 does not do.  _Atomic as a qualifier
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
    qualifiers_t qualifiers;

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

        const bool is_signed = n_unsigned == 0;
        const auto of = [&](int width, int_kind_t s, int_kind_t u) {
            return make_int(width, is_signed, is_signed ? s : u);
        };

        if (n_char)
            return of(8, int_kind_t::signed_char, int_kind_t::unsigned_char);

        if (n_short)
            return of(16, int_kind_t::short_, int_kind_t::unsigned_short);

        if (n_long == 2)
            return of(64, int_kind_t::long_long,
                      int_kind_t::unsigned_long_long);

        if (n_long)
            return of(64, int_kind_t::long_, int_kind_t::unsigned_long);

        return of(32, int_kind_t::int_, int_kind_t::unsigned_);
    };

    for (;;) {
        const token_t &t = peek(p);

        if (accept_qualifier(p, qualifiers))
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
            qualifiers = qualifiers | named_qualifiers(p, t);

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
                type = make_not_yet(identity_of(p, "_Atomic", type),
                                    "type qualifier " +
                                        quoted(atomic_qualifier->text));

            return { type,
                     storage_class,
                     tagged,
                     function_specifier,
                     inline_specifier,
                     marking,
                     markings,
                     qualifiers };
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

// A member after a flexible array member, a bit-field and an anonymous
// one among them, is an error: the flexible array member is the last,
// 6.7.3.2p3.
void
refuse_member_after_flexible(const member_list_t &list)
{
    if (list.flexible)
        refuse_flexible(list,
                        "flexible array member " + quoted(list.flexible->text) +
                            " not at the end of " + record_words(list.record));
}

// The width of a bit-field after its ':', an integer constant
// expression, 6.7.3.2p4, of a bit-field whose type is an integer type,
// p5, _Bool among them, or one rcc does not use yet.  The width is not
// negative, does not exceed the type's, 1 for _Bool by p4's footnote
// and its bits for any other, and is zero only where the field has no
// name, p4; a type rcc does not use yet has no width to exceed.  name
// is the field's, or null.  Attributes after the width are said of the
// member, and change nothing that is laid out.  A bit-field after a
// flexible array member is an error, as any member there is.  Returns
// the width.
int
bit_field(parser_t &p,
          member_list_t &list,
          const type_t &t,
          const token_t *name)
{
    refuse_member_after_flexible(list);

    const token_t &colon = advance(p);
    if (!is< int_t >(t) && !is< not_yet_t >(t))
        fail(colon, "bit-field of a type that is not an integer type");

    const token_t &start = peek(p);
    const expr_t width = rvalue(start, conditional_expression(p));
    if (!is_integer_constant(width))
        refuse_unfolded(
            start, width,
            "bit-field width is not an integer constant expression");

    const std::string field =
        name ? "bit-field " + quoted(name->text) : "unnamed bit-field";
    const exact_t bits = exact_value(width);
    if (bits < 0)
        fail(start, field + " of negative width");

    if (bits == 0 && name)
        fail(start, "named " + field + " of zero width");

    const int most = is_bool(t) ? 1 : is< int_t >(t) ? as< int_t >(t).width : 0;
    if (most > 0 && bits > most)
        fail(start,
             field + " of " +
                 std::to_string(static_cast< unsigned long long >(bits)) +
                 " bits, wider than its type's " + std::to_string(most));

    attribute_specifier_list(p);

    return int(std::min(bits, exact_t(std::numeric_limits< int >::max())));
}

// A member's type: a complete object type, 6.7.3.2p11, so neither a
// function, p3, nor void, nor an incomplete record, the record being
// defined among them, which holds no instance of itself, p3.  An array
// of unknown size is a flexible array member, p20, which stands last,
// p3, as member_list() checks; in a union it is an error, p3.  zero is
// the size of such an array written as zero, or null.  A record that
// holds a flexible array member is no member of a struct, p3, an error
// in the program's own text; a system header's struct is read as it is
// and laid out.  A union with such a member holds one too.  A type rcc
// does not use yet, a record that holds a construct rcc does not lay
// out among them, leaves the record without its layout, refused by the
// member type's words, but for one is_sized() says is laid out.
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

        r.flexible = true;
    }

    if (const record_t *inner = flexible_record(t)) {
        if (r.is_union)
            r.flexible = true;
        else if (!name.where.system_header)
            fail(name, "member " + quoted(name.text) + " of " +
                           record_words(r) + " has type " +
                           flexible_words(*inner));
    }

    if (const std::string *words = not_yet_in(t); words && !is_sized(t))
        mark_not_yet(r, *words);
}

// The member of the name in a record, at its offset from the start of
// the record, or none: a member the record declares, or one of an
// anonymous member's, at any depth, at the anonymous member's offset
// plus its own, and qualified as it is too, since its members are the
// record's, 6.7.3.2p15, reached through it.  A named bit-field is found
// as any member is; an unnamed one, p14, names nothing.
std::optional< member_t >
member_named(const record_t &r, std::string_view name)
{
    for (const member_t &m : r.members) {
        if (m.name == name)
            return m;

        if (!is_anonymous(m))
            continue;

        auto inner = member_named(*as< record_type_t >(m.type).record, name);
        if (inner) {
            inner->offset += m.offset;
            inner->qualifiers = inner->qualifiers | m.qualifiers;
            return inner;
        }
    }

    return std::nullopt;
}

// The names of a record's members, its named bit-fields and its
// anonymous members' at any depth among them, 6.7.3.2p15.
std::vector< std::string >
member_names(const record_t &r)
{
    std::vector< std::string > names;

    for (const member_t &m : r.members) {
        if (!m.name.empty()) {
            names.push_back(m.name);
            continue;
        }

        if (!is_anonymous(m))
            continue;

        const auto inner = member_names(*as< record_type_t >(m.type).record);
        names.insert(names.end(), inner.begin(), inner.end());
    }

    return names;
}

// A named member, its type checked and its name one the record has not
// declared yet, its anonymous members' and its bit-fields' among them,
// 6.7.3.2p1, p15 and 6.2.3p1: each record is a name space of its own.  A member after a
// flexible array member is an error, p3.  zero is the member's own
// array's size written as zero, or null.  qualifiers are those its
// declaration says of it.
void
add_member(member_list_t &list,
           const token_t &name,
           const type_t &t,
           qualifiers_t qualifiers,
           const token_t *zero)
{
    record_t &r = list.record;

    refuse_member_after_flexible(list);

    require_member_type(name, t, zero, list);

    if (member_named(r, name.text))
        fail(name, "duplicate member " + quoted(name.text));

    r.members.push_back(member_t { std::string(name.text), t, 0, qualifiers });
}

// An anonymous member, 6.7.3.2p15: a member of no name and of the
// record of the specifier, laid out as a member of its type is, as
// clang lays it out, so its own layout is kept.  Its members are the
// enclosing record's: each name is one the record has not declared
// yet, refused at the ';', since the member's own list was read before
// it was known to be anonymous, and each counts among the record's
// named members, p10.  A record rcc does not lay out leaves the
// enclosing one without its layout, by its words.  qualifiers are those
// the specifiers say of the member.
void
anonymous_member(const token_t &semicolon,
                 member_list_t &list,
                 const type_t &t,
                 qualifiers_t qualifiers)
{
    record_t &r = list.record;
    const record_t &inner = *as< record_type_t >(t).record;

    for (const std::string &name : member_names(inner)) {
        if (member_named(r, name))
            fail(semicolon, "duplicate member " + quoted(name));

        ++list.named;
    }

    if (const std::string *words = not_yet_in(t))
        mark_not_yet(r, *words);

    r.members.push_back(member_t { { }, t, 0, qualifiers });
}

// A bit-field of the width, named or not, 6.7.3.2p11, p14: a member,
// kept for the layout, lay_out()'s.  A named one's name is one the
// record has not declared yet, its anonymous members' and its
// bit-fields' among them, p1, p15 and 6.2.3p1, and qualifiers are those
// its declaration says of it.  A field of a type rcc does not use yet
// leaves the record without its layout, by the type's words, as a
// member of the type does.
void
add_bit_field(member_list_t &list,
              const token_t *name,
              const type_t &t,
              qualifiers_t qualifiers,
              int width)
{
    record_t &r = list.record;

    if (name && member_named(r, name->text))
        fail(*name, "duplicate member " + quoted(name->text));

    if (const std::string *words = not_yet_in(t))
        mark_not_yet(r, *words);

    const std::string named = name ? std::string(name->text) : std::string();
    r.members.push_back(member_t { named, t, 0, qualifiers, width });
}

// One member declarator, 6.7.3.2p1: a declarator, which names the
// member, then a bit-field's width or none; or a width alone, an
// unnamed bit-field, p14.  An attribute after the declarator is said of
// the member, and one that marks, aligned and packed among them,
// changes its layout, which rcc does not honour yet.  A member is no
// ordinary identifier, so it has no variable length array type,
// 6.7.7.3p2.
void
member_declarator(parser_t &p, const specifiers_t &spec, member_list_t &list)
{
    if (peek(p).kind == tok_t::colon) {
        const int width = bit_field(p, list, spec.type, nullptr);
        add_bit_field(list, nullptr, spec.type, { }, width);
        return;
    }

    const declarator_t d =
        declarator(p, spec.type, spec.qualifiers, naming_t::member, false);
    if (is_variable_array(d.type))
        fail(*d.name, "member " + quoted(d.name->text) +
                          " of a variable length array type");

    ++list.named;

    if (const token_t *marking = attribute_specifier_list(p))
        mark_not_yet(list.record, attribute_words(*marking) + " in " +
                                      record_words(list.record));

    if (peek(p).kind == tok_t::colon) {
        const int width = bit_field(p, list, d.type, d.name);
        add_bit_field(list, d.name, d.type, d.qualifiers, width);
        return;
    }

    add_member(list, *d.name, d.type, d.qualifiers, d.zero);
}

// A member declaration with no declarator, 6.7.3.2p2: a struct or a
// union specifier without a tag declares an anonymous member, p15,
// which is no member of a struct when it holds a flexible array
// member, p3, as require_member_type() says of a named one, and none
// after a flexible array member, as any member there.  Any other
// struct, union or enum specifier declares its tag or its constants
// alone, as at file scope; one of no specifier of the kind declares
// nothing, an error.
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
        refuse_member_after_flexible(list);

    if (inner && inner->tag.empty() && inner->flexible) {
        if (list.record.is_union)
            list.record.flexible = true;
        else if (!semicolon.where.system_header)
            fail(semicolon, "anonymous member of " + record_words(list.record) +
                                " has type " + flexible_words(*inner));
    }

    if (inner && inner->tag.empty())
        anonymous_member(semicolon, list, spec.type, spec.qualifiers);
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

// The next free bit of a struct as its layout goes: the byte, and the
// count of its bits, 0 to 7, that the bit-fields before took.
struct next_bit_t
{
    long byte = 0;
    int bit = 0;
};

// The access unit of a named bit-field, its container's offset and its
// first bit in the container in hand: the narrowest unit of 1, 2, 4 or
// 8 bytes, at a multiple of its size from the record's start, that
// holds the field's bits.  Its offset, its size and the field's first
// bit in it become the member's (docs/shape.md chapter 3).  Units of
// sizes that are powers of two nest, and the container is one, so the
// unit lies in the container.
void
access_unit(member_t &m)
{
    const int size = int(size_of(m.type));

    for (int unit = 1; unit <= size; unit *= 2) {
        const int start = m.bit / (8 * unit) * unit;
        if (m.bit + m.width <= (start + unit) * 8) {
            m.offset += start;
            m.unit = unit;
            m.bit -= start * 8;
            return;
        }
    }

    throw std::logic_error("access_unit: a bit-field past its container");
}

// The place of a bit-field, 6.7.3.2p13, p14, in its container, the unit
// of its declared type, of the type's size at a multiple of it from the
// record's start: the next free bit, next, where the rest of that
// container holds the field's width, and the next container's first
// bit where it does not, so no field straddles two; a width of 0 moves
// to the next container's first bit, unless next is one.  Fields fill
// a container from its lowest bit up.  In a union next stays at the
// record's start, so every field is at bit 0, p18.  A named field
// takes its access unit, access_unit()'s.  Returns the end of the
// field's bits, in bytes; close is the list's '}', where a record too
// large for a size is refused.
long
bit_field_place(const token_t &close,
                const record_t &r,
                member_t &m,
                next_bit_t &next)
{
    constexpr long largest = std::numeric_limits< long >::max();
    const long size = size_of(m.type);

    if (next.byte > largest - 2 * size)
        fail(close, record_words(r) + " is too large");

    long container = next.byte / size * size;
    int bit = int(next.byte - container) * 8 + next.bit;

    const bool moves = m.width == 0 ? bit > 0 : bit + m.width > size * 8;
    if (moves) {
        container += size;
        bit = 0;
    }

    m.offset = container;
    m.bit = bit;
    if (!m.name.empty())
        access_unit(m);

    if (!r.is_union)
        next = { container + (bit + m.width) / 8, (bit + m.width) % 8 };

    return container + (bit + m.width + 7) / 8;
}

// The layout of a record its list completed, as clang lays it out on
// the target: each member of a struct at the offset so far rounded up
// to the member's alignment, in the order declared, 6.7.3.2p16, p17;
// each member of a union at 0, p18; the record's alignment its
// members' largest, and its size the end of its members rounded up to
// that alignment, p19, so that each element of an array of it is
// aligned.  A flexible array member stands at its element's alignment
// past the other members and adds no size, p20, as clang lays it out.
// A bit-field takes its bits, bit_field_place()'s, and the member after
// it the next byte past them, at its own alignment, so it may lie in
// the field's container; the end of a field is the end of its last
// bit.  A field's declared type counts in the record's alignment, an
// unnamed field's only where the target says so.  close is the list's
// '}', where a record too large for a size is refused.
void
lay_out(const token_t &close, record_t &r, const target_t &target)
{
    constexpr long largest = std::numeric_limits< long >::max();

    next_bit_t next;
    long end = 0;
    int alignment = 1;

    for (member_t &m : r.members) {
        if (is_bit_field(m)) {
            end = std::max(end, bit_field_place(close, r, m, next));
            if (!m.name.empty() || target.unnamed_field_aligns)
                alignment = std::max(alignment, align_of(m.type));

            continue;
        }

        const bool flexible =
            is< array_t >(m.type) && as< array_t >(m.type).count < 0;
        const long size = flexible ? 0 : size_of(m.type);
        const int align = align_of(m.type);

        if (end > largest - align)
            fail(close, record_words(r) + " is too large");

        m.offset = r.is_union ? 0 : (end + align - 1) / align * align;

        if (size > largest - m.offset)
            fail(close, record_words(r) + " is too large");

        if (!r.is_union)
            next = { m.offset + size, 0 };

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
        return { { "gp_offset", make_int(32, false, int_kind_t::unsigned_), 0 },
                 { "fp_offset", make_int(32, false, int_kind_t::unsigned_), 0 },
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
    lay_out(p.tokens.front(), r, p.target);

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
        lay_out(close, r, p.target);

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
            refuse_unfolded(
                start, e,
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

    const bool is_signed = as< int_t >(previous->type).is_signed;
    const type_t wider =
        make_int(64, is_signed,
                 is_signed ? int_kind_t::long_ : int_kind_t::unsigned_long);
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

    const type_t types[] = {
        make_int(32, negative,
                 negative ? int_kind_t::int_ : int_kind_t::unsigned_),
        make_int(64, negative,
                 negative ? int_kind_t::long_ : int_kind_t::unsigned_long)
    };

    for (const type_t &t : types)
        if (holds(t, least.value) && holds(t, most.value))
            return t;

    const std::string what =
        at.kind == tok_t::identifier ? "enum " + quoted(at.text) : "the enum";
    fail(at, "no integer type holds every value of " + what);
}

// The enumerated type a list defines, 6.2.5p21: the machine's view and
// the kind of its compatible integer type, and an identity of its own.
type_t
enumerated(parser_t &p, const type_t &compatible)
{
    const int_t &t = as< int_t >(compatible);

    return make_int(t.width, t.is_signed, t.kind, p.identities++);
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
// and p6 to p8, each need a diagnostic of their own.  A ':' before
// anything but a type's specifiers is the context's, as a generic
// association's in _Generic (e, enum color: 1).
void
refuse_fixed_underlying_type(const parser_t &p)
{
    if (peek(p).kind == tok_t::colon && is_declaration_start(p, 1))
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
// compatible with it, compatible_type()'s, of an identity of its own,
// enumerated()'s, which nothing reads yet, as it keeps the type a
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

    type_t t = enumerated(p, compatible_type(tag ? *tag : keyword, list));
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
// so it is the not-yet type the qualifier makes of the type name's
// type, identity_of()'s, and the use of its object is refused.  The type name is no array type and no
// function type, p3; an atomic or a qualified one cannot be told, since
// the type is not-yet and qualifiers are dropped.
type_t
atomic_type_specifier(parser_t &p)
{
    const token_t &keyword = advance(p);
    advance(p);

    const type_t t = type_name(p);
    if (is< array_t >(t))
        fail(keyword, "invalid application of " + quoted(keyword.text) +
                          " to an array type");

    if (is< function_t >(t))
        fail(keyword, "invalid application of " + quoted(keyword.text) +
                          " to a function type");

    expect(p, tok_t::rparen, "expected ')' after type name");

    return make_not_yet(identity_of(p, "_Atomic", t),
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
// member's own array, which may have a size of zero.  qualifiers are
// those said of the base type, which the declarator derives from with
// it.
declarator_t
declarator(parser_t &p,
           const type_t &base,
           qualifiers_t qualifiers,
           naming_t naming,
           bool typedef_)
{
    const type_t t = pointer(p, base, qualifiers);

    if (starts_parenthesized_declarator(p, naming))
        return parenthesized_declarator(p, t, qualifiers, naming, typedef_);

    const token_t *name = nullptr;
    if (naming == naming_t::required || naming == naming_t::member ||
        (naming == naming_t::optional && peek(p).kind == tok_t::identifier))
        name = &expect(p, tok_t::identifier, "expected identifier");

    return declarator_suffix(
        p, name, t, qualifiers, typedef_, naming == naming_t::optional,
        naming == naming_t::member, naming == naming_t::optional);
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
// own, and a member's own likewise.  The declarator inside derives from
// the suffixes' type with the qualifiers the suffixes leave it, so the
// pointer of const int (*a)[3] points to a const array.
declarator_t
parenthesized_declarator(parser_t &p,
                         const type_t &t,
                         qualifiers_t qualifiers,
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
        declarator_suffix(p, nullptr, t, qualifiers, typedef_, parameter,
                          member, naming == naming_t::optional);
    const std::size_t after = p.pos;

    p.pos = inside;
    declarator_t d =
        declarator(p, suffix.type, suffix.qualifiers, naming, typedef_);
    expect(p, tok_t::rparen, "expected ')'");
    p.pos = after;

    if (d.type == suffix.type) {
        d.params = std::move(suffix.params);
        d.scope = std::move(suffix.scope);
        d.star = suffix.star;
        d.zero = suffix.zero;
        d.bracket_qualifiers = suffix.bracket_qualifiers;
        d.variable_size = std::move(suffix.variable_size);
    }

    if (!d.inner_star)
        d.inner_star = suffix.inner_star;

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
// in_parameter says that they are in a parameter's declarators, at any
// depth, where '*' may size an array, 6.7.7.3p4; a list that may not
// be a definition's refuses its parameters' stars, refuse_inner_stars().
// qualifiers are those said of t: an array keeps them as its element's,
// 6.7.4.1p10, and a function drops them from its result, 6.7.7.4p4.
declarator_t
declarator_suffix(parser_t &p,
                  const token_t *name,
                  const type_t &t,
                  qualifiers_t qualifiers,
                  bool typedef_,
                  bool parameter,
                  bool member,
                  bool in_parameter)
{
    declarator_t d { name, t, qualifiers, std::nullopt, { } };

    if (peek(p).kind == tok_t::lbracket) {
        d.type = array_declarator(p, t, typedef_, parameter, member,
                                  in_parameter, d.star, d.zero, d.inner_star,
                                  d.bracket_qualifiers, d.variable_size);

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

        const bool definable =
            name && p.scopes.size() == 1 && !typedef_ && !member;
        if (!definable)
            refuse_inner_stars(params);

        std::vector< type_t > types;
        for (const parameter_t &param : params)
            types.push_back(param.type);

        d.params = std::move(params);
        const token_t *marked = marked_parameter(d);
        d.type = make_function(t, std::move(types), variadic,
                               marked ? attribute_words(*marked) : "");
        d.qualifiers = { };
    }

    return d;
}

// Each '*' derives a pointer to the type so far, 6.7.7.2p1, which keeps
// the qualifiers said of that type as its pointee's: qualifiers holds
// them on entry, and on return those said of the type derived last, a
// const or a volatile after its '*', which qualify the pointer, p1.  A
// restrict there qualifies it too and is dropped, as in the specifiers;
// the pointer is one restrict may qualify unless it points to a
// function, as in void (*restrict f)(void).  An _Atomic pointer is not
// yet, and so is a pointer to a variable length array, int (*p)[n],
// whose type is variably modified, 6.7.7.1p3.
type_t
pointer(parser_t &p, type_t t, qualifiers_t &qualifiers)
{
    while (peek(p).kind == tok_t::star) {
        const token_t &star = advance(p);
        if (is_variable_array(t))
            not_yet(star, "pointer to variable length array");

        t = make_pointer(std::move(t), qualifiers);
        qualifiers = { };

        for (;;) {
            const token_t &q = peek(p);
            if (q.kind == tok_t::kw_atomic)
                not_yet(q, "type qualifier " + quoted(q.text));

            if (q.kind == tok_t::kw_restrict)
                require_restrictable(q, t);

            if (!accept_qualifier(p, qualifiers) &&
                !accept(p, tok_t::kw_restrict))
                break;
        }
    }

    return t;
}

// The count of an array, from its size: an expression of integer type,
// 6.7.7.3p1, which for an array that is not a variable length array is
// an integer constant expression, p4, folded by now to a literal; any
// other size makes a variable length array, which array_declarator()
// takes before it asks.  A constant size is greater than zero, p1.  An
// unsigned size past LONG_MAX is LONG_MAX, which no element size lets
// through as an array's size.
long
array_count(const token_t &at, const expr_t &size)
{
    if (!is< int_t >(size.type))
        fail(at, "size of array has non-integer type");

    const exact_t value = exact_value(size);
    if (value <= 0)
        fail(at, "array size must be greater than zero");

    return long(std::min< exact_t >(value, std::numeric_limits< long >::max()));
}

// An array suffix as written: its '[', its count, -1 when the size is
// left out, and the words of what the size needs that rcc does not
// have yet, empty when it needs nothing, with the words of the size,
// the means its not-yet type is made by.  variable_size is a variable
// length array's size, absent for any other suffix.
struct array_suffix_t
{
    const token_t *open;
    long count;
    std::string missing;
    std::string size = { };
    std::optional< expr_t > variable_size = { };
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
// parameter is the pointer either way.  In a definition the parameter
// has block scope, 6.2.1p4, and the size is evaluated on entry for its
// effects, 6.9.2p11, which array_declarator() keeps it for.
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
// taken into qualifiers as after a '*'; restrict is returned in
// restrict_, for the caller to check on that pointer, and dropped.
// _Atomic is not yet, as on a pointer.  static promises that each
// argument points to the first of at least as many elements as the
// size says, p6, which no call rcc writes depends on, and is dropped;
// its token is returned, or null, since a size must follow it.
const token_t *
parameter_array_qualifiers(parser_t &p,
                           const token_t *&restrict_,
                           qualifiers_t &qualifiers)
{
    const token_t *static_ =
        peek(p).kind == tok_t::kw_static ? &advance(p) : nullptr;

    for (;;) {
        const token_t &q = peek(p);
        if (q.kind == tok_t::kw_atomic)
            not_yet(q, "type qualifier " + quoted(q.text));

        if (q.kind == tok_t::kw_restrict && !restrict_)
            restrict_ = &q;

        if (!accept_qualifier(p, qualifiers) && !accept(p, tok_t::kw_restrict))
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

// A '*' in brackets other than those a parameter is adjusted from,
// 6.7.7.3p4: in a parameter's declarators, outside its array sizes, a
// variable length array of unspecified size, whose first '*' is
// returned in inner_star for its refusal, refuse_inner_stars();
// anywhere else an error.  False when the brackets hold none.
bool
inner_unspecified_size(parser_t &p,
                       bool in_parameter,
                       const token_t *&inner_star)
{
    if (peek(p).kind != tok_t::star || peek(p, 1).kind != tok_t::rbracket)
        return false;

    if (!in_parameter)
        fail(peek(p), "'*' as an array size outside a parameter declaration");

    const token_t &star = advance(p);
    if (!inner_star)
        inner_star = &star;

    return true;
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

// A variable length array type's length, 6.7.7.3p5: the local of
// size_t that holds the size in bytes, which the array's declaration
// declares before it, local_variable_array()'s.  No name stands for it,
// so it is in no scope.
const symbol_t &
array_length(parser_t &p)
{
    return p.unit.symbols.emplace_back(
        symbol_t { "__array_size", size_type(), storage_t::local, false, -1 });
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
// brackets may hold static and type qualifiers, 6.7.7.3p1, the
// qualifiers returned in bracket_qualifiers, and its size need not be
// an integer constant expression, or may be '*', returned in star;
// every other suffix keeps the rules of any array, so a
// variable length array behind the parameter's pointer is a pointer to
// one, not yet: one of a size in a parameter's declarators,
// in_parameter, is refused at its size, and its '*' makes a not-yet
// type in place of the array until its refusal
// (inner_unspecified_size()).
// When member says that the first suffix is a member's own array, its
// size may be zero, returned in zero.  No element is a record that
// holds a flexible array member, 6.7.3.2p3, in the program's own text;
// a system header's array of one is read as it is and laid out.  Any
// other size that is no integer constant expression makes a variable
// length array, p4, of a length of its own, its size returned in
// variable_size for the declaration to evaluate, and the caller decides
// whether its context takes one; an array of variable length arrays is
// not yet.  The size of the array a parameter is adjusted from is
// returned in variable_size too when it is no constant, for a
// definition to evaluate.
type_t
array_declarator(parser_t &p,
                 const type_t &element,
                 bool typedef_,
                 bool parameter,
                 bool member,
                 bool in_parameter,
                 const token_t *&star,
                 const token_t *&zero,
                 const token_t *&inner_star,
                 qualifiers_t &bracket_qualifiers,
                 std::optional< expr_t > &variable_size)
{
    std::vector< array_suffix_t > suffixes;
    const token_t *restrict_ = nullptr;

    while (peek(p).kind == tok_t::lbracket) {
        array_suffix_t suffix { &advance(p), -1, { } };
        const bool adjusted = parameter && suffixes.empty();
        const bool own = member && suffixes.empty();

        bool unspecified = false;
        if (adjusted) {
            const token_t *static_ =
                parameter_array_qualifiers(p, restrict_, bracket_qualifiers);
            star = unspecified_size(p, static_);
        }
        else {
            refuse_array_qualifier(peek(p));
            unspecified = inner_unspecified_size(p, in_parameter, inner_star);
        }

        if (unspecified)
            suffix.missing = "variable length array";

        if (!(adjusted && star) && !unspecified &&
            peek(p).kind != tok_t::rbracket) {
            const token_t &start = peek(p);

            std::optional< expr_t > size =
                typedef_ ? typedef_array_size(p, suffix.missing) :
                           array_size(p);
            if (size) {
                refuse_file_scope_variable_size(p, start, *size);
                if (!adjusted && is< int_t >(size->type) &&
                    !is< literal_t >(*size)) {
                    if (in_parameter)
                        not_yet(start, "pointer to variable length array");

                    suffix.variable_size = std::move(size);
                }
                else
                    suffix.count =
                        adjusted ? parameter_array_count(start, *size) :
                        own      ? member_array_count(start, *size, zero) :
                                   array_count(start, *size);

                if (adjusted && !is< literal_t >(*size))
                    variable_size = std::move(size);
            }
        }

        expect(p, tok_t::rbracket, "expected ']'");

        const auto first = std::size_t(suffix.open - p.tokens.data()) + 1;
        if (!suffix.missing.empty())
            suffix.size = "[" + spelling(p, first, p.pos - 1) + "]";

        suffixes.push_back(std::move(suffix));
    }

    type_t t = element;

    for (auto it = suffixes.rbegin(); it != suffixes.rend(); ++it) {
        auto &[open, count, missing, size, variable] = *it;

        if (is< function_t >(t))
            fail(*open, "array element type cannot be a function");

        if (is_incomplete(t))
            fail(*open, "array has incomplete element type");

        if (const record_t *r = flexible_record(t);
            r && !open->where.system_header)
            fail(*open, "array element of type " + flexible_words(*r));

        if (is_variable_array(t))
            not_yet(*open, "array of variable length arrays");

        if (!missing.empty()) {
            const int identity = identity_of(p, size, t);
            t = make_not_yet_array(identity, missing, std::move(t));
            continue;
        }

        if (variable) {
            t = make_variable_array(std::move(t), array_length(p));
            variable_size = std::move(variable);
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
// or a type qualifier on it leaves the behavior undefined, J.2 (75), so
// (register void) and (const void) are refused, and so is a typedef
// name of a qualified void, a type of its own, 6.2.5p31.  Each
// parameter is declared as it is read, so that the next one sees it.
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
                param.qualifiers == qualifiers_t { } &&
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
// (*f)(void)) declare one function.  A typedef's array of a size rcc
// cannot compute keeps its element for the adjustment.  The pointer an
// array is adjusted to points to the element qualified as the array is,
// and is itself qualified as the brackets say, p6.  A parameter of type
// void is left for the list to take or refuse.
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

    declarator_t d =
        declarator(p, spec.type, spec.qualifiers, naming_t::optional);

    const token_t *marking = attribute_specifier_list(p);
    if (spec.marking)
        marking = spec.marking;

    // a register parameter of array or function type is a register
    // pointer, since the adjustment comes first
    type_t t = d.type;
    qualifiers_t qualifiers = d.qualifiers;
    if (is< array_t >(t)) {
        t = make_pointer(*as< array_t >(t).element, d.qualifiers);
        qualifiers = d.bracket_qualifiers;
    }
    else if (is< not_yet_t >(t) && as< not_yet_t >(t).element) {
        t = make_pointer(*as< not_yet_t >(t).element, d.qualifiers);
        qualifiers = d.bracket_qualifiers;
    }
    else if (is< function_t >(t))
        t = make_pointer(std::move(t));

    return parameter_t { std::move(t), &start,
                         d.name,       declared(spec, tok_t::kw_register),
                         marking,      d.star,
                         d.inner_star, std::move(d.variable_size),
                         qualifiers };
}

// Specifiers and an abstract declarator, the declarator of a
// declaration without its name, 6.7.8p1, p2: int *, int [3],
// void (*)(int), with the qualifiers at the top of the type.  A marking
// attribute would make the type one rcc does not use, which no name
// stands for here, so it is not yet, and so is a variable length
// array, sizeof (char [n]).
qualified_type_t
qualified_type_name(parser_t &p)
{
    const token_t &start = peek(p);
    const specifiers_t spec = declaration_specifiers(p);

    // 6.7.8p1: a specifier-qualifier list, no storage class
    if (spec.storage_class)
        fail(*spec.storage_class,
             quoted(spec.storage_class->text) + " in a type name");

    refuse_function_specifier(spec);

    if (spec.marking)
        not_yet(*spec.marking, attribute_words(*spec.marking));

    const declarator_t d =
        declarator(p, spec.type, spec.qualifiers, naming_t::none);
    if (is_variable_array(d.type))
        not_yet(start, "variable length array in a type name");

    return { d.type, d.qualifiers };
}

// A type name's type, its qualifiers at the top left out: a cast
// converts to the unqualified type, 6.5.5p6, and sizeof, _Alignof,
// va_arg and offsetof read none.  A generic association reads them,
// qualified_type_name()'s.
type_t
type_name(parser_t &p)
{
    return qualified_type_name(p).type;
}

// The declarator of a declaration in a block that is no typedef's.  A
// function declared in a block takes no storage class but extern,
// 6.7.2p13, so static and register on one are errors.
declarator_t
block_declarator(parser_t &p, const specifiers_t &spec)
{
    declarator_t d = declarator(p, spec.type, spec.qualifiers);

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
// 6.2.1p7, so the initializer sees the object.  An object of static
// storage duration has no variable length array type, 6.7.7.3p2.
void
static_local(parser_t &p, const specifiers_t &spec, const declarator_t &d)
{
    if (is_variable_array(d.type))
        fail(*d.name, "variable length array " + quoted(d.name->text) +
                          " declared 'static'");

    require_block_object(p, spec, d);

    const bool initialized = peek(p).kind == tok_t::assign;
    if (!initialized && is< array_t >(d.type) &&
        as< array_t >(d.type).count < 0)
        fail(*d.name, "array size missing in " + quoted(d.name->text));

    if (p.scopes.back().names.contains(d.name->text))
        fail(*d.name, "redefinition of " + quoted(d.name->text));

    symbol_t &sym = added(p, d.name->text, d.type, storage_t::global);
    sym.asm_name = static_label(p, sym.name);
    sym.qualifiers = d.qualifiers;

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
// block holds declarations and statements, 6.8.3p1.  A name of linkage
// has no variable length array type, 6.7.7.3p2.
void
linked_declaration(parser_t &p, const specifiers_t &spec, const declarator_t &d)
{
    if (is_variable_array(d.type))
        fail(*d.name, "variable length array " + quoted(d.name->text) +
                          " declared 'extern'");

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
    refuse_visibility(*d.name, linkage);

    auto &names = p.scopes.back().names;
    const auto here = names.find(d.name->text);
    if (here != names.end() && here->second != entity)
        fail(*d.name, "redefinition of " + quoted(d.name->text));

    if (entity) {
        redeclared(*d.name, *entity, d.type, d.qualifiers, storage);
        require_linkage(*d.name, *entity, linkage);
        names.emplace(entity->name, entity);
    }
    else {
        entity = &added(p, d.name->text, d.type, storage);
        entity->linkage = linkage;
        entity->qualifiers = d.qualifiers;
        p.block_entities.emplace(entity->name, entity);

        if (function)
            p.unit.functions.push_back(
                function_decl_t { entity, { }, std::nullopt, 0 });
    }

    record_tail(p, *entity, tail);
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
// does not make.  objects_only is a for's declaration, which declares
// no function (for_declaration()).
std::vector< stmt_t >
init_declarator_list(parser_t &p, const specifiers_t &spec, bool objects_only)
{
    std::vector< stmt_t > decls;

    do {
        if (declared(spec, tok_t::kw_typedef)) {
            refuse_function_specifier(spec);
            type_definition(p, spec,
                            declarator(p, spec.type, spec.qualifiers,
                                       naming_t::required, true));
        }
        else {
            declarator_t d = block_declarator(p, spec);
            if (objects_only && is< function_t >(d.type))
                fail(*d.name, "function " + quoted(d.name->text) +
                                  " declared in the declaration of a 'for'");

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

// The list that initializes the local record or array sym, its '{'
// ahead: the walk of an object of static storage's, 6.7.11p17 to p23,
// whose scalars are stores at run time, local_store()'s, appended to
// stores in the list's order after the declaration, which zeroes the
// whole object, so every member, element and byte of padding no value
// is listed for is zero, p20, p22 (shape.md chapter 5).  An array of
// unknown size takes its size from the list, p23.
void
local_list(parser_t &p, symbol_t &sym, std::vector< stmt_t > &stores)
{
    destination_t to { .local = &sym, .stores = &stores };
    const long count = braced_initializer(p, sym.type, 0, to);

    sym.type = completed_type(sym.type, count);
}

// The string literal that initializes the local array sym, after its
// '=', alone or in braces: one for an array of char, 6.7.11p15, or of a
// wide literal's element type, p16, as string_value() checks it, and an
// array of unknown size takes its elements and the terminating zero,
// p23.  The declaration's initializer is the copy of the literal into
// the array, string_copy()'s, and the declaration zeroes the elements
// past the copy after it, p22, so each byte of the array is written
// once (G22).  Any other expression initializes no array, p17.
void
local_string(parser_t &p, symbol_t &sym, decl_stmt_t &decl)
{
    advance(p);
    const bool braced = accept(p, tok_t::lbrace);

    const token_t &start = peek(p);
    expr_t e = assignment_expression(p);
    if (!is< string_t >(e) || !initializes(sym.type, e))
        fail(start, "array initializer must be an initializer list");

    std::vector< subobject_t > checked;
    const long count =
        string_value(start, as< string_t >(e), sym.type, 0, checked);

    if (braced)
        close_list(p, true, string_array_words(sym.type));

    sym.type = completed_type(sym.type, count);
    decl.init = string_copy(p, start, sym, sym.type, 0, std::move(e));
    decl.zeroed = size_of(decl.init->type) < size_of(sym.type);
}

// A local larger than the target's largest frame, its frame_limit, is
// refused at its name, since no frame holds it.
void
require_frame(const parser_t &p, const token_t &name, const type_t &t)
{
    if (size_of(t) > p.target.frame_limit)
        fail(name,
             "variable " + quoted(name.text) + " is too large for a local");
}

// The initializer of a variable length array: none but the empty one,
// 6.7.11p4, which C23 adds and rcc does not take yet, since it zeroes
// an object whose size is known at run time alone.
void
refuse_variable_array_initializer(const parser_t &p, const declarator_t &d)
{
    if (peek(p).kind != tok_t::assign)
        return;

    if (peek(p, 1).kind == tok_t::lbrace && peek(p, 2).kind == tok_t::rbrace)
        not_yet(peek(p, 1), "empty initializer of variable length array " +
                                quoted(d.name->text));

    fail(peek(p), "variable length array " + quoted(d.name->text) +
                      " cannot have an initializer");
}

// The local a variable length array's declaration keeps the stack
// pointer in before it takes the storage, a pointer's 8 bytes, which
// the declaration declares before the array, local_variable_array()'s.
// No name stands for it, so it is in no scope.
const symbol_t &
saved_stack_pointer(parser_t &p)
{
    return p.unit.symbols.emplace_back(
        symbol_t { "__stack_pointer", make_pointer(make_void()),
                   storage_t::local, false, -1 });
}

// A local variable length array, 6.7.7.3p4.  Its declaration evaluates
// the size where it is reached, 6.8.1p3, converted to size_t as by
// assignment and multiplied by the element's size, into the type's
// length, so that sizeof reads the size of the instance, which does not
// change, 6.7.7.3p5; a multiplication by 1 is the identity and is left
// out.  The array's own symbol is a pointer to it, by_reference, which
// a stack allocation of the length's bytes initializes, and its name
// reads as the array (shape.md chapter 5).  The storage lasts until the
// program leaves the declaration's scope, 6.2.4p7, so the declaration
// keeps the stack pointer first, in a local of the array's own,
// saved_stack_pointer()'s, which every way out of the scope puts back:
// a declaration reached again, in a loop or after a jump back, takes
// its storage from where the stack pointer stood the time before.  The
// name's token goes to its scope's variable length arrays, for the
// checks of a jump into its scope and the count at each label.
void
local_variable_array(parser_t &p, declarator_t &d, std::vector< stmt_t > &decls)
{
    refuse_variable_array_initializer(p, d);

    const symbol_t &length = *as< array_t >(d.type).length;
    const long element = size_of(*as< array_t >(d.type).element);

    expr_t size = convert_to(std::move(*d.variable_size), size_type());
    if (element != 1) {
        expr_t factor { size_type(), literal_t { element } };
        size = expr_t { size_type(),
                        binary_t { binary_op_t::mul, box(std::move(size)),
                                   box(std::move(factor)) } };
    }

    decls.push_back(stmt_t { decl_stmt_t { &length, std::move(size) } });

    const symbol_t &saved = saved_stack_pointer(p);
    decls.push_back(stmt_t { decl_stmt_t { &saved, std::nullopt } });

    symbol_t *sym = declare(p, *d.name, make_pointer(d.type, d.qualifiers),
                            storage_t::local, false);
    sym->by_reference = true;
    p.scopes.back().variable_arrays.push_back(d.name);

    expr_t bytes { length.type, name_t { &length } };
    expr_t storage { make_pointer(make_void()),
                     alloca_t { box(std::move(bytes)) } };

    decls.push_back(stmt_t { decl_stmt_t {
        sym, convert_to(std::move(storage), sym->type), false, &saved } });
}

// A local, an object of automatic storage duration, 6.2.4p5, appended
// to decls as its declaration, and for a record initialized by an
// expression the record_initializer() statement after it, or for a
// record or an array initialized by a list the declaration zeroed and
// local_list()'s statements, or for an array a string literal
// initializes the declaration with its copy, local_string()'s.  The
// name is in scope from the end of its declarator, 6.2.1p7, so an
// initializer sees the object it initializes.  The initializer is
// converted as by assignment, 6.7.11p12, an error of the conversion
// said at the =; an initializer that has no value, a void one,
// 6.3.2.2p1, is the error of the operand, said at its first token.  An
// object's type must be complete, 6.7.1p8, so an array needs its size,
// or a list or a string to give it, 6.7.11p23.  A brace initializer of
// a scalar is not yet: a local's initializer is stores at run time,
// where an object of static storage takes its value as data.  A
// register array is not yet either: its decay is undefined, 6.3.2.1p3,
// and it has no slot to decay to; nor is a register record, which no
// register holds.  A local too large for a frame is refused,
// require_frame(), an array of unknown size once its list or its
// string has given the size.  A variable length array is
// local_variable_array()'s.
void
init_declarator(parser_t &p,
                const specifiers_t &spec,
                declarator_t &d,
                std::vector< stmt_t > &decls)
{
    require_block_object(p, spec, d);

    if (is< array_t >(d.type) && declared(spec, tok_t::kw_register))
        not_yet(*d.name, "register array " + quoted(d.name->text));

    const record_t *r = record_in(d.type);
    if (r && declared(spec, tok_t::kw_register))
        not_yet(*d.name, "register object " + quoted(d.name->text) + " of " +
                             record_words(*r));

    if (is_variable_array(d.type)) {
        local_variable_array(p, d, decls);
        return;
    }

    const bool array = is< array_t >(d.type);
    const bool record = is< record_type_t >(d.type);
    const bool listed = peek(p).kind == tok_t::assign &&
                        peek(p, 1).kind == tok_t::lbrace && (array || record);
    const bool string = array && peek(p).kind == tok_t::assign &&
                        (!listed || string_ahead(p, d.type, 2));

    const bool sized = !array || as< array_t >(d.type).count >= 0;
    if (!sized && !listed && !string)
        fail(*d.name, "array size missing in " + quoted(d.name->text));

    if (sized)
        require_frame(p, *d.name, d.type);

    symbol_t *sym = declare(p, *d.name, d.type, storage_t::local,
                            declared(spec, tok_t::kw_register));
    sym->qualifiers = d.qualifiers;
    decl_stmt_t decl { sym, std::nullopt };

    if (string) {
        local_string(p, *sym, decl);

        decls.push_back(stmt_t { std::move(decl) });
        if (!sized)
            require_frame(p, *d.name, sym->type);

        return;
    }

    if (listed) {
        advance(p);
        decl.zeroed = true;

        decls.push_back(stmt_t { std::move(decl) });
        local_list(p, *sym, decls);
        if (!sized)
            require_frame(p, *d.name, sym->type);

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

        decl.init = assignment_conversion(p, op, std::move(init), d.type);
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

// The variable length arrays in scope where the parse stands, by their
// names' tokens, the outermost scope's first.
std::vector< const token_t * >
variable_arrays_in_scope(const parser_t &p)
{
    std::vector< const token_t * > arrays;
    for (const scope_t &scope : p.scopes)
        arrays.insert(arrays.end(), scope.variable_arrays.begin(),
                      scope.variable_arrays.end());

    return arrays;
}

// The variable length arrays in scope at a label or a goto, kept by its
// name's token where there is one, for require_labels().
void
record_jump_scope(parser_t &p, const token_t &name)
{
    std::vector< const token_t * > arrays = variable_arrays_in_scope(p);
    if (!arrays.empty())
        p.jump_scopes.emplace(&name, std::move(arrays));
}

// A label, identifier :, which defines its name in the function's
// labels, 6.2.1p3; a name is defined once in a function, 6.8.2p3.  One
// inside a statement expression is a label of the function too, and a
// goto from outside the expression, which would jump into it, is
// require_labels()'s error, as is one from outside the scope of a
// variable length array the label is in.
stmt_t
named_label(parser_t &p)
{
    const token_t &name = advance(p);
    advance(p);

    if (!p.labels.emplace(name.text, &name).second)
        fail(name, "redefinition of label " + quoted(name.text));

    record_jump_scope(p, name);

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

// A case or a default label inside a statement expression its switch is
// outside of: the switch would jump into the expression, which GNU C
// does not permit.  So is one in the scope of a variable length array
// its switch is outside of, 6.8.5.3p2: one the switch's body declares
// before the label, whose scope the label is still in.
void
refuse_jump_into(const parser_t &p, const token_t &keyword)
{
    if (p.switches.back().statement_expressions != p.statement_expressions)
        fail(keyword,
             quoted(keyword.text) +
                 " label of a switch outside the statement expression");

    const std::size_t outside = p.switches.back().variable_arrays;
    const std::vector< const token_t * > arrays = variable_arrays_in_scope(p);
    if (arrays.size() > outside)
        fail(keyword, quoted(keyword.text) +
                          " label of a switch outside the scope of variable "
                          "length array " +
                          quoted(arrays[outside]->text));
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
    refuse_jump_into(p, keyword);

    const token_t &start = peek(p);
    expr_t constant = rvalue(start, conditional_expression(p));
    if (!is_integer_constant(constant))
        refuse_unfolded(start, constant,
                        "case label is not an integer constant expression");

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
    refuse_jump_into(p, keyword);

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

// An __asm__ statement rcc does not compile, refused at its keyword in
// one message whatever it holds: a template with text, an operand, a
// label, goto, a clobber of another name.
[[noreturn]] void
refuse_asm_statement(const token_t &keyword)
{
    not_yet(keyword, quoted(keyword.text) + " statement");
}

// The outputs or the inputs of an __asm__ statement, after their colon,
// which rcc takes empty alone.  A token there that is neither the next
// colon nor the ')' begins an operand, refused before its grammar, the
// constraint string, the [name] and the expression, is read: so a
// malformed operand is refused too, and is no error.
void
require_no_asm_operand(const parser_t &p, const token_t &keyword)
{
    if (!next_is(p, { tok_t::colon, tok_t::rparen }))
        refuse_asm_statement(keyword);
}

// The clobbers of an __asm__ statement, the strings after its third
// colon separated by commas, or none: whether each is "memory" or "cc",
// a barrier's.  No name is checked against the target's registers.
bool
asm_clobbers_are_barrier(parser_t &p, const token_t &keyword)
{
    if (next_is(p, { tok_t::colon, tok_t::rparen }))
        return true;

    bool barrier = true;
    do {
        const std::string name = asm_string(p, keyword);
        barrier = barrier && (name == "memory" || name == "cc");
    } while (accept(p, tok_t::comma));

    return barrier;
}

// The sections after an __asm__ statement's template, each opened by a
// colon: the outputs, the inputs, the clobbers and the labels.  Whether
// the clobbers are a barrier's; an operand or a label is refused.  The
// lexer has no '::' token, so ::: is three colons.
bool
asm_sections_are_barrier(parser_t &p, const token_t &keyword)
{
    if (!accept(p, tok_t::colon))
        return true;
    require_no_asm_operand(p, keyword);

    if (!accept(p, tok_t::colon))
        return true;
    require_no_asm_operand(p, keyword);

    if (!accept(p, tok_t::colon))
        return true;
    const bool barrier = asm_clobbers_are_barrier(p, keyword);

    if (peek(p).kind == tok_t::colon)
        refuse_asm_statement(keyword);

    return barrier;
}

// Whether a token is a qualifier of an __asm__ statement: volatile,
// inline or goto, in any spelling.
bool
is_asm_qualifier(const token_t &t)
{
    return t.kind == tok_t::kw_volatile || t.kind == tok_t::kw_inline ||
           t.kind == tok_t::kw_goto;
}

// The qualifiers after an __asm__ statement's keyword, volatile, inline
// and goto in any order and each once: one written a second time, in
// any spelling, is an error at its token, as GNU C has it.  Whether
// goto is among them.
bool
asm_qualifiers_have_goto(parser_t &p, const token_t &keyword)
{
    std::vector< tok_t > seen;

    while (is_asm_qualifier(peek(p))) {
        const token_t &qualifier = advance(p);
        if (std::ranges::find(seen, qualifier.kind) != seen.end())
            fail(qualifier, "duplicate qualifier " + quoted(qualifier.text) +
                                " in " + quoted(keyword.text) + " statement");

        seen.push_back(qualifier.kind);
    }

    return std::ranges::find(seen, tok_t::kw_goto) != seen.end();
}

// GNU C's __asm__ statement, also spelled asm and __asm, after which
// volatile, inline and goto may stand.  The barrier, of an empty
// template, no operand, no label, no goto, and clobbers of "memory" and
// "cc" alone or none, is the null statement (shape.md chapter 5); every
// other one is not yet.  volatile and inline change nothing: a
// statement with no output is volatile already, and the null statement
// has nothing to inline.
stmt_t
asm_statement(parser_t &p)
{
    const token_t &keyword = advance(p);

    const bool goto_ = asm_qualifiers_have_goto(p, keyword);

    expect(p, tok_t::lparen, "expected '(' after " + quoted(keyword.text));
    const bool empty = asm_string(p, keyword).empty();
    const bool barrier = asm_sections_are_barrier(p, keyword);
    expect(p, tok_t::rparen,
           "expected ')' in " + quoted(keyword.text) + " statement");
    expect(p, tok_t::semicolon,
           "expected ';' after " + quoted(keyword.text) + " statement");

    if (goto_ || !empty || !barrier)
        refuse_asm_statement(keyword);

    return stmt_t { expr_stmt_t { } };
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
    case tok_t::kw_asm: return asm_statement(p);
    default: break;
    }

    if (is_label_start(p))
        return labeled_statement(p);

    if (is_declaration_start(p))
        fail(t, "expected statement, found a declaration");

    return expression_statement(p);
}

// A new scope for the block's declarations, whose end c1 knows by the
// block's scope.
block_t
compound_statement(parser_t &p)
{
    block_t block { .body = { }, .scope = true };

    expect(p, tok_t::lbrace, "expected '{'");
    p.scopes.emplace_back();
    block_item_list(p, block);
    close_block(p);
    expect(p, tok_t::rbrace, "expected '}' at end of block");

    return block;
}

// One block item, appended to items: a label, which is an item of its
// own, 6.8.3p1, so it may stand before a declaration and before the
// closing brace; a declaration, of the statements it makes; or a
// statement.  Whether it is an expression statement with an expression,
// whose expression is a statement expression's value when the item is
// its block's last: a declaration is not one, though a record's list
// makes stores of it after the declaration (shape.md chapter 5).
bool
block_item(parser_t &p, std::vector< stmt_t > &items)
{
    if (is_label_start(p)) {
        items.push_back(label(p));
        return false;
    }

    if (is_declaration_start(p)) {
        for (stmt_t &decl : declaration(p))
            items.push_back(std::move(decl));
        return false;
    }

    items.push_back(statement(p));
    const stmt_t &st = items.back();

    return is< expr_stmt_t >(st) && as< expr_stmt_t >(st).e;
}

// The items up to the closing brace, which is left for the caller.
void
block_item_list(parser_t &p, block_t &block)
{
    while (!next_is(p, { tok_t::rbrace, tok_t::eof }))
        block_item(p, block.body);
}

// Whether the tokens of the parse from first to before last are those
// from inner to before end alone, in as many parentheses as they open.
bool
spans(const parser_t &p,
      std::size_t first,
      std::size_t last,
      std::size_t inner,
      std::size_t end)
{
    if (inner < first || end > last || inner - first != last - end)
        return false;

    for (std::size_t k = 0; k < inner - first; ++k)
        if (p.tokens[first + k].kind != tok_t::lparen ||
            p.tokens[end + k].kind != tok_t::rparen)
            return false;

    return true;
}

// An expression evaluated for its effect alone, e, the tokens of the
// parse from first to before last: where it is the value of the last
// store into a bit-field, its compound assignment or its step, alone
// in any parentheses, spans()'s, the store alone, read_operand()'s,
// since nothing reads the value.
expr_t
stored_effect(const parser_t &p, expr_t e, std::size_t first, std::size_t last)
{
    if (!p.field_store ||
        !spans(p, first, last, p.field_store->first, p.field_store->second))
        return e;

    return read_operand(std::move(e));
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
    const std::size_t first = p.pos;
    expr_t e = expression(p);
    e = stored_effect(p, std::move(e), first, p.pos);

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

    p.switches.push_back({ cond.type, p.statement_expressions,
                           variable_arrays_in_scope(p).size() });
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
// is refused there, and so is a function, by C17's rule that the
// declaration declare objects of storage class auto or register only,
// its 6.8.5p3, the dialect's (shape.md chapter 1).  A static
// assertion, a declaration too, 6.7.1p1, makes no statement, and the
// init is null.
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

    std::vector< stmt_t > decls = init_declarator_list(p, spec, true);
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
        const std::size_t first = p.pos;
        expr_t step = expression(p);
        step = stored_effect(p, std::move(step), first, p.pos);
        loop.step = discarded(start, std::move(step));
    }

    expect(p, tok_t::rparen, "expected ')' after for clauses");

    loop.body = loop_body(p);

    close_block(p);

    return stmt_t { std::move(loop) };
}

// A goto names a label of the function, defined before it or after
// it, so the name is checked at the function's end, 6.8.7.2p1.  GNU
// C's computed goto, goto *p, is not yet.
stmt_t
goto_statement(parser_t &p)
{
    const token_t &keyword = advance(p);
    if (peek(p).kind == tok_t::star)
        not_yet(keyword, "computed " + quoted(keyword.text));

    const token_t &name =
        expect(p, tok_t::identifier, "expected label name after 'goto'");
    p.gotos.push_back(&name);
    record_jump_scope(p, name);

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

// return E; of a void E in a void function breaks 6.8.7.5p1, and GNU C
// takes it: rcc takes it with a warning, as the block of E for its
// effect and the return, a block of no scope (shape.md chapter 5).
stmt_t
void_return(parser_t &p,
            const token_t &keyword,
            const token_t &start,
            expr_t value)
{
    warn(p, keyword,
         "void function returns a void expression, as GNU C allows");
    expect(p, tok_t::semicolon, "expected ';' after return statement");

    block_t block;
    block.body.push_back(
        stmt_t { expr_stmt_t { discarded(start, std::move(value)) } });
    block.body.push_back(stmt_t { return_t { } });

    return stmt_t { std::move(block) };
}

// A value is converted to the result type as if by assignment,
// 6.8.7.5p3; a void function takes no value and a non-void one
// requires one, 6.8.7.5p1, but for GNU C's void value of a void
// function.
stmt_t
return_statement(parser_t &p)
{
    const token_t &keyword = advance(p);

    return_t ret;

    if (peek(p).kind != tok_t::semicolon) {
        const token_t &start = peek(p);

        expr_t value = expression(p);
        if (is< void_t >(p.result) && is< void_t >(value.type))
            return void_return(p, keyword, start, std::move(value));

        if (is< void_t >(p.result))
            fail(keyword, "void function should not return a value");

        ret.value = assignment_conversion(p, start, std::move(value), p.result);
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
    std::size_t first = p.pos;
    expr_t e = assignment_expression(p);

    while (peek(p).kind == tok_t::comma) {
        e = stored_effect(p, std::move(e), first, p.pos);
        e = discarded(advance(p), std::move(e));

        first = p.pos;
        e = comma(std::move(e), assignment_expression(p));
    }

    return e;
}

// A record's simple assignment, 6.5.17.2p1: its left operand a
// modifiable lvalue of a complete record, 6.3.2.1p1, so neither const
// nor holding a member declared const, holds_const_member()'s, and its
// right one of a compatible record, which rcc takes as the same record,
// so that two records of one tag are not yet.  The right operand is an
// lvalue too, since a record's value is no operand yet: the value of
// another assignment or of a comma is refused in refuse_record_value()'s
// words.  The tree is an assign_t of the record's type without an
// operation, the two lvalues, for c1 to copy the right's object into
// the left's.
expr_t
record_assignment(parser_t &p, const token_t &op, expr_t lhs)
{
    const record_t &r = *as< record_type_t >(lhs.type).record;
    if (!is_lvalue(lhs))
        fail(op, "expression is not assignable");

    require_whole_record(op, lhs.type);
    refuse_const_lvalue(op, lhs);

    if (holds_const_member(r))
        fail(op, "cannot assign to " + record_words(r) +
                     ", which holds a member declared const");

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

// The last access of a bit-field, when it is the whole of the tokens
// of the parse from first to before last, spans()'s, or null.
const field_access_t *
field_operand(const parser_t &p, std::size_t first, std::size_t last)
{
    if (!p.field || !spans(p, first, last, p.field->first, p.field->end))
        return nullptr;

    return &*p.field;
}

// The bit-field an operator stores into, its operand the tokens of the
// parse from first to before last, where that operand is the access of
// a field that does not fill its unit, field_operand()'s, whose store,
// compound assignment and step are the parser's trees; or null.
const field_access_t *
stored_field(const parser_t &p, std::size_t first, std::size_t last)
{
    const field_access_t *f = field_operand(p, first, last);
    if (!f || f->fills)
        return nullptr;

    return f;
}

// The step op of the bit-field m, stored_field()'s, whose read is arg,
// the step's tokens those of the parse from first: field_step()'s on
// the unit's lvalue, a modifiable one, 6.5.4.1p1, 6.5.3.5p1, the tokens
// kept as the last store's, which stored_effect() reads.
expr_t
stepped_field(parser_t &p,
              const token_t &op,
              expr_t arg,
              const member_t &m,
              std::size_t first,
              bool postfix)
{
    expr_t place = read_operand(std::move(arg));
    require_steppable(op, place);

    expr_t e = field_step(p, op, std::move(place), m, postfix);
    p.field_store = { first, p.pos };

    return e;
}

// An assignment expression, 6.5.17.  A store into a bit-field that does
// not fill its unit, the left operand the field's read alone, is
// field_store()'s, and its value, 6.5.17.1p3, the field's read of the
// unit the store leaves, field_read()'s: the right operand converted to
// the field and promoted, with no second load of the unit.  Its
// compound assignment is field_compound()'s.  Where nothing reads the
// value, stored_effect() takes the store alone.
expr_t
assignment_expression(parser_t &p)
{
    const std::size_t first = p.pos;
    expr_t lhs = conditional_expression(p);
    if (!is_assignment_operator(peek(p).kind))
        return lhs;

    const field_access_t *field = stored_field(p, first, p.pos);

    const token_t &op = advance(p);
    if (op.kind == tok_t::assign && is< record_type_t >(lhs.type))
        return record_assignment(p, op, std::move(lhs));

    if (field) {
        const member_t m = field->member;
        expr_t place = read_operand(std::move(lhs));
        require_modifiable_lvalue(op, place);

        expr_t rhs = assignment_expression(p);
        p.field_store = { first, p.pos };
        if (op.kind != tok_t::assign)
            return field_compound(p, op, std::move(place), m, std::move(rhs));

        expr_t store = field_store(p, op, std::move(place), m, std::move(rhs));

        return field_read(std::move(store), m);
    }

    require_modifiable_lvalue(op, lhs);

    const type_t t = lhs.type;
    expr_t e = assignment(p, op, std::move(lhs), assignment_expression(p));
    if (op.kind != tok_t::assign)
        warn_void_arithmetic(p, op, t);

    return e;
}

// The condition is a scalar and stays as it is, 6.5.16p2, but in GNU
// C's a ?: b, omitted_middle()'s.
expr_t
conditional_expression(parser_t &p)
{
    expr_t cond = logical_or_expression(p);
    if (peek(p).kind != tok_t::question)
        return cond;

    const token_t &op = advance(p);
    // before the arms are parsed, so that this error comes first
    cond = rvalue(op, std::move(cond));

    // GNU C's a ?: b, the condition its own middle operand
    if (peek(p).kind == tok_t::colon) {
        advance(p);
        return omitted_middle(op, std::move(cond), conditional_expression(p));
    }

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
        const type_t t = is< pointer_t >(e.type) ? e.type : rhs.type;

        e = op.kind == tok_t::plus ?
                addition(op, std::move(e), std::move(rhs)) :
                subtraction(op, std::move(e), std::move(rhs));
        warn_void_arithmetic(p, op, t);
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

// GNU C's __extension__ before a cast expression is dropped, as it is
// before a declaration.  Its address of a label, &&next, is not yet.
expr_t
unary_expression(parser_t &p)
{
    const token_t &op = peek(p);

    switch (op.kind) {
    case tok_t::inc:
    case tok_t::dec: {
        const std::size_t at = p.pos;
        advance(p);

        const std::size_t first = p.pos;
        expr_t arg = unary_expression(p);
        if (const field_access_t *f = stored_field(p, first, p.pos))
            return stepped_field(p, op, std::move(arg), f->member, at, false);

        expr_t e = prefix_step(op, std::move(arg));
        warn_void_arithmetic(p, op, e.type);

        return e;
    }
    case tok_t::plus: advance(p); return unary_plus(op, cast_expression(p));
    case tok_t::minus:
    case tok_t::tilde:
        advance(p);
        return unary_arithmetic(op, cast_expression(p));
    case tok_t::bang: advance(p); return logical_not(op, cast_expression(p));
    case tok_t::amp: {
        advance(p);

        const std::size_t first = p.pos;
        expr_t arg = cast_expression(p);
        if (const field_access_t *f = field_operand(p, first, p.pos))
            fail(op, "cannot take the address of bit-field " +
                         quoted(f->member.name));

        return address_of(op, std::move(arg));
    }
    case tok_t::star: advance(p); return indirection(op, cast_expression(p));
    case tok_t::kw_sizeof: return sizeof_expression(p);
    case tok_t::kw_alignof: return alignof_expression(p);
    case tok_t::kw_extension: advance(p); return cast_expression(p);
    case tok_t::log_and: not_yet(op, "address of a label");
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
    else {
        const std::size_t first = p.pos;
        t = unary_expression(p).type;
        if (const field_access_t *f = field_operand(p, first, p.pos))
            fail(op, "invalid application of 'sizeof' to bit-field " +
                         quoted(f->member.name));
    }

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

// The member of a record by its name, 6.5.3.4p1, p2, member_named()'s,
// at its offset in the record; a name the record has no member of is
// an error.
member_t
named_member(const record_t &r, const token_t &name)
{
    if (auto m = member_named(r, name.text))
        return std::move(*m);

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
// the offset the member's in its record plus the one P carries.  It is
// qualified as the member is and as E is, or as P's pointee is, p3, p4,
// which is read before record_pointer() reads P through its casts.
expr_t
member_lvalue(const parser_t &p, const token_t &op, expr_t e, const member_t &m)
{
    const qualifiers_t record = op.kind == tok_t::dot ?
                                    qualifiers_of(e) :
                                    as< pointer_t >(e.type).qualifiers;
    auto [pointer, offset] = record_pointer(op, std::move(e));

    return lvalue_at(p, std::move(pointer), offset + m.offset, m.type,
                     record | m.qualifiers);
}

// Whether a token continues a postfix expression, 6.5.3.1p1.
bool
is_postfix_operator(tok_t kind)
{
    return kind == tok_t::lparen || kind == tok_t::lbracket ||
           kind == tok_t::dot || kind == tok_t::arrow || kind == tok_t::inc ||
           kind == tok_t::dec;
}

// Whether the member access whose member's name is the token ahead ends
// the whole operand of a sizeof, in the postfix expression begun at the
// token start: the keyword, then only '(' up to start, as many ')'
// after the member's name, and no postfix operator after them.  Such
// an operand is the member's lvalue alone, whose type sizeof takes,
// 6.5.4.4p2, and no operator uses its value.
bool
ends_sizeof_operand(const parser_t &p, std::size_t start)
{
    std::size_t open = 0;
    while (open < start && p.tokens[start - open - 1].kind == tok_t::lparen)
        ++open;

    if (open == start || p.tokens[start - open - 1].kind != tok_t::kw_sizeof)
        return false;

    for (std::size_t close = 0; close < open; ++close)
        if (peek(p, 1 + close).kind != tok_t::rparen)
            return false;

    return !is_postfix_operator(peek(p, 1 + open).kind);
}

// E.m and P->m, 6.5.3.4: the lvalue of the member, p3, p4, member_lvalue()'s.
// A member of an array type decays where its value is used, and one of
// a record type is an lvalue whose value is refused and whose members
// are reached.  A member of a floating type, or an array of one, which
// the record lays out and rcc does not use yet, is refused by the
// type's words, its address as its value, but where the access is the
// whole operand of sizeof, ends_sizeof_operand()'s.  A bit-field is
// reached through its access unit's lvalue, of the unit's size and of
// the field's signedness: where the field fills the unit, starting at
// its bit 0 and as wide, it is that lvalue, an object as any member of
// the unit's type; any other field is its read, field_read()'s, a
// value.  The access is the parse's last, field_access_t's.  start is
// the token the postfix expression begins at.
expr_t
member_access(parser_t &p, const token_t &op, expr_t e, std::size_t start)
{
    const bool sized_operand = ends_sizeof_operand(p, start);
    const token_t &name = expect(p, tok_t::identifier, "expected member name");

    if (op.kind == tok_t::arrow) {
        if (is< record_type_t >(e.type))
            fail(op, "member reference type is not a pointer");

        e = rvalue(op, std::move(e));
    }

    const member_t m = named_member(accessed_record(op, e.type), name);
    if (is_bit_field(m)) {
        const bool fills = m.bit == 0 && m.width == m.unit * 8;
        member_t unit = m;
        unit.type = unit_type(m.unit, as< int_t >(m.type).is_signed);

        expr_t lvalue = member_lvalue(p, op, std::move(e), unit);
        p.field = field_access_t { start, p.pos, m, fills };

        if (fills)
            return lvalue;

        return field_read(std::move(lvalue), m);
    }

    if (is_sized(m.type) && !sized_operand)
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
        // a ?: b has no first arm to copy
        expr_ptr_t cond = pure_operand(c.cond),
                   yes = c.yes ? pure_operand(c.yes) : nullptr,
                   no = pure_operand(c.no);
        if (cond && (yes || !c.yes) && no)
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
        assignment(p, operator_at(at, tok_t::assign),
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
next_argument(parser_t &p, const list_operand_t &l)
{
    const list_layout_t m = list_layout(p);
    const token_t &at = l.at;

    expr_t test = comparison(operator_at(at, m.test),
                             list_member(p, l, m.offset), int_constant(m.end));

    expr_t step = assignment(p, operator_at(at, tok_t::add_assign),
                             list_member(p, l, m.offset), int_constant(8));
    expr_t slot =
        addition(operator_at(at, tok_t::plus), list_bytes(p, l, m.area, 0),
                 subtraction(operator_at(at, tok_t::minus),
                             list_member(p, l, m.offset), int_constant(8)));
    expr_t in_area = comma(discarded(at, std::move(step)), std::move(slot));

    expr_t past =
        assignment(p, operator_at(at, tok_t::assign),
                   list_member(p, l, m.stack), list_bytes(p, l, m.stack, 8));
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
        p.va_area = &p.unit.symbols.emplace_back(symbol_t {
            "__va_area",
            make_array(make_int(8, false, int_kind_t::unsigned_char), size),
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

    expr_t e =
        assignment(p, assign, list_member(p, l, 0), std::move(values[0]));
    for (std::size_t k = 1; k < values.size(); ++k)
        e = comma(
            discarded(keyword, std::move(e)),
            assignment(p, assign, list_member(p, l, k), std::move(values[k])));

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
// makes the object it returns.  So is a call through a function type a
// marking attribute on a parameter marks, which would pass the
// parameter as the attribute says.
expr_t
postfix_expression(parser_t &p)
{
    const std::size_t start = p.pos;
    expr_t e = primary_expression(p);

    for (;;) {
        const token_t &op = peek(p);

        switch (op.kind) {
        case tok_t::lparen: {
            advance(p);

            e = callee(op, std::move(e));
            const function_t &type = callee_type(e);
            if (!type.marking.empty())
                not_yet(op, type.marking);

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
            if (const field_access_t *f = stored_field(p, start, p.pos)) {
                advance(p);
                e = stepped_field(p, op, std::move(e), f->member, start, true);
                break;
            }

            advance(p);
            e = postfix_step(p, op, std::move(e));
            warn_void_arithmetic(p, op, e.type);
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
            e = member_access(p, op, std::move(e), start);
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
        return assignment_conversion(p, start, std::move(arg), *param);

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
member_t
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
        refuse_unfolded(
            start, index,
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
// keyword is the identifier ahead's.  A bit-field has no offset in
// bytes, and its offsetof is an error: p4 leaves it undefined.
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
        const member_t m = designated_member(*at, t, name);
        if (is_bit_field(m))
            fail(name, "offsetof of bit-field " + quoted(m.name));

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
        assignment_conversion(p, e_at, assignment_expression(p), long_type());
    expect(p, tok_t::comma, "expected ',' after the expression");
    const token_t &c_at = peek(p);
    expr_t c =
        assignment_conversion(p, c_at, assignment_expression(p), long_type());
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
    expr_t set = assignment(p, operator_at(keyword, tok_t::assign),
                            expr_t { t.type, name_t { &t } }, std::move(e));

    return comma(
        discarded(keyword, std::move(set)),
        comma(discarded(c_at, std::move(c)), expr_t { t.type, name_t { &t } }));
}

// The parse state a statement expression's block changes and gives
// back at its end: the count of statement expressions around the
// parse, one more inside it, and the loops and the switches its
// statements open.  The destructor gives them back however the parse of
// the block ends, so a refusal of the not-yet kind that a caller
// catches, as typedef_array_size() does, leaves no loop, no switch and
// no statement expression open behind it.
class [[nodiscard]] statement_expression_depth_t
{
public:

    explicit statement_expression_depth_t(parser_t &p)
        : m_p(p), m_breaks(p.break_targets), m_continues(p.continue_targets),
          m_switches(p.switches.size())
    {
        ++m_p.statement_expressions;
    }

    ~statement_expression_depth_t()
    {
        --m_p.statement_expressions;
        m_p.break_targets = m_breaks;
        m_p.continue_targets = m_continues;
        m_p.switches.erase(m_p.switches.begin() +
                               static_cast< std::ptrdiff_t >(m_switches),
                           m_p.switches.end());
    }

    statement_expression_depth_t(const statement_expression_depth_t &) = delete;
    statement_expression_depth_t &
    operator=(const statement_expression_depth_t &) = delete;

private:

    parser_t &m_p;
    int m_breaks;
    int m_continues;
    std::size_t m_switches;
};

// GNU C's statement expression, a compound statement in parentheses,
// the '(' at open and the '{' ahead: the block's items in a scope of
// their own, as a compound statement's, 6.8.3p2, and the value, the
// expression of the last item where that is an expression statement,
// decayed, or void, as discarded() leaves an expression statement's; a
// block that ends in anything else, a declaration among them, is void.
// It stands in a function's body alone, where its statements run; an
// initializer, an array size or a parameter list outside one is an
// error, as clang has it.  A value of a record is not yet, as anywhere
// rcc would read the whole object.  Its span goes to the function's,
// for the check of its labels, require_labels().
expr_t
statement_expression(parser_t &p, const token_t &open)
{
    if (!p.temporaries)
        fail(open, "statement expression outside a function body");

    const statement_expression_depth_t depth(p);
    advance(p);
    p.scopes.emplace_back();

    std::vector< stmt_t > body;
    const token_t *last = nullptr;
    bool valued = false;
    while (!next_is(p, { tok_t::rbrace, tok_t::eof })) {
        last = &peek(p);
        valued = block_item(p, body);
    }

    close_block(p);
    expect(p, tok_t::rbrace, "expected '}' at end of block");
    const token_t &close =
        expect(p, tok_t::rparen, "expected ')' after statement expression");
    p.statement_expression_spans.emplace_back(&open, &close);

    if (!valued)
        return expr_t { make_void(),
                        statement_expression_t { std::move(body), nullptr } };

    expr_t value = std::move(*std::get< expr_stmt_t >(body.back().v).e);
    body.pop_back();
    refuse_record_value(*last, value.type);

    type_t t = value.type;

    return expr_t { std::move(t),
                    statement_expression_t { std::move(body),
                                             box(std::move(value)) } };
}

// Whether an undeclared name is one of clang's builtin functions of
// the atomic operations, which the macros of stdatomic.h call:
// __c11_atomic_exchange for atomic_flag_test_and_set, and their like.
bool
is_atomic_builtin(std::string_view name)
{
    return name.starts_with("__c11_atomic_") || name.starts_with("__atomic_");
}

// The type a generic association names is a complete object type,
// 6.5.2.1p2: no function, and no incomplete type, void, an array of
// unknown size or a record before its list among them.
void
require_association_type(const token_t &at, const type_t &t)
{
    if (is< function_t >(t))
        fail(at, "generic association of a function type");

    if (is_incomplete(t))
        fail(at, "generic association of an incomplete type");
}

// Whether two types a generic selection meets are compatible, the
// qualifiers at the top of each read too, 6.7.4.1p11: const int and int
// are not.  A pair rcc cannot tell, of two records of one tag or of a
// type a typedef made through a marking attribute or an array size rcc
// cannot compute, is not yet, as in a comparison.
bool
selection_compatible(const token_t &at,
                     const qualified_type_t &a,
                     const qualified_type_t &b)
{
    if (a.qualifiers == b.qualifiers && compatible(a.type, b.type))
        return true;

    refuse_tag_twins(at, a.type, b.type);
    refuse_made_pointees(at, make_pointer(a.type), make_pointer(b.type),
                         compatibility_words);

    return false;
}

// A generic selection as its associations are read: the controlling
// expression's type and its first token; the types named so far; the
// expression of the association selected, and the default's, with the
// uses of names of internal linkage the default's makes, kept apart
// until the default is known to be the one selected.
struct selection_t
{
    qualified_type_t controlling;
    const token_t *start;
    std::vector< qualified_type_t > named = { };
    std::optional< expr_t > selected = { };
    std::optional< expr_t > fallback = { };
    std::vector< internal_use_t > fallback_uses = { };
};

// default : e, the one default of a selection, 6.5.2.1p2.
void
default_association(parser_t &p, selection_t &s)
{
    const token_t &keyword = advance(p);
    if (s.fallback)
        fail(keyword, "generic selection with a second default association");

    expect(p, tok_t::colon, "expected ':' after 'default'");

    const auto uses = std::ssize(p.internal_uses);
    s.fallback = assignment_expression(p);
    s.fallback_uses.assign(p.internal_uses.begin() + uses,
                           p.internal_uses.end());
    p.internal_uses.erase(p.internal_uses.begin() + uses,
                          p.internal_uses.end());
}

// T : e, 6.5.2.1p2: T a complete object type compatible with no type
// an earlier association names, and e selected when T is compatible
// with the controlling expression's type, which may be so of one T
// alone.  The uses of names of internal linkage of an e not selected
// are dropped, C23's 6.9.1p3.
void
generic_association(parser_t &p, selection_t &s)
{
    if (peek(p).kind == tok_t::kw_default) {
        default_association(p, s);
        return;
    }

    const token_t &start = peek(p);
    const qualified_type_t t = qualified_type_name(p);
    require_association_type(start, t.type);

    for (const qualified_type_t &earlier : s.named)
        if (selection_compatible(start, t, earlier))
            fail(start, "generic association compatible with an earlier one");

    s.named.push_back(t);
    expect(p, tok_t::colon, "expected ':' after the type name");

    const auto uses = std::ssize(p.internal_uses);
    expr_t e = assignment_expression(p);

    if (!selection_compatible(start, t, s.controlling)) {
        p.internal_uses.erase(p.internal_uses.begin() + uses,
                              p.internal_uses.end());
        return;
    }

    if (s.selected)
        fail(*s.start,
             "controlling expression compatible with two generic associations");

    s.selected = std::move(e);
}

// _Generic ( E , association, ... ), 6.5.2.1: the expression of the
// association whose type is compatible with E's, or else the default's,
// p3.  E's type is that of an operand, decayed, and of no qualifier at
// its top, since an lvalue conversion drops them, p2 and its footnote;
// E is not evaluated, p3, so the tree keeps nothing of it, nor of any
// association but the one selected, whose node the selection is, of
// its type and its value, an lvalue where it is one, p4.  The uses of
// names of internal linkage E makes are dropped, as sizeof's are, C23's
// 6.9.1p3.  An E compatible with no association's type and a selection
// of no default is an error, p2.
expr_t
generic_selection(parser_t &p)
{
    const token_t &keyword = advance(p);
    expect(p, tok_t::lparen, "expected '(' after " + quoted(keyword.text));

    const token_t &start = peek(p);
    const auto uses = std::ssize(p.internal_uses);
    selection_t s { { decayed(assignment_expression(p)).type, { } }, &start };
    p.internal_uses.erase(p.internal_uses.begin() + uses,
                          p.internal_uses.end());

    expect(p, tok_t::comma, "expected ',' after the controlling expression");
    do
        generic_association(p, s);
    while (accept(p, tok_t::comma));

    expect(p, tok_t::rparen, "expected ')' after the generic associations");

    if (s.selected)
        return std::move(*s.selected);

    if (!s.fallback)
        fail(start, "controlling expression compatible with no generic "
                    "association");

    p.internal_uses.insert(p.internal_uses.end(), s.fallback_uses.begin(),
                           s.fallback_uses.end());

    return std::move(*s.fallback);
}

// A name must be declared, 6.5.2p2, as an object or a function: a
// typedef name names a type.  An enumeration constant is a constant,
// p2's footnote, of its type and its value, 6.4.4.4p2, 6.7.3.3p10: the
// literal, an integer constant thus.  The use of a symbol an attribute
// marked is refused here, with the name in hand, by the attribute's
// words, and so is a global's of a not-yet type, which an object
// declared elsewhere may have, and a constant's of one, an enum's an
// attribute marked, by the type's; but for a global of a type
// is_sized() says has its size, where the name is the whole operand of
// sizeof, ends_sizeof_operand()'s, as for a member's access.  A use of a name of internal linkage
// is kept for the check of its definition at the end of the unit.
// Parentheses leave the expression as it
// is, an lvalue included, 6.5.2p6, but for GNU C's statement
// expression, ({ ... }), statement_expression()'s.  alloca, which
// glibc's alloca.h defines as __builtin_alloca, is not yet, and so is
// an atomic operation, is_atomic_builtin()'s, by its name.  A floating
// constant is not yet: the lexer took its spelling, and its value is
// not computed.  A character constant is character_constant()'s, and
// adjacent string literals are one, string_literal()'s.
expr_t
primary_expression(parser_t &p)
{
    const token_t &t = peek(p);

    switch (t.kind) {
    case tok_t::identifier: {
        const bool sized_operand = ends_sizeof_operand(p, p.pos);
        advance(p);

        if (t.text == "__builtin_offsetof")
            return offsetof_expression(p, t);

        if (t.text == "__builtin_expect")
            return expect_expression(p, t);

        if (t.text == "__builtin_alloca")
            not_yet(t, "alloca, " + quoted(t.text));

        if (std::optional< expr_t > e = va_builtin(p, t))
            return std::move(*e);

        const symbol_t *sym = lookup(p, t.text);
        if (!sym && is_atomic_builtin(t.text))
            not_yet(t, "atomic operation " + quoted(t.text));

        if (!sym)
            fail(t, quoted(t.text) + " undeclared");

        if (sym->storage == storage_t::type_name)
            fail(t, "unexpected type name " + quoted(t.text) +
                        ": expected expression");

        if (!sym->marking.empty())
            not_yet(t, sym->marking);

        if (sym->linkage != linkage_t::none)
            p.used.insert(sym);

        const bool sized = is_sized(sym->type) && sized_operand;
        if ((sym->storage == storage_t::global ||
             sym->storage == storage_t::enumerator) &&
            !sized)
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
        if (peek(p).kind == tok_t::lbrace)
            return statement_expression(p, t);

        expr_t e = expression(p);
        expect(p, tok_t::rparen, "expected ')'");

        return e;
    }
    case tok_t::kw_generic: return generic_selection(p);
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
