// -*- mode: c++; -*-

// The handles' interfaces and models, docs/typed-tree.md 3.1: the
// operations of each handle, one for each target in a line of its own
// (4.7), and the model that holds a node and finds its row by overload
// where the model is instantiated.  A node type with no row on a
// target does not build.  An expression's interface holds the builders
// that need the held node's type too, the hops of 3.3, which name no
// target; each finds its overload in build.cc as a row is found.
// build.cc alone includes this header, beside every row of both
// targets, so every model is instantiated in one unit and no two units
// see different rows.

#pragma once

#include "expr.hh"
#include "register-stack.hh"
#include "stmt.hh"
#include "zoo.hh"

#include <concepts>
#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace rcc {

// A constant on the right of a binary operator as it crosses the
// interface, the class as data (docs/typed-tree.md 3.3): its value and
// its view, which the left operand's model makes a constant again,
// typed(): of the operation's view, since the parser converted it to
// the type of the operation, but for an operand of && or ||, which
// keeps its own.  A shift's constant count is made one of the
// operation's view too, since its rows read its low bits alone.
struct constant_class_t
{
    long long value;
    view_t view;
};

// A load on the right as it crosses: its where, a local's slot or a
// global, and the sign of the object's type, which the left operand's
// model makes a load of the operation's view again.
template< typename W >
struct load_class_t
{
    W where;
    sign_t sign;
};

struct expr_t::interface_t
{
    virtual ~interface_t() = default;

    virtual value_t value(amd64::state_t &, unsigned wanted) const = 0;
    virtual value_t value(arm64::state_t &, unsigned wanted) const = 0;
    virtual void effect(amd64::state_t &) const = 0;
    virtual void effect(arm64::state_t &) const = 0;
    virtual void initialize(amd64::state_t &,
                            const amd64::address_t &) const = 0;
    virtual void initialize(arm64::state_t &,
                            const arm64::address_t &) const = 0;
    virtual void branch(amd64::state_t &, int label, bool sense) const = 0;
    virtual void branch(arm64::state_t &, int label, bool sense) const = 0;

    // x86-64's jump on the value compared with 0, e == 0's on e
    virtual void
    value_branch(amd64::state_t &, int label, bool sense) const = 0;

    // the summaries of the subtree, x86-64's (principle 3)
    virtual unsigned pinned_within(const amd64::state_t &) const = 0;
    virtual unsigned moved_within(const amd64::state_t &) const = 0;

    // the summaries both targets read (principle 3): the width rule, at
    // a sign, and the view
    virtual int valid(sign_t) const = 0;
    virtual view_t view() const = 0;

    // the builders: self is the handle that holds this node, which a
    // builder returns, or erases a part into, without a copy of the node
    virtual expr_t rvalue(const expr_t &self) const = 0;
    virtual std::optional< long long > constant_of() const = 0;
    virtual expr_t unary(unary_op_t op, view_t, const expr_t &self) const = 0;

    // a conversion, by the operand's model, which knows the operand's
    // type and so whether it is a load (rcc's widened_read())
    virtual expr_t conversion(view_t to,
                              sign_t to_sign,
                              view_t from,
                              sign_t from_sign,
                              const expr_t &self) const = 0;

    // an assignment: the place's model, which knows the place's type,
    // makes the node
    virtual bool is_assignable() const = 0;
    virtual expr_t assign(const expr_t &value) const = 0;

    // a step and a compound assignment: the place's model, as for an
    // assignment, makes the node
    virtual expr_t step(step_op_t op) const = 0;
    virtual expr_t compound(binary_op_t op, const expr_t &value) const = 0;

    // a binary operator, by the same two hops: the right operand's
    // model, which knows the right operand's class, calls the left's
    // operation for that class, which knows the left's class and makes
    // the node; the view is the operation's, at which the left's model
    // makes the class a type again
    virtual expr_t right_of(binary_op_t op,
                            view_t,
                            const expr_t &lhs,
                            const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const constant_class_t &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const load_class_t< slot_t > &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const load_class_t< global_t > &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const top_t< family_t::division > &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const top_t< family_t::shift > &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const expr_t &rhs,
                          const expr_t &self) const = 0;

