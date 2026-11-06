// -*- mode: c++; -*-

// Tokens, and the one function that makes them.  The lexer knows no
// names: whether an identifier is a type is the parser's question.

#pragma once

#include "error.hh"

#include <string>
#include <string_view>
#include <vector>

namespace rcc {

enum class tok_t {
    eof,
    identifier,
    integer,
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

// The spelling is a view into the source, the quotes included for a
// character constant or a string literal; an integer token's value is
// read from it by the parser, a literal's by the two functions below.
struct token_t
{
    tok_t kind;
    std::string_view text;
    location_t where;
};

// The whole preprocessed text as tokens, ending in one eof.  The file
// name is the location of the first line until a line marker says
// otherwise.  Throws error_t on a character no token starts with, a
// malformed constant, or a malformed line marker.
std::vector< token_t > tokenize(std::string_view source, std::string_view file);

// The bytes of a string literal token, escapes applied, the quotes and
// the terminating zero left out; and the value of a character
// constant, an int (6.4.4.4p10).  Neither fails: tokenize() refused
// every malformed escape, and a constant of more than one character
// is not yet.
std::string string_bytes(const token_t &);
int character_value(const token_t &);

} // namespace rcc
