// -*- mode: c++; -*-

// The parser's one entry: tokens in, a typed unit out.  Throws error_t
// at the first error.

#pragma once

#include "ast.hh"
#include "lex.hh"

#include <vector>

namespace rcc {

unit_t parse(const std::vector< token_t > &);

} // namespace rcc
