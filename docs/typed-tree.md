# rcc: the typed tree

The typed tree is a second design of rcc's tree and of its
generators. It grows in `src/v2` and is built as `rcc-v2`, beside the
compiler `backend.md` describes, which stays the reference. This page
states the design: the idea, its principles, its idioms and its
architecture. It leaves the details to the code.

Of the design the tree has a thin line through every joint: a unit of
functions, declared and defined in any order, `static` or not, of a
result of any integer type or `void` and of `int` parameters, whose
locals are of the integer types from `char` to `unsigned long long`,
declared, assigned and returned, with parentheses, the casts, the
unary operators `+ - ~ !`, `+ - * / % << >> & | ^`, the comparisons,
`&&`, `||` and calls of functions of an `int` or a `void` result, to
any depth, and the statements `if`, `else`, `while`, `do`, `for`
with a declaration or an expression first, `break`, `continue`,
`goto` and labels, and `return` with a value or without, on both
targets.
Each mechanism
below was compiled and run in a small model first; chapter 5 says
what the models showed and chapter 6 what nothing has shown. A
statement marked *working* is a position taken and run, not yet
agreed.

## 1. The idea

A *row* is the code a generator writes for one operator in one form.
In rcc a row asks its operands what they are while it generates, and
`designs.md` chapter 3 finds that most edge cases come from those
questions: each is a predicate written by hand that mirrors what
another function will do.

The typed tree asks each question once, where the node is built, and
keeps the answer as the node's type.

```c++
template< binary_op_t op, view_t V, typename L = expr_t,
          typename R = expr_t >
struct binary_t { L lhs; R rhs; };

// the primary: asks nothing
template< binary_op_t op, view_t V, typename L, typename R >
value_t value(state_t &, const binary_t< op, V, L, R > &);

// a niche: addl $5, %eax
template< binary_op_t op, view_t V, typename L >
value_t value(state_t &, const binary_t< op, V, L, literal_t< V > > &);
```

- The tree is not erased uniformly. `expr_t` is an erased handle. What
  it holds is a complete type, whose parts are concrete where a
  decision depends on them and erased where none does.
- The types are a finite *zoo*: primary templates that describe the
  constructs, and their specializations.
- A row is a free function overloaded on the node's type. The
  *primary* is the row that asks nothing. A *niche* is a row on a
  more special type, which buys an instruction back.
- There is no variant, so a row cannot ask what a child is. The fact
  is in the row's own type, or the row does not have it.

This is the Seventh Edition compiler's code table (`ancestor.md`),
with C++'s types in place of its operand classes and overload
resolution in place of its `match()`. It needs no second language.

## 2. Principles

1. **Correctness is the primaries'; optimization is the niches'.** A
   niche is a shorter spelling of what its primary computes, never
   another meaning. The design is judged first by whether primaries
   alone compile C. Each instruction saved is a niche, added alone
   and checked against its primary.
2. **A question is asked once, and its answer is a type.** A question
   about a type is an overload. A question about a constant's value is
   asked in the builder that makes the leaf, and its answer is a type:
   `zero_t`, `bit_t` for a constant of one bit, or `wide_t` for one of
   64 bits past `int`'s range. Only a question about the context or
   the state stays an `if`, and one more: a question of one target's
   encoding about a row's own leaf, whether aarch64's `add` holds its
   constant, which no builder can ask, since a builder names no
   target. Overloads and specializations are how conditionals leave
   the generator.
3. **A fact lives where its distance puts it.**

   | The fact | Its distance | Where it lives |
   | --- | --- | --- |
   | "my right operand is a constant" | bounded: an edge or two | the type |
   | "this subtree names `r`" | unbounded, below | the summary, handed up |
   | "this value's register is valid to 64 bits" | unbounded, below | the summary, handed up |
   | "the value is wanted in `%ebx`" | unbounded, above | the context, handed down |
   | "which register is free now" | a moment of generation | the state |

4. **The context carries only what a type cannot.** The form wanted
   is not context: value, effect, jump and address are the entry
   point a parent calls. The context is the register a consumer
   would like, which is a hint, and a jump's target and sense.
5. **The zoo's size is no cost.** A primary that is a template covers
   every instantiation, so a fine type costs no code. A fine type is a
   niche kept open. The count of types is never an argument. Which
   niches are written is the project's older question, what the
   ancestor's generator wrote by itself (`ancestor.md`), and this
   design does not answer it.
6. **A static fact is a template parameter; a question is a free
   function.** These are the standard library's traits. A closed
   domain is a parameter as it stands: the operator, the storage, the
   width. An open domain is data: an offset, a constant's value, a C
   type. But its values can be classed, and the class is a parameter.
   The machine's view of a type is finite and is parameters; C's view
   is open and is data. A parameter is seen only where the type is
   concrete, so each one is a crossing from data to a type, made once,
   in a builder.
