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

#include "amd64.hh"
#include "arm64.hh"
#include "build-typed.hh"
#include "expr.hh"
#include "register-stack.hh"
#include "rows-expressions.hh"
#include "stmt.hh"
#include "zoo.hh"

#include <concepts>
#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

// Whether a node type has a niche of its own on a line that defaults_t
// answers: the line's row found by argument alone, from outside rcc,
// where no using-directive is seen, so that a primary a row still asks
// of an operand of its own type, which lives in a namespace primary of
// its own (zoo.hh), is not found.  A model whose node has none answers
// the line by its default, written once.
namespace niche {

template< typename T, typename S >
concept on_effect = requires(S &s, const T &t) { effect(s, t); };

template< typename T, typename S, typename A >
concept on_initialize =
    requires(S &s, const A &at, const T &t) { initialize(s, at, t); };

template< typename T, typename S >
concept on_branch = requires(S &s, const T &t) { branch(s, t, 0, true); };

template< typename T >
concept on_value_in =
    requires(rcc::arm64::state_t &s, const T &t) { value_in(s, t, 0u); };

template< typename T >
concept on_takes_name =
    requires(const rcc::arm64::state_t &s, const T &t) { takes_name(s, t); };

template< typename T >
concept on_truth_value = requires(const T &t) { truth_value(t); };

template< typename T >
concept on_under_widening =
    requires(const T &t, const rcc::expr_t &self) { under_widening(t, self); };

template< typename T >
concept on_arm_widening = requires(const T &t) { arm_widening(t); };

template< typename T >
concept on_rvalue =
    requires(const T &t, const rcc::expr_t &self) { rvalue(t, self); };

template< typename T >
concept on_constant_of = requires(const T &t) { constant_of(t); };

template< typename T >
concept on_amd64_pointed =
    requires(rcc::amd64::state_t &s, const T &t, const rcc::expr_t &self) {
        pointed(s, t, self);
    };

template< typename T >
concept on_arm64_pointed =
    requires(rcc::arm64::state_t &s,
             const T &t,
             rcc::arm64::between_t between,
             const rcc::expr_t &self) { pointed(s, t, between, 0, self); };

template< typename T >
concept on_named_offset = requires(const T &t) { named_offset(t); };

template< typename T >
concept on_lvalue_offset =
    requires(const T &t, const rcc::expr_t &self) { lvalue_offset(t, self); };

template< typename T >
concept on_index_of =
    requires(rcc::arm64::state_t &s, const T &t) { index_of(s, t); };

template< typename T >
concept on_denoted =
    requires(const T &t, const rcc::expr_t &self, rcc::arm64::denoted_t d) {
        denoted(t, self, d);
    };

template< typename T >
concept on_constant_sum = requires(const T &t) { constant_sum(t); };

template< typename T >
concept on_bitcast =
    requires(const T &t, const rcc::expr_t &self) { make_bitcast(t, self); };

} // namespace niche

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

