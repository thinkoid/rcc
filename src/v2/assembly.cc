// -*- mode: c++; -*-

// The text both targets write alike, declared in assembly.hh.

#include "assembly.hh"

#include <bit>
#include <format>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

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

void
binding(std::ostream &out, std::string_view name, bool external)
{
    if (external)
        out << std::format("\t.globl\t{}\n", name);
}

namespace {

// The directive of a value of size bytes, which both targets'
// assemblers take: .byte, .short, .long or .quad.
std::string_view
data_directive(int size)
{
    switch (size) {
    case 1: return ".byte";
    case 2: return ".short";
    case 4: return ".long";
    default: return ".quad";
    }
}

// A constant as its directive writes it, rcc's constant_value(): the
// value of the object's type, read unsigned for an unsigned type.  The
// tree holds a constant as its register does, so a signed one is its
// value already, and an unsigned one is its bits at the view: 255 of an
// unsigned char, and 4294967295 of an unsigned held as -1.
std::string
constant_value(long long value, view_t view, sign_t sign)
{
    if (sign == sign_t::signed_)
        return std::to_string(value);

    const auto bits = static_cast< unsigned long long >(value);
    if (width(view) == 64)
        return std::to_string(bits);

    return std::to_string(bits & ((1ull << width(view)) - 1));
}

// One object the unit defines, rcc's object(): its binding, .globl for
// one of external linkage; .type and .size, the symbol's kind and its
// bytes in the symbol table, which the linker and a debugger read; its
// alignment, an integer's its size, 6.2.8, and no directive for an
// alignment of 1; the label; then its value in the directive of its
// size, or its zeros.
void
object(std::ostream &out, const object_decl_t &od)
{
    const int size = width(od.view) / 8;

    binding(out, od.name, od.external);
    out << std::format("\t.type\t{}, @object\n"
                       "\t.size\t{}, {}\n",
                       od.name, od.name, size);

    if (size > 1)
        out << std::format("\t.p2align {}\n",
                           std::countr_zero(static_cast< unsigned >(size)));

    out << od.name << ":\n";

    if (od.value)
        out << std::format("\t{}\t{}\n", data_directive(size),
                           constant_value(*od.value, od.view, od.sign));
    else
        out << std::format("\t.zero\t{}\n", size);
}

// One block of objects, under its section's directive, or nothing when
// no object belongs to it, rcc's object_block(): those with a value or
// those without.
void
object_block(std::ostream &out,
             const std::vector< object_decl_t > &objects,
             bool initialized)
{
    bool opened = false;

    for (const object_decl_t &od : objects) {
        if (od.value.has_value() != initialized)
            continue;

        if (!opened)
            out << (initialized ? "\t.data\n" : "\t.bss\n");
        opened = true;

        object(out, od);
    }
}

} // namespace

// One .data block of the objects with a value, then one .bss block of
// those without, which are zero, 6.7.11p11, each in the order of the
// unit's entries.  int n = 0; is in .data, since the source wrote its
// initializer.
void
data(std::ostream &out, const std::vector< object_decl_t > &objects)
{
    object_block(out, objects, true);
    object_block(out, objects, false);
}

} // namespace rcc
