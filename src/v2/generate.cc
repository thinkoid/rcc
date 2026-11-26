// -*- mode: c++; -*-

// The back end's entry, declared in generate.hh: the unit's entry of
// the target, one per target, docs/aarch64.md chapter 12.

#include "generate.hh"

#include "amd64.hh"
#include "arm64.hh"

#include <ostream>
#include <stdexcept>
#include <string_view>

namespace rcc {

void
generate(const translation_unit_t &unit,
         const target_t &target,
         std::string_view file,
         std::ostream &out)
{
    if (target.name == x86_64.name)
        return amd64::generate(unit, file, out);

    if (target.name == aarch64.name)
        return arm64::generate(unit, file, out);

    throw std::logic_error("a target with no generator");
}

} // namespace rcc
