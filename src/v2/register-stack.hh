// -*- mode: c++; -*-

// The register stack of docs/backend.md 6.7, in rcc's vocabulary:
// which expression register holds which value, a value taken in a
// register and released, the spill and the reload.  Its text is the
// same for both targets, so it is templates over a target's state,
// which holds the record, and each target defines take(), the
// register a new value is taken in, which spills when every one is
// held; allocate(), the next slot of the frame; and the two
// instructions the stack writes, store_spill(), the store of a
// register to a spill slot, and reload_spill(), the reload from one,
// all 8 bytes.  A value is in a register, the one it was taken in but
// where x86-64 moves it out of the way of an instruction that pins a
// register, or in its spill slot from its spill to its reload, and
// the record follows it.  Before the paths of && or || part, every
// value held in a register is spilled, and every value held is marked
// until the paths meet, so that a move of one inside them stops the
// generation.

#pragma once

#include "operators.hh"

#include <cstddef>
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
// whose slot's displacement is spill; whether it is held still, live,
// until it is consumed; across, the count of the branches around the
// row now generated that it is held across, incoming_t's; and for the
// join of ?:, joined, the width it is valid to at the conditional's
// sign, which the moves into the join decide at generation and no
// summary knows (docs/typed-tree.md 4.2), 0 for any other value.
template< typename R >
struct held_t
{
    std::optional< R > reg;
    int spill = 0;
    bool live = true;
    int across = 0;
    int joined = 0;
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

// A value held across the paths of && or || is where both paths expect
// it until they meet, backend.md 6.7: a spill, a reload or a move of
// one inside them would leave the paths with two register maps, and
// the listing would assemble and be wrong, so generation stops at it
// instead, rcc's check_movable().  It is the generator's check on
// itself, as check_none_held() is.
template< typename S >
void
check_movable(const S &s, value_t v)
{
    if (s.registers.values[v.id].across > 0)
        throw std::logic_error("v2: a value held across a branch was "
                               "moved inside it");
}

// Stores the value in r to the next spill slot, all 8 bytes, and frees
// the register, backend.md 6.2.  The slot is the frame's next, and it
// is never given back, so a later declaration lands past it.
template< typename S, typename R >
void
spill(S &s, R r)
{
    const int id = s.registers.owner.at(r);
    check_movable(s, value_t { id });

    const int displacement = allocate(s, 8, 8);

    store_spill(s, r, displacement);

    s.registers.values[id].reg = std::nullopt;
    s.registers.values[id].spill = displacement;
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

    check_movable(s, v);

    const auto r = take(s, avoid);
    reload_spill(s, r, s.registers.values[v.id].spill);

    s.registers.values[v.id].reg = r;
    s.registers.owner.emplace(r, v.id);

    return r;
}

// The width the held value v of the node e is valid to at the sign,
// the width rule (zoo.hh): what the join of ?: recorded where e is that
// join, is_join()'s, the conditional itself or a comma or an assignment
// whose value it is, held in the register the join left it in, and the
// node's summary, valid_of(), otherwise.  rcc keeps the width with
// every held value; the tree asks the subtree, but for the one fact a
// subtree does not hold, which arm a mov brought into a join.
template< typename S, typename E >
int
held_valid(const S &s, value_t v, const E &e, sign_t sign)
{
    const int joined = s.registers.values[v.id].joined;

    return joined && is_join(e) ? joined : valid_of(e, sign);
}

// The value is consumed: its register, if it has one, is free.
template< typename S >
void
release(S &s, value_t v)
{
    if (const auto r = s.registers.values[v.id].reg)
        s.registers.owner.erase(*r);

    s.registers.values[v.id].live = false;
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

// Spills every value held in a register, rcc's spill_all(), in the
// order of the registers and not of the values' ages: the record's map
// is ordered by the register's number, which is the order take() gives
// them on both targets.
template< typename S >
void
spill_all(S &s)
{
    while (!s.registers.owner.empty())
        spill(s, s.registers.owner.begin()->first);
}

// The values held where the paths of a branch part, each marked held
// across the branch for as long as this lives, rcc's incoming_t: its
// scope ends where the paths meet, and the destructor takes the mark
// back, so no return and no exception leaves a value marked.  A branch
// nested in a path marks the outer branch's values once more, and its
// end takes back its own mark alone.
template< typename S >
class [[nodiscard]] incoming_t
{
public:

    explicit incoming_t(S &s) : m_s(s)
    {
        for (std::size_t id = 0; id < s.registers.values.size(); ++id) {
            if (!s.registers.values[id].live)
                continue;

            ++s.registers.values[id].across;
            m_ids.push_back(static_cast< int >(id));
        }
    }

    ~incoming_t()
    {
        for (const int id : m_ids)
            --m_s.registers.values[id].across;
    }

    incoming_t(const incoming_t &) = delete;
    incoming_t &operator=(const incoming_t &) = delete;

private:

    S &m_s;
    std::vector< int > m_ids { };
};

// rcc's before_paths(), docs/backend.md 6.7: before the two paths of
// && or || every value held in a register is spilled, so that each has
// one location on both paths, and each value held is marked held
// across them until they meet, where the caller's incoming_t ends.  A
// value already spilled stays in its slot.
template< typename S >
incoming_t< S >
before_paths(S &s)
{
    spill_all(s);

    return incoming_t< S >(s);
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
