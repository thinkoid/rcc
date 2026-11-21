// -*- mode: c++; -*-

// The statements of backend.md 7.2 and 7.3 and aarch64.md 7.2 and 7.3:
// each a shape of labels and jumps around the generator's own
// functions, with no instruction of its own; and GNU C's statement
// expression, whose rows generate statements (backend.md 6.7).  Its text is the same for
// every target, but it names each target's state and calls each
// target's functions, so this is not a header in the ordinary sense: it
// is part of a generator's text, kept in one place, as c1-stack.hh is.
// c1-x86-64.cc and c1-aarch64.cc each include it once, inside their
// unnamed namespace, after the expressions, and so each compiles its
// own copy against its own state_t and functions.  It includes nothing,
// since it is read inside a namespace; the generator's own includes
// serve it.
//
// Before the include the generator declares reg_t; state_t with body,
// live, floor, ret, terminated, jump_targets, user_labels and
// case_labels, and arrays and label_arrays, the variable length arrays
// in scope and the count at each label (shape.md chapter 5);
// loop_alignment, the power of two of .p2align before a loop's head,
// 0 for none;
// result_register, the register the convention returns a result in;
// new_label(), place_label() and jump(); the three entry points,
// rvalue(), cbranch() and effect(), and rvalue_in(), a value for a
// consumer that names its register; test_zero(), the jump on a held
// value's test of zero; decl_stmt(), a declaration, which is the
// generator's own, since the store row of its initializer takes the
// target's form of the slot's address; and save_stack_pointer() and
// restore_stack_pointer(), the stack pointer kept in a slot and put
// back from it, a variable length array's rows.  For a switch's
// dispatch, whose form is chosen here and whose instructions are the
// generator's, it declares right_operand() and arithmetic(), the rows of
// a binary operator; compare_with(), the comparison row of a held value
// and a constant; jump_if(), the conditional jump on its flags;
// equal_jump(), a case's test; table_jump(), the jump through a table;
// and divided(), the hash's division.  For a statement expression it
// declares context_t, set_aside() and take_back(), the context of the
// expression around set aside for the statements and taken back for
// the value, and the three rows themselves, which its entry points
// call.  It includes c1-stack.hh first, whose reg_of() and release()
// the statements call, with mov(), the stack's copy of a register into
// another, for the result, and whose before_paths(), incoming_t,
// borrowed() and copy() a statement expression's rows call, with take()
// and expression_registers, the generator's.  integer(),
// jump_target_t and the rule of a switch are c1-common.hh's, and the
// tree ast.hh's.

void statement(state_t &, const stmt_t &);

void
expr_stmt(state_t &s, const expr_stmt_t &x)
{
    if (x.e)
        effect(s, *x.e);
}

// The storage of the variable length arrays in scope past count given
// back, 6.2.4p7: the stack pointer put back where it stood before the
// first of them, which its declaration kept; each one declared after it
// took its storage below there.  Nothing when there is none.
void
give_back(state_t &s, std::size_t count)
{
    if (s.arrays.size() > count)
        restore_stack_pointer(s, s.arrays[count]);
}

// The end of a scope, whose own arrays are those in scope past count:
// their storage given back where a path reaches the end, and they leave
// the arrays in scope.
void
close_scope(state_t &s, std::size_t count)
{
    if (!s.terminated)
        give_back(s, count);

    s.arrays.resize(count);
}

// A declaration, the generator's row.  A variable length array's keeps
// the stack pointer first, in the slot the parser declared for it, and
// the array is in scope from there, so that every way out of the scope
// puts the stack pointer back (shape.md chapter 5).
void
declaration(state_t &s, const decl_stmt_t &d)
{
    if (!d.saved) {
        decl_stmt(s, d);
        return;
    }

    save_stack_pointer(s, d.saved);
    decl_stmt(s, d);
    s.arrays.push_back(d.saved);
}

