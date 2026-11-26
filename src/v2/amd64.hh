// -*- mode: c++; -*-

// x86-64, docs/backend.md: the registers, the state of a function's
// generation, the rows of the zoo's types this target writes itself,
// and the unit's entry.  The rows are declared here for build.cc, whose
// models call them, and defined in amd64.cc, which reaches the tree
// only through a handle's row.

#pragma once

#include "targets.hh"
#include "unit.hh"
#include "zoo.hh"

#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

namespace rcc::amd64 {

// The registers, chapter 4: rax, the convention's result register.
enum reg_t : int { rax };

// The state of one function's generation, which the function's row
// builds fresh.  The body is a buffer, which the frame is written
// around; the function's name makes its labels, which are counted per
// function, and ret is the one epilogue's.  terminated says that no
// path reaches what is written next: the last instruction written is
// an unconditional jump, which jump() sets, so main writes no return of
// 0 (backend.md 7.2).
struct state_t
{
    std::ostringstream body { };
    std::string function;
    int labels = 0;
    int ret = 0;
    bool terminated = false;
};

// The constant row of an int, 32 bits into r.
void into(state_t &, const literal_t &, reg_t r);

// The value into the result register, then the jump to the epilogue.
void generate(state_t &, const return_t &);

// The unit's assembly; file is the source's name, for the .file
// directive.
void generate(const translation_unit_t &,
              std::string_view file,
              std::ostream &assembly);

} // namespace rcc::amd64
