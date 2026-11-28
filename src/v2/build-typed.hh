// -*- mode: c++; -*-

// The typed builders, docs/typed-tree.md 3.3: the overloads on the
// node types that a builder of build.hh reaches through a model, where
// a part's type is complete.  The overload chosen makes the node, so a
// rule of C and a shape of the zoo are decided here: which node a
// place's value is, what an assignment stores into, in which class a
// value is kept, what an operator on constants folds to, which operator
// is the identity and leaves its operand, whether an if's then part is
// one jump.  A model finds a typed builder as it finds a row, by
// overload where it is instantiated, so build.cc includes this header
// beside the rows.

#pragma once

#include "build.hh"
#include "expr.hh"
#include "stmt.hh"
#include "zoo.hh"

#include <cstdint>
#include <optional>
#include <type_traits>

namespace rcc {

// A node that is no place is a value already, and the lvalue conversion
// leaves it the node it is: its own handle.
template< typename T >
expr_t
rvalue(const T &, const expr_t &self)
{
    return self;
}

// A place becomes the value its object holds, 6.3.2.1p2, read at its
// view by its object's sign, a local's slot and a global alike.
template< typename W, view_t V, sign_t S >
expr_t
rvalue(const place_t< W, V, S > &place, const expr_t &)
{
    return expr_t(load_t< W, V, S > { place.where });
}

// The value of a node the builders made a constant, value_of()'s (4.3),
// and none for any other node: the fold is the builders', so a constant
// expression is a constant node, as rcc's evaluator leaves a literal.
template< typename T >
std::optional< long long >
constant_of(const T &)
{
    return std::nullopt;
}

template< constant C >
std::optional< long long >
constant_of(const C &c)
{
    return value_of(c);
}

// An assignment's builder exists for a place alone, a local or a
// global, and so defines what is assignable (principle 8); the place's
// view is the store's.
template< typename W, view_t V, sign_t S >
expr_t
make_assign(const place_t< W, V, S > &place, const expr_t &value)
{
    return expr_t(assign_t< W, V > { place.where, value });
}

// A step's builder exists for a place alone, as an assignment's does
// (principle 8), and keeps the place's where, view and sign.
template< step_op_t op, typename W, view_t V, sign_t S >
expr_t
make_step(const place_t< W, V, S > &place)
{
    return expr_t(step_t< op, W, V, S > { place.where });
}

// The bits of an operation at the view V on constants, as rcc folds it,
// read as the register holds them: the low bits of V, signed (G1, G2),
// rcc's normalized(), where 6.5p5 leaves a signed overflow undefined.
// The arithmetic is on the bits, modulo 2^64, so it wraps as the
// instruction does.
template< view_t V >
long long
wrapped(unsigned long long bits)
{
    if constexpr (width(V) == 64)
        return static_cast< long long >(bits);
    else {
        const auto low = bits & ((1ull << width(V)) - 1);
        const auto sign = 1ull << (width(V) - 1);

        return static_cast< long long >(low ^ sign) -
               static_cast< long long >(sign);
    }
}

// The bits of a value at the view V read unsigned, the operand of an
// unsigned operation, 6.2.5p11, and a narrow unsigned constant's
// promotion.
template< view_t V >
unsigned long long
unsigned_bits(long long value)
{
    const auto bits = static_cast< unsigned long long >(value);

    if constexpr (width(V) == 64)
        return bits;
    else
        return bits & ((1ull << width(V)) - 1);
}

// - ~ ! of a constant at the view V, 6.5.4.3p3 to p5: the negative of
// the most negative value wraps to itself, and !c is 1 where c is 0 and
// 0 otherwise.
template< unary_op_t op, view_t V >
long long
folded(long long value)
{
    const auto bits = static_cast< unsigned long long >(value);

    switch (op) {
    case unary_op_t::neg: return wrapped< V >(0 - bits);
    case unary_op_t::bit_not: return wrapped< V >(~bits);
    case unary_op_t::log_not: return value == 0;
    }
}

// The view of the value of op at the view V: an int's for a comparison,
// ! and && and ||, 6.5.3.3p5, 6.5.9p7, 6.5.10p4, 6.5.14p3, 6.5.15p3,
// and the operation's otherwise.
constexpr view_t
result_view(unary_op_t op, view_t view)
{
    return op == unary_op_t::log_not ? view_t::i32 : view;
}

constexpr view_t
result_view(binary_op_t op, view_t view)
{
    return is_comparison(op) || is_logical(op) ? view_t::i32 : view;
}

// - ~ ! of anything but a constant: the node, its operand erased (3.3).
template< unary_op_t op, view_t V, typename A >
expr_t
make_unary(const A &, const expr_t &arg)
{
    return expr_t(unary_t< op, V > { arg });
}

// - ~ ! of a constant fold where the node would be built, as rcc folds
// them: a constant, which make_literal() classes.
template< unary_op_t op, view_t V, constant C >
expr_t
make_unary(const C &arg, const expr_t &)
{
    return make_literal(folded< op, V >(value_of(arg)), result_view(op, V));
}

// The count of a shift at the view V, as rcc's shift_count() takes it:
// the count modulo the width, as the instruction masks it (G3), where
// 6.5.8p3 leaves a count that is negative or not less than the width
// undefined.
template< view_t V >
long long
shift_count(long long count)
{
    return count & (width(V) - 1);
}

// Whether the division op of lhs by rhs at the view V has a value,
// rcc's divides(): not by zero, 6.5.6p6, and for a signed one not with a
// quotient the type cannot hold, p7, which is the one case of the most
// negative value divided by -1; the remainder of that division has no
// value either.  The instruction traps on both at run time (G4).
template< binary_op_t op, view_t V >
bool
divides(long long lhs, long long rhs)
{
    const bool is_signed = op == binary_op_t::sdiv || op == binary_op_t::srem;
    const long long most_negative = wrapped< V >(1ull << (width(V) - 1));

    return rhs != 0 && !(is_signed && lhs == most_negative && rhs == -1);
}

// * / % + - << >> & | ^ of two constants at the view V, 6.5.6p5 to p7,
// 6.5.7p6, p7, 6.5.8p4, p5, 6.5.11p4, 6.5.12p4, 6.5.13p4.  / and % on a
// division that divides() says has a value, by the operator's sign: a
// signed operator on the values, an unsigned one on the bits, modulo
// 2^N, 6.2.5p11.  The quotient truncates toward zero and the remainder
// has the dividend's sign.  << shifts the bits and wraps, and >> of a
// negative value is arithmetic, G3, the implementation's choice by
// 6.5.8p5, where an unsigned one is logical.  A comparison is 1 where
// its relation holds and 0 where it does not, 6.5.9p7, 6.5.10p4, and so
// are && and || of the operands compared with 0, 6.5.14p3, 6.5.15p3.
template< binary_op_t op, view_t V >
long long
folded(long long lhs, long long rhs)
{
    const auto a = static_cast< unsigned long long >(lhs);
    const auto b = static_cast< unsigned long long >(rhs);
    const auto ua = unsigned_bits< V >(lhs);
    const auto ub = unsigned_bits< V >(rhs);

    switch (op) {
    case binary_op_t::add: return wrapped< V >(a + b);
    case binary_op_t::sub: return wrapped< V >(a - b);
    case binary_op_t::mul: return wrapped< V >(a * b);
    case binary_op_t::sdiv: return lhs / rhs;
    case binary_op_t::udiv: return wrapped< V >(ua / ub);
    case binary_op_t::srem: return lhs % rhs;
    case binary_op_t::urem: return wrapped< V >(ua % ub);
    case binary_op_t::shl: return wrapped< V >(a << shift_count< V >(rhs));
    case binary_op_t::ashr: return lhs >> shift_count< V >(rhs);
    case binary_op_t::lshr: return wrapped< V >(ua >> shift_count< V >(rhs));
    case binary_op_t::slt: return lhs < rhs;
    case binary_op_t::sle: return lhs <= rhs;
    case binary_op_t::sgt: return lhs > rhs;
    case binary_op_t::sge: return lhs >= rhs;
    case binary_op_t::ult: return ua < ub;
    case binary_op_t::ule: return ua <= ub;
    case binary_op_t::ugt: return ua > ub;
    case binary_op_t::uge: return ua >= ub;
    case binary_op_t::eq: return lhs == rhs;
    case binary_op_t::ne: return lhs != rhs;
    case binary_op_t::bit_and: return lhs & rhs;
    case binary_op_t::bit_or: return lhs | rhs;
    case binary_op_t::bit_xor: return lhs ^ rhs;
    case binary_op_t::log_and: return lhs != 0 && rhs != 0;
    case binary_op_t::log_or: return lhs != 0 || rhs != 0;
    }
}

// Whether the constant c on the right of op at the view V makes op the
// identity, as rcc's is_identity_by() says: + - | ^ by 0, * / by 1, &
// by -1, every bit of V, << >> by a count that is 0 modulo V's width,
// so x << 32 is x on an int and no identity on a long; % by none, x % 1
// being 0, and a comparison, && and || by none, their value being 0 or
// 1, so x && 1 is the node, its constant loaded and tested.
template< binary_op_t op, view_t V >
bool
is_identity_by(long long c)
{
    switch (op) {
    case binary_op_t::add:
    case binary_op_t::sub:
    case binary_op_t::bit_or:
    case binary_op_t::bit_xor: return c == 0;
    case binary_op_t::mul:
    case binary_op_t::sdiv:
    case binary_op_t::udiv: return c == 1;
    case binary_op_t::srem:
    case binary_op_t::urem:
    case binary_op_t::slt:
    case binary_op_t::sle:
    case binary_op_t::sgt:
    case binary_op_t::sge:
    case binary_op_t::ult:
    case binary_op_t::ule:
    case binary_op_t::ugt:
    case binary_op_t::uge:
    case binary_op_t::eq:
    case binary_op_t::ne:
    case binary_op_t::log_and:
    case binary_op_t::log_or: return false;
    case binary_op_t::shl:
    case binary_op_t::ashr:
    case binary_op_t::lshr: return shift_count< V >(c) == 0;
    case binary_op_t::bit_and: return c == -1;
    }
}

// The node an operand is, out of the class a builder kept it in: a
// top_t holds its node, and any other class is a node of its own.
template< typename T >
expr_t
node_of(const T &operand)
{
    return expr_t(operand);
}

template< family_t F >
expr_t
node_of(const top_t< F > &operand)
{
    return operand.node;
}

// A binary operator of operands neither of which is a constant: the
// node, each operand in its class.
template< binary_op_t op, view_t V, typename L, typename R >
expr_t
make_binary(const L &lhs, const R &rhs)
{
    return expr_t(binary_t< op, V, L, R > { lhs, rhs });
}

// A constant on the right that makes the operator the identity, x + 0,
// leaves the other operand, the node it is, its conversions with it;
// rcc drops such a node in a pass after the body is parsed,
// drop_identities(), and rcc-v2 where it would be built.  Any other
// constant is the node's.
template< binary_op_t op, view_t V, typename L, constant R >
expr_t
make_binary(const L &lhs, const R &rhs)
{
    if (is_identity_by< op, V >(value_of(rhs)))
        return expr_t(lhs);

    return expr_t(binary_t< op, V, L, R > { lhs, rhs });
}

// A constant on the left makes the operator the identity as one on the
// right does, but for an operator that commutes alone: 0 + x is x, and
// 0 - x, 0 << x and 1 / x are the node.
template< binary_op_t op, view_t V, constant L, typename R >
expr_t
make_binary(const L &lhs, const R &rhs)
{
    if (commutes(op) && is_identity_by< op, V >(value_of(lhs)))
        return node_of(rhs);

    return expr_t(binary_t< op, V, L, R > { lhs, rhs });
}

// Two constants fold where the node would be built, as rcc folds them:
// a constant, whatever the operator, but for a division that has no
// value, divides(), which is the node with its two constants, its
// instruction written: 1 / 0 is movl $1, %eax, movl $0, %ecx, cltd and
// idivl %ecx.
template< binary_op_t op, view_t V, constant L, constant R >
expr_t
make_binary(const L &lhs, const R &rhs)
{
    if constexpr (is_division(op)) {
        if (!divides< op, V >(value_of(lhs), value_of(rhs)))
            return expr_t(binary_t< op, V, L, R > { lhs, rhs });
    }

    return make_literal(folded< op, V >(value_of(lhs), value_of(rhs)),
                        result_view(op, V));
}

// An operand of a conversion as its builder keeps it: erased, but a
// top_t of its family where its top operator, read through its own
// conversions, pins registers on x86-64, a division or a shift by a
// count that is no constant.  rcc's pinned_by() reads a right operand
// through its casts, which convert in the operand's register and pin
// none, so the class of a conversion is its operand's: a + (long) (b /
// c) holds a out of rax and rdx while it generates the quotient.
template< typename T >
const expr_t &
pinning(const T &, const expr_t &self)
{
    return self;
}

template< binary_op_t op, view_t V, typename L, typename R >
auto
pinning(const binary_t< op, V, L, R > &, const expr_t &self)
{
    if constexpr (is_division(op))
        return top_t< family_t::division > { self };
    else if constexpr (is_shift(op) && !constant< R >)
        return top_t< family_t::shift > { self };
    else
        return self;
}

// A top_t is the class already, which a conversion's model hands its
// builder, converted_operand().
template< family_t P >
const top_t< P > &
pinning(const top_t< P > &operand, const expr_t &)
{
    return operand;
}

template< view_t V, view_t F, family_t P >
top_t< P >
pinning(const sext_t< V, F, top_t< P > > &, const expr_t &self)
{
    return { self };
}

template< view_t V, view_t F, family_t P >
top_t< P >
pinning(const zext_t< V, F, top_t< P > > &, const expr_t &self)
{
    return { self };
}

template< view_t V, family_t P >
top_t< P >
pinning(const trunc_t< V, top_t< P > > &, const expr_t &self)
{
    return { self };
}

// The node of a widening of the operand A of view F to V by the sign S
// of the type converted from, LLVM's names: sext_t of a signed one,
// zext_t of an unsigned one.
template< view_t V, view_t F, sign_t S, typename A >
expr_t
widening_of(const A &operand)
{
    if constexpr (S == sign_t::signed_)
        return expr_t(sext_t< V, F, A > { operand });
    else
        return expr_t(zext_t< V, F, A > { operand });
}

// The widening of an operand of view F to V by the sign S of its type,
// rcc's converted() for a widening, the object in memory found where
// rcc's widened_read() finds it.  Any operand: its node over the
// operand in the class pinning() keeps, whose row extends where the
// width rule says.
template< view_t V, view_t F, sign_t S, typename A >
expr_t
make_widening(const A &arg, const expr_t &self)
{
    const auto operand = pinning(arg, self);

    return widening_of< V, F, S >(operand);
}

// A load is read at the type converted from, the conversion over a load
// (docs/typed-tree.md 4.2), whose niche is the extending load of a
// signed one.
template< view_t V, view_t F, sign_t S, typename W, sign_t O >
expr_t
make_widening(const load_t< W, F, O > &l, const expr_t &)
{
    return widening_of< V, F, S >(load_t< W, F, S > { l.where });
}

// A change of sign or a cast to its own type over a load keeps the
// object's bits, so the widening reads the object at the type it
// converts from, rcc's object_under() under under_kept_bits(): (long)
// (int) u on an unsigned u is movslq -4(%rbp), %rax, and (int)
// (unsigned char) c on a signed char c movzbl -1(%rbp), %eax.
template< view_t V, view_t F, sign_t S, typename W, sign_t O >
expr_t
make_widening(const trunc_t< F, load_t< W, F, O > > &t, const expr_t &)
{
    return widening_of< V, F, S >(load_t< W, F, S > { t.arg.where });
}

// A widening over a promotion of the object, rcc's widened_read(),
// keeps the promotion as its operand's type, so the one load that is
// the two extensions is that type's row (rows-expressions.hh): a signed
// object under a signed widening, (long) (int) c on a signed char c
// being the one movsbq -1(%rbp), %rax; and any unsigned one, whose zero
// extension the promotion keeps, (long) (int) c on an unsigned char the
// one movzbl -1(%rbp), %eax.  A widening over that is over a value, no
// promotion of the object, and extends it by the width rule, as rcc's
// does: (long) (int) (short) c is movsbl -1(%rbp), %eax and movslq
// %eax, %rax.  A signed object under an unsigned widening is no such
// case, its extension to 32 bits no extension to 64: (long) (unsigned)
// c is the widening of the promotion, which writes nothing, since the
// extending load is valid to 64 as an unsigned.
template< view_t V, view_t F, sign_t S, view_t G, typename W >
expr_t
make_widening(const sext_t< F, G, load_t< W, G, sign_t::signed_ > > &e,
              const expr_t &self)
{
    if constexpr (S == sign_t::signed_)
        return widening_of< V, F, S >(e);
    else
        return widening_of< V, F, S >(pinning(e, self));
}

template< view_t V, view_t F, sign_t S, view_t G, typename W >
expr_t
make_widening(const zext_t< F, G, load_t< W, G, sign_t::unsigned_ > > &e,
              const expr_t &)
{
    return widening_of< V, F, S >(e);
}

// A conversion that is no widening, a narrowing, a change of sign or a
// cast to the operand's own type: trunc_t, which writes nothing, over
// the operand in the class pinning() keeps.
template< view_t V, typename A >
expr_t
make_narrowing(const A &arg, const expr_t &self)
{
    const auto operand = pinning(arg, self);

    return expr_t(trunc_t< V, std::decay_t< decltype(operand) > > { operand });
}

// Over a load, trunc_t keeps the load as its class, the object a parent
// may name in place.
template< view_t V, typename W, view_t F, sign_t S >
expr_t
make_narrowing(const load_t< W, F, S > &l, const expr_t &)
{
    return expr_t(trunc_t< V, load_t< W, F, S > > { l });
}

// Two conversions that write nothing are one, rcc's under_kept_bits()
// over any number of them: (int) (long) l is the one trunc_t over the
// load.
template< view_t V, view_t W, typename B >
expr_t
make_narrowing(const trunc_t< W, B > &t, const expr_t &)
{
    return expr_t(trunc_t< V, B > { t.arg });
}

// The value of a constant of view F and of a type of the sign S
// converted to the view V of a type of the sign to_sign, rcc's
// converted_constant() and wrapped(), 6.3.1.3: the exact value, its bits
// at F extended by S, so that an unsigned -1 widened is 4294967295; then
// its low bits at V, read as the register holds them: zero-extended for
// a narrow unsigned type, as its promotion reads them, and signed
// otherwise.  So (unsigned char) 300 is 44, (char) 200 is -56 where
// plain char is signed and 200 where it is unsigned, and (unsigned
// short) (signed char) 128 is 65408.
template< view_t V, view_t F, sign_t S >
long long
converted(long long value, sign_t to_sign)
{
    const long long exact =
        S == sign_t::signed_ ?
            value :
            static_cast< long long >(unsigned_bits< F >(value));

    if (width(V) < 32 && to_sign == sign_t::unsigned_)
        return static_cast< long long >(unsigned_bits< V >(exact));

    return wrapped< V >(static_cast< unsigned long long >(exact));
}

// The conversion of an operand of view F and of a type of the sign S to
// the view V of a type of the sign to_sign, 6.3.1.3: a constant folds to
// the constant of the new type, since a conversion hides its operand
// from the parent's niches; anything else is a widening where V is the
// wider, and otherwise a conversion that writes nothing.
template< view_t V, view_t F, sign_t S, typename T >
expr_t
make_conversion(sign_t to_sign, const T &e, const expr_t &self)
{
    if constexpr (constant< T >)
        return make_literal(converted< V, F, S >(value_of(e), to_sign), V);
    else if constexpr (width(V) > width(F))
        return make_widening< V, F, S >(e, self);
    else
        return make_narrowing< V >(e, self);
}

// A comma of the left operand lhs and the right operand in its class:
// the node, which nothing folds, 6.6p3, and which drops no identity, a
// + (b, 0) adding 0 as rcc's does.
template< typename R >
expr_t
make_comma(const expr_t &lhs, const R &rhs, const expr_t &)
{
    return expr_t(comma_t< R > { lhs, rhs });
}

// A comma on the right is taken apart, (x, (y, c)) built as ((x, y), c),
// which keeps c's class: its effect is x's then y's, as rcc's
// operand_of() recurses, and every entry point writes the same text in
// either shape.
template< typename R >
expr_t
make_comma(const expr_t &lhs, const comma_t< R > &rhs, const expr_t &)
{
    return expr_t(comma_t< R > { make_comma(lhs, rhs.lhs), rhs.rhs });
}

// An if of the then part then, a node's type S: with the else part
// otherwise where there is one.
template< typename S >
stmt_t
if_node(const expr_t &cond,
        const S &then,
        const std::optional< stmt_t > &otherwise)
{
    if (otherwise)
        return stmt_t(if_else_t< S > { cond, then, *otherwise });

    return stmt_t(if_t< S > { cond, then });
}

// An if whose then part is any statement but one jump: the node with
// the then part erased, the handle that holds it.
template< typename T >
stmt_t
make_if(const expr_t &cond,
        const T &,
        const stmt_t &then,
        const std::optional< stmt_t > &otherwise)
{
    return if_node(cond, then, otherwise);
}

// A then part that is one jump, one_jump's, is the node's own type,
// docs/backend.md 7.2: the condition's jump to its label, as the first
// pass of the Seventh Edition compiler writes it, c02.c.
template< one_jump J >
stmt_t
make_if(const expr_t &cond,
        const J &then,
        const stmt_t &,
        const std::optional< stmt_t > &otherwise)
{
    return if_node(cond, then, otherwise);
}

// The second hop of an if's builder, build.cc's: through the model of
// item, the one item of the block that is the then part.
stmt_t if_on_item(const stmt_t &item,
                  const expr_t &cond,
                  const std::optional< stmt_t > &otherwise,
                  const stmt_t &block);

// A block of one item is one jump where its item is one, the braces
// writing nothing: the answer is the item's, asked through its model,
// if_on_item(), and a block of any other count is no jump.
inline stmt_t
make_if(const expr_t &cond,
        const block_t &b,
        const stmt_t &then,
        const std::optional< stmt_t > &otherwise)
{
    if (b.items.size() != 1)
        return if_node(cond, then, otherwise);

    return if_on_item(b.items.front(), cond, otherwise, then);
}

// The one item of a block that is a then part, asked whether it is one
// jump: a type of one_jump is, and any other statement is not, a block
// among them, so { { break; } } is no jump and the then part is the
// block.
template< typename T >
stmt_t
make_if_of_item(const expr_t &cond,
                const T &,
                const stmt_t &block,
                const std::optional< stmt_t > &otherwise)
{
    return if_node(cond, block, otherwise);
}

template< one_jump J >
stmt_t
make_if_of_item(const expr_t &cond,
                const J &item,
                const stmt_t &,
                const std::optional< stmt_t > &otherwise)
{
    return if_node(cond, item, otherwise);
}

} // namespace rcc
