// -*- mode: c++; -*-

// The closed domains of the expressions, each a parameter of the node
// types as it stands (docs/typed-tree.md principle 6): the views, the
// operators and the families of operator a class keeps.  The parser
// holds a value of each and passes it to a builder, which makes it the
// node's parameter by the domain's crossing, below; the parser sees no
// node type, so the domains are a header of their own, which the
// handle's includes.  The names are LLVM's where an instruction
// differs by the sign, and rcc's otherwise (principle 12).

#pragma once

namespace rcc {

// How a value is read, the width alone, LLVM's names (4.2): an int's 32
// bits, a long's 64.  No view has a sign; the operators that read one
// say it in their names.
enum class view_t { i32, i64 };

// The width of a view in bits.
constexpr int
width(view_t view)
{
    switch (view) {
    case view_t::i32: return 32;
    case view_t::i64: return 64;
    }
}

// The unary operators of 6.5.4.3 that make a node: - ~ !.
enum class unary_op_t { neg, bit_not, log_not };

// The binary operators, 6.5.6 to 6.5.15: * / % + - << >> & | ^, the
// comparisons < > <= >= == !=, and && and ||.  An operator whose
// instruction differs by the sign of its operands has the sign in its
// name, LLVM's: sdiv for /, srem for %, ashr for >>, slt sle sgt sge for
// < <= > >=, the signed ones, which the parser picks by the type of the
// operation.
enum class binary_op_t {
    add,
    sub,
    mul,
    sdiv,
    srem,
    shl,
    ashr,
    slt,
    sle,
    sgt,
    sge,
    eq,
    ne,
    bit_and,
    bit_or,
    bit_xor,
    log_and,
    log_or
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
    case binary_op_t::sdiv:
    case binary_op_t::srem:
    case binary_op_t::shl:
    case binary_op_t::ashr:
    case binary_op_t::slt:
    case binary_op_t::sle:
    case binary_op_t::sgt:
    case binary_op_t::sge:
    case binary_op_t::eq:
    case binary_op_t::ne:
    case binary_op_t::log_and:
    case binary_op_t::log_or: return false;
    }
}

// Whether op is a division, / or %, whose two constants fold only where
// the division has a value, and whose instruction pins two registers on
// x86-64.
constexpr bool
is_division(binary_op_t op)
{
    return op == binary_op_t::sdiv || op == binary_op_t::srem;
}

// Whether op is a shift, << or >>, rcc's is_shift(): its right operand
// is a count of its own type, which the builder and x86-64's rows read
// apart.
constexpr bool
is_shift(binary_op_t op)
{
    return op == binary_op_t::shl || op == binary_op_t::ashr;
}

// Whether op is a comparison, rcc's is_comparison(): its value is 1
// where the relation holds and 0 where it does not, 6.5.9p7, 6.5.10p4,
// of the flags its cmp sets.
constexpr bool
is_comparison(binary_op_t op)
{
    return op == binary_op_t::slt || op == binary_op_t::sle ||
           op == binary_op_t::sgt || op == binary_op_t::sge ||
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
    case binary_op_t::slt: return binary_op_t::sgt;
    case binary_op_t::sgt: return binary_op_t::slt;
    case binary_op_t::sle: return binary_op_t::sge;
    case binary_op_t::sge: return binary_op_t::sle;
    default: return op;
    }
}

// The comparison that holds exactly when op does not, rcc's inverse():
// a comparison's jump on its false sense.
constexpr binary_op_t
inverse(binary_op_t op)
{
    switch (op) {
    case binary_op_t::slt: return binary_op_t::sge;
    case binary_op_t::sge: return binary_op_t::slt;
    case binary_op_t::sgt: return binary_op_t::sle;
    case binary_op_t::sle: return binary_op_t::sgt;
    case binary_op_t::eq: return binary_op_t::ne;
    default: return binary_op_t::eq;
    }
}

// Whether op is && or ||, whose right operand is evaluated only where
// the left one leaves the value open, 6.5.14p4, 6.5.15p4: a jump inside
// the expression.
constexpr bool
is_logical(binary_op_t op)
{
    return op == binary_op_t::log_and || op == binary_op_t::log_or;
}

// && and ||, a family whose rows both targets write alike, of jumps
// (docs/typed-tree.md 3.2).
template< binary_op_t op >
concept logical_operator = is_logical(op);

// The family of a right operand's top operator that a class keeps of
// it, top_t's: a division or a shift by a count that is no constant,
// whose instruction pins registers on x86-64 whatever its sign
// (docs/typed-tree.md 3.3).
enum class family_t { division, shift };

// The crossings, principle 6: a switch over a domain that calls f with
// the value as its template argument, once, where the builder makes
// the node's parameter.  They compose, so the product of the domains
// is instantiated and never written.
template< typename F >
decltype(auto)
cross(view_t view, F &&f)
{
    switch (view) {
    case view_t::i32: return f.template operator()< view_t::i32 >();
    case view_t::i64: return f.template operator()< view_t::i64 >();
    }
}

template< typename F >
decltype(auto)
cross(unary_op_t op, F &&f)
{
    switch (op) {
    case unary_op_t::neg: return f.template operator()< unary_op_t::neg >();
    case unary_op_t::bit_not:
        return f.template operator()< unary_op_t::bit_not >();
    case unary_op_t::log_not:
        return f.template operator()< unary_op_t::log_not >();
    }
}

template< typename F >
decltype(auto)
cross(binary_op_t op, F &&f)
{
    using enum binary_op_t;

    switch (op) {
    case add: return f.template operator()< add >();
    case sub: return f.template operator()< sub >();
    case mul: return f.template operator()< mul >();
    case sdiv: return f.template operator()< sdiv >();
    case srem: return f.template operator()< srem >();
    case shl: return f.template operator()< shl >();
    case ashr: return f.template operator()< ashr >();
    case slt: return f.template operator()< slt >();
    case sle: return f.template operator()< sle >();
    case sgt: return f.template operator()< sgt >();
    case sge: return f.template operator()< sge >();
    case eq: return f.template operator()< eq >();
    case ne: return f.template operator()< ne >();
    case bit_and: return f.template operator()< bit_and >();
    case bit_or: return f.template operator()< bit_or >();
    case bit_xor: return f.template operator()< bit_xor >();
    case log_and: return f.template operator()< log_and >();
    case log_or: return f.template operator()< log_or >();
    }
}

} // namespace rcc
