// -*- mode: c++; -*-

// The zoo, docs/typed-tree.md chapter 1: the node types under the
// handles.  A node is an aggregate of its parts, each concrete where a
// row's choice depends on it and erased where none does.  The builders
// make them, build.cc; the rows that write them are each target's,
// amd64.hh and arm64.hh, and rows-statements.hh's for those both
// targets write alike.

#pragma once

#include "expr.hh"
#include "stmt.hh"

#include <vector>

namespace rcc {

// An integer constant of type int, 6.4.4.2: its row writes 32 bits.
struct literal_t
{
    int value;
};

// return E;, 6.8.7.5: the value in the convention's result register,
// then the jump to the function's one epilogue.
struct return_t
{
    expr_t value;
};

// A compound statement, 6.8.3: its items in order, erased, since their
// count is not bounded (4.4).
struct block_t
{
    std::vector< stmt_t > items;
};

} // namespace rcc
