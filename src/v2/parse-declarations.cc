// -*- mode: c++; -*-

// 6.7: the declarations, of which rcc-v2 takes the declaration
// specifiers static, extern, typedef, void and the type specifiers of
// the integer types from char to unsigned long long, in any order and
// spelling, a typedef name, a struct or union specifier,
// parse-records.cc's, and an enum specifier, the pointers to int, to
// pointers, to records and to the arrays of int or of pointers, the
// declarators of a function and of an object, the declaration in a
// block and in a for's first clause of objects of those types, each
// with an initializer or without, a local array's without one, the type
// definitions, the type name of a cast and of sizeof, and the
// initializer of an object of static storage; it refuses the rest as
// not yet.  rcc reads more of each than rcc-v2 does, so each is read as
// far as rcc would read it, and what rcc would take is refused before
// an error rcc would not say.

#include "parse-common.hh"

#include "build.hh"

#include <algorithm>
#include <cstddef>
#include <format>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace rcc {

namespace {

// Whether a declaration specifier starts at the token, as rcc's
// declaration specifiers read one: a storage class, a type specifier
// or a qualifier, a function specifier, an alignment specifier or GNU
// C's attribute, those outside the subset included, so that they are
// refused by name, 6.7.1p1.
bool
is_specifier(const token_t &t)
{
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
    case tok_t::kw_attribute:
    case tok_t::kw_float128:
    case tok_t::kw_int128: return true;
    default: return false;
    }
}

// Whether an identifier at the token may name a type the compiler
// declares, as rcc's builtin types are named: a name reserved for the
// implementation, 6.4.2.1p7, as __builtin_va_list is.
bool
may_name_builtin_type(const token_t &t)
{
    return t.kind == tok_t::identifier && is_reserved(t.text);
}

// The type an identifier names where it stands, or null, rcc's
// named_type(): a typedef name's, 6.7.9p3, where the innermost
// declaration of the name in scope is a typedef, so an object of an
// inner scope hides a typedef name, and a typedef name an object.
const type_t *
named_type(const parser_t &p, const token_t &t)
{
    if (t.kind != tok_t::identifier)
        return nullptr;

    const symbol_t *sym = lookup(p, t.text);

    return sym && sym->type_name ? &sym->operand.type : nullptr;
}

// Whether the current scope declares the name already as an ordinary
// identifier, 6.2.3p1, which no other declaration of no linkage
// declares again there, 6.7.1p4: among its names, and the file's scope
// among the file's functions and objects too.
bool
declares(const parser_t &p, std::string_view name)
{
    if (p.scopes.back().names.contains(name))
        return true;

    return p.scopes.size() == 1 &&
           (p.objects.contains(name) || p.functions.contains(name));
}

} // namespace

// Whether a declaration starts at the token ahead: a declaration
// specifier, which __extension__ may stand before, or a static
// assertion, a declaration of its own, 6.7.1p1.  An identifier starts
// one where it is a typedef name in scope, but before a colon, where it
// is a label, since labels have a name space of their own, 6.2.3p1.
// rcc-v2 does not know the names of the types the compiler declares,
// __builtin_va_list among them, so no other identifier starts one: such
// a name is reserved, and an expression statement it starts refuses
// it, identifier().
bool
is_declaration_start(const parser_t &p, std::size_t ahead)
{
    const token_t &t = peek(p, ahead);
    if (t.kind == tok_t::identifier)
        return named_type(p, t) && peek(p, ahead + 1).kind != tok_t::colon;

    if (t.kind == tok_t::kw_extension)
        return is_declaration_start(p, ahead + 1);

    return t.kind == tok_t::kw_static_assert || is_specifier(t);
}

