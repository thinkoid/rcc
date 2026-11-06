// -*- mode: c++; -*-

// The lexer: preprocessed C in, tokens out.  The input is what
// clang -E writes, so there are no comments and no directives but the
// line markers, which move the locations back onto the source file.

#include "lex.hh"

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <format>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace rcc {

namespace {

// Where the lexer stands.  The column of a position is its distance
// from origin, the offset of the first byte of the current line.
struct state_t
{
    std::string_view source;
    std::size_t pos;
    std::string_view file;
    int line;
    std::size_t origin;
};

struct keyword_t
{
    std::string_view spelling;
    tok_t kind;
};

const keyword_t keywords[] = {
    { "auto", tok_t::kw_auto },         { "break", tok_t::kw_break },
    { "case", tok_t::kw_case },         { "char", tok_t::kw_char },
    { "const", tok_t::kw_const },       { "continue", tok_t::kw_continue },
    { "default", tok_t::kw_default },   { "do", tok_t::kw_do },
    { "double", tok_t::kw_double },     { "else", tok_t::kw_else },
    { "enum", tok_t::kw_enum },         { "extern", tok_t::kw_extern },
    { "float", tok_t::kw_float },       { "for", tok_t::kw_for },
    { "goto", tok_t::kw_goto },         { "if", tok_t::kw_if },
    { "int", tok_t::kw_int },           { "long", tok_t::kw_long },
    { "register", tok_t::kw_register }, { "restrict", tok_t::kw_restrict },
    { "return", tok_t::kw_return },     { "short", tok_t::kw_short },
    { "signed", tok_t::kw_signed },     { "sizeof", tok_t::kw_sizeof },
    { "static", tok_t::kw_static },     { "struct", tok_t::kw_struct },
    { "switch", tok_t::kw_switch },     { "typedef", tok_t::kw_typedef },
    { "union", tok_t::kw_union },       { "unsigned", tok_t::kw_unsigned },
    { "void", tok_t::kw_void },         { "volatile", tok_t::kw_volatile },
    { "while", tok_t::kw_while },
};

struct punctuator_t
{
    std::string_view spelling;
    tok_t kind;
};

// Longest first: the first entry that matches is the maximal munch.
const punctuator_t punctuators[] = {
    { "...", tok_t::ellipsis },   { "<<=", tok_t::shl_assign },
    { ">>=", tok_t::shr_assign },

    { "->", tok_t::arrow },       { "++", tok_t::inc },
    { "--", tok_t::dec },         { "<<", tok_t::shl },
    { ">>", tok_t::shr },         { "<=", tok_t::le },
    { ">=", tok_t::ge },          { "==", tok_t::eq },
    { "!=", tok_t::ne },          { "&&", tok_t::log_and },
    { "||", tok_t::log_or },      { "*=", tok_t::mul_assign },
    { "/=", tok_t::div_assign },  { "%=", tok_t::rem_assign },
    { "+=", tok_t::add_assign },  { "-=", tok_t::sub_assign },
    { "&=", tok_t::and_assign },  { "^=", tok_t::xor_assign },
    { "|=", tok_t::or_assign },

    { "[", tok_t::lbracket },     { "]", tok_t::rbracket },
    { "(", tok_t::lparen },       { ")", tok_t::rparen },
    { "{", tok_t::lbrace },       { "}", tok_t::rbrace },
    { ".", tok_t::dot },          { "&", tok_t::amp },
    { "*", tok_t::star },         { "+", tok_t::plus },
    { "-", tok_t::minus },        { "~", tok_t::tilde },
    { "!", tok_t::bang },         { "/", tok_t::slash },
    { "%", tok_t::percent },      { "<", tok_t::lt },
    { ">", tok_t::gt },           { "^", tok_t::caret },
    { "|", tok_t::pipe },         { "?", tok_t::question },
    { ":", tok_t::colon },        { ";", tok_t::semicolon },
    { "=", tok_t::assign },       { ",", tok_t::comma },
};

// The character at offset i, or '\0' past the end, so a lookahead
// never needs its own bounds check.
char
at(const state_t &st, std::size_t i)
{
    return i < st.source.size() ? st.source[i] : '\0';
}

bool
is_digit(char c)
{
    return c >= '0' && c <= '9';
}

bool
is_octal_digit(char c)
{
    return c >= '0' && c <= '7';
}

bool
is_hex_digit(char c)
{
    return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

// ASCII only: a byte outside it, a UTF-8 lead byte included, starts
// no token and is an error.
bool
is_ident_start(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool
is_ident_char(char c)
{
    return is_ident_start(c) || is_digit(c);
}

// Whitespace within a line; the newline is counted apart.
bool
is_line_blank(char c)
{
    return c == ' ' || c == '\t' || c == '\f' || c == '\v' || c == '\r';
}

// Columns count bytes from 1, not characters.
location_t
location_of(const state_t &st, std::size_t pos)
{
    return { st.file, st.line, int(pos - st.origin) + 1 };
}

[[noreturn]] void
fail(const location_t &where, std::string what)
{
    throw error_t { where, std::move(what) };
}

// A character as a message names it: quoted when printable, in hex
// when not, so a stray control byte is still legible.
std::string
describe(char c)
{
    const auto u = static_cast< unsigned char >(c);

    if (u >= 0x20 && u < 0x7f)
        return std::format("'{}'", c);

    return std::format("{:#04x}", u);
}

// A construct of C outside the subset this lexer takes, said as the
// parser says it.
[[noreturn]] void
not_yet(const location_t &where, std::string_view construct)
{
    fail(where, "not yet: " + std::string(construct));
}

// The token of the given length at the current position.  A token
// never spans a newline, so its location is that of its first byte.
token_t
take(state_t &st, tok_t kind, std::size_t length)
{
    const token_t tok { kind, st.source.substr(st.pos, length),
                        location_of(st, st.pos) };

    st.pos += length;

    return tok;
}

// Step over the newline at the current position; the next byte is
// column 1 of the next line.
void
next_line(state_t &st)
{
    ++st.pos;
    ++st.line;
    st.origin = st.pos;
}

// Newlines included: each one advances the line and moves the column
// origin.
void
skip_blanks(state_t &st)
{
    for (char c = at(st, st.pos); c == '\n' || is_line_blank(c);
         c = at(st, st.pos)) {
        if (c == '\n')
            next_line(st);
        else
            ++st.pos;
    }
}

void
skip_line_blanks(state_t &st)
{
    while (is_line_blank(at(st, st.pos)))
        ++st.pos;
}

// Leaves the position on the newline, or at the end of the text, so
// the newline is counted where every other one is.
void
skip_rest_of_line(state_t &st)
{
    while (st.pos < st.source.size() && st.source[st.pos] != '\n')
        ++st.pos;
}

// True when only blanks precede the current position on its line: a
// '#' there opens a directive, anywhere else it is a stray character.
bool
starts_line(const state_t &st)
{
    for (std::size_t i = st.origin; i < st.pos; ++i)
        if (!is_line_blank(st.source[i]))
            return false;

    return true;
}

// The optional "line" after the '#', as in #line N "name".  The
// identifier test keeps a #linear or the like from passing for it.
void
skip_line_keyword(state_t &st)
{
    if (st.source.substr(st.pos).starts_with("line") &&
        !is_ident_char(at(st, st.pos + 4))) {
        st.pos += 4;
        skip_line_blanks(st);
    }
}

// The line number of a marker.  It names the line after the marker.
int
line_number(state_t &st)
{
    const std::size_t first = st.pos;

    while (is_digit(at(st, st.pos)))
        ++st.pos;

    const char *begin = st.source.data() + first;
    const char *end = st.source.data() + st.pos;

    int n = 0;
    if (std::from_chars(begin, end, n).ec != std::errc { })
        fail(location_of(st, first), "line number out of range in line marker");

    return n;
}

// The quoted name of a marker, without the quotes.  Escapes are left
// as written: the name is a view into the source and is not rewritten.
std::string_view
file_name(state_t &st)
{
    const std::size_t first = ++st.pos;

    for (char c = at(st, st.pos); c != '"'; c = at(st, st.pos)) {
        if (c == '\n' || st.pos >= st.source.size())
            fail(location_of(st, first - 1),
                 "unterminated file name in line marker");

        st.pos += c == '\\' && at(st, st.pos + 1) != '\n' ? 2 : 1;
    }

    const std::string_view name = st.source.substr(first, st.pos - first);
    ++st.pos;

    return name;
}

// A line opening with '#'.  A line marker, # N "name" flags or
// #line N "name", sets the file and the number of the next line; any
// other directive (a #pragma) is skipped whole.
void
line_marker(state_t &st)
{
    ++st.pos;
    skip_line_blanks(st);
    skip_line_keyword(st);

    if (!is_digit(at(st, st.pos))) {
        skip_rest_of_line(st);
        return;
    }

    const int n = line_number(st);
    skip_line_blanks(st);

    if (at(st, st.pos) == '"')
        st.file = file_name(st);

    skip_rest_of_line(st);
    if (st.pos < st.source.size())
        next_line(st);

    st.line = n;
}

tok_t
keyword_or_identifier(std::string_view spelling)
{
    for (const auto &kw : keywords)
        if (kw.spelling == spelling)
            return kw.kind;

    return tok_t::identifier;
}

// An identifier or a keyword.  A typedef name is an identifier here;
// telling it apart is the parser's work.
token_t
identifier(state_t &st)
{
    std::size_t end = st.pos;
    while (is_ident_char(at(st, end)))
        ++end;

    const auto spelling = st.source.substr(st.pos, end - st.pos);

    return take(st, keyword_or_identifier(spelling), spelling.size());
}

// The end of the digits of a constant starting at the current
// position.  Octal and decimal digits are scanned alike; an 8 or a 9
// in an octal constant is caught once the spelling is known.
std::size_t
digits_end(const state_t &st)
{
    std::size_t end = st.pos;

    if (at(st, end) == '0' &&
        (at(st, end + 1) == 'x' || at(st, end + 1) == 'X')) {
        end += 2;

        const std::size_t first = end;
        while (is_hex_digit(at(st, end)))
            ++end;

        if (end == first)
            fail(location_of(st, st.pos),
                 "hexadecimal constant without digits");

        return end;
    }

    while (is_digit(at(st, end)))
        ++end;

    return end;
}

bool
is_unsigned_suffix(char c)
{
    return c == 'u' || c == 'U';
}

// After an l, L, ll or LL at i.  A mixed-case lL is not a suffix: the
// first letter is taken and the second is left to fail as junk.
std::size_t
long_suffix_end(const state_t &st, std::size_t i)
{
    const char c = at(st, i);
    if (c != 'l' && c != 'L')
        return i;

    return at(st, i + 1) == c ? i + 2 : i + 1;
}

// After the suffix at i: a u or U and a long part in either order,
// each at most once, either or both absent.
std::size_t
suffix_end(const state_t &st, std::size_t i)
{
    const bool u_first = is_unsigned_suffix(at(st, i));
    if (u_first)
        ++i;

    i = long_suffix_end(st, i);

    if (!u_first && is_unsigned_suffix(at(st, i)))
        ++i;

    return i;
}

// An 8 or a 9 after a leading 0, which the digit scan let through.
void
check_octal(const state_t &st, std::string_view digits)
{
    if (digits.size() < 2 || digits[0] != '0' || digits[1] == 'x' ||
        digits[1] == 'X')
        return;

    for (const char c : digits)
        if (!is_octal_digit(c))
            fail(location_of(st, st.pos),
                 std::format("invalid digit '{}' in octal constant", c));
}

// An integer constant, digits and suffix; the value is the parser's to
// read.  An identifier character or a '.' right after the suffix is an
// error: the constant is malformed, or a floating one, outside the
// language this lexer takes.
token_t
number(state_t &st)
{
    const std::size_t digits = digits_end(st);
    check_octal(st, st.source.substr(st.pos, digits - st.pos));

    const std::size_t end = suffix_end(st, digits);

    std::size_t junk = end;
    while (is_ident_char(at(st, junk)) || at(st, junk) == '.')
        ++junk;

    if (junk != end)
        fail(location_of(st, st.pos),
             std::format("invalid integer constant '{}'",
                         st.source.substr(st.pos, junk - st.pos)));

    return take(st, tok_t::integer, end - st.pos);
}

// The value of a simple escape sequence, 6.4.4.5p3, p8, or -1 when
// the character after the backslash names none.
int
simple_escape(char c)
{
    switch (c) {
    case '\'':
    case '"':
    case '?':
    case '\\': return c;
    case 'a': return '\a';
    case 'b': return '\b';
    case 'f': return '\f';
    case 'n': return '\n';
    case 'r': return '\r';
    case 't': return '\t';
    case 'v': return '\v';
    default: return -1;
    }
}

int
hex_digit_value(char c)
{
    if (is_digit(c))
        return c - '0';

    return (c | 0x20) - 'a' + 10;
}

// One character of a literal and the position after it.
struct char_t
{
    int value;
    std::size_t end;
};

// The character at i in the spelling of a literal, whose escapes
// tokenize() has checked.  An octal escape takes at most three digits
// and a hexadecimal one every hex digit that follows, 6.4.4.5p7; a
// hexadecimal value stops growing once it is past a byte, so a long run
// of digits cannot overflow.
char_t
next_char(std::string_view s, std::size_t i)
{
    const auto byte_at = [&](std::size_t k) {
        return k < s.size() ? s[k] : '\0';
    };

    if (s[i] != '\\')
        return { static_cast< unsigned char >(s[i]), i + 1 };

    const char c = byte_at(i + 1);
    if (const int value = simple_escape(c); value >= 0)
        return { value, i + 2 };

    int value = 0;

    if (is_octal_digit(c)) {
        std::size_t k = i + 1;
        for (; k < i + 4 && is_octal_digit(byte_at(k)); ++k)
            value = value * 8 + (s[k] - '0');

        return { value, k };
    }

    std::size_t k = i + 2;
    for (; is_hex_digit(byte_at(k)); ++k)
        value = std::min(value * 16 + hex_digit_value(s[k]), 0x100);

    return { value, k };
}

// The escape sequence whose backslash is at i, checked against the
// grammar of 6.4.4.5p1 and the range of p9: an octal or hexadecimal
// value must fit an unsigned char.  Returns the position after it.
std::size_t
escape_end(const state_t &st, std::size_t i)
{
    const location_t where = location_of(st, i);
    const char c = at(st, i + 1);

    if (c == 'u' || c == 'U')
        not_yet(where, "universal character name");

    if (c == 'x' && !is_hex_digit(at(st, i + 2)))
        fail(where, "\\x used with no following hex digits");

    if (simple_escape(c) < 0 && !is_octal_digit(c) && c != 'x')
        fail(where, std::format("unknown escape sequence: '\\' followed by {}",
                                describe(c)));

    const char_t ch = next_char(st.source, i);
    if (ch.value > 0xff)
        fail(where, std::format("{} escape sequence out of range",
                                c == 'x' ? "hex" : "octal"));

    return ch.end;
}

// A character constant or a string literal, quote to quote, 6.4.4.5p1,
// 6.4.5p1.  Neither spans a line, and every escape is checked here, so
// the decoding functions below cannot fail.  A character constant
// holds one character: none is an error, and the value of more than
// one is implementation-defined, 6.4.4.5p11, outside the subset.
token_t
literal(state_t &st, tok_t kind)
{
    const char quote = st.source[st.pos];
    const location_t where = location_of(st, st.pos);

    std::size_t i = st.pos + 1;
    int count = 0;

    for (char c = at(st, i); c != quote; c = at(st, i), ++count) {
        if (c == '\n' || i >= st.source.size())
            fail(where, std::format("missing terminating {} character", quote));

        i = c == '\\' && at(st, i + 1) != '\n' ? escape_end(st, i) : i + 1;
    }

    if (kind == tok_t::character && count == 0)
        fail(where, "empty character constant");

    if (kind == tok_t::character && count > 1)
        not_yet(where, "multi-character constant");

    return take(st, kind, i + 1 - st.pos);
}

// The length of an encoding prefix at the current position, u8, u, U
// or L right before a quote, 6.4.4.5p1, 6.4.5p1; 0 when there is none.
std::size_t
encoding_prefix_length(const state_t &st)
{
    const auto is_quote = [](char c) { return c == '\'' || c == '"'; };
    const char c = at(st, st.pos);

    if (c == 'u' && at(st, st.pos + 1) == '8' && is_quote(at(st, st.pos + 2)))
        return 2;

    if ((c == 'u' || c == 'U' || c == 'L') && is_quote(at(st, st.pos + 1)))
        return 1;

    return 0;
}

// The longest punctuator at the current position.  A '.' before a
// digit is the start of a floating constant, not a member access.
token_t
punctuator(state_t &st)
{
    const auto rest = st.source.substr(st.pos);

    if (rest[0] == '.' && is_digit(at(st, st.pos + 1)))
        fail(location_of(st, st.pos), "floating constant not supported");

    for (const auto &p : punctuators)
        if (rest.starts_with(p.spelling))
            return take(st, p.kind, p.spelling.size());

    fail(location_of(st, st.pos),
         std::format("unexpected character {}", describe(rest[0])));
}

} // namespace

std::vector< token_t >
tokenize(std::string_view source, std::string_view file)
{
    state_t st { source, 0, file, 1, 0 };
    std::vector< token_t > tokens;

    for (skip_blanks(st); st.pos < source.size(); skip_blanks(st)) {
        const char c = source[st.pos];

        // a line marker's file name is quoted too, but the marker is
        // taken whole before a quote can open a string
        if (c == '#' && starts_line(st))
            line_marker(st);
        else if (const std::size_t n = encoding_prefix_length(st); n > 0)
            not_yet(
                location_of(st, st.pos),
                std::format("encoding prefix '{}'", source.substr(st.pos, n)));
        else if (c == '\'')
            tokens.push_back(literal(st, tok_t::character));
        else if (c == '"')
            tokens.push_back(literal(st, tok_t::string));
        else if (is_ident_start(c))
            tokens.push_back(identifier(st));
        else if (is_digit(c))
            tokens.push_back(number(st));
        else
            tokens.push_back(punctuator(st));
    }

    tokens.push_back(take(st, tok_t::eof, 0));

    return tokens;
}

// The spelling between the quotes, decoded; no zero is appended.
std::string
string_bytes(const token_t &t)
{
    const std::string_view body = t.text.substr(1, t.text.size() - 2);

    std::string bytes;
    for (std::size_t i = 0; i < body.size();) {
        const char_t c = next_char(body, i);
        bytes.push_back(static_cast< char >(c.value));
        i = c.end;
    }

    return bytes;
}

// The value of a char object holding the character, converted to int,
// 6.4.4.5p11.  char is signed on x86-64, so '\xff' is -1.
int
character_value(const token_t &t)
{
    const char_t c = next_char(t.text, 1);

    return static_cast< signed char >(c.value);
}

} // namespace rcc
