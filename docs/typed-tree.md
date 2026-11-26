# rcc: the typed tree

The typed tree is a second design of rcc's tree and of its
generators. It grows in `src/v2` and is built as `rcc-v2`, beside the
compiler `backend.md` describes, which stays the reference. This page
states the design: the idea, its principles, its idioms and its
architecture. It leaves the details to the code.

Of the design the tree has a thin line through every joint: functions
whose locals are `int`, declared, assigned and returned, with
parentheses, the unary operators and `+ - & | ^`, on both targets.
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
template< binary_op_t op, typename L = expr_t, typename R = expr_t >
struct binary_t { L lhs; R rhs; };

// the primary: asks nothing
template< binary_op_t op, typename L, typename R >
void into(state_t &, const binary_t< op, L, R > &, reg_t);

// a niche: addl $5, %eax
template< binary_op_t op, typename L >
void into(state_t &, const binary_t< op, L, literal_t > &, reg_t);
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
   `zero_t`, or `bit_t` for a constant of one bit. Only a question
   about the context or the state stays an `if`, and one more: a
   question of one target's encoding about a row's own leaf, whether
   aarch64's `add` holds its constant, which no builder can ask,
   since a builder names no target. Overloads and specializations are
   how conditionals leave the generator.
3. **A fact lives where its distance puts it.**

   | The fact | Its distance | Where it lives |
   | --- | --- | --- |
   | "my right operand is a constant" | bounded: an edge or two | the type |
   | "this subtree names `r`" | unbounded, below | the summary, handed up |
   | "the value is wanted in `%ebx`" | unbounded, above | the context, handed down |
   | "which register is free now" | a moment of generation | the state |

4. **The context carries only what a type cannot.** The form wanted
   is not context: value, effect, jump and address are the entry
   point a parent calls. The context is a destination, and a jump's
   target and sense.
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
   row needs into the node: a stride, a view, an operator's sign. The
   C type is the parser's, and it picks the next builder.
10. **An interface is whole.** Every type under a handle has a
    meaningful body for every operation of that handle. By this test
    statements and expressions are two handles, and two targets are
    one.
11. **Introspection has one address: the builders.** `is< >` and
    `as< >` are not ruled out. They belong where data becomes a type,
    in a builder or in the parser. A row that uses one on a child is
    asking an operand's kind again.
12. **Names are found, not invented.** LLVM's conventions are the
    first place to look: the views are `i8` to `i64`.

## 3. Idioms

### 3.1 The handle

The handle is value-semantic type erasure: an interface, a model
template, and free functions.