7. **A category is derived from rows, never listed.** A category is a
   concept that holds for the types which have a given row. A list
   kept by hand is a second copy of the rows, and the two drift.
8. **A rule of C is overload resolution.** A builder constrained by a
   category exists only for what C permits: an assignment for a
   place, and none for a `const` one. Where the builder does not
   exist the parser says the diagnostic, "expression is not
   assignable". rcc says it before it reads the right operand, so the
   parser asks first, and the question is the builder's existence, a
   concept: no list of the types that are places is kept.
9. **No row reads the C type.** *Working.* The builder moves what a
   row needs into the node: a stride, a view, an operator's sign,
   which is in the operator's name. The C type is the parser's, and
   it picks the next builder.
10. **An interface is whole.** Every type under a handle has a
    meaningful body for every operation of that handle. By this test
    statements and expressions are two handles, and two targets are
    one.
11. **Introspection has one address: the builders.** `is< >` and
    `as< >` are not ruled out. They belong where data becomes a type,
    in a builder or in the parser. A row that uses one on a child is
    asking an operand's kind again.
12. **Names are found, not invented.** LLVM's conventions are the
    first place to look: the views are `i8` to `i64`, the operators
    of a sign `sdiv` and `udiv`, `ashr` and `lshr`, `slt` and `ult`,
    and the conversions `sext`, `zext` and `trunc`.

## 3. Idioms

### 3.1 The handle

The handle is value-semantic type erasure: an interface, a model
template, and free functions.

```c++
class expr_t
{
    struct interface_t                    // the operations: closed
    {
        virtual value_t value(state_t &) const = 0;
        virtual expr_t member(long offset) const = 0;
    };

    template< typename T >                // the types: open
    struct model_t final : interface_t
    {
        value_t value(state_t &s) const override
        {
            return fwd_value(s, held);    // to the row, by overload
        }

        expr_t member(long offset) const override
        {
            return make_member(held, offset);    // T is complete here
        }

        T held;
    };

    std::shared_ptr< const interface_t > self;

public:
    template< typename T > explicit expr_t(T);
};
```

- The row is found by overload where the model is instantiated. A
  type with no row does not build. Where a family has no generic
  form, its primary is declared and not defined, and "not yet" is a
  compile error of rcc itself.
- The constructor is `explicit`. Otherwise a type with no row converts
  to the handle, and the handle's own row is a catch-all nobody wrote.
- A member named as the free function it calls hides that function.
  The call goes through a forwarder, `fwd_` and the function's name,
  which keeps the lookup by argument.
- Every model is instantiated in one translation unit, `build.cc`,
  which includes the node types and every row of both targets. Two
  units that saw different overloads would break the one-definition
  rule with no diagnostic; one unit cannot.
- A model hands each builder its operand in the class the builder
  reads, as the binary builder's left operand is classed: a constant,
  a load, a conversion a builder reads through, a `top_t`, or the
  handle. So a builder's crossings are instantiated once for each
  class and not for each node type, and a model costs its rows and
  little more.
- The parser sees a handle as an opaque value and the builders as
  plain functions (`build.hh`). It cannot make a node any other way.
- The handle has no member but its constructor. Its own row is the
  overload that serves an erased child, so a row calls a child by the
  free name whether the child's type is erased or concrete.

### 3.2 Rows

- Function templates are overloaded, not specialized. Partial ordering
  picks the most special row: that is the table's `match()`.
- Two niches that overlap are ambiguous until a third says which wins.
  The priority is stated, or rcc does not build.
- A row may be the row of another node. A constant on the left of an
  operator that commutes is written by the row of the node with its
  operands exchanged. The tree keeps the operands as they are
  written, since the rule that exchanges them is each target's.
- Operators whose rows differ are families, each named by a concept
  on the operator: on x86-64 the arithmetic operators take an
  immediate and an operand in memory, a division no immediate, and a
  shift neither. A
  row is constrained by its family, so a niche of one family is no
  candidate for a node of another.
- A type that a builder makes and a target never refines compiles,
  and writes its primary's listing. Only a pinned listing catches
  that.

### 3.3 The builder is the matcher

The parser holds erased operands. It calls a builder through the
handle, and inside the model the type is complete, so the overload is
resolved when rcc is compiled. `give ().f` reaches `make_member(const
call_t &, long)` and comes back as a member of a call.

- A shape travels with its handle. Parentheses and `_Generic` cost
  nothing, and no shape is found again from its tokens.
- **The primary erases its children again.** A builder whose result
  nests its operand's type for every type instantiates without end.
  This rule is what makes the zoo finite.
- One operand's type comes for nothing. The second is had by a second
  hop: the builder holds the right operand's type from its model and
  calls through the left's, one operation of the interface for each
  class of the operand already known.
- A builder returns the best node, of any family. `x + 0` is the
  value of `x`; `*&x` is the place of `x`; a sum of two literals is a
  literal.
