// -*- mode: c++; -*-

// The back end's one entry: a unit in, the target's assembly text out,
// for the system assembler.  Each target has a generator of its own,
// read against its own design page: docs/backend.md for x86-64,
// docs/aarch64.md for aarch64.  The x86-64 design's tables are its
// generator's checklist: every local in its slot off the frame
// pointer, the caller-saved registers as an expression stack,
// conditions as jumps, the System V convention at every call and
// entry.  The instruction written is the instruction in the table;
// nothing between the tree and the assembler reasons about the
// program.  A construct outside the slices landed so far throws
// logic_error, which the driver reports as an internal error.

#pragma once

#include "ast.hh"
#include "target.hh"

#include <ostream>
#include <string_view>

namespace rcc {

// file is the source's name, for the .file directive
void generate(const unit_t &,
              const target_t &,
              std::string_view file,
              std::ostream &);

// The generators generate() calls, one per target written.
void generate_x86_64(const unit_t &,
                     const target_t &,
                     std::string_view file,
                     std::ostream &);

} // namespace rcc
