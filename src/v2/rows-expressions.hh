// -*- mode: c++; -*-

// The rows of the expressions that both targets write alike: templates
// over the target's state, which name no register and write no text of
// their own.  value(), the node's value held in a register its row
// takes, is the entry point with no primary (docs/typed-tree.md 3.4):
// every node type has a row that returns its value, and the primaries
// of effect() and branch() are derived from it, a value dropped and a
// value compared with 0.  Each target hands a value's row a hint,
// wanted, the registers the consumer names, and a row here hands it on
// to the operand whose register the node's value is, as rcc's rows hand
// wanted on (3.4).  build.cc includes this header beside the targets'
// rows.

#pragma once

#include "assembly.hh"
#include "register-stack.hh"
#include "zoo.hh"

#include <stdexcept>
#include <string_view>
#include <vector>

namespace rcc {

// A place is read only through its load, the lvalue conversion the
// parser asks of every operand whose value it uses, rvalue(): a place
// asked for its value is a tree the parser does not make.
template< typename S, typename W, view_t V, sign_t O >
value_t
value(S &, const place_t< W, V, O > &, unsigned = 0)
{
    throw std::logic_error("v2: a place read without its lvalue conversion");
}

// A right operand kept as a top_t is the node it holds, whose own row
// writes it through the handle.
template< typename S, family_t F >
value_t
value(S &s, const top_t< F > &t, unsigned wanted = 0)
{
    return value(s, t.node, wanted);
}

// A widening, rcc's converted() for one: the operand's value, handed
// the consumer's register, then its extension by the sign the operand's
// type has, the node's, where the width rule read at that sign says
// that the register is not valid to V's width yet; each target's
// extended() writes it into the register the consumer names where it
// can.  (long) (a + b) on two ints is movslq %eax, %rax, the same on two
// unsigneds writes nothing, their 32-bit write valid to 64 bits, and
// so does (long) (a < b), a truth value; (unsigned long) (unsigned) i is
// movl %eax, %eax.  The row asks its operand's summary, as x86-64's rows
// ask pinned_within(); a niche of each target reads a load extended.
template< typename S, view_t V, view_t F, typename A >
value_t
value(S &s, const sext_t< V, F, A > &e, unsigned wanted = 0)
{
    const value_t v = value(s, e.arg, wanted);
    if (valid_of(e.arg, sign_t::signed_) >= width(V))
        return v;

    return extended(s, v, F, sign_t::signed_, V, wanted);
}

template< typename S, view_t V, view_t F, typename A >
value_t
value(S &s, const zext_t< V, F, A > &e, unsigned wanted = 0)
{
    const value_t v = value(s, e.arg, wanted);
    if (valid_of(e.arg, sign_t::unsigned_) >= width(V))
        return v;

    return extended(s, v, F, sign_t::unsigned_, V, wanted);
}

// A widening over a promotion of the object, which its builder makes
// only where one extension is the two, rcc's widened_read(): the row of
// the widening of the object itself, the one load of the object at its
// own type, (long) (int) c on a signed char c being movsbq -1(%rbp),
// %rax, (long) (int) c on an unsigned char movzbl -1(%rbp), %eax.  The
// row of another node (docs/typed-tree.md 3.2), so each target's niche
// of the extending load serves both.
template< typename S, view_t V, view_t F, view_t G, typename W >
value_t
value(S &s,
      const sext_t< V, F, sext_t< F, G, load_t< W, G, sign_t::signed_ > > > &e,
      unsigned wanted = 0)
{
    return value(s,
                 sext_t< V, G, load_t< W, G, sign_t::signed_ > > { e.arg.arg },
                 wanted);
}

template< typename S, view_t V, view_t F, view_t G, typename W >
value_t
value(
    S &s,
    const sext_t< V, F, zext_t< F, G, load_t< W, G, sign_t::unsigned_ > > > &e,
    unsigned wanted = 0)
{
    return value(
        s, zext_t< V, G, load_t< W, G, sign_t::unsigned_ > > { e.arg.arg },
        wanted);
}

template< typename S, view_t V, view_t F, view_t G, typename W >
value_t
value(
    S &s,
    const zext_t< V, F, zext_t< F, G, load_t< W, G, sign_t::unsigned_ > > > &e,
    unsigned wanted = 0)
{
    return value(
        s, zext_t< V, G, load_t< W, G, sign_t::unsigned_ > > { e.arg.arg },
        wanted);
}

// A widening over a change of sign of a load, which its builder keeps
// as its operand's type, is the one load of the object at the type
// converted from, the row of that node, so each target's niche of the
// extending load serves it; and a widening over its promotion is the
// widening over the change of sign itself, as over the promotion of a
// load.
template< typename S, view_t V, view_t F, typename W >
value_t
value(S &s,
      const sext_t< V, F, trunc_t< F, load_t< W, F, sign_t::unsigned_ > > > &e,
      unsigned wanted = 0)
{
    return value(
        s,
        sext_t< V, F, load_t< W, F, sign_t::signed_ > > { { e.arg.arg.where } },
        wanted);
}

template< typename S, view_t V, view_t F, typename W >
value_t
value(S &s,
      const zext_t< V, F, trunc_t< F, load_t< W, F, sign_t::signed_ > > > &e,
      unsigned wanted = 0)
{
    return value(s,
                 zext_t< V, F, load_t< W, F, sign_t::unsigned_ > > {
                     { e.arg.arg.where } },
                 wanted);
}

template< typename S, view_t V, view_t F, view_t G, typename W >
value_t
value(S &s,
      const sext_t<
          V,
          F,
          sext_t< F, G, trunc_t< G, load_t< W, G, sign_t::unsigned_ > > > > &e,
      unsigned wanted = 0)
{
    return value(
        s,
        sext_t< V, G, trunc_t< G, load_t< W, G, sign_t::unsigned_ > > > {
            e.arg.arg },
        wanted);
}

template< typename S, view_t V, view_t F, view_t G, typename W >
value_t
value(S &s,
      const sext_t<
          V,
          F,
          zext_t< F, G, trunc_t< G, load_t< W, G, sign_t::signed_ > > > > &e,
      unsigned wanted = 0)
{
    return value(s,
                 zext_t< V, G, trunc_t< G, load_t< W, G, sign_t::signed_ > > > {
                     e.arg.arg },
                 wanted);
}

template< typename S, view_t V, view_t F, view_t G, typename W >
value_t
value(S &s,
      const zext_t<
          V,
          F,
          zext_t< F, G, trunc_t< G, load_t< W, G, sign_t::signed_ > > > > &e,
      unsigned wanted = 0)
{
    return value(s,
                 zext_t< V, G, trunc_t< G, load_t< W, G, sign_t::signed_ > > > {
                     e.arg.arg },
                 wanted);
}

// A conversion that writes nothing is its operand's value, the low bits
// of whose register are the value at V (G2).
template< typename S, view_t V, typename A >
value_t
value(S &s, const trunc_t< V, A > &t, unsigned wanted = 0)
{
    return value(s, t.arg, wanted);
}

// The store of a held value at an address, both targets' rcc store():
// the value's register, reloaded outside the address's registers where
// it was spilled, then the address's operand formed outside it, which
// on aarch64 writes the adrp of a global's page right before its
// access, and the store at the view.  The address stays held, for the
// caller to give back.
template< typename S, typename A >
void
store(S &s, value_t v, const A &at, view_t view)
{
    const auto r = reg_of(s, v, registers_of(s, at));
    store(s, r, operand(s, at, bit(r)), view);
}

// The store row of an initializer, rcc's initialize(), chapter 6.6: the
// value in a register, stored at the address its caller formed, at the
// value's view, which is the place's, since the parser converted the
// value to the place's type (docs/typed-tree.md 3.4), and released.
// The targets' niches store a constant with no register held, found by
// overload on the value's type, which the handle's row, the entry
// point, knows.
template< typename S, typename A, typename T >
void
initialize(S &s, const A &at, const T &e)
{
    const value_t v = value(s, e);
    store(s, v, at, view_of(e));
    release(s, v);
}

// A local's slot, of its view's bytes at as many bytes' alignment, from
// the frame's allocator, which a spill's slot comes from too, so the
// two interleave in the order they are taken; its displacement is
// recorded under the local's number, where a use reads it
// (docs/typed-tree.md 4.5).
template< typename S >
void
allocate(S &s, slot_t where, view_t view)
{
    const int bytes = width(view) / 8;

    s.slots.emplace(where.id, allocate(s, bytes, bytes));
}

// An expression for its effect, 6.8.4p2, its value dropped: the value
// held, and released at once.
template< typename S, typename T >
void
effect(S &s, const T &t)
{
    release(s, value(s, t));
}

// An assignment for its effect, rcc's assign() and
// constant_assignment(): the place first, G13, formed outside what each
// target's place_of() leaves for last, then the value's store at it, its
// own entry point, so x = 7; writes what int x = 7; writes and each
// target's niche of initialize() serves both, then the place given
// back.
template< typename S, typename W, view_t V >
void
effect(S &s, const assign_t< W, V > &a)
{
    const auto at = place_of(s, a.where, a.value, 0);
    initialize(s, at, a.value);
    release(s, at);
}

// The primary of branch, rcc's cbranch() for a node that is no
// comparison, no ! and no && or ||: a jump on a value is the value
// compared with 0 (3.4), each target's value_branch(), where x86-64 has
// niches of its own and aarch64 reads the width rule.
template< typename S, typename T >
void
branch(S &s, const T &t, int label, bool sense)
{
    value_branch(s, t, label, sense);
}

// !e as a jump writes nothing of its own: the jump on e, the sense
// flipped.
template< typename S, view_t V, typename A >
void
branch(S &s,
       const unary_t< unary_op_t::log_not, V, A > &u,
       int label,
       bool sense)
{
    branch(s, u.arg, label, !sense);
}

// && and || as a jump, rcc's cbranch(), 6.5.14p4, 6.5.15p4: where
// either operand decides the jump, && jumping on 0 and || on nonzero,
// each operand jumps to label in the same sense, with no label of its
// own; otherwise the left operand jumps the other way past the right
// one, to a skip label taken before it and placed after the right
// one's jump.  The right operand is reached only where the left one
// leaves the value open.
template< typename S, binary_op_t op, view_t V, typename L, typename R >
    requires logical_operator< op >
void
branch(S &s, const binary_t< op, V, L, R > &b, int label, bool sense)
{
    if (sense != (op == binary_op_t::log_and)) {
        branch(s, b.lhs, label, sense);
        branch(s, b.rhs, label, sense);
        return;
    }

    const int skip = new_label(s);
    branch(s, b.lhs, skip, !sense);
    branch(s, b.rhs, label, sense);
    place_label(s, skip);
}

// && and || as a value, rcc's logical(), 6.5.14p3, 6.5.15p3: no value
// is held in a register across the paths, before_paths(), which spills
// every one and marks each held value until the join; then the labels
// of the 0 path and of the join, taken before either operand; the jump
// on the whole to the 0 path; and on both paths one register, taken
// after the jumps, holding 1 or 0, movl $1, %eax and movl $0, %eax,
// mov w0, #1 and mov w0, #0, never set from the flags.  The join is the
// label past the 0 path.  The register is the lowest free outside the
// reservations, whatever the consumer names, as in rcc.
template< typename S, binary_op_t op, view_t V, typename L, typename R >
    requires logical_operator< op >
value_t
value(S &s, const binary_t< op, V, L, R > &b, unsigned = 0)
{
    const auto incoming = before_paths(s);

    const int no = new_label(s);
    const int end = new_label(s);

    branch(s, b, no, false);

    const value_t v = fresh(s);
    move_immediate(s, reg_of(s, v), 1);
    jump(s, end);

    place_label(s, no);
    move_immediate(s, reg_of(s, v), 0);

    place_label(s, end);

    return v;
}

// An assignment as a value, rcc's assign(): the place first, G13, its
// reservation each target's place_of(), the consumer's register among
// it, then the value held, handed that register, then stored from its
// register at the place's view, V, since the value is erased, and the
// place given back; the register holds the place's value after the
// store, 6.5.17.1p3, the node's value.  So a constant goes through a
// register here, on x86-64 too, and 0 is no store of wzr.
template< typename S, typename W, view_t V >
value_t
value(S &s, const assign_t< W, V > &a, unsigned wanted = 0)
{
    const auto at = place_of(s, a.where, a.value, wanted);
    const value_t v = value(s, a.value, wanted);
    store(s, v, at, V);
    release(s, at);

    return v;
}

// A call, rcc's call(), backend.md chapter 8, as both targets write it
// up to the transfer: every held value spilled first, in the order of
// the registers, since the call destroys every expression register, so
// the arguments find every register free; then each argument, in order
// and complete before the next, G13, stored to its hidden local, whose
// slot is taken at the value's view before its value is generated;
// then the target's transfer(), the arguments moved to where the
// convention passes them and the call made.  Nothing is held when the
// arguments are stored, so rcc's second spill, before the transfer,
// spills nothing.
template< typename S >
void
call(S &s, std::string_view name, const std::vector< argument_t > &args)
{
    spill_all(s);

    for (const argument_t &a : args) {
        allocate(s, a.where, view_of(a.value));
        initialize(s, address(s, a.where), a.value);
    }

    transfer(s, name, args);
}

// A call of a void function has no value, 6.3.2.2p1, and the parser
// asks none of it, require_value(): a void call asked for its value is
// a tree the parser does not make.
template< typename S >
value_t
value(S &, const call_void_t &, unsigned = 0)
{
    throw std::logic_error("v2: the value of a void call");
}

// A call of a void function for its effect is the call, and leaves
// nothing held.
template< typename S >
void
effect(S &s, const call_void_t &c)
{
    call(s, c.name, c.args);
}

// A conditional for its effect, rcc's conditional_effect(), 6.6: no value
// held across the paths, before_paths(); the labels of the second arm
// and of the join, taken before the condition; the condition's jump to
// the second arm where it is 0; each arm for its effect, so that a ?
// (b = 1) : (c = 2); stores each constant in one movl; the jump past
// the second arm.  Two void arms are the same.
template< typename S >
void
conditional_effect(S &s,
                   const expr_t &cond,
                   const expr_t &yes,
                   const expr_t &no)
{
    const auto incoming = before_paths(s);

    const int other = new_label(s);
    const int end = new_label(s);

    branch(s, cond, other, false);
    effect(s, yes);
    jump(s, end);

    place_label(s, other);
    effect(s, no);

    place_label(s, end);
}

template< typename S, view_t V, bool jumps >
void
effect(S &s, const conditional_t< V, jumps > &c)
{
    conditional_effect(s, c.cond, c.yes, c.no);
}

template< typename S >
void
effect(S &s, const conditional_void_t &c)
{
    conditional_effect(s, c.cond, c.yes, c.no);
}

// A void conditional has no value, as a void call has none.
template< typename S >
value_t
value(S &, const conditional_void_t &, unsigned = 0)
{
    throw std::logic_error("v2: the value of a void conditional");
}

// A conditional as a jump where an arm is a truth value, rcc's
// conditional_branch(): jump code, as && and || are, and no join, the
// held values spilled first, before_paths(); the condition's jump to the
// second arm where it is 0, each arm a jump of the same label and sense,
// the first followed by the jump past the second.  The condition and
// the arms are tested without the conversions that keep whether they
// are 0, under_widening()'s.  Of no truth-value arm, the primary, the
// join's value compared with 0.
template< typename S, view_t V >
void
branch(S &s, const conditional_t< V, true > &c, int label, bool sense)
{
    const auto incoming = before_paths(s);

    const int other = new_label(s);
    const int end = new_label(s);

    branch(s, under_widening(c.cond), other, false);
    branch(s, under_widening(c.yes), label, sense);
    jump(s, end);

    place_label(s, other);
    branch(s, under_widening(c.no), label, sense);

    place_label(s, end);
}

// A comma, rcc's binary(), cbranch() and effect() of one, 6.5.18p2: the
// left operand for its effect, then the right operand by the entry
// point the comma was called by, its value handed the consumer's
// register, its jump, or its effect.  A comma takes no label and writes
// nothing of its own.  Its store into a slot is the primary's, the
// value in a register, since rcc stores in place an immediate alone:
// c = (1, 2); is movl $2, %eax and its store.
template< typename S, typename R >
value_t
value(S &s, const comma_t< R > &c, unsigned wanted = 0)
{
    effect(s, c.lhs);

    return value(s, c.rhs, wanted);
}

template< typename S, typename R >
void
effect(S &s, const comma_t< R > &c)
{
    effect(s, c.lhs);
    effect(s, c.rhs);
}

template< typename S, typename R >
void
branch(S &s, const comma_t< R > &c, int label, bool sense)
{
    effect(s, c.lhs);
    branch(s, c.rhs, label, sense);
}

} // namespace rcc
