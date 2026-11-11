// -*- mode: c++; -*-

// The parser's one entry: tokens in, a typed unit out.  The target
// gives the count of register variables, the signedness of plain char
// and the frame limit.  Throws error_t at the first error.

#pragma once

#include "ast.hh"
#include "lex.hh"
#include "target.hh"

#include <vector>

namespace rcc {

unit_t parse(const std::vector< token_t > &, const target_t &);

} // namespace rcc
