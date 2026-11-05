// -*- mode: c++; -*-

// The one diagnostic: where and what.  Thrown by the parser at the
// first error, caught and printed by the driver.  There is no
// recovery and no second error.

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

} // namespace rcc
