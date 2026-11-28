// -*- mode: c++; -*-

// x86-64, docs/backend.md: the registers, the state of a function's
// generation, the rows of the zoo's types this target writes itself,
// and the unit's entry.  The rows are declared here for build.cc, whose
// models call them, and those of a template defined here too; the rest
// are defined in amd64.cc, which reaches the tree only through a
// handle's row.  Every width a row writes is its node's view, handed to
// the text as data.

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
// builds fresh.  defined is the names of the objects the unit defines,
// defined_symbols()'s, which a global's address reads.  The body is a
// buffer, which the frame is written around; the function's name makes
// its labels, which are counted per function, and ret is the one
// epilogue's.  terminated says that no path reaches what is written
// next: the last instruction written is an unconditional jump, which
// jump() sets and place_label() clears, so an if whose then part ends
// so writes no jump over its else part, and main writes no return of 0
// (backend.md 7.2).  zero_flag is the width at which the last
// instruction written set ZF from the value it computed, flagged()'s,
// or 0, until the next instruction or label: a jump on that value at
// that width then needs no cmp (7.1).  breaks is the label each loop
// and switch generated so far took for the breaks of its body, by its
// number, and continues each loop's for its continues; cases is the
// label of each case and default label of those switches, by the
// label's number, and user_labels the label of each C label that the
// generation has met so far, by the number the parser gave its name.
// frame is the bytes the slots take below rbp, the locals' and the
// spills' in the order they are taken, and slots each local's
// displacement off rbp, by its number; calls says that the function
// calls, which moves rsp below the slots (chapter 5).  reserved is the
// registers take() leaves for last while a value meant for a pinned
// register is generated, so it lands there (chapter 6.5).  ahead is the
// registers a pinned instruction generated after the value now being
// generated would move a held value out of, which the instruction on a
// spilled value keeps its destination out of (right_written()).
struct state_t
{
    const std::unordered_set< std::string > &defined;
    std::ostringstream body { };
    std::string function;
    int labels = 0;
    int ret = 0;
    bool terminated = false;
    int zero_flag = 0;
    std::unordered_map< int, int > breaks { };
    std::unordered_map< int, int > continues { };
    std::unordered_map< int, int > cases { };
    std::unordered_map< int, int > user_labels { };

    long frame = 0;
    std::unordered_map< int, int > slots { };
    bool calls = false;

    registers_t< reg_t > registers { };
    unsigned reserved = 0;
    unsigned ahead = 0;
};

// A jump to the function's label n; no path reaches what follows it.
void jump(state_t &, int n);

// A register for a new value, register-stack.hh's, taken outside avoid;
// it spills a held value when every one is held.
reg_t take(state_t &, unsigned avoid = 0);

// The function's label n, where a jump may reach it from elsewhere with
// the flags its own path left: a path reaches what follows, and no
// instruction's ZF stands.
void place_label(state_t &, int n);

// The jump to label on the flags a cmp set, where op holds, rcc's
// jump_if(): jl, and je and jne for == and !=, which read ZF alone.
void jump_if(state_t &, binary_op_t op, int label);

// The jump on the held value v compared with 0 at its view, rcc's
// test_zero(): cmpl $0, %eax or cmpq $0, %rax, then jne where sense, je
// otherwise; v stays held.
void test_zero(state_t &, value_t v, view_t view, int label, bool sense);

// A mov of the immediate value into r, at 32 bits: movl $1, %eax.
void move_immediate(state_t &, reg_t r, int value);

// The directive before a loop's head, rcc's align_loop_head(): .p2align
// 5, the head on a 32-byte boundary, a convention of the target; with
// no fill byte named, the assembler fills the gap with nops.
void align_loop_head(state_t &);

// The next slot of the frame, of size bytes at align, a local's or a
// spill's: its displacement off rbp.
int allocate(state_t &, long size, int align);

// The store of r to a spill slot, and the reload of a spilled value
// into r from its slot, register-stack.hh's.
void store_spill(state_t &, reg_t r, int displacement);
void reload_spill(state_t &, reg_t r, int displacement);

// An immediate, as the assembler reads it: $5.
std::string imm(long long value);

// An object in memory as an instruction names it, rcc's address_t
// (backend.md 6.3, chapter 10): a slot's displacement off rbp, a
// global's label off rip, or a base held in a register, the address
// the GOT held.  The base is a held value, spilled and reloaded as any;
// the access consumes it.
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
// it, a slot with another spelling, g(%rip); otherwise the GOT's entry
// loaded into a register, movq g@GOTPCREL(%rip), %rax, rcc's
// symbol_address(), the base of the access, (%rax).  The unit's answer
// is asked here, and no row asks it.
address_t address(state_t &, const global_t &where);

// The address of where formed with the registers in reserved left for
// last, rcc's address_avoiding(): a GOT's load takes its register
// outside them, so that it does not land where an instruction
// generated after it wants its operands.
template< typename W >
address_t
address_avoiding(state_t &s, const W &where, unsigned reserved)
{
    const unsigned outer = std::exchange(s.reserved, s.reserved | reserved);
    const address_t a = address(s, where);
    s.reserved = outer;

    return a;
}

// The registers an address's values are in, rcc's registers_of(), for
// a take or a reload to stay clear of: the base's, none where it is
// spilled or there is none.
unsigned registers_of(const state_t &, const address_t &);

// The address as an instruction's operand, rcc's operand(): g(%rip),
// -4(%rbp), or (%rax), a spilled base reloaded first outside avoid.
std::string operand(state_t &, const address_t &, unsigned avoid = 0);

// The address is consumed: its base's register is free, rcc's release().
void release(state_t &, const address_t &);

// The register of the held value l and the operand of the address at,
// for an instruction that reads both, rcc's from_memory(): l's register,
// a spilled l reloaded outside the address's, then the address's
// operand outside l's, as operands() takes two held values' registers.
std::pair< reg_t, std::string >
operands(state_t &, value_t l, const address_t &at);

// The constant row, a constant of the view in a register taken for it,
// movl $5, %eax, movq $5, %rax, or movabsq for a 64-bit one no 32-bit
// immediate holds.  Every value row of x86-64 is handed wanted, the
// registers the consumer of the value names, a set of at most one, rax
// for a return's (6.1): a hint, which a row is free not to honour
// (docs/typed-tree.md 3.4), none where the consumer names none.  A leaf
// takes the lowest free register whatever the consumer names, as rcc's
// literal() and load() do.
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

// The load row, rcc's load(): the object at an address at its view,
// extended to 32 bits by its sign where it is narrow, in a register
// taken for it once the address is consumed, so a GOT's base and the
// value share one, movsbl -1(%rbp), %eax, movl g(%rip), %eax, movq
// g@GOTPCREL(%rip), %rax and movl (%rax), %eax.
value_t load(state_t &, const address_t &, view_t view, sign_t sign);

template< typename W, view_t V, sign_t S >
value_t
value(state_t &s, const load_t< W, V, S > &l, unsigned = 0)
{
    return load(s, address(s, l.where), V, S);
}

// The address as a value, valid to 64, rcc's address_of(): the leaq of
// its operand into a register taken once the address is consumed,
// leaq -4(%rbp), %rax.
value_t address_of(state_t &, const address_t &);

// &x, 6.5.4.2p3, rcc's address_value() of a name: the leaq of its
// where's address.
template< typename W >
value_t
value(state_t &s, const address_of_t< W > &a, unsigned = 0)
{
    return address_of(s, address(s, a.where));
}

// - or ~ on r in place, at the view.
void operate(state_t &, unary_op_t op, reg_t r, view_t view);

// - and ~, 6.5.4.3p3, p4, rcc's unary(): the operand's value, handed the
// consumer's register, then the operation on its register in place,
// negl %eax, which leaves the node's value there.  ! is a row of its
// own, below.
template< unary_op_t op, view_t V, typename A >
    requires(op != unary_op_t::log_not)
value_t
value(state_t &s, const unary_t< op, V, A > &u, unsigned wanted = 0)
{
    const value_t v = value(s, u.arg, wanted);
    operate(s, op, reg_of(s, v), V);

    return v;
}

// The operation of op into r from an operand at the view: a register,
// or an operand as its text, an immediate as immediate_of() writes it,
// an object in memory as operand() writes it or a spilled value's slot.
void operate(state_t &, binary_op_t op, reg_t r, reg_t source, view_t view);
void operate(
    state_t &, binary_op_t op, reg_t r, std::string_view source, view_t view);

// The operation of op on the held values l and r at the view, rcc's
// arithmetic(): its value is in l's register, or in r's where op
// commutes and that is the one wanted names, or where l is spilled and
// is read in its slot; the other value is consumed.
value_t arithmetic(state_t &,
                   binary_op_t op,
                   value_t l,
                   value_t r,
                   view_t view,
                   unsigned wanted);

