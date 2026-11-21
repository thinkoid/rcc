// -*- mode: c++; -*-

// The part of c1 that is the same for every target, declared in
// c1-common.hh: the integer view of a type, the symbol and label names,
// the tree predicates, the rule that chooses a switch's form, and the
// blocks of the string literals and of the objects.  Nothing here writes
// an instruction.

#include "c1-common.hh"

#include <algorithm>
#include <bit>
#include <format>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace rcc {

refusal_t::refusal_t(const std::string &construct)
    : std::logic_error("c1: not yet: " + construct), construct(construct)
{
}

[[noreturn]] void
not_yet(std::string_view construct)
{
    throw refusal_t(std::string(construct));
}

// The tree carries no positions, so the place a refusal names is the
// function, quoted as the parser quotes a name: "c1: not yet: a frame
// of 2 GiB or more, in function 'main'".
[[noreturn]] void
refused_in(std::string_view function, const refusal_t &e)
{
    not_yet(std::format("{}, in function '{}'", e.construct, function));
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

// Two byte offsets added modulo 2^64: G6, the address arithmetic wraps.
long long
wrapped_add(long long a, long long b)
{
    return static_cast< long long >(static_cast< unsigned long long >(a) +
                                    static_cast< unsigned long long >(b));
}

// The width a value is valid to after an instruction writes it at
// width: a 32-bit write clears the upper half of the register, which
// is the extension of an unsigned value and not of a signed one.
int
written(int width, bool is_signed)
{
    return width == 32 && !is_signed ? 64 : width;
}

// The width the value of a compound assignment or a step is valid to,
// backend.md 6.3, the width rule: the value has the object's type, so
// the bits above the object's width are the operation's and are cut;
// but an operation at the object's own width wrote the value at it, so
// the value is valid as that write leaves it, an unsigned of 32 bits
// to 64.  valid is what the operation's row left, width the operation's.
int
stored_valid(int valid, int width, const int_t &object)
{
    const int cut = width == object.width ?
                        written(object.width, object.is_signed) :
                        object.width;

    return std::min(valid, cut);
}

// The units of a record's copy and of the zeroing of a record or an
// array, backend.md 6.6, from the object's size and alignment, an
// array's its element's: the alignment, at most 8 bytes, a unit the
// machine loads and stores in one access, which tiles the size; more
// than eight units are a loop, which writes the same instructions
// whatever the count (G18, G20).
units_t
units_of(long size, int alignment)
{
    const int unit = std::min(alignment, 8);
    const long count = size / unit;

    return { unit, count, count > 8 };
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

// A symbol and a byte offset as the assembler's expression, table,
// table+12, table-4, .Ls0+1: the address the relocation resolves, the
// operand of an address constant's .quad (backend.md chapter 10) and of
// an access by a name and a constant (6.3).
std::string
symbol_plus(std::string_view symbol, long long offset)
{
    std::string text { symbol };

    const auto bits = static_cast< unsigned long long >(offset);
    if (offset > 0)
        text += std::format("+{}", bits);
    else if (offset < 0)
        text += std::format("-{}", 0 - bits);

    return text;
}

// The binding of a symbol the unit defines, its .globl, chapter 10:
// one of external linkage is global, and the linker binds every
// file's references to the name to it; any other, a static function or
// object, is local to the file's object, which writes no directive for
// it, and only the file's own references reach it.
void
binding(std::ostream &out, const symbol_t &sym)
{
    if (sym.linkage == linkage_t::external)
        out << std::format("\t.globl\t{}\n", assembly_name(sym));
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
// symbol its register, or declared one past the target's count an
// ordinary local.
const symbol_t *
register_variable(const expr_t &e)
{
    if (!is< name_t >(e))
        return nullptr;

    const auto *sym = as< name_t >(e).sym;

    return in_register(*sym) ? sym : nullptr;
}

// Whether e reads an object in memory, by the load row of backend.md
// 6.3: a name that is no register variable, a local's or a global's,
// or a dereference.  A register variable's read is its register.
bool
in_memory(const expr_t &e)
{
    if (is< unary_t >(e))
        return as< unary_t >(e).op == unary_op_t::deref;

    return is< name_t >(e) && !register_variable(e);
}

bool
is_step(unary_op_t op)
{
    return op == unary_op_t::pre_inc || op == unary_op_t::pre_dec ||
           op == unary_op_t::post_inc || op == unary_op_t::post_dec;
}

namespace {

template< typename F >
bool any_expression(const stmt_t &, const F &);

// Whether test holds of an expression of the statements, at any depth of
// the statements they hold: an expression statement's, a declaration's
// initializer, the condition of an if, a switch or a loop, a for's step
// and a return's value.
template< typename F >
bool
any_expression(const std::vector< stmt_t > &items, const F &test)
{
    return std::ranges::any_of(
        items, [&test](const stmt_t &st) { return any_expression(st, test); });
}

template< typename F >
bool
any_expression(const stmt_ptr_t &st, const F &test)
{
    return st && any_expression(*st, test);
}

template< typename F >
bool
any_expression(const std::optional< expr_t > &e, const F &test)
{
    return e && test(*e);
}

template< typename F >
bool
any_expression(const stmt_t &st, const F &test)
{
    if (is< expr_stmt_t >(st))
        return any_expression(as< expr_stmt_t >(st).e, test);

    if (is< decl_stmt_t >(st))
        return any_expression(as< decl_stmt_t >(st).init, test);

    if (is< block_t >(st))
        return any_expression(as< block_t >(st).body, test);

    if (is< if_t >(st)) {
        const auto &i = as< if_t >(st);
        return test(i.cond) || any_expression(i.yes, test) ||
               any_expression(i.no, test);
    }

    if (is< switch_t >(st))
        return test(as< switch_t >(st).cond) ||
               any_expression(as< switch_t >(st).body, test);

    if (is< while_t >(st))
        return test(as< while_t >(st).cond) ||
               any_expression(as< while_t >(st).body, test);

    if (is< do_t >(st))
        return any_expression(as< do_t >(st).body, test) ||
               test(as< do_t >(st).cond);

    if (is< for_t >(st)) {
        const auto &f = as< for_t >(st);
        return any_expression(f.init, test) || any_expression(f.cond, test) ||
               any_expression(f.step, test) || any_expression(f.body, test);
    }

    if (is< return_t >(st))
        return any_expression(as< return_t >(st).value, test);

    return false;
}

// Whether test holds of an expression of a statement expression: of its
// statements, or of its value.
template< typename F >
bool
any_expression(const statement_expression_t &x, const F &test)
{
    return any_expression(x.body, test) || (x.value && test(*x.value));
}

} // namespace

// Whether evaluating e may write the register variable sym: an
// assignment to it or a ++ or -- of it anywhere in e, in the statements
// of a statement expression in e among them.  A declaration there
// writes the block's own variable, which no read held outside the block
// is of.
bool
writes(const expr_t &e, const symbol_t *sym)
{
    if (is< statement_expression_t >(e))
        return any_expression(
            as< statement_expression_t >(e),
            [sym](const expr_t &operand) { return writes(operand, sym); });

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
        return writes(*c.cond, sym) || (c.yes && writes(*c.yes, sym)) ||
               writes(*c.no, sym);
    }

    return false;
}

// Whether a cast keeps every bit of its operand: an integer or a
// pointer to an integer or a pointer of the same width, so it changes
// the signedness alone, int to unsigned, long to unsigned long, or the
// view of 64 bits, a pointer to a long or back, one pointer type to
// another.  It writes no instruction (backend.md 6.3, the row of (T) a).
bool
same_bits(const cast_t &c, const type_t &to)
{
    const auto &from = c.arg->type;
    const bool scalars = (is< int_t >(from) || is< pointer_t >(from)) &&
                         (is< int_t >(to) || is< pointer_t >(to));

    return scalars && integer(from).width == integer(to).width;
}

// The operand under the casts that keep its bits, same_bits()'s, which
// write no instruction (backend.md 6.3): q of (int *) q on an int *q.
const expr_t &
under_kept_bits(const expr_t &e)
{
    const expr_t *top = &e;
    while (is< cast_t >(*top) && same_bits(as< cast_t >(*top), top->type))
        top = as< cast_t >(*top).arg.get();

    return *top;
}

// The object in memory a conversion's operand reads, or nullptr: the
// operand, or the read under casts that keep its bits, (long) (int) u's
// u, an object of the operand's width either way.  The conversion's own
// load may read it (backend.md 6.3, a widening by the load; chapter 4).
const expr_t *
object_under(const expr_t &operand)
{
    const expr_t &top = under_kept_bits(operand);

    return in_memory(top) ? &top : nullptr;
}

// The object in memory a cast's operand reads, object_under()'s.
const expr_t *
object_under(const cast_t &c)
{
    return object_under(*c.arg);
}

// The object in memory a widening of e reads in one load, and the type
// the load extends it by, backend.md 6.3: e's own object, under casts
// that keep its bits, object_under()'s, extended by e's type; or the
// object under a widening of e's own, the promotion of a char or a
// short, extended by the type it is promoted from, where one extension
// is the two: a signed one under a signed widening, (long) (int) c on
// a signed char c being movsbq -1(%rbp), %rax on x86-64 and ldrsb x0,
// [x29, #16] on aarch64, and any unsigned one, whose zero extension the
// promotion keeps, movzbl -1(%rbp), %eax and ldrb w0, [x29, #16] on an
// unsigned char, valid to 64 bits.  The object is read once at its own
// width (G10).  None for any other e, a signed object under an unsigned
// widening among them, whose extension to that width is no extension
// to the outer one.
widened_read_t
widened_read(const expr_t &e)
{
    if (const auto *object = object_under(e))
        return { object, &integer(e.type) };

    if (!is< cast_t >(e) || !is< int_t >(e.type))
        return { };

    const auto &c = as< cast_t >(e);
    if (!is< int_t >(c.arg->type))
        return { };

    const auto &from = integer(c.arg->type);
    const auto &promoted = integer(e.type);
    const auto *object = object_under(c);
    if (!object || promoted.width <= from.width ||
        (from.is_signed && !promoted.is_signed))
        return { };

    return { object, &from };
}

// Whether a conversion to the type to changes the signedness alone, of
// a value handed a register variable (backend.md chapter 4): two
// integer types of one width, the variable not yet written, and its
// value computed in its own type.
bool
retypes(const cast_t &c, const type_t &to, const into_t *into)
{
    if (!into || into->taken || into->as)
        return false;

    if (!is< int_t >(to) || !is< int_t >(c.arg->type))
        return false;

    const auto &from = integer(c.arg->type);
    const auto &target = integer(to);

    return from.width == target.width && from.is_signed != target.is_signed;
}

// Whether e names the register variable sym anywhere, to read it or to
// write it, in the statements of a statement expression in e among
// them.  Such a variable has no address, 6.5.4.2p1, so no other
// expression can reach it.
bool
names(const expr_t &e, const symbol_t *sym)
{
    if (is< name_t >(e))
        return as< name_t >(e).sym == sym;

    if (is< statement_expression_t >(e))
        return any_expression(
            as< statement_expression_t >(e),
            [sym](const expr_t &operand) { return names(operand, sym); });

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
        return names(*c.cond, sym) || (c.yes && names(*c.yes, sym)) ||
               names(*c.no, sym);
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

// Whether a and b may change places under op: +, *, &, | and ^, whose
// value is the same either way (backend.md 6.1).
bool
commutes(binary_op_t op)
{
    switch (op) {
    case binary_op_t::add:
    case binary_op_t::mul:
    case binary_op_t::bit_and:
    case binary_op_t::bit_or:
    case binary_op_t::bit_xor: return true;
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

// The operator that holds of b and a exactly when op holds of a and b:
// 5 < x is x > 5, and an operator that commutes is itself, 5 + x is
// x + 5.  The Seventh Edition compiler's maprel[].
binary_op_t
mirrored(binary_op_t op)
{
    switch (op) {
    case binary_op_t::lt: return binary_op_t::gt;
    case binary_op_t::gt: return binary_op_t::lt;
    case binary_op_t::le: return binary_op_t::ge;
    case binary_op_t::ge: return binary_op_t::le;
    default: return op;
    }
}

// An operation with its operands in the order the source has them.
operation_t
as_written(const binary_t &b)
{
    return { b.op, b.lhs.get(), b.rhs.get() };
}

// Whether an operation's left operand is a constant and its right one
// is not, under an operator whose operands may change places, a
// comparison or one that commutes (backend.md 6.1): the parser's folded
// constant, a literal, the null pointer of 0 == p among them; an
// address constant, &g, is no literal.  -, /, % and the shifts keep
// their order.
bool
constant_first(const binary_t &b)
{
    if (!is_comparison(b.op) && !commutes(b.op))
        return false;

    return is< literal_t >(*b.lhs) && !is< literal_t >(*b.rhs);
}

// An operation with its constant on the left generated with the
// operands exchanged, 5 < x as x > 5 and 5 + x as x + 5 (backend.md
// 6.1): the constant has no evaluation, so G13 has nothing to order,
// and the right operand is evaluated once, first.  The operands' type
// is common, so the signedness and the width are the same.
operation_t
mirrored(const binary_t &b)
{
    return { mirrored(b.op), b.rhs.get(), b.lhs.get() };
}

// switches

// The bits of a value of width bits read as an unsigned number: the
// (unsigned) of the rule's hash, backend.md 7.3.
unsigned long long
unsigned_bits(long long value, int width)
{
    const auto bits = static_cast< unsigned long long >(value);

    return width == 64 ? bits : bits & ((1ull << width) - 1);
}

// A switch's cases sorted by value, in the order of t, the controlling
// expression's promoted type: signed or unsigned, at its width.
std::vector< case_t >
sorted_cases(std::vector< case_t > cases, const int_t &t)
{
    std::ranges::sort(cases, [&t](const case_t &a, const case_t &b) {
        if (t.is_signed)
            return a.value < b.value;

        return unsigned_bits(a.value, t.width) <
               unsigned_bits(b.value, t.width);
    });

    return cases;
}

// A value less the first of a switch's sorted cases, as an unsigned
// number of width bits: its entry in a direct table, backend.md 7.3.
unsigned long long
case_offset(long long value, long long first, int width)
{
    const auto difference = static_cast< unsigned long long >(value) -
                            static_cast< unsigned long long >(first);

    return unsigned_bits(static_cast< long long >(difference), width);
}

// The last value of sorted cases less the first, the rule's range.
unsigned long long
case_range(const std::vector< case_t > &sorted, int width)
{
    return case_offset(sorted.back().value, sorted.front().value, width);
}

// The form of a switch's dispatch on sorted cases, by the rule of the
// Seventh Edition compiler, backend.md 7.3: none without a case; direct
// when the range is above 0 and at most three times the count of the
// cases; simple below ten cases; hash otherwise.
dispatch_t
dispatch_of(const std::vector< case_t > &sorted, int width)
{
    if (sorted.empty())
        return dispatch_t::none;

    const unsigned long long count = sorted.size();
    const auto range = case_range(sorted, width);
    if (range > 0 && range <= 3 * count)
        return dispatch_t::direct;

    if (count < 10)
        return dispatch_t::simple;

    return dispatch_t::hash;
}

// The size of a hash switch's table, backend.md 7.3: the i from a
// quarter of the count of the cases to a half that makes i times the
// most cases one remainder modulo i holds the least, the first such i
// on a tie.  A value's remainder is of its bits read unsigned.  The hash
// form has ten cases or more, so i is 2 or more.
unsigned long long
table_size(const std::vector< case_t > &sorted, int width)
{
    const unsigned long long count = sorted.size();

    unsigned long long tabs = 0;
    auto best = std::numeric_limits< unsigned long long >::max();
    for (auto i = count / 4; i <= count / 2; ++i) {
        std::vector< unsigned long long > held(i);
        for (const auto &c : sorted)
            ++held[unsigned_bits(c.value, width) % i];

        const auto worst = std::ranges::max(held);
        if (i * worst < best) {
            tabs = i;
            best = i * worst;
        }
    }

    return tabs;
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

// A pointer that is an lvalue's address plus a constant (backend.md
// 6.3): the decay of an array lvalue, 6.3.2.1p3, or & of an lvalue,
// 6.5.4.2p3, the constant 0, so *a is a[0] and *&x is x; or a sum of
// such a pointer and an integer constant, p + c, c + p or p - c, the
// constant times the pointee's size added or subtracted modulo 2^64
// (G6); or such a pointer under a cast to another pointer type, which
// keeps the address (6.3.2.3p7), so that (int *) ((char *) &s + 4) is
// s and 4, each sum's constant scaled by its own pointee's size.  The
// decay or the &, the lvalue under it, and the offset in bytes.  None
// for any other pointer, a variable index, a pointer object's value and
// any other cast among them.
std::optional< lvalue_offset_t >
lvalue_offset(const expr_t &pointer)
{
    if (is< cast_t >(pointer) && is< array_t >(as< cast_t >(pointer).arg->type))
        return lvalue_offset_t { .address = &pointer,
                                 .lvalue = as< cast_t >(pointer).arg.get(),
                                 .offset = 0 };

    if (is< cast_t >(pointer) && is< pointer_t >(pointer.type) &&
        is< pointer_t >(as< cast_t >(pointer).arg->type))
        return lvalue_offset(*as< cast_t >(pointer).arg);

    if (is< unary_t >(pointer) && as< unary_t >(pointer).op == unary_op_t::addr)
        return lvalue_offset_t { .address = &pointer,
                                 .lvalue = as< unary_t >(pointer).arg.get(),
                                 .offset = 0 };

    if (!is< binary_t >(pointer) || !is< pointer_t >(pointer.type))
        return std::nullopt;

    const auto &b = as< binary_t >(pointer);
    if (b.op != binary_op_t::add && b.op != binary_op_t::sub)
        return std::nullopt;

    const bool left = is< pointer_t >(b.lhs->type);
    const auto &p = left ? *b.lhs : *b.rhs;
    const auto &n = left ? *b.rhs : *b.lhs;
    if (!is< literal_t >(n))
        return std::nullopt;

    auto at = lvalue_offset(p);
    if (!at)
        return std::nullopt;

    const long size = size_of(*as< pointer_t >(pointer.type).pointee);
    const auto bytes = scaled(as< literal_t >(n).value, size);
    const auto negated = static_cast< long long >(
        0ull - static_cast< unsigned long long >(bytes));

    at->offset =
        wrapped_add(at->offset, b.op == binary_op_t::add ? bytes : negated);

    return at;
}

// The lvalue at the root of an address by a name and a constant
// (backend.md 6.3), offset 0: a local's or a parameter's name, whose
// address is its slot; the name of a global the unit defines, or a
// string literal, whose address is its label; or the dereference of
// such an address, whose address it is, so m[1][2] composes.  None for
// any other lvalue, a global the unit only declares among them, whose
// address comes from the GOT.
std::optional< named_offset_t >
named_lvalue(const expr_t &e,
             const std::unordered_set< const symbol_t * > &defined)
{
    if (is< unary_t >(e) && as< unary_t >(e).op == unary_op_t::deref)
        return named_offset(*as< unary_t >(e).arg, defined);

    if (is< string_t >(e))
        return named_offset_t { .object = &e, .offset = 0 };

    if (!is< name_t >(e))
        return std::nullopt;

    const auto &sym = *as< name_t >(e).sym;
    const bool slot = (sym.storage == storage_t::local ||
                       sym.storage == storage_t::parameter) &&
                      !in_register(sym);
    const bool label =
        sym.storage == storage_t::global && defined.contains(&sym);
    if (!slot && !label)
        return std::nullopt;

    return named_offset_t { .object = &e, .offset = 0 };
}

// A pointer that is a name's address plus a constant: lvalue_offset()'s
// shape on an lvalue named_lvalue() roots at a slot or a label, the
// offsets of each level summed.  The lvalue and the offset.  Ritchie's
// c1 subsumes the constant of "&x+c" into the name's offset; the
// generators fold it into the operand's displacement, so no instruction
// forms the object's address first.
std::optional< named_offset_t >
named_offset(const expr_t &pointer,
             const std::unordered_set< const symbol_t * > &defined)
{
    const auto at = lvalue_offset(pointer);
    if (!at)
        return std::nullopt;

    auto named = named_lvalue(*at->lvalue, defined);
    if (!named)
        return std::nullopt;

    named->offset = wrapped_add(named->offset, at->offset);

    return named;
}

// Whether a label's offset folds, backend.md 6.3: from 0 to the
// object's size, one past the end included (6.5.7p9).  The linker
// resolves a label and an offset, and one outside the object it may
// not reach.  An array of unknown size has no bound and does not fold.
bool
within_object(const named_offset_t &named)
{
    const auto &object = *named.object;
    const auto &type =
        is< name_t >(object) ? as< name_t >(object).sym->type : object.type;
    if (is< array_t >(type) && as< array_t >(type).count < 0)
        return false;

    return named.offset >= 0 && named.offset <= size_of(type);
}

// functions

namespace {

std::string quoted_bytes(std::string_view bytes);
std::string wide_elements(const string_t &str, long count);

} // namespace

// A string literal's array, 6.4.5p6: one per literal met, in the order
// met, never merged with an equal one, which 6.4.5p7 leaves
// unspecified, and writable (G17).  strings holds each one's data as
// literals() writes it to .data, its label among it: a char literal's
// bytes under .string, which appends the zero; a wide literal aligned to
// its element, its elements under the directive of their width and a
// zero element after them.
std::string
string_label(std::vector< std::string > &strings, const string_t &str)
{
    const auto label = std::format(".Ls{}", strings.size());

    if (str.width == 1) {
        strings.push_back(std::format("{}:\n\t.string\t{}\n", label,
                                      quoted_bytes(str.bytes)));
        return label;
    }

    const long count = long(str.bytes.size()) / str.width + 1;
    strings.push_back(std::format("\t.p2align {}\n{}:\n{}",
                                  str.width == 2 ? 1 : 2, label,
                                  wide_elements(str, count)));

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

// The first count elements of a wide string literal, as one directive
// of their width, .short or .long, each read from its bytes,
// little-endian; the elements past its own are the zero appended,
// 6.4.5p6.
std::string
wide_elements(const string_t &str, long count)
{
    const long length = long(str.bytes.size()) / str.width;

    std::string values;
    for (long i = 0; i < count; ++i) {
        unsigned long element = 0;
        for (int b = str.width - 1; b >= 0 && i < length; --b)
            element = element << 8 | static_cast< unsigned char >(
                                         str.bytes[i * str.width + b]);

        values += std::format("{}{}", i > 0 ? ", " : "", element);
    }

    return std::format("\t{}\t{}\n", data_directive(str.width), values);
}

// Chapter 10: the string literals, each its own labeled array in .data,
// never .rodata and never merged (G17); a char array aligns to 1, so a
// char literal has no alignment directive, and a wide one has its
// element's.
void
literals(std::ostream &out, const std::vector< std::string > &strings)
{
    if (strings.empty())
        return;

    out << "\t.data\n";

    for (const std::string &literal : strings)
        out << literal;
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
    std::string_view symbol = label;
    if (const auto *sym = std::get_if< const symbol_t * >(&address.base))
        symbol = assembly_name(**sym);

    return symbol_plus(symbol, address.offset);
}

// The bytes of an array of char that a string literal initializes,
// 6.7.11p15, in the escapes of the literals' block: .string, which
// appends the terminating zero, when the array has room for it, and
// .ascii, which appends none, when the literal fills the array.  An
// array of a wide literal's element type, p16, takes its elements, and
// the zero element when it has room.  Returns the bytes written; the
// elements past them are zero, p22.
long
string_data(std::ostream &out, const string_t &str, long size)
{
    const long length = long(str.bytes.size());

    if (str.width > 1) {
        const long count = std::min(size, length + str.width) / str.width;
        out << wide_elements(str, count);

        return count * str.width;
    }

    const auto directive = size > length ? ".string" : ".ascii";
    out << std::format("\t{}\t{}\n", directive, quoted_bytes(str.bytes));

    return std::min(size, length + 1);
}

// The value of one subobject, at its own width: a constant in the
// directive of its size, an address in a .quad, or an array's bytes.
// label is the label of the string literal the address points into, if
// it does.  Returns the bytes written.
long
value_data(std::ostream &out, const subobject_t &sub, std::string_view label)
{
    const long size = size_of(sub.type);

    if (const auto *constant = std::get_if< literal_t >(&sub.value)) {
        out << std::format("\t{}\t{}\n", data_directive(size),
                           constant_value(*constant, sub.type));
        return size;
    }

    if (const auto *address = std::get_if< address_t >(&sub.value)) {
        out << std::format("\t.quad\t{}\n", address_value(*address, label));
        return size;
    }

    return string_data(out, std::get< string_t >(sub.value), size);
}

// A run of bytes no subobject holds, which are zero, 6.7.11p11: one
// .zero, or nothing for an empty run.
void
zero_data(std::ostream &out, long bytes)
{
    if (bytes > 0)
        out << std::format("\t.zero\t{}\n", bytes);
}

// The value an object's initializer gives it, exactly its size in bytes:
// each subobject's value in the order of their offsets, and a .zero for
// each run of bytes between them and after the last that none holds.
// An object without an initializer, or with {}, is one .zero.  labels
// holds the label of the string literal each subobject's address points
// into, by the subobject's index.
void
initial_data(std::ostream &out,
             const object_decl_t &od,
             const std::vector< std::string > &labels,
             long size)
{
    long at = 0;

    if (od.init)
        for (std::size_t i = 0; i < od.init->size(); ++i) {
            const subobject_t &sub = (*od.init)[i];

            zero_data(out, sub.offset - at);
            at = sub.offset + value_data(out, sub, labels[i]);
        }

    zero_data(out, size - at);
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

// One object the unit defines, chapter 10: its binding, .globl for one
// of external linkage; .type and .size, the symbol's kind and its bytes
// in the symbol table, which the linker and a debugger read; its
// alignment, and no directive for an alignment of 1; the label; then
// the value of its initializer, or its zeros.
void
object(std::ostream &out,
       const object_decl_t &od,
       const std::vector< std::string > &labels,
       const target_t &target)
{
    const auto name = assembly_name(*od.sym);
    const long size = size_of(od.sym->type);
    const int align = object_alignment(od.sym->type, target);

    binding(out, *od.sym);
    out << std::format("\t.type\t{}, @object\n"
                       "\t.size\t{}, {}\n",
                       name, name, size);

    if (align > 1)
        out << std::format("\t.p2align {}\n",
                           std::countr_zero(static_cast< unsigned >(align)));

    out << name << ":\n";

    initial_data(out, od, labels, size);
}

// One block of objects, under its section's directive, or nothing when
// no object belongs to it: those with an initializer or those without.
// labels holds the labels of each object's string literals, by its
// index.
void
object_block(std::ostream &out,
             const std::vector< object_decl_t > &objects,
             const std::vector< std::vector< std::string > > &labels,
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

// The label of the string literal each subobject's address points into,
// by the object's index and the subobject's, or empty for one whose
// value names none.  Each such literal is one more literal of the file,
// added to strings after those the functions name, in the order of the
// definitions and of the subobjects in each, so no function's literal
// changes its number.
std::vector< std::vector< std::string > >
initializer_literals(std::vector< std::string > &strings,
                     const std::vector< object_decl_t > &objects)
{
    std::vector< std::vector< std::string > > labels;

    for (const auto &od : objects) {
        auto &object_labels = labels.emplace_back();
        if (!od.init)
            continue;

        for (const auto &sub : *od.init) {
            const auto *address = std::get_if< address_t >(&sub.value);
            const auto *str =
                address ? std::get_if< string_t >(&address->base) : nullptr;

            object_labels.push_back(str ? string_label(strings, *str) :
                                          std::string());
        }
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

// The objects c1 writes after the functions: the unit's, but the static
// objects the body of each function it dropped declares, which nothing
// outside that body names (shape.md chapter 5).
std::vector< object_decl_t >
written_objects(const unit_t &unit,
                const std::vector< const function_decl_t * > &dropped)
{
    const auto declared_by_dropped = [&dropped](std::size_t i) {
        return std::ranges::any_of(dropped, [i](const function_decl_t *fd) {
            return i >= fd->statics_begin && i < fd->statics_end;
        });
    };

    std::vector< object_decl_t > written;
    for (std::size_t i = 0; i < unit.objects.size(); ++i)
        if (!declared_by_dropped(i))
            written.push_back(unit.objects[i]);

    return written;
}

// Chapter 10: the aliases after the data, as clang writes them.  Each
// is a symbol the assembler gives its target's address: its binding,
// .globl, as an alias is of external linkage; a function's kind,
// .type, which the linker and a debugger read; and the assignment of
// the target's symbol.  The target is the unit's, so no relocation
// names the alias.
void
aliases(std::ostream &out, const std::vector< alias_t > &aliases)
{
    for (const alias_t &a : aliases) {
        const auto name = assembly_name(*a.sym);

        binding(out, *a.sym);
        if (a.sym->storage == storage_t::function)
            out << std::format("\t.type\t{}, @function\n", name);

        out << std::format("{} = {}\n", name, assembly_name(*a.target));
    }
}

} // namespace rcc
