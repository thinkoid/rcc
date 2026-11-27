// -*- mode: c++; -*-

// The translation unit as the parser fills it, 6.9: its functions, in
// the order of each one's first declaration.  The two types are under
// no handle: a target writes a unit and a function by its own entry,
// generate.hh's, and no row is written for either.  They hold what the
// listing needs; what C says of a function's name, its type among it,
// is the parser's.

#pragma once

#include "stmt.hh"

#include <optional>
#include <string>
#include <vector>

namespace rcc {

// A function the unit declares, 6.9.1, entered at its first
// declaration, a later one and the definition finding the same entry:
// its name; whether its linkage is external, 6.2.2p5, so its symbol is
// global, which a static one's is not, p3; and its body, a compound
// statement, which is a statement, 6.9.2, where the unit defines it.  A
// function only declared writes nothing, since the assembler leaves its
// symbol to the link.
struct function_decl_t
{
    std::string name;
    bool external;
    std::optional< stmt_t > body = std::nullopt;
};

struct translation_unit_t
{
    std::vector< function_decl_t > functions;
};

} // namespace rcc
