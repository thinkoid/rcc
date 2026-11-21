// -*- mode: c++; -*-

// The register stack of backend.md 6.7: which expression register holds
// which value, the spill and the reload, the borrowed read of a register
// variable and the variable's register as a row's destination
// (chapter 4), and the values held where the paths of a branch diverge.
// Its text is the same for every target, but it names each target's
// registers and state, so this is not a header in the ordinary sense: it
// is part of a generator's text, kept in one place.  c1-x86-64.cc and
// c1-aarch64.cc each include it once, inside their unnamed namespace,
// and so each compiles its own copy against its own reg_t and state_t.
// It includes nothing, since it is read inside a namespace; the
// generator's own includes serve it.
//
// Before the include the generator declares reg_t, expression_registers,
// variable_registers, and state_t with owner, values, age and live;
// allocate(), the next slot of the frame; take(), a register for a
// value, which is defined after the include, since it spills; and the
// three instructions the stack writes: store_spill(), the store of a
// register to a spill slot, all 8 bytes; reload_spill(), the reload from
// one, all 8 bytes; mov(), a register copied into another at a width;
// and two rows one_path() writes, cbranch(), the jump code, and
// arm_into(), an arm of ?: into a register variable, each defined after
// the include.  held_t and value_t are c1-common.hh's.

unsigned
bit(reg_t r)
{
    return 1u << r;
}

// A value held across a branch is where both paths expect it until they
// meet; an instruction on one path that moves it leaves the paths with
// two register maps (backend.md 6.7).  Such a move means a value was
// held where the generator did not save it, as when the walk of writes()
// missed a write, and the listing would assemble and be wrong, so the
// generator stops at the instruction instead.
void
check_movable(const state_t &s, int id)
{
    if (s.values[id].across > 0)
        throw std::logic_error(
            "c1: a value held across a branch was moved inside it");
}

// Stores the value in r to the next spill slot, all 8 bytes, and frees
// the register, backend.md 6.2.
void
spill(state_t &s, reg_t r)
{
    const int id = s.owner[r];
    check_movable(s, id);

    const int displacement = allocate(s, 8, 8);

    store_spill(s, r, displacement);

    s.values[id].reg = -1;
    s.values[id].spill = displacement;
    s.owner[r] = -1;
}

void
spill_all(state_t &s)
{
    for (const reg_t r : expression_registers)
        if (s.owner[r] >= 0)
            spill(s, r);
}

value_t
hold(state_t &s, reg_t r, int valid)
{
    const int id = static_cast< int >(s.values.size());

    s.values.push_back({ r, 0, s.age++, true, valid, nullptr });
    s.owner[r] = id;
    ++s.live;

    return { id };
}

// A new value in a register taken for it, valid to the width the
// instruction about to write it leaves.
value_t
fresh(state_t &s, int valid, unsigned avoid = 0)
{
    return hold(s, take(s, avoid), valid);
}

// The register a value is in; a spilled value is reloaded first, all 8
// bytes, into a free register outside avoid.
reg_t
reg_of(state_t &s, value_t v, unsigned avoid = 0)
{
    if (v.id < 0)
        throw std::logic_error("c1: a void expression used as a value");

    if (s.values[v.id].reg >= 0)
        return static_cast< reg_t >(s.values[v.id].reg);

    check_movable(s, v.id);

    const reg_t r = take(s, avoid);
    reload_spill(s, r, s.values[v.id].spill);

    s.values[v.id].reg = r;
    s.owner[r] = v.id;

    return r;
}

// The value is consumed: its register, if it has one and owns it, is
// free.  A borrowed read's register is the variable's, which has no
// owner entry.
void
release(state_t &s, value_t v)
{
    if (v.id < 0)
        return;

    auto &h = s.values[v.id];
    if (h.reg >= 0 && !h.variable)
        s.owner[h.reg] = -1;

    h.reg = -1;
    h.live = false;
    --s.live;
}

// The registers of a binary node's two values, either reloaded if it
// was spilled, neither reload taking the other's register.
std::pair< reg_t, reg_t >
operands(state_t &s, value_t l, value_t r)
{
    const int held = s.values[l.id].reg;
    const reg_t right =
        reg_of(s, r, held >= 0 ? bit(static_cast< reg_t >(held)) : 0);
    const reg_t left = reg_of(s, l, bit(right));

    return { left, right };
}

// register variables