namespace {

// The type specifiers of 6.7.3.1p2 that rcc counts, as rcc's
// declaration_specifiers() counts them, void beside them: each a count
// of its keyword, so that the order of the keywords does not matter.
struct counts_t
{
    int void_ = 0;
    int char_ = 0;
    int short_ = 0;
    int int_ = 0;
    int long_ = 0;
    int signed_ = 0;
    int unsigned_ = 0;
};

// Whether the type specifier at t cannot join those counted before it,
// rcc's: void joins none, nor any one void; char joins no char, short,
// int or long; short no char, short or long; int no char or int; long no
// char or short, nor two longs; signed and unsigned neither of the two.
// Counts it if it is one.
bool
cannot_join(counts_t &n, const token_t &t)
{
    const bool bad = n.void_ > 0;

    switch (t.kind) {
    case tok_t::kw_void:
        ++n.void_;
        return bad || n.char_ || n.short_ || n.int_ || n.long_ || n.signed_ ||
               n.unsigned_;
    case tok_t::kw_char:
        ++n.char_;
        return bad || n.char_ > 1 || n.short_ || n.int_ || n.long_;
    case tok_t::kw_short:
        ++n.short_;
        return bad || n.char_ || n.short_ > 1 || n.long_;
    case tok_t::kw_int: ++n.int_; return bad || n.char_ || n.int_ > 1;
    case tok_t::kw_long:
        ++n.long_;
        return bad || n.char_ || n.short_ || n.long_ > 2;
    case tok_t::kw_signed:
    case tok_t::kw_unsigned: {
        const bool either = n.signed_ || n.unsigned_;
        ++(t.kind == tok_t::kw_signed ? n.signed_ : n.unsigned_);
        return bad || either;
    }
    default: return false;
    }
}

// Whether a token is a storage-class specifier rcc-v2 reads, 6.7.2p1:
// static, extern, or typedef, which p1 lists with them and which
// declares typedef names, 6.7.9p3.
bool
is_storage_class(const token_t &t)
{
    return t.kind == tok_t::kw_static || t.kind == tok_t::kw_extern ||
           t.kind == tok_t::kw_typedef;
}

// Whether a token is one of the type specifiers counts_t counts.
bool
is_counted(const token_t &t)
{
    switch (t.kind) {
    case tok_t::kw_void:
    case tok_t::kw_char:
    case tok_t::kw_short:
    case tok_t::kw_int:
    case tok_t::kw_long:
    case tok_t::kw_signed:
    case tok_t::kw_unsigned: return true;
    default: return false;
    }
}

// Plain char is signed or unsigned as the target has it, and a type of
// its own, 6.2.5p20, rcc's char_type().
type_t
char_type(const parser_t &p)
{
    return make_int(8, p.target.char_is_signed, int_kind_t::char_);
}

// The type the counted specifiers name, rcc's specified(): void; char
// with neither signed nor unsigned plain char; else signed unless
// unsigned was counted, and the width the first that applies, char 8,
// short 16, two longs long long, one long long, and int otherwise,
// signed alone among them, 6.7.3.1p2.
type_t
specified(const parser_t &p, const counts_t &n)
{
    if (n.void_)
        return make_void();

    if (n.char_ && !n.signed_ && !n.unsigned_)
        return char_type(p);

    const bool is_signed = n.unsigned_ == 0;
    const auto of = [&](int width, int_kind_t s, int_kind_t u) {
        return make_int(width, is_signed, is_signed ? s : u);
    };

    if (n.char_)
        return of(8, int_kind_t::signed_char, int_kind_t::unsigned_char);

    if (n.short_)
        return of(16, int_kind_t::short_, int_kind_t::unsigned_short);

    if (n.long_ == 2)
        return of(64, int_kind_t::long_long, int_kind_t::unsigned_long_long);

    if (n.long_)
        return of(64, int_kind_t::long_, int_kind_t::unsigned_long);

    return of(32, int_kind_t::int_, int_kind_t::unsigned_);
}

// An enumeration constant while its list is read: its name, its type
// and its value exactly, which the enumerated type must hold, rcc's
// enumerator_t.
struct enumerator_t
{
    std::string_view name;
    type_t type;
    exact_t value;
};

// What a use of an enumeration constant copies: the literal of its
// value, of its type, 6.4.4.4p2, so no name in the tree is an
// enumeration constant's, and the constant is an integer constant
// expression wherever one is read.
operand_t
constant(const type_t &t, exact_t value)
{
    return { t, literal_of(t, value) };
}

// The type and the value of an enumeration constant while its list is
// read, 6.7.3.3p10, p12, rcc's enumerator_value().  After = they are
// those of the integer constant expression, p4, but an int where int
// holds the value: its value read through the model, constant_of(), an
// error at its first token where it folds to no constant or to one of no
// integer type, warned of where a signed overflow reached it,
// warn_overflowed().  Without =, the first constant is the int 0, and
// each other the previous one's value plus one, in the previous one's
// type, or in the 64-bit type of its sign where that type does not hold
// the sum; no type holds a sum past the 64-bit types, an error at the
// name, p12 and its footnote.
enumerator_t
enumerator_value(parser_t &p, const token_t &name, const enumerator_t *previous)
{
    if (accept(p, tok_t::assign)) {
        const token_t &start = peek(p);
        const operand_t e = conditional_expression(p);
        const std::optional< long long > bits = constant_of(rvalue(start, e));
        if (!bits || !is< int_t >(e.type))
            fail(start,
                 "enumerator value is not an integer constant expression");

        warn_overflowed(p, start, e);

        const exact_t value = exact_value(e.type, *bits);
        if (holds(int_type(), value))
            return { name.text, int_type(), value };

        return { name.text, e.type, value };
    }

    if (!previous)
        return { name.text, int_type(), 0 };

    const exact_t next = previous->value + 1;
    if (holds(previous->type, next))
        return { name.text, previous->type, next };

    const bool is_signed = as< int_t >(previous->type).is_signed;
    const type_t wider =
        make_int(64, is_signed,
                 is_signed ? int_kind_t::long_ : int_kind_t::unsigned_long);
    if (!holds(wider, next))
        fail(name, "value of " + quoted(name.text) +
                       " is too large for any integer type");

    return { name.text, wider, next };
}

// One or more enumerators, the list perhaps ending in a comma,
// 6.7.3.3p1, rcc's enumerator_list().  Each declares its enumeration
// constant as an ordinary identifier, 6.2.3p1, in the current scope,
// once there, 6.7.1p4, else an error at the second, of the type and the
// value enumerator_value() gives it.  Its scope begins after its
// enumerator, 6.2.1p7, so its own value cannot name it and the next
// one's can.
std::vector< enumerator_t >
enumerator_list(parser_t &p)
{
    std::vector< enumerator_t > list;

    do {
        const token_t &name =
            expect(p, tok_t::identifier, "expected identifier");
        if (declares(p, name.text))
            fail(name, "redefinition of " + quoted(name.text));

        enumerator_t e =
            enumerator_value(p, name, list.empty() ? nullptr : &list.back());
        p.scopes.back().names.emplace(
            name.text, symbol_t { .operand = constant(e.type, e.value) });
        list.push_back(std::move(e));
    } while (accept(p, tok_t::comma) && peek(p).kind != tok_t::rbrace);

    return list;
}

// The integer type compatible with an enumerated type, 6.7.3.3p13, the
// choice J.3.10 leaves to the implementation, as clang makes it on both
// targets, rcc's compatible_type(): unsigned int when no value is
// negative and it holds every one, else unsigned long; int when a value
// is negative and int holds every one, else long.  No type holds a
// negative value and one past LONG_MAX together, p4, an error at the
// tag, or at the keyword of an enum without one.
type_t
compatible_type(const token_t &at, const std::vector< enumerator_t > &list)
{
    exact_t least = list.front().value;
    exact_t most = least;
    for (const enumerator_t &e : list) {
        least = std::min(least, e.value);
        most = std::max(most, e.value);
    }

    const bool negative = least < 0;
    const type_t types[] = {
        make_int(32, negative,
                 negative ? int_kind_t::int_ : int_kind_t::unsigned_),
        make_int(64, negative,
                 negative ? int_kind_t::long_ : int_kind_t::unsigned_long)
    };

    for (const type_t &t : types)
        if (holds(t, least) && holds(t, most))
            return t;

    const std::string what =
        at.kind == tok_t::identifier ? "enum " + quoted(at.text) : "the enum";
    fail(at, "no integer type holds every value of " + what);
}

// The enumerated type a list defines, 6.2.5p21, rcc's enumerated(): the
// machine's view and the kind of its compatible integer type, and an
// identity of its own, which compatible() and same_type() read and no
// row does.
type_t
enumerated(parser_t &p, const type_t &compatible)
{
    const int_t &t = as< int_t >(compatible);

    return make_int(t.width, t.is_signed, t.kind, ++p.identities);
}

// The constants of an enumeration whose list is complete take its
// member type, 6.7.3.3p15, rcc's complete_constants(): int when int
// holds every value, else the enumerated type, t; each a literal of that
// type again, in the current scope, which declared them.
void
complete_constants(parser_t &p,
                   const std::vector< enumerator_t > &list,
                   const type_t &t)
{
    const bool ints = std::ranges::all_of(
        list, [](const enumerator_t &e) { return holds(int_type(), e.value); });
    const type_t member = ints ? int_type() : t;

    for (const enumerator_t &e : list)
        p.scopes.back().names.at(e.name).operand = constant(member, e.value);
}

// C23's enum type specifier, a ':' and a type's specifiers after enum or
// its tag, 6.7.3.3p1, which fixes the enumerated type, p2, is not yet in
// rcc's words, rcc's refuse_fixed_underlying_type().
void
refuse_fixed_underlying_type(const parser_t &p)
{
    if (peek(p).kind == tok_t::colon && is_declaration_start(p, 1))
        not_yet(peek(p), "enum with a fixed underlying type");
}

// The tag of an enum whose list follows, declared in the current scope
// from the list's '{' on, 6.2.1p7, by tagged_type(), as the incomplete
// type it is until the '}', 6.7.3.4p5, rcc's open_enum_tag(): a not-yet
// type, whose every use the list makes is refused by its words,
// refuse_not_yet_type().
void
open_enum_tag(parser_t &p, const token_t &keyword, const token_t &tag)
{
    tagged_type(p, keyword, tag, tag_use_t::defines);

    tag_in(p.scopes.back(), tag.text)->type = make_not_yet(
        ++p.identities, { "enum " + quoted(tag.text) + " in its own list" });
}

// enum, then a tag, an enumerator list in braces, or both, 6.7.3.3p1,
// rcc's enum_specifier().  A list defines an enumerated type, and the
// tree keeps the integer type compatible with it, compatible_type()'s,
// of an identity of its own, enumerated()'s, so an object, a parameter,
// a result, a cast, sizeof, a switch and the arithmetic of one are its
// integer type's; its constants take the types and the values of p10,
// p12 and p15, and the tag, in the current scope, the enumerated type.
// A tag without a list names the type its list defined, and is an error
// before the list, 6.7.3.4p2, tagged_type()'s: GNU C's forward
// declaration, enum e;, is not taken.  GNU C's attributes after the
// keyword are not yet, and so are those after the list, which the
// specifiers refuse.
type_t
enum_specifier(parser_t &p)
{
    const token_t &keyword = advance(p);
    if (peek(p).kind == tok_t::kw_attribute)
        not_yet(peek(p), quoted(peek(p).text) + " in an enum specifier");

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

    const type_t t = enumerated(p, compatible_type(tag ? *tag : keyword, list));
    complete_constants(p, list, t);

    if (tag)
        tag_in(p.scopes.back(), tag->text)->type = t;

    return t;
}

} // namespace

