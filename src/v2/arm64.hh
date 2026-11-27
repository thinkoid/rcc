// -*- mode: c++; -*-

// aarch64, docs/aarch64.md: the registers, the state of a function's
// generation, the rows of the zoo's types this target writes itself,
// and the unit's entry.  The rows are declared here for build.cc, whose
// models call them, and those of a template defined here too; the rest
// are defined in arm64.cc, which reaches the tree only through a
// handle's row.  Every width a row writes is its node's view, handed to
// the text as data: the w name of a register at 32 bits, the x name at
// 64.

#pragma once

#include "register-stack.hh"
#include "targets.hh"
#include "unit.hh"
#include "zoo.hh"

#include <concepts>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace rcc::arm64 {

// The general registers by number, chapter 2.1: the fifteen expression
// registers of chapter 4, the caller-saved set without x8, x0 the
// convention's result register.
enum reg_t : int {
    x0,
    x1,
    x2,
    x3,
    x4,
    x5,
    x6,
    x7,
    x9 = 9,
    x10,
    x11,
    x12,
    x13,
    x14,
    x15
};

// A register as a set of one, rcc's bit(): the sets are the registers
// a value is taken outside of, and those the consumer of a value names.
constexpr unsigned
bit(reg_t r)
{
    return 1u << r;
}

// The state of one function's generation, which the function's row
// builds fresh.  defined is the names of the objects the unit defines,
// defined_symbols()'s, which a global's address reads.  The body is a
// buffer, which the frame is written around; the function's name makes
// its labels, which are counted per function, and ret is the one
// epilogue's.  terminated says that no path reaches what is written
// next: the last instruction written is an unconditional jump, which
// jump() sets and place_label() clears, so an if whose then part ends
// so writes no jump over its else part, and main writes no return of 0
// (backend.md 7.2).  breaks and continues are the labels each loop
// generated so far took for the breaks and for the continues of its
// body, by the loop's number, and user_labels the label of each C label
// the generation has met so far, by its number.  frame is the bytes
// from x29 to the end of the last slot, the locals' and the spills' in
// the order they are taken, the frame record's alone before the first,
// and slots each local's displacement off x29, by its number.  reserved
// is the registers take() leaves for last while an assignment's place
// is formed, the one its consumer names, so that the value takes it
// (6.1).
struct state_t
{
    const std::unordered_set< std::string > &defined;
    std::ostringstream body { };
    std::string function;
    int labels = 0;
    int ret = 0;
    bool terminated = false;
    std::unordered_map< int, int > breaks { };
    std::unordered_map< int, int > continues { };
    std::unordered_map< int, int > user_labels { };

    long frame = 0;
    std::unordered_map< int, int > slots { };

    registers_t< reg_t > registers { };
    unsigned reserved = 0;
};

// A jump to the function's label n; no path reaches what follows it.
void jump(state_t &, int n);

// A register for a new value, register-stack.hh's, taken outside avoid
// and, where it can, outside s.reserved; it spills a held value when
// every one is held.
reg_t take(state_t &, unsigned avoid = 0);

// The function's label n, where a jump may reach it from elsewhere: a
// path reaches what follows.
void place_label(state_t &, int n);

// The branch to label on the flags a cmp set, where op holds, rcc's
// jump_if(): b.lt, b.eq.
void jump_if(state_t &, binary_op_t op, int label);

// The test of the held value v against 0 at its view, valid to valid
// bits, rcc's test_zero(): with a label, the jump to it where v is
// nonzero, sense, or zero, cbnz w0 or cbz x0, one instruction and no
// flags; without one, the flags for the caller's cset, cmp w0, #0, the
// row of !.  A value valid to less than 32 bits, the value of a
// narrowing, has stale bits above its width, and the machine compares
// whole registers only: either way it is tst of the low bits alone,
// tst w0, #0xff, then b.ne or b.eq to the label.  v stays held.
void test_zero(state_t &,
               value_t v,
               view_t view,
               int valid,
               std::optional< int > label,
               bool sense = false);

// A mov of the immediate value into r, at 32 bits: mov w0, #1.
void move_immediate(state_t &, reg_t r, int value);