// Whether a value is a register variable's read, a borrowed one,
// backend.md 6.7.
bool
borrowed(const state_t &s, value_t v)
{
    return v.id >= 0 && s.values[v.id].variable != nullptr;
}

// The register a register variable lives in, from its declaration to
// the end of its block: the one of variable_registers the parser's
// index names, backend.md chapter 4.
reg_t
variable_register(const symbol_t *sym)
{
    return variable_registers[sym->register_index];
}

// The width a register variable's register is written at: 64 bits for a
// long or a pointer, 32 for the rest.
int
register_width(const symbol_t *sym)
{
    return std::max(32, integer(sym->type).width);
}

// The width a register variable's register is valid to, its
// representation, backend.md chapter 4: the value extended by its
// signedness to 32 bits, which the 32-bit write extends to 64 for an
// unsigned; a long or a pointer whole.
int
represented(const symbol_t *sym)
{
    return written(register_width(sym), integer(sym->type).is_signed);
}

// A read of a register variable: its register as an operand, and no
// instruction, backend.md 6.7.  The register holds the value as
// establish() left it, which is what the read is valid to.
value_t
read(state_t &s, const symbol_t *sym)
{
    const int id = static_cast< int >(s.values.size());

    s.values.push_back(
        { variable_register(sym), 0, s.age++, true, represented(sym), sym });
    ++s.live;

    return { id };
}

// A register variable's read under conversions that write nothing, and
// the width the read is valid to after them (backend.md 6.3, the width
// rule): a narrowing leaves it valid to its width, and a widening
// writes nothing where the read is valid to its width already, so
// (int) c on a register char is c's register, valid to 32 bits.  The
// variable is null for any other e, a read under a widening that
// extends among them.
struct register_read_t
{
    const symbol_t *variable = nullptr;
    int valid = 0;
};

register_read_t
register_read(const expr_t &e)
{
    if (const auto *sym = register_variable(e))
        return { sym, represented(sym) };

    if (!is< cast_t >(e))
        return { };

    const auto &from = as< cast_t >(e).arg->type;
    if (!(is< int_t >(from) || is< pointer_t >(from)) ||
        !(is< int_t >(e.type) || is< pointer_t >(e.type)))
        return { };

    const auto inner = register_read(*as< cast_t >(e).arg);
    const int to = integer(e.type).width;
    if (!inner.variable || (to > integer(from).width && inner.valid < to))
        return { };

    return { inner.variable, std::min(inner.valid, to) };
}

// Whether an arm of ?: is the read of the register variable its paths
// write, under conversions that write nothing, register_read()'s: the
// register holds the arm's value already, so its path writes nothing,
// move_in()'s (chapter 4).
bool
own_read(const expr_t &arm, const symbol_t *variable)
{
    return register_read(arm).variable == variable;
}

// The type the row handed into computes the value in: the variable's,
// or into's as.
const type_t &
computed_type(const into_t &into)
{
    return into.as ? *into.as : into.variable->type;
}

// The width a row handed into must leave its result valid to, in the
// signedness of computed_type(): the variable's representation, or for
// a row of a type that differs from the variable's in the signedness
// alone, that type's at the variable's width.  Such a row's last
// instruction writes the register at that width, so the bits are the
// variable's representation too: an int row's 32-bit write leaves an
// unsigned variable valid to 64, the upper half cleared (backend.md
// chapter 4).
int
needed(const into_t &into)
{
    const auto &t = integer(computed_type(into));

    return written(register_width(into.variable), t.is_signed);
}

// Whether a row handed into writes the variable's register, its last
// instruction leaving the result valid to valid: when no row has, and
// the register is then the variable's representation, backend.md
// chapter 4.
bool
takes(const into_t *into, int valid)
{
    return into && !into->taken && valid >= needed(*into);
}

// The value of a row whose last instruction wrote the variable's
// register in place of v's: v is consumed, and the value is the
// variable's read, as after any write of it.
value_t
taken(state_t &s, into_t &into, value_t v = { })
{
    release(s, v);
    into.taken = true;

    return read(s, into.variable);
}