bool
declared(const specifiers_t &spec, tok_t storage_class)
{
    return spec.storage_class && spec.storage_class->kind == storage_class;
}

void
refuse_static_pointer(const token_t &name, const type_t &t)
{
    if (is< pointer_t >(t))
        not_yet(name, "pointer " + quoted(name.text) + " of static storage");
}

void
refuse_static_array(const token_t &name, const type_t &t)
{
    if (is< array_t >(t))
        not_yet(name, "array " + quoted(name.text) + " of static storage");
}

// The declaration specifiers: one of the sets of 6.7.3.1p2 in any
// order, and the storage class static, extern or typedef anywhere among
// them, 6.7.2p1, read as rcc reads them, up to the first token that is
// none.  A typedef name in scope is the whole type specifier,
// 6.7.3.1p2, where it is the first: after another, an identifier is the
// declarator's name, so unsigned T declares a T.  So is a struct, union
// or enum specifier, struct_or_union_specifier()'s and
// enum_specifier()'s, which another may stand neither before nor
// after.  Each type specifier is refused where it cannot join those
// before it, and a storage class where one came before it, 6.7.2p2, in
// rcc's words.  Any other specifier is not yet where it stands, and so
// is a reserved identifier in place of the first type specifier, which
// may name a type the compiler declares; any other token there is rcc's
// error, another identifier's among them.
specifiers_t
declaration_specifiers(parser_t &p)
{
    const token_t *storage_class = nullptr;
    counts_t counts;
    const token_t *first = nullptr;
    std::optional< type_t > whole;
    bool tagged = false;

    for (;;) {
        const token_t &t = peek(p);
        if (is_storage_class(t)) {
            if (storage_class)
                fail(t, "cannot combine " + quoted(t.text) +
                            " with the storage class before it");

            storage_class = &advance(p);
            continue;
        }

        if (!first && named_type(p, t)) {
            whole = *named_type(p, t);
            first = &advance(p);
            continue;
        }

        if (next_is(p, { tok_t::kw_struct, tok_t::kw_union, tok_t::kw_enum })) {
            if (first)
                fail(t, "cannot combine " + quoted(t.text) +
                            " with the type specifiers before it");

            first = &t;
            whole = t.kind == tok_t::kw_enum ? enum_specifier(p) :
                                               struct_or_union_specifier(p);
            tagged = true;
            continue;
        }

        if (!is_counted(t))
            break;

        advance(p);
        if (cannot_join(counts, t) || whole)
            fail(t, "cannot combine " + quoted(t.text) +
                        " with the type specifiers before it");

        if (!first)
            first = &t;
    }

    const token_t &t = peek(p);
    if (is_specifier(t) || (!first && may_name_builtin_type(t)))
        not_yet(t, "declaration specifier " + quoted(t.text));

    if (!first)
        fail(t, "expected a type specifier");

    return { storage_class, *first, whole ? *whole : specified(p, counts),
             tagged };
}