// The three-operand imul at the view, rcc's object_product(): the
// source, a register or an object in memory as its operand's text,
// times the immediate factor into r.
void multiply(
    state_t &, reg_t r, reg_t source, std::string_view factor, view_t view);
void multiply(state_t &,
              reg_t r,
              std::string_view source,
              std::string_view factor,
              view_t view);

// The division of op, / or %, of the held dividend l by the divisor, a
// held value r, which is consumed, or an object in memory at its
// address, which is consumed, at the view: the quotient or the
// remainder, held where idiv leaves it or in l's register.
value_t divide(state_t &,
               binary_op_t op,
               value_t l,
               value_t r,
               view_t view,
               unsigned wanted);
value_t divide(state_t &,
               binary_op_t op,
               value_t l,
               const address_t &divisor,
               view_t view,
               unsigned wanted);

// The store rows, an operand into an object in memory, to, its
// operand's text, at the view: a register, or an immediate as
// immediate_of() writes it.
void store(state_t &, reg_t r, std::string_view to, view_t view);
void
store(state_t &, std::string_view immediate, std::string_view to, view_t view);

// A constant as the immediate operand of an instruction, 32 bits,
// sign-extended in a 64-bit one: a literal_t and a zero_t, and no
// wide_t, which no immediate holds.  The value is written at its view,
// modulo 2^N and signed, so a narrow store of an unsigned char's 255 is
// movb $-1, -1(%rbp).
std::string immediate_at(long long value, view_t view);

template< view_t V >
std::string
immediate_of(const literal_t< V > &c)
{
    return immediate_at(c.value, V);
}

template< view_t V >
std::string
immediate_of(const zero_t< V > &)
{
    return imm(0);
}

// A shift's constant count as its instruction's immediate: the low 8
// bits of the count, which is what the instruction encodes.
std::string count_of(long long count);

// The instruction of a shift of the value l by the count held in r, at
// the view, shll %cl, %eax, with what is in the way moved first, the
// count into rcx at its own view, count; r is consumed.
void shift(
    state_t &, binary_op_t op, value_t l, value_t r, view_t view, view_t count);

// A value an instruction takes as its immediate: a type immediate_of()
// is written for (principle 7).
template< typename C >
concept immediate = requires(const C &c) { immediate_of(c); };

// The niche of initialize(), chapter 6.6, rcc's constant_assignment():
// an immediate is stored by one mov at the address its caller formed,
// at its view, movl $7, g(%rip) and movl $7, (%rax), and no register is
// held.
template< immediate C >
void
initialize(state_t &s, const address_t &at, const C &c)
{
    store(s, immediate_of(c), operand(s, at), view_of(c));
}

// The operators of rcc's arithmetic_mnemonic(), + - * & | ^, whose
// instruction is op R, L, R a register, an immediate or an operand in
// memory.  The divisions and the shifts are rows of their own, below:
// idiv takes no immediate, and a shift no operand in memory and an
// immediate of its own.
template< binary_op_t op >
concept arithmetic_operator =
    op == binary_op_t::add || op == binary_op_t::sub ||
    op == binary_op_t::mul || op == binary_op_t::bit_and ||
    op == binary_op_t::bit_or || op == binary_op_t::bit_xor;

// The operators whose instruction takes the object in memory as its
// destination, op R, obj, rcc's arithmetic_on(): those of
// arithmetic_mnemonic() but imul, which has none.
template< binary_op_t op >
concept memory_operator = arithmetic_operator< op > && op != binary_op_t::mul;

template< binary_op_t op >
concept division_operator = is_division(op);

template< binary_op_t op >
concept shift_operator = is_shift(op);

// The registers the instruction of a family pins, chapter 6.5, rcc's
// pins(): rax and rdx for / and %, rcx for << and >> by a count that is
// no constant, which is the immediate.
constexpr unsigned
pins(family_t family)
{
    return family == family_t::division ? bit(rax) | bit(rdx) : bit(rcx);
}

// The registers the instruction of op pins, its family's; constant_right
// says whether the right operand is a constant, which a shift takes as
// its immediate.
constexpr unsigned
pins(binary_op_t op, bool constant_right)
{
    if (is_division(op))
        return pins(family_t::division);

    return is_shift(op) && !constant_right ? pins(family_t::shift) : 0;
}

// The registers a node holds out of the choice for its left operand's
// value while it generates its right operand, rcc's pinned_by(): those
// the right operand's top operator pins, a fact of its class, top_t,
// which keeps its family.  The value then does not stand where that
// instruction wants its operands.  Any other class pins nothing, an
// erased one too, since the builder keeps a node that pins as a top_t;
// a division or a shift deeper in the right operand changes nothing.
template< typename R >
constexpr unsigned
pinned_by(const R &)
{
    return 0;
}

template< family_t F >
constexpr unsigned
pinned_by(const top_t< F > &)
{
    return pins(F);
}

// The registers the instruction of a node or of any operand within it
// pins, rcc's pinned_within(): pins() over the whole tree, a summary
// each node makes of its operands' and an erased one hands up through
// its handle (principle 3).  A value held while the node is generated
// may have to move out of one of them, as out of rcx for a shift's
// count, or out of rax and rdx for a division, deeper in a right
// operand.  A leaf pins nothing, and a conversion what its operand
// pins.
template< constant C >
constexpr unsigned
pinned_within(const state_t &, const C &)
{
    return 0;
}

template< typename W, view_t V, sign_t S >
constexpr unsigned
pinned_within(const state_t &, const load_t< W, V, S > &)
{
    return 0;
}

template< typename W, view_t V, sign_t S >
constexpr unsigned
pinned_within(const state_t &, const place_t< W, V, S > &)
{
    return 0;
}

template< typename W >
constexpr unsigned
pinned_within(const state_t &, const address_of_t< W > &)
{
    return 0;
}

template< unary_op_t op, view_t V, typename A >
unsigned
pinned_within(const state_t &s, const unary_t< op, V, A > &u)
{
    return pinned_within(s, u.arg);
}

template< binary_op_t op, view_t V, typename L, typename R >
unsigned
pinned_within(const state_t &s, const binary_t< op, V, L, R > &b)
{
    return pins(op, constant< R >) | pinned_within(s, b.lhs) |
           pinned_within(s, b.rhs);
}

template< family_t F >
unsigned
pinned_within(const state_t &s, const top_t< F > &t)
{
    return pinned_within(s, t.node);
}

template< view_t V, view_t F, typename A >
unsigned
pinned_within(const state_t &s, const sext_t< V, F, A > &e)
{
    return pinned_within(s, e.arg);
}

template< view_t V, view_t F, typename A >
unsigned
pinned_within(const state_t &s, const zext_t< V, F, A > &e)
{
    return pinned_within(s, e.arg);
}

template< view_t V, typename A >
unsigned
pinned_within(const state_t &s, const trunc_t< V, A > &t)
{
    return pinned_within(s, t.arg);
}

template< typename W, view_t V >
unsigned
pinned_within(const state_t &s, const assign_t< W, V > &a)
{
    return pinned_within(s, a.value);
}

// A call pins rax, where the convention puts its result (chapter 8),
// and whatever its arguments pin, call_pins()'s.
unsigned call_pins(const state_t &, const std::vector< argument_t > &args);

template< view_t V >
unsigned
pinned_within(const state_t &s, const call_t< V > &c)
{
    return call_pins(s, c.args);
}

inline unsigned
pinned_within(const state_t &s, const call_void_t &c)
{
    return call_pins(s, c.args);
}

// A compound assignment pins what its operator's instruction pins, rcc's
// pins() of an assign_t, and what its value pins.
template< binary_op_t op, typename W, view_t V, view_t O, typename R >
unsigned
pinned_within(const state_t &s, const compound_t< op, W, V, O, R > &c)
{
    return pins(op, constant< R >) | pinned_within(s, c.value);
}

// A step pins nothing, rcc's pins() of a unary_t.
template< step_op_t op, typename W, view_t V, sign_t S >
constexpr unsigned
pinned_within(const state_t &, const step_t< op, W, V, S > &)
{
    return 0;
}

// A conditional pins what its three parts pin, and moves nothing held,
// since its paths spill every held value first.
template< view_t V, bool jumps >
unsigned
pinned_within(const state_t &s, const conditional_t< V, jumps > &c)
{
    return pinned_within(s, c.cond) | pinned_within(s, c.yes) |
           pinned_within(s, c.no);
}

inline unsigned
pinned_within(const state_t &s, const conditional_void_t &c)
{
    return pinned_within(s, c.cond) | pinned_within(s, c.yes) |
           pinned_within(s, c.no);
}

// A conditional under a widening pins what the conditional pins: an
// arm's widening pins nothing of its own.
template< view_t V, bool jumps >
unsigned
pinned_within(const state_t &s, const widened_arms_t< V, jumps > &w)
{
    return pinned_within(s, w.conditional);
}