    // a comma on the right, its class as data: its left operand erased
    // and its right one's class, as a right operand of its own crosses
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const comma_t< constant_class_t > &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const comma_t< load_class_t< slot_t > > &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const comma_t< load_class_t< global_t > > &rhs,
                          const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const comma_t<> &rhs,
                          const expr_t &self) const = 0;

    // a comma: the right operand's model, which knows the right
    // operand's class, makes the node
    virtual expr_t comma(const expr_t &lhs, const expr_t &self) const = 0;
};

// The rows of a held node, by overload and by the state's namespace: a
// member named value, effect, initialize, branch, value_branch,
// pinned_within or moved_within would hide them.  A value's row is
// handed the target's hint, the registers the consumer names.
template< typename S, typename T >
value_t
fwd_value(S &s, const T &t, unsigned wanted)
{
    return value(s, t, wanted);
}

template< typename S, typename T >
void
fwd_effect(S &s, const T &t)
{
    effect(s, t);
}

template< typename S, typename T >
void
fwd_branch(S &s, const T &t, int label, bool sense)
{
    branch(s, t, label, sense);
}

template< typename S, typename T >
void
fwd_value_branch(S &s, const T &t, int label, bool sense)
{
    value_branch(s, t, label, sense);
}

template< typename S, typename A, typename T >
void
fwd_initialize(S &s, const A &at, const T &t)
{
    initialize(s, at, t);
}

template< typename S, typename T >
unsigned
fwd_pinned_within(const S &s, const T &t)
{
    return pinned_within(s, t);
}

template< typename S, typename T >
unsigned
fwd_moved_within(const S &s, const T &t)
{
    return moved_within(s, t);
}

// The lvalue conversion of a held node, as fwd_value() finds a row.
template< typename T >
expr_t
fwd_rvalue(const T &t, const expr_t &self)
{
    return rvalue(t, self);
}

// The value of a held node where it is a constant, as fwd_rvalue()
// finds the lvalue conversion.
template< typename T >
std::optional< long long >
fwd_constant_of(const T &t)
{
    return constant_of(t);
}

// The operand of a unary operator in the class its builder reads,
// make_unary(): a constant, which folds, or anything else erased, the
// handle that holds it.  The crossings of fwd_unary() are instantiated
// once for each class, and not for each node type.
template< typename T >
const expr_t &
unary_operand(const T &, const expr_t &self)
{
    return self;
}

template< constant C >
const C &
unary_operand(const C &c, const expr_t &)
{
    return c;
}

// A unary operator on a held node in its class: the crossing from the
// operator and the view, values in the parser, to the node's
// parameters (principle 6), the typed builder of each.  The view of -
// and ~ is an operation's, of the promoted operand, and that of ! the
// operand's own, which may be narrow.
template< typename A >
expr_t
fwd_unary(unary_op_t op, view_t view, const A &arg, const expr_t &self)
{
    return cross(op, [&]< unary_op_t o >() {
        const auto make = [&]< view_t V >() {
            return make_unary< o, V >(arg, self);
        };

        if constexpr (o == unary_op_t::log_not)
            return cross(view, make);
        else
            return cross_operation(view, make);
    });
}

// The operand of a conversion in the class its builders read
// (build-typed.hh): itself where they read its type, a constant, which
// folds, a load, and a conversion over a load or that writes nothing,
// which a conversion reads through; anything else as pinning() keeps
// it, a top_t where its top operator pins registers on x86-64, and
// erased otherwise.  The three crossings of fwd_conversion() are
// instantiated once for each class, and not for each node type.
template< typename T >
decltype(auto)
converted_operand(const T &e, const expr_t &self)
{
    return pinning(e, self);
}

template< constant C >
const C &
converted_operand(const C &c, const expr_t &)
{
    return c;
}

template< typename W, view_t V, sign_t S >
const load_t< W, V, S > &
converted_operand(const load_t< W, V, S > &l, const expr_t &)
{
    return l;
}

template< view_t V, typename A >
const trunc_t< V, A > &
converted_operand(const trunc_t< V, A > &t, const expr_t &)
{
    return t;
}