namespace {

// A declarator in parentheses is not yet, refused at its '('.
void
refuse_parenthesized(const parser_t &p)
{
    if (peek(p).kind == tok_t::lparen)
        not_yet(peek(p), "declarator in parentheses");
}

// The pointers before a declarator's name, 6.7.7.2, rcc's pointer():
// each '*' a pointer to the type so far, so int **pp is a pointer to a
// pointer to int.  The pointees rcc-v2 takes are the integer types, the
// enumerated ones among them, the pointers, the arrays a typedef name
// names, and the records, complete or not, 6.2.5p27, laid out or not:
// a pointer to void, to a function, which a typedef name may name, or
// to an enum in its own list, which rcc takes, is not yet at its '*',
// and so is a qualifier after one, which qualifies the pointer, p1.
type_t
pointer(parser_t &p, type_t t)
{
    while (peek(p).kind == tok_t::star) {
        const token_t &star = advance(p);
        if (is< void_t >(t))
            not_yet(star, "pointer to 'void'");

        if (is< function_t >(t))
            not_yet(star, "pointer to a function");

        refuse_not_yet_type(star, t);

        t = make_pointer(std::move(t));

        const token_t &q = peek(p);
        if (next_is(p, { tok_t::kw_const, tok_t::kw_volatile,
                         tok_t::kw_restrict, tok_t::kw_atomic }))
            not_yet(q, "type qualifier " + quoted(q.text) + " of a pointer");
    }

    return t;
}

// The count of an array from its size, rcc's array_count(): an
// expression of integer type, 6.7.7.3p1, a constant one by now, of a
// value greater than zero, each an error at the size's first token, at.
// An unsigned size past LONG_MAX is LONG_MAX, which no element's size
// lets through as an array's.
long
array_count(const token_t &at, const operand_t &size)
{
    if (!is< int_t >(size.type))
        fail(at, "size of array has non-integer type");

    const exact_t value = exact_value(size.type, *constant_of(*size.node));
    if (value <= 0)
        fail(at, "array size must be greater than zero");

    return static_cast< long >(
        std::min< exact_t >(value, std::numeric_limits< long >::max()));
}

// The size of an array suffix, rcc's array_size(): an assignment
// expression, 6.7.7.1p1, read for its value, and warned of where a
// constant expression overflowed, warn_overflowed()'s.
operand_t
array_size(parser_t &p)
{
    const token_t &start = peek(p);

    operand_t size = assignment_expression(p);
    size.node = rvalue(start, size);
    if (constant_of(*size.node))
        warn_overflowed(p, start, size);

    return size;
}

// static and the type qualifiers stand only in the outermost array
// type derivation of a parameter, 6.7.7.3p1, rcc's
// refuse_array_qualifier(), and are refused in any other brackets.
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

// What a parameter's brackets hold before a size, 6.7.7.3p3, which rcc
// takes and rcc-v2 does not yet: static and the type qualifiers in the
// outermost, the one the parameter is adjusted from, and '*', a
// variable length array of unspecified size, p4, in any; outside a
// parameter the first two are refuse_array_qualifier()'s errors, and
// '*' rcc's error.
void
refuse_bracket_contents(const parser_t &p, bool parameter, bool outermost)
{
    const token_t &t = peek(p);
    const bool star =
        t.kind == tok_t::star && peek(p, 1).kind == tok_t::rbracket;

    if (parameter && star)
        not_yet(t, "'*' as an array size");

    if (star)
        fail(t, "'*' as an array size outside a parameter declaration");

    if (!(parameter && outermost))
        refuse_array_qualifier(t);
    else if (next_is(p, { tok_t::kw_static, tok_t::kw_const, tok_t::kw_volatile,
                          tok_t::kw_restrict, tok_t::kw_atomic }))
        not_yet(t, quoted(t.text) + " in an array parameter's brackets");
}

// An array suffix as written: its '['; its count, -1 where its size is
// left out; and whether its size is no constant, a variable length
// array's.
struct array_suffix_t
{
    const token_t &open;
    long count;
    bool variable = false;
};

// The elements rcc-v2 takes are those of a pointer: int and the
// pointers, 6.7.7.3p1.  An array of arrays, whose rows no form of an
// index reaches yet, and of any other type are not yet, refused at the
// '[' of the derivation that made the element, or where a typedef name
// made it, at the '[' that derives from it.
void
refuse_element(const token_t &at, const type_t &t)
{
    refuse_not_yet_type(at, t);

    if (is< array_t >(t))
        not_yet(at, "array of arrays");

    if (!is< pointer_t >(t) && t != int_type())
        not_yet(at, "array of an integer type other than 'int'");
}

// One or more [N], N an integer constant expression, or [] for an array
// of unknown size, 6.7.7.3p1, p4, rcc's array_declarator(), deriving
// from the element type t: the suffixes read first, then the type built
// from the right, so int m[2][3] is an array of two arrays of three,
// p3, an element of a complete object type, p1, each an error at its
// '[', and of a count whose size in bytes a long holds.  A size that is
// no integer constant expression makes a variable length array, p4,
// which rcc compiles and rcc-v2 does not yet: its first token is
// returned in variable, for the declaration to refuse in its words, and
// the array is of unknown size; an array of such arrays is rcc's not
// yet, at its '['.  parameter says that the suffixes are a parameter's,
// whose first is the array the parameter is adjusted from, 6.7.7.4p6:
// its size may be left out, and a size that is no constant, treated as
// '*' at prototype scope, 6.7.7.3p5, and evaluated on entry to a
// definition, is not yet; a variable length array past it is behind the
// parameter's pointer, rcc's not yet.
type_t
array_declarator(parser_t &p,
                 const type_t &t,
                 bool parameter,
                 const token_t *&variable)
{
    std::vector< array_suffix_t > suffixes;

    while (peek(p).kind == tok_t::lbracket) {
        array_suffix_t suffix { advance(p), -1 };
        const bool outermost = suffixes.empty();
        refuse_bracket_contents(p, parameter, outermost);

        if (peek(p).kind != tok_t::rbracket) {
            const token_t &start = peek(p);
            const operand_t size = array_size(p);
            const bool constant = constant_of(*size.node).has_value();

            if (is< int_t >(size.type) && !constant && parameter)
                not_yet(start, outermost ?
                                   "an array parameter's size that is no "
                                   "constant" :
                                   "pointer to variable length array");

            suffix.variable = is< int_t >(size.type) && !constant;
            if (suffix.variable && !variable)
                variable = &start;

            if (!suffix.variable)
                suffix.count = array_count(start, size);
        }

        expect(p, tok_t::rbracket, "expected ']'");
        suffixes.push_back(suffix);
    }

    type_t element = t;
    const array_suffix_t *inner = nullptr;

    for (auto it = suffixes.rbegin(); it != suffixes.rend(); ++it) {
        if (is< function_t >(element))
            fail(it->open, "array element type cannot be a function");

        if (inner && inner->variable)
            not_yet(it->open, "array of variable length arrays");

        if (is< void_t >(element) ||
            (is< array_t >(element) && as< array_t >(element).count < 0))
            fail(it->open, "array has incomplete element type");

        refuse_element(inner ? inner->open : it->open, element);

        if (it->count > std::numeric_limits< long >::max() / size_of(element))
            fail(it->open, "array is too large");

        element = make_array(std::move(element), it->count);
        inner = &*it;
    }

    return element;
}

// The declarator of a parameter past its pointers, of the type t so
// far, whose name is optional, 6.7.7.1p1: the name, or null, and the
// parameter's type, an array adjusted to a pointer to its element,
// 6.7.7.4p6, whatever made the array, its suffix or a typedef name.  A
// function, adjusted to a pointer to it, p7, is not yet.
std::pair< const token_t *, type_t >
parameter_declarator(parser_t &p, type_t t)
{
    refuse_parenthesized(p);

    const token_t *name =
        peek(p).kind == tok_t::identifier ? &advance(p) : nullptr;

    const token_t *variable = nullptr;
    if (peek(p).kind == tok_t::lbracket) {
        t = array_declarator(p, t, true, variable);
        if (peek(p).kind == tok_t::lparen)
            fail(peek(p), "array element type cannot be a function");
    }
    else if (peek(p).kind == tok_t::lparen)
        not_yet(peek(p), "parameter of function type");

    if (is< array_t >(t))
        t = make_pointer(*as< array_t >(t).element);

    return { name, std::move(t) };
}

// A parameter's specifiers and declarator, its pointers first,
// 6.7.7.1p1, and GNU C's attributes after it, which are not yet.
// register is the one storage class a parameter takes, p2, and static,
// extern and typedef are refused in rcc's words; register itself is not
// yet, as a specifier.  A parameter of type void is left for the list to
// take or refuse; one of a function type a typedef name names, adjusted
// to a pointer to the function, 6.7.7.4p7, is not yet, as one whose
// declarator writes the type is.
parameter_t
parameter_declaration(parser_t &p)
{
    const token_t &start = peek(p);

    const specifiers_t spec = declaration_specifiers(p);
    if (spec.storage_class)
        fail(*spec.storage_class,
             quoted(spec.storage_class->text) + " in a parameter declaration");

    const auto [name, t] = parameter_declarator(p, pointer(p, spec.type));
    if (peek(p).kind == tok_t::kw_attribute)
        not_yet(peek(p), quoted(peek(p).text) + " after a declarator");

    if (is< function_t >(t))
        not_yet(start, "parameter of function type");

    return { start, name, t };
}

// A parameter's name, declared in the scope of its list, which the
// parameters of one list share, 6.7.1p4, 6.2.1p4, whether a body
// follows or not: a name declared twice there is an error at the
// second, which names no parameter where the first is an enumeration
// constant a parameter's type declared, the one name of the list whose
// operand is a constant.  The parameter's leaf is a local's, its slot
// numbered by its place in the list, so the parameters of a definition
// are its first locals, and its body's are numbered after them
// (docs/typed-tree.md 4.5); a record parameter has none, since a
// definition of one is not yet, function_definition()'s, before its
// body.
void
declare_parameter(parser_t &p, const parameter_t &param, int index)
{
    if (!param.name)
        return;

    scope_t &scope = p.scopes.back();
    if (const auto it = scope.names.find(param.name->text);
        it != scope.names.end())
        fail(*param.name,
             it->second.operand.node && constant_of(*it->second.operand.node) ?
                 "redefinition of " + quoted(param.name->text) :
                 "redefinition of parameter " + quoted(param.name->text));

    if (record_in(param.type)) {
        scope.names.emplace(param.name->text,
                            symbol_t { { param.type, std::nullopt } });
        return;
    }

    const operand_t local { param.type, make_local(index, view_of(param.type),
                                                   sign_of(param.type)) };
    scope.names.emplace(param.name->text, symbol_t { local });
}

// (void), (), or parameters, read in the scope the caller opened for
// the list, each declared as it is read.  An empty list is (void) in
// C23, 6.7.7.4p13.  An unnamed parameter of type void as the only one
// is no parameter, p9; void stands nowhere else.  A variadic function,
// ", ...", is not yet, and with no named parameter rcc's own not yet.
void
parameter_type_list(parser_t &p, std::vector< parameter_t > &params)
{
    if (accept(p, tok_t::rparen))
        return;

    do {
        if (peek(p).kind == tok_t::ellipsis)
            not_yet(peek(p), params.empty() ?
                                 "variadic function without named parameters" :
                                 "variadic function");

        parameter_t param = parameter_declaration(p);
        if (is< void_t >(param.type)) {
            if (params.empty() && !param.name && accept(p, tok_t::rparen))
                return;

            fail(param.start, "'void' must be the only parameter");
        }

        declare_parameter(p, param, static_cast< int >(params.size()));
        params.push_back(std::move(param));
    } while (accept(p, tok_t::comma));

    expect(p, tok_t::rparen, "expected ')' after parameter list");
}

// The suffix after a declarator's name, which makes d's type of base:
// none, an object's, base itself; an array's, array_declarator()'s,
// whose element is no function, 6.7.7.3p1, an error at a list after
// it; or a parameter list, a function returning base of its
// parameters' types, 6.7.7.4p4, whose list opens a scope at its '(' and
// closes it at its ')', the function prototype scope of 6.2.1p4, which
// d keeps for a definition.  A function returns neither an array nor a
// function, 6.7.7.4p1, which rcc says at the suffix after the list,
// and at the list's '(' where base is the array or the function a
// typedef name names.
void
declarator_suffix(parser_t &p, declarator_t &d, const type_t &base)
{
    if (peek(p).kind == tok_t::lbracket) {
        d.type = array_declarator(p, base, false, d.variable_size);
        if (peek(p).kind == tok_t::lparen)
            fail(peek(p), "array element type cannot be a function");

        return;
    }

    const token_t &open = peek(p);
    if (!accept(p, tok_t::lparen))
        return;

    p.scopes.emplace_back();
    parameter_type_list(p, d.params);
    d.scope = std::move(p.scopes.back());
    p.scopes.pop_back();

    if (peek(p).kind == tok_t::lbracket)
        fail(peek(p), "function cannot return an array");

    if (peek(p).kind == tok_t::lparen)
        fail(peek(p), "function cannot return a function");

    if (is< array_t >(base))
        fail(open, "function cannot return an array");

    if (is< function_t >(base))
        fail(open, "function cannot return a function");

    std::vector< type_t > types;
    for (const parameter_t &param : d.params)
        types.push_back(param.type);

    d.type = make_function(base, std::move(types), false);
}

} // namespace

