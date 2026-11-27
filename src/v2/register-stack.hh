// -*- mode: c++; -*-

// The register stack of docs/backend.md 6.7, in rcc's vocabulary: which
// expression register holds which value, a value taken in a register
// and released, the spill and the reload.  Its text is the same for
// both targets, so it is templates over a target's state, which holds
// the record, and each target defines take(), the register a new value
// is taken in, which spills when every one is held; allocate(), the
// next slot of the frame; and the two instructions the stack writes,
// store_spill(), the store of a register to a spill slot, and
// reload_spill(), the reload from one, all 8 bytes.  A value is in a
// register, the one it was taken in but where x86-64 moves it out of
// the way of an instruction that pins a register, or in its spill slot
// from its spill to its reload, and the record follows it.

#pragma once

#include <map>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rcc {

// A handle on a held value: its number among the values of the
// function, which is its age too, since values are numbered in the
// order they are taken: the lower, the older.
struct value_t
{
    int id;
};

// Where a held value is: its register, or none for a value spilled,
// whose slot's displacement is spill.
template< typename R >
struct held_t
{
    std::optional< R > reg;
    int spill = 0;
};

// The state's record: the value each held register holds, by the
// value's number, a register absent from it being free; and where each
// value is, by the value's number.
template< typename R >
struct registers_t
{
    std::map< R, int > owner { };
    std::vector< held_t< R > > values { };
};

// Stores the value in r to the next spill slot, all 8 bytes, and frees
// the register, backend.md 6.2.  The slot is the frame's next, and it
// is never given back, so a later declaration lands past it.
template< typename S, typename R >
void
spill(S &s, R r)
{
    const int id = s.registers.owner.at(r);
    const int displacement = allocate(s, 8, 8);

    store_spill(s, r, displacement);

    s.registers.values[id] = { .reg = std::nullopt, .spill = displacement };
    s.registers.owner.erase(r);
}

// A new value in the free register r, where the instruction a row has
// written left it: on x86-64 a remainder in rdx, and a quotient that
// stays in rax, where idivl writes them.
template< typename S, typename R >
value_t
hold(S &s, R r)
{
    const int id = static_cast< int >(s.registers.values.size());

    s.registers.values.push_back({ .reg = r });
    s.registers.owner.emplace(r, id);

    return { id };
}

// A new value in a register taken for it, which the row that took it
// writes and returns held (docs/typed-tree.md 3.4).
template< typename S >
value_t
fresh(S &s)
{
    return hold(s, take(s));
}

// The register a value is in; a spilled value is reloaded first, all 8
// bytes, into a register take() gives outside avoid.  So a row that asks
// for two values' registers writes the reloads in the order it asks.
template< typename S >
auto
reg_of(S &s, value_t v, unsigned avoid = 0)
{
    if (const auto r = s.registers.values[v.id].reg)
        return *r;

    const auto r = take(s, avoid);
    reload_spill(s, r, s.registers.values[v.id].spill);

    s.registers.values[v.id].reg = r;
    s.registers.owner.emplace(r, v.id);

    return r;
}

// The value is consumed: its register, if it has one, is free.
template< typename S >
void
release(S &s, value_t v)
{
    if (const auto r = s.registers.values[v.id].reg)
        s.registers.owner.erase(*r);
}

// The registers of a binary node's two values, either reloaded if it
// was spilled, neither reload taking the other's register: the right
// operand's first, then the left's.
template< typename S >
auto
operands(S &s, value_t l, value_t r)
{
    const auto held = s.registers.values[l.id].reg;
    const auto right = reg_of(s, r, held ? bit(*held) : 0u);
    const auto left = reg_of(s, l, bit(right));

    return std::pair { left, right };
}

// Every value a statement holds is consumed by its end; nothing is held
// from one statement to the next.  A value left held means a row took a
// register and did not give it back, and the next statement's take
// would pass it over, so generation stops.
template< typename S >
void
check_none_held(const S &s)
{
    if (!s.registers.owner.empty())
        throw std::logic_error("v2: a value held across a statement");
}

} // namespace rcc
