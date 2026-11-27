// -*- mode: c++; -*-

// The text both targets write alike, to the letter, which names no
// register: an instruction's line, a label and its count, the .file
// directive's string, a symbol's binding, the unit's data and the names
// of the objects it defines.  insn() and new_label() are templates over
// a target's state, which holds the function's text and its count of
// labels.

#pragma once

#include "unit.hh"

#include <initializer_list>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace rcc {

// One instruction: a tab, the mnemonic, a tab and the operands
// separated by a comma and a space, as gcc -S and clang -S write them.
template< typename S >
void
insn(S &s,
     std::string_view mnemonic,
     std::initializer_list< std::string_view > operands)
{
    s.body << '\t' << mnemonic;

    const char *separator = "\t";
    for (const auto operand : operands) {
        s.body << separator << operand;
        separator = ", ";
    }

    s.body << '\n';
}

// Labels are counted per function, the epilogue's first.  The
// function's name in the label keeps the file's labels unique, since an
// identifier holds no dot.
template< typename S >
int
new_label(S &s)
{
    return ++s.labels;
}

// A label of the function, docs/backend.md 7.2: .L, the function's name
// and the number, .Lsum.1.
std::string label_name(std::string_view function, int n);

// The file name as a .file directive's string: its base name in quotes,
// a quote and a backslash in it escaped.
std::string quoted_file(std::string_view file);

// The binding of a symbol the unit defines, docs/backend.md chapter 10,
// rcc's binding(): .globl where its linkage is external, and nothing
// for one of internal linkage, which is local to the file's object.
void binding(std::ostream &, std::string_view name, bool external);

// The objects the unit defines, written after its last function, rcc's
// data() (backend.md chapter 10): the same text on both targets.
void data(std::ostream &, const std::vector< object_decl_t > &objects);

// The names of the objects the unit defines, rcc's defined_symbols() of
// objects alone, which a target takes before its first function: an
// access of one is its label, and of any other the address the GOT
// holds.
std::unordered_set< std::string > defined_symbols(const translation_unit_t &);

} // namespace rcc
