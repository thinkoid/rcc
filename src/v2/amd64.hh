// -*- mode: c++; -*-

// x86-64, docs/backend.md: the registers, the state of a function's
// generation, the rows of the zoo's types this target writes itself,
// and the unit's entry.  The rows are declared here for build.cc, whose
// models call them, and defined in amd64.cc, which reaches the tree
// only through a handle's row.

#pragma once

#include "register-stack.hh"
#include "targets.hh"
#include "unit.hh"
#include "zoo.hh"

#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace rcc::amd64 {

// The registers, chapter 4: the nine expression registers, the
// caller-saved set, in the order a new value takes them, rax first.
enum reg_t : int { rax, rcx, rdx, rsi, rdi, r8, r9, r10, r11 };

// A register as a set of one, rcc's bit(): the sets are the registers
// a value is taken outside of, and those an instruction pins.
constexpr unsigned
bit(reg_t r)
{
    return 1u << r;
}

// The state of one function's generation, which the function's row
// builds fresh.  The body is a buffer, which the frame is written
// around; the function's name makes its labels, which are counted per
// function, and ret is the one epilogue's.  terminated says that no
// path reaches what is written next: the last instruction written is
// an unconditional jump, which jump() sets, so main writes no return of
// 0 (backend.md 7.2).  frame is the bytes the slots take below rbp, and
// slots each local's displacement off rbp, by its number.  reserved is
// the registers take() leaves for last while a value meant for a pinned
// register is generated, so it lands there (chapter 6.5).
struct state_t
{
    std::ostringstream body { };
    std::string function;
    int labels = 0;
    int ret = 0;
    bool terminated = false;

    long frame = 0;
    std::unordered_map< int, int > slots { };

    registers_t< reg_t > registers { };
    unsigned reserved = 0;
};

// A register for a new value, register-stack.hh's, taken outside avoid.
reg_t take(state_t &, unsigned avoid = 0);

// The next slot of the frame, the local's.
void allocate(state_t &, slot_t where);

// The constant rows of an int: its 32 bits in a register taken for it.
// Every value row of x86-64 is handed wanted, the registers the
// consumer of the value names, a set of at most one, rax for a
// return's (6.1): a hint, which a row is free not to honour
// (docs/typed-tree.md 3.4), none where the consumer names none.  A
// leaf takes the lowest free register whatever the consumer names, as
// rcc's literal() and load() do.
value_t value(state_t &, const literal_t &, unsigned wanted = 0);
value_t value(state_t &, const zero_t &, unsigned wanted = 0);

// The load row: the int a slot holds, in a register taken for it.
value_t value(state_t &, const load_t &, unsigned wanted = 0);

// - or ~ on r in place, at 32 bits.
void operate(state_t &, unary_op_t op, reg_t r);

// The operation of op into r from an operand at 32 bits: a register,
// the int in a slot, or an immediate as immediate_of() writes it.
void operate(state_t &, binary_op_t op, reg_t r, reg_t source);
void operate(state_t &, binary_op_t op, reg_t r, slot_t source);
void operate(state_t &, binary_op_t op, reg_t r, std::string_view immediate);

// The operation of op on the held values l and r, rcc's arithmetic():
// its value is in l's register, or in r's where op commutes and that is
// the one wanted names; the other value is consumed.
value_t
arithmetic(state_t &, binary_op_t op, value_t l, value_t r, unsigned wanted);

// The three-operand imul at 32 bits, rcc's object_product(): the
// source, a register or the int in a slot, times the immediate factor
// into r.
void multiply(state_t &, reg_t r, reg_t source, std::string_view factor);
void multiply(state_t &, reg_t r, slot_t source, std::string_view factor);

// The store rows, an operand into a slot at 32 bits: a register, or an
// immediate as immediate_of() writes it.
void store(state_t &, reg_t r, slot_t where);
void store(state_t &, std::string_view immediate, slot_t where);

// A constant as the immediate operand of an instruction, 32 bits.
std::string immediate_of(const literal_t &);
std::string immediate_of(const zero_t &);

// A shift's constant count as its instruction's immediate: the low 8
// bits of the count, which is what the instruction encodes.
std::string count_of(int count);

// The instruction of a shift of the value l by the count held in r, at
// 32 bits, shll %cl, %eax, with what is in the way moved first; r is
// consumed.
void shift(state_t &, binary_op_t op, value_t l, value_t r);

// A value an instruction takes as its immediate: a type immediate_of()
// is written for (principle 7).
template< typename V >
concept immediate = requires(const V &e) { immediate_of(e); };

// The niche of initialize(), chapter 6.6: an immediate is stored by
// one mov into the slot, and no register is held.
template< immediate V >
void
initialize(state_t &s, slot_t where, const V &e)
{
    store(s, immediate_of(e), where);
}

