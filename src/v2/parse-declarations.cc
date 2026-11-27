// -*- mode: c++; -*-

// 6.7: the declarations, of which rcc-v2 takes the declaration
// specifiers static, void and the type specifiers of the integer types
// from char to unsigned long long, in any order and spelling,
// the declarators of a function of int parameters and of an object,
// the declaration in a block of objects of those types, each with an
// initializer or without, and the type name of a cast; it refuses the
// rest as not yet.  rcc reads more of each than rcc-v2 does, so each is
// read as far as rcc would read it, and what rcc would take is refused
// before an error rcc would not say.

#include "parse-common.hh"

#include "build.hh"

#include <cstddef>
#include <string>
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

// Whether an identifier at the token may name a type to rcc: a name
// reserved for the implementation, 6.4.2.1p7, as __builtin_va_list is.
// rcc-v2 declares no typedef name, so no other identifier names one.
bool
names_a_type(const token_t &t)
{
    return t.kind == tok_t::identifier && is_reserved(t.text);
}

} // namespace

// Whether a declaration starts at the token ahead: a declaration
// specifier, which __extension__ may stand before, or a static
// assertion, a declaration of its own, 6.7.1p1.  rcc-v2 knows no
// typedef name, nor the names of the types the compiler declares,
// __builtin_va_list among them, so no identifier starts one: such a
// name is reserved, and an expression statement it starts refuses it,
// identifier().
bool
is_declaration_start(const parser_t &p, std::size_t ahead)
{
    const token_t &t = peek(p, ahead);
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

} // namespace

// The declaration specifiers: the storage class static, 6.7.2p1, then
// one of the sets of 6.7.3.1p2 in any order, read as rcc reads them, up
// to the first token that is none, each type specifier refused where it
// cannot join those before it, in rcc's words.  Any other specifier is
// not yet where it stands, and so is a reserved identifier in place of
// the first type specifier, which may name a type the compiler
// declares; any other token there is rcc's error, another identifier's
// among them.
specifiers_t
declaration_specifiers(parser_t &p)
{
    const token_t *storage_class = nullptr;
    if (peek(p).kind == tok_t::kw_static)
        storage_class = &advance(p);

    counts_t counts;
    const token_t *first = nullptr;

    while (is_counted(peek(p))) {
        const token_t &t = advance(p);
        if (cannot_join(counts, t))
            fail(t, "cannot combine " + quoted(t.text) +
                        " with the type specifiers before it");

        if (!first)
            first = &t;
    }

    const token_t &t = peek(p);
    if (is_specifier(t) || (!first && names_a_type(t)))
        not_yet(t, "declaration specifier " + quoted(t.text));

    if (!first)
        fail(t, "expected a type specifier");

    return { storage_class, *first, specified(p, counts) };
}

