// -*- mode: c++; -*-

// The pipeline's one entry, which the driver calls between the
// preprocessor and the assembler: preprocessed C in, the target's
// assembly text out.  The driver is written against this header
// alone, so an executable is the driver and one pipeline.  Each
// warning is added to warnings as the pipeline meets it, a list the
// caller owns, so the warnings before an error outlive it.  Throws
// error_t at the first error of the source, unplaced_error_t for an
// error with no position, and logic_error for an internal error,
// which includes a construct the pipeline does not compile yet.

#pragma once

#include "error.hh"
#include "target.hh"

#include <ostream>
#include <string_view>
#include <vector>

namespace rcc {

// file is the source's name, for the diagnostics and the .file
// directive
void compile(std::string_view text,
             std::string_view file,
             const target_t &,
             std::vector< warning_t > &warnings,
             std::ostream &assembly);

} // namespace rcc
