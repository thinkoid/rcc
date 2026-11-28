// -*- mode: c++; -*-

// The zoo, docs/typed-tree.md chapter 1: the node types under the
// handles.  A node is an aggregate of its parts, each concrete where a
// row's choice depends on it and erased where none does, and every
// value node has a view, the width of its value (4.2).  The builders
// make them, build.cc; the rows that write them are each target's,
// amd64.hh and arm64.hh, and rows-expressions.hh's and
// rows-statements.hh's for those both targets write alike.  The two
// summaries both targets read, the view and the width rule, are here,
// beside the nodes they are of, and so is the arithmetic of a switch's
// cases, which its builder and its rows read.  A switch holds its cases
// as the parser hands them over, build.hh's case_t.

#pragma once

#include "build.hh"
#include "expr.hh"
#include "stmt.hh"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace rcc {

// An integer constant of view V, 6.4.4.2, whose value a sign-extended
// 32-bit immediate holds: every constant of 32 bits or less, and one of
// 64 in int's range.  The value is what its register holds, read as a
// signed number of 32 bits or V's, a narrow constant's promotion, so no
// constant needs its sign (4.3).
template< view_t V >
struct literal_t
{
    long long value;
};

// The constant 0 of view V, which make_literal() makes of a literal
// whose value is 0 (principle 2): a store of it names the zero register
// where the target has one.
template< view_t V >
struct zero_t
{
};

// A constant of 64 bits past int's range, which make_literal() makes of
// such a value (principle 2): no 32-bit immediate holds it, so x86-64
// writes it by movabsq into a register, where the source has it.
struct wide_t
{
    long long value;
};

// The value of a constant, which a builder asks to fold an operator on
// it (4.3): the bits its register holds, read signed.
template< view_t V >
long long
value_of(const literal_t< V > &c)
{
    return c.value;
}

template< view_t V >
long long
value_of(const zero_t< V > &)
{
    return 0;
}

inline long long
value_of(const wide_t &c)
{
    return c.value;
}

// A constant, a node whose value is known: a type value_of() is written
// for (principle 7).
template< typename C >
concept constant = requires(const C &c) { value_of(c); };

// The where of a local, 4.2: its number among the function's locals.
// The displacement is the state's, since a slot is taken where its
// declaration is generated; a use reads it by the number.
struct slot_t
{
    int id;
};

// The where of an object of static storage, 4.2: its bytes in the
// unit's data, named by the assembler's name.  Whether the unit defines
// it is the unit's, asked by the where's address at generation, each
// target's address(): a slot with another spelling where it does, its
// label, and the address the GOT holds where it only declares it
// (backend.md chapter 10).
struct global_t
{
    std::string name;
};

// A place, 4.2: the where W of an object of view V and of a type of the
// sign S, a local's slot in the function's frame or a global, its
// object's sign kept since rcc reads a narrow object by an extending
// load of its own sign whatever consumes it.  A parameter is a local, of
// its type's view and sign.
template< typename W, view_t V, sign_t S >
struct place_t
{
    W where;
};

// What a place holds, 6.3.2.1p2: the object read at its view, which is
// no place, extended to 32 bits by S where V is narrow; a widening's
// builder may read a where at another sign, the type it converts from.
template< typename W, view_t V, sign_t S >
struct load_t
{
    W where;
};

// -e ~e !e, 6.5.4.3p3 to p5, on an operand that is no constant, which
// the builder folds instead; the operand is erased.  V is the view of
// the operation for - and ~, the promoted operand's, and for ! the view
// of the operand, which a target compares with 0 at.
template< unary_op_t op, view_t V, typename A = expr_t >
struct unary_t
{
    A arg;
};

// L op R, 6.5.6 to 6.5.15, on operands that do not fold, each in the
// class a target's niches read: the builder keeps each a constant, a
// load or erased, the right one a top_t besides, and a row that
// exchanges the operands makes the node of the other order.  V is the
// view of the operation: the operands' common type's, the promoted left
// operand's for a shift, whose count keeps its own, and an int's for &&
// and ||, whose operands keep theirs.
template< binary_op_t op, view_t V, typename L = expr_t, typename R = expr_t >
struct binary_t
{
    L lhs;
    R rhs;
};