// A comma pins nothing of its own, rcc's pins() of one, and what its
// two operands pin.
template< typename R >
unsigned
pinned_within(const state_t &s, const comma_t< R > &c)
{
    return pinned_within(s, c.lhs) | pinned_within(s, c.rhs);
}

// The registers a pinned instruction within a node would move a value
// held across the node out of, rcc's moved_within(), 6.5: pins() over
// the tree, as pinned_within(), but nothing below && and || and nothing
// within a call, which spill every held value first, the first before
// their paths, before_paths(), and a call at its start, so that no
// value is held in a register where an instruction within them pins
// one.  A summary of its own, each node's of its operands', which an
// erased one hands up through its handle.
template< constant C >
constexpr unsigned
moved_within(const state_t &, const C &)
{
    return 0;
}

template< typename W, view_t V, sign_t S >
constexpr unsigned
moved_within(const state_t &, const load_t< W, V, S > &)
{
    return 0;
}

template< typename W, view_t V, sign_t S >
constexpr unsigned
moved_within(const state_t &, const place_t< W, V, S > &)
{
    return 0;
}

template< typename W >
constexpr unsigned
moved_within(const state_t &, const address_of_t< W > &)
{
    return 0;
}

template< unary_op_t op, view_t V, typename A >
unsigned
moved_within(const state_t &s, const unary_t< op, V, A > &u)
{
    return moved_within(s, u.arg);
}

template< binary_op_t op, view_t V, typename L, typename R >
unsigned
moved_within(const state_t &s, const binary_t< op, V, L, R > &b)
{
    return pins(op, constant< R >) | moved_within(s, b.lhs) |
           moved_within(s, b.rhs);
}

template< binary_op_t op, view_t V, typename L, typename R >
    requires logical_operator< op >
constexpr unsigned
moved_within(const state_t &, const binary_t< op, V, L, R > &)
{
    return 0;
}

template< family_t F >
unsigned
moved_within(const state_t &s, const top_t< F > &t)
{
    return moved_within(s, t.node);
}

template< view_t V, view_t F, typename A >
unsigned
moved_within(const state_t &s, const sext_t< V, F, A > &e)
{
    return moved_within(s, e.arg);
}

template< view_t V, view_t F, typename A >
unsigned
moved_within(const state_t &s, const zext_t< V, F, A > &e)
{
    return moved_within(s, e.arg);
}

template< view_t V, typename A >
unsigned
moved_within(const state_t &s, const trunc_t< V, A > &t)
{
    return moved_within(s, t.arg);
}

template< typename W, view_t V >
unsigned
moved_within(const state_t &s, const assign_t< W, V > &a)
{
    return moved_within(s, a.value);
}

template< view_t V >
constexpr unsigned
moved_within(const state_t &, const call_t< V > &)
{
    return 0;
}

constexpr unsigned
moved_within(const state_t &, const call_void_t &)
{
    return 0;
}

template< binary_op_t op, typename W, view_t V, view_t O, typename R >
unsigned
moved_within(const state_t &s, const compound_t< op, W, V, O, R > &c)
{
    return pins(op, constant< R >) | moved_within(s, c.value);
}

template< step_op_t op, typename W, view_t V, sign_t S >
constexpr unsigned
moved_within(const state_t &, const step_t< op, W, V, S > &)
{
    return 0;
}

template< view_t V, bool jumps >
constexpr unsigned
moved_within(const state_t &, const conditional_t< V, jumps > &)
{
    return 0;
}

constexpr unsigned
moved_within(const state_t &, const conditional_void_t &)
{
    return 0;
}

template< view_t V, bool jumps >
constexpr unsigned
moved_within(const state_t &, const widened_arms_t< V, jumps > &)
{
    return 0;
}

