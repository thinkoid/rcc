// -*- mode: c++; -*-

// The register stack of docs/backend.md 6.7, in rcc's vocabulary: which
// expression register holds which value, a value taken in a register,
// or held in one a row names, and released.  Its text is the same for
// both targets, so it is templates over a target's state, which holds
// the record as registers, and each target defines take(), the
// register a new value is taken in.  There is no spill yet: a value
// stays in the register it was taken in until it is released.

#pragma once

#include <map>
#include <stdexcept>
#include <vector>

namespace rcc {

// A handle on a held value: its number among the values of the
// function.
struct value_t
{
    int id;
};

// The state's record: the value each held register holds, by the
// value's number, a register absent from it being free; and the
// register each value was taken in, by the value's number.
template< typename R >
struct registers_t
{
    std::map< R, int > owner { };
    std::vector< R > values { };
};

// A new value in the register r, which is free: a value whose register
// a row names, as a return names the result register, so that nothing
// it generates there takes r.
template< typename S, typename R >
value_t
hold(S &s, R r)
{
    const int id = static_cast< int >(s.registers.values.size());

    s.registers.values.push_back(r);
    s.registers.owner.emplace(r, id);

    return { id };
}

// A new value in a register taken for it.
template< typename S >
value_t
fresh(S &s)
{
    return hold(s, take(s));
}

// The register a value is in.
template< typename S >
auto
reg_of(const S &s, value_t v)
{
    return s.registers.values[v.id];
}

// The value is consumed: its register is free.
template< typename S >
void
release(S &s, value_t v)
{
    s.registers.owner.erase(s.registers.values[v.id]);
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