// The right operand of a binary operator whose top operator is of the
// family F, a division or a shift by a count that is no constant: the
// node erased, with its own niches, and the family kept as the class's
// parameter, since x86-64 generates the left operand away from the
// registers that instruction pins, a fact of the right operand's type
// (pinned_by()).  It is a class of an operand and no node: an operator
// that is the identity leaves the node it holds, node_of().
template< family_t F >
struct top_t
{
    expr_t node;
};

// The conversion of a value of view F to the wider view V by the sign
// of its type, 6.3.1.3: a sign extension, LLVM's sext, of a signed one,
// and a zero extension, zext, of an unsigned one, each where the
// operand's register is not valid to V's width already (the width rule,
// valid_of()).  A is erased, or the operand's load, whose niche is the
// extending load of a signed one, or the promotion of a load, over
// which the widening is one load, rcc's widened_read(), or a top_t,
// pinning()'s.
template< view_t V, view_t F, typename A = expr_t >
struct sext_t
{
    A arg;
};

template< view_t V, view_t F, typename A = expr_t >
struct zext_t
{
    A arg;
};

// The conversion of a value to the view V that is no widening, 6.3.1.3:
// a narrowing, a change of the sign alone, or a cast to the type the
// operand has.  It writes nothing, since the low bits of the register
// are the value (G2), and leaves the value valid to V's width.  LLVM's
// trunc only narrows, and its bitcast keeps the width; one node serves
// the three, since their rows are one.  A is erased, or the operand's
// load, so that a parent may name the object in place, or a top_t,
// pinning()'s.
template< view_t V, typename A = expr_t >
struct trunc_t
{
    A arg;
};

// x = e, 6.5.17.2: the value stored in the place of view V at the where
// W, whose value after the store is the expression's, 6.5.17.1p3.  The
// value is erased, converted to the place's type by the parser, so its
// view is V: its store is a row of its handle, which finds a target's
// niche.
template< typename W, view_t V >
struct assign_t
{
    W where;
    expr_t value;
};

// An argument of a call, 6.5.3.3p4: its value, converted to its
// parameter's type, so of that type's view; the hidden local of the
// call it is stored in, rcc's argument slot (backend.md chapter 8),
// numbered by the parser among the function's locals, whose slot is
// taken where the call is generated, before the value is, of the
// value's view; and the sign of the parameter's type, which the load
// of a narrow one from the slot into its register extends it by.  The
// value is erased, since the count of arguments is not bounded, and its
// store is a row of its handle, which finds a target's niche.
struct argument_t
{
    slot_t where;
    sign_t sign;
    expr_t value;
};

// f(a, b), 6.5.3.3: a call of the function named, whose result is of
// view V, LLVM's call i32 @f().
template< view_t V >
struct call_t
{
    std::string name;
    std::vector< argument_t > args;
};

// A call of a void function, which has no value, 6.3.2.2p1, and only
// an effect: LLVM's call void @f().
struct call_void_t
{
    std::string name;
    std::vector< argument_t > args;
};

// l, r, 6.5.18: the left operand evaluated as a void expression, then
// the right one, whose type and value are the result's, p2.  The left
// is erased; the right is kept in its class, a constant, a load or
// erased, as a binary operator keeps an operand, since x86-64's rows
// read a comma on their right through to that class, rcc's
// operand_of().  Nothing folds it, 6.6p3.
template< typename R = expr_t >
struct comma_t
{
    expr_t lhs;
    R rhs;
};
// c ? y : n, 6.5.16: the first operand compared with 0, and the second
// or the third evaluated, never both, p5; the value the evaluated one's,
// of the arms' common type, p6, whose view is V, and no lvalue, its
// footnote.  jumps says whether an arm is a truth value, truth_value()'s,
// which makes the jump on the whole rcc's jump code on the arms,
// conditional_branch(); the builder asks it of the arms.  The parts are
// erased: each is reached through its own entry points.  The sign of
// the arms' type is data, which the width rule of the join reads, the
// row's record of what the join is valid to (register-stack.hh).
template< view_t V, bool jumps >
struct conditional_t
{
    expr_t cond;
    expr_t yes;
    expr_t no;
    sign_t sign;
};