template< typename R >
unsigned
moved_within(const state_t &s, const comma_t< R > &c)
{
    return moved_within(s, c.lhs) | moved_within(s, c.rhs);
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

// The place of an assignment, held across its value, rcc's place_of():
// the address of where, formed outside the registers a pinned
// instruction within the value would move it out of, moved_within(),
// and outside the consumer's register where no instruction within the
// value pins one, wanted_left(), so that the value takes it.  rcc adds
// pinned_by() of the value, which moved_within() holds: return g = a /
// b on an object only declared takes rcx for the GOT's base, outside
// rax and rdx, and return g = 7 rcx, outside the result's rax.  A
// slot's address and a label take no register, so a local's and a
// defined object's place is no reservation.
template< typename W >
address_t
place_of(state_t &s, const W &where, const expr_t &value, unsigned wanted)
{
    return address_avoiding(
        s, where, moved_within(s, value) | wanted_left(s, wanted, 0, value));
}

// The registers a node generates its right operand with left for last,
// rcc's held_out(): named, those its left operand l was handed, where l
// holds one of them, so that a right operand that spills l leaves it
// free for l's reload; none where l holds another register, where the
// result stays.  l is the value its row has just returned, in a
// register, so rcc's case of a spilled l does not arise.
unsigned held_out(const state_t &, value_t l, unsigned named);

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

// The left operand of a node whose right operand is rhs, generated
// outside pinned and handed named, rvalue_avoiding()'s, with the
// registers a pinned instruction within rhs would move it out of added
// to s.ahead, rcc's binary() and moved_within(): the left operand's
// value is held while rhs is generated, so a row within it that chooses
// its result's register, right_written(), chooses none of them.  A call
// within rhs spills the value first, so it adds none, (a + g(b)) * (b +
// g(c)) reading a in its slot where (a + g(b)) * (b + c / a) reloads
// it.
template< typename L, typename R >
value_t
left_operand(
    state_t &s, const L &lhs, const R &rhs, unsigned pinned, unsigned named)
{
    const unsigned ahead =
        std::exchange(s.ahead, s.ahead | moved_within(s, rhs));
    const value_t l = rvalue_avoiding(s, lhs, pinned, named);
    s.ahead = ahead;

    return l;
}

// The operation of op on the object at an address at the view, op R,
// obj, rcc's arithmetic_on() and modify() of a step, chapter 6.6: the
// source an immediate or a register's name, addl $1, -4(%rbp), addl
// %eax, g(%rip), addl $1, (%rcx), the address's operand formed outside
// avoid; add, sub, and, or and xor set ZF from the object's new value
// (7.1).  The address stays held, for the caller to give back.
void operate_on(state_t &,
                binary_op_t op,
                std::string_view source,
                const address_t &at,
                view_t view,
                unsigned avoid = 0);

// The operation of op on the object at an address by the held value r,
// consumed, op R, obj, rcc's arithmetic_on(): r's register reloaded
// outside the address's, then the address's operand outside r's, addl
// %eax, -4(%rbp), addl %eax, (%rcx).  The address stays held.
void operate_on(
    state_t &, binary_op_t op, value_t r, const address_t &at, view_t view);

// The shift of the object at an address by the held count r, of the
// view count, consumed, rcc's shift_on(): the count moved into cl,
// count_into_cl(), outside the address's registers, then the address's
// operand outside rcx, shll %cl, -4(%rbp), sar for a signed object.
void shift_on(state_t &,
              binary_op_t op,
              value_t r,
              view_t count,
              const address_t &at,
              view_t view);

// The load of the object at an address held for a store through it
// after the load, rcc's fetch(): the address's operand, then the object
// of the view, extended by the sign to a register of to bits,
// extension()'s, taken outside avoid and the address's own, movl
// -4(%rbp), %eax, movl (%rcx), %eax, and movsbq -1(%rbp), %rax for a
// char under an operation of 64 bits.  The address stays held.
value_t fetch(state_t &,
              const address_t &at,
              view_t view,
              sign_t sign,
              int to,
              unsigned avoid = 0);

// The value of a step of the object at an address, of the view and of
// a type of the sign, rcc's step(), chapter 6.4, which gives the
// address back: prefix, the object fetched into a fresh register, the
// add or sub of 1 on it and its store, the value that register;
// postfix, the object fetched outside the registers the consumer
// names, wanted, its old value copied into a fresh register outside
// that one and the address's, then the add and the store, the value
// the copy: movl -4(%rbp), %ecx, movl %ecx, %eax, addl $1, %ecx and
// movl %ecx, -4(%rbp) for return a++.  Nothing reads the flags of the
// add before the store, and rcc records none.
value_t step(state_t &,
             step_op_t op,
             const address_t &at,
             view_t view,
             sign_t sign,
             unsigned wanted);

// The store of the held value v through the address at, held, rcc's
// store(): v's register, reloaded outside the address's, then the
// address's operand outside v's.
void stored(state_t &, value_t v, const address_t &at, view_t view);

// op= by an immediate source on the object of the view and of a type
// of the sign, at the view of the operation, rcc's assign() row on an
// immediate: the object fetched, extended to the operation's view, the
// operation on its register, addl $5, %eax, the three-operand imull $5,
// %eax, %eax, and the store at the object's view, the address given
// back, the value that register.
value_t immediate_compound(state_t &,
                           binary_op_t op,
                           const address_t &at,
                           std::string_view source,
                           view_t view,
                           sign_t sign,
                           view_t operation);

// The value of op= of the held right operand r on the object at an
// address held across it, of the view and of a type of the sign, at the
// view of the operation, rcc's assign() past its right operand, chapter
// 6.6, which gives the address back: where op commutes, the operation
// is at the object's view and right_written() says, the object the
// instruction's source on r's register, addl -4(%rbp), %ecx, flagged
// but for *, and the store from it; otherwise the object fetched into a
// fresh register, extended to the operation's view, the operation of
// op's family on it, the count of the view count in cl for a shift, and
// the store at the object's view, the value that register, or for a
// remainder rdx, where idiv leaves it.
value_t compound(state_t &,
                 binary_op_t op,
                 value_t r,
                 const address_t &at,
                 view_t view,
                 sign_t sign,
                 view_t operation,
                 view_t count,
                 unsigned wanted);

// The three-operand imul of the object at an address by the immediate
// factor at the view, rcc's object_product(): imull $5, -4(%rbp), %eax,
// into the register the consumer names, wanted, where it is free, else
// a fresh one outside the address's.  The address stays held.
value_t object_product(state_t &,
                       const address_t &at,
                       std::string_view factor,
                       view_t view,
                       unsigned wanted);

// The place of a compound assignment, formed first and held across its
// value, rcc's place_of() of one: an assignment's, place_of()'s
// registers, and those the operation's own instruction pins, which no
// summary of the value holds, rcc's pinned_by() of an assign_t: g /= b
// on an object only declared forms its GOT's base outside rax and rdx.
template< binary_op_t op, typename W, typename R >
address_t
compound_place(state_t &s, const W &where, const R &value, unsigned wanted)
{
    return address_avoiding(s, where,
                            moved_within(s, value) |
                                wanted_left(s, wanted, 0, value) |
                                pins(op, constant< R >));
}

// The value of a compound assignment, rcc's assign(), 6.6: the place
// first, then the right operand, held with rax left for last so that the
// object's load takes it, then compound().  A division's is held
// outside rax and rdx too, a constant also, since idiv takes no
// immediate, and its reservation is cleared around the whole, as the
// division's other rows clear theirs, rcc's pinned(); a shift's count
// is shift_count()'s.  A wide_t is no immediate, so it is held, movabsq
// into rcx.
template< binary_op_t op, typename W, view_t V, view_t O, typename R >
    requires arithmetic_operator< op >
value_t
value(state_t &s, const compound_t< op, W, V, O, R > &c, unsigned wanted = 0)
{
    const address_t at = compound_place< op >(s, c.where, c.value, wanted);
    const value_t r = rvalue_avoiding(s, c.value, bit(rax));

    return compound(s, op, r, at, V, c.sign, O, O, wanted);
}

template< binary_op_t op, typename W, view_t V, view_t O, typename R >
    requires division_operator< op >
value_t
value(state_t &s, const compound_t< op, W, V, O, R > &c, unsigned wanted = 0)
{
    const unsigned outer = std::exchange(s.reserved, 0u);

    const address_t at = compound_place< op >(s, c.where, c.value, wanted);
    const value_t r =
        rvalue_avoiding(s, c.value, bit(rax) | pins(op, constant< R >));
    const value_t v = compound(s, op, r, at, V, c.sign, O, O, wanted);

    s.reserved = outer;

    return v;
}

template< binary_op_t op, typename W, view_t V, view_t O, typename R >
    requires shift_operator< op >
value_t
value(state_t &s, const compound_t< op, W, V, O, R > &c, unsigned wanted = 0)
{
    const address_t at = compound_place< op >(s, c.where, c.value, wanted);
    const value_t r = shift_count(s, c.value, bit(rax));

    return compound(s, op, r, at, V, c.sign, O, view_of(c.value), wanted);
}

// An immediate the operation takes, + - * & | ^ by it and a shift by a
// constant count, rcc's: the place, then the object fetched, the
// operation on its register, addl $5, %eax, imull $5, %eax, %eax, shll
// $33, %eax, and the store.
template< binary_op_t op, typename W, view_t V, view_t O, immediate I >
    requires arithmetic_operator< op >
value_t
value(state_t &s, const compound_t< op, W, V, O, I > &c, unsigned wanted = 0)
{
    const address_t at = compound_place< op >(s, c.where, c.value, wanted);

    return immediate_compound(s, op, at, immediate_of(c.value), V, c.sign, O);
}

template< binary_op_t op, typename W, view_t V, view_t O, constant C >
    requires shift_operator< op >
value_t
value(state_t &s, const compound_t< op, W, V, O, C > &c, unsigned wanted = 0)
{
    const address_t at = compound_place< op >(s, c.where, c.value, wanted);

    return immediate_compound(s, op, at, count_of(value_of(c.value)), V, c.sign,
                              O);
}

// *= by an immediate where the operation is at the object's view,
// object_product()'s, the object imul's source, and the store.
template< typename W, view_t V, immediate I >
value_t
value(state_t &s,
      const compound_t< binary_op_t::mul, W, V, V, I > &c,
      unsigned wanted = 0)
{
    const address_t at =
        compound_place< binary_op_t::mul >(s, c.where, c.value, wanted);
    const value_t v = object_product(s, at, immediate_of(c.value), V, wanted);
    stored(s, v, at, V);
    release(s, at);

    return v;
}

// A constant the memory form on an object of the view V takes as its
// immediate, written at V, rcc's immediate() at the object's width: any
// below 64 bits, whose bits modulo 2^N a 32-bit immediate holds, c +=
// 300 being addb $44, -1(%rbp); at 64 bits one in int's range, which a
// wide_t is not.
template< typename C, view_t V >
concept memory_immediate = constant< C > && (width(V) < 64 || immediate< C >);

// The shift of the memory form, by the sign of the object's type, rcc's
// shift_mnemonic() on the object: >> is sar on a signed object and shr
// on an unsigned one whatever the operation's sign, which is its
// promoted type's, so unsigned char >>= is shrb, where its value is
// sarl's.
constexpr binary_op_t
shift_on_object(binary_op_t op, sign_t sign)
{
    if (op == binary_op_t::shl)
        return op;

    return sign == sign_t::signed_ ? binary_op_t::ashr : binary_op_t::lshr;
}

// A compound assignment for its effect, rcc's modify(), arithmetic_on()
// and shift_on(), chapter 6.6: the place first, then + - & | ^, << and
// >> on the object in memory at its view, the immediate as written at
// that view, addl $5, -4(%rbp), addl $0 for a += 0, addb $44 for c +=
// 300, shll $33 for a <<= 33, or the value held, its low bits the
// instruction's source, addb %al, -1(%rbp) for c += l, the count in cl,
// shll %cl, -4(%rbp).  * / % have no such form, imul no memory
// destination and idiv none at all, and are their value dropped.
template< binary_op_t op, typename W, view_t V, view_t O, typename R >
    requires memory_operator< op >
void
effect(state_t &s, const compound_t< op, W, V, O, R > &c)
{
    const address_t at = compound_place< op >(s, c.where, c.value, 0);
    operate_on(s, op, value(s, c.value), at, V);
    release(s, at);
}

template< binary_op_t op, typename W, view_t V, view_t O, typename C >
    requires memory_operator< op > && memory_immediate< C, V >
void
effect(state_t &s, const compound_t< op, W, V, O, C > &c)
{
    const address_t at = compound_place< op >(s, c.where, c.value, 0);
    operate_on(s, op, immediate_at(value_of(c.value), V), at, V);
    release(s, at);
}

template< binary_op_t op, typename W, view_t V, view_t O, typename R >
    requires shift_operator< op >
void
effect(state_t &s, const compound_t< op, W, V, O, R > &c)
{
    const address_t at = compound_place< op >(s, c.where, c.value, 0);
    shift_on(s, shift_on_object(op, c.sign), shift_count(s, c.value, 0),
             view_of(c.value), at, V);
    release(s, at);
}

template< binary_op_t op, typename W, view_t V, view_t O, constant C >
    requires shift_operator< op >
void
effect(state_t &s, const compound_t< op, W, V, O, C > &c)
{
    const address_t at = compound_place< op >(s, c.where, c.value, 0);
    operate_on(s, shift_on_object(op, c.sign), count_of(value_of(c.value)), at,
               V);
    release(s, at);
}

// The place first, its address formed outside the registers the
// consumer names, so that the value takes them, rcc's: a step pins
// nothing, so nothing within it moves them.
template< step_op_t op, typename W, view_t V, sign_t S >
value_t
value(state_t &s, const step_t< op, W, V, S > &t, unsigned wanted = 0)
{
    const address_t at = address_avoiding(s, t.where, wanted);

    return step(s, op, at, V, S, wanted);
}

// A step for its effect, rcc's modify(): addl $1 or subl $1 on the object
// in memory, never incl, and no old value copied, since nothing reads
// it; addl $1, g(%rip) and, through the GOT, addl $1, (%rax).
template< step_op_t op, typename W, view_t V, sign_t S >
void
effect(state_t &s, const step_t< op, W, V, S > &t)
{
    const address_t at = address(s, t.where);
    operate_on(s, is_increment(op) ? binary_op_t::add : binary_op_t::sub,
               imm(1), at, V);
    release(s, at);
}

// The primary of + - * & | ^, 6.5.6, 6.5.7, 6.5.11 to 6.5.13, rcc's
// binary() and arithmetic(), chapter 6.1: the left operand's value
// first, G13, generated outside the registers the right operand's top
// operator pins, so that a + (b << a) loads a into eax and not into the
// ecx the count goes to, and handed the consumer's register,
// wanted_left(); held while the right operand's is taken in a register
// of its own, outside held_out(); then arithmetic(), addl %edx, %eax,
// or into the right operand's register where it is the one the
// consumer names: the count (0 << d) + (d = c) is addl %edx, %ecx, its
// left operand a shift, held outside ecx, and its right one loaded into
// the ecx a count's consumer names.  A wide_t is no immediate, so it is
// this row, movabsq into a register where the source has it.
template< binary_op_t op, view_t V, typename L, typename R >
    requires arithmetic_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    const unsigned named = wanted_left(s, wanted, 0, b.rhs);

    const value_t l = left_operand(s, b.lhs, b.rhs, pinned_by(b.rhs), named);
    const value_t r = rvalue_avoiding(s, b.rhs, held_out(s, l, named));

    return arithmetic(s, op, l, r, V, wanted);
}