template< view_t V, view_t F, typename W >
const sext_t< V, F, load_t< W, F, sign_t::signed_ > > &
converted_operand(const sext_t< V, F, load_t< W, F, sign_t::signed_ > > &e,
                  const expr_t &)
{
    return e;
}

template< view_t V, view_t F, typename W >
const zext_t< V, F, load_t< W, F, sign_t::unsigned_ > > &
converted_operand(const zext_t< V, F, load_t< W, F, sign_t::unsigned_ > > &e,
                  const expr_t &)
{
    return e;
}

// The conversion of a held node in its class from the view from, at
// the sign of its type, to the view to at the sign to_sign: the three
// crossings, then the typed builder, by overload on the class.  The
// sign of the type converted to is read by a constant's conversion
// alone, as data.
template< typename T >
expr_t
fwd_conversion(view_t to,
               sign_t to_sign,
               view_t from,
               sign_t from_sign,
               const T &e,
               const expr_t &self)
{
    return cross(to, [&]< view_t V >() {
        return cross(from, [&]< view_t F >() {
            return cross(from_sign, [&]< sign_t S >() {
                return make_conversion< V, F, S >(to_sign, e, self);
            });
        });
    });
}

// A constant of view V by its value, f handed the node: the one
// question asked of a constant's value (principle 2), whose answer is
// its type.  0 is a zero_t, a 64-bit value past int's range a wide_t,
// and any other a literal_t.
template< view_t V, typename F >
decltype(auto)
classed_constant(long long value, F &&f)
{
    if (value == 0)
        return f(zero_t< V > { });

    if constexpr (V == view_t::i64) {
        if (value < INT32_MIN || value > INT32_MAX)
            return f(wide_t { value });
    }

    return f(literal_t< V > { value });
}

// An operand of a binary operator as a builder keeps it on the left, in
// the classes a target has a niche for: a constant and a load of any
// view, since the operands of && and || keep their own types, and a
// trunc_t of a load's own view, which keeps the object's bits, as its
// load, rcc's memory_operand() under under_kept_bits(); anything else
// erased, the handle that holds it.
template< typename T >
const expr_t &
classed_operand(const T &, const expr_t &self)
{
    return self;
}

template< constant C >
const C &
classed_operand(const C &c, const expr_t &)
{
    return c;
}

template< typename W, view_t V, sign_t S >
const load_t< W, V, S > &
classed_operand(const load_t< W, V, S > &l, const expr_t &)
{
    return l;
}

template< view_t V, typename W, sign_t S >
const load_t< W, V, S > &
classed_operand(const trunc_t< V, load_t< W, V, S > > &t, const expr_t &)
{
    return t.arg;
}

// The right operand of a binary operator as it crosses the interface,
// its class as data: a constant's value and a load's where, a load
// under a trunc_t of its own view among them; a top_t for a node whose
// top operator, read through its conversions, x86-64 generates the left
// operand away from, a division and a shift by a count that is no
// constant, pinning()'s; anything else erased.
template< typename T >
decltype(auto)
crossing(const T &e, const expr_t &self)
{
    return pinning(e, self);
}

template< constant C >
constant_class_t
crossing(const C &c, const expr_t &)
{
    return { value_of(c), view_of(c) };
}

template< typename W, view_t V, sign_t S >
load_class_t< W >
crossing(const load_t< W, V, S > &l, const expr_t &)
{
    return { l.where, S };
}

template< view_t V, typename W, sign_t S >
load_class_t< W >
crossing(const trunc_t< V, load_t< W, V, S > > &t, const expr_t &)
{
    return { t.arg.where, S };
}

// A comma crosses as a class of its own, its left operand erased and
// its right one's class as data, since x86-64's rows read a comma on
// their right through to that class, rcc's operand_of(): a constant's
// value, a load's where, a load under a trunc_t of its own view among
// them, or the right operand erased.
template< constant C >
comma_t< constant_class_t >
crossing(const comma_t< C > &c, const expr_t &)
{
    return { c.lhs, { value_of(c.rhs), view_of(c.rhs) } };
}

template< typename W, view_t V, sign_t S >
comma_t< load_class_t< W > >
crossing(const comma_t< load_t< W, V, S > > &c, const expr_t &)
{
    return { c.lhs, { c.rhs.where, S } };
}

