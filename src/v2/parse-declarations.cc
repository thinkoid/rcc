// -*- mode: c++; -*-

// 6.7: the declarations, of which rcc-v2 takes the declaration
// specifiers static, int and void, the declarators of a function of no
// parameters, int f(void), and of an object, and the declaration in a
// block of int objects, each with an initializer or without; it refuses
// the rest as not yet.  rcc reads more of each than rcc-v2 does, so
// each is read as far as rcc would read it, and what rcc would take is
// refused before an error rcc would not say.

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

// The suffix after the name, and the type it makes of base: none, an
// object's, base itself; or the parameter list (void), or (), which C23
// reads as (void), 6.7.7.4p13, a function of no parameters returning
// base, p9.  An array, and any other list, are not yet.  A function
// returns neither an array nor a function, 6.7.7.4p1, which rcc says at
// the suffix after the list.
type_t
declarator_suffix(parser_t &p, const type_t &base)
{
    if (peek(p).kind == tok_t::lbracket)
        not_yet(peek(p), "array declarator");

    if (!accept(p, tok_t::lparen))
        return base;

    if (peek(p).kind == tok_t::kw_void && peek(p, 1).kind == tok_t::rparen)
        advance(p);

    if (!accept(p, tok_t::rparen))
        not_yet(peek(p), "parameter list");

    if (peek(p).kind == tok_t::lbracket)
        fail(peek(p), "function cannot return an array");

    if (peek(p).kind == tok_t::lparen)
        fail(peek(p), "function cannot return a function");

    return make_function(base, { }, false);
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

// The declaration specifiers: the storage class static, 6.7.2p1, then
// one of the sets int and void of 6.7.3.1p2.  rcc reads every specifier
// up to the first token that is none, so another one, before the type
// specifier or after it, is not yet, and so is an identifier in its
// place, which may name a type to rcc; any other token there is rcc's
// error.
specifiers_t
declaration_specifiers(parser_t &p)
{
    const token_t *storage_class = nullptr;
    if (peek(p).kind == tok_t::kw_static)
        storage_class = &advance(p);

    const token_t &t = peek(p);
    if (!next_is(p, { tok_t::kw_int, tok_t::kw_void }) &&
        (is_specifier(t) || t.kind == tok_t::identifier))
        not_yet(t, "declaration specifier " + quoted(t.text));

    if (!next_is(p, { tok_t::kw_int, tok_t::kw_void }))
        fail(t, "expected a type specifier");

    advance(p);
    if (is_specifier(peek(p)))
        not_yet(peek(p), "declaration specifier " + quoted(peek(p).text));

    return { storage_class, t,
             t.kind == tok_t::kw_void ? make_void() : int_type() };
}

// A declarator of a function of no parameters, f(void), or of an
// object, of the type base: its name, which a declarator must write in
// a declaration, 6.7.7.1p1, and its suffix.  A pointer and a declarator
// in parentheses are not yet.
declarator_t
declarator(parser_t &p, const type_t &base)
{
    const token_t &t = peek(p);
    if (t.kind == tok_t::star)
        not_yet(t, "pointer declarator");

    if (t.kind == tok_t::lparen)
        not_yet(t, "declarator in parentheses");

    const token_t &name = expect(p, tok_t::identifier, "expected identifier");

    return { name, declarator_suffix(p, base) };
}

namespace {

// A declarator of an object and its initializer, appended to items: the
// name declared, then, after '=', the initializer, whose scope holds
// the name already, 6.2.1p7.  rcc reads GNU C's attributes and an
// assembler name after the declarator, which are not yet, and so is a
// brace initializer, in rcc's words.  The initializer's value is
// converted to the object's type as if by assignment, 6.7.11p12: a
// value of another type is not yet.
void
init_declarator(parser_t &p,
                const type_t &type,
                const token_t &name,
                std::vector< stmt_t > &items)
{
    if (next_is(p, { tok_t::kw_attribute, tok_t::kw_asm }))
        not_yet(peek(p), quoted(peek(p).text) + " after a declarator");

    const int slot = declare(p, name, type);
    if (!accept(p, tok_t::assign)) {
        items.push_back(make_decl(slot));
        return;
    }

    if (peek(p).kind == tok_t::lbrace)
        not_yet(peek(p), "brace initializer");

    const token_t &start = peek(p);
    operand_t init = assignment_expression(p);
    if (init.type != type)
        not_yet(start, "conversion to the type of " + quoted(name.text));

    items.push_back(make_init(slot, rvalue(std::move(init.node))));
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