// Whether a node with the constant L on the left of op is written with
// its operands exchanged, rcc's constant_first() as ordered() asks it:
// op commutes, and the constant is an immediate, which a wide_t is not.
template< binary_op_t op, typename L >
concept constant_first = commutes(op) && immediate< L >;

// The niches of the primary, chapter 6.1.  A constant on the left of an
// operator that commutes is the row of the node with its operands
// exchanged, 5 + a as a + 5: the value of the right operand, which is
// no constant, then the constant as the immediate.
template< binary_op_t op, view_t V, typename L, typename R >
    requires arithmetic_operator< op > && constant_first< op, L >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    return value(s, binary_t< op, V, R, L > { b.rhs, b.lhs }, wanted);
}

// A load of the operation's view on the right is the instruction's
// operand in memory, addl -8(%rbp), %eax, addl g(%rip), %eax, and takes
// no register but a GOT's base, addl (%rcx), %eax, rcc's
// binary_operand() and from_memory(): the left operand's value, then
// the address, formed outside held_out(), then the instruction on the
// left's register and the address's operand, operands(), and the
// address given back.  Its type is the more special, so the node the
// mirror takes, 5 + a, is excluded by name; 5 - a is this row, movl $5,
// %eax and subl -4(%rbp), %eax.  The right operand pins nothing, so the
// left operand is handed the consumer's register, as in every niche of
// an operand in memory or an immediate.
template< binary_op_t op, view_t V, typename L, typename W, sign_t S >
    requires(arithmetic_operator< op > && !constant_first< op, L >)
value_t
value(state_t &s,
      const binary_t< op, V, L, load_t< W, V, S > > &b,
      unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    const address_t at =
        address_avoiding(s, b.rhs.where, held_out(s, l, wanted));
    const auto [left, source] = operands(s, l, at);
    operate(s, op, left, source, V);
    release(s, at);

    return l;
}

// An immediate on the right is the instruction's, negative ones as
// written: addl $5, %eax, andq $-2, %rax.  Of the mirror's nodes it
// would take only a constant on both sides, which the builder folds.
template< binary_op_t op, view_t V, typename L, immediate I >
    requires arithmetic_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, I > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    operate(s, op, reg_of(s, l), immediate_of(b.rhs), V);

    return l;
}

// imul by an immediate is the three-operand form, rcc's arithmetic():
// the left operand's value, its register the source and the
// destination, imull $5, %eax, %eax.
template< view_t V, typename L, immediate I >
value_t
value(state_t &s,
      const binary_t< binary_op_t::mul, V, L, I > &b,
      unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    multiply(s, reg_of(s, l), reg_of(s, l), immediate_of(b.rhs), V);

    return l;
}

// A load of the operation's view on the left of imul by an immediate
// is the source in memory, rcc's product(): imull $5, -4(%rbp), %eax,
// no load, the address's operand formed and the address given back
// first, then the product in a register taken for it, the lowest free
// whatever the consumer names, a GOT's base among them, imull $5,
// (%rax), %eax.  5 * a is this row, through the mirror.
template< view_t V, immediate I, typename W, sign_t S >
value_t
value(state_t &s,
      const binary_t< binary_op_t::mul, V, load_t< W, V, S >, I > &b,
      unsigned = 0)
{
    const address_t at = address(s, b.lhs.where);
    const std::string source = operand(s, at);
    release(s, at);

    const value_t v = fresh(s);
    multiply(s, reg_of(s, v), source, immediate_of(b.rhs), V);

    return v;
}

// / and % by a divisor that is no local, 6.5.6, rcc's binary() and
// divide(), chapter 6.5: idiv takes its dividend in rax, and writes the
// quotient there and the remainder in rdx.  A division keeps its own
// pin, rcc's rvalue(): it is generated outside any reservation around
// it, since the pin, not the outer wish, decides where its result is
// written.  The dividend first, G13, outside what the divisor's top
// operator pins and handed the consumer's register, wanted_left();
// then the divisor, held outside rax and rdx and held_out(), a constant
// too, since idiv takes no immediate, movl $5, %ecx; then divide().
template< binary_op_t op, view_t V, typename L, typename R >
    requires division_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    const unsigned outer = std::exchange(s.reserved, 0u);
    const unsigned named = wanted_left(s, wanted, 0, b.rhs);

    const value_t l = left_operand(s, b.lhs, b.rhs, pinned_by(b.rhs), named);
    const value_t r = rvalue_avoiding(
        s, b.rhs, held_out(s, l, named) | pins(op, constant< R >));
    const value_t v = divide(s, op, l, r, V, wanted);

    s.reserved = outer;

    return v;
}

// An object of the operation's view as the divisor is idiv's operand
// in memory, idivl -8(%rbp), idivl g(%rip), and takes no register but a
// GOT's base, rcc's binary_operand(): the dividend's value, handed the
// consumer's register, since the divisor pins nothing; then the
// divisor's address, formed outside rax and rdx and held_out(), so that
// a base lands where the division moves nothing; then divide().
template< binary_op_t op, view_t V, typename L, typename W, sign_t S >
    requires division_operator< op >
value_t
value(state_t &s,
      const binary_t< op, V, L, load_t< W, V, S > > &b,
      unsigned wanted = 0)
{
    const unsigned outer = std::exchange(s.reserved, 0u);

    const value_t l = value(s, b.lhs, wanted);
    const address_t at = address_avoiding(
        s, b.rhs.where, held_out(s, l, wanted) | pins(op, false));
    const value_t v = divide(s, op, l, at, V, wanted);

    s.reserved = outer;

    return v;
}

// A shift's count that is no constant, rcc's shift_count(): generated
// with rax and avoid reserved and with rcx taken out of what a node
// around the shift reserves for its own operand, since the instruction
// consumes the count in cl (chapter 6.5), so a count's load lands in
// rcx; and rcx is the register the count's consumer names, as a return
// names rax (6.1).  The count's row takes the name where rcx is free, so
// the extension of a call's narrow result is movsbl %al, %ecx, and the
// shifted value, spilled by the call, is reloaded into rax.
template< typename R >
value_t
shift_count(state_t &s, const R &count, unsigned avoid)
{
    const unsigned outer = std::exchange(s.reserved, s.reserved & ~bit(rcx));
    const value_t v = rvalue_avoiding(s, count, avoid | bit(rax), bit(rcx));
    s.reserved = outer;

    return v;
}

// << and >> by a count that is no constant, 6.5.8, rcc's binary() and
// shift(), chapter 6.5: the shifted value first, G13, generated with
// rcx left for last, where the count goes, and outside what the count's
// top operator pins, and handed the consumer's register but rcx,
// wanted_left(); then the count, shift_count(), held, since the
// instruction takes no operand in memory, outside held_out(); then
// shift(), shll %cl, %eax, sarq for >> of a long, which the machine
// masks to the width (G3).  The count is of its own type, which its view
// says.  The shifted value's register holds the node's value.
template< binary_op_t op, view_t V, typename L, typename R >
    requires shift_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    const unsigned own = pins(op, constant< R >);
    const unsigned named = wanted_left(s, wanted, own, b.rhs);

    const value_t l =
        left_operand(s, b.lhs, b.rhs, own | pinned_by(b.rhs), named);
    const value_t r = shift_count(s, b.rhs, held_out(s, l, named));

    shift(s, op, l, r, V, view_of(b.rhs));

    return l;
}

