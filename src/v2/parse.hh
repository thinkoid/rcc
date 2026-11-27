// -*- mode: c++; -*-

// The parser's one entry: the tokens of a translation unit in, the
// unit out, its function bodies built through the builders.  The
// target says what plain char is.  The first error is thrown as error_t
// at its token, rcc's own words where rcc says it; a construct rcc-v2
// does not compile yet is an error_t too, "not yet: " and the
// construct's name.  A warning joins warnings, a list the caller owns.

#pragma once

#include "error.hh"
#include "lex.hh"
#include "target.hh"
#include "unit.hh"

#include <vector>

namespace rcc {

translation_unit_t parse(const std::vector< token_t > &,
                         const target_t &target,
                         std::vector< warning_t > &warnings);

} // namespace rcc
