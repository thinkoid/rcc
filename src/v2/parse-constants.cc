// -*- mode: c++; -*-

// 6.4.4.2 and 6.4.4.5: an integer constant's value and type, read from
// its spelling as rcc's parser reads it, and the operand of a constant
// of any of its types; a character constant of no prefix; and 6.6p4, the
// mark of a constant reached through a signed overflow, which every
// operator's production sets and a constant expression warns of.

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
// the form; a value past 64 bits has no type (6.4.4.2p7).  The node's
// value is the bits its register holds, read signed at its type's
// width: the unsigned 4294967295 is -1.
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

    const long long bits = as< int_t >(*type).width == 32 ?
                               static_cast< std::int32_t >(value) :
                               static_cast< long long >(value);

    return { *type, make_literal(bits, view_of(*type)) };
}

// A character constant, 6.4.4.5, rcc's character_constant(): of no
// prefix, an int holding the value a char object holding the character
// has, converted to int, p11, so '\377' is -1 where plain char is signed
// and 255 where it is unsigned, as the target has it.  A prefixed one,
// of wchar_t, char8_t, char16_t or char32_t, p12 to p15, is not yet.
operand_t
character_constant(parser_t &p)
{
    const token_t &t = advance(p);
    if (encoding_of(t) != encoding_t::none)
        not_yet(t, "prefixed character constant");

    const int value = character_value(t, p.target.char_is_signed);

    return { int_type(), make_literal(value, view_t::i32) };
}

namespace {

// A constant's value exactly, past 64 bits, where a sum, a difference
// or a product of two 64-bit values is judged: GNU C's __int128.
using exact_t = __int128;

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

// Whether an operation of the type t on two constants a and b takes a
// signed result t does not hold, 6.6p4, rcc's overflows(): a sum, a
// difference or a product past t's range.  A left shift, as clang
// judges one, only where a set bit of a nonnegative operand passes the
// sign bit, so 2 << 31 overflows and 1 << 31 is the most negative int;
// a negative operand shifted draws nothing.  Every other operation
// gives a value its type holds.  A signed constant's value is the
// tree's as it is: the register's bits, read signed at its width.
bool
overflows(binary_op_t op, const type_t &t, long long a, long long b)
{
    const int_t &type = as< int_t >(t);
    if (!type.is_signed)
        return false;

    switch (op) {
    case binary_op_t::add: return !holds(t, exact_t(a) + b);
    case binary_op_t::sub: return !holds(t, exact_t(a) - b);
    case binary_op_t::mul: return !holds(t, exact_t(a) * b);
    case binary_op_t::shl: {
        const auto count =
            static_cast< unsigned long long >(b) & unsigned(type.width - 1);

        return a >= 0 && (exact_t(a) << count) >> type.width != 0;
    }
    default: return false;
    }
}

// Whether an operand op evaluates was reached through a signed
// overflow, rcc's operand_overflowed(): the right one of && and || is
// not evaluated where the left decides, 6.5.14p4, 6.5.15p4.  a is the
// left operand's value.
bool
operand_overflowed(binary_op_t op,
                   const operand_t &lhs,
                   long long a,
                   const operand_t &rhs)
{
    const bool decided = (op == binary_op_t::log_and && a == 0) ||
                         (op == binary_op_t::log_or && a != 0);

    return lhs.overflowed || (!decided && rhs.overflowed);
}

} // namespace

bool
overflowed(binary_op_t op,
           const type_t &t,
           const operand_t &lhs,
           const expr_t &l,
           const operand_t &rhs,
           const expr_t &r)
{
    const std::optional< long long > a = constant_of(l);
    const std::optional< long long > b = constant_of(r);
    if (!a || !b)
        return false;

    return overflows(op, t, *a, *b) || operand_overflowed(op, lhs, *a, rhs);
}

// The negative of a signed constant overflows where it is the most
// negative value of its type, which the type does not hold.
bool
overflowed(unary_op_t op,
           const type_t &t,
           const operand_t &arg,
           const expr_t &a)
{
    const std::optional< long long > value = constant_of(a);
    if (!value)
        return false;

    const bool overflows = op == unary_op_t::neg && as< int_t >(t).is_signed &&
                           !holds(t, -exact_t(*value));

    return overflows || arg.overflowed;
}

void
warn_overflowed(parser_t &p, const token_t &at, const operand_t &e)
{
    if (e.overflowed && !at.where.system_header)
        warn(p, at,
             "signed overflow in a constant expression: the value wraps, "
             "as at run time");
}

} // namespace rcc
