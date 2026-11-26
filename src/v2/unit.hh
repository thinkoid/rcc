// -*- mode: c++; -*-

// The translation unit as the parser fills it, 6.9.1: its function
// definitions, in order.  The two types are under no handle: a target
// writes a unit and a function by its own entry, generate.hh's, and
// no row is written for either.

#pragma once

#include "stmt.hh"

#include <string>
#include <vector>

namespace rcc {

// A function's name and its body, 6.9.2: a compound statement is a
// statement.
struct function_definition_t
{
    std::string name;
    stmt_t body;
};

struct translation_unit_t
{
    std::vector< function_definition_t > functions;
};

} // namespace rcc