// A constant count is the instruction's immediate, count_of(), and pins
// nothing: shll $33, %eax, shlq $137, %rax, and shll $255, %eax for a
// << -1.
template< binary_op_t op, view_t V, typename L, constant C >
    requires shift_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, C > &b, unsigned wanted = 0)
{
    const value_t l = value(s, b.lhs, wanted);
    operate(s, op, reg_of(s, l), count_of(value_of(b.rhs)), V);

    return l;
}

// A comma on the right, rcc's binary() and operand_of(), chapter 6.1:
// the left operand as for any right operand, then the comma's left
// operand for its effect, outside the reservation of the right one,
// then the comma's right operand as the instruction takes it in its own
// form, binary_operand()'s.  The registers the right operand is taken
// outside, held_out()'s, are said before the effect, as rcc says them,
// while the left operand is in the register its row returned.  A
// constant on the left of an operator that commutes is the mirror's,
// which wins.  The family's operation on a comma's right operand by its
// class is the family's *_after(), below each.

// The operation of + - * & | ^ on the held left value l and a comma's
// right operand: an object in memory as the instruction's operand,
// addl -12(%rbp), %eax, its address formed outside avoid, a GOT's base
// among them, and given back; an immediate, addl $5, %eax and the
// three-operand imull $5, %eax, %eax; anything else its value, held
// outside avoid, then arithmetic().
template< binary_op_t op, view_t V, typename W, sign_t S >
value_t
arithmetic_after(state_t &s,
                 value_t l,
                 const load_t< W, V, S > &rhs,
                 unsigned avoid,
                 unsigned)
{
    const address_t at = address_avoiding(s, rhs.where, avoid);
    const auto [left, source] = operands(s, l, at);
    operate(s, op, left, source, V);
    release(s, at);

    return l;
}

template< binary_op_t op, view_t V, immediate I >
value_t
arithmetic_after(state_t &s, value_t l, const I &rhs, unsigned, unsigned)
{
    const reg_t r = reg_of(s, l);
    if constexpr (op == binary_op_t::mul)
        multiply(s, r, r, immediate_of(rhs), V);
    else
        operate(s, op, r, immediate_of(rhs), V);

    return l;
}

template< binary_op_t op, view_t V, typename R >
value_t
arithmetic_after(
    state_t &s, value_t l, const R &rhs, unsigned avoid, unsigned wanted)
{
    const value_t r = rvalue_avoiding(s, rhs, avoid);

    return arithmetic(s, op, l, r, V, wanted);
}

template< binary_op_t op, view_t V, typename L, typename R >
    requires arithmetic_operator< op > && (!constant_first< op, L >)
value_t
value(state_t &s,
      const binary_t< op, V, L, comma_t< R > > &b,
      unsigned wanted = 0)
{
    const unsigned named = wanted_left(s, wanted, 0, b.rhs);

    const value_t l = left_operand(s, b.lhs, b.rhs, pinned_by(b.rhs), named);
    const unsigned avoid = held_out(s, l, named);
    effect(s, b.rhs.lhs);

    return arithmetic_after< op, V >(s, l, b.rhs.rhs, avoid, wanted);
}

// The division of / and % of the held dividend l by a comma's right
// operand: an object in memory as idiv's operand, idivl -12(%rbp), its
// address formed outside avoid and rax and rdx; anything else held
// outside them, a constant too.
template< binary_op_t op, view_t V, typename W, sign_t S >
value_t
divide_after(state_t &s,
             value_t l,
             const load_t< W, V, S > &rhs,
             unsigned avoid,
             unsigned wanted)
{
    const address_t at =
        address_avoiding(s, rhs.where, avoid | pins(op, false));

    return divide(s, op, l, at, V, wanted);
}

template< binary_op_t op, view_t V, typename R >
value_t
divide_after(
    state_t &s, value_t l, const R &rhs, unsigned avoid, unsigned wanted)
{
    const value_t r = rvalue_avoiding(s, rhs, avoid | pins(op, constant< R >));

    return divide(s, op, l, r, V, wanted);
}

// / and % keep their own pin, the reservation around them cleared, as
// the division's other rows do.
template< binary_op_t op, view_t V, typename L, typename R >
    requires division_operator< op >
value_t
value(state_t &s,
      const binary_t< op, V, L, comma_t< R > > &b,
      unsigned wanted = 0)
{
    const unsigned outer = std::exchange(s.reserved, 0u);
    const unsigned named = wanted_left(s, wanted, 0, b.rhs);

    const value_t l = left_operand(s, b.lhs, b.rhs, pinned_by(b.rhs), named);
    const unsigned avoid = held_out(s, l, named);
    effect(s, b.rhs.lhs);
    const value_t v = divide_after< op, V >(s, l, b.rhs.rhs, avoid, wanted);

    s.reserved = outer;

    return v;
}

// The shift of the held value l by a comma's right operand: a constant
// is the instruction's immediate, count_of(); anything else is
// shift_count()'s, held outside avoid, then shift().
template< binary_op_t op, view_t V, constant C >
void
shift_after(state_t &s, value_t l, const C &count, unsigned)
{
    operate(s, op, reg_of(s, l), count_of(value_of(count)), V);
}

template< binary_op_t op, view_t V, typename R >
void
shift_after(state_t &s, value_t l, const R &count, unsigned avoid)
{
    const value_t r = shift_count(s, count, avoid);
    shift(s, op, l, r, V, view_of(count));
}

// << and >> pin rcx for a comma's count whatever its right operand is,
// rcc's pins() of a count that is no literal, so the shifted value is
// generated outside rcx: d + (a << (b, 3)) is movl -4(%rbp), %edx, movl
// -8(%rbp), %ecx and shll $3, %edx.
template< binary_op_t op, view_t V, typename L, typename R >
    requires shift_operator< op >
value_t
value(state_t &s,
      const binary_t< op, V, L, comma_t< R > > &b,
      unsigned wanted = 0)
{
    const unsigned own = pins(op, false);
    const unsigned named = wanted_left(s, wanted, own, b.rhs);

    const value_t l =
        left_operand(s, b.lhs, b.rhs, own | pinned_by(b.rhs), named);
    const unsigned avoid = held_out(s, l, named);
    effect(s, b.rhs.lhs);
    shift_after< op, V >(s, l, b.rhs.rhs, avoid);

    return l;
}

// cmp of a source and the held value l at the view, rcc's
// compare_with(): a held value r, cmpl %ecx, %eax, the registers of
// operands(), a spilled value reloaded, and r released; or an operand
// as its text, an immediate, cmpl $5, %eax, or an object in memory,
// cmpq -16(%rbp), %rax.  It sets the flags and writes no register; l
// stays held.
void compare(state_t &, value_t l, value_t r, view_t view);
void compare(state_t &, value_t l, std::string_view source, view_t view);

// cmp of a constant and the held value l at the view, the constant the
// immediate written at the view, immediate_at(), cmpl $2, %eax; l stays
// held.
void compare(state_t &, value_t l, long long constant, view_t view);

// cmp of an immediate and the object at an address at the view, rcc's
// condition_on(): the object compared in place, cmpl $5, -4(%rbp), cmpl
// $5, (%rax), and no register holds it; the address is consumed.
void compare(state_t &,
             const address_t &at,
             std::string_view immediate,
             view_t view);

// The value of a comparison of op from the flags its cmp set, rcc's
// comparison() after compared(): setcc into the low byte of a register,
// then movzbl to the int.  l is the value the cmp held, none where it
// compared an object in place.
value_t
setcc(state_t &, binary_op_t op, std::optional< value_t > l, unsigned wanted);

// The cmp of the held left value l and the right operand of a
// comparison at the view V, rcc's compare_with(), by the right
// operand's class: its value, generated as for any consumer, held and
// freed, cmpl %ecx, %eax, a wide_t's by movabsq; a load, the
// instruction's operand in memory, cmpl -8(%rbp), %eax, which takes no
// register but a GOT's base, its address formed after l and the object
// read by the cmp, operands(); an immediate, cmpl $5, %eax.
template< view_t V, typename R >
void
compare_with(state_t &s, value_t l, const R &rhs)
{
    compare(s, l, value(s, rhs), V);
}

template< view_t V, typename W, sign_t S >
void
compare_with(state_t &s, value_t l, const load_t< W, V, S > &rhs)
{
    const address_t at = address(s, rhs.where);
    const std::string source = operands(s, l, at).second;
    compare(s, l, source, V);
    release(s, at);
}

template< view_t V, immediate I >
void
compare_with(state_t &s, value_t l, const I &rhs)
{
    compare(s, l, immediate_of(rhs), V);
}

