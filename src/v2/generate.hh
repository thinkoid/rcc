// -*- mode: c++; -*-

// The back end's one entry: a unit in, the target's assembly text out,
// for the system assembler.  Each target writes its own text, read
// against its own design page: docs/backend.md for x86-64,
// docs/aarch64.md for aarch64.  The listing is the one rcc writes for
// the same unit.

#pragma once

#include "target.hh"
#include "unit.hh"

#include <ostream>
#include <string_view>

namespace rcc {

// file is the source's name, for the .file directive
void generate(const translation_unit_t &,
              const target_t &,
              std::string_view file,
              std::ostream &);

} // namespace rcc
