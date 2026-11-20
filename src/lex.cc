// -*- mode: c++; -*-

// The lexer: preprocessed C in, tokens out.  The input is what
// clang -E writes, so there are no comments and no directives but the
// line markers, which move the locations back onto the source file and
// say which text is a system header's.

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

// A packing a #pragma pack(push) saved, under the label the push names
// or none, for the pop that restores it.
struct saved_packing_t
{
    std::string_view label;
    bool packed;
};

// Where the lexer stands.  The column of a position is its distance
// from origin, the offset of the first byte of the current line.
// system_header is what the last line marker said of the text after
// it.  packed is whether the #pragma pack in force sets a packing,
// saved the packings its pushes saved; unread is whether one was in a
// form rcc does not read, after which any packing may be in force.
struct state_t
{
    std::string_view source;
    std::size_t pos;
    std::string_view file;
    int line;
    std::size_t origin;
    bool system_header;
    bool packed;
    std::vector< saved_packing_t > saved;
    bool unread;
};

struct keyword_t
{
    std::string_view spelling;
    tok_t kind;
};

const keyword_t keywords[] = {
    { "auto", tok_t::kw_auto },
    { "break", tok_t::kw_break },
    { "case", tok_t::kw_case },
    { "char", tok_t::kw_char },
    { "const", tok_t::kw_const },
    { "continue", tok_t::kw_continue },
    { "default", tok_t::kw_default },
    { "do", tok_t::kw_do },
    { "double", tok_t::kw_double },
    { "else", tok_t::kw_else },
    { "enum", tok_t::kw_enum },
    { "extern", tok_t::kw_extern },
    { "float", tok_t::kw_float },
    { "for", tok_t::kw_for },
    { "goto", tok_t::kw_goto },
    { "if", tok_t::kw_if },
    { "int", tok_t::kw_int },
    { "long", tok_t::kw_long },
    { "register", tok_t::kw_register },
    { "restrict", tok_t::kw_restrict },
    { "return", tok_t::kw_return },
    { "short", tok_t::kw_short },
    { "signed", tok_t::kw_signed },
    { "sizeof", tok_t::kw_sizeof },
    { "static", tok_t::kw_static },
    { "struct", tok_t::kw_struct },
    { "switch", tok_t::kw_switch },
    { "typedef", tok_t::kw_typedef },
    { "union", tok_t::kw_union },
    { "unsigned", tok_t::kw_unsigned },
    { "void", tok_t::kw_void },
    { "volatile", tok_t::kw_volatile },
    { "while", tok_t::kw_while },

    // the keywords C99 and C11 add but restrict; C17 adds none
    { "_Alignas", tok_t::kw_alignas },
    { "_Alignof", tok_t::kw_alignof },
    { "_Atomic", tok_t::kw_atomic },
    { "_Bool", tok_t::kw_bool },
    { "_Complex", tok_t::kw_complex },
    { "_Generic", tok_t::kw_generic },
    { "_Imaginary", tok_t::kw_imaginary },
    { "_Noreturn", tok_t::kw_noreturn },
    { "_Static_assert", tok_t::kw_static_assert },
    { "_Thread_local", tok_t::kw_thread_local },
    { "inline", tok_t::kw_inline },

    // GNU C's spellings in the implementation's name space, which a
    // header writes so that no dialect and no macro of the program's
    // takes them: each is a keyword of C's, or one of GNU C's own
    { "__asm", tok_t::kw_asm },
    { "__asm__", tok_t::kw_asm },
    { "__attribute", tok_t::kw_attribute },
    { "__attribute__", tok_t::kw_attribute },
    { "__const", tok_t::kw_const },
    { "__const__", tok_t::kw_const },
    { "__extension__", tok_t::kw_extension },
    { "__float128", tok_t::kw_float128 },
    { "__inline", tok_t::kw_inline },
    { "__inline__", tok_t::kw_inline },
    { "__int128", tok_t::kw_int128 },
    { "__restrict", tok_t::kw_restrict },
    { "__restrict__", tok_t::kw_restrict },
    { "__signed", tok_t::kw_signed },
    { "__signed__", tok_t::kw_signed },
    { "__volatile", tok_t::kw_volatile },
    { "__volatile__", tok_t::kw_volatile },
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
    return { st.file, st.line, int(pos - st.origin) + 1, st.system_header };
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
// A packing rcc cannot read leaves every token after it packed.
token_t
take(state_t &st, tok_t kind, std::size_t length)
{
    const token_t tok { kind, st.source.substr(st.pos, length),
                        location_of(st, st.pos), st.packed || st.unread };

    st.pos += length;

    return tok;
}

// The location just past a token, where what follows it is said: a
// token never spans a newline, so it is on the token's line, its
// length past the token's first byte.
location_t
end_of(const token_t &t)
{
    location_t where = t.where;
    where.column += int(t.text.size());

    return where;
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

// Whether the flags after a marker's file name say that the text after
// it is a system header's.  Each flag is a number: 1 enters a file, 2
// returns to one, 3 marks a system header's text, and 4 text read as
// if in an implicit extern "C".  Only 3 matters here.
bool
system_header_flag(state_t &st)
{
    bool system = false;

    for (skip_line_blanks(st); is_digit(at(st, st.pos)); skip_line_blanks(st)) {
        const std::size_t first = st.pos;
        while (is_digit(at(st, st.pos)))
            ++st.pos;

        if (st.source.substr(first, st.pos - first) == "3")
            system = true;
    }

    return system;
}

// The next word of a directive, the blanks before it skipped: a run of
// identifier characters, which is a name or a number, or one other
// character; empty at the end of the line.
std::string_view
directive_word(state_t &st)
{
    skip_line_blanks(st);
    const std::size_t first = st.pos;

    if (is_ident_char(at(st, st.pos)))
        while (is_ident_char(at(st, st.pos)))
            ++st.pos;
    else if (st.pos < st.source.size() && at(st, st.pos) != '\n')
        ++st.pos;

    return st.source.substr(first, st.pos - first);
}

// The arguments of a #pragma pack, each one word: ( ), or a word and
// more after commas, in parentheses that end the line.  False for any
// other spelling, which the arguments then do not hold.
bool
pack_arguments(state_t &st, std::vector< std::string_view > &args)
{
    if (directive_word(st) != "(")
        return false;

    std::string_view word = directive_word(st);
    if (word != ")")
        for (;;) {
            if (word.empty() || !is_ident_char(word[0]))
                return false;
            args.push_back(word);

            word = directive_word(st);
            if (word == ")")
                break;
            if (word != ",")
                return false;

            word = directive_word(st);
        }

    return directive_word(st).empty();
}

// Whether a number of a #pragma pack sets a packing: any but zero,
// which sets none, as clang reads 0 and 0x0.
bool
sets_packing(std::string_view number)
{
    return number.find_first_not_of("0xX") != std::string_view::npos;
}

// The packing saved last under the label, or the end of the saved
// ones where none is.
std::vector< saved_packing_t >::iterator
saved_under(state_t &st, std::string_view label)
{
    for (auto it = st.saved.end(); it != st.saved.begin(); --it)
        if (it[-1].label == label)
            return it - 1;

    return st.saved.end();
}

// pack(pop) restores the packing saved last, and changes nothing with
// nothing saved, as clang has it; pack(pop, label) the one saved
// under the label, which apply_pack() found saved, dropping those
// saved after it.
void
pop_packing(state_t &st, std::string_view label)
{
    if (st.saved.empty())
        return;

    const auto it = label.empty() ? st.saved.end() - 1 : saved_under(st, label);
    st.packed = it->packed;
    st.saved.erase(it, st.saved.end());
}

// The arguments of a #pragma pack in clang's forms: none, a packing of
// none; a number; show, which changes nothing; push or pop, then a
// label, a number, or both in that order.  push saves the packing in
// force, pop restores one, and the number then sets the packing.
// clang -E writes the pragma as the source does, a macro in it
// unexpanded, where clang expands it, so a name is a label only where
// it can be nothing else: before a number after push, and after pop
// where a push saved a packing under it.  A bare name where a number
// may stand may be a macro for one.  False for that and for any other
// form.
bool
apply_pack(state_t &st, const std::vector< std::string_view > &args)
{
    if (args.empty() || (args.size() == 1 && is_digit(args[0][0]))) {
        st.packed = !args.empty() && sets_packing(args[0]);
        return true;
    }

    if (args.size() == 1 && args[0] == "show")
        return true;

    if ((args[0] != "push" && args[0] != "pop") || args.size() > 3)
        return false;

    const bool push = args[0] == "push";
    const bool labelled = args.size() > 1 && !is_digit(args[1][0]);
    const std::size_t numbers = args.size() - 1 - (labelled ? 1 : 0);
    if (numbers > 1 || (numbers == 1 && !is_digit(args.back()[0])))
        return false;

    const std::string_view label = labelled ? args[1] : std::string_view();
    const bool only_label =
        push ? numbers == 1 : saved_under(st, label) != st.saved.end();
    if (labelled && !only_label)
        return false;

    if (push)
        st.saved.push_back(saved_packing_t { label, st.packed });
    else
        pop_packing(st, label);

    if (numbers == 1)
        st.packed = sets_packing(args.back());

    return true;
}

// A #pragma pack sets the packing of the tokens after it, which clang
// applies to a record whose list opens there (shape.md chapter 2).
// After a form rcc does not read, any packing may be in force, so
// every token after it is packed.
void
pack_pragma(state_t &st)
{
    std::vector< std::string_view > args;

    if (!pack_arguments(st, args) || !apply_pack(st, args))
        st.unread = true;
}

// A #pragma: pack is read; weak, which makes a symbol weak or names
// an alias of it, and GCC visibility, which hides the symbols a region
// declares, are not yet, refused at their name, since rcc writes each
// symbol strong and visible (shape.md chapter 2); any other is
// skipped.
void
pragma(state_t &st)
{
    const std::string_view name = directive_word(st);
    const location_t where = location_of(st, st.pos - name.size());

    if (name == "pack")
        pack_pragma(st);
    else if (name == "weak")
        not_yet(where, "pragma 'weak'");
    else if (name == "GCC" && directive_word(st) == "visibility")
        not_yet(where, "pragma 'GCC visibility'");
}

// A directive other than a line marker: a #pragma is read, every other
// one skipped whole.
void
directive(state_t &st)
{
    if (directive_word(st) == "pragma")
        pragma(st);

    skip_rest_of_line(st);
}

// A line opening with '#'.  A line marker, # N "name" flags or
// #line N "name", sets the file and the number of the next line, and
// says anew whether the text after it is a system header's; any other
// is a directive().
void
line_marker(state_t &st)
{
    ++st.pos;
    skip_line_blanks(st);
    skip_line_keyword(st);

    if (!is_digit(at(st, st.pos))) {
        directive(st);
        return;
    }

    const int n = line_number(st);
    skip_line_blanks(st);

    if (at(st, st.pos) == '"')
        st.file = file_name(st);

    st.system_header = system_header_flag(st);
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

// Whether the constant at the current position opens with 0x or 0X.
bool
is_hex_prefix(const state_t &st)
{
    return at(st, st.pos) == '0' &&
           (at(st, st.pos + 1) == 'x' || at(st, st.pos + 1) == 'X');
}

// The binary prefix, 0b or 0B, 6.4.4.2p1.
bool
is_binary_prefix(const state_t &st)
{
    return at(st, st.pos) == '0' &&
           (at(st, st.pos + 1) == 'b' || at(st, st.pos + 1) == 'B');
}

// The end of the digits of a constant starting at the current
// position.  Octal and decimal digits are scanned alike; an 8 or a 9
// in an octal constant is caught once the spelling is known.  A
// hexadecimal floating constant may have its digits after the period
// alone, 6.4.4.3p1.  A binary constant has one digit at least, each 0
// or 1, 6.4.4.2p1, p4.
std::size_t
digits_end(const state_t &st)
{
    std::size_t end = st.pos;

    if (is_binary_prefix(st)) {
        end += 2;

        const std::size_t first = end;
        for (; is_digit(at(st, end)); ++end)
            if (at(st, end) != '0' && at(st, end) != '1')
                fail(location_of(st, end),
                     std::format("invalid digit '{}' in binary constant",
                                 at(st, end)));

        if (end == first)
            fail(location_of(st, st.pos), "binary constant without digits");

        return end;
    }

    if (is_hex_prefix(st)) {
        end += 2;

        const std::size_t first = end;
        while (is_hex_digit(at(st, end)))
            ++end;

        const bool fraction =
            at(st, end) == '.' && is_hex_digit(at(st, end + 1));
        if (end == first && !fraction)
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

// After the bit-precise suffix at i, wb or WB, 6.4.4.2p1; i where there
// is none.  A mixed-case wB is not one.
std::size_t
bit_precise_suffix_end(const state_t &st, std::size_t i)
{
    const char w = at(st, i);
    const char b = at(st, i + 1);
    if ((w == 'w' && b == 'b') || (w == 'W' && b == 'B'))
        return i + 2;

    return i;
}

// After the suffix at i: a u or U and a long part or a bit-precise
// part in either order, each at most once, either or both absent.
std::size_t
suffix_end(const state_t &st, std::size_t i)
{
    const bool u_first = is_unsigned_suffix(at(st, i));
    if (u_first)
        ++i;

    const std::size_t bit_precise = bit_precise_suffix_end(st, i);
    i = bit_precise != i ? bit_precise : long_suffix_end(st, i);

    if (!u_first && is_unsigned_suffix(at(st, i)))
        ++i;

    return i;
}

// An 8 or a 9 after a leading 0, which the digit scan let through.
void
check_octal(const state_t &st, std::string_view digits)
{
    if (digits.size() < 2 || digits[0] != '0' || digits[1] == 'x' ||
        digits[1] == 'X' || digits[1] == 'b' || digits[1] == 'B')
        return;

    for (const char c : digits)
        if (!is_octal_digit(c))
            fail(location_of(st, st.pos),
                 std::format("invalid digit '{}' in octal constant", c));
}

// An identifier character or a '.' right after the suffix of a
// constant that ends at end makes it malformed, and the message names
// the whole run; kind is the constant's, integer or floating.
void
check_constant_end(const state_t &st, std::size_t end, std::string_view kind)
{
    std::size_t junk = end;
    while (is_ident_char(at(st, junk)) || at(st, junk) == '.')
        ++junk;

    if (junk != end)
        fail(location_of(st, st.pos),
             std::format("invalid {} constant '{}'", kind,
                         st.source.substr(st.pos, junk - st.pos)));
}

// The letter that opens the exponent part, 6.4.4.3p1: e or E after a
// decimal significand, p or P after a hexadecimal one, where an e is a
// digit.
bool
is_exponent_letter(char c, bool hex)
{
    if (hex)
        return c == 'p' || c == 'P';

    return c == 'e' || c == 'E';
}

// After the exponent part whose letter is at i: an optional sign and a
// digit sequence, decimal in either base, 6.4.4.3p1.
std::size_t
exponent_end(const state_t &st, std::size_t i)
{
    ++i;
    if (at(st, i) == '+' || at(st, i) == '-')
        ++i;

    const std::size_t first = i;
    while (is_digit(at(st, i)))
        ++i;

    if (i == first)
        fail(location_of(st, st.pos), "exponent has no digits");

    return i;
}

// After the floating suffix at i, or i where there is none: f, l, F or
// L, or one of df, dd, dl, DF, DD and DL, 6.4.4.3p1.  p2 keeps the six
// decimal ones off a hexadecimal constant, where they are junk.
std::size_t
floating_suffix_end(const state_t &st, std::size_t i, bool hex)
{
    const char c = at(st, i);
    if (c == 'f' || c == 'F' || c == 'l' || c == 'L')
        return i + 1;

    const char d = at(st, i + 1);
    const bool lower = c == 'd' && (d == 'f' || d == 'd' || d == 'l');
    const bool upper = c == 'D' && (d == 'F' || d == 'D' || d == 'L');
    if (!hex && (lower || upper))
        return i + 2;

    return i;
}

// A floating constant, 6.4.4.3p1, whose whole-number digits end at i:
// the period and the fraction's digits, the exponent part, which a
// hexadecimal constant must have, and the suffix.  Either the
// whole-number part or the fraction is present, since the caller
// starts here only on a digit or on a period before one.  The value
// is not computed: the parser refuses the constant where an expression
// uses it.
token_t
floating(state_t &st, std::size_t i, bool hex)
{
    if (at(st, i) == '.') {
        ++i;
        while (hex ? is_hex_digit(at(st, i)) : is_digit(at(st, i)))
            ++i;
    }

    if (is_exponent_letter(at(st, i), hex))
        i = exponent_end(st, i);
    else if (hex)
        fail(location_of(st, st.pos),
             "hexadecimal floating constant requires an exponent");

    const std::size_t end = floating_suffix_end(st, i, hex);
    check_constant_end(st, end, "floating");

    return take(st, tok_t::floating, end - st.pos);
}

// An integer or a floating constant, digits and suffix; the value of an
// integer one is the parser's to read.  The constant is a floating one
// when its digits go on to a period or to the letter of an exponent,
// 6.4.4.3p1, but for a binary one, which no floating constant is.  An
// identifier character or a '.' right after the suffix is an error:
// the constant is malformed.
token_t
number(state_t &st)
{
    const bool hex = is_hex_prefix(st);
    const bool binary = is_binary_prefix(st);
    const std::size_t digits = digits_end(st);

    const bool fraction =
        at(st, digits) == '.' || is_exponent_letter(at(st, digits), hex);
    if (!binary && fraction)
        return floating(st, digits, hex);

    check_octal(st, st.source.substr(st.pos, digits - st.pos));

    const std::size_t end = suffix_end(st, digits);
    check_constant_end(st, end, "integer");

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

// The encoding a prefix spells, 6.4.4.5p1: u8, u, U or L, or none.
encoding_t
encoding_named(std::string_view prefix)
{
    if (prefix == "u8")
        return encoding_t::utf8;

    if (prefix == "u")
        return encoding_t::utf16;

    if (prefix == "U")
        return encoding_t::utf32;

    if (prefix == "L")
        return encoding_t::wide;

    return encoding_t::none;
}

// Whether an encoding is a wide one, whose elements are wider than a
// byte, 6.4.4.5p2, 6.4.5p3.
bool
is_wide(encoding_t encoding)
{
    return encoding != encoding_t::none && encoding != encoding_t::utf8;
}

// The greatest value of an element in the encoding, which bounds an
// octal or a hexadecimal escape, 6.4.4.5p9: an unsigned char's, or the
// char8_t's, a byte; char16_t's; char32_t's, and the unsigned type of
// wchar_t's, both 32 bits on both targets.
std::uint32_t
greatest_unit(encoding_t encoding)
{
    switch (encoding) {
    case encoding_t::none:
    case encoding_t::utf8: return 0xff;
    case encoding_t::utf16: return 0xffff;
    default: return 0xffffffff;
    }
}

// A character of UTF-8 at i, its code point and its length in bytes, or
// a length of 0 where the bytes are none: a byte that leads no
// character, a lead byte without its continuation bytes, an overlong
// form, a surrogate, or a code point past 10FFFF (RFC 3629).
struct utf8_t
{
    std::uint32_t code_point;
    std::size_t length;
};

utf8_t
utf8_at(std::string_view s, std::size_t i)
{
    const auto byte = [&](std::size_t k) -> std::uint32_t {
        return k < s.size() ? static_cast< unsigned char >(s[k]) : 0;
    };

    const std::uint32_t lead = byte(i);
    if (lead < 0x80)
        return { lead, 1 };

    std::size_t length = 0;
    std::uint32_t least = 0;
    std::uint32_t code_point = 0;

    if ((lead & 0xe0) == 0xc0) {
        length = 2;
        least = 0x80;
        code_point = lead & 0x1f;
    }
    else if ((lead & 0xf0) == 0xe0) {
        length = 3;
        least = 0x800;
        code_point = lead & 0x0f;
    }
    else if ((lead & 0xf8) == 0xf0) {
        length = 4;
        least = 0x10000;
        code_point = lead & 0x07;
    }
    else
        return { 0, 0 };

    for (std::size_t k = 1; k < length; ++k) {
        const std::uint32_t c = byte(i + k);
        if ((c & 0xc0) != 0x80)
            return { 0, 0 };

        code_point = code_point << 6 | (c & 0x3f);
    }

    const bool surrogate = code_point >= 0xd800 && code_point <= 0xdfff;
    if (code_point < least || code_point > 0x10ffff || surrogate)
        return { 0, 0 };

    return { code_point, length };
}

// One element of a literal's text and the position after it.  value is
// the element's own value where an escape gives it, a simple one or an
// octal or hexadecimal one, 6.4.4.5p5, p6, and where the text is read as
// bytes; it is a code point, code_point set, for a universal character
// name, 6.4.3p4, and for a character of the text read as UTF-8, which
// the literal's encoding then encodes.  A hexadecimal escape's value is
// held past 32 bits, so that escape_end() can refuse it.
struct char_t
{
    std::uint64_t value;
    bool code_point;
    std::size_t end;
};

// The element at i in the spelling of a literal, whose escapes and
// characters tokenize() has checked, its text read as UTF-8 when utf8
// is set and as bytes otherwise.  An octal escape takes at most three
// digits and a hexadecimal one every hex digit that follows, 6.4.4.5p7;
// a hexadecimal value stops growing once it is past 32 bits, so a long
// run of digits cannot overflow.  \u takes four hex digits and \U
// eight, 6.4.3p1.
char_t
next_char(std::string_view s, std::size_t i, bool utf8)
{
    const auto byte_at = [&](std::size_t k) {
        return k < s.size() ? s[k] : '\0';
    };

    if (s[i] != '\\' && utf8) {
        const utf8_t c = utf8_at(s, i);
        return { c.code_point, true, i + c.length };
    }

    if (s[i] != '\\')
        return { static_cast< unsigned char >(s[i]), false, i + 1 };

    const char c = byte_at(i + 1);
    if (const int value = simple_escape(c); value >= 0)
        return { std::uint32_t(value), false, i + 2 };

    std::uint64_t value = 0;

    if (is_octal_digit(c)) {
        std::size_t k = i + 1;
        for (; k < i + 4 && is_octal_digit(byte_at(k)); ++k)
            value = value * 8 + std::uint64_t(s[k] - '0');

        return { value, false, k };
    }

    const std::size_t digits = c == 'u' ? 4 : c == 'U' ? 8 : 0;
    std::size_t k = i + 2;
    for (; is_hex_digit(byte_at(k)) && (digits == 0 || k < i + 2 + digits); ++k)
        value = std::min< std::uint64_t >(
            value * 16 + std::uint64_t(hex_digit_value(s[k])), 1ull << 32);

    return { value, digits > 0, k };
}

// A universal character name at i, checked against 6.4.3p1, four or
// eight hex digits, and the constraint of p2: no surrogate and no code
// point past 10FFFF.  Inside a literal, the only place the lexer reads
// one, any other code point is taken, the basic characters' included.
void
check_universal_name(const state_t &st, std::size_t i)
{
    const location_t where = location_of(st, i);
    const std::size_t digits = at(st, i + 1) == 'u' ? 4 : 8;

    for (std::size_t k = i + 2; k < i + 2 + digits; ++k)
        if (!is_hex_digit(at(st, k)))
            fail(where, "incomplete universal character name");

    const auto code_point = std::uint32_t(next_char(st.source, i, false).value);
    const bool surrogate = code_point >= 0xd800 && code_point <= 0xdfff;
    if (surrogate || code_point > 0x10ffff)
        fail(where, "invalid universal character");
}

// The escape sequence whose backslash is at i, checked against the
// grammar of 6.4.4.5p1 and the range of p9: an octal or hexadecimal
// value must fit an element of the literal's encoding.  Returns the
// position after it.
std::size_t
escape_end(const state_t &st, std::size_t i, encoding_t encoding)
{
    const location_t where = location_of(st, i);
    const char c = at(st, i + 1);

    if (c == 'u' || c == 'U') {
        check_universal_name(st, i);
        return next_char(st.source, i, false).end;
    }

    if (c == 'x' && !is_hex_digit(at(st, i + 2)))
        fail(where, "\\x used with no following hex digits");

    if (simple_escape(c) < 0 && !is_octal_digit(c) && c != 'x')
        fail(where, std::format("unknown escape sequence: '\\' followed by {}",
                                describe(c)));

    const char_t ch = next_char(st.source, i, false);
    if (ch.value > greatest_unit(encoding))
        fail(where, std::format("{} escape sequence out of range",
                                c == 'x' ? "hex" : "octal"));

    return ch.end;
}

// The words a message names a literal's prefix by, after the spelling of
// the token at begin.
std::string
prefix_words(const state_t &st, std::size_t begin, std::size_t prefix)
{
    return std::format("'{}'", st.source.substr(begin, prefix));
}

// One element of a literal's text, the character or the escape at i,
// checked, and the position after it.  A character of a literal of a
// prefix is UTF-8, which u8 asks for, 6.4.5p6, and which a wide one is
// decoded from; a malformed one is an error.  A literal of no prefix
// takes the bytes of its text as they are, in the literal encoding,
// which is the source's own, UTF-8.
char_t
checked_char(const state_t &st,
             std::size_t i,
             std::size_t begin,
             std::size_t prefix,
             encoding_t encoding)
{
    if (at(st, i) == '\\' && at(st, i + 1) != '\n') {
        const std::size_t end = escape_end(st, i, encoding);
        const char_t ch = next_char(st.source, i, false);

        return { ch.value, ch.code_point, end };
    }

    if (encoding == encoding_t::none)
        return next_char(st.source, i, false);

    const utf8_t c = utf8_at(st.source, i);
    if (c.length == 0)
        fail(location_of(st, i),
             "invalid UTF-8 sequence after encoding prefix " +
                 prefix_words(st, begin, prefix));

    return { c.code_point, true, i + c.length };
}

// What 6.4.4.5 asks of a character constant's one element, ch: one of
// no prefix holds a byte, its value a char's, p11, so a character outside
// ASCII, or a universal character name of one, is too large for it, as
// clang has it; one of u holds a code point of one UTF-16 code unit,
// p10.  An escape's value is checked by escape_end().
void
check_character(const location_t &where,
                const char_t &ch,
                bool from_text,
                encoding_t encoding)
{
    const bool character = ch.code_point || from_text;
    if (encoding == encoding_t::none && character && ch.value > 0x7f)
        fail(where, "character too large for a character constant");

    if (encoding == encoding_t::utf16 && ch.code_point && ch.value > 0xffff)
        fail(where, "character too large for a 'u' character constant");
}

// A character constant or a string literal, its prefix and quote to
// quote, 6.4.4.5p1, 6.4.5p1.  Neither spans a line, and every escape and
// every character is checked here, so the decoding functions below
// cannot fail.  A character constant holds one character: none is an
// error; one of a prefix holds no more, p10 for u and U, and clang
// refuses more after L too; the value of more than one without a
// prefix is implementation-defined, p11, outside the subset.
token_t
literal(state_t &st, tok_t kind, std::size_t prefix)
{
    const std::size_t begin = st.pos;
    const char quote = st.source[begin + prefix];
    const location_t where = location_of(st, begin);
    const encoding_t encoding = encoding_named(st.source.substr(begin, prefix));

    std::size_t i = begin + prefix + 1;
    int count = 0;

    for (char c = at(st, i); c != quote; c = at(st, i), ++count) {
        if (c == '\n' || i >= st.source.size())
            fail(where, std::format("missing terminating {} character", quote));

        const bool from_text = c != '\\';
        const char_t ch = checked_char(st, i, begin, prefix, encoding);
        if (kind == tok_t::character)
            check_character(where, ch, from_text, encoding);

        i = ch.end;
    }

    if (kind == tok_t::character && count == 0)
        fail(where, "empty character constant");

    if (kind == tok_t::character && count > 1 && encoding != encoding_t::none)
        fail(where, "more than one character in a wide character constant");

    if (kind == tok_t::character && count > 1)
        not_yet(where, "multi-character constant");

    return take(st, kind, i + 1 - begin);
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

// A literal of a prefix.  u8 on a character constant is C23's, and the
// dialect rcc preprocesses in has none, so it is not yet.
token_t
prefixed_literal(state_t &st, std::size_t prefix)
{
    if (at(st, st.pos + prefix) == '"')
        return literal(st, tok_t::string, prefix);

    if (prefix == 2)
        not_yet(location_of(st, st.pos),
                "encoding prefix 'u8' on a character constant");

    return literal(st, tok_t::character, prefix);
}

// The longest punctuator at the current position.
token_t
punctuator(state_t &st)
{
    const auto rest = st.source.substr(st.pos);

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
    state_t st { source, 0, file, 1, 0, false, false, { }, false };
    std::vector< token_t > tokens;

    for (skip_blanks(st); st.pos < source.size(); skip_blanks(st)) {
        const char c = source[st.pos];

        // a line marker's file name is quoted too, but the marker is
        // taken whole before a quote can open a string
        if (c == '#' && starts_line(st))
            line_marker(st);
        else if (const std::size_t n = encoding_prefix_length(st); n > 0)
            tokens.push_back(prefixed_literal(st, n));
        else if (c == '\'')
            tokens.push_back(literal(st, tok_t::character, 0));
        else if (c == '"')
            tokens.push_back(literal(st, tok_t::string, 0));
        else if (is_ident_start(c))
            tokens.push_back(identifier(st));
        // a '.' before a digit starts a floating constant, not a
        // member access
        else if (is_digit(c) || (c == '.' && is_digit(at(st, st.pos + 1))))
            tokens.push_back(number(st));
        else
            tokens.push_back(punctuator(st));
    }

    token_t eof = take(st, tok_t::eof, 0);
    if (!tokens.empty())
        eof.where = end_of(tokens.back());

    tokens.push_back(eof);
    return tokens;
}

namespace {

// The length of a literal's prefix: what comes before its quote.
std::size_t
prefix_length(const token_t &t)
{
    return t.text.find_first_of("'\"");
}

// The text between a literal's quotes.
std::string_view
literal_body(const token_t &t)
{
    const std::size_t open = prefix_length(t) + 1;

    return t.text.substr(open, t.text.size() - open - 1);
}

// A code point's code units in an encoding: its UTF-8 bytes for none and
// u8, one or two UTF-16 code units, the second pair of a surrogate past
// FFFF, or itself in UTF-32.
void
encode(std::uint32_t code_point,
       encoding_t encoding,
       std::vector< std::uint32_t > &units)
{
    if (is_wide(encoding) && encoding != encoding_t::utf16) {
        units.push_back(code_point);
        return;
    }

    if (encoding == encoding_t::utf16) {
        if (code_point < 0x10000) {
            units.push_back(code_point);
            return;
        }

        const std::uint32_t v = code_point - 0x10000;
        units.push_back(0xd800 | (v >> 10));
        units.push_back(0xdc00 | (v & 0x3ff));
        return;
    }

    if (code_point < 0x80) {
        units.push_back(code_point);
        return;
    }

    const int length = code_point < 0x800 ? 2 : code_point < 0x10000 ? 3 : 4;
    const std::uint32_t lead = length == 2 ? 0xc0 : length == 3 ? 0xe0 : 0xf0;

    units.push_back(lead | (code_point >> (6 * (length - 1))));
    for (int k = length - 2; k >= 0; --k)
        units.push_back(0x80 | ((code_point >> (6 * k)) & 0x3f));
}

} // namespace

encoding_t
encoding_of(const token_t &t)
{
    return encoding_named(t.text.substr(0, prefix_length(t)));
}

std::vector< std::uint32_t >
string_units(const token_t &t, encoding_t encoding)
{
    const std::string_view body = literal_body(t);

    std::vector< std::uint32_t > units;
    for (std::size_t i = 0; i < body.size();) {
        const char_t c = next_char(body, i, is_wide(encoding));

        if (c.code_point)
            encode(std::uint32_t(c.value), encoding, units);
        else
            units.push_back(std::uint32_t(c.value));

        i = c.end;
    }

    return units;
}

std::string
string_bytes(const token_t &t)
{
    std::string bytes;
    for (const std::uint32_t unit : string_units(t, encoding_t::none))
        bytes.push_back(static_cast< char >(unit));

    return bytes;
}

// The value of a char object holding the character, converted to int,
// 6.4.4.5p11: '\xff' is -1 where plain char is signed, 255 where it is
// unsigned.
int
character_value(const token_t &t, bool char_is_signed)
{
    const char_t c = next_char(literal_body(t), 0, false);
    if (char_is_signed)
        return static_cast< signed char >(c.value);

    return int(c.value);
}

std::uint32_t
wide_character_value(const token_t &t)
{
    return std::uint32_t(next_char(literal_body(t), 0, true).value);
}

bool
is_utf8(const token_t &t)
{
    const std::string_view body = literal_body(t);

    for (std::size_t i = 0; i < body.size();) {
        if (body[i] == '\\') {
            i = next_char(body, i, false).end;
            continue;
        }

        const std::size_t length = utf8_at(body, i).length;
        if (length == 0)
            return false;

        i += length;
    }

    return true;
}

} // namespace rcc
