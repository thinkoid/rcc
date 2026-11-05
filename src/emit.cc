// -*- mode: c++; -*-

// The emitter: the typed tree in, LLVM IR text out.  Every promise of
// docs/guarantees.md is an instruction written here or a flag never
// written here; a comment names the promise where an instruction
// delivers one.  The emitter diagnoses nothing: a construct outside
// the first slice throws logic_error, since the parser makes none.

#include "emit.hh"

#include <format>
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

// The state of one module's emission.  Everything but the stream is
// reset per definition, so the names in a function depend on that
// function alone and the pinned text diffs cleanly.
struct state_t
{
    std::ostream &out;

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

// The integer type of an operand; nothing else is in the first slice.
const int_t &
integer(const type_t &t)
{
    if (!is< int_t >(t))
        throw std::logic_error("emit: a non-integer operand");

    return as< int_t >(t);
}

std::string ir_type(const type_t &);

// The parameter list of a function type as LLVM spells it, the
// ellipsis included.
std::string
param_list(const function_t &f)
{
    std::string list;

    for (const auto &p : f.params)
        list += (list.empty() ? "" : ", ") + ir_type(*p);

    if (f.variadic)
        list += list.empty() ? "..." : ", ...";

    return list;
}

// The IR spelling of a type.  An array has no value in the first slice.
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
        return std::format("{} ({})", ir_type(*f.result), param_list(f));
    }

    throw std::logic_error("emit: an array type");
}

