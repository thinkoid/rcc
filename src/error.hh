// -*- mode: c++; -*-

// The diagnostics: where and what.  An error is thrown by the lexer or
// the parser at the first error, caught and printed by the driver;
// there is no recovery and no second error.  A warning does not stop
// the translation: the parser records it in the unit, and the driver
// prints it after a successful parse.

#pragma once

#include <string>
#include <string_view>

namespace rcc {

// A position in the source as the preprocessor's line markers name
// it.  The file name is a view into the preprocessed text, which
// outlives every token and every error.
struct location_t
{
    std::string_view file;
    int line;
    int column;
};

struct error_t
{
    location_t where;
    std::string what;
};

struct warning_t
{
    location_t where;
    std::string what;
};

} // namespace rcc