// c ? v() : w(), two void arms, 6.5.16p3, p6: an effect, and no value,
// which the parser asks of no void expression, require_value().
struct conditional_void_t
{
    expr_t cond;
    expr_t yes;
    expr_t no;
};


// E1 op= E2, 6.5.17.3: E1 evaluated once, and the operation one
// evaluation, p4, after E2's, so the object is read after the right
// operand, as rcc reads it.  The place is its where, a local's slot or
// a global, its object's view, V, and the sign of its object's type, by
// which its read is extended to the operation's view, O, the view of
// the common type of E1 and E2, p3, or of E1's promoted type for <<=
// and >>=, the result stored at V, G2.  O is a parameter, since x86-64
// takes the object as an instruction's operand where O is V; the sign
// is data, read by the read's extension and by x86-64's sar or shr on
// the object in memory, and no row is picked by it.  The value,
// converted to O by the parser, a shift's count promoted on its own, is
// kept a constant of O or erased, since x86-64's memory forms and
// aarch64's rows take an immediate.  It is no E1 = E1 op E2: nothing
// folds it, 6.6p3, and no identity drops it, so a += 0 adds 0, as in
// rcc.
template< binary_op_t op, typename W, view_t V, view_t O, typename R = expr_t >
struct compound_t
{
    W where;
    sign_t sign;
    R value;
};

// ++E --E E++ E--, 6.5.4.1, 6.5.3.5: the object of view V and of a
// type of the sign S stepped by 1 and stored, the value the new one,
// prefix, or the old one, postfix, p2.  The place is its where alone, a
// local's slot or a global, as an assignment's; it is no E += 1, as in
// rcc, so x86-64's postfix form copies the old value, and nothing folds
// it, 6.6p3.
template< step_op_t op, typename W, view_t V, sign_t S >
struct step_t
{
    W where;
};

// e;, 6.8.4p2: the expression evaluated for its effect, its value
// dropped.
struct expr_stmt_t
{
    expr_t e;
};

// ;, the null statement, 6.8.4p3, which writes nothing.
struct null_t
{
};

// return E;, 6.8.7.5: the value, of the result's view V, in the
// convention's result register, then the jump to the function's one
// epilogue.  LLVM's ret i32.
template< view_t V >
struct return_t
{
    expr_t value;
};

// return;, 6.8.7.5, which stands in a function whose result type is
// void alone, p1: the jump to the one epilogue.  LLVM's ret void.
struct return_void_t
{
};

// T x;, a declaration without an initializer of a local of view V: the
// local's slot taken, and nothing stored, 6.2.4p6.
template< view_t V >
struct decl_t
{
    slot_t where;
};

// T x = e;, a declaration with an initializer, 6.7.11p12: the slot
// taken, then the value stored in it.  An initialization, not an
// assignment (4.5); the value is erased, as an assignment's is.
template< view_t V >
struct init_t
{
    slot_t where;
    expr_t value;
};

// A compound statement, 6.8.3: its items in order, erased, since their
// count is not bounded (4.4).
struct block_t
{
    std::vector< stmt_t > items;
};

// while (e) s, 6.8.6.2: the condition at the head, as a jump past the
// loop where it is 0, before each execution of the body, 6.8.6.2p1,
// then the body and the jump back.  loop is the loop's number among
// the function's, which names it to the jumps of its body, break_t and
// continue_t; its row records its labels under it.
struct while_t
{
    expr_t cond;
    stmt_t body;
    int loop;
};

// do s while (e);, 6.8.6.3: the body, then the condition as a jump
// back to it where it is nonzero, after each execution of the body,
// 6.8.6.3p1.  broken and continued say whether a break and a continue
// of the loop stand in its body, which the parser knows when it has
// read the body: the label past the test is placed only where a break
// names it, and the test's own where a continue does.
template< bool broken, bool continued >
struct do_t
{
    stmt_t body;
    expr_t cond;
    int loop;
};