- A class may keep one fact of an operand it erases. A right operand
  whose top operator pins a register on a target is kept as
  `top_t< F >`: the node erased, its own niches intact, and its
  operator's family, a division or a shift by a count that is no
  constant, the class's parameter, which the parent's row reads as a
  constant. A conversion over such an operand keeps the class, as
  rcc's `pinned_by()` reads through a cast. It is a class and no
  node: a builder that returns the operand itself returns the node
  inside.
- A class crosses the interface as data. A view or a sign as a type
  would make one operation of the interface for each, so the right
  operand's class is handed to the left operand's model with its
  parameters as values, a constant with its view and a load with its
  sign, and made a type again there, at the operation's view. A right
  operand of a type of its own, a shift's count and an operand of
  `&&` or `||`, crosses erased, but for a constant, which folds.
- A type exists only where a builder makes it. The zoo is as fine as
  the builders are, and no finer: the parser converts both operands
  of an operator to one type, so a left operand of another view than
  the operation's is never built, and the crossing stops there
  instead of naming the type, which would instantiate its rows. `&&`
  and `||`, whose operands keep their types, take every class.

### 3.4 Entry points are roles

A parent asks a child for a form by the entry point it calls: its
value, its effect, its store into a slot, a jump to a label on a
sense, its address. The choice is static. The value, the effect and
the jump are the ancestor's `regtab`, `efftab` and `cctab`.

- The other direction is derived: an effect is a value dropped, a
  jump on a value is the value compared with 0.
- `value` is the entry point with no primary: every type has a row
  that returns its value, held in a register the row takes
  (principle 10). The consumer reads where the value is when it
  writes its own instruction, and moves it if it must be elsewhere: a
  `return` moves it to the result register. The value a row returns
  need not be an operand's: on x86-64 a remainder is a new value in
  `rdx`, where the instruction leaves it.
- The register discipline is rcc's, copied as its text is: which
  register a value lands in, when a held value is moved or spilled,
  what a pinned instruction needs free. The typed tree changes how a
  question about an operand's kind is asked, and never where a value
  lands.
- The store into a slot is rcc's `initialize()`. An initializer, an
  assignment for its effect and a call's argument store their value
  through the value's own entry point, whose row is found by overload
  on the value's type: a target's niche for a constant, which holds
  no register, or the primary, the value in a register, stored and
  released. So the parent keeps the value erased. The store's width
  is the value's own view: the parser converts every value it stores
  to its place's type, so the row on the value's type knows it, and
  the place hands the row its where alone.
- A call spills every held value at its start, in the order of the
  registers, so its arguments find every register free. Each
  argument is stored to a slot of its own before the next is
  generated, and loaded into its register just before the call. A
  call keeps its own pin as a division does: on x86-64 its row
  generates it outside the reservation around it.
- A value is in a register or in a spill slot. When every register
  is held, the oldest held value is stored to a new slot of the
  frame, all 8 bytes, and it is reloaded where a row asks for its
  register. So the order in which a row asks for its operands'
  registers is part of the listing, and it is rcc's: the right
  operand's, then the left's. On x86-64 a spilled left operand of an
  operator that commutes is read in its slot where no register is
  named for it, `addl -16(%rbp), %ecx`.
- A register the consumer would like is context, handed down as a
  hint, which a row is free not to honour. It is a parameter of
  `value`, the registers the consumer names, none by default: a
  `return` names the result register, and on x86-64 a shift names
  `rcx` for its count. On x86-64, where an instruction writes one of
  its operands, a row hands the hint to the operand whose register
  will hold the node's value. On aarch64 it goes to the row's last
  instruction, which names its destination apart from its sources:
  `add w0, w1, w0`. A row both targets write alike hands on what it
  is given. Every value row takes the parameter: a niche without it
  is no candidate when a parent hands a hint, and its primary is
  written in its place.
- `branch` jumps to a label where the node's value is nonzero, its
  sense true, or where it is 0, and holds no value after it: rcc's
  `cbranch()`. Its primary is the derived direction, the value
  compared with 0. A comparison's row is its `cmp` and the jump of
  its operator, or of the inverse on the other sense. `!`'s is its
  operand's jump on the other sense, and writes nothing. `&&`'s and
  `||`'s are their operands' jumps, with a skip label taken before
  the left operand where the left one jumps the other way.
- On x86-64 the value compared with 0 is an entry point of its own,
  `value_branch`, a line of that target. Its niches are a load
  compared in place, `cmpl $0, -4(%rbp)`, and the operators whose own
  instruction sets ZF: `+ - | ^` and unary `-` jump on it with no
  `cmp`, and `&` is `test`. `e == 0` and `e != 0` jump as `e`'s
  `value_branch`, whatever `e` is: a parent that needs one fact of
  an operand it erased asks the operand. aarch64 has no such line:
  `a == 0` is `cmp` and `b.ne`, where `if (a)` is `cbz`. The primary
  of the value compared with 0 is each target's own: aarch64's tests
  a value whose register is valid to less than 32 bits with `tst`,
  `tst w0, #0xff` (4.2).
