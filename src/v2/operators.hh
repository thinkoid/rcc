// -*- mode: c++; -*-

// The operators of the expressions, a closed domain, so a parameter of
// the node types as it stands (docs/typed-tree.md principle 6).  The
// parser holds an operator as a value and passes it to a builder,
// which makes it the node's parameter; the parser sees no node type,
// so the operators are a header of their own, which the handle's
// includes.  The names are rcc's.

#pragma once

namespace rcc {

// The unary operators of 6.5.4.3 that make a node: - ~ !.
enum class unary_op_t { neg, bit_not, log_not };

// The binary operators of int, 6.5.6 to 6.5.13: * / % + - << >> & | ^,
// and the comparisons < > <= >= == !=.
enum class binary_op_t {
    add,
    sub,
    mul,
    div,
    rem,
    shl,
    shr,
    lt,
    gt,
    le,
    ge,
    eq,
    ne,
    bit_and,
    bit_or,
    bit_xor
};

// Whether the operands of op may change places, its value the same
// either way, rcc's commutes(): + * & | ^, and not / % - << >>.
constexpr bool
commutes(binary_op_t op)
{
    switch (op) {
    case binary_op_t::add:
    case binary_op_t::mul:
    case binary_op_t::bit_and:
    case binary_op_t::bit_or:
    case binary_op_t::bit_xor: return true;
    case binary_op_t::sub:
    case binary_op_t::div:
    case binary_op_t::rem:
    case binary_op_t::shl:
    case binary_op_t::shr:
    case binary_op_t::lt:
    case binary_op_t::gt:
    case binary_op_t::le:
    case binary_op_t::ge:
    case binary_op_t::eq:
    case binary_op_t::ne: return false;
    }
}

// Whether op is a division, / or %, whose two constants fold only where
// the division has a value, and whose instruction pins two registers on
// x86-64.
constexpr bool
is_division(binary_op_t op)
{
    return op == binary_op_t::div || op == binary_op_t::rem;
}

// Whether op is a shift, << or >>, rcc's is_shift(): its right operand
// is a count, which the builder and x86-64's rows read apart.
constexpr bool
is_shift(binary_op_t op)
{
    return op == binary_op_t::shl || op == binary_op_t::shr;
}

// Whether op is a comparison, rcc's is_comparison(): its value is 1
// where the relation holds and 0 where it does not, 6.5.9p7, 6.5.10p4,
// of the flags its cmp sets.
constexpr bool
is_comparison(binary_op_t op)
{
    return op == binary_op_t::lt || op == binary_op_t::gt ||
           op == binary_op_t::le || op == binary_op_t::ge ||
           op == binary_op_t::eq || op == binary_op_t::ne;
}

// The comparisons, a family of operators whose rows both targets write
// apart from the arithmetic ones' (docs/typed-tree.md 3.2).
template< binary_op_t op >
concept comparison_operator = is_comparison(op);

// The comparison that holds of b and a exactly when op holds of a and
// b, rcc's mirrored(): 5 < x is x > 5, and == and != are themselves.
// The Seventh Edition compiler's maprel[].
constexpr binary_op_t
mirrored(binary_op_t op)
{
    switch (op) {
    case binary_op_t::lt: return binary_op_t::gt;
    case binary_op_t::gt: return binary_op_t::lt;
    case binary_op_t::le: return binary_op_t::ge;
    case binary_op_t::ge: return binary_op_t::le;
    default: return op;
    }
}

} // namespace rcc