// A conditional into a register variable an arm of which is the
// variable's own read, own_read()'s, whose path writes nothing, chapter
// 4: the condition's jump goes to the join past the other path, the one
// path the other arm into the variable, arm_into()'s, its row's last
// instruction writing the register or move_in()'s mov after it, movl
// %eax, %ebx on x86-64 and mov w19, w0 on aarch64 after a call.  Where
// the first arm is the read, the jump is taken when the condition
// holds, cbranch()'s sense inverted, jne .L3 or cbnz w0, .L3 for r = c
// ? r : x; where the second is, when it fails, je .L3 or cbz w0, .L3
// for r = c ? x : r.  A path that writes nothing has no jump over it,
// and no label where no jump names one (7.2); r = c ? r : r is the
// condition's jump alone.  The value is the variable's read.
value_t
one_path(state_t &s, const conditional_t &c, into_t &into, int end)
{
    const bool first = own_read(*c.yes, into.variable);

    cbranch(s, *c.cond, end, first);
    arm_into(s, first ? *c.no : *c.yes, into);

    place_label(s, end);

    return taken(s, into);
}

// What a conversion to t hands on to the row of its operand.  The
// conversion leaves the value valid to t's width at most (backend.md
// 6.3, the width rule), so the operand's row leaves the variable's
// representation only where that width holds it.  A row computing the
// value in another signedness, into's as, hands it on through a
// conversion that widens alone, u = c + b on a char c being movsbl
// -9(%rbp), %ebx; addl -16(%rbp), %ebx: under a narrowing one, the last
// instruction would write the register wider than the variable.
into_t *
through(into_t *into, const int_t &t, bool widens)
{
    if (!into || (into->as && !widens))
        return nullptr;

    return t.width >= needed(*into) ? into : nullptr;
}

// Copies a borrowed read into the free expression register to, by mov at
// its variable's register width; the value is then to's, owned, valid
// to what it was, since the mov carries every bit the read was valid to.
void
copy(state_t &s, value_t v, reg_t to)
{
    check_movable(s, v.id);

    auto &h = s.values[v.id];
    const int width = register_width(h.variable);

    mov(s, static_cast< reg_t >(h.reg), to, width);

    h.reg = to;
    h.variable = nullptr;
    s.owner[to] = v.id;
}

// Saves a held value to a spill slot, whichever kind it is, backend.md
// 6.7: an owned value in a register is spilled, and a borrowed read has
// its variable's register stored, all 8 bytes, with no copy.  The read
// is then owned and spilled, no longer the variable's, so a later write
// of the variable leaves it alone; valid holds, since the reload is all
// 8 bytes too.  An owned value already spilled is saved as it is.
void
save(state_t &s, value_t v)
{
    auto &h = s.values[v.id];
    if (h.reg < 0)
        return;

    const reg_t r = static_cast< reg_t >(h.reg);
    if (!h.variable) {
        spill(s, r);
        return;
    }

    check_movable(s, v.id);

    const int displacement = allocate(s, 8, 8);

    store_spill(s, r, displacement);

    h.reg = -1;
    h.spill = displacement;
    h.variable = nullptr;
}

// The values held where the paths of a branch diverge, each marked held
// across the branch for as long as this lives.  Its scope ends where the
// paths meet, and the destructor unmarks them, so no return and no
// exception leaves a value marked.  A branch nested in a path marks the
// outer branch's values once more, and its end takes back its own mark
// alone.
class [[nodiscard]] incoming_t
{
public:

    explicit incoming_t(state_t &s) : m_s(s)
    {
        for (std::size_t id = 0; id < s.values.size(); ++id) {
            if (!s.values[id].live)
                continue;

            ++s.values[id].across;
            m_ids.push_back(static_cast< int >(id));
        }
    }

    ~incoming_t()
    {
        for (const int id : m_ids)
            --m_s.values[id].across;
    }

    incoming_t(const incoming_t &) = delete;
    incoming_t &operator=(const incoming_t &) = delete;

private:

    state_t &m_s;
    std::vector< int > m_ids { };
};

// Before the two paths of &&, || or ?:, a borrowed read that e writes is
// saved, and every owned value is spilled, as for a call; a read e does
// not write stays in its variable's register.  Each then has one
// location on both paths: backend.md 6.7.  preserve() on one path alone
// would leave the paths with two register maps, so each value is then
// marked held across the paths until they meet, where the caller's
// incoming_t ends, and a copy, a spill or a reload of one inside them is
// an error of the generator, not a wrong listing.  A statement
// expression's statements, which may branch, loop and call, are taken
// as such paths, from their first to the expression's value.
incoming_t
before_paths(state_t &s, const expr_t &e)
{
    for (std::size_t id = 0; id < s.values.size(); ++id) {
        const auto &h = s.values[id];
        if (h.live && h.variable && writes(e, h.variable))
            save(s, { static_cast< int >(id) });
    }

    spill_all(s);

    return incoming_t { s };
}
