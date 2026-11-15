// -*- mode: c++; -*-

// The part of c1 that is the same for every target, declared in
// c1-common.hh: the integer view of a type, the symbol and label names,
// the tree predicates, and the blocks of the string literals and of the
// objects.  Nothing here writes an instruction.

#include "c1-common.hh"

#include <algorithm>
#include <bit>
#include <format>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace rcc {

[[noreturn]] void
not_yet(std::string_view what)
{
    throw std::logic_error(std::format("c1: not yet: {}", what));
}

// types

// The integer type of an operand.  A pointer is a 64-bit unsigned
// quantity to the machine, so its integer view is unsigned long's: the
// width rule, the suffixes and the unsigned conditions of G7 apply to
// it unchanged.  An array is never a value, since the parser decays
// every array it reads, 6.3.2.1p3; anything else the parser does not
// make.
const int_t &
integer(const type_t &t)
{
    static constexpr int_t address_view { 64, false };

    if (is< pointer_t >(t))
        return address_view;

    if (is< array_t >(t))
        throw std::logic_error("c1: an array as a value");

    if (!is< int_t >(t))
        throw std::logic_error("c1: a non-integer operand");

    return as< int_t >(t);
}

// The width of an operator's operation, 32 or 64 bits: the parser
// promotes every narrower operand before an operator sees it.
int
operation_width(const type_t &t)
{
    const int width = integer(t).width;
    if (width < 32)
        throw std::logic_error("c1: an operator on a narrow operand");

    return width;
}

// A constant of N bits as the assembler takes it: the value modulo
// 2^N, written signed.  G2: the low bits are the value.
long long
normalized(long long value, int width)
{
    if (width >= 64)
        return value;

    const auto mask = (1ull << width) - 1;
    const auto sign = 1ull << (width - 1);
    const auto bits = static_cast< unsigned long long >(value) & mask;

    return static_cast< long long >(bits ^ sign) -
           static_cast< long long >(sign);
}

// A constant narrower than int as the promotion to int reads it: the
// low bits of the value, sign- or zero-extended by the type.
long long
widened(long long value, const int_t &t)
{
    if (t.is_signed)
        return normalized(value, t.width);

    const auto mask = (1ull << t.width) - 1;

    return static_cast< long long >(static_cast< unsigned long long >(value) &
                                    mask);
}

// A constant index times the size, modulo 2^64: G6, the address
// arithmetic wraps.
long long
scaled(long long index, long size)
{
    return static_cast< long long >(static_cast< unsigned long long >(index) *
                                    static_cast< unsigned long long >(size));
}

// The width a value is valid to after an instruction writes it at
// width: a 32-bit write clears the upper half of the register, which
// is the extension of an unsigned value and not of a signed one.
int
written(int width, bool is_signed)
{
    return width == 32 && !is_signed ? 64 : width;
}

// operands

// The name the assembly gives a symbol, at its label and at every use:
// the one __asm__ gave it, or else its name in C.
std::string_view
assembly_name(const symbol_t &sym)
{
    if (sym.asm_name.empty())
        return sym.name;

    return sym.asm_name;
}

// A label of the function, chapter 7.2: .L, the function's name and
// the number, .Lsum.1.
std::string
label_name(std::string_view function, int n)
{
    return std::format(".L{}.{}", function, n);
}

// the frame

// The global a name designates, or null for any other lvalue: its
// address is its label or comes from the GOT (backend.md chapter 10),
// where a local's is its slot.
const symbol_t *
global_of(const expr_t &e)
{
    if (!is< name_t >(e))
        return nullptr;

    const auto *sym = as< name_t >(e).sym;

    return sym->storage == storage_t::global ? sym : nullptr;
}

// The local or parameter a name designates, which has a slot.  A
// register variable has none, and the parser refuses its address,
// 6.5.4.2p1; a global has none either, and global_of() takes it first.
const symbol_t *
object_of(const expr_t &e)
{
    if (!is< name_t >(e))
        throw std::logic_error("c1: an lvalue the parser does not make");

    const auto *sym = as< name_t >(e).sym;
    if (sym->storage == storage_t::function)
        throw std::logic_error("c1: a function as an object");

    if (sym->storage == storage_t::global)
        throw std::logic_error("c1: a global in a slot");

    if (in_register(*sym))
        throw std::logic_error("c1: the address of a register variable");

    return sym;
}

// register variables

// The register variable an lvalue names, or null.  The parser gave the
// symbol its register, or declared the sixth an ordinary local.
const symbol_t *
register_variable(const expr_t &e)
{
    if (!is< name_t >(e))
        return nullptr;

    const auto *sym = as< name_t >(e).sym;

    return in_register(*sym) ? sym : nullptr;
}

