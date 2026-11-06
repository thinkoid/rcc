// -*- mode: c++; -*-

// The emitter: the typed tree in, LLVM IR text out.  Every promise of
// docs/guarantees.md is an instruction written here or a flag never
// written here; a comment names the promise where an instruction
// delivers one.  The emitter diagnoses nothing: a construct outside
// the second slice throws logic_error, since the parser makes none.

#include "emit.hh"

#include <format>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rcc {

namespace {

// An operand: an IR register or a constant, and its IR type.  A void
// value has an empty name.
struct value_t
{
    std::string name;
    std::string type;
};

// An address and the alignment proven of it, in bytes (G11).
struct address_t
{
    std::string name;
    int align;
};

// The state of one module's emission.  Everything but the stream and
// the string literals is reset per definition, so the names in a
// function depend on that function alone, and on the count of literals
// before it, and the pinned text diffs cleanly.
struct state_t
{
    std::ostream &out;

    // G17: one global's definition per literal, numbered in the order
    // the literals are emitted, written ahead of the functions
    std::vector< std::string > strings { };

    std::unordered_map< const symbol_t *, std::string > slots { };

    int temps = 0;
    int labels = 0;
    int slot_names = 0;

    type_t result { };
    std::string function { };

    std::string block { };
    bool terminated = false;
};

value_t rvalue(state_t &, const expr_t &);
void statement(state_t &, const stmt_t &);

// types

// The integer type of an operand that is not a pointer.
const int_t &
integer(const type_t &t)
{
    if (!is< int_t >(t))
        throw std::logic_error("emit: a non-integer operand");

    return as< int_t >(t);
}

// The type a parameter or a result travels as across a call: an
// integer narrower than 32 bits as the 32-bit one of its signedness,
// every other type as itself.  The slot keeps the C type; only the
// signature and the register carry this one.
type_t
passed(const type_t &t)
{
    if (!is< int_t >(t) || as< int_t >(t).width >= 32)
        return t;

    return make_int(32, as< int_t >(t).is_signed);
}

std::string ir_type(const type_t &);

// The parameter list of a function type as LLVM spells it, the
// ellipsis included.
std::string
param_list(const function_t &f)
{
    std::string list;

    for (const auto &p : f.params)
        list += (list.empty() ? "" : ", ") + ir_type(passed(*p));

    if (f.variadic)
        list += list.empty() ? "..." : ", ...";

    return list;
}

// The IR spelling of a type.  An array is the type of a slot, of a
// string's global, or the element of a step through a pointer to one;
// no value has it.
std::string
ir_type(const type_t &t)
{
    if (is< void_t >(t))
        return "void";

    if (is< int_t >(t))
        return std::format("i{}", as< int_t >(t).width);

    if (is< pointer_t >(t))
        return "ptr";

    if (is< function_t >(t)) {
        const auto &f = as< function_t >(t);
        return std::format("{} ({})", ir_type(passed(*f.result)),
                           param_list(f));
    }

    const auto &a = as< array_t >(t);
    if (a.count < 0)
        throw std::logic_error("emit: an array of unknown size");

    return std::format("[{} x {}]", a.count, ir_type(*a.element));
}

// A constant of N bits as LLVM reads it: the value modulo 2^N, written
// signed, so an unsigned constant with its top bit set is in range.
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

// writing

// A value as an instruction operand, or "void" for no value.
std::string
typed(const value_t &v)
{
    return v.name.empty() ? v.type : v.type + " " + v.name;
}

void
line(state_t &s, std::string_view text)
{
    s.out << "    " << text << '\n';
}

// An instruction that yields a value.  The register is the next
// temporary, never LLVM's implicit numbering, so no block or value
// needs a number the reader has to count.
value_t
yield(state_t &s, std::string type, std::string_view text)
{
    auto name = std::format("%t{}", ++s.temps);
    s.out << std::format("    {} = {}\n", name, text);

    return { std::move(name), std::move(type) };
}

void
terminate(state_t &s, std::string_view text)
{
    line(s, text);
    s.terminated = true;
}

// A branch to label, unless the block already ended.
void
jump(state_t &s, std::string_view label)
{
    if (!s.terminated)
        terminate(s, std::format("br label %{}", label));
}

// A two-way branch on a truth value.
void
branch(state_t &s,
       const value_t &bit,
       std::string_view on_true,
       std::string_view on_false)
{
    terminate(s, std::format("br i1 {}, label %{}, label %{}", bit.name,
                             on_true, on_false));
}

// Start the block named label.  A block still open falls through into
// it, so every block ends in exactly one terminator.
void
open(state_t &s, const std::string &label)
{
    jump(s, label);

    s.out << std::format("\n{}:\n", label);
    s.block = label;
    s.terminated = false;
}

// One number per control construct; the labels of a construct share it.
int
next_label(state_t &s)
{
    return ++s.labels;
}

// memory

// G11: a declared object carries its natural alignment and no more.
// G12: the slot lives for the whole activation; no llvm.lifetime.start
// or end is ever written.
void
allocate(state_t &s, const symbol_t *sym)
{
    auto name = std::format("%{}.{}", sym->name, s.slot_names++);
    line(s, std::format("{} = alloca {}, align {}", name, ir_type(sym->type),
                        align_of(sym->type)));

    s.slots.emplace(sym, std::move(name));
}

// A local's or a parameter's slot, of its natural alignment (G11).
address_t
slot(const state_t &s, const symbol_t *sym)
{
    return { s.slots.at(sym), align_of(sym->type) };
}

// G11, shape.md chapter 6: the alignment an access through the lvalue
// carries, which is what its address is proven to have.  A declared
// object and a string have their own.  A dereference has the one of
// the address it starts from, through any pointer arithmetic, whose
// element stride keeps it: a decayed array, the element's, or &x, x's.
// Every other address carries 1: a pointer variable, a cast, a call,
// an assignment, a conditional, a load, a pointer of unknown origin.
int
alignment_of(const expr_t &e)
{
    if (!is< unary_t >(e))
        return align_of(e.type);

    const expr_t *p = as< unary_t >(e).arg.get();
    while (is< binary_t >(*p) && is< pointer_t >(p->type) &&
           as< binary_t >(*p).op != binary_op_t::comma) {
        const auto &b = as< binary_t >(*p);
        p = is< pointer_t >(b.lhs->type) ? b.lhs.get() : b.rhs.get();
    }

    if (is< cast_t >(*p) && is< array_t >(as< cast_t >(*p).arg->type))
        return alignment_of(*as< cast_t >(*p).arg);

    if (is< unary_t >(*p) && as< unary_t >(*p).op == unary_op_t::addr)
        return alignment_of(*as< unary_t >(*p).arg);

    return 1;
}

// G5: every read is frozen, so a slot never written reads as some
// value of its type, the same at every read; never undef or poison,
// and the load carries no !range and no noundef.  A read through a
// pointer is frozen too, since the pointer may name such a slot.  G11:
// the alignment proven of the address.
value_t
load(state_t &s, const address_t &addr, const type_t &t)
{
    const auto type = ir_type(t);

    const auto raw = yield(
        s, type,
        std::format("load {}, ptr {}, align {}", type, addr.name, addr.align));

    return yield(s, type, std::format("freeze {} {}", type, raw.name));
}

// G10: no !tbaa.  G11: the alignment proven of the address.
void
store(state_t &s, const value_t &v, const address_t &addr)
{
    line(s, std::format("store {}, ptr {}, align {}", typed(v), addr.name,
                        addr.align));
}

// The bytes of a string literal as LLVM's c"..." spells them, the
// terminating zero included: printable ASCII as itself, but for the
// quote and the backslash, and every other byte as \XX.
std::string
escaped(std::string_view bytes)
{
    std::string text;

    for (const unsigned char c : bytes) {
        if (c >= 0x20 && c < 0x7f && c != '"' && c != '\\')
            text += static_cast< char >(c);
        else
            text += std::format("\\{:02X}", c);
    }

    return text + "\\00";
}

// G17: each literal written is its own array of static storage, a
// private global, neither constant nor unnamed_addr, so nothing marks
// it read-only and nothing merges it with another.  Returns its name.
std::string
string_literal(state_t &s, const string_t &str, const type_t &t)
{
    auto name = std::format("@.str.{}", s.strings.size());
    s.strings.push_back(std::format("{} = private global {} c\"{}\", align {}",
                                    name, ir_type(t), escaped(str.bytes),
                                    align_of(t)));

    return name;
}

// expressions

// The address of an lvalue: a local's or a parameter's slot, a
// string's global, or the pointer a dereference reads, 6.5.4.2p4.
address_t
lvalue(state_t &s, const expr_t &e)
{
    if (is< string_t >(e))
        return { string_literal(s, as< string_t >(e), e.type),
                 alignment_of(e) };

    if (is< unary_t >(e) && as< unary_t >(e).op == unary_op_t::deref)
        return { rvalue(s, *as< unary_t >(e).arg).name, alignment_of(e) };

    if (!is< name_t >(e))
        throw std::logic_error("emit: an lvalue other than a name, a string "
                               "or a dereference");

    const auto *sym = as< name_t >(e).sym;
    if (sym->storage != storage_t::local &&
        sym->storage != storage_t::parameter)
        throw std::logic_error("emit: a global or function as an object");

    return slot(s, sym);
}

// The zero a scalar is compared with as a truth value: 0, or the null
// pointer for a pointer, 6.3.2.3p3.
std::string_view
zero_of(const value_t &v)
{
    return v.type == "ptr" ? "null" : "0";
}

// A scalar as a condition: unequal to zero.  G7: a pointer is compared
// with null as an address.
value_t
test(state_t &s, const expr_t &e)
{
    const auto v = rvalue(s, e);

    return yield(s, "i1", std::format("icmp ne {}, {}", typed(v), zero_of(v)));
}

// A truth value widened to the expression's type, int for every
// operator that yields one.
value_t
widen(state_t &s, const value_t &bit, const type_t &t)
{
    const auto type = ir_type(t);

    return yield(s, type, std::format("zext i1 {} to {}", bit.name, type));
}

value_t
literal(const literal_t &l, const type_t &t)
{
    const auto width = integer(t).width;

    return { std::to_string(normalized(l.value, width)), ir_type(t) };
}

// G3: the count is masked by the instruction on x86 and on aarch64,
// which is G3's promise; the front end adds nothing.  No nsw, nuw or
// exact: a signed left shift wraps, a right shift drops the bits it
// shifts out.
value_t
shift(state_t &s,
      binary_op_t op,
      const value_t &l,
      const value_t &r,
      const int_t &t)
{
    if (r.type != l.type)
        throw std::logic_error("emit: a shift count of another width");

    const auto *mnemonic = op == binary_op_t::shl ? "shl" :
                           t.is_signed            ? "ashr" :
                                                    "lshr";

    return yield(s, l.type,
                 std::format("{} {}, {}", mnemonic, typed(l), r.name));
}

// An arithmetic, bitwise or shift operator on two values of one
// integer type, which is t.
value_t
apply(state_t &s,
      binary_op_t op,
      const value_t &l,
      const value_t &r,
      const int_t &t)
{
    const auto operands = std::format("{}, {}", typed(l), r.name);

    switch (op) {
    // G1: no nsw, no nuw; signed arithmetic wraps
    case binary_op_t::add: return yield(s, l.type, "add " + operands);
    case binary_op_t::sub: return yield(s, l.type, "sub " + operands);
    case binary_op_t::mul: return yield(s, l.type, "mul " + operands);

    // G4: the instruction the machine has, a fault on x86 for a zero
    // divisor or INT_MIN / -1, defined results on aarch64; no guard,
    // because nothing between here and the instruction reasons about
    // the divisor.  No exact.
    case binary_op_t::div:
        return yield(s, l.type, (t.is_signed ? "sdiv " : "udiv ") + operands);
    case binary_op_t::rem:
        return yield(s, l.type, (t.is_signed ? "srem " : "urem ") + operands);

    case binary_op_t::shl:
    case binary_op_t::shr: return shift(s, op, l, r, t);

    case binary_op_t::bit_and: return yield(s, l.type, "and " + operands);
    case binary_op_t::bit_or: return yield(s, l.type, "or " + operands);
    case binary_op_t::bit_xor: return yield(s, l.type, "xor " + operands);

    default: break;
    }

    throw std::logic_error("emit: not an arithmetic operator");
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

// A relational or equality operator.  The predicate follows the
// operands' signedness, which the parser's conversions made common.
// G7: two pointers compare as addresses, unsigned.
value_t
compare(state_t &s,
        binary_op_t op,
        const value_t &l,
        const value_t &r,
        const type_t &operands,
        const type_t &result)
{
    const bool sign = !is< pointer_t >(operands) && integer(operands).is_signed;

    const char *predicate = nullptr;
    switch (op) {
    case binary_op_t::lt: predicate = sign ? "slt" : "ult"; break;
    case binary_op_t::gt: predicate = sign ? "sgt" : "ugt"; break;
    case binary_op_t::le: predicate = sign ? "sle" : "ule"; break;
    case binary_op_t::ge: predicate = sign ? "sge" : "uge"; break;
    case binary_op_t::eq: predicate = "eq"; break;
    default: predicate = "ne"; break;
    }

    const auto bit = yield(
        s, "i1", std::format("icmp {} {}, {}", predicate, typed(l), r.name));

    return widen(s, bit, result);
}

// && and ||.  The right operand runs only when the left does not
// decide; the phi joins the deciding constant from the left's block
// with the right's test from the block the right operand ended in.
value_t
logical(state_t &s, const binary_t &b, const type_t &result)
{
    const bool is_and = b.op == binary_op_t::log_and;
    const auto *stem = is_and ? "land" : "lor";

    const int n = next_label(s);
    const auto rhs_label = std::format("{}.rhs.{}", stem, n);
    const auto end_label = std::format("{}.end.{}", stem, n);

    const auto l = test(s, *b.lhs);
    const auto from_lhs = s.block;
    if (is_and)
        branch(s, l, rhs_label, end_label);
    else
        branch(s, l, end_label, rhs_label);

    open(s, rhs_label);
    const auto r = test(s, *b.rhs);
    const auto from_rhs = s.block;

    open(s, end_label);
    const auto bit = yield(s, "i1",
                           std::format("phi i1 [ {}, %{} ], [ {}, %{} ]",
                                       is_and ? "false" : "true", from_lhs,
                                       r.name, from_rhs));

    return widen(s, bit, result);
}

// G6: the address p + n * sizeof (T), modulo 2^64.  No inbounds, nusw
// or nuw: the address computed is the address, inside an object or
// not, wrapped or not.
value_t
stepped(state_t &s, const value_t &p, const value_t &n, const type_t &pointee)
{
    return yield(s, "ptr",
                 std::format("getelementptr {}, ptr {}, {}", ir_type(pointee),
                             p.name, typed(n)));
}

// p + n or p - n, n an i64.  The subtraction is the operation written,
// so n is negated first, then stepped by; G1: no nsw on the negation.
value_t
offset(state_t &s,
       binary_op_t op,
       const value_t &p,
       const value_t &n,
       const type_t &pointee)
{
    if (op == binary_op_t::add)
        return stepped(s, p, n, pointee);

    const auto minus =
        yield(s, n.type, std::format("sub {} 0, {}", n.type, n.name));

    return stepped(s, p, minus, pointee);
}

// A pointer and an integer, p + n, n + p or p - n, the integer widened
// to 64 bits by the parser.  G13: the operands in the order written,
// whichever side the pointer is on.
value_t
pointer_arithmetic(state_t &s, const binary_t &b, const type_t &t)
{
    const auto l = rvalue(s, *b.lhs);
    const auto r = rvalue(s, *b.rhs);

    const bool left = is< pointer_t >(b.lhs->type);

    return offset(s, b.op, left ? l : r, left ? r : l,
                  *as< pointer_t >(t).pointee);
}

// p - q.  G6: the address difference divided by the element size, for
// any two pointers; sdiv without exact, as G4 divides, and no division
// for a size of 1.
value_t
difference(state_t &s, const binary_t &b)
{
    const auto l = rvalue(s, *b.lhs);
    const auto r = rvalue(s, *b.rhs);

    const auto left =
        yield(s, "i64", std::format("ptrtoint {} to i64", typed(l)));
    const auto right =
        yield(s, "i64", std::format("ptrtoint {} to i64", typed(r)));
    const auto bytes =
        yield(s, "i64", std::format("sub i64 {}, {}", left.name, right.name));

    const auto size = size_of(*as< pointer_t >(b.lhs->type).pointee);
    if (size == 1)
        return bytes;

    return yield(s, "i64", std::format("sdiv i64 {}, {}", bytes.name, size));
}

value_t
binary(state_t &s, const binary_t &b, const type_t &t)
{
    switch (b.op) {
    case binary_op_t::log_and:
    case binary_op_t::log_or: return logical(s, b, t);
    case binary_op_t::comma: rvalue(s, *b.lhs); return rvalue(s, *b.rhs);
    default: break;
    }

    if (is< pointer_t >(t))
        return pointer_arithmetic(s, b, t);

    if (b.op == binary_op_t::sub && is< pointer_t >(b.lhs->type))
        return difference(s, b);

    // G13: the left operand's code before the right's
    const auto l = rvalue(s, *b.lhs);
    const auto r = rvalue(s, *b.rhs);

    if (is_comparison(b.op))
        return compare(s, b.op, l, r, b.lhs->type, t);

    return apply(s, b.op, l, r, integer(b.lhs->type));
}

// ++ and --, either side.  G1: the add or sub of 1 wraps; G6: a pointer
// steps by one element either way.  Prefix yields the stored value,
// postfix the one loaded.
value_t
step(state_t &s, const unary_t &u)
{
    const auto &arg = *u.arg;

    const auto addr = lvalue(s, arg);
    const auto old = load(s, addr, arg.type);

    const bool up = u.op == unary_op_t::pre_inc || u.op == unary_op_t::post_inc;
    const auto now =
        is< pointer_t >(arg.type) ?
            stepped(s, old, { up ? "1" : "-1", "i64" },
                    *as< pointer_t >(arg.type).pointee) :
            yield(s, old.type,
                  std::format("{} {}, 1", up ? "add" : "sub", typed(old)));
    store(s, now, addr);

    const bool prefix =
        u.op == unary_op_t::pre_inc || u.op == unary_op_t::pre_dec;

    return prefix ? now : old;
}

value_t
unary(state_t &s, const unary_t &u, const type_t &t)
{
    switch (u.op) {
    case unary_op_t::neg: {
        // G1: no nsw, so -INT_MIN is INT_MIN
        const auto v = rvalue(s, *u.arg);
        return yield(s, v.type, std::format("sub {} 0, {}", v.type, v.name));
    }

    case unary_op_t::bit_not: {
        const auto v = rvalue(s, *u.arg);
        return yield(s, v.type, std::format("xor {}, -1", typed(v)));
    }

    case unary_op_t::log_not: {
        const auto v = rvalue(s, *u.arg);
        const auto bit =
            yield(s, "i1", std::format("icmp eq {}, {}", typed(v), zero_of(v)));
        return widen(s, bit, t);
    }

    case unary_op_t::pre_inc:
    case unary_op_t::pre_dec:
    case unary_op_t::post_inc:
    case unary_op_t::post_dec: return step(s, u);

    // the lvalue's address, nothing read
    case unary_op_t::addr: return { lvalue(s, *u.arg).name, "ptr" };

    // rvalue() reads it as the lvalue it is
    case unary_op_t::deref: throw std::logic_error("emit: unary * as a value");
    }

    throw std::logic_error("emit: an unknown unary operator");
}

// An integer conversion of a value of type from.  A change of
// signedness alone is no instruction, nor is a value already of the
// type, a pointer included.
value_t
convert(state_t &s, const value_t &v, const type_t &from, const type_t &to)
{
    if (from == to)
        return v;

    const auto &source = integer(from);
    const auto &into = integer(to);
    if (into.width == source.width)
        return v;

    const auto type = ir_type(to);

    // G2: narrowing keeps the low bits, read as two's complement
    if (into.width < source.width)
        return yield(s, type, std::format("trunc {} to {}", typed(v), type));

    const auto *widening = source.is_signed ? "sext" : "zext";

    return yield(s, type, std::format("{} {} to {}", widening, typed(v), type));
}

// Plain or compound assignment; its value is the value stored.  A
// compound one computes in the common type the parser chose, which
// the right operand already has: the old value goes into it, and the
// result back to the left operand's type before the store.  A pointer's
// += and -= step it by the right operand, an i64 as for p + n, and
// convert nothing.
value_t
assign(state_t &s, const assign_t &a)
{
    const auto &lhs = *a.lhs;
    const bool steps = a.op && is< pointer_t >(lhs.type);

    if (steps ? !(a.common == lhs.type) || integer(a.rhs->type).width != 64 :
                !(a.rhs->type == a.common))
        throw std::logic_error("emit: an assignment across two types");

    // G13: the lvalue before the rvalue, and the old value before the
    // right operand of a compound assignment
    const auto addr = lvalue(s, lhs);

    value_t v;
    if (steps) {
        const auto old = load(s, addr, lhs.type);
        const auto n = rvalue(s, *a.rhs);

        v = offset(s, *a.op, old, n, *as< pointer_t >(lhs.type).pointee);
    }
    else if (a.op) {
        const auto old = load(s, addr, lhs.type);
        const auto l = convert(s, old, lhs.type, a.common);
        const auto r = rvalue(s, *a.rhs);

        const auto result = apply(s, *a.op, l, r, integer(a.common));
        v = convert(s, result, a.common, lhs.type);
    }
    else
        v = rvalue(s, *a.rhs);

    store(s, v, addr);

    return v;
}

// A conversion by the operand's type and the target's: a discard to
// void, the decay of an array, a pointer to or from an integer, an
// integer conversion; to the same type, or pointer to pointer, no
// instruction.
value_t
cast(state_t &s, const cast_t &c, const type_t &to)
{
    const auto &arg = *c.arg;

    if (is< void_t >(to)) {
        rvalue(s, arg);
        return { "", "void" };
    }

    // 6.3.2.1p3: the address of the array is its first element's
    if (is< array_t >(arg.type))
        return { lvalue(s, arg).name, "ptr" };

    // 6.3.2.3p3: the literal 0 as a pointer is the null pointer, and
    // stays one through void *, pointer to pointer
    if (is< pointer_t >(to) && is< literal_t >(arg) &&
        as< literal_t >(arg).value == 0)
        return { "null", "ptr" };

    const auto v = rvalue(s, arg);

    if (is< pointer_t >(arg.type) && is< pointer_t >(to))
        return v;

    // G2: an integer narrower than the pointer takes its low bits
    if (is< pointer_t >(arg.type)) {
        const auto type = ir_type(to);
        return yield(s, type, std::format("ptrtoint {} to {}", typed(v), type));
    }

    // 6.3.2.3p5: the integer is widened to 64 bits in its own
    // signedness first, as the platform's compiler does
    if (is< pointer_t >(to)) {
        const auto wide =
            convert(s, v, arg.type, make_int(64, integer(arg.type).is_signed));
        return yield(s, "ptr", std::format("inttoptr {} to ptr", typed(wide)));
    }

    return convert(s, v, arg.type, to);
}

// The function a call names; the parser keeps the callee a name and
// never decays it.  An indirect call is not in the slice.
const symbol_t &
callee_of(const call_t &c)
{
    const auto &e = *c.callee;
    if (!is< name_t >(e) || as< name_t >(e).sym->storage != storage_t::function)
        throw std::logic_error("emit: an indirect call");

    return *as< name_t >(e).sym;
}

// A direct call.  The callee's type is the call site's: the parser
// sees to it in this slice.
value_t
call(state_t &s, const call_t &c)
{
    const auto &callee = callee_of(c);
    const auto &f = as< function_t >(callee.type);

    // G13: every argument, in order, before the call.  Each already has
    // its parameter's type, and one past the parameters its promoted
    // type, which the check holds the parser to.
    std::string args;
    for (std::size_t i = 0; i < c.args.size(); ++i) {
        const auto &arg = c.args[i];

        const auto param =
            i < f.params.size() ? *f.params[i] : passed(arg.type);
        if (!(arg.type == param))
            throw std::logic_error("emit: an argument unlike its parameter");

        // a narrow argument goes out extended to 32 bits, 6.5.3.3p6's
        // default argument promotion applied to every parameter
        const auto v = convert(s, rvalue(s, arg), arg.type, passed(arg.type));
        args += (args.empty() ? "" : ", ") + typed(v);
    }

    // G16: where the call site's type will differ from the callee's,
    // the callee goes through `call ptr asm "", "=r,0"` here, and the
    // call is made through the laundered pointer.
    const auto result = ir_type(passed(*f.result));
    const auto callee_type =
        f.variadic ? std::format("{} ({})", result, param_list(f)) : result;

    // G5, G9: no attribute on the call or an argument; no noundef,
    // nonnull or dereferenceable
    const auto text =
        std::format("call {} @{}({})", callee_type, callee.name, args);

    if (is< void_t >(*f.result)) {
        line(s, text);
        return { "", "void" };
    }

    // the upper bits of a narrow result are the callee's, never
    // trusted: the value is the low bits, in the result's C type
    const auto returned = yield(s, result, text);

    return convert(s, returned, passed(*f.result), *f.result);
}

value_t
conditional(state_t &s, const conditional_t &c, const type_t &t)
{
    const int n = next_label(s);
    const auto true_label = std::format("cond.true.{}", n);
    const auto false_label = std::format("cond.false.{}", n);
    const auto end_label = std::format("cond.end.{}", n);

    const auto cond = test(s, *c.cond);
    branch(s, cond, true_label, false_label);

    open(s, true_label);
    const auto yes = rvalue(s, *c.yes);
    const auto from_yes = s.block;
    jump(s, end_label);

    open(s, false_label);
    const auto no = rvalue(s, *c.no);
    const auto from_no = s.block;

    open(s, end_label);
    if (is< void_t >(t))
        return { "", "void" };

    const auto type = ir_type(t);
    return yield(s, type,
                 std::format("phi {} [ {}, %{} ], [ {}, %{} ]", type, yes.name,
                             from_yes, no.name, from_no));
}

// The value of an expression, its code written first.  No value has an
// array type: the parser decays every array it reads, 6.3.2.1p3.
value_t
rvalue(state_t &s, const expr_t &e)
{
    if (is< array_t >(e.type))
        throw std::logic_error("emit: an array-typed value");

    if (is< literal_t >(e))
        return literal(as< literal_t >(e), e.type);

    // the read of an object, named or pointed to, 6.3.2.1p2
    if (is< name_t >(e) || is_lvalue(e))
        return load(s, lvalue(s, e), e.type);

    if (is< unary_t >(e))
        return unary(s, as< unary_t >(e), e.type);

    if (is< binary_t >(e))
        return binary(s, as< binary_t >(e), e.type);

    if (is< assign_t >(e))
        return assign(s, as< assign_t >(e));

    if (is< call_t >(e))
        return call(s, as< call_t >(e));

    if (is< cast_t >(e))
        return cast(s, as< cast_t >(e), e.type);

    return conditional(s, as< conditional_t >(e), e.type);
}

// statements

void
expr_stmt(state_t &s, const expr_stmt_t &x)
{
    if (x.e)
        rvalue(s, *x.e);
}

// The slot is in the entry block already.  G5: without an initializer
// nothing is stored; the slot holds what it holds.
void
decl_stmt(state_t &s, const decl_stmt_t &d)
{
    if (d.init)
        store(s, rvalue(s, *d.init), slot(s, d.sym));
}

void
block_stmt(state_t &s, const block_t &b)
{
    for (const auto &st : b.body)
        statement(s, st);
}

void
if_stmt(state_t &s, const if_t &i)
{
    const int n = next_label(s);
    const auto then_label = std::format("if.then.{}", n);
    const auto else_label = std::format("if.else.{}", n);
    const auto end_label = std::format("if.end.{}", n);

    const auto c = test(s, i.cond);
    branch(s, c, then_label, i.no ? else_label : end_label);

    open(s, then_label);
    statement(s, *i.yes);
    jump(s, end_label);

    if (i.no) {
        open(s, else_label);
        statement(s, *i.no);
    }

    open(s, end_label);
}

// G14: the loop runs until its condition says stop; no mustprogress,
// no willreturn, no loop metadata.
void
while_stmt(state_t &s, const while_t &w)
{
    const int n = next_label(s);
    const auto cond_label = std::format("while.cond.{}", n);
    const auto body_label = std::format("while.body.{}", n);
    const auto end_label = std::format("while.end.{}", n);

    open(s, cond_label);
    const auto c = test(s, w.cond);
    branch(s, c, body_label, end_label);

    open(s, body_label);
    statement(s, *w.body);
    jump(s, cond_label);

    open(s, end_label);
}

// G14, as for while.  A for without a condition branches to its body
// unconditionally and ends only by a return.
void
for_stmt(state_t &s, const for_t &f)
{
    const int n = next_label(s);
    const auto cond_label = std::format("for.cond.{}", n);
    const auto body_label = std::format("for.body.{}", n);
    const auto step_label = std::format("for.step.{}", n);
    const auto end_label = std::format("for.end.{}", n);

    if (f.init)
        statement(s, *f.init);

    open(s, cond_label);
    if (f.cond) {
        const auto c = test(s, *f.cond);
        branch(s, c, body_label, end_label);
    }

    open(s, body_label);
    statement(s, *f.body);

    open(s, step_label);
    if (f.step)
        rvalue(s, *f.step);
    jump(s, cond_label);

    open(s, end_label);
}

// A return ends its block; what follows it goes in a fresh block, so
// the statements after a return need no special case.
void
return_stmt(state_t &s, const return_t &r)
{
    if (r.value) {
        // the register is 32 bits wide, and a narrow result fills it,
        // extended by its signedness, so the caller trusts no upper bit
        const auto v = rvalue(s, *r.value);
        terminate(s, "ret " + typed(convert(s, v, s.result, passed(s.result))));
    }
    else
        terminate(s, "ret void");

    open(s, std::format("after.ret.{}", next_label(s)));
}

void
statement(state_t &s, const stmt_t &st)
{
    if (is< expr_stmt_t >(st))
        expr_stmt(s, as< expr_stmt_t >(st));
    else if (is< decl_stmt_t >(st))
        decl_stmt(s, as< decl_stmt_t >(st));
    else if (is< block_t >(st))
        block_stmt(s, as< block_t >(st));
    else if (is< if_t >(st))
        if_stmt(s, as< if_t >(st));
    else if (is< while_t >(st))
        while_stmt(s, as< while_t >(st));
    else if (is< for_t >(st))
        for_stmt(s, as< for_t >(st));
    else
        return_stmt(s, as< return_t >(st));
}

// functions

// Every local the statement declares, at any depth, in source order.
// G12: each gets one slot in the entry block for the whole activation.
void
collect_locals(const stmt_t &st, std::vector< const symbol_t * > &locals)
{
    if (is< decl_stmt_t >(st))
        locals.push_back(as< decl_stmt_t >(st).sym);
    else if (is< block_t >(st)) {
        for (const auto &inner : as< block_t >(st).body)
            collect_locals(inner, locals);
    }
    else if (is< if_t >(st)) {
        const auto &i = as< if_t >(st);
        collect_locals(*i.yes, locals);
        if (i.no)
            collect_locals(*i.no, locals);
    }
    else if (is< while_t >(st))
        collect_locals(*as< while_t >(st).body, locals);
    else if (is< for_t >(st)) {
        const auto &f = as< for_t >(st);
        if (f.init)
            collect_locals(*f.init, locals);
        collect_locals(*f.body, locals);
    }
}

// A function that falls off its end.  5.1.2.2.3: main returns 0.
// Another non-void function returns what an unwritten slot holds; G5:
// a value of the type, not one the compiler chose.
void
finish(state_t &s)
{
    if (s.terminated)
        return;

    if (is< void_t >(s.result))
        terminate(s, "ret void");
    else if (s.function == "main")
        terminate(s, std::format("ret {} 0", ir_type(passed(s.result))));
    else {
        // extended to 32 bits as return_stmt extends a written result
        const auto v =
            load(s, { "%ret.indeterminate", align_of(s.result) }, s.result);
        terminate(s, "ret " + typed(convert(s, v, s.result, passed(s.result))));
    }
}

bool
needs_return_slot(const state_t &s)
{
    return !is< void_t >(s.result) && s.function != "main";
}

// The define line.  The incoming values are %p.0 .. %p.n-1, and the
// slot counter starts at n, so a local named p never takes one of
// their names.  G9: null_pointer_is_valid on every definition.
void
signature(state_t &s, const function_decl_t &fd)
{
    const auto &f = as< function_t >(fd.sym->type);

    std::string params;
    for (std::size_t i = 0; i < fd.params.size(); ++i)
        params += std::format("{}{} %p.{}", params.empty() ? "" : ", ",
                              ir_type(passed(fd.params[i]->type)), i);

    if (f.variadic)
        params += params.empty() ? "..." : ", ...";

    s.out << std::format("define {} @{}({}) null_pointer_is_valid {{\n",
                         ir_type(passed(*f.result)), fd.sym->name, params);
}

// The entry block's prologue: every slot, then each parameter stored
// into its own, before any statement.
void
prologue(state_t &s, const function_decl_t &fd)
{
    s.out << "entry:\n";

    s.slot_names = static_cast< int >(fd.params.size());
    for (const auto *p : fd.params)
        allocate(s, p);

    std::vector< const symbol_t * > locals;
    for (const auto &st : fd.body->body)
        collect_locals(st, locals);
    for (const auto *sym : locals)
        allocate(s, sym);

    if (needs_return_slot(s))
        line(s, std::format("%ret.indeterminate = alloca {}, align {}",
                            ir_type(s.result), align_of(s.result)));

    // FastISel cannot lower a narrow argument, so it arrives as i32,
    // and only its low bits are the value the caller converted
    for (std::size_t i = 0; i < fd.params.size(); ++i) {
        const auto *p = fd.params[i];
        const auto wide = passed(p->type);

        const value_t incoming { std::format("%p.{}", i), ir_type(wide) };
        const auto v = convert(s, incoming, wide, p->type);
        store(s, v, slot(s, p));
    }
}

// A definition.  The counters and the side table start over, so a
// function's text does not depend on the functions before it.
void
function(state_t &s, const function_decl_t &fd)
{
    s.slots.clear();
    s.temps = 0;
    s.labels = 0;
    s.result = *as< function_t >(fd.sym->type).result;
    s.function = fd.sym->name;
    s.block = "entry";
    s.terminated = false;

    signature(s, fd);
    prologue(s, fd);

    block_stmt(s, *fd.body);
    finish(s);

    s.out << "}\n";
}

void
declaration(state_t &s, const function_decl_t &fd)
{
    const auto &f = as< function_t >(fd.sym->type);

    s.out << std::format("declare {} @{}({})\n", ir_type(passed(*f.result)),
                         fd.sym->name, param_list(f));
}

} // namespace

// No target triple, no datalayout, no module attributes: llc
// supplies the host's when it reads the file.  The functions go
// through a buffer, so the string literals they name are written
// first and the IR reads top down.
void
emit(const unit_t &unit, std::ostream &out)
{
    std::ostringstream functions;
    state_t s { .out = functions };

    for (std::size_t i = 0; i < unit.functions.size(); ++i) {
        const auto &fd = unit.functions[i];

        if (i > 0)
            functions << '\n';

        if (fd.body)
            function(s, fd);
        else
            declaration(s, fd);
    }

    for (const auto &definition : s.strings)
        out << definition << '\n';

    if (!s.strings.empty())
        out << '\n';

    out << functions.str();
}

} // namespace rcc