template< view_t V, typename W, sign_t S >
comma_t< load_class_t< W > >
crossing(const comma_t< trunc_t< V, load_t< W, V, S > > > &c, const expr_t &)
{
    return { c.lhs, { c.rhs.arg.where, S } };
}

inline const comma_t<> &
crossing(const comma_t<> &c, const expr_t &)
{
    return c;
}

// Whether a right operand of a class crosses erased all the same, a
// question of the operator, data here, asked once (3.3): a shift's count
// and an operand of && or || keep their own types, so their load is no
// operand of the operation's view.  A constant crosses as one, so that
// two constants fold.
template< typename W >
constexpr bool
crosses_erased(binary_op_t op, const load_class_t< W > &)
{
    return is_shift(op) || is_logical(op);
}

template< typename C >
constexpr bool
crosses_erased(binary_op_t, const C &)
{
    return false;
}

// A comma of any class crosses erased to a comparison and to && and ||,
// which read no comma through, as rcc's comparisons take no operand of
// binary_operand().
template< typename C >
constexpr bool
crosses_erased(binary_op_t op, const comma_t< C > &)
{
    return is_comparison(op) || is_logical(op);
}

// Whether a class is a comma's of a load, of either where.
template< typename C >
constexpr bool load_comma = false;

template< typename W >
constexpr bool load_comma< comma_t< load_class_t< W > > > = true;

// Whether a node crosses as a comma of a load, crossing()'s, which as a
// shift's count crosses with its right operand erased: a count keeps
// its own type, and what a shift reads of it is whether it is a
// constant, as a load as a count crosses erased.
template< typename T >
constexpr bool comma_of_load = load_comma< std::decay_t< decltype(crossing(
    std::declval< const T & >(), std::declval< const expr_t & >())) > >;

// The right operand's class made a type again for op at the
// operation's view V, and handed to f: a constant by its value, at V,
// or at its own view for && and ||; a load at V; a top_t and an erased
// operand as they are.
template< binary_op_t op, view_t V, typename F >
expr_t
typed(const constant_class_t &c, F &&f)
{
    if constexpr (is_logical(op))
        return cross(c.view, [&]< view_t W >() {
            return classed_constant< W >(c.value, f);
        });
    else
        return classed_constant< V >(c.value, f);
}

template< binary_op_t op, view_t V, typename W, typename F >
expr_t
typed(const load_class_t< W > &l, F &&f)
{
    return cross(
        l.sign, [&]< sign_t S >() { return f(load_t< W, V, S > { l.where }); });
}

// A comma's class made a type again as its right operand's is, the
// comma's left operand as it is; but a shift's count keeps its own view,
// since the comma's value is no immediate on aarch64, where the count
// is written in a register at its width, mov x1, #63.
template< binary_op_t op, view_t V, typename F >
expr_t
typed(const comma_t< constant_class_t > &c, F &&f)
{
    const auto comma = [&](const auto &k) {
        return f(comma_t< std::decay_t< decltype(k) > > { c.lhs, k });
    };

    if constexpr (is_shift(op))
        return cross_operation(c.rhs.view, [&]< view_t W >() {
            return classed_constant< W >(c.rhs.value, comma);
        });
    else
        return classed_constant< V >(c.rhs.value, comma);
}

template< binary_op_t op, view_t V, typename W, typename F >
expr_t
typed(const comma_t< load_class_t< W > > &c, F &&f)
{
    return cross(c.rhs.sign, [&]< sign_t S >() {
        return f(comma_t< load_t< W, V, S > > { c.lhs, { c.rhs.where } });
    });
}

template< binary_op_t op, view_t V, typename R, typename F >
expr_t
typed(const R &rhs, F &&f)
{
    return f(rhs);
}

// Whether the class C of a left operand is one of an operation op at
// the view V: an erased one is, and a constant or a load of V, since
// the parser converts every operand of an operation to its type, but
// for && and ||, whose operands keep their own types, so that any is.
template< binary_op_t op, view_t V, typename C >
constexpr bool
operand_at()
{
    if constexpr (is_logical(op) || std::same_as< C, expr_t >)
        return true;
    else
        return view_of(C { }) == V;
}

