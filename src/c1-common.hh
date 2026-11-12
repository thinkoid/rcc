// -*- mode: c++; -*-

// The part of c1 that is the same for every target: the integer view
// of a type, the label names, the predicates on the tree, and the
// string literals' block after the last function.  None of it names a
// register, a mnemonic or an operand form; each generator includes it
// and writes its own instructions around it.

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

} // namespace rcc