// A declarator of a function or of an object, of the type base: its
// pointers, then its name, which a declarator must write in a
// declaration, 6.7.7.1p1, and its suffix, which makes the type of the
// pointers' type, so int *f(void) is a function returning a pointer.
declarator_t
declarator(parser_t &p, const type_t &base)
{
    const type_t t = pointer(p, base);
    refuse_parenthesized(p);

    declarator_t d { expect(p, tok_t::identifier, "expected identifier"), t };
    declarator_suffix(p, d, t);

    return d;
}

// A type name, 6.7.8p1: the specifiers, of which no storage class, in
// rcc's words, then an abstract declarator, a declarator without its
// name, of which rcc-v2 takes the pointers and the arrays, (int *[5]),
// whose element is no function: a declarator in parentheses is not yet,
// refused at its first token, and so is a variable length array, in
// rcc's words at the type name's first token.
type_t
type_name(parser_t &p)
{
    const token_t &start = peek(p);
    const specifiers_t spec = declaration_specifiers(p);
    if (spec.storage_class)
        fail(*spec.storage_class,
             quoted(spec.storage_class->text) + " in a type name");

    type_t t = pointer(p, spec.type);
    refuse_parenthesized(p);
    if (peek(p).kind != tok_t::lbracket)
        return t;

    const token_t *variable = nullptr;
    t = array_declarator(p, t, false, variable);
    if (peek(p).kind == tok_t::lparen)
        fail(peek(p), "array element type cannot be a function");

    if (variable)
        not_yet(start, "variable length array in a type name");

    return t;
}

