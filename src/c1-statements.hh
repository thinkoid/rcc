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
// live, ret, jump_targets and user_labels; loop_alignment, the power of
// two of .p2align before a loop's head, 0 for none; result_register,
// the register the convention returns a result in; new_label(),
// place_label() and jump(); the three entry points, rvalue(), cbranch()
// and effect(); and decl_stmt(), a declaration, which is the
// generator's own, since the store row of its initializer takes the
// target's form of the slot's address.  It includes c1-stack.hh first,
// whose reg_of() and release() the statements call, with mov(), the
// stack's copy of a register into another, for the result.  integer()
// and jump_target_t are c1-common.hh's, and the tree ast.hh's.

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

// The body of a loop, backend.md 7.2: a break in it jumps to end and a
// continue to next, but for one in a loop inside it, which takes its
// own.
void
loop_body(state_t &s, const stmt_t &body, int next, int end)
{
    s.jump_targets.push_back({ end, next });
    statement(s, body);
    s.jump_targets.pop_back();
}

// G14: the condition at the top, where it is written, tested once per
// iteration and once more; no rotation and no reasoning about it.  A
// continue jumps to the condition.
void
while_stmt(state_t &s, const while_t &w)
{
    const int cond = new_label(s);
    const int end = new_label(s);

    align_loop_head(s);
    place_label(s, cond);
    cbranch(s, w.cond, end, false);

    loop_body(s, *w.body, cond, end);
    jump(s, cond);

    place_label(s, end);
}

// backend.md 7.2: the body first, then the condition at the bottom,
// where it is written, tested once per iteration, 6.8.6.3p1, and the
// jump back taken while it holds.  The body's label is the loop's head.
// A continue jumps to the condition.  The label past the test is placed
// whether or not a break jumps to it, as while's and for's are.
void
do_stmt(state_t &s, const do_t &d)
{
    const int body = new_label(s);
    const int cond = new_label(s);
    const int end = new_label(s);

    align_loop_head(s);
    place_label(s, body);
    loop_body(s, *d.body, cond, end);

    place_label(s, cond);
    cbranch(s, d.cond, body, true);

    place_label(s, end);
}

// G14, as for while.  The step has its label, which a continue jumps
// to.  The init is a statement: a declaration, a block of them for
// several declarators, or an expression.  Its names are in scope for
// the whole loop, 6.8.6.4p1.
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

    loop_body(s, *f.body, next, end);

    place_label(s, next);
    if (f.step)
        effect(s, *f.step);
    jump(s, cond);

    place_label(s, end);
}

// A jump out of the innermost statement around it that takes a break,
// 6.8.7.4p2.  Nothing is held at a statement's start, so the jump is
// the whole of it.
void
break_stmt(state_t &s)
{
    if (s.jump_targets.empty())
        throw std::logic_error("c1: a break outside a loop or a switch");

    jump(s, s.jump_targets.back().break_);
}

// A jump to the continuation of the innermost loop around it,
// 6.8.7.3p2, past any switch between.
void
continue_stmt(state_t &s)
{
    for (auto t = s.jump_targets.rbegin(); t != s.jump_targets.rend(); ++t)
        if (t->continue_) {
            jump(s, *t->continue_);
            return;
        }

    throw std::logic_error("c1: a continue outside a loop");
}

// The number of a label the source names: one more label of the
// function, numbered when c1 first meets it, at its definition or at a
// goto to it.  No listing shows the name.
int
user_label(state_t &s, const std::string &name)
{
    const auto [at, added] = s.user_labels.try_emplace(name, 0);
    if (added)
        at->second = new_label(s);

    return at->second;
}

// A label the source names is placed where it is written and is not
// aligned, even when a goto jumps back to it: a loop built of goto is
// laid out as written, backend.md 7.2.
void
label_stmt(state_t &s, const label_t &l)
{
    place_label(s, user_label(s, l.name));
}

void
goto_stmt(state_t &s, const goto_t &g)
{
    jump(s, user_label(s, g.name));
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
    else if (is< do_t >(st))
        do_stmt(s, as< do_t >(st));
    else if (is< for_t >(st))
        for_stmt(s, as< for_t >(st));
    else if (is< break_t >(st))
        break_stmt(s);
    else if (is< continue_t >(st))
        continue_stmt(s);
    else if (is< label_t >(st))
        label_stmt(s, as< label_t >(st));
    else if (is< goto_t >(st))
        goto_stmt(s, as< goto_t >(st));
    else
        return_stmt(s, as< return_t >(st));

    if (s.live != 0)
        throw std::logic_error("c1: a value held across a statement");
}
