// -*- mode: c++; -*-

// The back end's entry, declared in c1.hh: the generator of the
// target, one per target, docs/aarch64.md chapter 12.

#include "c1.hh"
#include "c1-common.hh"

#include <ostream>
#include <stdexcept>
#include <string_view>

namespace rcc {

void
generate(const unit_t &unit,
         const target_t &target,
         std::string_view file,
         std::ostream &out)
{
    if (target.name == x86_64.name)
        return generate_x86_64(unit, target, file, out);

    if (target.name == aarch64.name)
        return generate_aarch64(unit, target, file, out);

    throw std::logic_error("c1: a target with no generator");
}

} // namespace rcc