```c++
class expr_t
{
    struct interface_t                    // the operations: closed
    {
        virtual reg_t value(state_t &) const = 0;
        virtual expr_t member(long offset) const = 0;
    };

    template< typename T >                // the types: open
    struct model_t final : interface_t
    {
        reg_t value(state_t &s) const override
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
- A type exists only where a builder makes it. The zoo is as fine as
  the builders are.

### 3.4 Entry points are roles

A parent asks a child for a form by the entry point it calls: its
value, its effect, a jump to a label on a sense, its address, its
value in a destination. The choice is static. The first three are
the ancestor's `regtab`, `efftab` and `cctab`.

- The other direction is derived: an effect is a value dropped, a
  jump on a value is the value compared with 0.
- *Working:* the value in a destination, `into`, is the entry point
  with no primary: every type has a row that writes its value in the
  register it is handed (principle 10). A value is then a register
  taken and `into`. A `return` asks for its value in the result
  register.
- The caller owns that register and holds it in the register stack
  while the value is generated. So an operator computes its left
  operand there and operates in place, and no operand of it is taken
  in the same register.
- An entry point carries a child's best form to a parent that does
  not know the child's type. In `if (x & 8) break;` the statement is
  `if_t< expr_t, jump_t >`, and the `tbnz` is the condition's own
  `branch` row. No `if_t< and_t< ... >, jump_t >` is needed.
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
| truth | a native `branch` | comparisons, `!`, `&&`, `\|\|` |

*Working:* the cut is a first one. C's own sorts (a place, a value, a
function designator, void), how the machine names an operand, and its
register class are separate axes.

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

A place is a *where* and a *view*: `place_t< W, V >`.

- The where is how the machine names the object: `slot_t`,
  `regvar_t`, a global, `indirect_t< P >` through a pointer's value.
- The view is how it is read, and it is the width alone: `i8`, `i16`,
  `i32`, `i64`.
- `&` takes the view off, `address_t< W >`. `*` puts one on, read
  from the pointer's C type. `*` over an address is the place again;
  its primary goes through the pointer's value.
- A cast between pointer types writes no node. It is the same node
  under another C type.
- No register has a sign; instructions have. So the sign is the
  operator's, and a conversion's over a load. A load that extends
  from memory is a niche: the conversion over a load. A cast that
  changes only the sign writes nothing.
- A cast that changes a value is a node around its operand. It hides
  the operand from the parent's niches, so its builder folds a
  literal.
- `load_t< W, V >` is what a place holds, and it is no place. An
  lvalue is a matter of type, so an identity is dropped where it is
  built.
- The parser converts every operand whose value it reads, by a
  builder through the model, `rvalue()`: a place becomes its load,
  and anything else stays the node it is. So a place asked for its
  value is a tree the parser does not make, and a place's `into`
  stops the generation.
- The tree has one where and one view so far, a local of type `int`:
  `slot_t` and `load_t` take no parameter until a second arrives.

### 4.3 Constants

Three things are kept apart: the value, the fold and a stage.

- The value is asked: `value_of()`, a free function, each node's from
  its operands'. A case label and an array bound need it early, and
  will ask it through the model. They do not need the fold. So far it
  is written for the two constant types, and a builder that folds
  asks it.
- The fold is an overload on the operands' types, beside the other
  builders. It comes off with every niche under the oracle. rcc-v2
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
- A production, its parser function and its type's parameter list are
  one thing spelt three times: `IF '(' expr ')' stmt`,
  `selection_statement()`, `if_t< C, S >`.
- Statements are built as a tree, not written as they are parsed.
- The parser's function is the accumulator: its locals hold the parts
  while it reads on. It ends in one call, through the model of the
  part whose type the row needs, the other part erased.
- A constant condition needs no statement type. It is the constant's
  own `branch` row under the primary loop: `while (1)` writes no
  test, and `if (0)` jumps over a body that is still written.
- A block holds erased statements: their count is not bounded.

### 4.5 Names and declarations

*Working.* A declaration is the builder of its name's leaf.

- A scope maps a name to the built leaf, and keeps beside it what C
  says of the name: its C type, its linkage, whether it was declared
  `register`.
- Every question about a symbol is asked once, at its declaration: a
  register or a slot, by the target's count; the width. A use of the
  name is a copy of the leaf, and no row looks a name up.
- The storage is the function's frame, or the unit's data.
- A local's leaf is its number among the function's locals,
  `slot_t`, and not a displacement. rcc takes a slot where the
  declaration is generated, and a spill's slot falls between two
  locals', so the displacement is the state's: the declaration's row
  takes it, and a use reads it by the number.
- What runs is a statement, where it is written: `decl_t` takes the
  slot, and `init_t< V >` takes it and stores, an initialization and
  not an assignment.
- An initializer's store is rcc's `initialize()`: its primary is the
  value in a register, stored and released, and a target's niche
  stores a constant with no register held. The value's class is
  asked once, in its model: a constant, the constant 0, or anything
  else, erased.
- An assignment whose value is dropped is that same store, so the
  niches serve both. One whose value is used is the value in the
  destination, then the store from it.

### 4.6 Contexts and state

*Working.* There are two contexts, and the state.

- **The parser's, while the tree is built.** The names, the C types,
  and the loops around the parse. A `break` is a jump to a label the
  parser named: `break` and `continue` are one type, `jump_t`, and
  generation holds nothing of the statements around the one
  generated. This stands on each statement being generated once.
- **An expression's, at generation.** A destination, a jump's target
  and sense: parameters of an entry point, set by the parent for each
  child.
- **The state.** What the registers hold at this moment, the labels,
  the frame. A value held across a call is state. The call's row
  saves what the state holds and asks the tree nothing.

### 4.7 Two targets

*Working.* The types are open and the operations closed, and a target
is a set of operations. So the targets are named in the handle's
interface, each entry point of each target in a line of its own.

```c++
struct expr_t::interface_t
{
    virtual void into(amd64::state_t &, amd64::reg_t) const = 0;
    virtual void into(arm64::state_t &, arm64::reg_t) const = 0;

    // the builders, which name no target
};
```

- A target's rows are in a namespace of its own, `amd64` and `arm64`,
  and are found by overload on the target's state type, through
  argument-dependent lookup.
- A row that both targets write alike is written once, as a template
  over the state.

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
| `operators.hh` | the operators, a closed domain that the parser and the zoo share |
| `zoo.hh` | the node types under the handles |
| `unit.hh` | the unit and its functions, under no handle |
| `model.hh`, `build.cc` | the models, and the one unit that instantiates them |
| `rows-expressions.hh`, `rows-statements.hh` | the rows both targets write alike |
| `register-stack.hh` | which register holds which value, as templates over a target's state |
| `amd64.*`, `arm64.*` | a target: its registers, its state, its rows, its frame |
| `assembly.*` | the text both targets write alike |
| `generate.*`, `pipeline.cc` | the back end's entry, and the pipeline |

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

## 6. Open

- The categories' cut (3.5).
- The builder on both operands at the size of all of C's operators.
  Five are built, over three classes of left operand and four of
  right, with one crossing from the operator's value to the node's
  parameter.
- The register state at a join. Whether an arm of `?:` leaves its
  value where the other arm's does is, in its hardest case, a fact of
  one moment of generation. No type, summary or context has it. Three
  forms answer it: a row that states the fact for its target; the
  join always, which moves listings; the second arm generated aside
  and read before the first is finished.
- `({ })`. It waits until the concepts around it are clear.
- A conversion of a value, and a narrow load as a conversion over a
  load. No model has them.
- Two targets that want shapes of one subtree that do not nest. The
  address family is where to look.
- Whether every question rcc's generators ask of a C type moves to a
  builder (principle 9).
- Of a jump's record: whether a label was jumped to, and the variable
  length arrays a jump gives back. Whether a path reaches a
  statement's end.
- The summary of a subtree: taken as rcc has it, not designed here.
- Records by value, and floating point as a register class.
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
