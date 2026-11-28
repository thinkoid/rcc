// -*- mode: c++; -*-

// The builders, docs/typed-tree.md 3.3: all the parser sees of the
// zoo.  Each takes a construct's parts and returns its node under its
// handle; build.cc defines them, where the node types and the models
// are seen, so a builder is where a part's type becomes the node's.  A
// local is passed as its number among the function's, which the parser
// counts from 0 in the order it declares them, and so is a loop or a
// switch.

#pragma once

#include "expr.hh"
#include "stmt.hh"

#include <optional>
#include <string_view>
#include <vector>

namespace rcc {

// A constant of the view, its value the bits its register holds, read
// signed, a narrow one's its promotion, 6.4.4.2; a local of the view and
// of a type of the sign, its place; and an object of static storage of
// the view and of a type of the sign, by its assembler's name, its
// place, whether the unit defines it or only declares it.
expr_t make_literal(long long value, view_t view);
expr_t make_local(int slot, view_t view, sign_t sign);
expr_t make_global(std::string_view name, view_t view, sign_t sign);

// A local array, 6.7.7.3: its leaf, the address of its slot, its
// decay, 6.3.2.1p3, which is no place; & of the array and of its first
// element are the same node.
expr_t make_local_array(int slot);

// The lvalue conversion, 6.3.2.1p2: a place becomes the value it holds;
// any other expression is a value already, and is returned as it is.
expr_t rvalue(expr_t e);

// The value of a node the builders made a constant (docs/typed-tree.md
// 4.3), as the tree holds a constant, the question of an initializer of
// an object of static storage, 6.7.11p4; none for any other node.
std::optional< long long > constant_of(const expr_t &e);

// Whether an expression is a place whose address & takes, 6.5.4.2p1,
// which make_address_of() takes; and the address, 6.5.4.2p3.
bool is_place(const expr_t &e);
expr_t make_address_of(expr_t place);

// *E of a pointer's value, 6.5.4.2p4: the place it points to, of the
// pointee's view and of a type of the sign.
expr_t make_indirection(expr_t pointer, view_t view, sign_t sign);

// *E of a pointer to an array, 6.5.4.2p4: the array it points to, whose
// node is its decay, the address of the place the pointer points to,
// &*E, so *&a is the array a again.
expr_t make_indirection_to_array(expr_t pointer);

// p + n, n + p and p - n, 6.5.7p9, of a pointer and an index widened to
// 64 bits, the stride the pointee's size in bytes, index_first saying
// the integer is written first; p - q of two pointers to one type,
// 6.5.7p10, a long; and a cast between pointer types, 6.3.2.3p7.
expr_t make_gep(binary_op_t op,
                expr_t pointer,
                expr_t index,
                long stride,
                bool index_first);
expr_t make_difference(expr_t lhs, expr_t rhs, long stride);
expr_t make_bitcast(expr_t pointer);

// Whether an expression is a place an assignment stores into, which
// make_assign() takes, 6.5.17.1p2.
bool is_assignable(const expr_t &e);
expr_t make_assign(expr_t place, expr_t value);

// The conversion of a value of the view from and the sign from_sign,
// its own type's, to the view to and the sign to_sign, 6.3.1.3; the
// parser's conversion() calls it, and nothing else does.
expr_t make_conversion(
    view_t to, sign_t to_sign, view_t from, sign_t from_sign, expr_t e);

// -e ~e !e of an operand converted, 6.5.4.3, at the view of the
// operation, or for ! at the operand's own.
expr_t make_unary(unary_op_t op, view_t view, expr_t arg);

// lhs op rhs of operands converted, 6.5.6 to 6.5.15, at the view of the
// operation.
expr_t make_binary(binary_op_t op, view_t view, expr_t lhs, expr_t rhs);

// An argument of a call as the parser hands it over: its value,
// converted to its parameter's type as if by assignment, 6.5.3.3p6, and
// the sign of that type, which the load of a narrow one into its
// register extends it by (docs/typed-tree.md principle 9).
struct converted_argument_t
{
    expr_t value;
    sign_t sign;
};

// E1 op= E2, 6.5.17.3, of the place E1, which the parser asks
// is_assignable() of first, and the value E2 converted to the
// operation's type, a shift's count promoted on its own; op is the
// operation's by its sign, at the view of the operation.
expr_t make_compound(binary_op_t op, view_t view, expr_t place, expr_t value);

// ++E --E E++ E-- of the place E, 6.5.4.1, 6.5.3.5, which the parser
// asks is_assignable() of first, as of an assignment's.
expr_t make_step(step_op_t op, expr_t place);

// c ? y : n, 6.5.16, of a condition read and of two arms of the view
// and the sign converted to their common type, p6, and of two void
// arms, p3.
expr_t
make_conditional(expr_t cond, expr_t yes, expr_t no, view_t view, sign_t sign);
expr_t make_conditional_void(expr_t cond, expr_t yes, expr_t no);

// lhs, rhs, 6.5.18, of a left operand evaluated for its effect and a
// right operand converted as an operand whose value may be read, the
// comma's own.
expr_t make_comma(expr_t lhs, expr_t rhs);

// A call of the function named, 6.5.3.3, of its arguments, each stored
// in a hidden local of the call, numbered from first: of a function
// whose result is of the view result, and of a void function.
expr_t make_call(std::string_view name,
                 view_t result,
                 int first,
                 std::vector< converted_argument_t > args);
expr_t make_call_void(std::string_view name,
                      int first,
                      std::vector< converted_argument_t > args);

stmt_t make_expr_stmt(expr_t e);
stmt_t make_null();

// A declaration of a local of the view, with an initializer or without,
// and a return of a value of the result's view.
stmt_t make_decl(int slot, view_t view);
stmt_t make_init(int slot, view_t view, expr_t value);
stmt_t make_return(expr_t value, view_t view);
stmt_t make_return_void();
stmt_t make_block(std::vector< stmt_t > items);

// A declaration of a local array of size bytes at the alignment align,
// its element's.
stmt_t make_array_decl(int slot, long size, int align);

// if (cond) then, and if (cond) then else otherwise, 6.8.5.2, of a
// condition converted.
stmt_t make_if(expr_t cond, stmt_t then);
stmt_t make_if_else(expr_t cond, stmt_t then, stmt_t otherwise);

// What the parser knows of a loop or a switch once it has read its body
// (4.4, 4.6), rcc's jump_target_t: its number, counted with the other's,
// since a break leaves either, 6.8.7.4p2, and whether a break and a
// continue of it stand in the body, which picks the labels its row
// places.  A continue passes through a switch, 6.8.7.3p2, so none is a
// switch's.
struct jump_target_t
{
    int id;
    bool broken = false;
    bool continued = false;
};

// while (cond) body, 6.8.6.2, do body while (cond);, 6.8.6.3, and for
// (init; cond; step) body, 6.8.6.4, of a condition converted; a for
// that omits its condition is the overload without one.  The init and
// the step are statements, an expression statement or the null
// statement where the clause is omitted.
stmt_t make_while(expr_t cond, stmt_t body, const jump_target_t &loop);
stmt_t make_do(stmt_t body, expr_t cond, const jump_target_t &loop);
stmt_t make_for(stmt_t init,
                expr_t cond,
                stmt_t step,
                stmt_t body,
                const jump_target_t &loop);
stmt_t
make_for(stmt_t init, stmt_t step, stmt_t body, const jump_target_t &loop);

// A case of a switch as the parser hands it over, rcc's case_t: its
// constant's value, converted to the promoted type of the controlling
// expression, 6.8.5.3p5, and its label's number among the function's
// case and default labels, which the parser counts from 0 in the order
// they are written.
struct case_t
{
    long long value;
    int label;
};

// switch (cond) body, 6.8.5.3, of a controlling expression promoted, of
// the view and the sign of its promoted type, its cases in the order
// they are written, the number of its default label or none, and what
// the parser knows of it once it has read the body; and a case or a
// default label, placed where it is written, by its number.
stmt_t make_switch(expr_t cond,
                   view_t view,
                   sign_t sign,
                   stmt_t body,
                   std::vector< case_t > cases,
                   std::optional< int > default_,
                   const jump_target_t &target);
stmt_t make_case_label(int label);

// break; of the loop or the switch numbered target, and continue; of the
// loop numbered loop, 6.8.7.4, 6.8.7.3.
stmt_t make_break(int target);
stmt_t make_continue(int loop);

// L: and goto L; of the C label numbered label among the function's,
// which the parser counts from 0 in the order it first meets their
// names, 6.8.2, 6.8.7.2.
stmt_t make_label(int label);
stmt_t make_goto(int label);

} // namespace rcc
