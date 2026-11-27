// -*- mode: c++; -*-

// The text both targets write alike, to the letter, which names no
// register: an instruction's line, a label and its count, the .file
// directive's string, a symbol's binding, a switch's table, the unit's
// data and the names of the objects it defines.  insn(), new_label() and
// jump_table() are templates over a target's state, which holds the
// function's text and its count of labels.

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

// The table of a switch's dispatch, rcc's jump_table(), backend.md 7.3,
// as the Seventh Edition compiler wrote it: in .data, in the function's
// text where the dispatch stands, aligned to 8, under a label of the
// function, an address for each entry, which the loader relocates as it
// does a global's address constant; then .text again.
template< typename S >
void
jump_table(S &s, int table, const std::vector< int > &entries)
{
    s.body << "\t.data\n\t.p2align 3\n"
           << label_name(s.function, table) << ":\n";

    for (const int entry : entries)
        s.body << "\t.quad\t" << label_name(s.function, entry) << '\n';

    s.body << "\t.text\n";
}

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