// The right operand's class as it crosses the interface to the left
// operand's model, one alternative for each class: a constant, a load
// of each where, a top_t of each family, the operand erased, and a
// comma of each of those its right operand crosses as.  The left
// operand's model takes every class by one operation, binary(), so a
// model has one line of the binary operators and not one for each
// class of the right operand.
using right_class_t = std::variant< constant_class_t,
                                    load_class_t< slot_t >,
                                    load_class_t< global_t >,
                                    load_class_t< indirect_t >,
                                    top_t< family_t::division >,
                                    top_t< family_t::shift >,
                                    expr_t,
                                    comma_t< constant_class_t >,
                                    comma_t< load_class_t< slot_t > >,
                                    comma_t< load_class_t< global_t > >,
                                    comma_t< load_class_t< indirect_t > >,
                                    comma_t<> >;

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
    virtual bool spills_within(const amd64::state_t &) const = 0;

    // the address a pointer's value designates, x86-64's descriptor
    virtual amd64::address_t pointed(amd64::state_t &,
                                     const expr_t &self) const = 0;

    // the summaries both targets read (principle 3): the width rule, at
    // a sign, and the view
    virtual int valid(sign_t) const = 0;
    virtual view_t view() const = 0;

    // aarch64's value at a return's top, rcc's rvalue_in(), and the
    // summary its ?: asks of an arm, rcc's takes_name()
    virtual value_t value_in(arm64::state_t &, unsigned wanted) const = 0;
    virtual bool takes_name(const arm64::state_t &) const = 0;

    // aarch64's summary of whether a call is within, rcc's calls_within()
    virtual bool calls_within(const arm64::state_t &) const = 0;

    // the summaries of a pointer both targets read: a local's address plus
    // a constant, and the address of a place through a pointer plus one
    virtual std::optional< named_offset_t > named_offset() const = 0;
    virtual std::optional< lvalue_offset_t >
    lvalue_offset(const expr_t &self) const = 0;

    // aarch64's: the address a pointer's value designates, an integer as
    // the form reads it, the walk down a pointer's chain and the sum by
    // a constant
    virtual arm64::address_t pointed(arm64::state_t &,
                                     arm64::between_t,
                                     int size,
                                     const expr_t &self) const = 0;
    virtual arm64::index_t index_of(arm64::state_t &) const = 0;
    virtual arm64::denoted_t denoted(const expr_t &self,
                                     arm64::denoted_t) const = 0;
    virtual std::optional< based_t > constant_sum() const = 0;

    // what the builder of ?: asks of an erased operand: whether it is a
    // truth value, and the operand under its conversions that keep
    // whether a value is 0, which a conditional's jump tests
    virtual bool truth_value() const = 0;
    virtual expr_t under_widening(const expr_t &self) const = 0;

    // whether the value is a join of ?:, whose width the state records
    virtual bool is_join() const = 0;

    // what a widening of a conditional asks of an erased arm: what the
    // arm costs the widening handed to it, and the arm widened
    virtual arm_widening_t arm_widening() const = 0;
    virtual expr_t widened_arm(const expr_t &self) const = 0;

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

    // & and *: the place's model, which knows the place's type, makes
    // its address, and the pointer's model the place it points to, at
    // the pointee's view and sign
    virtual bool is_place() const = 0;
    virtual expr_t address_of() const = 0;
    virtual expr_t indirection(view_t, sign_t, const expr_t &self) const = 0;

    // pointer arithmetic: the index's model, which knows the index's
    // class, makes the sum; the right pointer's, the difference; and the
    // operand's, a cast between pointer types
    virtual expr_t gep(binary_op_t op,
                       const expr_t &pointer,
                       long stride,
                       bool index_first,
                       const expr_t &self) const = 0;
    virtual expr_t
    difference(const expr_t &lhs, long stride, const expr_t &self) const = 0;
    virtual expr_t bitcast(const expr_t &self) const = 0;

    // an assignment: the place's model, which knows the place's type,
    // makes the node
    virtual bool is_assignable() const = 0;
    virtual expr_t assign(const expr_t &value) const = 0;

    // a step and a compound assignment: the place's model, as for an
    // assignment, makes the node; the view is the operation's
    virtual expr_t step(step_op_t op) const = 0;
    virtual expr_t
    compound(binary_op_t op, view_t, const expr_t &value) const = 0;

    // a binary operator, by the same two hops: the right operand's
    // model, which knows the right operand's class, calls the left's
    // operation with that class, which knows the left's class and makes
    // the node; the view is the operation's, at which the left's model
    // makes the class a type again; a comma on the right crosses as a
    // class of its own, its left operand erased and its right one's
    // class as a right operand of its own crosses
    virtual expr_t right_of(binary_op_t op,
                            view_t,
                            const expr_t &lhs,
                            const expr_t &self) const = 0;
    virtual expr_t binary(binary_op_t op,
                          view_t,
                          const right_class_t &rhs,
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

template< typename S, typename T >
bool
fwd_spills_within(const S &s, const T &t)
{
    return spills_within(s, t);
}

template< typename S, typename T >
amd64::address_t
fwd_pointed(S &s, const T &t, const expr_t &self)
{
    return pointed(s, t, self);
}

template< typename S, typename T >
value_t
fwd_value_in(S &s, const T &t, unsigned wanted)
{
    return value_in(s, t, wanted);
}

template< typename S, typename T >
bool
fwd_takes_name(const S &s, const T &t)
{
    return takes_name(s, t);
}

template< typename S, typename T >
bool
fwd_calls_within(const S &s, const T &t)
{
    return calls_within(s, t);
}

template< typename S, typename T >
arm64::address_t
fwd_pointed(
    S &s, const T &t, arm64::between_t between, int size, const expr_t &self)
{
    return pointed(s, t, between, size, self);
}

template< typename S, typename T >
arm64::index_t
fwd_index_of(S &s, const T &t)
{
    return index_of(s, t);
}

template< typename T >
arm64::denoted_t
fwd_denoted(const T &t, const expr_t &self, arm64::denoted_t d)
{
    return denoted(t, self, std::move(d));
}

template< typename T >
std::optional< based_t >
fwd_constant_sum(const T &t)
{
    return constant_sum(t);
}

template< typename T >
std::optional< named_offset_t >
fwd_named_offset(const T &t)
{
    return named_offset(t);
}

template< typename T >
std::optional< lvalue_offset_t >
fwd_lvalue_offset(const T &t, const expr_t &self)
{
    return lvalue_offset(t, self);
}

template< typename T >
bool
fwd_truth_value(const T &t)
{
    return truth_value(t);
}

template< typename T >
expr_t
fwd_under_widening(const T &t, const expr_t &self)
{
    return under_widening(t, self);
}

template< typename T >
bool
fwd_is_join(const T &t)
{
    return is_join(t);
}

template< typename T >
arm_widening_t
fwd_arm_widening(const T &t)
{
    return arm_widening(t);
}

// An arm in the class a conversion reads widened by the builder of
// that class, as a conversion is.
template< typename A >
expr_t
fwd_widened_arm(const A &arg, const expr_t &self)
{
    return widened_arm(arg, self);
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
// folds, a load, a conversion over a load or that writes nothing, which
// a conversion reads through, and a conditional, whose signed widening
// is a type of its own; anything else as pinning() keeps it, a top_t
// where its top operator pins registers on x86-64, and erased otherwise.
// The three crossings of fwd_conversion() are instantiated once for
// each class, and not for each node type.
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

template< view_t V, view_t F, typename W >
const sext_t< V, F, trunc_t< F, load_t< W, F, sign_t::unsigned_ > > > &
converted_operand(
    const sext_t< V, F, trunc_t< F, load_t< W, F, sign_t::unsigned_ > > > &e,
    const expr_t &)
{
    return e;
}

template< view_t V, view_t F, typename W >
const zext_t< V, F, trunc_t< F, load_t< W, F, sign_t::signed_ > > > &
converted_operand(
    const zext_t< V, F, trunc_t< F, load_t< W, F, sign_t::signed_ > > > &e,
    const expr_t &)
{
    return e;
}

template< view_t V, bool jumps >
const conditional_t< V, jumps > &
converted_operand(const conditional_t< V, jumps > &c, const expr_t &)
{
    return c;
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

// The view of a class of a left operand, a constant's or a load's, its
// type's parameter: a load through a pointer holds a handle, which no
// default value stands for.
template< typename C >
constexpr view_t class_view = view_of(C { });

template< typename W, view_t V, sign_t S >
constexpr view_t class_view< load_t< W, V, S > > = V;

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
        return class_view< C > == V;
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

// A binary operator on the left operand in its two classes, as an
// operand of && or || and of any other operator, and the right
// operand's class, the alternative right_class_t holds: the question of
// the operator, data here, asked once, as right_of() asks it.  It takes
// the classes and not the node, so it is instantiated once for each
// pair of the left operand's classes, and not for each node type.
template< typename C, typename G >
expr_t
binary_of(binary_op_t op,
          view_t view,
          const C &classed,
          const G &logical,
          const right_class_t &rhs)
{
    return std::visit(
        [&](const auto &r) {
            if (is_logical(op))
                return fwd_binary(op, view, logical, r);

            return fwd_binary(op, view, classed, r);
        },
        rhs);
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

// Whether a node is a place, whose address & takes: whether its
// builder, make_address_of(), is written for it (principle 8).
template< typename P >
concept place = requires(const P &p) { make_address_of(p); };

// The address of a held place, by its builder.
template< place P >
expr_t
fwd_address_of(const P &p)
{
    return make_address_of(p);
}

// The pointer of * in the class its builder reads, make_indirection():
// an address, whose place * gives again, or anything else erased, the
// handle that holds it.  The crossings of fwd_indirection() are
// instantiated once for each class, and not for each node type.
template< typename T >
const expr_t &
pointed_operand(const T &, const expr_t &self)
{
    return self;
}

template< typename W >
const address_of_t< W > &
pointed_operand(const address_of_t< W > &address, const expr_t &)
{
    return address;
}

// The place a pointer in its class points to: the crossing from the
// pointee's view and sign, values in the parser, to the place's
// parameters (principle 6), the typed builder of each.
template< typename P >
expr_t
fwd_indirection(view_t view, sign_t sign, const P &pointer)
{
    return cross(view, [&]< view_t V >() {
        return cross(sign, [&]< sign_t S >() {
            return make_indirection< V, S >(pointer);
        });
    });
}

// The index of pointer arithmetic in the class its builder reads,
// make_gep(): a constant, a load, a load under a trunc_t of its own
// view among them, rcc's memory_operand() under under_kept_bits(), and
// a top_t where its top operator, read through its conversions, pins
// registers on x86-64, pinning()'s; anything else erased.  The
// crossings of fwd_gep() are instantiated once for each class, and not
// for each node type.
template< typename T >
decltype(auto)
gep_operand(const T &e, const expr_t &self)
{
    return pinning(e, self);
}

template< constant C >
const C &
gep_operand(const C &c, const expr_t &)
{
    return c;
}

template< typename W, view_t V, sign_t S >
const load_t< W, V, S > &
gep_operand(const load_t< W, V, S > &l, const expr_t &)
{
    return l;
}

template< view_t V, typename W, sign_t S >
const load_t< W, V, S > &
gep_operand(const trunc_t< V, load_t< W, V, S > > &t, const expr_t &)
{
    return t.arg;
}

// A sum on an index in its class: the crossing from the operator and the
// order, values in the parser, to the node's parameters (principle 6),
// - never having its integer first.
template< typename N >
expr_t
fwd_gep(binary_op_t op,
        const expr_t &pointer,
        const N &index,
        long stride,
        bool index_first,
        const expr_t &self)
{
    if (op == binary_op_t::sub)
        return make_gep< binary_op_t::sub, false >(pointer, index, stride,
                                                   self);

    if (index_first)
        return make_gep< binary_op_t::add, true >(pointer, index, stride, self);

    return make_gep< binary_op_t::add, false >(pointer, index, stride, self);
}

// The right pointer of a difference in the class its builder reads,
// make_difference(): a load, or anything else erased.
template< typename T >
const expr_t &
difference_operand(const T &, const expr_t &self)
{
    return self;
}

template< typename W, view_t V, sign_t S >
const load_t< W, V, S > &
difference_operand(const load_t< W, V, S > &l, const expr_t &)
{
    return l;
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

// A step of a held place, by its builder: the crossing from the step's
// operator, data in the parser, to the node's parameter (principle 6).
template< assignable P >
expr_t
fwd_step(step_op_t op, const P &place)
{
    return cross(op, [&]< step_op_t o >() { return make_step< o >(place); });
}

// A place through a pointer has no step and no compound assignment yet:
// the parser refuses either at its operator, so neither is built.
template< view_t V, sign_t S >
expr_t
fwd_step(step_op_t, const place_t< indirect_t, V, S > &)
{
    throw std::logic_error("v2: a step through a pointer");
}

// A compound assignment to a held place, by its builder, as a step's:
// the crossings from the operator and the operation's view to the
// node's parameters, of the operators of a compound assignment alone,
// is_compound()'s, and of a view no narrower than the place's, which
// the operation's type is; and the value in the class the builder keeps
// it in, asked through its handle, constant_of(): a constant of the
// operation's view, the type the parser converted it to, a shift's
// count among them, whose rows read its low bits alone, or erased.
template< assignable P >
expr_t
fwd_compound(binary_op_t op, view_t view, const P &place, const expr_t &value)
{
    return cross(op, [&]< binary_op_t o >() -> expr_t {
        return cross_operation(view, [&]< view_t O >() -> expr_t {
            if constexpr (is_compound(o) && width(O) >= width(view_of(P { }))) {
                const auto k = constant_of(value);
                if (!k)
                    return make_compound< o, O >(place, value);

                return classed_constant< O >(*k, [&](const auto &c) {
                    return make_compound< o, O >(place, c);
                });
            }
            else
                throw std::logic_error(
                    "v2: a compound assignment of no operator or view");
        });
    });
}

template< view_t V, sign_t S >
expr_t
fwd_compound(binary_op_t,
             view_t,
             const place_t< indirect_t, V, S > &,
             const expr_t &)
{
    throw std::logic_error("v2: a compound assignment through a pointer");
}

// The lines whose answer, for a node type with no niche of its own on
// them, does not read the node's type, each default written once, here,
// over the node's handle self and the node's own lines, value(),
// view(), valid(), where its primary would be instantiated for every
// node type (docs/typed-tree.md 3.1): an effect, a store and aarch64's
// jump derived from the value, x86-64's jump from value_branch(); a
// pointer's address formed from its value; the value for an index and
// for a consumer at a return's top; the node itself for a conversion
// that keeps it, for a cast between pointer types and for the lvalue
// conversion; the summaries' answers of a node that is none of theirs;
// and each builder's hop on the handle's class.  Each body is the
// primary's, so a model that takes it writes what the primary writes.
// The parser asks is_place() before it builds an address and
// is_assignable() before an assignment, a step or a compound
// assignment, and says the error of one that is not, so none of those
// four is built of a node that is no place.
struct expr_t::defaults_t: interface_t
{
    // an expression for its effect, 6.8.4p2, its value dropped
    void effect(amd64::state_t &s) const override
    {
        release(s, value(s, 0));
    }

    void effect(arm64::state_t &s) const override
    {
        release(s, value(s, 0));
    }

    // the store row of an initializer, rcc's initialize(), chapter 6.6:
    // the value in a register, stored at the address its caller formed,
    // at the value's view, which is the place's, since the parser
    // converted the value to the place's type, and released; the
    // targets' niches store a constant with no register held
    void initialize(amd64::state_t &s,
                    const amd64::address_t &at) const override
    {
        const value_t v = value(s, 0);
        store(s, v, at, view());
        release(s, v);
    }

    void initialize(arm64::state_t &s,
                    const arm64::address_t &at) const override
    {
        const value_t v = value(s, 0);
        store(s, v, at, view());
        release(s, v);
    }

    // a jump on a value is the value compared with 0 (3.4): x86-64's
    // value_branch(), and aarch64's test of the register at the value's
    // view and the width it is valid to
    void branch(amd64::state_t &s, int label, bool sense) const override
    {
        value_branch(s, label, sense);
    }

    void branch(arm64::state_t &s, int label, bool sense) const override
    {
        const value_t v = value(s, 0);
        arm64::test_zero(s, v, view(), valid(sign_t::signed_), label, sense);
        release(s, v);
    }

    value_t value_in(arm64::state_t &s, unsigned wanted) const override
    {
        return value(s, wanted);
    }

    // a pointer's address formed from its value, each target's
    // erased_pointed(), and its summaries none, the node itself the
    // walk's base and the value the index
    amd64::address_t pointed(amd64::state_t &s,
                             const expr_t &self) const override
    {
        return amd64::erased_pointed(s, self);
    }

    arm64::address_t pointed(arm64::state_t &s,
                             arm64::between_t between,
                             int size,
                             const expr_t &self) const override
    {
        return arm64::erased_pointed(s, self, between, size);
    }

    std::optional< named_offset_t > named_offset() const override
    {
        return std::nullopt;
    }

    std::optional< lvalue_offset_t >
    lvalue_offset(const expr_t &) const override
    {
        return std::nullopt;
    }

    std::optional< based_t > constant_sum() const override
    {
        return std::nullopt;
    }

    arm64::denoted_t denoted(const expr_t &self,
                             arm64::denoted_t d) const override
    {
        d.base = self;
        return d;
    }

    arm64::index_t index_of(arm64::state_t &s) const override
    {
        return { value(s, 0) };
    }

    expr_t bitcast(const expr_t &self) const override
    {
        return self;
    }

    bool takes_name(const arm64::state_t &) const override
    {
        return true;
    }

    bool truth_value() const override
    {
        return false;
    }

    expr_t under_widening(const expr_t &self) const override
    {
        return self;
    }

    arm_widening_t arm_widening() const override
    {
        return arm_widening_t::extension;
    }

    expr_t rvalue(const expr_t &self) const override
    {
        return self;
    }

    std::optional< long long > constant_of() const override
    {
        return std::nullopt;
    }

    expr_t address_of() const override
    {
        throw std::logic_error("v2: the address of a node that is no place");
    }

    expr_t assign(const expr_t &) const override
    {
        throw std::logic_error("v2: an assignment to a node that is no place");
    }

    expr_t step(step_op_t) const override
    {
        throw std::logic_error("v2: a step of a node that is no place");
    }

    expr_t compound(binary_op_t, view_t, const expr_t &) const override
    {
        throw std::logic_error("v2: a compound assignment to a node that is no "
                               "place");
    }

    bool is_place() const override
    {
        return false;
    }

    bool is_assignable() const override
    {
        return false;
    }

    // the builders' hops of an operand erased in every class a builder
    // reads (erased_operand): the crossings of the handle's class
    expr_t widened_arm(const expr_t &self) const override
    {
        return fwd_widened_arm(self, self);
    }

    expr_t unary(unary_op_t op, view_t view, const expr_t &self) const override
    {
        return fwd_unary(op, view, self, self);
    }

    expr_t
    indirection(view_t view, sign_t sign, const expr_t &self) const override
    {
        return fwd_indirection(view, sign, self);
    }

    expr_t gep(binary_op_t op,
               const expr_t &pointer,
               long stride,
               bool index_first,
               const expr_t &self) const override
    {
        return fwd_gep(op, pointer, self, stride, index_first, self);
    }

    expr_t difference(const expr_t &lhs,
                      long stride,
                      const expr_t &self) const override
    {
        return make_difference(lhs, self, stride, self);
    }

    expr_t conversion(view_t to,
                      sign_t to_sign,
                      view_t from,
                      sign_t from_sign,
                      const expr_t &self) const override
    {
        return fwd_conversion(to, to_sign, from, from_sign, self, self);
    }

    expr_t right_of(binary_op_t op,
                    view_t view,
                    const expr_t &lhs,
                    const expr_t &self) const override
    {
        return lhs.self->binary(op, view, self, lhs);
    }

    expr_t binary(binary_op_t op,
                  view_t view,
                  const right_class_t &rhs,
                  const expr_t &self) const override
    {
        return binary_of(op, view, self, self, rhs);
    }

    expr_t comma(const expr_t &lhs, const expr_t &self) const override
    {
        return make_comma(lhs, self, self);
    }
};

// Whether a class of an operand is the handle, held or a copy of it:
// pinning() answers a binary operator that pins nothing by its handle's
// copy.
template< typename C >
concept handle_class = std::same_as< std::remove_cvref_t< C >, expr_t >;

// Whether a node type is erased in every class a builder reads an
// operand in: each builder's hop on it is then the handle's, defaults_t's.
template< typename T >
concept erased_operand = requires(const T &t, const expr_t &self) {
    { unary_operand(t, self) } -> handle_class;
    { converted_operand(t, self) } -> handle_class;
    { pointed_operand(t, self) } -> handle_class;
    { gep_operand(t, self) } -> handle_class;
    { difference_operand(t, self) } -> handle_class;
    { comma_operand(t, self) } -> handle_class;
    { crossing(t, self) } -> handle_class;
    { classed_operand(t, self) } -> handle_class;
    { logical_operand(t, self) } -> handle_class;
};

// The model of a node type: the lines every node type answers by its
// own rows, its values and the summaries of its subtree; its jumps,
// truth_value(), takes_name() and arm_widening(), whose niches are
// many, by its niche where it has one and by the default otherwise.
// defaults_t answers the rest, where niched_model_t does not.
template< typename T >
struct expr_t::model_t: defaults_t
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

    void branch(amd64::state_t &s, int label, bool sense) const override
    {
        if constexpr (niche::on_branch< T, amd64::state_t >)
            fwd_branch(s, held, label, sense);
        else
            defaults_t::branch(s, label, sense);
    }

    void branch(arm64::state_t &s, int label, bool sense) const override
    {
        if constexpr (niche::on_branch< T, arm64::state_t >)
            fwd_branch(s, held, label, sense);
        else
            defaults_t::branch(s, label, sense);
    }

    void value_branch(amd64::state_t &s, int label, bool sense) const override
    {
        fwd_value_branch(s, held, label, sense);
    }

    unsigned pinned_within(const amd64::state_t &s) const override
    {
        return fwd_pinned_within(s, held);
    }

    unsigned moved_within(const amd64::state_t &s) const override
    {
        return fwd_moved_within(s, held);
    }

    bool spills_within(const amd64::state_t &s) const override
    {
        return fwd_spills_within(s, held);
    }

    int valid(sign_t sign) const override
    {
        return valid_of(held, sign);
    }

    view_t view() const override
    {
        return view_of(held);
    }

    bool takes_name(const arm64::state_t &s) const override
    {
        if constexpr (niche::on_takes_name< T >)
            return fwd_takes_name(s, held);
        else
            return defaults_t::takes_name(s);
    }

    bool calls_within(const arm64::state_t &s) const override
    {
        return fwd_calls_within(s, held);
    }

    bool truth_value() const override
    {
        if constexpr (niche::on_truth_value< T >)
            return fwd_truth_value(held);
        else
            return defaults_t::truth_value();
    }

    bool is_join() const override
    {
        return fwd_is_join(held);
    }

    arm_widening_t arm_widening() const override
    {
        if constexpr (niche::on_arm_widening< T >)
            return fwd_arm_widening(held);
        else
            return defaults_t::arm_widening();
    }

    T held;
};

// Whether a node type answers a line of defaults_t by its own row, a
// niche of niche::'s, a place, or of a class of its own as an operand,
// and so has the model of its own kind, niched_model_t.  The one list of
// the lines kept by hand: a line niched_model_t answers is named here
// too, or a node type whose only niche is on it takes the default,
// silently.
template< typename T >
concept niched = niche::on_effect< T, amd64::state_t > ||
                 niche::on_effect< T, arm64::state_t > ||
                 niche::on_initialize< T, amd64::state_t, amd64::address_t > ||
                 niche::on_initialize< T, arm64::state_t, arm64::address_t > ||
                 niche::on_value_in< T > || niche::on_under_widening< T > ||
                 niche::on_rvalue< T > || niche::on_constant_of< T > ||
                 niche::on_amd64_pointed< T > || niche::on_arm64_pointed< T > ||
                 niche::on_named_offset< T > || niche::on_lvalue_offset< T > ||
                 niche::on_index_of< T > || niche::on_denoted< T > ||
                 niche::on_constant_sum< T > || niche::on_bitcast< T > ||
                 place< T > || assignable< T > || !erased_operand< T >;

// The model of a niched node type: the lines of defaults_t by its niche
// where it has one and by the default otherwise, and the builders' hops
// in its classes.
template< typename T >
struct expr_t::niched_model_t final: model_t< T >
{
    using model_t< T >::model_t;
    using model_t< T >::held;

    void effect(amd64::state_t &s) const override
    {
        if constexpr (niche::on_effect< T, amd64::state_t >)
            fwd_effect(s, held);
        else
            defaults_t::effect(s);
    }

    void effect(arm64::state_t &s) const override
    {
        if constexpr (niche::on_effect< T, arm64::state_t >)
            fwd_effect(s, held);
        else
            defaults_t::effect(s);
    }

    void initialize(amd64::state_t &s,
                    const amd64::address_t &at) const override
    {
        if constexpr (niche::on_initialize< T, amd64::state_t,
                                            amd64::address_t >)
            fwd_initialize(s, at, held);
        else
            defaults_t::initialize(s, at);
    }

    void initialize(arm64::state_t &s,
                    const arm64::address_t &at) const override
    {
        if constexpr (niche::on_initialize< T, arm64::state_t,
                                            arm64::address_t >)
            fwd_initialize(s, at, held);
        else
            defaults_t::initialize(s, at);
    }

    value_t value_in(arm64::state_t &s, unsigned wanted) const override
    {
        if constexpr (niche::on_value_in< T >)
            return fwd_value_in(s, held, wanted);
        else
            return defaults_t::value_in(s, wanted);
    }

    expr_t under_widening(const expr_t &self) const override
    {
        if constexpr (niche::on_under_widening< T >)
            return fwd_under_widening(held, self);
        else
            return defaults_t::under_widening(self);
    }

    expr_t widened_arm(const expr_t &self) const override
    {
        return fwd_widened_arm(converted_operand(held, self), self);
    }

    expr_t rvalue(const expr_t &self) const override
    {
        if constexpr (niche::on_rvalue< T >)
            return fwd_rvalue(held, self);
        else
            return defaults_t::rvalue(self);
    }

    std::optional< long long > constant_of() const override
    {
        if constexpr (niche::on_constant_of< T >)
            return fwd_constant_of(held);
        else
            return defaults_t::constant_of();
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

    bool is_place() const override
    {
        return place< T >;
    }

    expr_t address_of() const override
    {
        if constexpr (place< T >)
            return fwd_address_of(held);
        else
            return defaults_t::address_of();
    }

    expr_t
    indirection(view_t view, sign_t sign, const expr_t &self) const override
    {
        return fwd_indirection(view, sign, pointed_operand(held, self));
    }

    amd64::address_t pointed(amd64::state_t &s,
                             const expr_t &self) const override
    {
        if constexpr (niche::on_amd64_pointed< T >)
            return fwd_pointed(s, held, self);
        else
            return defaults_t::pointed(s, self);
    }

    arm64::address_t pointed(arm64::state_t &s,
                             arm64::between_t between,
                             int size,
                             const expr_t &self) const override
    {
        if constexpr (niche::on_arm64_pointed< T >)
            return fwd_pointed(s, held, between, size, self);
        else
            return defaults_t::pointed(s, between, size, self);
    }

    std::optional< named_offset_t > named_offset() const override
    {
        if constexpr (niche::on_named_offset< T >)
            return fwd_named_offset(held);
        else
            return defaults_t::named_offset();
    }

    std::optional< lvalue_offset_t >
    lvalue_offset(const expr_t &self) const override
    {
        if constexpr (niche::on_lvalue_offset< T >)
            return fwd_lvalue_offset(held, self);
        else
            return defaults_t::lvalue_offset(self);
    }

    std::optional< based_t > constant_sum() const override
    {
        if constexpr (niche::on_constant_sum< T >)
            return fwd_constant_sum(held);
        else
            return defaults_t::constant_sum();
    }

    arm64::denoted_t denoted(const expr_t &self,
                             arm64::denoted_t d) const override
    {
        if constexpr (niche::on_denoted< T >)
            return fwd_denoted(held, self, std::move(d));
        else
            return defaults_t::denoted(self, std::move(d));
    }

    arm64::index_t index_of(arm64::state_t &s) const override
    {
        if constexpr (niche::on_index_of< T >)
            return fwd_index_of(s, held);
        else
            return defaults_t::index_of(s);
    }

    expr_t gep(binary_op_t op,
               const expr_t &pointer,
               long stride,
               bool index_first,
               const expr_t &self) const override
    {
        return fwd_gep(op, pointer, gep_operand(held, self), stride,
                       index_first, self);
    }

    expr_t difference(const expr_t &lhs,
                      long stride,
                      const expr_t &self) const override
    {
        return make_difference(lhs, difference_operand(held, self), stride,
                               self);
    }

    expr_t bitcast(const expr_t &self) const override
    {
        if constexpr (niche::on_bitcast< T >)
            return make_bitcast(held, self);
        else
            return defaults_t::bitcast(self);
    }

    bool is_assignable() const override
    {
        return assignable< T >;
    }

    expr_t assign(const expr_t &value) const override
    {
        if constexpr (assignable< T >)
            return fwd_assign(held, value);
        else
            return defaults_t::assign(value);
    }

    expr_t step(step_op_t op) const override
    {
        if constexpr (assignable< T >)
            return fwd_step(op, held);
        else
            return defaults_t::step(op);
    }

    expr_t
    compound(binary_op_t op, view_t view, const expr_t &value) const override
    {
        if constexpr (assignable< T >)
            return fwd_compound(op, view, held, value);
        else
            return defaults_t::compound(op, view, value);
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
                  const right_class_t &rhs,
                  const expr_t &self) const override
    {
        return binary_of(op, view, classed_operand(held, self),
                         logical_operand(held, self), rhs);
    }

    expr_t comma(const expr_t &lhs, const expr_t &self) const override
    {
        return make_comma(lhs, comma_operand(held, self), self);
    }
};

// The handle owns its model through a pointer to the interface, whose
// destructor is virtual, so one shared_ptr serves every model: a
// make_shared of each model's type would instantiate a shared_ptr, a
// weak_ptr and an allocator of that type, for each node type of the zoo.
template< typename T >
expr_t::expr_t(T t)
{
    using model =
        std::conditional_t< niched< T >, niched_model_t< T >, model_t< T > >;

    const interface_t *m = new model(std::move(t));
    self.reset(m);
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
