// -*- mode: c++; -*-

// The part of c1 that is the same for every target: the integer view
// of a type, the label names, the predicates on the tree, the string
// literals' block after the last function, and the register stack's
// held value and its handle.  None of it names a register, a mnemonic
// or an operand form; each generator includes it and writes its own
// instructions around it.  The register stack's functions are the same
// text for every target too, but they name the target's registers and
// state: they are in c1-stack.hh, which each generator includes inside
// its own namespace.

#pragma once

#include "ast.hh"

#include <ostream>
#include <string>
#include <string_view>
#include <vector>

namespace rcc {

[[noreturn]] void not_yet(std::string_view what);

const int_t &integer(const type_t &);
int operation_width(const type_t &);
long long normalized(long long value, int width);
long long widened(long long value, const int_t &);
long long scaled(long long index, long size);
int written(int width, bool is_signed);

std::string label_name(std::string_view function, int n);

const symbol_t *object_of(const expr_t &);
const symbol_t *register_variable(const expr_t &);
const symbol_t &callee_of(const call_t &);
const expr_t *bare_base(const expr_t &);

bool is_step(unary_op_t);
bool is_shift(binary_op_t);
bool is_comparison(binary_op_t);
binary_op_t inverse(binary_op_t);

bool writes(const expr_t &, const symbol_t *);
bool names(const expr_t &, const symbol_t *);
int registers_in(const std::vector< stmt_t > &);

std::string string_label(std::vector< std::string > &strings, const string_t &);
std::string quoted_file(std::string_view file);
void literals(std::ostream &, const std::vector< std::string > &strings);

// A value the generator holds: in an expression register, or in its
// spill slot once the register was needed for another.  The age orders
// the values for the spill, the oldest first.  valid is the width, in
// bits, up to which the register's bits are the value extended by its
// type's signedness: what the instruction that wrote it left there.  A
// load extends to 32 or 64, a 32-bit write clears the upper half, a
// narrowing cast leaves the bits above its width stale; a widening cast
// that finds the value valid to its width writes nothing.  A read of a
// register variable is borrowed (backend.md 6.7): variable names it, and
// reg is the variable's register, which the value does not own.  A
// borrowed read is never spilled or freed, no row writes its register
// but a compound assignment's in place, and it survives a call, since
// the register is callee-saved.  across counts the branches whose paths
// the value is held across, which may not move it (backend.md 6.7).
struct held_t
{
    int reg;
    int spill;
    long age;
    bool live;
    int valid;
    const symbol_t *variable;
    int across = 0;
};

// A handle on a held value; the default is the no-value of a void
// expression.
struct value_t
{
    int id = -1;
};

} // namespace rcc