// The condition a for omits, 6.8.6.4p2, replaced by a nonzero constant:
// no test is written for it, so the label past the loop is named by a
// break alone.
struct omitted_t
{
};

// for (init; cond; step) s, 6.8.6.4: the init, the condition at the
// head as a jump past the loop where it is 0, the body, then the step
// and the jump back.  C is expr_t, or omitted_t for a condition the
// for omits; an omitted init or step is the null statement, which
// writes nothing.  broken and continued are do_t's: the step's label is
// placed only where a continue names it.
template< typename C, bool broken, bool continued >
struct for_t
{
    stmt_t init;
    C cond;
    stmt_t step;
    stmt_t body;
    int loop;
};

// The forms of a switch's dispatch, rcc's dispatch_t, backend.md 7.3,
// which the switch's builder chooses from the values of its cases by the
// rule of the Seventh Edition compiler: none, for a switch of no case;
// direct, a jump through a table indexed by the value; simple, a test of
// each case in turn; hash, a jump through a table of buckets by the
// value's remainder, then a test of each case of the bucket.
enum class dispatch_t { none, direct, simple, hash };

// switch (e) s, 6.8.5.3: the controlling expression's value, then the
// dispatch of the form the builder chose, then the body, in which each
// case and default label is placed where it is written, case_label_t,
// and the label past the body, which a break of the switch jumps to and
// which stands for the default where there is none.  V is the view of
// the controlling expression's promoted type, p5, every instruction of
// the dispatch's; sign is that type's sign, data, which x86-64's
// extension of a direct table's index reads the width rule at, and no
// row is picked by.  defaulted says whether the switch has a default
// label and broken whether a break of it stands in its body, which the
// parser knows when it has read the body: with a default and no break
// nothing names the label past the body, which is not placed.  cases
// are sorted by value in the order of the promoted type, the builder's
// order, which the dispatch tests them in; default_ is the default
// label's number; id is the switch's number among the function's loops
// and switches, which names it to its body's breaks.
template< dispatch_t form, view_t V, bool defaulted, bool broken >
struct switch_t
{
    expr_t cond;
    sign_t sign;
    stmt_t body;
    std::vector< case_t > cases;
    std::optional< int > default_;
    int id;
};

// case K: and default:, 6.8.2p1: the label of a switch's case or default
// placed where it is written, by its number among the function's case
// and default labels, which its switch's row took a label for.
struct case_label_t
{
    int label;
};

// The bits of a value of the view read as an unsigned number, rcc's
// unsigned_bits(): the (unsigned) of the rule's hash, backend.md 7.3.
inline unsigned long long
unsigned_bits(long long value, view_t view)
{
    const auto bits = static_cast< unsigned long long >(value);
    if (view == view_t::i64)
        return bits;

    return static_cast< std::uint32_t >(bits);
}

// A value less the first of a switch's sorted cases, as an unsigned
// number of the view, rcc's case_offset(): its entry in a direct table.
inline unsigned long long
case_offset(long long value, long long first, view_t view)
{
    const auto difference = static_cast< unsigned long long >(value) -
                            static_cast< unsigned long long >(first);

    return unsigned_bits(static_cast< long long >(difference), view);
}

// The last value of a switch's sorted cases less the first, modulo 2^N
// at the view, the range of the rule of the Seventh Edition compiler,
// rcc's case_range().
inline unsigned long long
case_range(const std::vector< case_t > &sorted, view_t view)
{
    return case_offset(sorted.back().value, sorted.front().value, view);
}

// break;, 6.8.7.4p2: a jump past the innermost loop or switch around
// it, which the parser found, by that statement's number, to the label
// its row recorded for its breaks.
struct break_t
{
    int target;
};

// continue;, 6.8.7.3p2: a jump to the continuation of the innermost
// loop around it, past any switch between, by the loop's number, to the
// label the loop's row recorded for its continues.
struct continue_t
{
    int loop;
};

// L: s, 6.8.2: the C label placed where it is written.  A label's name
// has function scope, 6.2.1p3, so the parser gives it a number among
// the function's C labels at its first meeting, a goto or its
// definition, and the node names it by the number; s is a statement of
// its own after it.
struct label_t
{
    int label;
};