// The directive before a loop's head, rcc's align_loop_head(): none on
// aarch64, whose loop_alignment is 0.
void align_loop_head(state_t &);

// The next slot of the frame, of size bytes at align, a local's or a
// spill's: its displacement off x29.
int allocate(state_t &, long size, int align);

// The store of r to a spill slot, and the reload of a spilled value
// into r from its slot, register-stack.hh's.
void store_spill(state_t &, reg_t r, int displacement);
void reload_spill(state_t &, reg_t r, int displacement);

// The constant row, a constant of the view in a register taken for it,
// mov w0, #5 or mov x0, #5, and the forms of a value no one mov writes.
// Every value row of aarch64 is handed wanted, the registers the
// consumer of the value names, a set of at most one, x0 for a return's
// (6.1): a hint, which a row is free not to honour (docs/typed-tree.md
// 3.4), none where the consumer names none.  A leaf takes the lowest
// free register whatever the consumer names, as rcc's literal() and
// load() do.
value_t constant_value(state_t &, long long value, view_t view);

template< view_t V >
value_t
value(state_t &s, const literal_t< V > &c, unsigned = 0)
{
    return constant_value(s, c.value, V);
}

template< view_t V >
value_t
value(state_t &s, const zero_t< V > &, unsigned = 0)
{
    return constant_value(s, 0, V);
}

inline value_t
value(state_t &s, const wide_t &c, unsigned = 0)
{
    return constant_value(s, c.value, view_t::i64);
}

// An object in memory as an access names it, rcc's address_t (6.3,
// chapter 10): a slot's displacement off x29, a global's label, which
// its access reaches through x16, or a base held in a register, the
// address the GOT held.  The base is a held value, spilled and reloaded
// as any; the access consumes it.
struct address_t
{
    std::optional< value_t > base = std::nullopt;
    int displacement = 0;
    std::string label = { };
};

// A slot's address, rcc's slot_address(): its displacement, and no
// instruction.
address_t address(state_t &, slot_t where);

// A global's, rcc's global_address(): its label where the unit defines
// it; otherwise the GOT's entry loaded into a register, adrp x0, :got:g
// and ldr x0, [x0, :got_lo12:g], rcc's symbol_address(), the base of
// the access, [x0].  The unit's answer is asked here, and no row asks
// it.
address_t address(state_t &, const global_t &where);

// The address of where formed with the registers in reserved left for
// last, rcc's address_avoiding(): a GOT's load takes its register
// outside them where it can.
template< typename W >
address_t
address_avoiding(state_t &s, const W &where, unsigned reserved)
{
    const unsigned outer = std::exchange(s.reserved, s.reserved | reserved);
    const address_t a = address(s, where);
    s.reserved = outer;

    return a;
}

// The place of an assignment, held across its value, rcc's assign():
// the address of where, formed outside the consumer's register, wanted,
// so that the value takes it: return g = 7 on an object only declared
// takes x1 for the GOT's base, outside the result's x0.  A slot's
// address and a label take no register, so a local's and a defined
// object's place is no reservation.
template< typename W >
address_t
place_of(state_t &s, const W &where, const expr_t &, unsigned wanted)
{
    return address_avoiding(s, where, wanted);
}

// The registers an address's values are in, rcc's registers_of(), for
// a take or a reload to stay clear of: the base's, none where it is
// spilled or there is none.
unsigned registers_of(const state_t &, const address_t &);

// The address as an access's operand, rcc's operand(): [x29, #16], [x0],
// a spilled base reloaded first outside avoid; or a global's label,
// adrp x16, g written here, at the access, and [x16, :lo12:g] returned,
// so every caller forms the operand in a statement of its own, after
// every register of the access is in hand and right before the access:
// no spill or reload stands between the adrp and its access.
std::string operand(state_t &, const address_t &, unsigned avoid = 0);

// The address is consumed: its base's register is free, rcc's release().
void release(state_t &, const address_t &);