namespace {

// The value of an object of static storage, 6.7.11p4, the expression
// after its '=' at, converted to its type t as if by assignment, p12:
// the constant the builders folded it to, rcc's scalar_value(), warned
// of where a signed overflow reached it, warn_overflowed().  A void
// expression is require_value()'s error at its first token, a function
// designator assignment_conversion()'s at the '=', and any other
// expression that folded to no constant is no constant expression, an
// error at its first token.
long long
scalar_value(parser_t &p, const token_t &at, const type_t &t)
{
    const token_t &start = peek(p);
    const operand_t e = assignment_expression(p);
    require_value(start, e);

    const std::optional< long long > value =
        constant_of(assignment_conversion(at, e, t));
    if (!value)
        fail(start, "initializer element is not a compile-time constant");

    warn_overflowed(p, start, e);

    return *value;
}

} // namespace

// A brace initializer is not yet.
void
initialize(parser_t &p, const type_t &t, std::size_t index)
{
    const token_t &op = advance(p);
    if (peek(p).kind == tok_t::lbrace)
        not_yet(peek(p), "brace initializer");

    p.unit.objects[index].value = scalar_value(p, op, t);
}

namespace {

// rcc reads GNU C's attributes and an assembler name after a
// declarator, which are not yet.
void
refuse_declarator_tail(const parser_t &p)
{
    if (next_is(p, { tok_t::kw_attribute, tok_t::kw_asm }))
        not_yet(peek(p), quoted(peek(p).text) + " after a declarator");
}

// A local array's declarator and its initializer, appended to items,
// rcc's init_declarator() of an array: a variable length array is not
// yet at its size; a brace initializer is not yet, in rcc's words, and
// so is a string, as a primary expression; any other initializer is no
// list and no string, 6.7.11p17, an error at its first token once the
// name is declared, and without one the array's type is complete,
// 6.7.1p8, so its size is written, an error at its name otherwise.  Its
// declaration takes the slot and stores nothing.
void
local_array(parser_t &p, const declarator_t &d, std::vector< stmt_t > &items)
{
    if (d.variable_size)
        not_yet(*d.variable_size, "variable length array");

    const bool initialized = peek(p).kind == tok_t::assign;
    if (initialized && peek(p, 1).kind == tok_t::lbrace)
        not_yet(peek(p, 1), "brace initializer");

    if (!initialized && as< array_t >(d.type).count < 0)
        fail(d.name, "array size missing in " + quoted(d.name.text));

    const int slot = declare(p, d.name, d.type);
    if (initialized) {
        advance(p);

        const token_t &start = peek(p);
        assignment_expression(p);
        fail(start, "array initializer must be an initializer list");
    }

    items.push_back(make_array_decl(slot, size_of(d.type), align_of(d.type)));
}

// A declarator of an object and its initializer, appended to items: the
// name declared, then, after '=', the initializer, whose scope holds
// the name already, 6.2.1p7.  A brace initializer is not yet, in rcc's
// words.  The initializer's value is converted to the object's type as
// if by assignment, 6.7.11p12, assignment_conversion()'s at the '=',
// but a void value, which rcc refuses at the initializer's first token.
// An array is local_array()'s.
void
init_declarator(parser_t &p,
                const declarator_t &d,
                std::vector< stmt_t > &items)
{
    refuse_declarator_tail(p);
    refuse_record_object(d.name, d.type);

    if (is< array_t >(d.type)) {
        local_array(p, d, items);
        return;
    }

    const type_t &type = d.type;
    const int slot = declare(p, d.name, type);
    if (peek(p).kind != tok_t::assign) {
        items.push_back(make_decl(slot, view_of(type)));
        return;
    }

    const token_t &op = advance(p);
    if (peek(p).kind == tok_t::lbrace)
        not_yet(peek(p), "brace initializer");

    const token_t &start = peek(p);
    operand_t init = assignment_expression(p);
    require_value(start, init);

    items.push_back(make_init(
        slot, view_of(type), assignment_conversion(op, std::move(init), type)));
}

// The label of a block's static object, rcc's static_label(),
// backend.md chapter 10: the function's name and the object's, f.n,
// and from the second of the name in the function on a number after
// them, counting from 1, f.n.1, whatever the block.  No C identifier
// holds a '.', so the label is no symbol's of the file and no other
// static object's.
std::string
static_label(parser_t &p, std::string_view name)
{
    const int n = p.statics[name]++;

    std::string label = std::format("{}.{}", p.function, name);
    if (n > 0)
        label += std::format(".{}", n);

    return label;
}

// An object a block declares static, rcc's static_local(): of static
// storage duration, 6.2.4p3, and no linkage, 6.2.2p6, so a name of the
// block's, which may hide another and be hidden as a local's is, and is
// declared once in its block, 6.7.1p4.  It is an object the unit
// defines, as one at file scope is, its entry in the unit's objects
// made where its declaration is parsed, of internal binding, under its
// own label, and its leaf a global of the label, which every access
// reaches as a defined object's: its initializer is constant, read as a
// global's, and gives its value once, before the program starts, and
// without one it is zero, 6.7.11p11.  Its declaration makes no
// statement.  The name is in scope from the end of its declarator,
// 6.2.1p7, so the initializer sees the object.
void
static_local(parser_t &p, const declarator_t &d)
{
    refuse_declarator_tail(p);
    refuse_static_pointer(d.name, d.type);
    refuse_static_array(d.name, d.type);
    refuse_record_object(d.name, d.type);

    scope_t &scope = p.scopes.back();
    if (scope.names.contains(d.name.text))
        fail(d.name, "redefinition of " + quoted(d.name.text));

    const std::string label = static_label(p, d.name.text);
    const view_t view = view_of(d.type);
    const sign_t sign = sign_of(d.type);

    const operand_t object { d.type, make_global(label, view, sign) };
    scope.names.emplace(d.name.text, symbol_t { object });

    const std::size_t index = p.unit.objects.size();
    p.unit.objects.push_back({ label, false, view, sign });

    if (peek(p).kind == tok_t::assign)
        initialize(p, d.type, index);
}

// The entity of linkage the unit has of an object's name, or null,
// rcc's unit_entity(): the file's object of the name, or the one a
// block declared extern first.
object_name_t *
unit_entity(parser_t &p, std::string_view name)
{
    if (const auto it = p.objects.find(name); it != p.objects.end())
        return &it->second;

    const auto it = p.block_entities.find(name);

    return it == p.block_entities.end() ? nullptr : &it->second;
}

// The entity a declaration in a block of a name of linkage denotes,
// rcc's linked_entity(): the scopes from the innermost out, the file's
// the outermost, the first that declares the name deciding: one of
// linkage, a block's extern or the file's, gives the entity's linkage,
// 6.2.2p4; one of no linkage, a local, a parameter or a block's static,
// stops the walk, and the linkage is external.  Every declaration of
// linkage of the name denotes the unit's one entity, unit_entity()'s,
// or none yet.  external is the linkage the declaration gives the name.
object_name_t *
linked_entity(parser_t &p, std::string_view name, bool &external)
{
    object_name_t *entity = unit_entity(p, name);
    external = true;

    for (auto scope = p.scopes.rbegin(); scope != p.scopes.rend(); ++scope) {
        const auto it = scope->names.find(name);
        if (it == scope->names.end())
            continue;

        if (!it->second.linked)
            return entity;

        break;
    }

    if (entity)
        external = entity->external;

    return entity;
}

// An object a block declares extern, rcc's linked_declaration(): the
// name has block scope, 6.2.1p4, and denotes an entity of linkage, the
// one linked_entity() finds, declared before the block at file scope or
// in an enclosing block, after it at file scope, or in another block,
// since every declaration of a name of external linkage denotes one
// entity, 6.2.2p2.  Every declaration of the entity is of its kind, its
// type, 6.2.7p2, whose composite it takes, p5, and its linkage,
// 6.2.2p7: a function of the name is a redefinition, as redeclared()
// has it.  A block declares the name once, but for the same entity
// again, 6.7.1p4.  The declaration takes no initializer, 6.7.11p6, and
// makes no object and no statement: the block's scope maps the name to
// the entity's leaf, and a new entity, of external linkage, is the
// unit's, which a later declaration at file scope adopts.
void
linked_declaration(parser_t &p, const declarator_t &d)
{
    refuse_declarator_tail(p);
    refuse_static_pointer(d.name, d.type);
    refuse_static_array(d.name, d.type);
    refuse_record_object(d.name, d.type);

    if (peek(p).kind == tok_t::assign)
        fail(peek(p), quoted(d.name.text) +
                          " declared with linkage in a block cannot have "
                          "an initializer");

    bool external;
    object_name_t *entity = linked_entity(p, d.name.text, external);
    refuse_visibility(d.name, external);

    scope_t &scope = p.scopes.back();
    const auto here = scope.names.find(d.name.text);
    if ((here != scope.names.end() && !here->second.linked) ||
        p.functions.contains(d.name.text))
        fail(d.name, "redefinition of " + quoted(d.name.text));

    if (entity) {
        if (!compatible(entity->type, d.type)) {
            refuse_tag_twins(d.name, entity->type, d.type);
            fail(d.name, "conflicting types for " + quoted(d.name.text));
        }

        require_linkage(d.name, entity->external, external);
        entity->type = composite(entity->type, d.type);
    }
    else {
        expr_t leaf =
            make_global(d.name.text, view_of(d.type), sign_of(d.type));
        object_name_t name { d.type, std::move(leaf), true };
        entity = &p.block_entities.emplace(d.name.text, std::move(name))
                      .first->second;
    }

    const operand_t object { entity->type, entity->leaf };
    scope.names.emplace(d.name.text, symbol_t { object, true });
}

} // namespace

