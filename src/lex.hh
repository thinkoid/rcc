// -*- mode: c++; -*-

// Tokens, and the one function that makes them.  The lexer knows no
// names: whether an identifier is a type is the parser's question.

#pragma once

#include "error.hh"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace rcc {

enum class tok_t {
    eof,
    identifier,
    integer,
    floating,
    character,
    string,

    // keywords, C89 and restrict
    kw_auto,
    kw_break,
    kw_case,
    kw_char,
    kw_const,
    kw_continue,
    kw_default,
    kw_do,
    kw_double,
    kw_else,
    kw_enum,
    kw_extern,
    kw_float,
    kw_for,
    kw_goto,
    kw_if,
    kw_int,
    kw_long,
    kw_register,
    kw_restrict,
    kw_return,
    kw_short,
    kw_signed,
    kw_sizeof,
    kw_static,
    kw_struct,
    kw_switch,
    kw_typedef,
    kw_union,
    kw_unsigned,
    kw_void,
    kw_volatile,
    kw_while,

    // keywords of C outside the subset, each refused by its name
    kw_alignas,
    kw_alignof,
    kw_atomic,
    kw_bool,
    kw_complex,
    kw_generic,
    kw_imaginary,
    kw_static_assert,
    kw_thread_local,

    // the function specifiers, and GNU C's words for what a header
    // says beside the C: __asm__, __attribute__, __extension__, and
    // the types __float128 and __int128
    kw_inline,
    kw_noreturn,
    kw_asm,
    kw_attribute,
    kw_extension,
    kw_float128,
    kw_int128,

    // punctuators, in the order of 6.4.6
    lbracket,
    rbracket,
    lparen,
    rparen,
    lbrace,
    rbrace,
    dot,
    arrow,
    inc,
    dec,
    amp,
    star,
    plus,
    minus,
    tilde,
    bang,
    slash,
    percent,
    shl,
    shr,
    lt,
    gt,
    le,
    ge,
    eq,
    ne,
    caret,
    pipe,
    log_and,
    log_or,
    question,
    colon,
    semicolon,
    ellipsis,
    assign,
    mul_assign,
    div_assign,
    rem_assign,
    add_assign,
    sub_assign,
    shl_assign,
    shr_assign,
    and_assign,
    xor_assign,
    or_assign,
    comma
};

// The spelling is a view into the source, the encoding prefix and the
// quotes included for a character constant or a string literal; an
// integer token's value is read from it by the parser, a literal's by
// the functions below.  A floating token's value is not read: the
// parser refuses its use.  packed is whether a #pragma pack sets a
// packing where the token stands: clang packs a record whose list
// opens there, which rcc does not do yet.  nondefault_visibility is
// whether a #pragma GCC visibility sets a visibility other than
// default there: clang gives it to a symbol of external linkage a
// declaration names there, which rcc does not do yet.
struct token_t
{
    tok_t kind;
    std::string_view text;
    location_t where;
    bool packed;
    bool nondefault_visibility;
};

// The whole preprocessed text as tokens, ending in one eof, which
// stands just past the last token, so that an error at the end of the
// text names a place the text has.  The file name is the location of
// the first line until a line marker says otherwise.  Every other
// directive is skipped, but a #pragma pack sets the packing of the
// tokens after it, and a #pragma GCC visibility their visibility.
// Throws error_t on a character no token starts with, a malformed
// constant, a malformed line marker, or a #pragma weak, which is not
// yet.
std::vector< token_t > tokenize(std::string_view source, std::string_view file);

// The encoding prefix of a character constant or a string literal,
// 6.4.4.5p1, 6.4.5p1, and the encoding it names: none and u8 the UTF-8
// of the source, u UTF-16, U UTF-32, and L the wide literal encoding,
// which is UTF-32 on both targets, as clang has it.
enum class encoding_t { none, utf8, utf16, utf32, wide };

// The decoding functions.  None fails: tokenize() refused every
// malformed escape and every malformed character of a literal of a
// prefix, and a constant of more than one character.
//
// encoding_of() is a literal's prefix.  string_units() is the elements
// of a string literal, escapes applied, the quotes and the terminating
// zero left out, in an encoding: the literal's own, or the one of a
// prefixed literal it is concatenated with, 6.4.5p5.  An escape gives
// its element's value, 6.4.4.5p5, p6; a universal character name,
// 6.4.3p4, and each character of the text, read as UTF-8 for any
// encoding but none's, give the code units that encode them.
// string_bytes() is the elements of a literal in the encoding none, each
// a byte.  character_value() is the value of a character constant of no
// prefix, an int, 6.4.4.5p11, which depends on the signedness of plain
// char; wide_character_value() is the one code unit of one of the
// prefix L, u or U, p13 to p15.  is_utf8() says whether the characters
// of a literal outside its escapes are UTF-8.
encoding_t encoding_of(const token_t &);
std::vector< std::uint32_t > string_units(const token_t &, encoding_t);
std::string string_bytes(const token_t &);
int character_value(const token_t &, bool char_is_signed);
std::uint32_t wide_character_value(const token_t &);
bool is_utf8(const token_t &);

} // namespace rcc