// The operators of rcc's arithmetic_mnemonic(), + - * & | ^, whose
// instruction is op R, L, R a register, an immediate or an operand in
// memory.  The shifts are rows of their own, which take no operand in
// memory and an immediate of their own, below.
template< binary_op_t op >
concept arithmetic_operator =
    op == binary_op_t::add || op == binary_op_t::sub ||
    op == binary_op_t::mul || op == binary_op_t::bit_and ||
    op == binary_op_t::bit_or || op == binary_op_t::bit_xor;

template< binary_op_t op >
concept shift_operator = is_shift(op);

// The registers the instruction of op pins, chapter 6.5, rcc's pins():
// rcx for << and >> by a count that is no constant, which is the
// immediate; constant_right says whether the right operand is a
// constant.
constexpr unsigned
pins(binary_op_t op, bool constant_right)
{
    return is_shift(op) && !constant_right ? bit(rcx) : 0;
}

// The registers a node holds out of the choice for its left operand's
// value while it generates its right operand, rcc's pinned_by(): those
// the right operand's top operator pins, a fact of its class, top_t.
// The value then does not stand where that instruction wants its
// operands.  Any other class pins nothing, an erased one too, since the
// builder keeps a node that pins as a top_t; a shift deeper in the
// right operand changes nothing.
template< typename R >
constexpr unsigned
pinned_by(const R &)
{
    return 0;
}

template< binary_op_t op >
constexpr unsigned
pinned_by(const top_t< op > &)
{
    return pins(op, false);
}

// The registers the instruction of a node or of any operand within it
// pins, rcc's pinned_within(): pins() over the whole tree, a summary
// each node makes of its operands' and an erased one hands up through
// its handle (principle 3).  A value held while the node is generated
// may have to move out of one of them, as out of rcx for a shift's
// count deeper in a right operand.  A leaf pins nothing.
template< constant C >
constexpr unsigned
pinned_within(const state_t &, const C &)
{
    return 0;
}

constexpr unsigned
pinned_within(const state_t &, const load_t &)
{
    return 0;
}

constexpr unsigned
pinned_within(const state_t &, const slot_t &)
{
    return 0;
}

template< unary_op_t op, typename A >
unsigned
pinned_within(const state_t &s, const unary_t< op, A > &u)
{
    return pinned_within(s, u.arg);
}

template< binary_op_t op, typename L, typename R >
unsigned
pinned_within(const state_t &s, const binary_t< op, L, R > &b)
{
    return pins(op, constant< R >) | pinned_within(s, b.lhs) |
           pinned_within(s, b.rhs);
}

template< binary_op_t op >
unsigned
pinned_within(const state_t &s, const top_t< op > &t)
{
    return pinned_within(s, t.node);
}

template< typename V >
unsigned
pinned_within(const state_t &s, const assign_t< V > &a)
{
    return pinned_within(s, a.value);
}

// The registers a node hands its left operand, of those its consumer
// names, wanted, rcc's wanted_left(): wanted, outside own, the
// registers the node's own instruction pins, where no instruction
// within its right operand pins one, which would move the left operand
// out of wanted or find the right operand's own values there.  None
// otherwise, and the left operand is as the lowest free register makes
// it.
template< typename R >
unsigned
wanted_left(const state_t &s, unsigned wanted, unsigned own, const R &rhs)
{
    return pinned_within(s, rhs) ? 0 : wanted & ~own;
}

// The value of e generated while the registers in pinned are left for
// last, so that it does not land where an instruction generated after
// it wants its operands, rcc's rvalue_avoiding() (chapter 6.5).  The
// reservation is an avoidance, not a pin (take()).
template< typename T >
value_t
rvalue_avoiding(state_t &s, const T &e, unsigned pinned, unsigned wanted = 0)
{
    const unsigned reserved = std::exchange(s.reserved, s.reserved | pinned);
    const value_t v = value(s, e, wanted);
    s.reserved = reserved;

    return v;
}

// The primary of + - * & | ^, 6.5.6, 6.5.7, 6.5.11 to 6.5.13, rcc's
// binary() and arithmetic(), chapter 6.1: the left operand's value
// first, G13, generated outside the registers the right operand's top
// operator pins, so that a + (b << a) loads a into eax and not into the
// ecx the count goes to, and handed the consumer's register,
// wanted_left(); held while the right operand's is taken in a register
// of its own; then arithmetic(), addl %edx, %eax, or into the right
// operand's register where it is the one the consumer names: the
// count (0 << d) + (d = c) is addl %edx, %ecx, its left operand a
// shift, held outside ecx, and its right one loaded into the ecx a
// count's consumer names.
template< binary_op_t op, typename L, typename R >
    requires arithmetic_operator< op >
value_t
value(state_t &s, const binary_t< op, L, R > &b, unsigned wanted = 0)
{
    const value_t l = rvalue_avoiding(s, b.lhs, pinned_by(b.rhs),
                                      wanted_left(s, wanted, 0, b.rhs));
    const value_t r = value(s, b.rhs);

    return arithmetic(s, op, l, r, wanted);
}