// Whether the class R of a right operand is one of op: any is, but a
// comma's, which reaches only the operators whose rows on x86-64 read
// it, and of a shift a comma of a constant or of an erased operand
// alone, crosses_erased() and comma_of_load's.
template< binary_op_t op, typename R >
constexpr bool right_at = true;

template< binary_op_t op, typename R >
constexpr bool right_at< op, comma_t< R > > =
    !is_comparison(op) && !is_logical(op) &&
    !(is_shift(op) && comma_of_load< comma_t< R > >);

// A binary operator on two operands in their classes: the crossing from
// the operator and the view, values in the parser, to the node's
// parameters (principle 6), and the right operand's class made a type
// again at the view, made once for every pair of classes, the typed
// builder of each.  A left operand of another view than the
// operation's is a tree the parser does not make, and a right operand
// of a class its operator does not read is one no crossing sends: no
// node type is instantiated of either.
template< typename L, typename R >
expr_t
fwd_binary(binary_op_t op, view_t view, const L &lhs, const R &rhs)
{
    return cross(op, [&]< binary_op_t o >() {
        return cross_operation(view, [&]< view_t V >() {
            return typed< o, V >(rhs, [&]< typename C >(const C &r) -> expr_t {
                if constexpr (operand_at< o, V, L >() && right_at< o, C >)
                    return make_binary< o, V >(lhs, r);
                else
                    throw std::logic_error(
                        "v2: an operand of no class or view of its operation");
            });
        });
    });
}

// The left operand of && or || in its class: classed_operand()'s, but a
// change of sign or a cast to its own type over a load is its node, the
// operand tested at its own type, since aarch64's test of zero reads
// its width, the conversion's, and not the load's: (unsigned char) c &&
// x on an unsigned char c is tst w0, #0xff where c && x is cbz.
template< typename T >
decltype(auto)
logical_operand(const T &held, const expr_t &self)
{
    return classed_operand(held, self);
}

template< view_t V, typename W, sign_t S >
const expr_t &
logical_operand(const trunc_t< V, load_t< W, V, S > > &, const expr_t &self)
{
    return self;
}

// A binary operator on the held left operand and the right operand's
// class: the left operand in its class, the question of the operator,
// data here, asked once, as right_of() asks it.
template< typename T, typename R >
expr_t
binary_of(binary_op_t op,
          view_t view,
          const T &held,
          const expr_t &self,
          const R &rhs)
{
    if (is_logical(op))
        return fwd_binary(op, view, logical_operand(held, self), rhs);

    return fwd_binary(op, view, classed_operand(held, self), rhs);
}

// The right operand of a comma in the class its builder keeps it in,
// make_comma(): a constant, a load, and a trunc_t of a load's own view,
// which a binary operator's crossing reads as the load, each the node it
// is, since a comma's value is its right operand's; anything else
// erased; and a comma itself, which the builder takes apart.
template< typename T >
const expr_t &
comma_operand(const T &, const expr_t &self)
{
    return self;
}

template< constant C >
const C &
comma_operand(const C &c, const expr_t &)
{
    return c;
}

template< typename W, view_t V, sign_t S >
const load_t< W, V, S > &
comma_operand(const load_t< W, V, S > &l, const expr_t &)
{
    return l;
}

template< view_t V, typename W, sign_t S >
const trunc_t< V, load_t< W, V, S > > &
comma_operand(const trunc_t< V, load_t< W, V, S > > &t, const expr_t &)
{
    return t;
}

template< typename R >
const comma_t< R > &
comma_operand(const comma_t< R > &c, const expr_t &)
{
    return c;
}

// Whether a node is a place an assignment stores into: whether the
// builder of an assignment, make_assign(), is written for it (principle
// 8).
template< typename P >
concept assignable = requires(const P &place, const expr_t &value) {
    make_assign(place, value);
};

// The assignment to a held place, by its builder.
template< assignable P, typename V >
expr_t
fwd_assign(const P &place, const V &value)
{
    return make_assign(place, value);
}

// The parser asks is_assignable() before it builds an assignment, and
// says the error of one that is not, so no assignment to a node that is
// no place is built.
template< typename P, typename V >
expr_t
fwd_assign(const P &, const V &)
{
    throw std::logic_error("v2: an assignment to a node that is no place");
}