// The load row, rcc's load(): the object at an address at its view,
// extended to 32 bits by its sign where it is narrow, in a register
// taken for it, ldrsb w0, [x29, #16], ldr w0, [x29, #16] or ldr x0,
// [x29, #16], ldr w0, [x16, :lo12:g] after adrp x16, g, or ldr w0, [x0]
// after the GOT's pair.
value_t load(state_t &, const address_t &, view_t view, sign_t sign);

template< typename W, view_t V, sign_t S >
value_t
value(state_t &s, const load_t< W, V, S > &l, unsigned = 0)
{
    return load(s, address(s, l.where), V, S);
}

// - or ~ of the held value v at the view, into the register the
// consumer names where it may be written, or into v's own.
value_t
negated(state_t &, unary_op_t op, value_t v, view_t view, unsigned wanted);

// The operation of op on the held value l and the right operand at the
// view, rcc's arithmetic(): a held value r, which is consumed, or a
// constant's value, the instruction's immediate where it encodes one.
// Its value is l, in the register the consumer names where the
// instruction may write it, or in l's own.
value_t arithmetic(state_t &,
                   binary_op_t op,
                   value_t l,
                   value_t r,
                   view_t view,
                   unsigned wanted);
value_t arithmetic(state_t &,
                   binary_op_t op,
                   value_t l,
                   long long constant,
                   view_t view,
                   unsigned wanted);

// - and ~, 6.5.4.3p3, p4, rcc's unary(): the operand's value, generated
// as for any consumer, then neg or mvn, whose destination is apart from
// its source: neg w0, w0, and neg w0, w1 for a return whose operand is
// in w1.  ! is a row of its own, below.
template< unary_op_t op, view_t V, typename A >
    requires(op != unary_op_t::log_not)
value_t
value(state_t &s, const unary_t< op, V, A > &u, unsigned wanted = 0)
{
    return negated(s, op, value(s, u.arg), V, wanted);
}

// The store row, r into an object in memory, to, its operand's text, at
// the view: str w0 or str x0.
void store(state_t &, reg_t r, std::string_view to, view_t view);

// The niche of initialize(), 6.6: 0 is the zero register's store at the
// address its caller formed, str wzr, [x29, #16], adrp x16, g and str
// wzr, [x16, :lo12:g], and no register is held.
void store_zero(state_t &, const address_t &at, view_t view);

template< view_t V >
void
initialize(state_t &s, const address_t &at, const zero_t< V > &)
{
    store_zero(s, at, V);
}

// The operators of rcc's arithmetic(), * / % + - << >> & | ^, whose
// row writes the operation into the left operand's register; a
// comparison's cmp writes none, and its rows are below.
template< binary_op_t op >
concept arithmetic_operator =
    op == binary_op_t::add || op == binary_op_t::sub ||
    op == binary_op_t::mul || op == binary_op_t::sdiv ||
    op == binary_op_t::udiv || op == binary_op_t::srem ||
    op == binary_op_t::urem || op == binary_op_t::shl ||
    op == binary_op_t::ashr || op == binary_op_t::lshr ||
    op == binary_op_t::bit_and || op == binary_op_t::bit_or ||
    op == binary_op_t::bit_xor;

// The primary of * / % + - << >> & | ^, 6.5.6 to 6.5.8, 6.5.11 to
// 6.5.13, rcc's binary(), chapter 6.1: the left operand's value first,
// G13, held while the right operand's is taken in a register of its
// own; then arithmetic(), the operation from the two registers at the
// operation's view, add w0, w0, w1, lsl x0, x0, x1, and the right
// released.  No instruction pins a register, so the operands are
// generated as for any consumer, and the register the node's consumer
// names goes to the last instruction, which names its destination apart
// from its sources: add w0, w1, w0 after a right operand that took x0.
template< binary_op_t op, view_t V, typename L, typename R >
    requires arithmetic_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs);
    const value_t r = value(s, b.rhs);

    return arithmetic(s, op, l, r, V, wanted);
}

// Whether a node with the constant L on the left of op is written with
// its operands exchanged, rcc's constant_first() as ordered() asks it:
// op commutes, and L is a constant, whether the instruction encodes it
// or not.
template< binary_op_t op, typename L >
concept constant_first = commutes(op) && constant< L >;