// The primary of < > <= >= == !=, 6.5.9, 6.5.10, rcc's comparison(),
// compared() and compare(), chapter 6.3: the left operand's value
// first, G13, generated outside the registers the right operand's top
// operator pins; then the cmp with the right operand at the operation's
// view, compare_with(); then the value of the flags, setcc(), in the
// left operand's register, cmpl -8(%rbp), %eax, setl %al and movzbl
// %al, %eax.
template< binary_op_t op, view_t V, typename L, typename R >
    requires comparison_operator< op >
value_t
value(state_t &s, const binary_t< op, V, L, R > &b, unsigned wanted = 0)
{
    const value_t l = rvalue_avoiding(s, b.lhs, pinned_by(b.rhs));
    compare_with< V >(s, l, b.rhs);

    return setcc(s, op, l, wanted);
}

// A load compared with an immediate is the cmp of the object in place,
// cmpl $5, -4(%rbp), compare(); the object is read by the cmp and by
// nothing before it, G13, and no register holds it, so the setcc takes
// one.
template< binary_op_t op, view_t V, immediate I, typename W, sign_t S >
    requires comparison_operator< op >
value_t
value(state_t &s,
      const binary_t< op, V, load_t< W, V, S >, I > &b,
      unsigned wanted = 0)
{
    compare(s, address(s, b.lhs.where), immediate_of(b.rhs), V);

    return setcc(s, op, std::nullopt, wanted);
}

// A constant on the left is the row of the node mirrored, rcc's
// ordered(): 5 < a as a > 5, mirrored(), the constant the immediate on
// the right, so 5 < a is cmpl $5, -4(%rbp) and setg %al.
template< binary_op_t op, view_t V, immediate I, typename R >
    requires comparison_operator< op >
value_t
value(state_t &s, const binary_t< op, V, I, R > &b, unsigned wanted = 0)
{
    return value(s, binary_t< mirrored(op), V, R, I > { b.rhs, b.lhs }, wanted);
}

// !e, 6.5.4.3p5, rcc's unary(): the operand's value, its test against
// 0 at the operand's view, cmpl $0, %eax, then the value of the flags
// as e == 0 has it, sete and movzbl, setcc().  The operand is loaded,
// where e == 0 compares a load in place.
template< view_t V, typename A >
value_t
value(state_t &s,
      const unary_t< unary_op_t::log_not, V, A > &u,
      unsigned wanted = 0)
{
    const value_t v = value(s, u.arg);
    compare(s, v, imm(0), V);

    return setcc(s, binary_op_t::eq, v, wanted);
}

// A comparison as a jump, rcc's comparison_branch(), 7.1: the cmp of
// the value row, compare_with(), then jcc of the operator where sense
// and of its inverse otherwise, jl or jge, and no value held.
template< binary_op_t op, view_t V, typename L, typename R >
    requires comparison_operator< op >
void
branch(state_t &s, const binary_t< op, V, L, R > &b, int label, bool sense)
{
    const value_t l = rvalue_avoiding(s, b.lhs, pinned_by(b.rhs));
    compare_with< V >(s, l, b.rhs);
    release(s, l);

    jump_if(s, sense ? op : inverse(op), label);
}

// e == 0 and e != 0, rcc's zero_compared(): the jump of 7.1 reads ZF
// alone, so e == 0 is e's own jump on its value compared with 0, the
// sense flipped, and e != 0 that jump as it is, value_branch(), where
// e's row may set ZF and write no cmp: (a + b) == 0 is addl and jne.
// It wins over the cmp of a load in place, below, whose text it writes
// for a == 0, cmpl $0, -4(%rbp).
template< binary_op_t op, view_t V, typename R >
concept zero_compared = comparison_operator< op > &&
                        (op == binary_op_t::eq || op == binary_op_t::ne) &&
                        std::same_as< R, zero_t< V > >;

template< binary_op_t op, view_t V, typename L, typename R >
    requires zero_compared< op, V, R >
void
branch(state_t &s, const binary_t< op, V, L, R > &b, int label, bool sense)
{
    value_branch(s, b.lhs, label, sense != (op == binary_op_t::eq));
}

// A load against an immediate is the cmp of the object in place, as
// for its value, cmpl $5, -4(%rbp), and no register is held.
template< binary_op_t op, view_t V, immediate I, typename W, sign_t S >
    requires comparison_operator< op > && (!zero_compared< op, V, I >)
void
branch(state_t &s,
       const binary_t< op, V, load_t< W, V, S >, I > &b,
       int label,
       bool sense)
{
    compare(s, address(s, b.lhs.where), immediate_of(b.rhs), V);
    jump_if(s, sense ? op : inverse(op), label);
}

// A constant on the left is the jump of the node mirrored, ordered():
// if (5 < a) is cmpl $5, -4(%rbp) and jle.
template< binary_op_t op, view_t V, immediate I, typename R >
    requires comparison_operator< op >
void
branch(state_t &s, const binary_t< op, V, I, R > &b, int label, bool sense)
{
    branch(s, binary_t< mirrored(op), V, R, I > { b.rhs, b.lhs }, label, sense);
}

// The jump on a value compared with 0, rcc's test_zero(), 7.1: the
// value held, then the jump on its register at the value's view, cmpl
// $0, %eax and je, and the value released.  A constant is no special
// case: if (1) loads 1 and tests it.
template< typename T >
void
value_branch(state_t &s, const T &t, int label, bool sense)
{
    const value_t v = value(s, t);
    test_zero(s, v, view_of(t), label, sense);
    release(s, v);
}

// A load's value compared with 0 is the cmp of the object in place at
// its view, rcc's condition_on(), cmpl $0, -4(%rbp), cmpq $0, -8(%rbp),
// then jne or je: if (a) and a == 0 alike.  A cast to its own type over
// a load is the object read as it is, so if ((int) i) is the same cmp.
void in_place_branch(
    state_t &, const address_t &at, view_t view, int label, bool sense);

template< typename W, view_t V, sign_t S >
void
value_branch(state_t &s, const load_t< W, V, S > &l, int label, bool sense)
{
    in_place_branch(s, address(s, l.where), V, label, sense);
}

template< view_t V, typename W, sign_t S >
void
value_branch(state_t &s,
             const trunc_t< V, load_t< W, V, S > > &t,
             int label,
             bool sense)
{
    in_place_branch(s, address(s, t.arg.where), V, label, sense);
}

// Whether the instruction of op sets ZF from the value it computes,
// rcc's flag_operator(): add, sub, and, or and xor, by Intel's
// reference (7.1); imul, the shifts and idiv do not.
constexpr bool
flag_operator(binary_op_t op)
{
    return op == binary_op_t::add || op == binary_op_t::sub ||
           op == binary_op_t::bit_and || op == binary_op_t::bit_or ||
           op == binary_op_t::bit_xor;
}

// rcc's flag_branch() on the held value v of a flag condition of the
// view: je or jne on the flags of the row's own instruction, with the
// cmp with 0 first where the record says the last instruction set no
// ZF from v at that width; v is released.
void flag_branch(state_t &, value_t v, view_t view, int label, bool sense);

// The niches of a flag condition, rcc's flag_condition(), 7.1, after
// the Seventh Edition compiler's cctab: + - | ^ and unary -, whose
// row's instruction sets ZF from the value, its value's row, then the
// jump on those flags, flag_branch(): addl -8(%rbp), %eax and je,
// negl %eax and jne.  & is test, below.
template< binary_op_t op, view_t V, typename L, typename R >
    requires(flag_operator(op))
void
value_branch(state_t &s,
             const binary_t< op, V, L, R > &b,
             int label,
             bool sense)
{
    flag_branch(s, value(s, b), V, label, sense);
}

template< view_t V, typename A >
void
value_branch(state_t &s,
             const unary_t< unary_op_t::neg, V, A > &u,
             int label,
             bool sense)
{
    flag_branch(s, value(s, u), V, label, sense);
}

// The jump on the flags a memory form just written set, rcc's modified()
// and flag_branch(): je or jne, and no cmp, since the form's instruction
// set ZF from the object's new value; one that set none stops the
// generation, rcc's guard.
void modified_branch(state_t &, view_t view, int label, bool sense);

// + - & | ^= as a jump, rcc's flag_condition() of an assign_t: its
// effect, the memory form, then the jump on its flags, subl %eax,
// -4(%rbp) and jne for (a -= b) == 0.  The others take the primary,
// their value compared with 0.
template< binary_op_t op, typename W, view_t V, view_t O, typename R >
    requires(flag_operator(op))
void
value_branch(state_t &s,
             const compound_t< op, W, V, O, R > &c,
             int label,
             bool sense)
{
    effect(s, c);
    modified_branch(s, V, label, sense);
}

// A prefix step as a jump, rcc's flag_condition() of one: its effect,
// the memory form, then the jump on its flags, addl $1, -4(%rbp) and
// je.  A postfix step's is the primary, its old value compared with 0.
template< step_op_t op, typename W, view_t V, sign_t S >
    requires(!is_postfix(op))