- Before the paths of `&&` or `||` part, every value held in a
  register is spilled, in the order of the registers, and every
  value held is marked until the paths meet: a spill, a reload or a
  move of one inside them stops the generation. This is the
  generator's check on itself.
- An entry point carries a child's best form to a parent that does
  not know the child's type. In `if (a & 1) break;` the statement is
  `if_t< jump_t >`, its condition erased, and x86-64's `testl $1,
  -4(%rbp)` is the condition's own row. No `if_t< and_t< ... >,
  jump_t >` is needed.
- So a niche on the parent and an entry point on the child are two
  spellings of one match. Both can hold.

### 3.5 Categories

```c++
template< typename B >
concept place = requires(state_t &s, const B &b) {
    address(s, b);    // B has a row that writes its address
};
```

| Category | The row that defines it | Members |
| --- | --- | --- |
| place | an address | a slot, a global, a dereference, a string literal |
| addressable | an address that writes no instruction | a place that stands as an operand, `-8(%rbp)` |

*Working:* the cut is a first one. C's own sorts (a place, a value, a
function designator, void), how the machine names an operand, and its
register class are separate axes.

No category of conditions is kept. A parent that jumps on a child
calls the child's `branch`, and the one parent that needs a fact of
an operand's jump, x86-64's `e == 0`, calls the operand's
`value_branch`.

A category's row may change the state: `label_of()` of a `goto` takes
its label where the generation first meets it, so the category of
the statements that are one jump asks the row of a state it may
write.

### 3.6 The oracle of primaries

An rcc whose builders make only primaries compiles every program to
the same result, in longer listings. It checks every niche against
its primary, and it is what makes a large zoo safe.

- It has two levels. The builders are a switch of a run. A row
  written on a leaf is a switch of a build, since a leaf is always
  concrete in its model.
- `is< >` of a complete type, asked outside the builder that made it,
  answers differently under the oracle. `is< >` of a family or of a
  leaf does not.
- The width rule (4.2) is no niche: a widening's primary writes its
  extension where its operand's summary says the register is not
  valid to the new width, as rcc's `extended()` does, so the oracle
  writes rcc's extensions. The extending load is a niche, and comes
  off.

## 4. Architecture

### 4.1 The pipeline

The driver preprocesses and calls one function (`pipeline.hh`). The
lexer is rcc's. The parser is a recursive descent with one function
for each production; it holds the C types and the scopes, and it
calls builders. The builders make the typed tree. Each target's rows
write its text. There is one parser and one zoo.

- The parser is the whole grammar from the start. A production whose
  construct rcc-v2 does not compile refuses it at its own token, as
  "not yet" and the construct's name, so no error is said of a
  construct the parser did not read.
- rcc is the reference. For a program, rcc-v2 writes rcc's listing or
  says rcc's error, or it says not yet. `make sides` compiles every
  program under `tests/` with both compilers and fails on anything
  else; the count of the runs that are the same is the measure of
  the way done. `tests/side/` holds the programs written for a step.

### 4.2 Expressions: a where and a view

A place is a *where* and a *view*: `place_t< V, S >` so far, over the
one where the tree has, `slot_t`, with its object's sign.

- The where is how the machine names the object: a local's slot, and
  later a register variable, a global, the object through a pointer's
  value. The store's entry point is handed the where.
- The view is how it is read, and it is the width alone: `i8`, `i16`,
  `i32`, `i64`. Every value node has a view, the width of its value,
  and an operator's parameter is its operation's view, `i32` or
  `i64`; a comparison's value is an `int`'s.
- `&` takes the view off, `address_t< W >`. `*` puts one on, read
  from the pointer's C type. `*` over an address is the place again;
  its primary goes through the pointer's value.
- A cast between pointer types writes no node. It is the same node
  under another C type.
- No register has a sign; instructions have. So the sign is the
  operator's: an operator of each sign where the instruction differs,
  `sdiv` and `udiv`, `srem` and `urem`, `ashr` and `lshr`, `slt` and
  `ult`, and a widening's, `sext_t` and `zext_t`. A place keeps its
  object's sign too, which its narrow read extends by, since rcc
  reads a `char` by `movsbl` or `movzbl` whatever consumes it. A
  constant's sign is in its value.
- `load_t< V, S >` is what a place holds, and it is no place. An
  lvalue is a matter of type, so an identity is dropped where it is
  built.
- A conversion is a node around its operand: `sext_t` and `zext_t`
  widen by the sign of the type converted from, and `trunc_t`
  narrows, changes the sign alone, or casts to the type the operand
  has, and writes nothing. LLVM's `trunc` only narrows and its
  `bitcast` keeps the width; here one node serves the three, whose
  rows are one. A conversion hides the operand from the parent's
  niches, so its builder folds a constant, and a parent that names an
  object in place looks through a `trunc_t` of its view over a load.
- A conversion over a load is the load's niche: a signed object
  widened to 64 bits is its extending load, `movslq -4(%rbp), %rax`.
  Any other widening of a load is its primary, since the load has
  extended the object already. The widening's builder finds the
  object under a change of sign and under a promotion of its own, as
  rcc's `widened_read()` does, and reads it once.
- The width a value is valid to is a summary of its subtree,
  `valid_of(e, sign)`: the width up to which its register's bits are
  the value extended by `sign`, the sign of the type its consumer
  converts it from. It is rcc's `written()`: an instruction that
  writes 32 bits clears the upper half, which is the extension of an
  unsigned value alone. A conversion leaves its value valid to its
  own width, and a call's result is valid to its type's. The parser
  builds every conversion from its operand's own C type, in one
  function, so a node that is no conversion has the type its consumer
  converts from, and no node needs its own sign for the rule. A
  widening writes its extension where the summary is below its
  width, and aarch64 tests a value valid to less than 32 bits with
  `tst`.
- The parser converts every operand whose value it reads, by a
  builder through the model, `rvalue()`: a place becomes its load,
  and anything else stays the node it is. So a place asked for its
  value is a tree the parser does not make, and a place's `value`
  row stops the generation.

### 4.3 Constants

Three things are kept apart: the value, the fold and a stage.

- The value is asked: `value_of()`, a free function, each node's from
  its operands'. It is the value the constant's register holds, read
  as a signed number of 32 bits or more: a narrow constant's
  promotion, an `unsigned` 4294967295 as -1. A case label and an
  array bound need it early, and will ask it through the model. They
  do not need the fold. So far it is written for the constant types,
  and a builder that folds asks it.
- The fold is an overload on the operands' types, beside the other
  builders, at the node's view and by its operator's sign. A
  constant's conversion is one function of the two views and the two
  signs. It comes off with every niche under the oracle. rcc-v2
  folds what rcc folds, with rcc's arithmetic, since its listing is
  rcc's. It drops an operator that its constant makes the identity
  where the node would be built, and rcc drops it in a pass after the
  body; the two agree, since the operand kept is never a constant.
- There is no stage. A node's type holds its operands' types, so an
  operand changed late changes what every node above should have
  been. A stage is a rebuild, never a patch.
- No fold is owed. The value is.

### 4.4 Statements

Statements have a handle of their own, `stmt_t`, by the same
technique.

- Expressions are parts of statements, not their equals. Every
  statement production names its expressions as parts, each in a
  place with a role, and the role is the entry point the statement's
  row calls there. No expression production names a statement, but
  for `({ })`.
- A production, its parser function and its node are one thing spelt
  three times: `IF '(' expr ')' stmt`, `selection_statement()`,
  `if_t< S >`. The condition is no parameter: it is erased, since
  its own row is its best jump (3.4).
- Statements are built as a tree, not written as they are parsed.
- The parser's function is the accumulator: its locals hold the parts
  while it reads on. It ends in one call, through the model of the
  part whose type the row needs, the other part erased.
- A constant condition is no special case, as in rcc: `while (1)`
  loads 1 and tests it, and `if (0)` jumps over a then part that is
  still written. A `for` that omits its condition writes no test:
  its condition is `omitted_t`, a parameter of the `for`.
- A fact the parser knows once it has read a part, and which picks
  the form of the row, is a type or a parameter: `if_t< S >` and
  `if_else_t< S >`; whether a `break` and a `continue` of a loop
  stand in its body, `do_t< broken, continued >` and `for_t< C,
  broken, continued >`. An omitted first or third clause of a `for`
  picks no form and is the null statement.
- An `if` whose then part is one jump, bare or the one item of a
  block, keeps the then part's type, `if_t< J >`. One jump is a
  category (principle 7): the types that have the row which names
  their label, a `break`, a `continue`, a `goto` and a `return`
  without a value. The builder asks the then part's model, and a
  block's asks its one item's, so a block in a block is not one
  jump. The row is the condition's jump to that label where the
  condition is nonzero; it asks the label before the condition's
  jump, since a `goto`'s label is taken where it is first met.
- A C label is a statement, `label_t`, placed where it is written,
  as a block item of its own, so it may stand before a declaration
  or a closing brace; a `goto` is `goto_t`, one jump. Where a
  statement is wanted, a label is a block of it and the next
  statement, which opens no scope and is no one jump.
- A block holds erased statements: their count is not bounded.

### 4.5 Names and declarations

*Working.* A declaration is the builder of its name's leaf.

- A scope maps a name to the built leaf, and keeps beside it what C
  says of the name: its C type, its linkage, whether it was declared
  `register`.
- Every question about a symbol is asked once, at its declaration: a
  register or a slot, by the target's count; the view and the sign.
  A use of the name is a copy of the leaf, and no row looks a name
  up.
- The storage is the function's frame, or the unit's data.
- A local's leaf is its place, `place_t< V, S >` over the where
  `slot_t`, its number among the function's locals, and not a
  displacement. The frame has one allocator: a local takes its
  view's bytes at its view's alignment where its declaration is
  generated, and a spill its 8 at an alignment of 8, each past every
  slot taken before it, and none is given back. A spill's slot falls
  between two locals', so the displacement is the state's: the
  declaration's row takes it, and a use reads it by the number.
- What runs is a statement, where it is written: `decl_t< V >` takes
  the slot, and `init_t< V >` takes it and stores, an initialization
  and not an assignment. The store is the value's own entry point
  (3.4), at the value's view, which is the local's.
- An assignment whose value is dropped is that same store. One whose
  value is used is the value held, then the store from its register.
- A parameter is a local of the body: its leaf is a place,
  `place_t< i32, signed_ >`, numbered by its position in the list,
  the function's first locals. A parameter of another type waits for
  the calls of every type. Where it lives is each target's, decided
  in the function's row before the body and recorded in the state
  under its number. On x86-64 the first six take a slot and the store
  of their argument register, and the seventh and later are where the
  caller pushed them, above `rbp`. On aarch64 each takes a slot, and
  the ninth and later are copied in once the frame's size is known.
  No row of the body asks whether a place is a parameter.
- A call's argument is a hidden local of its call, numbered by the
  parser, whose slot is taken where the call is generated.
- The file's scope holds each function's C type and its entry in the
  unit. The entry is made at the first declaration, so the unit
  writes its functions in that order, and it holds what the listing
  needs: the name, the linkage, the count of parameters, the body or
  none.
- A function's name has no node. Its value, the function's address,
  waits for a where of the unit's data; a call is built from the
  name alone.
- A `for` whose first clause is a declaration opens a scope before
  it, which its body's braces nest in. The declaration's statements
  are the `for`'s init, one or a block of several, so their slots
  are taken before the loop's labels.

### 4.6 Contexts and state

*Working.* There are two contexts, and the state.

- **The parser's, while the tree is built.** The names, the C types,
  and the loops around the parse. A loop has a number, given as a
  local's is, and the parser's function for a loop is the
  accumulator: when it has read the body it knows whether a `break`
  or a `continue` of the loop stands in it. `break` and `continue`
  are one type, `jump_t`, which names its loop's number and which of
  the loop's two labels. A loop's row takes its labels at rcc's
  moments and records the two under its number before the body;
  `jump_t`'s row reads them. A C label is a number the parser gives
  its name at its first meeting, a `goto` or its definition, and the
  state gives it a label at rcc's moment: the first of its
  definition, a `goto` to it, or an `if` whose then part is that
  `goto`. That every `goto` names a label the function defines is
  checked at the function's end. Generation holds nothing of the
  statements around the one generated. This stands on each statement
  being generated once.
- **An expression's, at generation.** A wanted register, a jump's
  target and sense: parameters of an entry point, set by the parent
  for each child.
- **The state.** What the registers hold at this moment, which
  values are spilled and where, and which are held across the paths
  of `&&` or `||`; the registers left for last while an operand is
  generated, and on x86-64 those an instruction generated later
  would move a held value out of; the labels, each loop's two by its
  number and each C label's once met, and whether a path reaches
  what is written next; on x86-64 the width at which the last
  instruction set ZF from its value, and whether the function calls,
  which moves `rsp` below any frame; the frame. A value held across
  a call is state: the call's row spills every held value at its
  start and asks the tree nothing. The state holds no width of a
  value: what a held value's register is valid to is its node's
  summary (4.2).

### 4.7 Two targets

*Working.* The types are open and the operations closed, and a target
is a set of operations. So the targets are named in the handle's
interface, each entry point of each target in a line of its own.

```c++
struct expr_t::interface_t
{
    virtual value_t value(amd64::state_t &, unsigned wanted) const = 0;
    virtual value_t value(arm64::state_t &, unsigned wanted) const = 0;