// The niches of the primary, chapter 6.1.  A constant on the left of an
// operator that commutes is the row of the node with its operands
// exchanged, 5 + a as a + 5: the value of the right operand, which is
// no constant, then the constant as the right operand.
template< binary_op_t op, view_t V, typename L, typename R >
    requires arithmetic_operator< op > && constant_first< op, L >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    return value(s, binary_t< op, V, R, L > { b.rhs, b.lhs }, wanted);
}

// A constant on the right is the instruction's immediate where it
// encodes it at the view, add w0, w0, #5, lsl x0, x0, #5, and is held
// as the primary holds it otherwise: mul encodes none, mov w1, #5 and
// mul w0, w0, w1, and 0 is no immediate of these, a & 0 being mov w1,
// #0 and the and.  Of the mirror's nodes it would take only a constant
// on both sides, which the builder folds.
template< binary_op_t op, view_t V, typename L, constant C >
    requires arithmetic_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, C > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs);

    return arithmetic(s, op, l, value_of(b.rhs), V, wanted);
}

// cmp of the held value l and a right operand at the view, rcc's
// compare_with(): a held value r, cmp w0, w1, the registers of
// operands(), a spilled value reloaded, and r released; or a constant's
// value, the immediate where cmp encodes it, add's and sub's of 2.4
// whatever the comparison, cmp w0, #5 and cmp x0, #-1, and otherwise a
// held value of its own, mov w1, #4097 and cmp w0, w1.  It sets the
// flags and writes no register; l stays held.
void compare(state_t &, value_t l, value_t r, view_t view);
void compare(state_t &, value_t l, long long constant, view_t view);

// The value of a comparison of op from the flags its cmp set, rcc's
// comparison(): cset of the condition, one instruction, into
// final_destination(), the register the consumer names where it may be
// written, cset w0, lt after cmp w1, w0, or v's own: an int, on the w
// name.
value_t cset(state_t &, binary_op_t op, value_t v, unsigned wanted);

// The cmp of l and the right operand of a comparison at the view V by
// its class: a constant as compare() takes its value, 0 the immediate
// #0, and any other right operand its value, generated as for any
// consumer.
template< view_t V, typename R >
void
compare_with(state_t &s, value_t l, const R &rhs)
{
    compare(s, l, value(s, rhs), V);
}

template< view_t V, constant C >
void
compare_with(state_t &s, value_t l, const C &rhs)
{
    compare(s, l, value_of(rhs), V);
}

// The primary of < > <= >= == !=, 6.5.9, 6.5.10, rcc's comparison() and
// compare(), 6.3: the left operand's value first, G13, generated as for
// any consumer, then the cmp with the right operand at the operation's
// view, compare_with(), then cset, cmp w0, w1 and cset w0, lt.
template< binary_op_t op, view_t V, typename L, typename R >
    requires comparison_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs);
    compare_with< V >(s, l, b.rhs);

    return cset(s, op, l, wanted);
}

// A constant on the left is the row of the node mirrored, rcc's
// ordered(): 5 < a as a > 5, mirrored(), whether cmp encodes the
// constant or not, so 5 < a is cmp w0, #5 after the load of a.
template< binary_op_t op, view_t V, constant C, typename R >
    requires comparison_operator< op >
value_t
value(state_t &s, const binary_t< op, V, C, R > &b, unsigned wanted = 0)
{
    return value(s, binary_t< mirrored(op), V, R, C > { b.rhs, b.lhs }, wanted);
}

// !e, 6.5.4.3p5, rcc's unary(): the operand's value, generated as for
// any consumer, its test against 0 at the operand's view, test_zero()'s
// without a label, cmp w0, #0, or tst w0, #0xff for a value valid to
// less than 32 bits, then cset of eq, as for e == 0.
template< view_t V, typename A >
value_t
value(state_t &s,
      const unary_t< unary_op_t::log_not, V, A > &u,
      unsigned wanted = 0)
{
    const value_t v = value(s, u.arg);
    test_zero(s, v, V, valid_of(u.arg, sign_t::signed_), std::nullopt);

    return cset(s, binary_op_t::eq, v, wanted);
}