// goto L;, 6.8.7.2: the jump to the C label numbered label.
struct goto_t
{
    int label;
};

// A statement that is one jump: a type that has the row which names the
// label it jumps to, label_of(), on both targets, the category the row
// defines (principle 7).  A break, a continue and a goto are, and so is
// a return without an expression, whose label is the epilogue's.  An if
// whose then part is one jump is the condition's jump to that label
// (4.4).  A goto's row takes its label where the generation first meets
// it, so the category asks the row of a state it may write.
template< typename T >
concept one_jump = requires(amd64::state_t &a, arm64::state_t &b, const T &t) {
    label_of(a, t);
    label_of(b, t);
};

// if (e) s, 6.8.5.1p1: the condition as a jump past the then part where
// it is 0, 6.8.5.2p2.  The condition is erased, since its own row is
// its best jump (3.4).  The then part is erased, or its own type where
// it is one jump, one_jump's, bare or the one item of a block: the
// condition jumps to the jump's label where it is nonzero, and nothing
// else is written.
template< typename S = stmt_t >
struct if_t
{
    expr_t cond;
    S then;
};

// if (e) s1 else s2, 6.8.5.1p1: the condition as a jump to the else
// part where it is 0, the then part, and a jump past the else part
// where a path leaves the then part at its end, 6.8.5.2p2.  A then part
// that is one jump is of its own type, as for if_t, and the else part
// follows the condition's jump.
template< typename S = stmt_t >
struct if_else_t
{
    expr_t cond;
    S then;
    stmt_t otherwise;
};

// The view of a value node, the width of its value (4.2): a leaf's and
// a conversion's V, an operator's V but an int's for a comparison, ! and
// && and ||, an assignment's place's, a call's result's.  A summary both
// targets read where a row's own type does not say the width: the store
// of a value, at the value's view, the place's (3.4); a call, for an
// argument's slot; x86-64's shift, for the count's move into rcx; and
// the test of a value with 0.
template< view_t V >
constexpr view_t
view_of(const literal_t< V > &)
{
    return V;
}

template< view_t V >
constexpr view_t
view_of(const zero_t< V > &)
{
    return V;
}

constexpr view_t
view_of(const wide_t &)
{
    return view_t::i64;
}

template< typename W, view_t V, sign_t S >
constexpr view_t
view_of(const place_t< W, V, S > &)
{
    return V;
}

template< typename W, view_t V, sign_t S >
constexpr view_t
view_of(const load_t< W, V, S > &)
{
    return V;
}

template< unary_op_t op, view_t V, typename A >
constexpr view_t
view_of(const unary_t< op, V, A > &)
{
    return op == unary_op_t::log_not ? view_t::i32 : V;
}

template< binary_op_t op, view_t V, typename L, typename R >
constexpr view_t
view_of(const binary_t< op, V, L, R > &)
{
    return is_comparison(op) || is_logical(op) ? view_t::i32 : V;
}

template< family_t F >
view_t
view_of(const top_t< F > &t)
{
    return view_of(t.node);
}

template< view_t V, view_t F, typename A >
constexpr view_t
view_of(const sext_t< V, F, A > &)
{
    return V;
}

template< view_t V, view_t F, typename A >
constexpr view_t
view_of(const zext_t< V, F, A > &)
{
    return V;
}

template< view_t V, typename A >
constexpr view_t
view_of(const trunc_t< V, A > &)
{
    return V;
}

template< typename W, view_t V >
constexpr view_t
view_of(const assign_t< W, V > &)
{
    return V;
}

template< view_t V >
constexpr view_t
view_of(const call_t< V > &)
{
    return V;
}

// A void call has no value, and the parser asks none of it: a void
// call asked for its view is a tree the parser does not make.
inline view_t
view_of(const call_void_t &)
{
    throw std::logic_error("v2: the view of a void call");
}

template< binary_op_t op, typename W, view_t V, view_t O, typename R >
constexpr view_t
view_of(const compound_t< op, W, V, O, R > &)
{
    return V;
}

