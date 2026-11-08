// -*- mode: c++; -*-

// The back end's one entry: a unit in, x86-64 assembly text out, AT&T
// syntax for the system assembler.  The design is docs/backend.md and
// its tables are this file's checklist: every local in its slot off
// the frame pointer, the caller-saved registers as an expression
// stack, conditions as jumps, the System V convention at every call
// and entry.  The instruction written is the instruction in the
// table; nothing between the tree and the assembler reasons about the
// program.  A construct outside the slices landed so far throws
// logic_error, which the driver reports as an internal error.

#pragma once

#include "ast.hh"

#include <ostream>
#include <string_view>

namespace rcc {

// file is the source's name, for the .file directive
void generate(const unit_t &, std::string_view file, std::ostream &);

} // namespace rcc
