// -*- mode: c++; -*-

// The translation unit as the parser fills it, 6.9: its functions, in
// the order of each one's first declaration, and the objects it
// defines, in the order of each one's first declaration that defines
// it.  The types are under no handle: a target writes a unit and a
// function by its own entry, generate.hh's, and the unit's data by
// assembly.hh's, and no row is written for any of them.  They hold what
// the listing needs; what C says of a name, its type among it, is the
// parser's.

#pragma once

#include "operators.hh"
#include "stmt.hh"

#include <optional>
#include <string>
#include <vector>

namespace rcc {

// A function the unit declares, 6.9.1, entered at its first
// declaration, a later one and the definition finding the same entry:
// its name; whether its linkage is external, 6.2.2p5, so its symbol is
// global, which a static one's is not, p3; the view of each of its
// parameters, in order, set where the unit defines it, which its body
// reads as its first locals, numbered from 0, and all its frame needs
// of one, a slot and the store of its argument at that view, since the
// body reads a parameter through its place, which has its sign
// (docs/typed-tree.md principle 9); and its body, a compound statement,
// which is a statement, 6.9.2, where the unit defines it.  A function
// only declared writes nothing, since the assembler leaves its symbol to
// the link, and has no views.
struct function_decl_t
{
    std::string name;
    bool external;
    std::vector< view_t > parameters;
    std::optional< stmt_t > body = std::nullopt;
};

// An object the unit defines, 6.9.3, entered at its first declaration
// that defines it, a definition or a tentative one, a later one finding
// the same entry: its name; whether its linkage is external, so its
// symbol is global; its view, the bytes it takes and their alignment,
// and the sign of its type, which its value's directive is written by;
// and its value, the constant its initializer folded to, as the tree
// holds a constant (docs/typed-tree.md 4.3), or none for one without,
// which is zero, 6.7.11p11.  An object only declared, every declaration
// of it extern without an initializer, has no entry, and writes
// nothing.
struct object_decl_t
{
    std::string name;
    bool external;
    view_t view;
    sign_t sign;
    std::optional< long long > value = std::nullopt;
};

struct translation_unit_t
{
    std::vector< function_decl_t > functions;
    std::vector< object_decl_t > objects;
};

} // namespace rcc
