// -*- mode: c++; -*-

// The back end's entry, declared in c1.hh: the generator of the
// target.  aarch64 has none yet, docs/aarch64.md chapter 14.

#include "c1.hh"
#include "c1-common.hh"

#include <ostream>
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

    not_yet("the aarch64 generator");
}

} // namespace rcc