template< step_op_t op, typename W, view_t V, sign_t S >
constexpr view_t
view_of(const step_t< op, W, V, S > &)
{
    return V;
}

template< view_t V, bool jumps >
constexpr view_t
view_of(const conditional_t< V, jumps > &)
{
    return V;
}

inline view_t
view_of(const conditional_void_t &)
{
    throw std::logic_error("v2: the view of a void conditional");
}

// A comma's value is its right operand's, and so is its view.
template< typename R >
view_t
view_of(const comma_t< R > &c)
{
    return view_of(c.rhs);
}

// The width rule, rcc's held_t::valid (c1-common.hh): the width up to
// which the bits of a node's register are its value extended by sign,
// read at the sign of the type its consumer converts it from; a summary
// of the subtree (principle 3) that the state does not record.  Its
// readers are the widening's rows, which write the extension where the
// summary is below the new width, as rcc's extended() does.  rcc sets
// it at each write by the sign of the value's own type, written(); here
// a node has no sign, and the rule rests on one invariant: the parser
// builds every conversion from its operand's own C type, in one
// function, conversion(), so a node that is no conversion has the type
// its consumer converts from, and its sign is the one the consumer
// hands.  An instruction that writes a register at a width leaves it
// written() at that width, a load and a constant at theirs, an
// operation at its own; a truth value, 0 or 1, is valid to 64 bits; a
// conversion caps the width at its own, rcc's converted(); an
// assignment's value is its right operand's, and a call's result is
// valid to its view's width, the convention leaving the bits above to
// the caller.

// rcc's written(): a write at 32 bits clears the upper half of the
// register, which is the extension of an unsigned value and not of a
// signed one.
constexpr int
written(int width, sign_t sign)
{
    return width == 32 && sign == sign_t::unsigned_ ? 64 : width;
}

// The width of the register a value of the view is held in, which a
// load writes and a constant's row: its view's, or 32 bits for a narrow
// one, which the register holds promoted.
constexpr int
promoted_width(view_t view)
{
    return width(view) < 32 ? 32 : width(view);
}

template< typename W, view_t V, sign_t S >
int
valid_of(const load_t< W, V, S > &, sign_t sign)
{
    return written(promoted_width(V), sign);
}

template< constant C >
int
valid_of(const C &c, sign_t sign)
{
    return written(promoted_width(view_of(c)), sign);
}

template< unary_op_t op, view_t V, typename A >
int
valid_of(const unary_t< op, V, A > &, sign_t sign)
{
    return op == unary_op_t::log_not ? 64 : written(width(V), sign);
}

template< binary_op_t op, view_t V, typename L, typename R >
int
valid_of(const binary_t< op, V, L, R > &, sign_t sign)
{
    return is_comparison(op) || is_logical(op) ? 64 : written(width(V), sign);
}

template< family_t F >
int
valid_of(const top_t< F > &t, sign_t sign)
{
    return valid_of(t.node, sign);
}

template< view_t V, view_t F, typename A >
int
valid_of(const sext_t< V, F, A > &, sign_t)
{
    return width(V);
}

// The one case where a conversion's own width is not the rule, rcc's
// (long) (unsigned) c in converted(): a signed narrow object read by its
// extending load at 32 bits is a write at 32, so it is valid to 64
// bits where the type its consumer reads it at is unsigned, u = c on an
// unsigned u, and to 32 where it is signed.  The same extension in a
// register, of a narrowed value, is valid to 32 alone.
template< view_t F, typename W >
int
valid_of(const sext_t< view_t::i32, F, load_t< W, F, sign_t::signed_ > > &,
         sign_t sign)
{
    return written(32, sign);
}

template< view_t V, view_t F, typename A >
int
valid_of(const zext_t< V, F, A > &, sign_t)
{
    return width(V);
}