// Whether a node with the constant L on the left of op is written with
// its operands exchanged, rcc's constant_first() as ordered() asks it:
// op commutes, and the constant is an immediate, as every int is.
template< binary_op_t op, typename L >
concept constant_first = commutes(op) && immediate< L >;

// The niches of the primary, chapter 6.1.  A constant on the left of an
// operator that commutes is the row of the node with its operands
// exchanged, 5 + a as a + 5: the value of the right operand, which is
// no constant, then the constant as the immediate.
template< binary_op_t op, typename L, typename R >
    requires arithmetic_operator< op > && constant_first< op, L >
value_t
value(state_t &s, const binary_t< op, L, R > &b, unsigned wanted = 0)
{
    return value(s, binary_t< op, R, L > { b.rhs, b.lhs }, wanted);
}

// A load on the right is the instruction's operand in memory, addl
// -8(%rbp), %eax, and takes no register.  Its type is the more special,
// so the node the mirror takes, 5 + a, is excluded by name; 5 - a is
// this row, movl $5, %eax and subl -4(%rbp), %eax.  The right operand
// pins nothing, so the left operand is handed the consumer's register,
// as in every niche of an operand in memory or an immediate.
template< binary_op_t op, typename L >
    requires(arithmetic_operator< op > && !constant_first< op, L >)
value_t
value(state_t &s, const binary_t< op, L, load_t > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    operate(s, op, reg_of(s, l), b.rhs.where);

    return l;
}

// An immediate on the right is the instruction's, negative ones as
// written: addl $5, %eax, andl $-2, %eax.  Of the mirror's nodes it
// would take only a constant on both sides, which the builder folds.
template< binary_op_t op, typename L, immediate I >
    requires arithmetic_operator< op >
value_t
value(state_t &s, const binary_t< op, L, I > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    operate(s, op, reg_of(s, l), immediate_of(b.rhs));

    return l;
}

// imul by an immediate is the three-operand form, rcc's arithmetic():
// the left operand's value, its register the source and the
// destination, imull $5, %eax, %eax.
template< typename L, immediate I >
value_t
value(state_t &s,
      const binary_t< binary_op_t::mul, L, I > &b,
      unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    multiply(s, reg_of(s, l), reg_of(s, l), immediate_of(b.rhs));

    return l;
}

// A load on the left of imul by an immediate is the source in memory,
// rcc's product(): imull $5, -4(%rbp), %eax, no load, and the product
// in a register taken for it, the lowest free whatever the consumer
// names.  5 * a is this row, through the mirror.
template< immediate I >
value_t
value(state_t &s,
      const binary_t< binary_op_t::mul, load_t, I > &b,
      unsigned = 0)
{
    const value_t v = fresh(s);
    multiply(s, reg_of(s, v), b.lhs.where, immediate_of(b.rhs));

    return v;
}

// A shift's count that is no constant, rcc's shift_count(): generated
// with rax reserved and with rcx taken out of what a node around the
// shift reserves for its own operand, since the instruction consumes
// the count in cl (chapter 6.5), so a count's load lands in rcx; and
// rcx is the register the count's consumer names, as a return names
// rax (6.1).
template< typename R >
value_t
shift_count(state_t &s, const R &count)
{
    const unsigned outer = std::exchange(s.reserved, s.reserved & ~bit(rcx));
    const value_t v = rvalue_avoiding(s, count, bit(rax), bit(rcx));
    s.reserved = outer;

    return v;
}

// << and >> by a count that is no constant, 6.5.8, rcc's binary() and
// shift(), chapter 6.5: the shifted value first, G13, generated with
// rcx left for last, where the count goes, and outside what the count's
// top operator pins, and handed the consumer's register but rcx,
// wanted_left(); then the count, shift_count(), held, since the
// instruction takes no operand in memory; then shift(), shll %cl, %eax,
// sarl for >>, which the machine masks to 5 bits (G3).  The shifted
// value's register holds the node's value.
template< binary_op_t op, typename L, typename R >
    requires shift_operator< op >
value_t
value(state_t &s, const binary_t< op, L, R > &b, unsigned wanted = 0)
{
    const unsigned own = pins(op, constant< R >);

    const value_t l = rvalue_avoiding(s, b.lhs, own | pinned_by(b.rhs),
                                      wanted_left(s, wanted, own, b.rhs));
    const value_t r = shift_count(s, b.rhs);

    shift(s, op, l, r);

    return l;
}

// A constant count is the instruction's immediate, count_of(), and pins
// nothing: shll $33, %eax, and shll $255, %eax for a << -1.
template< binary_op_t op, typename L, constant C >
    requires shift_operator< op >
value_t
value(state_t &s, const binary_t< op, L, C > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    operate(s, op, reg_of(s, l), count_of(value_of(b.rhs)));

    return l;
}

// The value, moved to the result register where its row left it
// elsewhere, then the jump to the epilogue.
void generate(state_t &, const return_t &);

// The unit's assembly; file is the source's name, for the .file
// directive.
void generate(const translation_unit_t &,
              std::string_view file,
              std::ostream &assembly);

} // namespace rcc::amd64
