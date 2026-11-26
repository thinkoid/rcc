// -*- mode: c++; -*-

// 6.7: the declarations, of which rcc-v2 takes the declaration
// specifiers and the declarator of a function definition, int f(void),
// and refuses the rest as not yet.  rcc reads more of either than
// rcc-v2 does, so each is read as far as rcc would read it, and what
// rcc would take is refused before an error rcc would not say.

#include "parse-common.hh"

#include <cstddef>
#include <string>

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

// The parameter list (void) after the name, a function of no
// parameters, 6.7.7.4p9; any other list, and no list, are not yet.  A
// function returns neither an array nor a function, 6.7.7.4p1, which
// rcc says at the suffix after the list.
void
declarator_suffix(parser_t &p, const token_t &name)
{
    if (peek(p).kind != tok_t::lparen)
        not_yet(name, "object " + quoted(name.text));

    advance(p);
    if (peek(p).kind != tok_t::kw_void || peek(p, 1).kind != tok_t::rparen)
        not_yet(peek(p), "parameter list");

    advance(p);
    advance(p);

    if (peek(p).kind == tok_t::lbracket)
        fail(peek(p), "function cannot return an array");

    if (peek(p).kind == tok_t::lparen)
        fail(peek(p), "function cannot return a function");
}

} // namespace

// Whether a declaration starts at the token ahead: a declaration
// specifier, which __extension__ may stand before, or a static
// assertion, a declaration of its own, 6.7.1p1.  rcc-v2 knows no
// typedef name, nor the names of the types the compiler declares,
// __builtin_va_list among them, so no identifier starts one.
bool
is_declaration_start(const parser_t &p, std::size_t ahead)
{
    const token_t &t = peek(p, ahead);
    if (t.kind == tok_t::kw_extension)
        return is_declaration_start(p, ahead + 1);

    return t.kind == tok_t::kw_static_assert || is_specifier(t);
}

// The declaration specifiers, int alone of the sets of 6.7.3.1p2.  rcc
// reads every specifier up to the first token that is none, so another
// one, before int or after it, is not yet, and so is an identifier in
// the place of int, which may name a type to rcc; any other token there
// is rcc's error.
type_t
declaration_specifiers(parser_t &p)
{
    const token_t &t = peek(p);
    if (t.kind != tok_t::kw_int &&
        (is_specifier(t) || t.kind == tok_t::identifier))
        not_yet(t, "declaration specifier " + quoted(t.text));

    expect(p, tok_t::kw_int, "expected a type specifier");
    if (is_specifier(peek(p)))
        not_yet(peek(p), "declaration specifier " + quoted(peek(p).text));

    return int_type();
}

// The declarator of a function of no parameters, f(void): its name,
// which a declarator must write in a declaration, 6.7.7.1p1, and its
// suffix.  A pointer and a declarator in parentheses are not yet.
const token_t &
declarator(parser_t &p)
{
    const token_t &t = peek(p);
    if (t.kind == tok_t::star)
        not_yet(t, "pointer declarator");

    if (t.kind == tok_t::lparen)
        not_yet(t, "declarator in parentheses");

    const token_t &name = expect(p, tok_t::identifier, "expected identifier");
    declarator_suffix(p, name);

    return name;
}

} // namespace rcc
