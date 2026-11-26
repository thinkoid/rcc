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

// The constant rows of an int, 32 bits into r.
void into(state_t &, const literal_t &, reg_t r);
void into(state_t &, const zero_t &, reg_t r);

// The load row, the int a slot holds into r.
void into(state_t &, const load_t &, reg_t r);

// - or ~ on r in place, at 32 bits.
void operate(state_t &, unary_op_t op, reg_t r);

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

// The value into the result register, then the jump to the epilogue.
void generate(state_t &, const return_t &);

// The unit's assembly; file is the source's name, for the .file
// directive.
void generate(const translation_unit_t &,
              std::string_view file,
              std::ostream &assembly);

} // namespace rcc::amd64
