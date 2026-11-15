// -*- mode: c++; -*-

// The part of c1 that is the same for every target: the integer view
// of a type, the symbol and label names, the predicates on the tree,
// the rule that chooses a switch's form, the blocks of the string
// literals and of the objects after the last function, and the
// register stack's held value and its handle.  None
// of it names a register, a mnemonic or an operand form; each
// generator includes it and writes its own instructions around it.
// The register stack's functions are
// the same text for every target too, but they name the target's
// registers and state: they are in c1-stack.hh, which each generator
// includes inside its own namespace.

#pragma once

#include "ast.hh"
#include "target.hh"

#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace rcc {

[[noreturn]] void not_yet(std::string_view what);

const int_t &integer(const type_t &);
int operation_width(const type_t &);
long long normalized(long long value, int width);
long long widened(long long value, const int_t &);
long long scaled(long long index, long size);
long long wrapped_add(long long a, long long b);
int written(int width, bool is_signed);

std::string_view assembly_name(const symbol_t &);
std::string symbol_plus(std::string_view symbol, long long offset);
void binding(std::ostream &, const symbol_t &);
std::string label_name(std::string_view function, int n);

const symbol_t *global_of(const expr_t &);
const symbol_t *object_of(const expr_t &);
const symbol_t *register_variable(const expr_t &);
const symbol_t *named_callee(const call_t &);
const function_t &callee_type(const call_t &);
std::unordered_set< const symbol_t * > defined_symbols(const unit_t &);
const expr_t *bare_base(const expr_t &);

// A pointer that is an lvalue's address plus a constant (backend.md
// 6.3): the decay or the & that takes the address, the lvalue under
// it, and the offset in bytes from that address.
struct lvalue_offset_t
{
    const expr_t *address;
    const expr_t *lvalue;
    long long offset;
};

std::optional< lvalue_offset_t > lvalue_offset(const expr_t &pointer);

// An address reached by a name and a constant, Ritchie's &x+c
// (backend.md 6.3): the lvalue whose address is a slot or a label, and
// the offset in bytes from that address.
struct named_offset_t
{
    const expr_t *object;
    long long offset;
};

std::optional< named_offset_t >
named_offset(const expr_t &pointer,
             const std::unordered_set< const symbol_t * > &defined);
bool within_object(const named_offset_t &);

bool is_step(unary_op_t);
bool is_shift(binary_op_t);
bool is_comparison(binary_op_t);
binary_op_t inverse(binary_op_t);

bool writes(const expr_t &, const symbol_t *);
bool names(const expr_t &, const symbol_t *);

// The forms of a switch's dispatch, backend.md 7.3: none, when there
// is no case; a direct jump through a table; a simple compare chain; a
// hash, a table jump by the remainder and a compare chain by the
// quotient.
enum class dispatch_t { none, direct, simple, hash };

unsigned long long unsigned_bits(long long value, int width);
std::vector< case_t > sorted_cases(std::vector< case_t >, const int_t &);
unsigned long long case_offset(long long value, long long first, int width);
unsigned long long case_range(const std::vector< case_t > &sorted, int width);
dispatch_t dispatch_of(const std::vector< case_t > &sorted, int width);
unsigned long long table_size(const std::vector< case_t > &sorted, int width);

std::string string_label(std::vector< std::string > &strings, const string_t &);
std::string quoted_file(std::string_view file);
void data(std::ostream &,
          std::vector< std::string > strings,
          const std::vector< object_decl_t > &objects,
          const target_t &);

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

// A statement a break jumps out of: the label past its end, and for a
// loop, which a continue goes on with, the label of the loop's
// continuation, backend.md 7.2.  A switch has none, so a continue in it
// goes on with the loop around it.
struct jump_target_t
{
    int break_;
    std::optional< int > continue_;
};

} // namespace rcc