// A widening over a promotion of the object is the one load of the
// object at its own type, rcc's widened_read(), and is valid as the
// widening of the object itself is: (unsigned long) (int) c on a signed
// char c to 64 bits, (int) (short) c to 32 at a signed consumer.
template< view_t V, view_t F, view_t G, typename W >
int
valid_of(
    const sext_t< V, F, sext_t< F, G, load_t< W, G, sign_t::signed_ > > > &e,
    sign_t sign)
{
    return valid_of(
        sext_t< V, G, load_t< W, G, sign_t::signed_ > > { e.arg.arg }, sign);
}

template< view_t V, view_t F, view_t G, typename W >
int
valid_of(
    const sext_t< V, F, zext_t< F, G, load_t< W, G, sign_t::unsigned_ > > > &e,
    sign_t sign)
{
    return valid_of(
        zext_t< V, G, load_t< W, G, sign_t::unsigned_ > > { e.arg.arg }, sign);
}

template< view_t V, view_t F, view_t G, typename W >
int
valid_of(
    const zext_t< V, F, zext_t< F, G, load_t< W, G, sign_t::unsigned_ > > > &e,
    sign_t sign)
{
    return valid_of(
        zext_t< V, G, load_t< W, G, sign_t::unsigned_ > > { e.arg.arg }, sign);
}

// A widening over a change of sign of a load is the one load of the
// object at the type converted from, and is valid as that load is; so is
// a widening over its promotion, as over the promotion of the load.
template< view_t V, view_t F, typename W >
int
valid_of(
    const sext_t< V, F, trunc_t< F, load_t< W, F, sign_t::unsigned_ > > > &e,
    sign_t sign)
{
    return valid_of(
        sext_t< V, F, load_t< W, F, sign_t::signed_ > > { { e.arg.arg.where } },
        sign);
}

template< view_t V, view_t F, typename W >
int
valid_of(const zext_t< V, F, trunc_t< F, load_t< W, F, sign_t::signed_ > > > &e,
         sign_t sign)
{
    return valid_of(
        zext_t< V, F, load_t< W, F, sign_t::unsigned_ > > {
            { e.arg.arg.where } },
        sign);
}

template< view_t V, view_t F, view_t G, typename W >
int
valid_of(
    const sext_t<
        V,
        F,
        sext_t< F, G, trunc_t< G, load_t< W, G, sign_t::unsigned_ > > > > &e,
    sign_t sign)
{
    return valid_of(
        sext_t< V, G, trunc_t< G, load_t< W, G, sign_t::unsigned_ > > > {
            e.arg.arg },
        sign);
}

template< view_t V, view_t F, view_t G, typename W >
int
valid_of(const sext_t<
             V,
             F,
             zext_t< F, G, trunc_t< G, load_t< W, G, sign_t::signed_ > > > > &e,
         sign_t sign)
{
    return valid_of(
        zext_t< V, G, trunc_t< G, load_t< W, G, sign_t::signed_ > > > {
            e.arg.arg },
        sign);
}

template< view_t V, view_t F, view_t G, typename W >
int
valid_of(const zext_t<
             V,
             F,
             zext_t< F, G, trunc_t< G, load_t< W, G, sign_t::signed_ > > > > &e,
         sign_t sign)
{
    return valid_of(
        zext_t< V, G, trunc_t< G, load_t< W, G, sign_t::signed_ > > > {
            e.arg.arg },
        sign);
}

template< view_t V, typename A >
int
valid_of(const trunc_t< V, A > &, sign_t)
{
    return width(V);
}

template< typename W, view_t V >
int
valid_of(const assign_t< W, V > &a, sign_t sign)
{
    return valid_of(a.value, sign);
}

template< view_t V >
int
valid_of(const call_t< V > &, sign_t)
{
    return width(V);
}

// A compound assignment's value is the store's source, which the
// operation wrote at O, rcc's stored_valid(): valid as that write
// leaves it where O is the object's view, and cut to the object's width
// where O is wider, so that a widening consumer writes the extension of
// the stored bits, movb %al, -1(%rbp) and movsbq %al, %rax for c += l.
template< binary_op_t op, typename W, view_t V, view_t O, typename R >
int
valid_of(const compound_t< op, W, V, O, R > &, sign_t sign)
{
    if (O == V)
        return written(width(V), sign);

    return width(V);
}