bool
lists_parameters(const specifiers_t &spec, const declarator_t &d)
{
    return is< function_t >(d.type) && !is< function_t >(spec.type);
}

void
refuse_typedef_function(const specifiers_t &spec, const declarator_t &d)
{
    if (is< function_t >(d.type) && !lists_parameters(spec, d))
        not_yet(d.name, "function " + quoted(d.name.text) +
                            " declared through a typedef");
}

// rcc's type_definition(): the name is declared in the current scope,
// the file's or a block's, as an ordinary identifier, 6.7.9p3, so a
// typedef name declared again there denotes the same type, 6.7.1p4,
// rcc's same_type(), else rcc's error, and a name the scope declares as
// anything else, declares()'s, is a redefinition, each said at the
// later name.  The name is in scope from the end of its declarator,
// 6.2.1p7.  It has no object, so the declaration makes no statement,
// and takes no initializer, which gives an object's value, 6.7.11p9,
// nor a function body, since a function definition's storage class is
// extern or static, 6.9.2p4, each an error at its first token.  GNU C's
// attributes and an assembler name after the declarator are not yet,
// and so is a typedef of a variable length array, whose size the
// typedef fixes, 6.7.9p8, rcc's own not yet at the name.
void
type_definition(parser_t &p, const specifiers_t &spec, const declarator_t &d)
{
    refuse_declarator_tail(p);

    if (peek(p).kind == tok_t::assign)
        fail(peek(p),
             "typedef " + quoted(d.name.text) + " cannot have an initializer");

    if (lists_parameters(spec, d) && peek(p).kind == tok_t::lbrace)
        fail(peek(p),
             "typedef " + quoted(d.name.text) + " cannot have a function body");

    if (d.variable_size)
        not_yet(d.name,
                "typedef of variable length array " + quoted(d.name.text));

    auto &names = p.scopes.back().names;
    const auto earlier = names.find(d.name.text);
    if (earlier != names.end() && earlier->second.type_name) {
        if (!same_type(earlier->second.operand.type, d.type)) {
            refuse_tag_twins(d.name, earlier->second.operand.type, d.type);
            fail(d.name, "redefinition of typedef " + quoted(d.name.text) +
                             " with another type");
        }

        return;
    }

    if (declares(p, d.name.text))
        fail(d.name, "redefinition of " + quoted(d.name.text));

    names.emplace(d.name.text, symbol_t { .operand = { .type = d.type,
                                                       .node = std::nullopt },
                                          .type_name = true });
}