bool
is_step(unary_op_t op)
{
    return op == unary_op_t::pre_inc || op == unary_op_t::pre_dec ||
           op == unary_op_t::post_inc || op == unary_op_t::post_dec;
}

// Whether evaluating e may write the register variable sym: an
// assignment to it or a ++ or -- of it anywhere in e.
bool
writes(const expr_t &e, const symbol_t *sym)
{
    if (is< unary_t >(e)) {
        const auto &u = as< unary_t >(e);
        return (is_step(u.op) && register_variable(*u.arg) == sym) ||
               writes(*u.arg, sym);
    }

    if (is< binary_t >(e))
        return writes(*as< binary_t >(e).lhs, sym) ||
               writes(*as< binary_t >(e).rhs, sym);

    if (is< assign_t >(e)) {
        const auto &a = as< assign_t >(e);
        return register_variable(*a.lhs) == sym || writes(*a.lhs, sym) ||
               writes(*a.rhs, sym);
    }

    if (is< call_t >(e)) {
        const auto &c = as< call_t >(e);
        return writes(*c.callee, sym) ||
               std::ranges::any_of(c.args, [sym](const expr_t &arg) {
                   return writes(arg, sym);
               });
    }

    if (is< cast_t >(e))
        return writes(*as< cast_t >(e).arg, sym);

    if (is< conditional_t >(e)) {
        const auto &c = as< conditional_t >(e);
        return writes(*c.cond, sym) || writes(*c.yes, sym) ||
               writes(*c.no, sym);
    }

    return false;
}

// Whether e names the register variable sym anywhere, to read it or to
// write it.  Such a variable has no address, 6.5.4.2p1, so no other
// expression can reach it.
bool
names(const expr_t &e, const symbol_t *sym)
{
    if (is< name_t >(e))
        return as< name_t >(e).sym == sym;

    if (is< unary_t >(e))
        return names(*as< unary_t >(e).arg, sym);

    if (is< binary_t >(e))
        return names(*as< binary_t >(e).lhs, sym) ||
               names(*as< binary_t >(e).rhs, sym);

    if (is< assign_t >(e))
        return names(*as< assign_t >(e).lhs, sym) ||
               names(*as< assign_t >(e).rhs, sym);

    if (is< call_t >(e)) {
        const auto &c = as< call_t >(e);
        return names(*c.callee, sym) ||
               std::ranges::any_of(c.args, [sym](const expr_t &arg) {
                   return names(arg, sym);
               });
    }

    if (is< cast_t >(e))
        return names(*as< cast_t >(e).arg, sym);

    if (is< conditional_t >(e)) {
        const auto &c = as< conditional_t >(e);
        return names(*c.cond, sym) || names(*c.yes, sym) || names(*c.no, sym);
    }

    return false;
}

// expressions

bool
is_shift(binary_op_t op)
{
    return op == binary_op_t::shl || op == binary_op_t::shr;
}

bool
is_comparison(binary_op_t op)
{
    switch (op) {
    case binary_op_t::lt:
    case binary_op_t::gt:
    case binary_op_t::le:
    case binary_op_t::ge:
    case binary_op_t::eq:
    case binary_op_t::ne: return true;
    default: return false;
    }
}

// The comparison that is true exactly when op is false.
binary_op_t
inverse(binary_op_t op)
{
    switch (op) {
    case binary_op_t::lt: return binary_op_t::ge;
    case binary_op_t::ge: return binary_op_t::lt;
    case binary_op_t::gt: return binary_op_t::le;
    case binary_op_t::le: return binary_op_t::gt;
    case binary_op_t::eq: return binary_op_t::ne;
    default: return binary_op_t::eq;
    }
}

// The function a direct call names, or null for a call through a
// pointer: the parser leaves a function's name as the callee of the
// one, and gives the other a callee of pointer-to-function type
// (shape.md chapter 4).
const symbol_t *
named_callee(const call_t &c)
{
    if (!is< function_t >(c.callee->type))
        return nullptr;

    return as< name_t >(*c.callee).sym;
}

// The function type a call is made with: the named function's, or the
// pointee of the callee's pointer, the call site's type whatever the
// pointer was converted from (G16).
const function_t &
callee_type(const call_t &c)
{
    const type_t &t = c.callee->type;
    if (is< function_t >(t))
        return as< function_t >(t);

    return as< function_t >(*as< pointer_t >(t).pointee);
}

