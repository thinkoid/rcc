// -*- mode: c++; -*-

// aarch64, docs/aarch64.md: the registers, the state of a function's
// generation, the rows of the zoo's types this target writes itself,
// and the unit's entry.  The rows are declared here for build.cc, whose
// models call them, and defined in arm64.cc, which reaches the tree
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

namespace rcc::arm64 {

// The general registers by number, chapter 2.1: the fifteen expression
// registers of chapter 4, the caller-saved set without x8, x0 the
// convention's result register.
enum reg_t : int {
    x0,
    x1,
    x2,
    x3,
    x4,
    x5,
    x6,
    x7,
    x9 = 9,
    x10,
    x11,
    x12,
    x13,
    x14,
    x15
};

// The state of one function's generation, which the function's row
// builds fresh.  The body is a buffer, which the frame is written
// around; the function's name makes its labels, which are counted per
// function, and ret is the one epilogue's.  terminated says that no
// path reaches what is written next: the last instruction written is
// an unconditional jump, which jump() sets, so main writes no return of
// 0 (backend.md 7.2).  frame is the bytes from x29 to the end of the
// last slot, the frame record's alone before the first, and slots each
// local's displacement off x29, by its number.
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

// The constant rows of an int, 32 bits into r.
void into(state_t &, const literal_t &, reg_t r);
void into(state_t &, const zero_t &, reg_t r);

// The load row, the int a slot holds into r.
void into(state_t &, const load_t &, reg_t r);

// - or ~ on r in place, at 32 bits.
void operate(state_t &, unary_op_t op, reg_t r);

// The operation of op into r from r and an operand at 32 bits: a
// register, or a literal, the instruction's immediate where it encodes
// one.
void operate(state_t &, binary_op_t op, reg_t r, reg_t source);
void operate(state_t &, binary_op_t op, reg_t r, const literal_t &source);

// The store row, r into a slot at 32 bits.
void store(state_t &, reg_t r, slot_t where);

// The niche of initialize(), 6.6: 0 is the zero register's store, and
// no register is held.
void initialize(state_t &, slot_t where, const zero_t &);

// Whether a node with the constant L on the left of op is written with
// its operands exchanged, rcc's constant_first() as ordered() asks it:
// op commutes, and L is a constant, whether the instruction encodes it
// or not.
template< binary_op_t op, typename L >
concept constant_first = commutes(op) && constant< L >;

// The niches of + - & | ^, chapter 6.1, over the primary of
// rows-expressions.hh.  A constant on the left of an operator that
// commutes is the row of the node with its operands exchanged, 5 + a
// as a + 5: the right operand, which is no constant, into r, then the
// constant as the right operand.
template< binary_op_t op, typename L, typename R >
    requires constant_first< op, L >
void
into(state_t &s, const binary_t< op, L, R > &b, reg_t r)
{
    into(s, binary_t< op, R, L > { b.rhs, b.lhs }, r);
}

// A literal on the right is the instruction's immediate where it
// encodes it, add w0, w0, #5, and the primary's register otherwise.  0
// takes the primary: a & 0 is mov w1, #0 and the and.  Of the mirror's
// nodes it would take only a constant on both sides, which the builder
// folds.
template< binary_op_t op, typename L >
void
into(state_t &s, const binary_t< op, L, literal_t > &b, reg_t r)
{
    into(s, b.lhs, r);
    operate(s, op, r, b.rhs);
}

// The value into the result register, then the jump to the epilogue.
void generate(state_t &, const return_t &);

// The unit's assembly; file is the source's name, for the .file
// directive.
void generate(const translation_unit_t &,
              std::string_view file,
              std::ostream &assembly);

} // namespace rcc::arm64
