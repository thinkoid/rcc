// -*- mode: c++; -*-

// The operators of the expressions, a closed domain, so a parameter of
// the node types as it stands (docs/typed-tree.md principle 6).  The
// parser holds an operator as a value and passes it to a builder,
// which makes it the node's parameter; the parser sees no node type,
// so the operators are a header of their own, which the handle's
// includes.  The names are rcc's.

#pragma once

namespace rcc {

// The unary operators of 6.5.4.3 that make a node: - and ~.
enum class unary_op_t { neg, bit_not };

// The binary operators of int, 6.5.6 to 6.5.8 and 6.5.11 to 6.5.13:
// * + - << >> & | ^.
enum class binary_op_t { add, sub, mul, shl, shr, bit_and, bit_or, bit_xor };

// Whether the operands of op may change places, its value the same
// either way, rcc's commutes(): + * & | ^, and not - << >>.
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
    case binary_op_t::shl:
    case binary_op_t::shr: return false;
    }
}

// Whether op is a shift, << or >>, rcc's is_shift(): its right operand
// is a count, which the builder and x86-64's rows read apart.
constexpr bool
is_shift(binary_op_t op)
{
    return op == binary_op_t::shl || op == binary_op_t::shr;
}

} // namespace rcc