namespace {

// A pointer and a declarator in parentheses are not yet, refused at the
// token that would start them.
void
refuse_declarator_start(const parser_t &p)
{
    const token_t &t = peek(p);
    if (t.kind == tok_t::star)
        not_yet(t, "pointer declarator");

    if (t.kind == tok_t::lparen)
        not_yet(t, "declarator in parentheses");
}

// The declarator of a parameter, whose name is optional, 6.7.7.1p1, and
// returned, or null: an array, adjusted to a pointer, p6, and a
// function, adjusted to a pointer to it, p7, are not yet, as a pointer
// is.
const token_t *
parameter_declarator(parser_t &p)
{
    refuse_declarator_start(p);

    const token_t *name =
        peek(p).kind == tok_t::identifier ? &advance(p) : nullptr;

    if (peek(p).kind == tok_t::lbracket)
        not_yet(peek(p), "array declarator");

    if (peek(p).kind == tok_t::lparen)
        not_yet(peek(p), "parameter of function type");

    return name;
}

// A parameter's specifiers and declarator, 6.7.7.1p1, and GNU C's
// attributes after it, which are not yet.  register is the one storage
// class a parameter takes, p2, and static is refused in rcc's words;
// register itself is not yet, as a specifier.  A parameter of type void
// is left for the list to take or refuse, and one of any other type
// than int is not yet, at its first type specifier.
parameter_t
parameter_declaration(parser_t &p)
{
    const token_t &start = peek(p);

    const specifiers_t spec = declaration_specifiers(p);
    if (spec.storage_class)
        fail(*spec.storage_class,
             quoted(spec.storage_class->text) + " in a parameter declaration");

    if (!is< void_t >(spec.type) && spec.type != int_type())
        not_yet(spec.specifier, "parameter whose type is not 'int'");

    const token_t *name = parameter_declarator(p);
    if (peek(p).kind == tok_t::kw_attribute)
        not_yet(peek(p), quoted(peek(p).text) + " after a declarator");

    return { start, name, spec.type };
}

// A parameter's name, declared in the scope of its list, which the
// parameters of one list share, 6.7.1p4, 6.2.1p4, whether a body
// follows or not: a name declared twice there is an error at the
// second.  The parameter's leaf is a local's, its slot numbered by its
// place in the list, so the parameters of a definition are its first
// locals, and its body's are numbered after them (docs/typed-tree.md
// 4.5).
void
declare_parameter(parser_t &p, const parameter_t &param, int index)
{
    if (!param.name)
        return;

    scope_t &scope = p.scopes.back();
    if (scope.contains(param.name->text))
        fail(*param.name,
             "redefinition of parameter " + quoted(param.name->text));

    scope.emplace(param.name->text,
                  operand_t { param.type, make_local(index, view_of(param.type),
                                                     sign_of(param.type)) });
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
// none, an object's, base itself; or a parameter list, a function
// returning base of its parameters' types, 6.7.7.4p4, whose list opens
// a scope at its '(' and closes it at its ')', the function prototype
// scope of 6.2.1p4, which d keeps for a definition.  An array is not
// yet.  A function returns neither an array nor a function, 6.7.7.4p1,
// which rcc says at the suffix after the list.
void
declarator_suffix(parser_t &p, declarator_t &d, const type_t &base)
{
    if (peek(p).kind == tok_t::lbracket)
        not_yet(peek(p), "array declarator");

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

    std::vector< type_t > types;
    for (const parameter_t &param : d.params)
        types.push_back(param.type);

    d.type = make_function(base, std::move(types), false);
}

} // namespace

// A declarator of a function or of an object, of the type base: its
// name, which a declarator must write in a declaration, 6.7.7.1p1, and
// its suffix.
declarator_t
declarator(parser_t &p, const type_t &base)
{
    refuse_declarator_start(p);

    declarator_t d { expect(p, tok_t::identifier, "expected identifier"),
                     base };
    declarator_suffix(p, d, base);

    return d;
}

// A type name, 6.7.8p1: the specifiers, of which no storage class, in
// rcc's words, then an abstract declarator, a declarator without its
// name, of which none is taken yet: a pointer, a declarator in
// parentheses or an array is not yet, refused at its first token.
type_t
type_name(parser_t &p)
{
    const specifiers_t spec = declaration_specifiers(p);
    if (spec.storage_class)
        fail(*spec.storage_class,
             quoted(spec.storage_class->text) + " in a type name");

    refuse_declarator_start(p);
    if (peek(p).kind == tok_t::lbracket)
        not_yet(peek(p), "array declarator");

    return spec.type;
}

namespace {

// A declarator of an object and its initializer, appended to items: the
// name declared, then, after '=', the initializer, whose scope holds
// the name already, 6.2.1p7.  rcc reads GNU C's attributes and an
// assembler name after the declarator, which are not yet, and so is a
// brace initializer, in rcc's words.  The initializer's value is
// converted to the object's type as if by assignment, 6.7.11p12,
// assignment_conversion()'s at the '=', but a void value, which rcc
// refuses at the initializer's first token.
void
init_declarator(parser_t &p,
                const type_t &type,
                const token_t &name,
                std::vector< stmt_t > &items)
{
    if (next_is(p, { tok_t::kw_attribute, tok_t::kw_asm }))
        not_yet(peek(p), quoted(peek(p).text) + " after a declarator");

    const int slot = declare(p, name, type);
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

// The declarators of a declaration in a block and its ';', 6.7.1p1: one
// statement for each, in order.  A function declared in a block is not
// yet.
std::vector< stmt_t >
init_declarator_list(parser_t &p, const type_t &type)
{
    std::vector< stmt_t > items;

    do {
        const declarator_t d = declarator(p, type);
        if (is< function_t >(d.type))
            not_yet(d.name, "declaration of function " + quoted(d.name.text));

        init_declarator(p, type, d.name, items);
    } while (accept(p, tok_t::comma));

    expect(p, tok_t::semicolon, "expected ';' at end of declaration");

    return items;
}

} // namespace

// A declaration in a block, 6.7.1p1.  rcc reads __extension__ and a
// static assertion before the declaration specifiers, which are not
// yet; and so are a static object and an object of type void, each
// refused at its specifier.
std::vector< stmt_t >
declaration(parser_t &p)
{
    const token_t &t = peek(p);
    if (t.kind == tok_t::kw_extension)
        not_yet(t, quoted(t.text) + " before a declaration");

    if (t.kind == tok_t::kw_static_assert)
        not_yet(t, quoted(t.text) + " declaration");

    const specifiers_t spec = declaration_specifiers(p);
    if (spec.storage_class)
        not_yet(*spec.storage_class,
                "declaration specifier " + quoted(spec.storage_class->text));

    if (is< void_t >(spec.type))
        not_yet(spec.specifier,
                "declaration specifier " + quoted(spec.specifier.text));

    return init_declarator_list(p, spec.type);
}

} // namespace rcc
