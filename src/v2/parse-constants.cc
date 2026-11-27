// -*- mode: c++; -*-

// 6.4.4.2: an integer constant's value and type, read from its
// spelling as rcc's parser reads it, and the operand of a constant of a
// signed type, int or long.

#include "parse-common.hh"

#include "build.hh"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <string_view>

namespace rcc {

namespace {

// A digit's value in a base up to 16, or -1 for a character that is no
// digit, which ends the digits.
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

// What a suffix says of the type: u, and the count of l.
struct suffix_t
{
    bool is_unsigned;
    int longs;
};

// u or U and l, L, ll or LL in either order, 6.4.4.2p1; a mixed-case
// lL is not a suffix.  The lexer has refused every other suffix but
// C23's wb, so what is left after these is that one or nothing.
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

    if (!s.empty())
        not_yet(t, "bit-precise integer constant");

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

} // namespace

// The value from the spelling, 6.4.4.2p4, p5.  The lexer has checked
// the form; a value past 64 bits has no type (6.4.4.2p7).  A constant
// of an unsigned type is not yet.  The node's value is the bits its
// register holds, which a signed type's value is.
operand_t
integer_constant(parser_t &p)
{
    const token_t &t = advance(p);

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

    if (!as< int_t >(*type).is_signed)
        not_yet(t, "integer constant whose type is unsigned");

    return { *type,
             make_literal(static_cast< long long >(value), view_of(*type)) };
}

} // namespace rcc