// The jump on a value compared with 0, rcc's test_zero(), 7.1: the
// value held, then the jump on its register at the value's view, cbz
// w0, or tst and b.eq where the width rule says the value is valid to
// less than 32 bits, the same at either sign, and the value released.
// A constant is no special case: if (1) loads 1 and tests it.
template< typename T >
void
value_branch(state_t &s, const T &t, int label, bool sense)
{
    const value_t v = value(s, t);
    test_zero(s, v, view_of(t), valid_of(t, sign_t::signed_), label, sense);
    release(s, v);
}

// A comparison as a jump, rcc's cbranch(), 7.1: the cmp of the value
// row, then b.cc of the operator where sense and of its inverse
// otherwise, b.lt or b.ge, and no value held.  e == 0 has no niche
// here: a == 0 is cmp w0, #0 and b.ne, where if (a) is cbz.
template< binary_op_t op, view_t V, typename L, typename R >
    requires comparison_operator< op >
void
branch(state_t &s, const binary_t< op, V, L, R > &b, int label, bool sense)
{
    const value_t l = value(s, b.lhs);
    compare_with< V >(s, l, b.rhs);
    release(s, l);

    jump_if(s, sense ? op : inverse(op), label);
}

// A constant on the left is the jump of the node mirrored, ordered():
// if (5 < a) is cmp w0, #5 and b.le.
template< binary_op_t op, view_t V, constant C, typename R >
    requires comparison_operator< op >
void
branch(state_t &s, const binary_t< op, V, C, R > &b, int label, bool sense)
{
    branch(s, binary_t< mirrored(op), V, R, C > { b.rhs, b.lhs }, label, sense);
}

// The extension of the held value v of the view from to the view to by
// the sign of its type, rcc's extended() past its test of the width
// rule, which the widening's row asks: sxtw x0, w0, or mov w0, w0 for an
// unsigned one, whose write of the w name clears the upper half,
// written into final_destination(), the register the consumer names
// where it may be written, sxtw x0, w1 for a return's (6.1), or v's
// own.
value_t extended(
    state_t &, value_t v, view_t from, sign_t sign, view_t to, unsigned wanted);

// The extending load, rcc's extending_load(), 6.3: the object of the
// view from at an address, loaded extended by its sign to 64 bits,
// ldrsw x0, [x29, #16], as load() takes the address.  It reads the
// bytes the load at its own view reads, at the object's width (G10),
// and leaves the value valid to 64.
value_t extending_load(state_t &, const address_t &, view_t from);

// A signed object widened to 64 bits is its extending load, the niche
// of the conversion over a load (docs/typed-tree.md 4.2), as on x86-64.
template< view_t F, typename W >
value_t
value(state_t &s,
      const sext_t< view_t::i64, F, load_t< W, F, sign_t::signed_ > > &e,
      unsigned = 0)
{
    return extending_load(s, address(s, e.arg.where), F);
}

// The arguments of a call moved from their slots to where the
// convention passes them, and the call of the function named, rcc's
// call() after the arguments' stores, chapter 8.
void transfer(state_t &,
              std::string_view name,
              const std::vector< argument_t > &args);

// A call of a function whose result is of the view V, 6.5.3.3, rcc's
// call(), chapter 8: its value is held in x0, where the call leaves it,
// valid to V's width and no further (valid_of()): the convention leaves
// the bits above a narrow one unspecified, and the widening's primary
// extends it where it is used wider, and test_zero() tests it with tst
// where it is narrow.
template< view_t V >
value_t
value(state_t &s, const call_t< V > &c, unsigned = 0)
{
    call(s, c.name, c.args);

    return hold(s, x0);
}

// The value of a return, of the result's view, moved to the result
// register where its row left it elsewhere, then the jump to the
// epilogue, rcc's return_stmt().
void return_stmt(state_t &, const expr_t &e, view_t view);

template< view_t V >
void
generate(state_t &s, const return_t< V > &r)
{
    return_stmt(s, r.value, V);
}

// The unit's assembly; file is the source's name, for the .file
// directive.
void generate(const translation_unit_t &,
              std::string_view file,
              std::ostream &assembly);

} // namespace rcc::arm64