    virtual void initialize(amd64::state_t &, slot_t where) const = 0;
    virtual void initialize(arm64::state_t &, slot_t where) const = 0;

    virtual void branch(amd64::state_t &, int label, bool sense) const = 0;
    virtual void branch(arm64::state_t &, int label, bool sense) const = 0;

    virtual void
    value_branch(amd64::state_t &, int label, bool sense) const = 0;
    virtual unsigned pinned_within(const amd64::state_t &) const = 0;
    virtual unsigned moved_within(const amd64::state_t &) const = 0;

    // the summaries both targets read
    virtual int valid(sign_t) const = 0;
    virtual view_t view() const = 0;

    // the builders, which name no target
};
```

- A target's rows are in a namespace of its own, `amd64` and `arm64`,
  and are found by overload on the target's state type, through
  argument-dependent lookup.
- A row that both targets write alike is written once, as a template
  over the state.
- What one target alone asks of a node is a line of that target:
  x86-64's jump on the value compared with 0, and its two summaries,
  the registers the instructions within a subtree pin and those a
  pinned instruction would move a held value out of, which is none
  below `&&` and `||` and none within a call. aarch64, which pins
  none, has no such line.

- The zoo is the union of the shapes either target wants. A shape one
  target has no niche for falls to that target's primary: `a + b` in
  memory is x86-64's niche, `a + (b << 2)` is aarch64's.
- A type with no row on one target does not build.

### 4.8 The sources

`src/v2` shares the driver, the lexer and the C types (`type.hh`)
with rcc, and nothing else.

| Files | What they are |
| --- | --- |
| `parse.hh`, `parse-*.cc` | the parser, one file for each clause of the grammar |
| `build.hh` | the builders, declared: all the parser sees of the zoo |
| `build-typed.hh` | the overloads on the node types that a builder reaches through a model |
| `expr.hh`, `stmt.hh` | the handles |
| `operators.hh` | the closed domains that the parser and the zoo share, the operators and their families, the views and the signs, and the crossing of each from a value to a parameter |
| `zoo.hh` | the node types under the handles, and the two summaries both targets read, a value's view and the width its register is valid to |
| `unit.hh` | the unit and its functions, under no handle: each one's name, linkage, count of parameters, and body or none |
| `model.hh`, `build.cc` | the models, and the one unit that instantiates them |
| `rows-expressions.hh`, `rows-statements.hh` | the rows both targets write alike: the primaries of an effect and of a jump, `!`, `&&` and `\|\|`, and the statements |
| `register-stack.hh` | which register or spill slot holds which value, and the values held across a branch, as templates over a target's state |
| `amd64.*`, `arm64.*` | a target: its registers, its state, its rows, its frame |
| `assembly.*` | the text both targets write alike |
| `generate.*`, `pipeline.cc` | the back end's entry, and the pipeline |

### 4.9 The questions rcc's rows ask

`designs.md` 3.2 lists the questions a row of rcc asks of its operand
before it generates it. For those the tree has reached, this is what
each became.

| rcc asks | The typed tree |
| --- | --- |
| `immediate()`, `memory_operand()` of an operand | the operand's class, and a niche on it: `binary_t< op, V, L, I >`, `binary_t< op, V, L, load_t< V, S > >` |
| `multiplicand()` | a niche on the left operand's class, `binary_t< mul, V, load_t< V, S >, I >` |
| `constant_first()`, `ordered()` | a niche on a constant on the left, which is the row of the node with its operands exchanged |
| whether an operator is a comparison, a division, a shift | a concept on the operator, which constrains the rows of its family |
| `pins()` | a constant of the operator |
| `pinned_by()` | the right operand's class, `top_t< F >`, kept through a conversion |
| `pinned_within()`, `moved_within()` | summaries, handed up through the handle, a line of x86-64 each; a call pins the result register in the first and moves nothing in the second |
| `pinned()` in `rvalue()` | the row of a node whose instruction pins its result's register clears the reservation itself: the divisions', the call's |
| `immediate()` in `initialize()` | the store's entry point, by overload on the value's type |
| the width of a store in `initialize()` | the value's own view |
| `integer(t).width`, `operation_width()` | the node's view, a parameter |
| `integer(t).is_signed` | the operator, by its name; a widening, `sext_t` or `zext_t`; a place's narrow read |
| the width of `extended()`, `valid` | a summary, `valid_of()`, read by a widening's primary and aarch64's test of zero |
| `widened_read()`, `object_under()`, `under_kept_bits()` | the conversion's builder, by overload on the operand's type |
| `extending_load()` | a niche of `sext_t` over a load |
| whether a callee returns `void`, its result's width | the call's node: `call_void_t`, or `call_t< V >` and its view |
| `flag_operator()`, `flag_condition()` | x86-64's `value_branch` rows of those node types |
| `zero_compared()` | the operand's own entry point, `value_branch` |
| `body_target()`, `target_of()`, an `if` of one jump | the category of the types that name their label, and the then part's type, `if_t< J >`, which the builder finds through the model |
| `user_label()` | the state's labels of the C labels met, taken by a `goto`'s `label_of()` and by the label's own row |
| a `for` without a condition, a loop broken or continued | parameters of the loop's node |
| the loops around a statement | the parser's; a loop's two labels are in the state, by its number |
| `arithmetic_immediate()`, `bitmask_immediate()` | an `if` in the row: one target's encoding of its own leaf (principle 2) |
| `wanted_left()`, `held_out()`, `right_written()`, `written_in_right()` | rcc's functions, unchanged: they read the state and the summaries |

- The questions about an operand's form are answered by its type, and
  a row cannot disagree with the type that chose it.
- The questions about the registers are rcc's, copied. A type does
  not say which register is free.
- The questions of a register variable, `lands_in()` to
  `joins_into()`, have no answer here yet: the tree has no register
  variable. Neither have those of a call through a pointer, a
  variadic callee and a register parameter.

## 5. What the models showed

The models are small programs kept outside the tree: one target each,
rows invented for the purpose.

- A parser that holds only erased operands builds `add_t< expr_t,
  literal_t >`, a member of a call and an assignment to a register
  variable, through the model.
- Seven programs with loops, jumps and declarations, each built with
  niches, with primaries only and with no leaf rows, assembled and
  run on aarch64: 21 results of 21 are clang's.
- `*((int *)&p) = v` has the shape of rcc's listing, a load and a
  store to the slot. Its primary is one instruction longer. The same
  store through a pointer held in `p` is rcc's listing, instruction
  for instruction.
- A builder that does not erase its children stops the compiler at
  its instantiation depth.
- One zoo and one parser serve two targets, each with a niche the
  other lacks. A row left out on one target stops the build.
- A statement as an operand does not build.
- A fold written as an overload, and a tree built in two stages,
  write the listings of one stage.
- Jumps resolved by the parser write the instructions of jumps
  resolved at generation.
- The integer types: a parser of C types as data builds the nodes of
  `+`, `-` and `/` at two views, with a sign only in the division's
  name, crossing the operator and the view once and the right
  operand's class as data, conversions over loads and over computed
  values, and stores through the value's entry point at the value's
  own view. Twenty-seven bodies of rcc's, among them every case of
  the width rule through an assignment, a change of sign and a
  promotion, are its bodies, byte for byte.

## 6. Open

- The categories' cut (3.5).
- The builder on both operands at the size of all of C's operators.
  Twenty-five are built at two views, over the classes of the integer
  types on each side (a constant, the constant 0, a constant past 32
  bits, a load of each sign, anything else) and one more on the
  right (an operand whose top operator pins), with one crossing each
  from the operator's and the view's value to the node's parameters.
  Each class kept on the right is one more operation of the
  interface.
- What the zoo costs the compiler. Principle 5 counts code, and a
  primary costs none; but every node type a builder can make is
  instantiated in `build.cc` with its rows of both targets, about
  2,500 binary node types now. With every builder's crossings
  instantiated for every model, and a `shared_ptr` of each model's
  own type, that unit took over two minutes and 12 GB; with the
  crossings instantiated once for each class of operand, and one
  `shared_ptr` for every model, it takes 40 seconds and 4 GB. What
  remains is a model's own functions, and the instantiation nests as
  deep as the longest chain of node types each first met inside the
  one before it, about a hundred. Globals, pointers and the calls of
  every type each multiply the classes again.
- The register state at a join. Whether an arm of `?:` leaves its
  value where the other arm's does is, in its hardest case, a fact of
  one moment of generation. No type, summary or context has it. Three
  forms answer it: a row that states the fact for its target; the
  join always, which moves listings; the second arm generated aside
  and read before the first is finished.
- `({ })`. It waits until the concepts around it are clear.
- Two targets that want shapes of one subtree that do not nest. The
  address family is where to look.
- Whether every question rcc's generators ask of a C type moves to a
  builder (principle 9). For the integer types every one does: the
  view, the operator's sign, the width rule. Pointers, records and
  floating point are not asked yet.
- Of a jump's record, the variable length arrays a jump gives back.
  Whether a loop's label is jumped to is the parser's fact, a
  parameter of the loop's node; whether a path reaches a statement's
  end is the state's.
- The summaries of a subtree: taken as rcc has them, not designed
  here. The two, the registers pinned within and those that would
  move a held value, are computed again by each node that asks, as
  rcc computes them.
- x86-64's record of ZF. In every row the tree has, the instruction
  that sets ZF is the row's last, so the `cmp` the record guards is
  never written.
- Records by value, and floating point as a register class.
- aarch64's forms through `x16`, for a slot, a parameter or an
  outgoing argument past the reach of its access.
- The way back to rcc's pinned listings. `make sides` measures it; it
  is not sized.

## 7. Lineage

- The Seventh Edition compiler: its code tables and `match()`
  (`ancestor.md`, `designs.md` chapter 4); its fold in the first
  pass, there "to allow constant expressions to be used in switches
  and array bounds", the rest left to the next pass; its `break` and
  `continue` labels, saved in the parser function's locals.
- GCC's grammar, 2.95.3 and 3.4.6: the productions that compose
  statements of expressions. The same grammar writes code as it
  parses in the first and builds statement trees in the second.
- LLVM's form at no optimization: a store of an `i32` through a
  pointer with no pointee type; a load of an `i8` and an extension
  over it; a signed and an unsigned division of the same loads.
- A parser that calls builders, and gets back what can be run and
  not inspected, is known in the literature as tagless final, or as
  object algebras. Its weakness is this design's, no matching on a
  built term, and so is its remedy: more handed up, an argument
  handed down. Said from memory; the papers were not read for this
  page.
- The dialect is C++20: concepts, a constrained constructor,
  `if constexpr`.
