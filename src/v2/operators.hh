// -*- mode: c++; -*-

// The operators of the expressions, a closed domain, so a parameter of
// the node types as it stands (docs/typed-tree.md principle 6).  The
// parser holds an operator as a value and passes it to a builder,
// which makes it the node's parameter; the parser sees no node type,
// so the operators are a header of their own, which the handle's
// includes.  The names are rcc's.

#pragma once

namespace rcc {

// The unary operators of 6.5.4.3 that make a node: - and ~.
enum class unary_op_t { neg, bit_not };

} // namespace rcc