// The functions and the objects the unit defines, whose address c1
// forms PC-relative; one the unit only declares is reached through the
// GOT (backend.md chapter 10).  A definition anywhere in the file
// counts, before the use or after it: a function's body, an object's
// entry in the unit's objects.
std::unordered_set< const symbol_t * >
defined_symbols(const unit_t &unit)
{
    std::unordered_set< const symbol_t * > defined;
    for (const auto &fd : unit.functions)
        if (fd.body)
            defined.insert(fd.sym);

    for (const auto &od : unit.objects)
        defined.insert(od.sym);

    return defined;
}

// The pointer of a subscript by a constant 0, p[0] or 0[p]: the pointer
// operand of the add, or null for any other pointer.  The address such
// a subscript designates has no displacement and no index, whatever the
// pointee's size; it is the bare base, as *p's is.  A row of an array
// of arrays is the same case, m[0] the address of m.
const expr_t *
bare_base(const expr_t &p)
{
    if (!is< binary_t >(p) || as< binary_t >(p).op != binary_op_t::add)
        return nullptr;

    const auto &b = as< binary_t >(p);
    const bool left = is< pointer_t >(b.lhs->type);
    const auto &n = left ? *b.rhs : *b.lhs;

    if (!is< literal_t >(n) || as< literal_t >(n).value != 0)
        return nullptr;

    return left ? b.lhs.get() : b.rhs.get();
}

// functions

// A string literal's array, 6.4.5p6: one per literal met, in the order
// met, never merged with an equal one, which 6.4.5p7 leaves
// unspecified, and writable (G17).  literals() writes them to .data.
std::string
string_label(std::vector< std::string > &strings, const string_t &str)
{
    const auto label = std::format(".Ls{}", strings.size());
    strings.push_back(str.bytes);

    return label;
}

// The file name as a .file directive's string.
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