// A step of a held place, by its builder: the crossing from the step's
// operator, data in the parser, to the node's parameter (principle 6).
// The parser asks is_assignable() first, as for an assignment, so no
// step of a node that is no place is built.
template< assignable P >
expr_t
fwd_step(step_op_t op, const P &place)
{
    return cross(op, [&]< step_op_t o >() { return make_step< o >(place); });
}

template< typename P >
expr_t
fwd_step(step_op_t, const P &)
{
    throw std::logic_error("v2: a step of a node that is no place");
}

// A compound assignment to a held place, by its builder, as a step's:
// the crossing from the operator to the node's parameter, of the
// operators of a compound assignment alone, is_compound()'s, and the
// value in the class the builder keeps it in, asked through its handle,
// constant_of(): a constant of the place's view, the type the parser
// converted it to, or erased.
template< assignable P >
expr_t
fwd_compound(binary_op_t op, const P &place, const expr_t &value)
{
    return cross(op, [&]< binary_op_t o >() -> expr_t {
        if constexpr (is_compound(o)) {
            const auto k = constant_of(value);
            if (!k)
                return make_compound< o >(place, value);

            return classed_constant< view_of(P { }) >(*k, [&](const auto &c) {
                return make_compound< o >(place, c);
            });
        }
        else
            throw std::logic_error("v2: a compound assignment of no operator");
    });
}

template< typename P >
expr_t
fwd_compound(binary_op_t, const P &, const expr_t &)
{
    throw std::logic_error("v2: a compound assignment to a node that is no "
                           "place");
}

template< typename T >
struct expr_t::model_t final: interface_t
{
    explicit model_t(T t) : held(std::move(t))
    {
    }

    value_t value(amd64::state_t &s, unsigned wanted) const override
    {
        return fwd_value(s, held, wanted);
    }

    value_t value(arm64::state_t &s, unsigned wanted) const override
    {
        return fwd_value(s, held, wanted);
    }

    void effect(amd64::state_t &s) const override
    {
        fwd_effect(s, held);
    }

    void effect(arm64::state_t &s) const override
    {
        fwd_effect(s, held);
    }

    void branch(amd64::state_t &s, int label, bool sense) const override
    {
        fwd_branch(s, held, label, sense);
    }

    void branch(arm64::state_t &s, int label, bool sense) const override
    {
        fwd_branch(s, held, label, sense);
    }

    void value_branch(amd64::state_t &s, int label, bool sense) const override
    {
        fwd_value_branch(s, held, label, sense);
    }

    void initialize(amd64::state_t &s,
                    const amd64::address_t &at) const override
    {
        fwd_initialize(s, at, held);
    }

    void initialize(arm64::state_t &s,
                    const arm64::address_t &at) const override
    {
        fwd_initialize(s, at, held);
    }

    unsigned pinned_within(const amd64::state_t &s) const override
    {
        return fwd_pinned_within(s, held);
    }

    unsigned moved_within(const amd64::state_t &s) const override
    {
        return fwd_moved_within(s, held);
    }

    int valid(sign_t sign) const override
    {
        return valid_of(held, sign);
    }

    view_t view() const override
    {
        return view_of(held);
    }

    expr_t rvalue(const expr_t &self) const override
    {
        return fwd_rvalue(held, self);
    }

    std::optional< long long > constant_of() const override
    {
        return fwd_constant_of(held);
    }

    expr_t unary(unary_op_t op, view_t view, const expr_t &self) const override
    {
        return fwd_unary(op, view, unary_operand(held, self), self);
    }

    expr_t conversion(view_t to,
                      sign_t to_sign,
                      view_t from,
                      sign_t from_sign,
                      const expr_t &self) const override
    {
        return fwd_conversion(to, to_sign, from, from_sign,
                              converted_operand(held, self), self);
    }

    bool is_assignable() const override
    {
        return assignable< T >;
    }

    expr_t assign(const expr_t &value) const override
    {
        return fwd_assign(held, value);
    }

    expr_t step(step_op_t op) const override
    {
        return fwd_step(op, held);
    }

    expr_t compound(binary_op_t op, const expr_t &value) const override
    {
        return fwd_compound(op, held, value);
    }

