// -*- mode: c++; -*-

// x86-64, docs/backend.md: the registers, the state of a function's
// generation, the rows of the zoo's types this target writes itself,
// and the unit's entry.  The rows are declared here for build.cc, whose
// models call them, and defined in amd64.cc, which reaches the tree
// only through a handle's row.

#pragma once

#include "register-stack.hh"
#include "targets.hh"
#include "unit.hh"
#include "zoo.hh"

#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

namespace rcc::amd64 {

// The registers, chapter 4: the nine expression registers, the
// caller-saved set, in the order a new value takes them, rax first.
enum reg_t : int { rax, rcx, rdx, rsi, rdi, r8, r9, r10, r11 };

// The state of one function's generation, which the function's row
// builds fresh.  The body is a buffer, which the frame is written
// around; the function's name makes its labels, which are counted per
// function, and ret is the one epilogue's.  terminated says that no
// path reaches what is written next: the last instruction written is
// an unconditional jump, which jump() sets, so main writes no return of
// 0 (backend.md 7.2).  frame is the bytes the slots take below rbp, and
// slots each local's displacement off rbp, by its number.
struct state_t
{
    std::ostringstream body { };
    std::string function;
    int labels = 0;
    int ret = 0;
    bool terminated = false;

    long frame = 0;
    std::unordered_map< int, int > slots { };

    registers_t< reg_t > registers { };
};

// A register for a new value, register-stack.hh's.
reg_t take(state_t &);

// The next slot of the frame, the local's.
void allocate(state_t &, slot_t where);

// The constant rows of an int: its 32 bits in a register taken for it.
value_t value(state_t &, const literal_t &);
value_t value(state_t &, const zero_t &);

// The load row: the int a slot holds, in a register taken for it.
value_t value(state_t &, const load_t &);

// - or ~ on r in place, at 32 bits.
void operate(state_t &, unary_op_t op, reg_t r);

// The operation of op into r from an operand at 32 bits: a register,
// the int in a slot, or an immediate as immediate_of() writes it.
void operate(state_t &, binary_op_t op, reg_t r, reg_t source);
void operate(state_t &, binary_op_t op, reg_t r, slot_t source);
void operate(state_t &, binary_op_t op, reg_t r, std::string_view immediate);

// The store rows, an operand into a slot at 32 bits: a register, or an
// immediate as immediate_of() writes it.
void store(state_t &, reg_t r, slot_t where);
void store(state_t &, std::string_view immediate, slot_t where);

// A constant as the immediate operand of an instruction, 32 bits.
std::string immediate_of(const literal_t &);
std::string immediate_of(const zero_t &);

// A value an instruction takes as its immediate: a type immediate_of()
// is written for (principle 7).
template< typename V >
concept immediate = requires(const V &e) { immediate_of(e); };

// The niche of initialize(), chapter 6.6: an immediate is stored by
// one mov into the slot, and no register is held.
template< immediate V >
void
initialize(state_t &s, slot_t where, const V &e)
{
    store(s, immediate_of(e), where);
}

// Whether a node with the constant L on the left of op is written with
// its operands exchanged, rcc's constant_first() as ordered() asks it:
// op commutes, and the constant is an immediate, as every int is.
template< binary_op_t op, typename L >
concept constant_first = commutes(op) && immediate< L >;

// The niches of + - & | ^, chapter 6.1, over the primary of
// rows-expressions.hh.  A constant on the left of an operator that
// commutes is the row of the node with its operands exchanged, 5 + a
// as a + 5: the value of the right operand, which is no constant, then
// the constant as the immediate.
template< binary_op_t op, typename L, typename R >
    requires constant_first< op, L >
value_t
value(state_t &s, const binary_t< op, L, R > &b)
{
    return value(s, binary_t< op, R, L > { b.rhs, b.lhs });
}

// A load on the right is the instruction's operand in memory, addl
// -8(%rbp), %eax, and takes no register.  Its type is the more special,
// so the node the mirror takes, 5 + a, is excluded by name; 5 - a is
// this row, movl $5, %eax and subl -4(%rbp), %eax.
template< binary_op_t op, typename L >
    requires(!constant_first< op, L >)
value_t
value(state_t &s, const binary_t< op, L, load_t > &b)
{
    const value_t l = value(s, b.lhs);
    operate(s, op, reg_of(s, l), b.rhs.where);

    return l;
}

// An immediate on the right is the instruction's, negative ones as
// written: addl $5, %eax, andl $-2, %eax.  Of the mirror's nodes it
// would take only a constant on both sides, which the builder folds.
template< binary_op_t op, typename L, immediate I >
value_t
value(state_t &s, const binary_t< op, L, I > &b)
{
    const value_t l = value(s, b.lhs);
    operate(s, op, reg_of(s, l), immediate_of(b.rhs));

    return l;
}

// The value, moved to the result register where its row left it
// elsewhere, then the jump to the epilogue.
void generate(state_t &, const return_t &);

// The unit's assembly; file is the source's name, for the .file
// directive.
void generate(const translation_unit_t &,
              std::string_view file,
              std::ostream &assembly);

} // namespace rcc::amd64
