// -*- mode: c++; -*-

// The builders, docs/typed-tree.md 3.3: all the parser sees of the
// zoo.  Each takes a construct's parts and returns its node under its
// handle; build.cc defines them, where the node types and the models
// are seen, so a builder is where a part's type becomes the node's.  A
// local is passed as its number among the function's, which the parser
// counts from 0 in the order it declares them.

#pragma once

#include "expr.hh"
#include "stmt.hh"

#include <vector>

namespace rcc {

expr_t make_literal(int value);
expr_t make_local(int slot);

// The lvalue conversion, 6.3.2.1p2: a place becomes the value it holds;
// any other expression is a value already, and is returned as it is.
expr_t rvalue(expr_t e);

stmt_t make_decl(int slot);
stmt_t make_init(int slot, expr_t value);
stmt_t make_return(expr_t value);
stmt_t make_block(std::vector< stmt_t > items);

} // namespace rcc
