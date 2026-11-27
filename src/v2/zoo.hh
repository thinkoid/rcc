// -*- mode: c++; -*-

// The zoo, docs/typed-tree.md chapter 1: the node types under the
// handles.  A node is an aggregate of its parts, each concrete where a
// row's choice depends on it and erased where none does.  The builders
// make them, build.cc; the rows that write them are each target's,
// amd64.hh and arm64.hh, and rows-expressions.hh's and
// rows-statements.hh's for those both targets write alike.

#pragma once

#include "expr.hh"
#include "stmt.hh"

#include <vector>

namespace rcc {

// An integer constant of type int, 6.4.4.2: its row writes 32 bits.
struct literal_t
{
    int value;
};

// The constant 0 of type int, which make_literal() makes of a literal
// whose value is 0 (principle 2): a store of it names the zero register
// where the target has one.
struct zero_t
{
};

// The value of a constant, which a builder asks to fold an operator on
// it (4.3).
inline int
value_of(const literal_t &c)
{
    return c.value;
}

inline int
value_of(const zero_t &)
{
    return 0;
}

// A constant, a node whose value is known: a type value_of() is written
// for (principle 7).
template< typename C >
concept constant = requires(const C &c) { value_of(c); };

// A local, a place, 4.2: the where of an int in the function's frame,
// named by the local's number among the function's.  The displacement
// is the state's, since a slot is taken where its declaration is
// generated.
struct slot_t
{
    int id;
};

// What a place holds, 6.3.2.1p2: the value read from it, which is no
// place.
struct load_t
{
    slot_t where;
};

// -e ~e !e, 6.5.4.3p3 to p5, on an operand that is no constant, which
// the builder folds instead; the operand is erased.
template< unary_op_t op, typename A = expr_t >
struct unary_t
{
    A arg;
};

// L op R, 6.5.6 to 6.5.13, on operands that do not fold, each in the
// class a target's niches read: the builder keeps each a constant, a
// load or erased, the right one a top_t besides, and a row that
// exchanges the operands makes the node of the other order.
template< binary_op_t op, typename L = expr_t, typename R = expr_t >
struct binary_t
{
    L lhs;
    R rhs;
};

// The right operand of a binary operator whose top operator is op, a
// division or a shift by a count that is no constant: the node erased,
// with its own niches, and op kept as the class's parameter, since
// x86-64 generates the left operand away from the registers that
// instruction pins, a fact of the right operand's type (pinned_by()).
// It is a class of an operand and no node: an operator that is the
// identity leaves the node it holds, node_of().
template< binary_op_t op >
struct top_t
{
    expr_t node;
};

// x = e, 6.5.17.2: the value stored in the place, whose value after the
// store is the expression's, 6.5.17.1p3.  The value's type is a
// literal_t's or a zero_t's where a target has a niche for it, else
// erased.
template< typename V = expr_t >
struct assign_t
{
    slot_t where;
    V value;
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

// return E;, 6.8.7.5: the value in the convention's result register,
// then the jump to the function's one epilogue.
struct return_t
{
    expr_t value;
};

// return;, 6.8.7.5, which stands in a function whose result type is
// void alone, p1: the jump to the one epilogue.  LLVM's ret void.
struct return_void_t
{
};

// int x;, a declaration without an initializer: the local's slot taken,
// and nothing stored, 6.2.4p6.
struct decl_t
{
    slot_t where;
};

// int x = e;, a declaration with an initializer, 6.7.11p12: the slot
// taken, then the value stored in it.  An initialization, not an
// assignment (4.5); the value's type is a literal_t's or a zero_t's
// where a target has a niche for it, else erased.
template< typename V = expr_t >
struct init_t
{
    slot_t where;
    V value;
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
// the function's, which names it to the jumps of its body, jump_t;
// its row records its labels under it.
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

// The two labels a loop takes for the jumps of its body, which its
// row records in the state under the loop's number before the body is
// generated: a break's, past the loop, and a continue's, the loop's
// continuation.
struct loop_labels_t
{
    int break_;
    int continue_;
};

// break; and continue;, 6.8.7.4p2, 6.8.7.3p2: a jump to one of the two
// labels of the innermost loop around it, which the parser found, by
// the loop's number; label says which of the two.
struct jump_t
{
    int loop;
    int loop_labels_t::*label;
};

// A statement that is one jump: a type that has the row which names the
// label it jumps to, label_of(), on both targets, the category the row
// defines (principle 7).  A break and a continue are, and so is a
// return without an expression, whose label is the epilogue's.  An if
// whose then part is one jump is the condition's jump to that label
// (4.4).
template< typename T >
concept one_jump =
    requires(const amd64::state_t &a, const arm64::state_t &b, const T &t) {
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

} // namespace rcc