// A block's statements, in order; a compound statement's end is the
// end of its scope.
void
block_stmt(state_t &s, const block_t &b)
{
    const std::size_t arrays = s.arrays.size();

    for (const auto &st : b.body)
        statement(s, st);

    if (b.scope)
        close_scope(s, arrays);
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
// own; either leaves the scope of the arrays the body declares.
// Returns the record of which of the two a jump named.
jump_target_t
loop_body(state_t &s, const stmt_t &body, int next, int end)
{
    s.jump_targets.push_back(
        { .break_ = end, .continue_ = next, .arrays = s.arrays.size() });
    statement(s, body);

    const auto named = s.jump_targets.back();
    s.jump_targets.pop_back();

    return named;
}

// G14: the condition at the top, where it is written, tested once per
// iteration and once more; no rotation and no reasoning about it.  A
// continue jumps to the condition.  A body that ends in a jump, the
// state terminated after it, takes no jump back: no path leaves the
// body at its end to take it (backend.md 7.2).
void
while_stmt(state_t &s, const while_t &w)
{
    const int cond = new_label(s);
    const int end = new_label(s);

    align_loop_head(s);
    place_label(s, cond);
    cbranch(s, w.cond, end, false);

    loop_body(s, *w.body, cond, end);
    if (!s.terminated)
        jump(s, cond);

    place_label(s, end);
}

// backend.md 7.2: the body first, then the condition at the bottom,
// where it is written, tested once per iteration, 6.8.6.3p1, and the
// jump back taken while it holds.  The body's label is the loop's head.
// The condition is written where a path reaches it: out of the body's
// end, or by a continue, which jumps to its label.  Every label after
// the head is placed only where a jump names it: the condition's where
// a continue does, the one past the test where a break does.
void
do_stmt(state_t &s, const do_t &d)
{
    const int body = new_label(s);
    const int cond = new_label(s);
    const int end = new_label(s);

    align_loop_head(s);
    place_label(s, body);
    const auto named = loop_body(s, *d.body, cond, end);

    if (named.continued)
        place_label(s, cond);
    if (!s.terminated)
        cbranch(s, d.cond, body, true);

    if (named.broken)
        place_label(s, end);
}

// G14, as for while.  The step and the jump back are written where a
// path reaches them: out of the body's end, or by a continue, which
// jumps to the step's label.  Every label after the head is placed
// only where a jump names it: the step's where a continue does, the
// one past the loop where the condition's exit or a break does.  The
// init is a statement: a declaration, a block of them for several
// declarators, or an expression.  Its names are in scope for the whole
// loop, 6.8.6.4p1, so the statement's end is the end of their scope.
void
for_stmt(state_t &s, const for_t &f)
{
    const std::size_t arrays = s.arrays.size();

    if (f.init)
        statement(s, *f.init);

    const int cond = new_label(s);
    const int next = new_label(s);
    const int end = new_label(s);

    align_loop_head(s);
    place_label(s, cond);
    if (f.cond)
        cbranch(s, *f.cond, end, false);

    const auto named = loop_body(s, *f.body, next, end);

    if (named.continued)
        place_label(s, next);
    if (!s.terminated) {
        if (f.step)
            effect(s, *f.step);
        jump(s, cond);
    }

    if (f.cond || named.broken)
        place_label(s, end);

    close_scope(s, arrays);
}

// A constant of a switch's dispatch, a case's value or a number of the
// rule, as a literal of type t, for the rows that take a constant.
expr_t
dispatch_constant(const type_t &t, long long value)
{
    return { t, literal_t { value } };
}

// The table of a switch's dispatch, backend.md 7.3, as the Seventh
// Edition compiler wrote it: in .data, in the function's text where the
// dispatch stands, aligned to 8, under a label of the function, an
// address for each entry, which the loader relocates as it does a
// global's address constant; then .text again.
void
jump_table(state_t &s, int table, const std::vector< int > &entries)
{
    s.body << "\t.data\n\t.p2align 3\n"
           << label_name(s.function, table) << ":\n";

    for (const int entry : entries)
        s.body << std::format("\t.quad\t{}\n", label_name(s.function, entry));

    s.body << "\t.text\n";
}

// The direct form, backend.md 7.3: the first value subtracted when it
// is not 0, the result compared unsigned with the range and above it to
// otherwise, then the jump through a table of range + 1 entries, each a
// case's label or otherwise.  The dispatch is an operation of the
// unsigned type of the controlling expression's width, which the
// compare reads the result as, so the width rule knows that a 32-bit
// subtraction leaves the index valid to 64.
void
direct_switch(state_t &s,
              value_t v,
              const std::vector< case_t > &cases,
              int width,
              int otherwise)
{
    const type_t t = make_int(width, false);
    const long long first = cases.front().value;
    const auto range = case_range(cases, width);

    if (unsigned_bits(first, width) != 0) {
        const auto constant = dispatch_constant(t, first);
        v = arithmetic(s, binary_op_t::sub, v,
                       right_operand(s, binary_op_t::sub, constant, width), t);
    }

    compare_with(s, v, dispatch_constant(t, static_cast< long long >(range)),
                 width);
    jump_if(s, binary_op_t::gt, false, otherwise);

    std::vector< int > entries(range + 1, otherwise);
    for (const auto &c : cases)
        entries[case_offset(c.value, first, width)] = s.case_labels.at(c.label);

    const int table = new_label(s);
    table_jump(s, v, width, table);
    jump_table(s, table, entries);
}

// The simple form, backend.md 7.3: a case's test for each case, in the
// order of the values, then the jump to otherwise.
void
simple_switch(state_t &s,
              value_t v,
              const std::vector< case_t > &cases,
              const type_t &t,
              int otherwise)
{
    for (const auto &c : cases)
        equal_jump(s, v, dispatch_constant(t, c.value),
                   s.case_labels.at(c.label));

    release(s, v);
    jump(s, otherwise);
}

// The hash form, backend.md 7.3: the value divided unsigned by the size
// of the table, the jump through the table by the remainder to a bucket,
// and in each bucket a case's test of the quotient against each of its
// cases' value divided by the size, in the order of the values, then
// the jump to otherwise.  The quotient is held across the table's jump
// and the buckets; no case's label is among them.
void
hash_switch(state_t &s,
            value_t v,
            const std::vector< case_t > &cases,
            int width,
            int otherwise)
{
    const type_t t = make_int(width, false);
    const auto tabs = table_size(cases, width);

    const auto [quotient, remainder] = divided(
        s, v, dispatch_constant(t, static_cast< long long >(tabs)), width);

    const int table = new_label(s);
    std::vector< int > buckets;
    for (unsigned long long i = 0; i < tabs; ++i)
        buckets.push_back(new_label(s));

    table_jump(s, remainder, width, table);
    jump_table(s, table, buckets);

    for (unsigned long long i = 0; i < tabs; ++i) {
        place_label(s, buckets[i]);

        for (const auto &c : cases) {
            const auto bits = unsigned_bits(c.value, width);
            if (bits % tabs != i)
                continue;

            equal_jump(
                s, quotient,
                dispatch_constant(t, static_cast< long long >(bits / tabs)),
                s.case_labels.at(c.label));
        }

        jump(s, otherwise);
    }

    release(s, quotient);
}

// The labels of a switch's cases and of its default, each one more
// label of the function, numbered when c1 meets the switch, in the order
// they are written, which is the order of the parser's numbers.
void
number_case_labels(state_t &s, const switch_t &sw)
{
    std::vector< int > labels;
    for (const auto &c : sw.cases)
        labels.push_back(c.label);
    if (sw.default_)
        labels.push_back(*sw.default_);

    std::ranges::sort(labels);
    for (const int n : labels)
        s.case_labels.emplace(n, new_label(s));
}

// backend.md 7.3: the controlling expression's value, the dispatch of
// the form the rule of the Seventh Edition compiler chooses, then the
// body in source order, each label where it is written, and the label
// past the body, which a break in it jumps to and which stands for the
// default when there is none; with a default and no break, no jump
// names it, and it is not placed.  Nothing is held from the dispatch into
// the body.  With no case, the dispatch is the jump to the default or
// past the body, after the value, since it is written.
void
switch_stmt(state_t &s, const switch_t &sw)
{
    number_case_labels(s, sw);
    const int end = new_label(s);
    const int otherwise = sw.default_ ? s.case_labels.at(*sw.default_) : end;

    const auto &t = integer(sw.cond.type);
    const auto cases = sorted_cases(sw.cases, t);

    const auto form = dispatch_of(cases, t.width);
    const auto v = rvalue(s, sw.cond);

    if (form == dispatch_t::direct)
        direct_switch(s, v, cases, t.width, otherwise);
    else if (form == dispatch_t::simple)
        simple_switch(s, v, cases, sw.cond.type, otherwise);
    else if (form == dispatch_t::hash)
        hash_switch(s, v, cases, t.width, otherwise);
    else {
        release(s, v);
        jump(s, otherwise);
    }

    s.jump_targets.push_back({ .break_ = end,
                               .continue_ = std::nullopt,
                               .arrays = s.arrays.size() });
    statement(s, *sw.body);

    const bool broken = s.jump_targets.back().broken;
    s.jump_targets.pop_back();

    if (!sw.default_ || broken)
        place_label(s, end);
}

// The record of the innermost statement around the one generated that
// takes a break, 6.8.7.4p2.
jump_target_t &
innermost_breakable(state_t &s)
{
    if (s.jump_targets.empty())
        throw std::logic_error("c1: a break outside a loop or a switch");

    return s.jump_targets.back();
}

// The label a break jumps to: past the innermost statement around it
// that takes a break.  Each caller writes the jump, so the statement's
// record says a jump names the label.
int
break_target(state_t &s)
{
    jump_target_t &t = innermost_breakable(s);
    t.broken = true;

    return t.break_;
}

// A jump out of the innermost statement around it that takes a break,
// the storage of the arrays it leaves given back first.  Nothing is held
// at a statement's start, so the two are the whole of it.
void
break_stmt(state_t &s)
{
    give_back(s, innermost_breakable(s).arrays);
    jump(s, break_target(s));
}

// The record of the innermost loop around the one generated, which a
// continue goes on with, 6.8.7.3p2, past any switch between.
jump_target_t &
innermost_loop(state_t &s)
{
    for (auto t = s.jump_targets.rbegin(); t != s.jump_targets.rend(); ++t)
        if (t->continue_)
            return *t;

    throw std::logic_error("c1: a continue outside a loop");
}

// The label a continue jumps to: the continuation of the innermost loop
// around it.  The caller writes the jump, as break_target()'s does, and
// the loop's record says so.
int
continue_target(state_t &s)
{
    jump_target_t &t = innermost_loop(s);
    t.continued = true;

    return *t.continue_;
}

// A jump to the continuation of the innermost loop around it, the
// storage of the arrays it leaves given back first, as for a break.
void
continue_stmt(state_t &s)
{
    give_back(s, innermost_loop(s).arrays);
    jump(s, continue_target(s));
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

// A case or a default label, placed where it is written under the
// number its switch gave it; no value is held there.
void
case_label_stmt(state_t &s, const case_label_t &l)
{
    place_label(s, s.case_labels.at(l.label));
}

// The count of the variable length arrays in scope at a label the
// source names, the parser's, and none for a label in the scope of
// none.
std::size_t
label_arrays(const state_t &s, const std::string &name)
{
    const auto at = s.label_arrays->find(name);

    return at == s.label_arrays->end() ? 0 : at->second;
}

// A jump to the label, the storage of the arrays it leaves given back
// first: those in scope past its label's count, since the arrays in
// scope at the label are the first of those in scope at the goto.
void
goto_stmt(state_t &s, const goto_t &g)
{
    give_back(s, label_arrays(s, g.name));
    jump(s, user_label(s, g.name));
}

// The value goes to the convention's result register, backend.md 7.2
// and aarch64.md 7.2: the return names it, and the value is computed
// there where its rows can write it, rvalue_in(), and moved there
// otherwise.  A char or a short is not extended: the convention leaves
// the bits above a narrow result unspecified, and the caller extends
// it, backend.md 6.3 and aarch64.md 6.3.  Every return jumps to the one
// epilogue, the last statement of the body included: the jump to the
// next instruction is what the statement says, and removing it was the
// optional peephole pass's work in the PDP-11 compiler, which rcc has
// no counterpart of.
void
return_stmt(state_t &s, const return_t &r)
{
    if (r.value) {
        const int width = std::max(32, integer(r.value->type).width);

        const auto v = rvalue_in(s, *r.value, result_register);
        const reg_t from = reg_of(s, v);
        if (from != result_register)
            mov(s, from, result_register, width);

        release(s, v);
    }

    jump(s, s.ret);
}

// The count of the variable length arrays in scope where a jump lands:
// at the statement a break leaves, at the loop a continue goes on with,
// at a goto's label.  Every array in scope for any other statement, a
// return among them, which leaves its storage to the epilogue.
std::size_t
landing_arrays(state_t &s, const stmt_t &st)
{
    if (is< break_t >(st))
        return innermost_breakable(s).arrays;
    if (is< continue_t >(st))
        return innermost_loop(s).arrays;
    if (is< goto_t >(st))
        return label_arrays(s, as< goto_t >(st).name);

    return s.arrays.size();
}

// The label of a statement that is one jump, backend.md 7.2: a break's,
// a continue's, a goto's, or the epilogue's for a return with no value.
// Nothing for any other statement, a return with a value among them,
// whose value is code of its own, nor for a jump that gives back the
// storage of an array it leaves, which is code of its own too.
std::optional< int >
target_of(state_t &s, const stmt_t &st)
{
    if (s.arrays.size() > landing_arrays(s, st))
        return std::nullopt;

    if (is< break_t >(st))
        return break_target(s);
    if (is< continue_t >(st))
        return continue_target(s);
    if (is< goto_t >(st))
        return user_label(s, as< goto_t >(st).name);
    if (is< return_t >(st) && !as< return_t >(st).value)
        return s.ret;

    return std::nullopt;
}

// The label an if's body jumps to when the body is one jump, bare or
// alone in a block, whose braces write nothing.
std::optional< int >
body_target(state_t &s, const stmt_t &body)
{
    if (!is< block_t >(body))
        return target_of(s, body);

    const auto &b = as< block_t >(body).body;
    if (b.size() != 1)
        return std::nullopt;

    return target_of(s, b.front());
}

// An if whose body is one jump, backend.md 7.2: the condition's jump to
// the body's target, taken when it holds, and the else part after it
// with no jump over it, as the first pass of the Seventh Edition
// compiler writes it, c02.c.
void
if_jump(state_t &s, const if_t &i, int target)
{
    cbranch(s, i.cond, target, true);

    if (i.no)
        statement(s, *i.no);
}

// backend.md 7.2 and aarch64.md 7.2: the condition's false exit to the
// else part, or past the statement without one, but for a body that is
// one jump, which is if_jump()'s.  A body that ends in a jump, the state
// terminated after it, takes no jump over the else part and no label
// past it: no path leaves the body at its end to take them.
void
if_stmt(state_t &s, const if_t &i)
{
    if (const auto target = body_target(s, *i.yes)) {
        if_jump(s, i, *target);
        return;
    }

    const int no = new_label(s);
    cbranch(s, i.cond, no, false);

    statement(s, *i.yes);

    if (!i.no) {
        place_label(s, no);
        return;
    }

    if (s.terminated) {
        place_label(s, no);
        statement(s, *i.no);
        return;
    }

    const int end = new_label(s);
    jump(s, end);

    place_label(s, no);
    statement(s, *i.no);

    place_label(s, end);
}

// Every value an expression statement holds is consumed by its end;
// nothing is held from one statement to the next.  A statement
// expression's statements run with the values of the expression around
// it held below the floor, which they leave as they find them
// (backend.md 6.7).
void
statement(state_t &s, const stmt_t &st)
{
    if (is< expr_stmt_t >(st))
        expr_stmt(s, as< expr_stmt_t >(st));
    else if (is< decl_stmt_t >(st))
        declaration(s, as< decl_stmt_t >(st));
    else if (is< block_t >(st))
        block_stmt(s, as< block_t >(st));
    else if (is< if_t >(st))
        if_stmt(s, as< if_t >(st));
    else if (is< switch_t >(st))
        switch_stmt(s, as< switch_t >(st));
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
    else if (is< case_label_t >(st))
        case_label_stmt(s, as< case_label_t >(st));
    else if (is< goto_t >(st))
        goto_stmt(s, as< goto_t >(st));
    else
        return_stmt(s, as< return_t >(st));

    if (s.live != s.floor)
        throw std::logic_error("c1: a value held across a statement");
}

// GNU C's statement expression (shape.md chapter 5)

// Whether a statement expression's block declares sym among its own
// items, an object whose life ends with the block, 6.2.4p6.
bool
declares(const statement_expression_t &x, const symbol_t *sym)
{
    return std::ranges::any_of(x.body, [sym](const stmt_t &st) {
        return is< decl_stmt_t >(st) && as< decl_stmt_t >(st).sym == sym;
    });
}

// A statement expression's statements, where it stands (G13), each the
// statement it would be at a boundary of statements: the values the
// expression around holds are saved by then, held below the floor and
// marked held across the statements by the caller's before_paths(), and
// that expression's context is set aside, set_aside()'s.
void
statement_expression_body(state_t &s, const statement_expression_t &x)
{
    const auto context = set_aside(s);
    const int floor = std::exchange(s.floor, s.live);

    for (const auto &st : x.body)
        statement(s, st);

    s.floor = floor;
    take_back(s, context);
}

// Whether the statements of a statement expression may move a value held
// beside it, backend.md 6.7: a node of moves_beside()'s; a write of a
// register variable whose read is held; or more values held at once in
// one statement or in the value, held_at_most()'s, than expression
// registers are free, where take() would spill the oldest value held,
// one beside the block.
bool
saves_beside(const state_t &s, const expr_t &e)
{
    const auto &x = as< statement_expression_t >(e);
    if (moves_beside(x))
        return true;

    for (const auto &h : s.values)
        if (h.live && h.variable && writes(e, h.variable))
            return true;

    int free = 0;
    for (const reg_t r : expression_registers)
        free += s.owner[r] < 0;

    return held_at_most(x) > free;
}

// The values held when a statement expression starts, backend.md 6.7:
// saved as at a branch where its statements may move them,
// before_paths()'s, and otherwise left where they are, each marked until
// the value is computed, so that an instruction that would move one is
// an error of the generator.
incoming_t
beside(state_t &s, const expr_t &e)
{
    if (saves_beside(s, e))
        return before_paths(s, e);

    return incoming_t { s };
}

// The value of a statement expression, backend.md 6.7: the values held
// when it starts saved where its statements may move them, beside()'s,
// and kept where they are until the value is computed; then its
// statements; then its value, handed into and wanted as the comma's
// right operand is.  A borrowed read of a register variable the block
// declares is copied into an expression register: the variable's
// register is given back at the block's end, and a block after it, a
// sibling statement expression's, takes it.  The end of its scope
// follows the value, which is computed in it.
value_t
statement_expression(state_t &s, const expr_t &e, into_t *into, unsigned wanted)
{
    const auto &x = as< statement_expression_t >(e);
    const auto incoming = beside(s, e);
    const std::size_t arrays = s.arrays.size();

    statement_expression_body(s, x);
    if (!x.value) {
        close_scope(s, arrays);
        return { };
    }

    const auto v = rvalue(s, *x.value, into, wanted);
    if (borrowed(s, v) && declares(x, s.values[v.id].variable))
        copy(s, v, take(s, 0));

    close_scope(s, arrays);

    return v;
}

// A statement expression for its effect: its statements, then its
// value's effect, the values held saved first as for its value, and the
// end of its scope.
void
statement_expression_effect(state_t &s, const expr_t &e)
{
    const auto &x = as< statement_expression_t >(e);
    const auto incoming = beside(s, e);
    const std::size_t arrays = s.arrays.size();

    statement_expression_body(s, x);
    if (x.value)
        effect(s, *x.value);

    close_scope(s, arrays);
}

// A statement expression as a condition: its statements, then its
// value's jump code, the values held saved first as for its value.  A
// block that declares a variable length array ends its scope after the
// value and before the jump: the value is computed into a register, the
// storage given back, and the register's test of zero is the jump.
void
statement_expression_branch(state_t &s, const expr_t &e, int label, bool sense)
{
    const auto &x = as< statement_expression_t >(e);
    const auto incoming = beside(s, e);
    const std::size_t arrays = s.arrays.size();

    statement_expression_body(s, x);
    if (s.arrays.size() == arrays) {
        cbranch(s, *x.value, label, sense);
        return;
    }

    const auto v = rvalue(s, *x.value);
    close_scope(s, arrays);
    test_zero(s, v, integer(x.value->type), label, sense);
    release(s, v);
}