// A string literal's bytes as the operand of .string: printable ASCII
// as itself but " and \ escaped, a newline, a tab and a return as the
// C escapes the assembler reads, every other byte a three-digit octal
// escape.  The directive appends the terminating zero, 6.4.5p6.
namespace {

std::string
quoted_bytes(std::string_view bytes)
{
    std::string text = "\"";
    for (const char c : bytes) {
        const auto byte = static_cast< unsigned char >(c);

        if (c == '\n' || c == '\t' || c == '\r') {
            text += c == '\n' ? "\\n" : c == '\t' ? "\\t" : "\\r";
            continue;
        }

        if (byte < 0x20 || byte > 0x7e) {
            text += std::format("\\{:03o}", byte);
            continue;
        }

        if (c == '"' || c == '\\')
            text += '\\';
        text += c;
    }

    return text + '"';
}

// Chapter 10: the string literals, each its own labeled array in .data,
// never .rodata and never merged (G17), with no alignment directive,
// since a char array aligns to 1.
void
literals(std::ostream &out, const std::vector< std::string > &strings)
{
    if (strings.empty())
        return;

    out << "\t.data\n";

    for (std::size_t i = 0; i < strings.size(); ++i)
        out << std::format(".Ls{}:\n\t.string\t{}\n", i,
                           quoted_bytes(strings[i]));
}

// The directive of a value of size bytes, which both targets'
// assemblers take: .byte, .short, .long or .quad.
std::string_view
data_directive(long size)
{
    switch (size) {
    case 1: return ".byte";
    case 2: return ".short";
    case 4: return ".long";
    default: return ".quad";
    }
}

// A constant as its directive writes it: the value of the object's type
// t, read unsigned for an unsigned type or a pointer, which the parser
// holds as its bits past LLONG_MAX.
std::string
constant_value(const literal_t &constant, const type_t &t)
{
    if (integer(t).is_signed)
        return std::to_string(constant.value);

    return std::to_string(static_cast< unsigned long long >(constant.value));
}

// An address constant as its directive writes it, a symbol and its
// offset, n, a+12, .Ls0+1: the name in the assembly of the global or
// the function, or label, the label of the string literal.  The
// assembler leaves the symbol to a relocation, which the linker
// resolves or leaves to the loader, for a symbol the file defines and
// for one it only declares alike.
std::string
address_value(const address_t &address, std::string_view label)
{
    std::string text { label };
    if (const auto *sym = std::get_if< const symbol_t * >(&address.base))
        text = assembly_name(**sym);

    const auto offset = static_cast< unsigned long long >(address.offset);
    if (address.offset > 0)
        text += std::format("+{}", offset);
    else if (address.offset < 0)
        text += std::format("-{}", 0 - offset);

    return text;
}

// The bytes of an array of char that a string literal initializes,
// 6.7.11p15, in the escapes of the literals' block: .string, which
// appends the terminating zero, when the array has room for it, and
// .ascii, which appends none, when the literal fills the array; then
// .zero for the elements past the literal's, p22.
void
string_data(std::ostream &out, const string_t &str, long size)
{
    const long length = long(str.bytes.size());
    const auto directive = size > length ? ".string" : ".ascii";

    out << std::format("\t{}\t{}\n", directive, quoted_bytes(str.bytes));

    if (size > length + 1)
        out << std::format("\t.zero\t{}\n", size - length - 1);
}

// The value an object's initializer gives it, or its zeros: a constant
// in the directive of the object's size, an address in a .quad, or an
// array's bytes.  label is the label of the string literal the address
// points into, if it does.
void
initial_data(std::ostream &out,
             const object_decl_t &od,
             std::string_view label,
             long size)
{
    if (!od.init) {
        out << std::format("\t.zero\t{}\n", size);
        return;
    }

    if (const auto *constant = std::get_if< literal_t >(&*od.init))
        out << std::format("\t{}\t{}\n", data_directive(size),
                           constant_value(*constant, od.sym->type));
    else if (const auto *address = std::get_if< address_t >(&*od.init))
        out << std::format("\t.quad\t{}\n", address_value(*address, label));
    else
        string_data(out, std::get< string_t >(*od.init), size);
}

// The alignment of an object the unit defines, chapter 10: its type's,
// 6.2.8, which aarch64's scaled :lo12: accesses rely on; an array of
// the target's array_alignment in bytes or more takes at least that,
// 16 on x86-64 by its psABI, since code another compiler writes for the
// array's declaration may assume it.
int
object_alignment(const type_t &t, const target_t &target)
{
    const int align = align_of(t);
    if (is< array_t >(t) && size_of(t) >= target.array_alignment)
        return std::max(align, target.array_alignment);

    return align;
}

// One object the unit defines, chapter 10: .globl, since every object
// has external linkage; .type and .size, the symbol's kind and its
// bytes in the symbol table, which the linker and a debugger read; its
// alignment, and no directive for an alignment of 1; the label; then
// the value of its initializer, or its zeros.
void
object(std::ostream &out,
       const object_decl_t &od,
       std::string_view label,
       const target_t &target)
{
    const auto name = assembly_name(*od.sym);
    const long size = size_of(od.sym->type);
    const int align = object_alignment(od.sym->type, target);

    out << std::format("\t.globl\t{}\n"
                       "\t.type\t{}, @object\n"
                       "\t.size\t{}, {}\n",
                       name, name, name, size);

    if (align > 1)
        out << std::format("\t.p2align {}\n",
                           std::countr_zero(static_cast< unsigned >(align)));

    out << name << ":\n";

    initial_data(out, od, label, size);
}

// One block of objects, under its section's directive, or nothing when
// no object belongs to it: those with an initializer or those without.
// labels holds the label of each object's string literal, by its index.
void
object_block(std::ostream &out,
             const std::vector< object_decl_t > &objects,
             const std::vector< std::string > &labels,
             const target_t &target,
             bool initialized)
{
    bool opened = false;

    for (std::size_t i = 0; i < objects.size(); ++i) {
        if (objects[i].init.has_value() != initialized)
            continue;

        if (!opened)
            out << (initialized ? "\t.data\n" : "\t.bss\n");
        opened = true;

        object(out, objects[i], labels[i], target);
    }
}

// The label of the string literal each object's initializer points
// into, by the object's index, or empty for an object whose initializer
// names none.  Each such literal is one more literal of the file,
// added to strings after those the functions name, in the order of the
// definitions, so no function's literal changes its number.
std::vector< std::string >
initializer_literals(std::vector< std::string > &strings,
                     const std::vector< object_decl_t > &objects)
{
    std::vector< std::string > labels;

    for (const auto &od : objects) {
        const auto *address =
            od.init ? std::get_if< address_t >(&*od.init) : nullptr;
        const auto *str =
            address ? std::get_if< string_t >(&address->base) : nullptr;

        labels.push_back(str ? string_label(strings, *str) : std::string());
    }

    return labels;
}

} // namespace

// Chapter 10: the data after the last function.  First the string
// literals, those the functions name, strings, and after them those the
// objects' initializers point into.  Then the objects the unit defines,
// in the order of their definitions: one .data block of those with an
// initializer, then one .bss block of those without, which are zero,
// 6.7.11p11.  int n = 0; is in .data, since the source wrote its
// initializer, and so is an object an address initializes, which the
// loader writes.  An object the unit only declares writes nothing.
void
data(std::ostream &out,
     std::vector< std::string > strings,
     const std::vector< object_decl_t > &objects,
     const target_t &target)
{
    const auto labels = initializer_literals(strings, objects);

    literals(out, strings);
    object_block(out, objects, labels, target, true);
    object_block(out, objects, labels, target, false);
}

} // namespace rcc