    expr_t right_of(binary_op_t op,
                    view_t view,
                    const expr_t &lhs,
                    const expr_t &self) const override
    {
        const auto &rhs = crossing(held, self);
        if (crosses_erased(op, rhs))
            return lhs.self->binary(op, view, self, lhs);

        if constexpr (comma_of_load< T >) {
            if (is_shift(op))
                return lhs.self->binary(
                    op, view, comma_t<> { held.lhs, expr_t(held.rhs) }, lhs);
        }

        return lhs.self->binary(op, view, rhs, lhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const constant_class_t &rhs,
                  const expr_t &self) const override
    {
        return binary_of(op, view, held, self, rhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const load_class_t< slot_t > &rhs,
                  const expr_t &self) const override
    {
        return binary_of(op, view, held, self, rhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const load_class_t< global_t > &rhs,
                  const expr_t &self) const override
    {
        return binary_of(op, view, held, self, rhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const top_t< family_t::division > &rhs,
                  const expr_t &self) const override
    {
        return binary_of(op, view, held, self, rhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const top_t< family_t::shift > &rhs,
                  const expr_t &self) const override
    {
        return binary_of(op, view, held, self, rhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const expr_t &rhs,
                  const expr_t &self) const override
    {
        return binary_of(op, view, held, self, rhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const comma_t< constant_class_t > &rhs,
                  const expr_t &self) const override
    {
        return binary_of(op, view, held, self, rhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const comma_t< load_class_t< slot_t > > &rhs,
                  const expr_t &self) const override
    {
        return binary_of(op, view, held, self, rhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const comma_t< load_class_t< global_t > > &rhs,
                  const expr_t &self) const override
    {
        return binary_of(op, view, held, self, rhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const comma_t<> &rhs,
                  const expr_t &self) const override
    {
        return binary_of(op, view, held, self, rhs);
    }

    expr_t comma(const expr_t &lhs, const expr_t &self) const override
    {
        return make_comma(lhs, comma_operand(held, self), self);
    }

    T held;
};

// The handle owns its model through a pointer to the interface, whose
// destructor is virtual, so one shared_ptr serves every model: a
// make_shared of each model's type would instantiate a shared_ptr, a
// weak_ptr and an allocator of that type, for each node type of the zoo.
template< typename T >
expr_t::expr_t(T t)
{
    const interface_t *model = new model_t< T >(std::move(t));
    self.reset(model);
}

struct stmt_t::interface_t
{
    virtual ~interface_t() = default;

    virtual void generate(amd64::state_t &) const = 0;
    virtual void generate(arm64::state_t &) const = 0;

    // the builders of an if: whose then part is this statement, self,
    // with an else part or without one; and whose then part is a block
    // of which this statement is the one item, the second hop of 3.3
    virtual stmt_t if_then(const expr_t &cond,
                           const std::optional< stmt_t > &otherwise,
                           const stmt_t &self) const = 0;
    virtual stmt_t if_item(const expr_t &cond,
                           const std::optional< stmt_t > &otherwise,
                           const stmt_t &block) const = 0;
};

// The row of a held statement, as fwd_value() finds an expression's.
template< typename S, typename T >
void
fwd_generate(S &s, const T &t)
{
    generate(s, t);
}

template< typename T >
struct stmt_t::model_t final: interface_t
{
    explicit model_t(T t) : held(std::move(t))
    {
    }

    void generate(amd64::state_t &s) const override
    {
        fwd_generate(s, held);
    }

    void generate(arm64::state_t &s) const override
    {
        fwd_generate(s, held);
    }

    stmt_t if_then(const expr_t &cond,
                   const std::optional< stmt_t > &otherwise,
                   const stmt_t &self) const override
    {
        return make_if(cond, held, self, otherwise);
    }

    stmt_t if_item(const expr_t &cond,
                   const std::optional< stmt_t > &otherwise,
                   const stmt_t &block) const override
    {
        return make_if_of_item(cond, held, block, otherwise);
    }

    T held;
};

// As an expression's handle owns its model.
template< typename T >
stmt_t::stmt_t(T t)
{
    const interface_t *model = new model_t< T >(std::move(t));
    self.reset(model);
}

} // namespace rcc
