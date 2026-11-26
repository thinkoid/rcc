// -*- mode: c++; -*-

// The text both targets write alike, declared in assembly.hh.

#include "assembly.hh"

#include <format>
#include <string>
#include <string_view>

namespace rcc {

std::string
label_name(std::string_view function, int n)
{
    return std::format(".L{}.{}", function, n);
}

std::string
quoted_file(std::string_view file)
{
    const auto slash = file.rfind('/');
    if (slash != std::string_view::npos)
        file.remove_prefix(slash + 1);

    std::string text = "\"";
    for (const char c : file) {
        if (c == '"' || c == '\\')
            text += '\\';
        text += c;
    }

    return text + '"';
}

} // namespace rcc
