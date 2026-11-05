// -*- mode: c++; -*-

// The emitter's one entry: a unit in, LLVM IR text out.  The
// guarantees are delivered here and nowhere else; the never-emitted
// table in docs/guarantees.md is this file's review checklist.

#pragma once

#include "ast.hh"

#include <ostream>

namespace rcc {

void emit(const unit_t &, std::ostream &);

} // namespace rcc
