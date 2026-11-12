// -*- mode: c++; -*-

// The part of c1 that is the same for every target, declared in
// c1-common.hh: the integer view of a type, the label names, the tree
// predicates, and the string literals' block.  Nothing here writes an
// instruction.

#include "c1-common.hh"

#include <algorithm>
#include <format>
#include <stdexcept>
#include <string>
#include <string_view>
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

// A label of the function, chapter 7.2: .L, the function's name and
// the number, .Lsum.1.
std::string
label_name(std::string_view function, int n)
{
    return std::format(".L{}.{}", function, n);
}

// the frame

// The local or parameter a name designates, which has a slot.  A
// register variable has none, and the parser refuses its address,
// 6.5.4.2p1; a global is a later slice.
const symbol_t *
object_of(const expr_t &e)
{
    if (!is< name_t >(e))
        throw std::logic_error("c1: an lvalue the parser does not make");

    const auto *sym = as< name_t >(e).sym;
    if (sym->storage == storage_t::function)
        throw std::logic_error("c1: a function as an object");

    if (sym->storage == storage_t::global)
        not_yet("a global");

    if (sym->in_register)
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

    return sym->in_register ? sym : nullptr;
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

// The function a call names; an indirect call is a later slice.
const symbol_t &
callee_of(const call_t &c)
{
    const auto &e = *c.callee;
    if (!is< name_t >(e) || as< name_t >(e).sym->storage != storage_t::function)
        not_yet("an indirect call");

    return *as< name_t >(e).sym;
}

// functions

// The most register variables live at once in a block's items, those
// of nested blocks included: a declaration adds one to the count to its
// block's end.  It is the count c1 reaches walking the block, which is
// the parser's assignment, known before the body is generated, since
// the slots lie below the pushes.
namespace {

int registers_in(const stmt_t &);

bool
declares_register(const stmt_t &st)
{
    return is< decl_stmt_t >(st) && as< decl_stmt_t >(st).sym->in_register;
}

} // namespace

int
registers_in(const std::vector< stmt_t > &items)
{
    int live = 0;
    int most = 0;

    for (const auto &st : items) {
        live += declares_register(st);
        most = std::max(most, live + registers_in(st));
    }

    return most;
}

namespace {

int
registers_in(const stmt_t &st)
{
    if (is< block_t >(st))
        return registers_in(as< block_t >(st).body);

    if (is< if_t >(st)) {
        const auto &i = as< if_t >(st);
        return std::max(registers_in(*i.yes), i.no ? registers_in(*i.no) : 0);
    }

    if (is< while_t >(st))
        return registers_in(*as< while_t >(st).body);

    // the init's declarations live with the body, 6.8.6.4p1
    if (is< for_t >(st)) {
        const auto &f = as< for_t >(st);

        int init = 0;
        if (f.init)
            init = registers_in(*f.init) + declares_register(*f.init);

        return init + registers_in(*f.body);
    }

    return 0;
}

} // namespace

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

} // namespace

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

} // namespace rcc