// A prefix step's value is the store's source, which an operation at
// the object's promoted width wrote, rcc's stored_valid(): valid as
// that write leaves it where the width is the object's, and cut to the
// object's width where it is wider, so that a widening consumer writes
// the extension of the stored bits, movb %al, -1(%rbp) and movsbl %al,
// %eax for ++c.  A postfix step's value is the old one, valid as its
// load left it, 32 bits for a signed char and 64 for an unsigned one.
template< step_op_t op, typename W, view_t V, sign_t S >
int
valid_of(const step_t< op, W, V, S > &, sign_t sign)
{
    if (is_postfix(op))
        return written(promoted_width(V), sign);

    return written(width(V), sign);
}

// A comma's value is its right operand's, in the register that
// operand's row left it in.
template< typename R >
int
valid_of(const comma_t< R > &c, sign_t sign)
{
    return valid_of(c.rhs, sign);
}
// A conditional's value is held where the join holds it, valid to the
// least of what its arms leave, as rcc's join holds it.  An arm a mov
// brought there is valid as the mov wrote it, which no summary knows
// before the arms are written, so the conditional's row records the
// join's width in the state, which a reader of the value asks first,
// held_valid() (register-stack.hh), and this summary serves a reader
// that holds no value of it.
template< view_t V, bool jumps >
int
valid_of(const conditional_t< V, jumps > &c, sign_t sign)
{
    return std::min(valid_of(c.yes, sign), valid_of(c.no, sign));
}

inline int
valid_of(const conditional_void_t &, sign_t)
{
    throw std::logic_error("v2: the value of a void conditional");
}

// Whether a node is a truth value, rcc's truth_value(): its value 0 or 1
// set from a compare or a jump, a comparison, !, && or ||, seen through
// the conversions that keep whether a value is 0, rcc's
// under_widening(), an integer converted to one as wide or wider.  A
// summary the builder of ?: asks of each arm through its handle
// (principle 3), and no category, since the conversion erases its
// operand.
template< typename T >
bool
truth_value(const T &)
{
    return false;
}

template< binary_op_t op, view_t V, typename L, typename R >
bool
truth_value(const binary_t< op, V, L, R > &)
{
    return is_comparison(op) || is_logical(op);
}

template< view_t V, typename A >
bool
truth_value(const unary_t< unary_op_t::log_not, V, A > &)
{
    return true;
}

template< family_t F >
bool
truth_value(const top_t< F > &t)
{
    return truth_value(t.node);
}

template< view_t V, view_t F, typename A >
bool
truth_value(const sext_t< V, F, A > &e)
{
    return truth_value(e.arg);
}

template< view_t V, view_t F, typename A >
bool
truth_value(const zext_t< V, F, A > &e)
{
    return truth_value(e.arg);
}

template< view_t V, typename A >
bool
truth_value(const trunc_t< V, A > &t)
{
    return width(V) >= width(view_of(t.arg)) && truth_value(t.arg);
}


// Whether a node's value is the join of ?: as the join holds it, so that
// the conditional's record of the join's width is the node's: the
// conditional, and a comma and an assignment, whose value is their
// right operand's and their value's, in the same register (3.4).  A
// summary its readers ask through the handle, held_valid()'s; any other
// node writes a value of its own, or one the join's width is not of,
// a conversion's.
template< typename T >
constexpr bool
is_join(const T &)
{
    return false;
}

template< view_t V, bool jumps >
constexpr bool
is_join(const conditional_t< V, jumps > &)
{
    return true;
}

template< typename R >
bool
is_join(const comma_t< R > &c)
{
    return is_join(c.rhs);
}

template< typename W, view_t V >
bool
is_join(const assign_t< W, V > &a)
{
    return is_join(a.value);
}

// A place is read only through its load, and a void call has no value:
// either asked for its width is a tree the parser does not make.
template< typename W, view_t V, sign_t S >
int
valid_of(const place_t< W, V, S > &, sign_t)
{
    throw std::logic_error("v2: a place read without its lvalue conversion");
}

inline int
valid_of(const call_void_t &, sign_t)
{
    throw std::logic_error("v2: the value of a void call");
}

} // namespace rcc