void
value_branch(state_t &s, const step_t< op, W, V, S > &t, int label, bool sense)
{
    effect(s, t);
    modified_branch(s, V, label, sense);
}

// test of a source and the held value l at the view, rcc's test(),
// which sets ZF from their & and writes no register: a held value r,
// testl %ecx, %eax, the registers of operands(), a spilled value
// reloaded, and r released; an operand as its text, an immediate, testl
// $1, %eax, or an object in memory, testl -8(%rbp), %eax; and of an
// immediate and the object at an address, testl $1, -4(%rbp), the
// object tested in place and the address consumed.  l stays held.
void test(state_t &, value_t l, value_t r, view_t view);
void test(state_t &, value_t l, std::string_view source, view_t view);
void
test(state_t &, const address_t &at, std::string_view immediate, view_t view);

// The test of the held left value l and the right operand of & at the
// view V, by the right operand's class, as compare_with() takes a
// comparison's.
template< view_t V, typename R >
void
test_with(state_t &s, value_t l, const R &rhs)
{
    test(s, l, value(s, rhs), V);
}

template< view_t V, typename W, sign_t S >
void
test_with(state_t &s, value_t l, const load_t< W, V, S > &rhs)
{
    const address_t at = address(s, rhs.where);
    const std::string source = operands(s, l, at).second;
    test(s, l, source, V);
    release(s, at);
}

template< view_t V, immediate I >
void
test_with(state_t &s, value_t l, const I &rhs)
{
    test(s, l, immediate_of(rhs), V);
}

// A comma, binary_operand()'s: its left operand for its effect, then its
// right one in its class, testl -12(%rbp), %eax for a & (b, c).
template< view_t V, typename R >
void
test_with(state_t &s, value_t l, const comma_t< R > &rhs)
{
    effect(s, rhs.lhs);
    test_with< V >(s, l, rhs.rhs);
}

// & as a jump, rcc's test(), 7.1, the PDP-11's bit of cctab's cc81: the
// left operand's value, generated outside the registers the right
// operand's top operator pins, then test R, L, which sets ZF and writes
// no result, so nothing is held; then je or jne.
template< view_t V, typename L, typename R >
void
value_branch(state_t &s,
             const binary_t< binary_op_t::bit_and, V, L, R > &b,
             int label,
             bool sense)
{
    const value_t l = rvalue_avoiding(s, b.lhs, pinned_by(b.rhs));
    test_with< V >(s, l, b.rhs);
    release(s, l);

    jump_if(s, sense ? binary_op_t::ne : binary_op_t::eq, label);
}

// A load of the operation's view and an immediate are tested in place,
// testl $1, -4(%rbp).
template< view_t V, immediate I, typename W, sign_t S >
void
value_branch(state_t &s,
             const binary_t< binary_op_t::bit_and, V, load_t< W, V, S >, I > &b,
             int label,
             bool sense)
{
    test(s, address(s, b.lhs.where), immediate_of(b.rhs), V);
    jump_if(s, sense ? binary_op_t::ne : binary_op_t::eq, label);
}

// A constant on the left is the test of the node with its operands
// exchanged, ordered(): 1 & a is testl $1, -4(%rbp).
template< view_t V, immediate I, typename R >
void
value_branch(state_t &s,
             const binary_t< binary_op_t::bit_and, V, I, R > &b,
             int label,
             bool sense)
{
    value_branch(s, binary_t< binary_op_t::bit_and, V, R, I > { b.rhs, b.lhs },
                 label, sense);
}

// The extension of the held value v of the view from to the view to by
// the sign of its type, rcc's extended() past its test of the width
// rule, which the widening's row asks: movslq %eax, %rax or movl %eax,
// %eax on v's own register, or into the register the consumer names,
// wanted, where it is free and v is elsewhere, movslq %edx, %rax for an
// int remainder returned as a long (6.1).
value_t extended(
    state_t &, value_t v, view_t from, sign_t sign, view_t to, unsigned wanted);

// The extending load, rcc's extending_load(), backend.md 6.3: the object
// of the view from at an address, loaded extended by its sign to 64
// bits, movslq -4(%rbp), %rax, movslq g(%rip), %rax, as load() takes
// the address.  It reads the bytes the load at its own view reads, at
// the object's width (G10), and leaves the value valid to 64.
value_t extending_load(state_t &, const address_t &, view_t from);

// A signed object widened to 64 bits is its extending load, the niche
// of the conversion over a load (docs/typed-tree.md 4.2).  The rule is
// the Seventh Edition compiler's: match(), in c10.c, tries a row again
// with the conversion dropped, and its tables convert an operand in
// memory in the instruction that reads it.
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
// call() and rvalue(), chapter 8: the call keeps its own pin, since its
// result is in rax by the convention, so it is generated outside any
// reservation around it, as a division is, and its arguments find no
// register left for last; its value is held in rax, where the call
// leaves it, valid to V's width and no further (valid_of()): the
// convention leaves the bits above a narrow one unspecified, and the
// widening's primary extends it where it is used wider.
template< view_t V >
value_t
value(state_t &s, const call_t< V > &c, unsigned = 0)
{
    const unsigned outer = std::exchange(s.reserved, 0u);

    call(s, c.name, c.args);
    const value_t v = hold(s, rax);

    s.reserved = outer;

    return v;
}

// The value of c ? y : n of the view, rcc's conditional(), backend.md
// 6.1: no value held across the paths, before_paths(); the two labels
// taken before the condition, then its jump to the second arm where it
// is 0; the first arm handed the register the consumer names, wanted,
// through arm(), where it is free, and as any value otherwise; the join
// the register the consumer names, or the one the first arm's row left
// its value in, read once that arm is written, a mov repairing a first
// arm that did not take the name; the jump past the second arm; the
// second arm handed the join through arm(), and moved there where its
// row left it elsewhere; the value held in the join.
value_t conditional(state_t &,
                    const expr_t &cond,
                    const expr_t &yes,
                    const expr_t &no,
                    view_t view,
                    sign_t sign,
                    unsigned wanted);

// Whether a signed widening of the conditional of the arms yes and no
// is handed to them, rcc's widens_arms() past what the node's type
// says: where an arm widens free, its arm_widening(), or is a remainder
// and the consumer names a register other than rdx, wanted_register(),
// which the remainder is extended into from rdx where the join's mov
// would be, movslq %edx, %rax for a return.
bool widens_arms(const state_t &,
                 const expr_t &yes,
                 const expr_t &no,
                 unsigned wanted);

template< view_t V, bool jumps >
value_t
value(state_t &s, const conditional_t< V, jumps > &c, unsigned wanted = 0)
{
    return conditional(s, c.cond, c.yes, c.no, V, c.sign, wanted);
}

// The pieces of a switch's dispatch, backend.md 7.3, which the
// dispatches of rows-statements.hh are written over, as rcc's
// direct_switch(), simple_switch() and hash_switch() are, each at the
// view of the controlling expression's promoted type.

// A case's test, rcc's equal_jump(): the cmp of the held value v and the
// case's value at the view, cmpl $5, %eax, cmpq $5, %rax, a value no
// 32-bit immediate holds in a register of its own, movabsq
// $4886718345, %rcx and cmpq %rcx, %rax; then je to its label; v stays
// held.  The Seventh Edition compiler tested a 0 where it compared
// another value; this machine's test of zero is the same row, cmpl $0,
// %eax (7.1).
void equal_jump(state_t &, value_t v, long long value, view_t view, int label);

// The first value of a direct switch subtracted from the held value v
// at the view, rcc's arithmetic() of the sub row, by its immediate,
// subl $1, %eax, in v's register, or by a value no 32-bit immediate
// holds in a register of its own, movabsq $4294967296, %rcx and subq
// %rcx, %rax; the value is v's.
value_t subtracted(state_t &, value_t v, long long first, view_t view);

// The jump through a switch's table by a held index of the view, rcc's
// table_jump(): the index extended to 64 bits as an unsigned value,
// movl %eax, %eax, where valid, the width its register is valid to,
// says that no write of an unsigned value at 32 bits left it so, rcc's
// extended(); the table's address PC-relative, leaq .Lf.8(%rip), %rcx,
// in the lowest free register; and the jump through its entry, jmp
// *(%rcx,%rax,8), after which no path reaches what is written next.
// The index is consumed.
void table_jump(state_t &, value_t index, int valid, view_t view, int table);

// The division of a hash switch, rcc's divided(): the unsigned division
// row at the view by the table's size, held in a register as the
// divisor of x / 7 is, outside rax and rdx, movl $3, %ecx, xorl %edx,
// %edx and divl %ecx, or movq $3, %rcx and divq %rcx; both results
// kept, the quotient in rax and the remainder in rdx, as the
// instruction leaves them.  v is consumed.
std::pair< value_t, value_t >
divided(state_t &, value_t v, long long size, view_t view);

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

} // namespace rcc::amd64