// G11: a declared object carries its natural alignment and no more.
int
alignment_of(const type_t &t)
{
    return integer(t).width / 8;
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

// G11: natural alignment.  G12: the slot lives for the whole
// activation; no llvm.lifetime.start or end is ever written.
void
allocate(state_t &s, const symbol_t *sym)
{
    auto name = std::format("%{}.{}", sym->name, s.slot_names++);
    line(s, std::format("{} = alloca {}, align {}", name, ir_type(sym->type),
                        alignment_of(sym->type)));

    s.slots.emplace(sym, std::move(name));
}

// G5: every read of a slot is frozen, so a slot never written reads
// as some value of its type, the same at every read; never undef or
// poison, and the load carries no !range and no noundef.
value_t
load(state_t &s, const value_t &addr, const type_t &t)
{
    const auto type = ir_type(t);

    const auto raw = yield(s, type,
                           std::format("load {}, ptr {}, align {}", type,
                                       addr.name, alignment_of(t)));

    return yield(s, type, std::format("freeze {} {}", type, raw.name));
}

// G10: no !tbaa.  G11: natural alignment.
void
store(state_t &s, const value_t &v, const value_t &addr, const type_t &t)
{
    line(s, std::format("store {}, ptr {}, align {}", typed(v), addr.name,
                        alignment_of(t)));
}

// expressions

// The address of an object.  Only a local or a parameter has one in
// the first slice.
value_t
lvalue(state_t &s, const expr_t &e)
{
    if (!is< name_t >(e))
        throw std::logic_error("emit: an lvalue other than a name");

    const auto *sym = as< name_t >(e).sym;
    if (sym->storage != storage_t::local &&
        sym->storage != storage_t::parameter)
        throw std::logic_error("emit: a global or function as an object");

    return { s.slots.at(sym), "ptr" };
}

// A scalar as a condition: unequal to zero.
value_t
test(state_t &s, const expr_t &e)
{
    const auto v = rvalue(s, e);

    return yield(s, "i1", std::format("icmp ne {}, 0", typed(v)));
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

// G4 waits for its guard: until it lands, a division or remainder is
// emitted only by a literal divisor LLVM defines on every dividend.
// Zero and a signed -1 are not such divisors, constant or not.
void
check_divisor(binary_op_t op, const expr_t &rhs)
{
    if (op != binary_op_t::div && op != binary_op_t::rem)
        return;

    if (!is< literal_t >(rhs))
        throw std::logic_error("emit: a division by a non-literal divisor");

    const auto &t = integer(rhs.type);
    const auto d = normalized(as< literal_t >(rhs).value, t.width);
    if (d == 0 || (t.is_signed && d == -1))
        throw std::logic_error("emit: a division by a literal 0 or -1");
}

// G3: the count is taken modulo the width, so the shift LLVM sees is
// in range and never poison.  No nsw, nuw or exact: a signed left
// shift wraps, a right shift drops the bits it shifts out.
value_t
shift(state_t &s,
      binary_op_t op,
      const value_t &l,
      const value_t &r,
      const int_t &t)
{
    if (r.type != l.type)
        throw std::logic_error("emit: a shift count of another width");

    const auto count =
        yield(s, l.type, std::format("and {}, {}", typed(r), t.width - 1));

    const auto *mnemonic = op == binary_op_t::shl ? "shl" :
                           t.is_signed            ? "ashr" :
                                                    "lshr";

    return yield(s, l.type,
                 std::format("{} {}, {}", mnemonic, typed(l), count.name));
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

    // no exact; check_divisor has let only a defined divisor through
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
value_t
compare(state_t &s,
        binary_op_t op,
        const value_t &l,
        const value_t &r,
        const int_t &t,
        const type_t &result)
{
    const bool sign = t.is_signed;

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

value_t
binary(state_t &s, const binary_t &b, const type_t &t)
{
    switch (b.op) {
    case binary_op_t::log_and:
    case binary_op_t::log_or: return logical(s, b, t);
    case binary_op_t::comma: rvalue(s, *b.lhs); return rvalue(s, *b.rhs);
    default: break;
    }

    check_divisor(b.op, *b.rhs);

    // G13: the left operand's code before the right's
    const auto l = rvalue(s, *b.lhs);
    const auto r = rvalue(s, *b.rhs);

    const auto &operands = integer(b.lhs->type);
    if (is_comparison(b.op))
        return compare(s, b.op, l, r, operands, t);

    return apply(s, b.op, l, r, operands);
}

// ++ and --, either side.  G1: the add or sub of 1 wraps.  Prefix
// yields the stored value, postfix the one loaded.
value_t
step(state_t &s, const unary_t &u)
{
    const auto &arg = *u.arg;

    const auto addr = lvalue(s, arg);
    const auto old = load(s, addr, arg.type);

    const bool up = u.op == unary_op_t::pre_inc || u.op == unary_op_t::post_inc;
    const auto now = yield(
        s, old.type, std::format("{} {}, 1", up ? "add" : "sub", typed(old)));
    store(s, now, addr, arg.type);

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
        const auto bit = yield(s, "i1", std::format("icmp eq {}, 0", typed(v)));
        return widen(s, bit, t);
    }

    case unary_op_t::pre_inc:
    case unary_op_t::pre_dec:
    case unary_op_t::post_inc:
    case unary_op_t::post_dec: return step(s, u);

    case unary_op_t::addr: throw std::logic_error("emit: unary &");
    case unary_op_t::deref: throw std::logic_error("emit: unary *");
    }

    throw std::logic_error("emit: an unknown unary operator");
}

// Plain or compound assignment; its value is the value stored.  A
// compound one computes in the left operand's type, so the right
// operand must already have it.
value_t
assign(state_t &s, const assign_t &a)
{
    const auto &lhs = *a.lhs;

    if (a.op) {
        check_divisor(*a.op, *a.rhs);

        if (!(a.rhs->type == lhs.type))
            throw std::logic_error(
                "emit: a compound assignment across two types");
    }

    // G13: the lvalue before the rvalue, and the old value before the
    // right operand of a compound assignment
    const auto addr = lvalue(s, lhs);

    value_t v;
    if (a.op) {
        const auto old = load(s, addr, lhs.type);
        const auto r = rvalue(s, *a.rhs);
        v = apply(s, *a.op, old, r, integer(lhs.type));
    }
    else
        v = rvalue(s, *a.rhs);

    store(s, v, addr, lhs.type);

    return v;
}

// An integer conversion, or a discard to void.  A change of signedness
// alone is no instruction.
value_t
cast(state_t &s, const cast_t &c, const type_t &to)
{
    const auto v = rvalue(s, *c.arg);
    if (is< void_t >(to))
        return { "", "void" };

    const auto &from = integer(c.arg->type);
    const auto &into = integer(to);
    if (into.width == from.width)
        return v;

    const auto type = ir_type(to);

    // G2: narrowing keeps the low bits, read as two's complement
    if (into.width < from.width)
        return yield(s, type, std::format("trunc {} to {}", typed(v), type));

    const auto *widening = from.is_signed ? "sext" : "zext";
    return yield(s, type, std::format("{} {} to {}", widening, typed(v), type));
}

// The function a call names: a name, or a name the parser decayed to
// a pointer.  An indirect call is not in the first slice.
const symbol_t &
callee_of(const call_t &c)
{
    const expr_t *e = c.callee.get();
    if (is< cast_t >(*e))
        e = as< cast_t >(*e).arg.get();

    if (!is< name_t >(*e) ||
        as< name_t >(*e).sym->storage != storage_t::function)
        throw std::logic_error("emit: an indirect call");

    return *as< name_t >(*e).sym;
}

// A direct call.  The callee's type is the call site's: the parser
// sees to it in this slice.
value_t
call(state_t &s, const call_t &c)
{
    const auto &callee = callee_of(c);
    const auto &f = as< function_t >(callee.type);

    // G13: every argument, in order, before the call
    std::string args;
    for (const auto &arg : c.args)
        args += (args.empty() ? "" : ", ") + typed(rvalue(s, arg));

    // G16: where the call site's type will differ from the callee's,
    // the callee goes through `call ptr asm "", "=r,0"` here, and the
    // call is made through the laundered pointer.
    const auto result = ir_type(*f.result);
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

    return yield(s, result, text);
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

// The value of an expression, its code written first.  A pointer value
// is not in the first slice.
value_t
rvalue(state_t &s, const expr_t &e)
{
    if (is< pointer_t >(e.type))
        throw std::logic_error("emit: a pointer-typed expression");

    if (is< literal_t >(e))
        return literal(as< literal_t >(e), e.type);

    if (is< name_t >(e))
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
        store(s, rvalue(s, *d.init), { s.slots.at(d.sym), "ptr" }, d.sym->type);
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
    if (r.value)
        terminate(s, "ret " + typed(rvalue(s, *r.value)));
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
        terminate(s, std::format("ret {} 0", ir_type(s.result)));
    else {
        const auto v = load(s, { "%ret.indeterminate", "ptr" }, s.result);
        terminate(s, "ret " + typed(v));
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
                              ir_type(fd.params[i]->type), i);

    if (f.variadic)
        params += params.empty() ? "..." : ", ...";

    s.out << std::format("define {} @{}({}) null_pointer_is_valid {{\n",
                         ir_type(*f.result), fd.sym->name, params);
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
                            ir_type(s.result), alignment_of(s.result)));

    for (std::size_t i = 0; i < fd.params.size(); ++i) {
        const auto *p = fd.params[i];
        store(s, { std::format("%p.{}", i), ir_type(p->type) },
              { s.slots.at(p), "ptr" }, p->type);
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

    s.out << std::format("declare {} @{}({})\n", ir_type(*f.result),
                         fd.sym->name, param_list(f));
}

} // namespace

// No target triple, no datalayout, no module attributes: clang
// supplies the host's when it reads the file.
void
emit(const unit_t &unit, std::ostream &out)
{
    state_t s { .out = out };

    for (std::size_t i = 0; i < unit.functions.size(); ++i) {
        const auto &fd = unit.functions[i];

        if (i > 0)
            out << '\n';

        if (fd.body)
            function(s, fd);
        else
            declaration(s, fd);
    }
}

} // namespace rcc
