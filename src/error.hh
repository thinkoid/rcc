// -*- mode: c++; -*-

// The diagnostics: where and what.  An error is thrown by the lexer or
// the parser at the first error, or by c1 with no position, caught and
// printed by the driver;
// there is no recovery and no second error.  A warning does not stop
// the translation: the parser adds it to a list the driver owns, and
// the driver prints the list after the parse, or before the error that
// ends it.

#pragma once

#include <string>
#include <string_view>

namespace rcc {

// A position in the source as the preprocessor's line markers name
// it.  The file name is a view into the preprocessed text, which
// outlives every token and every error.  system_header is whether the
// marker in force says, by its flag 3, that the text is a system
// header's.
struct location_t
{
    std::string_view file;
    int line;
    int column;
    bool system_header;
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

// An error of the program that c1 finds: c1's tree carries no
// positions, so what names the function in place of one, and the
// driver prints it after rcc's name.
struct unplaced_error_t
{
    std::string what;
};

} // namespace rcc
