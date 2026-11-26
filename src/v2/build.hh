// -*- mode: c++; -*-

// The builders, docs/typed-tree.md 3.3: all the parser sees of the
// zoo.  Each takes a construct's parts and returns its node under its
// handle; build.cc defines them, where the node types and the models
// are seen, so a builder is where a part's type becomes the node's.

#pragma once

#include "expr.hh"
#include "stmt.hh"

#include <vector>

namespace rcc {

expr_t make_literal(int value);
stmt_t make_return(expr_t value);
stmt_t make_block(std::vector< stmt_t > items);

} // namespace rcc