namespace {

// The declarators of a declaration in a block and its ';', 6.7.1p1: a
// local's statement for each, in order; a typedef's declarators make
// none, their names no objects, nor a static declaration's, their
// objects being no locals, nor an extern one's, which declare objects
// the block does not make.  A function declared in a block is not yet,
// one declared through a typedef name in rcc's words, and objects_only
// is a for's declaration, which declares no function,
// for_declaration()'s, and rcc says so at its name.
std::vector< stmt_t >
init_declarator_list(parser_t &p, const specifiers_t &spec, bool objects_only)
{
    std::vector< stmt_t > items;

    do {
        const declarator_t d = declarator(p, spec.type);
        if (declared(spec, tok_t::kw_typedef)) {
            type_definition(p, spec, d);
            continue;
        }

        if (objects_only && is< function_t >(d.type))
            fail(d.name, "function " + quoted(d.name.text) +
                             " declared in the declaration of a 'for'");

        refuse_typedef_function(spec, d);
        if (is< function_t >(d.type))
            not_yet(d.name, "declaration of function " + quoted(d.name.text));

        if (declared(spec, tok_t::kw_extern))
            linked_declaration(p, d);
        else if (declared(spec, tok_t::kw_static))
            static_local(p, d);
        else
            init_declarator(p, d, items);
    } while (accept(p, tok_t::comma));

    expect(p, tok_t::semicolon, "expected ';' at end of declaration");

    return items;
}

// rcc reads __extension__ and a static assertion, a declaration of its
// own, 6.7.1p1, before a declaration's specifiers, and both are not
// yet.
void
refuse_declaration_start(const parser_t &p)
{
    const token_t &t = peek(p);
    if (t.kind == tok_t::kw_extension)
        not_yet(t, quoted(t.text) + " before a declaration");

    if (t.kind == tok_t::kw_static_assert)
        not_yet(t, quoted(t.text) + " declaration");
}

} // namespace

// A declaration in a block, 6.7.1p1, after what refuse_declaration_start()
// refuses: of locals, of static objects, of extern ones or of typedef
// names, or of an enum's tag and constants alone, 6.7.1p2, which makes
// no statement; an object of type void is not yet, refused at its
// specifier, and a typedef name of type void is taken.
std::vector< stmt_t >
declaration(parser_t &p)
{
    refuse_declaration_start(p);

    const specifiers_t spec = declaration_specifiers(p);
    if (is< void_t >(spec.type) && !declared(spec, tok_t::kw_typedef))
        not_yet(spec.specifier,
                "declaration specifier " + quoted(spec.specifier.text));

    if (spec.tagged && accept(p, tok_t::semicolon))
        return { };

    return init_declarator_list(p, spec, false);
}

// The declaration of a for's first clause, as the for's init, rcc's
// for_declaration(): one declarator's statement is the init itself, and
// several are a block of theirs, which opens no scope, since the for's
// own holds their names, 6.8.6.4p1.  A storage class there is rcc's
// error, by C17's rule that the declaration declare objects of storage
// class auto or register only, its 6.8.5p3, which C23 drops; a function
// is init_declarator_list()'s; an object of type void is not yet, as in
// a block.
stmt_t
for_declaration(parser_t &p)
{
    refuse_declaration_start(p);

    const specifiers_t spec = declaration_specifiers(p);
    if (spec.storage_class)
        fail(*spec.storage_class, quoted(spec.storage_class->text) +
                                      " in the declaration of a 'for'");

    if (is< void_t >(spec.type))
        not_yet(spec.specifier,
                "declaration specifier " + quoted(spec.specifier.text));

    std::vector< stmt_t > items = init_declarator_list(p, spec, true);
    if (items.size() == 1)
        return std::move(items.front());

    return make_block(std::move(items));
}

} // namespace rcc
