// -*- mode: c++; -*-

// The statements of backend.md 7.2 and aarch64.md 7.2: each a shape of
// labels and jumps around the generator's own functions, with no
// instruction of its own.  Its text is the same for every target, but
// it names each target's state and calls each target's functions, so
// this is not a header in the ordinary sense: it is part of a
// generator's text, kept in one place, as c1-stack.hh is.  c1-x86-64.cc
// and c1-aarch64.cc each include it once, inside their unnamed
// namespace, after the expressions, and so each compiles its own copy
// against its own state_t and functions.  It includes nothing, since it
// is read inside a namespace; the generator's own includes serve it.
//
// Before the include the generator declares reg_t; state_t with body,
// live and ret; loop_alignment, the power of two of .p2align before a
// loop's head, 0 for none; result_register, the register the convention
// returns a result in; new_label(), place_label() and jump(); the three
// entry points, rvalue(), cbranch() and effect(); and decl_stmt(), a
// declaration, which is the generator's own, since the store row of its
// initializer takes the target's form of the slot's address.  It
// includes c1-stack.hh first, whose reg_of() and release() the
// statements call, with mov(), the stack's copy of a register into
// another, for the result.  integer() is c1-common.hh's, and the tree
// ast.hh's.

void statement(state_t &, const stmt_t &);

void
expr_stmt(state_t &s, const expr_stmt_t &x)
{
    if (x.e)
        effect(s, *x.e);
}

void
block_stmt(state_t &s, const block_t &b)
{
    for (const auto &st : b.body)
        statement(s, st);
}

// backend.md 7.2 and aarch64.md 7.2: the condition's false exit to the
// else part, or past the statement without one.
void
if_stmt(state_t &s, const if_t &i)
{
    const int no = new_label(s);
    cbranch(s, i.cond, no, false);

    statement(s, *i.yes);

    if (!i.no) {
        place_label(s, no);
        return;
    }

    const int end = new_label(s);
    jump(s, end);

    place_label(s, no);
    statement(s, *i.no);

    place_label(s, end);
}

// The directive before a loop's head, by the target's loop_alignment,
// and nothing where that is 0.  The directive is a convention of the
// target, as the stack's 16 bytes at a call are, not knowledge of the
// program.  With no fill byte named, the assembler fills the gap in a
// code section with multi-byte nops.
void
align_loop_head(state_t &s)
{
    if (loop_alignment != 0)
        s.body << std::format("\t.p2align {}\n", loop_alignment);
}

// G14: the condition at the top, where it is written, tested once per
// iteration and once more; no rotation and no reasoning about it.
void
while_stmt(state_t &s, const while_t &w)
{
    const int cond = new_label(s);
    const int end = new_label(s);

    align_loop_head(s);
    place_label(s, cond);
    cbranch(s, w.cond, end, false);

    statement(s, *w.body);
    jump(s, cond);

    place_label(s, end);
}

// G14, as for while.  The step has its label, for continue.  The init
// is a statement: a declaration, a block of them for several
// declarators, or an expression.  Its names are in scope for the whole
// loop, 6.8.6.4p1.
void
for_stmt(state_t &s, const for_t &f)
{
    if (f.init)
        statement(s, *f.init);

    const int cond = new_label(s);
    const int next = new_label(s);
    const int end = new_label(s);

    align_loop_head(s);
    place_label(s, cond);
    if (f.cond)
        cbranch(s, *f.cond, end, false);

    statement(s, *f.body);

    place_label(s, next);
    if (f.step)
        effect(s, *f.step);
    jump(s, cond);

    place_label(s, end);
}

// The value goes to the convention's result register, backend.md 7.2
// and aarch64.md 7.2.  A char or a short is not extended: the
// convention leaves the bits above a narrow result unspecified, and the
// caller extends it, backend.md 6.3 and aarch64.md 6.3.  Every return
// jumps to the one epilogue, the last statement of the body included:
// the jump to the next instruction is what the statement says, and
// removing it was the optional peephole pass's work in the PDP-11
// compiler, which rcc has no counterpart of.
void
return_stmt(state_t &s, const return_t &r)
{
    if (r.value) {
        const int width = std::max(32, integer(r.value->type).width);

        const auto v = rvalue(s, *r.value);
        const reg_t from = reg_of(s, v);
        if (from != result_register)
            mov(s, from, result_register, width);

        release(s, v);
    }

    jump(s, s.ret);
}

// Every value an expression statement holds is consumed by its end;
// nothing is held from one statement to the next.
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

    if (s.live != 0)
        throw std::logic_error("c1: a value held across a statement");
}
