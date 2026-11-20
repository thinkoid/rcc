# rcc: organizing designs for c1

c1, rcc's code generator, is two generators written by hand, one for
x86-64 and one for aarch64, that walk a typed tree once. For some
weeks the work on it was making listings an instruction shorter, one
shape at a time, and each step left edge cases behind it. A survey
asked whether an organizing design taken from another implementation
would end that. It assessed four designs, each against the tree and
against its own sources, and argued the case against each first: the
Seventh Edition compiler's code tables, destination-driven code
generation, delayed code generation through operand descriptors, and
selection as a pass of its own. This page explains each design from
the ground up: the design in its source, quoted at file and line or at
page; a small example of it at work where it was born; the case
against it for this compiler; what of it c1 already has; and the part
worth taking. Then the count of four days' work under each, three rows
written four ways, the promises, the roadmap, the migration, and how
the designs fit together. Nothing here is decided: c1 is as
`backend.md` describes it. The page is written as a tutorial, in the
register of `ancestor.md` and `findings.md`, for a reader who knows C
and has seen assembly but has not read a table-driven generator, the
1990 paper, Wirth's compilers, tcc or lcc. Chapter 14 says where the
sources came from and under what terms they are quoted.

## 0. tl;dr

- The question: is there an organizing design, taken from another
  implementation, under which c1's edge cases disappear by
  construction and the features still to come land as additions, not
  as one more parameter threaded through the rows?
- The finding the assessments share: most edges do not come from how a
  destination is handed down. They come from questions a row asks
  about its operand before it generates it. Each question is a
  predicate written by hand that mirrors what another function will
  do, and an edge is where the two disagree. x86-64 has 27 such
  predicates by one count and 21 by another, aarch64 1 and 6. A
  two-address instruction writes its destination with its first
  instruction, a three-address one with its last (chapter 3).
- Tables, the Seventh Edition compiler's code tables and their
  descendants in pcc and lcc: take three parts and leave the
  mechanism. The parts are the destination as the one argument of the
  code that writes a row, a result declared for each row, and a pure
  match over one operand classifier per target. Left: the template
  language, the order by degree, and covering by cost (chapter 4).
- Destinations, the destination-driven code generation of Dybvig, Hieb
  and Butler: c1 has the design already, in pieces. Take one mandatory
  destination type with one delivery step per target; leave the
  paper's rows and its threading of jumps (chapter 5).
- Operands, Wirth's items and tcc's value stack: G13 forbids the delay
  that pays in both sources. Take the address descriptor with its two
  operations, one operand descriptor as the return of every row, and
  the rule that a result stays where its instruction left it; leave
  delay (chapter 6).
- Selection as a pass of its own, which decides each node's form
  before any instruction is written: take a part, not the strong form.
  763 of the generators' 828 branch sites are decided from the tree,
  and 34 need to know which registers are occupied, which a pass could
  know only by a second copy of the register stack. Take a selection
  step for conditions, then addresses, then the conditional's
  attributes, with each form's facts declared and the record
  printable; leave "generation asks nothing" (chapter 7).
- The count, over the 46 commits that changed the generators from
  2026-11-15 to 4b95569 and the 11 entries of the queue's chapter on
  the back end: 8, 6 and 13 of the 57 items are unnecessary by
  construction under tables, destinations and operands; 22, 45 and 39
  are the same work as now; none is harder. Selection, counted over
  the 51 commits to d0b66bd and the 10 entries then, finds 7 of 61
  unnecessary and 46 the same work. 18 of the 53 items all four
  classify are the same work under all four (chapter 8).
- Each assessment found its design a component and not a rival, not
  worth a rewrite, and reachable in place, a step at a time, against
  the pins. The pass of selection is where the tree halves of the
  other three's kept parts meet, the destination and the operand's
  form; their register halves meet at emission, in one delivery step.
  What the pass adds of its own is the kept record (chapter 13).

## 1. Background: how to read what follows

### 1.1 c1 in a page

rcc's parser, `src/parse.cc`, builds a typed tree of each function. c1
walks the tree once and writes assembly text, which the system cc
assembles and links. There is no intermediate form and no pass after
c1. The two generators, `src/c1-x86-64.cc` and `src/c1-aarch64.cc`,
share one design and the sources `src/c1-common.*`, `src/c1-stack.hh`
and `src/c1-statements.hh`.

`backend.md` 6.1 states the rule for an expression
(`docs/backend.md:580-588`):

> An expression is generated for its value into a register, left to
> right, by one recursive walk of the tree. For a binary node: the
> left child is generated into register `L`; the right child is
> generated as the instruction's source operand `R`, in one of three
> forms: the immediate of a constant the instruction takes (6.4), the
> operand of an object in memory (below), or a register, into which it
> is generated; the instruction for the operator is written as `op R,
> L`; `R` is freed; the result is `L`, but for a remainder, which is
> the value in `rdx` (6.5).

The registers follow Ritchie's model. Every local lives in its stack
slot, a `register` variable in a callee-saved register by the
programmer's word, and an expression in the remaining registers, used
as a stack: a value takes the lowest free one. Nothing allocates
registers across statements.

Three rules bind any design:

- **Literal.** "The work the source writes is the work executed, in
  the written order: `x / 4` divides, `x * 4` multiplies, a
  loop-invariant is computed every iteration" (`README.md:18-20`).
- **G13, left to right.** "Operands and arguments are evaluated left
  to right, and an operand's side effects complete before the next
  operand starts" (`docs/guarantees.md:106-108`).
- **G10, memory is bytes.** "A load through any lvalue reads the bytes
  at the address as the lvalue's type. Instruction: the same `mov`, at
  the lvalue's width and extension" (`docs/guarantees.md:88-90`).
  Below, "G10" means that each access is made at its own width.

The *pins* are the listings the tree keeps for its test programs, and
every change is diffed against them (`backend.md` chapter 12). At
4b95569 there are, for each target, 110 cost programs and 25 programs
of the oracle's corpus, each with its pinned listing, and 15 probes,
which run rcc's code against clang's. At d0b66bd the cost programs are
111, and the pinned listings of both targets 272. A change meant to
move no listing is *inert*, and a diff of every listing against the
base compiler's proves it.

### 1.2 The terms

- A *row* is the code c1 writes for one operator in one form. In the
  tree a row is a function of a generator, or a case of one,
  `binary()` for `+` and its kin on x86-64 (`src/c1-x86-64.cc:3163`),
  and a line of the tables of `backend.md` 6.3.
- A *destination* is where a value should go: a register, a register
  variable, or a jump.
- A *hand-down* is a parameter that carries a destination from a row
  to the rows below it, before they are generated.
- A *predicate* is a question a row asks about its operand's subtree
  before it generates the operand.
- An *edge* is a shape where a predicate and the row it mirrors
  disagree, or where a predicate stops short, so that the listing is
  longer than the rule promises. The queue, `TODO`, holds each edge
  found as an entry.
- A *two-address* instruction writes one of its operands, `addl
  -8(%rbp), %eax` on x86-64; a *three-address* one names its
  destination apart from its sources, `add w0, w0, w1` on aarch64.

### 1.3 Reading this page

- The first three assessments read the tree at master 4b95569, the
  fourth at d0b66bd, five generator commits later. This page cites the
  tree at d0b66bd: every `file:line` of `src/`, `docs/`, `tests/` and
  `TODO` is at d0b66bd. A citation of an older text names its commit
  as `git show` takes it, `4b95569:TODO:37-38`.
- A count of the first three is at 4b95569, and the text says so; the
  fourth's are at d0b66bd. A count made again for this page says
  where.
- The queue, `TODO`, is written in org's markup; quoted here, its
  `=code=` and `~code~` are in backticks.
- rcc's listings are what `rcc -S` writes at d0b66bd, with `-target
  x86-64` or `-target aarch64`, cut to the construct's own lines. An
  earlier listing is a pin at the commit named.
- PDP-11 listings are what the Seventh Edition compiler writes, its
  own binaries run under a PDP-11 emulator.
- A *sketch* is C++ written for an assessment in rcc's vocabulary,
  with new names where the design needs them. No sketch was compiled.
  Each is given as the assessment wrote it and marked as a sketch.
- The Seventh Edition sources are quoted at file and line in
  `usr/src/cmd/c/`, the paper at its page, and Oberon, tcc, lcc and
  pcc at file and line in the copies chapter 14 names.
- Each assessment classified the work of four days under its design;
  chapter 8 has the classes. A class is a judgment of what a design
  nobody has built would do, and each assessment said so.

## 2. The problem, shown

### 2.1 A row and where its value goes

On x86-64 the row of `+`, `-`, `&`, `|` and `^` is the tail of
`binary()` (`src/c1-x86-64.cc:3205-3217`). It generates the left
operand into a register, then the right operand as an immediate, an
object in memory or a register, and hands both to `arithmetic()`,
which writes `op R, L` (`src/c1-x86-64.cc:1842-1906`). The instruction
writes `L`, so the register the left operand lands in is the register
the result is in. For `return a + b` on two parameters:

```
	movl	-4(%rbp), %eax
	addl	-8(%rbp), %eax
```

Where the left operand lands is the row's choice. Left to itself, the
register stack gives it the lowest free register. A consumer may want
the value somewhere else: a `return` in `rax`, a `register` variable
in its own register, a condition in the flags and not in any register.
Telling the row is a *hand-down*.

### 2.2 The three hand-downs of c1

c1 has three, each grown on its own:

- **`into_t`, a register variable as the destination**
  (`src/c1-common.hh:177-192`). An assignment to the variable, or its
  initializer, "hands the variable to the row at the top of the right
  operand, which computes the value in the variable's register where
  the target lets it and that leaves the register the variable's
  representation". The flag `taken` says the row did; otherwise the
  assignment moves the value in.
- **`wanted`, the register a consumer names.** "On x86-64 three
  consumers name one: a `register` variable names its own, the
  destination of the value assigned to it (chapter 4), a `return`
  names the result register, `rax` (7.2), and a shift names `rcx` for
  its count, read in `cl` by the instruction (6.5)"
  (`docs/backend.md:759-763`). It is a set of at most one register,
  and `wanted_register()` gives the register where a row may write it,
  "held by no value and left for last by no reservation"
  (`src/c1-x86-64.cc:461-477`).
- **`cbranch(e, label, sense)`, a condition wanted as a jump.** It
  "writes code that jumps to `label` when `e` is true (sense 1) or
  false (sense 0), and falls through otherwise, without producing a
  value" (`docs/backend.md:1482-1484`; `src/c1-x86-64.cc:4109-4179`).
  `effect()`, a value not wanted at all, is its sibling
  (`src/c1-x86-64.cc:4378`).

The first two are parameters of every row that might honour them. The
declaration of x86-64's entry point for a value reads
(`src/c1-x86-64.cc:635`):

```cpp
value_t
rvalue(state_t &, const expr_t &, into_t * = nullptr, unsigned wanted = 0);
```

Two more destinations travel in the generator's state, not as
parameters (`src/c1-x86-64.cc:129-135`):

- `s.in_place`, "the read of a compound assignment's register
  variable, which its operation writes in place";
- `s.reserved`, "the registers take() leaves for last while a value
  meant for a pinned register is generated, so it lands there".

Around them stands a family of helpers: `held_out()`, `wanted_left()`,
`pinned_within()`, `preserve()`, `lands_in()`, `ordered()` and others.
How far the threading reaches, counted at 4b95569:

- 52 of the x86-64 generator's 171 functions take `into_t` or
  `wanted`, and 12 take both; 30 of aarch64's 121 take one, and 16
  take both.
- 25 parameters of type `into_t *` and 15 named `wanted` are defaulted
  on x86-64, and 23 and 17 on aarch64, so a call that forgets one
  compiles.
- `in_place` is named on 11 code lines of the x86-64 generator and 6
  of aarch64's, `reserved` on 17 of x86-64's and none of aarch64's.

On aarch64 the two data destinations already meet in one function:
`final_destination()` gives a row's last instruction its register, the
variable's where the variable is handed down, the named register where
the consumer names one and the instruction may write it, and otherwise
a fresh one (`src/c1-aarch64.cc:557-578`). It has 13 call sites.

### 2.3 One edge, walked through

The cost program `conditional` has this function
(`tests/cost/conditional.c:119-123`):

```c
long
widened(int a, int b, int c)
{
	return a ? b % c : c;
}
```

The `return` names `rax`. Under it stands a cast that widens the `int`
conditional to `long`, and under the cast the conditional, whose first
arm is a remainder, left by `idivl` in `edx`. Before bb736a0 the name
went down through the cast, the arms joined in `eax`, and the cast
extended the join
(`bb736a0^:tests/cost/x86-64/conditional.s:134-144`):

```
	cmpl	$0, -4(%rbp)
	je	.Lwidened.2
	movl	-8(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	movl	%edx, %eax
	jmp	.Lwidened.3
.Lwidened.2:
	movl	-12(%rbp), %eax
.Lwidened.3:
	movslq	%eax, %rax
```

The queue said what was wrong, and why it stayed
(`4b95569:TODO:31-38`): a join left in `rdx` "would be extended into
`rax` by the one `movslq %edx, %rax`", and

> Fix: none taken; the cast would need to know, before its operand is
> generated, whether the operand's value is valid to the new width.

bb736a0 hands the conversion to each arm, so that each path writes
`rax` extended and nothing follows the join
(`tests/cost/x86-64/conditional.s:134-143`):

```
	cmpl	$0, -4(%rbp)
	je	.Lwidened.2
	movl	-8(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	movslq	%edx, %rax
	jmp	.Lwidened.3
.Lwidened.2:
	movslq	-12(%rbp), %rax
.Lwidened.3:
```

The cast decides before either arm is generated. `widens_arms()` asks
whether one arm at least widens with no instruction of its own
(`src/c1-x86-64.cc:3662-3699`), and `widens_free()` answers for an arm
(`src/c1-x86-64.cc:3642-3660`):

```cpp
bool
widens_free(const expr_t &arm,
            const symbol_t *variable,
            std::optional< reg_t > named)
{
    if (lands_in(arm, variable, true) || register_read(arm).variable)
        return true;

    return named && *named != rdx && is< binary_t >(arm) &&
           as< binary_t >(arm).op == binary_op_t::rem;
}
```

Each clause is a promise about code that has not run. `lands_in()`
promises what the rows of a constant, a load and a comparison will
write; the last clause promises that `divide()` leaves a remainder in
`rdx`. The edge was closed by two more predicates, and the rule has a
new edge, which `backend.md` 6.1 states (`docs/backend.md:798-799`):
"two quotients, `return a ? b / c : c / b`, keep the join in `eax` and
the one extension after it".

That is what an edge is in c1. A row that hands a destination down
asks a question about rows it has not yet run. The answer is code that
mirrors those rows. A shape the mirror does not cover keeps the longer
listing, nothing fails, and the shape waits until someone finds it.

## 3. Where the edges come from

### 3.1 The questions a row asks

Before a row hands a destination down, it needs facts about its
operand that only the operand's own row decides. They are of three
kinds:

- whether the operand's row will write the destination itself:
  `lands_in()` (`src/c1-x86-64.cc:2509-2541`), `loads_into()`
  (`src/c1-x86-64.cc:2812-2853`), `operates_in()`
  (`src/c1-x86-64.cc:2855-2884`);
- whether the operand pins a register, as a division pins `rax` and
  `rdx`: `pinned_within()` (`src/c1-x86-64.cc:546-591`);
- whether the operand writes or names the variable: `writes()`
  (`src/c1-common.cc:264`), `names()` (`src/c1-common.cc:361`),
  `leaf_apart_from()` (`src/c1-x86-64.cc:2732-2740`).

2.3 added a fourth: whether the operand's value is valid to a width.
Each predicate is written by hand from what the operand's rows do.
`loads_into()` says so in its comment, "Whether L of the operation
form goes into the variable's register by an instruction it has in any
case: its load, its constant, or again the operation form"
(`src/c1-x86-64.cc:2812-2814`), and its body is a list of those rows
(`src/c1-x86-64.cc:2830-2852`). When a row learns a new form, its
predicate must learn it too. When the predicate does not, the listing
is an instruction longer, it still assembles and runs, and no test
fails.

### 3.2 The predicates, counted

Three counts were made at 4b95569, each of a different set:

- The predicates a parent asks of a child before generating it, the
  three kinds of 3.1, counted by a list of 35 names over the
  generators: 27 on x86-64, in 360 lines, and 1 on aarch64, in 4
  lines. 4 more are in `src/c1-stack.hh` and 7 in `src/c1-common.cc`,
  shared by both.
- The pure functions of the tree that decide which form a row takes,
  selected by hand from a list of every function: 21 on x86-64 and 6
  on aarch64.
- The predicates that predict an operand's form from the tree before
  it is generated: 14, across the generators and the shared sources.

The first two counts share 19 functions on x86-64. The first also
counts the seven that ask which registers a subtree pins and which to
hold out, `held_out()`, the two `pins()`, `pinned_within()`,
`wanted_left()` and the two `pinned_by()`, and `register_read()`; the
second also counts `immediate()` and `multiplicand()`. On aarch64 the
second count's six are five operand classes, the immediates an
instruction encodes and the reach of a displacement, and
`joins_into()`, the first count's one.

The list, at d0b66bd. The columns 1, 2 and 3 mark the counts that hold
each function.

| Function | Where | What it asks | 1 | 2 | 3 |
|---|---|---|---|---|---|
| `held_out()` | `src/c1-x86-64.cc:489` | which registers the right operand is generated with left for last, so the result can take the named one | x | | |
| `pins()`, two | `src/c1-x86-64.cc:510`, `src/c1-x86-64.cc:528` | which registers an operator's instruction pins | x | | |
| `pinned_within()` | `src/c1-x86-64.cc:554` | which registers any instruction within a subtree pins, a call's `rax` included | x | | |
| `wanted_left()` | `src/c1-x86-64.cc:603` | whether the left operand may take the named register | x | | |
| `pinned_by()`, two | `src/c1-x86-64.cc:615`, `src/c1-x86-64.cc:629` | which registers the right operand's top operator pins | x | | |
| `immediate()` | `src/c1-x86-64.cc:1299` | whether a constant is an immediate the instruction takes | | x | |
| `memory_operand()` | `src/c1-x86-64.cc:1320` | which object in memory an instruction names as its own operand | x | x | x |
| `register_operand()` | `src/c1-x86-64.cc:1338` | which register variable's register an instruction names | x | x | x |
| `multiplies()` | `src/c1-x86-64.cc:1955` | whether pointer arithmetic multiplies its index by the size | x | x | |
| `scaled_sum()` | `src/c1-x86-64.cc:2109` | whether a pointer sum ends in a scaled `leaq` | x | x | |
| `lands_in()` | `src/c1-x86-64.cc:2518` | whether an arm's row writes the variable's register itself | x | x | x |
| `joins_into()` | `src/c1-x86-64.cc:2548`; `src/c1-aarch64.cc:2259` | whether each path of `?:` may write the variable | x | x | |
| `named_pointer()` | `src/c1-x86-64.cc:2686` | whether a pointer reaches its object by one operand that evaluates nothing | x | x | x |
| `leaf()` | `src/c1-x86-64.cc:2719` | whether the right operand is a leaf | x | x | |
| `leaf_apart_from()` | `src/c1-x86-64.cc:2737` | whether it is a leaf that does not name the variable | x | x | |
| `register_read()` | `src/c1-x86-64.cc:2759` | whether an operand is a register variable's read under conversions that write nothing | x | | x |
| `cast_loads_into()` | `src/c1-x86-64.cc:2789` | whether a cast as the left operand goes into the variable's register by its own row | x | x | x |
| `loads_into()` | `src/c1-x86-64.cc:2828` | whether the left operand goes into the variable's register by an instruction it has in any case | x | x | x |
| `operates_in()` | `src/c1-x86-64.cc:2865` | whether `r = L op R` is the operation on the variable's register | x | x | |
| `offsets_in()` | `src/c1-x86-64.cc:2918` | the same for pointer arithmetic | x | x | |
| `multiplicand()` | `src/c1-x86-64.cc:3076` | which object in memory a multiply by a constant names | | x | x |
| `negates_in()` | `src/c1-x86-64.cc:3372` | whether `-x` or `~x` is computed on the variable's register | x | x | |
| `pinned()` | `src/c1-x86-64.cc:3857` | whether an instruction pins the register of its own result | x | x | |
| `flag_operator()` | `src/c1-x86-64.cc:3931` | whether an operator's instruction sets ZF | x | x | |
| `flag_condition()` | `src/c1-x86-64.cc:3952` | whether a condition's top operator sets ZF from its value | x | x | x |
| `zero_compared()` | `src/c1-x86-64.cc:3983` | whether `e == 0` or `e != 0` can jump on `e`'s flags | x | x | x |
| `in_reach()` | `src/c1-aarch64.cc:301` | whether an access encodes a displacement | | x | |
| `is_scale()` | `src/c1-aarch64.cc:684` | whether a size is a scale of the addressing form | | x | |
| `in_frame_reach()` | `src/c1-aarch64.cc:1037` | whether a displacement off `x29` is reached | | x | |
| `arithmetic_immediate()` | `src/c1-aarch64.cc:1439` | whether `add`, `sub` and `cmp` encode a constant | | x | |
| `bitmask_immediate()` | `src/c1-aarch64.cc:1465` | whether `and`, `orr`, `eor` and `tst` encode a constant | | x | |
| `in_memory()` | `src/c1-common.cc:246` | whether an expression reads an object in memory | | | x |
| `object_under()`, two | `src/c1-common.cc:322`, `src/c1-common.cc:333` | which object in memory a conversion's operand reads | | | x |

The first count's list at 4b95569 also holds `widens_into()`. bb736a0
removed it and added `widens_free()` and `widens_arms()` (2.3), which
the list does not name; at d0b66bd it counts 26 on x86-64, and 28, in
380 lines, with the two added.

### 3.3 Two addresses and three

The difference between the targets is the machine's. A function from
`ancestor.md` 7.4:

```c
int
named(int a, int b)
{
	register int r = b;

	r = a - r;
	return r;
}
```

aarch64 writes the variable's register with the subtraction itself:

```
	ldr	w0, [x29, #24]
	sub	w19, w0, w19
```

x86-64 computes the difference elsewhere and moves it in, one
instruction more:

```
	movl	-12(%rbp), %eax
	subl	%ebx, %eax
	movl	%eax, %ebx
```

On x86-64, `subl %ebx, %eax` writes its left operand, so computing in
the variable's register means loading `a` into `ebx` first. That first
instruction destroys `r` before the `subl` reads it. Written by hand,
the listing that form would give is wrong:

```
	movl	-12(%rbp), %ebx
	subl	%ebx, %ebx
```

It computes `a - a`. So x86-64 must ask, before it hands the variable
down, whether the right operand reads the variable:
`leaf_apart_from()`, the Seventh Edition compiler's two conditions
(`src/c1-x86-64.cc:2732-2740`). aarch64's `sub w19, w0, w19` reads
both sources before it writes `w19`, so the question does not arise:
`final_destination()` names the destination at the last instruction,
after every operand is read, and "no condition on them is needed"
(`src/c1-aarch64.cc:562-564`).

The same holds for the register a consumer names. On x86-64 the left
operand holds the named register while the right operand is generated,
so the right operand must not land in it, and must not pin it:
`held_out()`, `wanted_left()`, `pinned_within()`. On aarch64 nothing
holds the destination before the last instruction.

So most of x86-64's predicates exist because a two-address instruction
writes its destination with its first instruction, and almost none of
aarch64's, because a three-address one writes it with its last.
aarch64 is not free of the hazard: a value held while another is
generated still needs its register kept clear. 5498bbf generates the
place of a store or a step outside `x0` on aarch64, and with it
`reserved` appears in the aarch64 generator, on 4 code lines at
d0b66bd where 4b95569 had none.

### 3.4 What any design must do with them

The finding binds every design. A design that changes only how a
destination is handed down carries the predicates unchanged: the
parent still has to know what the child will do. A design removes an
edge only where it makes the answer a fact computed once, which the
row cannot then contradict:

- a row declared as data and asked before it runs (tables, chapter 4);
- the operand's form observed after it is generated (operands, chapter
  6);
- a decision recorded beside the tree before generation (chapter 7),
  which its count finds possible for the facts of the tree and not for
  those of the registers.

## 4. Tables: the Seventh Edition compiler's code tables

### 4.1 The design in its source

`ancestor.md` 1.3 to 1.5 teaches the table's notation from the ground
up. What the design needs here is shorter.

The table is a file of assembler source, `table.s`, of 1601 lines,
which holds 332 operand patterns in 44 row lists. A *row* is one or
more patterns, each saying what the two operands may be, and a
*template*, the instructions to write with macro letters where the
operands and the registers go. The rows of `+` and its kin begin
(`table.s:274-287`):

```
/ +, -, |, &~, <<
cr40:
%n,z
	F

%n,1
	F
	I'	R

%[add1:]
%n,aw
%nf,ad
	F
	IB2	A2,R
```

A program, `cvopt`, compiles each pattern into a `struct optab`
(`c1.h:78-84`):

```c
struct	optab {
	char	tabdeg1;
	char	tabtyp1;
	char	tabdeg2;
	char	tabtyp2;
	char	*tabstring;
};
```

a *degree* and a type for each operand, and the template. A degree
measures how hard an operand is to reach, and a pattern's class letter
bounds it: `z` the constant 0, `c` a positive constant, `r` a register
variable, `a` an *addressable* operand, one an instruction names in
place, `e` an expression whose registers fit in those left, `n`
anything (`cvopt.c:204-244`; `dcalc()`, `c11.c:157-196`). `%n,aw` thus
reads "anything on the left, an addressable word on the right".

The machinery is four pieces:

- **`match()`** (`c10.c:86-153`) takes the first row of a list whose
  operands fit, by degree, `dcalc()`, and by type, `notcompat()`
  (`c11.c:198-223`). When nothing fits, it tries again with a
  conversion over an operand taken out (`c10.c:111-130`), so a row
  whose instruction converts as it reads takes the unconverted
  operand.
- **`rcexpr()` and `cexpr()`** (`c10.c:176-388`, `c10.c:399-798`)
  drive it, and carry the *register argument*: "If there is a value,
  it is desired that it appear in reg. The routine returns the
  register in which the value actually appears" (`c10.c:159-160`).
- **The four tables** (`c1.h:109-112`), each mapping operators to row
  lists: `regtab` for a value in a register, `efftab` for the effect
  alone, `cctab` for the condition codes, `sptab` for a value pushed.
  A table with no row falls back (`c10.c:350-370`): `cctab` to
  `regtab` and a `tst`, `sptab` to `regtab` and a push, `efftab` to
  `regtab`.
- **The expander**, the part of `cexpr()` that copies a template and
  compiles the operands its letters name. `F`, `S` and `H` compile the
  left operand, the right one or the whole tree, each with a table and
  a register (`c10.c:609-660`); `movreg()` writes nothing when the two
  registers are one (`c10.c:1184-1190`).

The descendants keep the rows and change the matching:

- pcc keeps one table for all contexts. A row of its amd64 table
  carries the contexts as bits, `INAREG`, `FOREFF` and `FORCC`, a
  shape for each operand, its *needs* (the registers its instruction
  pins), where its result is, and the template. The `int` division is
  a row whose needs are `NSPECIAL`, whose result is `RDEST`, and whose
  template is `cltd` and `idivl AR` (`pcc/table.c:1131-1135`). 27 of
  the table's 230 rows are `NSPECIAL`, and `nspecial()` lists their
  registers in C (`pcc/order.c:240-321`).
- lcc keeps one tree grammar per target over one shared emitter,
  `lcc/gen.c`: 306 rules for x86 under Linux and 183 for MIPS.
  `rewrite()` labels a tree with the grammar's labeller and then
  reduces it from the root (`lcc/gen.c:470-478`); the labeller is the
  C that `lburg` writes from the grammar.

### 4.2 The design at work where it was born

The Seventh Edition compiler writes, for three shapes of the dialect:

- `return(a + b)` on two external `int`s, by cr40's `%n,aw`: `F` loads
  the left operand into `R`, and the instruction names the right one
  in place.

  ```
  mov	_a,r0
  add	_b,r0
  ```

- `r = a + b` into a `register` variable in `r4`. `sreorder()`
  rewrites the assignment, by its comment (`c10.c:836-837`):
  "expressions like "reg = x+y" are best done as "reg = x; reg =+ y"
  (so long as "reg" and "y" are not the same!)". Each half is then an
  `efftab` row:

  ```
  mov	_a,r4
  add	_b,r4
  ```

- `if (p[i] & 1) break;` with `p` an external pointer and `i` a
  parameter. Under `cctab` the `&` is the PDP-11's `bit`, the and that
  sets the condition codes alone, by cc81, whose comment is `/ & as in
  "if ((a&b) ==0)"` (`table.s:1454-1460`):

  ```
  mov	4(r5),r0
  asl	r0
  add	_p,r0
  bit	$1,(r0)
  jne	L20
  ```

Each row is chosen by `match()` from the operands' classes, and its
template is the listing. The PDP-11 made that possible with few rows.
Its two-address instructions take a register, a name in memory or an
offset from a register on either side, and its `div` takes any
even-odd pair of registers, so `oddreg()` takes one above the
registers in use (`c11.c:278-310`) and nothing is in the way.

### 4.3 The case against, for this compiler

**The ancestor's tables are not neutral about order or literalness.**
The Seventh Edition compiler writes:

| Source | The Seventh Edition writes | What it breaks |
|---|---|---|
| `a + f(b)` | `mov _b,(sp); jsr pc,*$_f; add _a,r0` | G13: `f` runs before `a` is read |
| `a + b / c` | `mov _b,r1; sxt r0; div _c,r0; add _a,r0` | G13: the division comes before `a` |
| `x =+ 0;` | nothing | literalness: the store the source writes is left out, `ci70`'s empty body (`table.s:1098-1102`) |
| `return(p[i] = a);` | `mov _a,(r0); mov (r0),r0` | a read of the object the source did not write, cr80's `%n*,aw` (`table.s:202-205`) |

`x =+ 0` is the dialect's spelling of `x += 0`. The reorders are
`c12.c`'s: a comparison's operands swapped by degree (`c12.c:127-134`)
and a commutative chain sorted by degree (`c12.c:978-986`). The table
carries them too: the rows `%n,n` compile the right operand first,
`SS` before `F` (`table.s:310-315`). The degrees exist to feed the
table, and G13 excludes them as an order. What survives is the classes
and the templates.

**The PDP-11 made the table small and an allocator unnecessary.**
x86-64 pins `rax` and `rdx` for a division and `cl` for a shift's
count, where the PDP-11 pinned nothing. Both descendants put those
pins outside the pattern language, and satisfy them with a register
allocator:

- lcc's `target()` and `clobber()` are C code beside the grammar
  (`lcc/x86linux.md:780-848`). Its templates that begin `?movl %0,%c`
  rely on the allocator to choose the result register: the emitter
  skips the `movl` when the allocator gave the result its operand's
  register (`lcc/gen.c:347-352`, `lcc/gen.c:631-637`).
- pcc's 27 `NSPECIAL` rows feed its register allocator, a graph
  colourer (recalled).

The charter gives rcc no allocator, so its reservations, 201 lines
(`src/c1-x86-64.cc:459-659`), would stay as code under any table. On
x86-64 the table would hold the easy part.

**The queue's edges are where the ancestor had no table either.**
`rcexpr()` handles eight groups of operators in C before it consults a
table (`c10.c:214-326`): structure assignment, initializers, a
`return`'s `RFORCE`, the comma, `&~`, the call, long shifts and the
multiply. `cexpr()` handles a relational's value and `?:` in C
(`c10.c:420-443`). rcc's queue clusters in the same places: the joins
of `?:`, the moves of a `return`, `preserve()`, values held across
branches and calls. A table moves none of this into data.

**The descendants' matcher is excluded.** `lburg` covers a tree at
least cost, so it chooses among forms. lcc's grammar writes `x = x +
c` in the memory form, `addl` or `incl` on the object, at a cost of 3
wherever `memop()` finds the two addresses to be one tree, and at
`LBURG_MAX`, a cost no cover takes, elsewhere
(`lcc/x86linux.md:458-469`, `lcc/x86.md:759-769`). That is the
selector's judgment `findings.md` was written against. Only first-fit
matching is admissible, and first-fit makes the order of the rows a
priority list that nothing states.

**The macros are a second language, and it drifts.**

- The letters, `F`, `S1*`, `A1''`, `#1`, `R-`, are a notation to learn
  beside C++.
- The ancestor's own description of its classes disagrees with its
  code (`cvopt.c:34-39` against `cvopt.c:204-244`; `ancestor.md` 1.3).
- A missing row fails while the compiler runs, "No code table for op"
  (`c10.c:383-386`), where C++'s type checker cannot see it.

**What it would not have helped with.** Of the 46 commits of chapter
8, 16 are outside expression selection: statements, data, the parser,
the frame, errors, comments. 2 more are a normalization already in one
place. 3 of the 11 entries are outside what a row holds: `preserve()`,
a comment and the call sequence. Of the roadmap, initializers, structs
by value, variadic definitions and the call convention are outside any
table (chapter 11).

**What it costs.** A rebuild rewrites 4893 lines of the two generators
(chapter 12), and the register names in every pin depend on the order
in which a new expander takes registers.

The tree has made this call once. `ancestor.md` 10 keeps rcc's rows as
code (`docs/ancestor.md:2484-2488`): "`match()`, `cvopt` and the macro
interpreter, a matcher over rows held as data, are not taken." The
assessment confirms the call for the interpreter and argues for three
parts the call did not weigh.

### 4.4 What c1 already has

- **Rows as code.** Each generator's functions are its table, read
  against its page: `backend.md` 6.3 has 29 rows and `aarch64.md` 6.3
  has 28, counted at 4b95569.
- **Three of the four contexts.** `regtab`, `efftab` and `cctab` are
  `rvalue()`, `effect()` and `cbranch()` in each generator
  (`ancestor.md` 3.3). `sptab` has no counterpart, since an argument
  is a hidden local of its call.
- **The register argument, in two halves.** `into_t` and `wanted` are
  `cexpr()`'s register argument, split by consumer.
- **Operand classes, as predicates.** `immediate()`,
  `memory_operand()`, `register_operand()` and `named_pointer()` play
  the part of the classes `c`, `a` and `r`, one predicate at a time,
  and `leaf()` cites `dcalc()`'s degree 12 in its comment
  (`src/c1-x86-64.cc:2713-2717`).
- **Declared results, as code.** A row's pinned result, the width its
  result is valid to and whether it set ZF are kept by statements in
  each row: `wrote()` (`src/c1-x86-64.cc:730-736`), `flagged()`
  (`src/c1-x86-64.cc:297`) and the moves of `divide()`
  (`src/c1-x86-64.cc:1729-1756`).
- **`movreg()`'s rule, in one place of two.** `conditional()` writes
  nothing for a first arm already in the variable's register
  (`src/c1-x86-64.cc:2642`); `arm_into()` has no such check
  (`src/c1-x86-64.cc:2496-2507`), which is the queue's entry on `movl
  %ebx, %ebx`.

### 4.5 What to take, and what to leave

The assessment's verdict is "adopt this part", three parts:

1. **The destination as the expander's one argument.** One `dest_t`
   replaces `into_t` and `wanted`, and one step places the result for
   every row, so a row cannot choose its result register. This is
   `cexpr()`'s register argument, with the destination kept apart from
   the floor of the register stack, which the ancestor did not do. It
   is the mechanism behind 12 of the 16 items of class (a) by their
   main change.
2. **Declared result descriptors.** Each row declares its pinned
   result register, the width its result is valid to, and whether it
   sets ZF, as pcc's rows declare `RDEST` and their needs. These
   replace the bookkeeping of `wrote()`, `flagged()` and `divide()`'s
   moves, and account for the other four.
3. **One operand classifier per target, and a pure `match()` over
   it.** The predicates that mirror rows by hand become questions put
   to the table, which a consumer can ask before anything is
   generated. Two entries' own Fix lines ask for exactly that: the
   widened join of 2.3 at 4b95569, and at d0b66bd the conditional
   below a cast on aarch64, which "would need to know before its arms
   are generated whether each can write `x0`" (`TODO:48-49`).

Left:

- the template language and `cvopt`;
- the degrees as an order of evaluation;
- covering by cost;
- one table for two machines. The answer, as lcc and pcc have it, is
  one mechanism, two tables, and an operand rule per target: two
  addresses on x86-64, the destination written last on aarch64.

The assessment's confidence: about 65% that the three parts pay, and
about 85% that a rebuild around tables, interpreter and all, does not.
What would change it:

- if the first step (12.1) grows `cbranch()` instead of shrinking it,
  or does not stay inert, part 3 goes;
- how much destination code remains after `dest_t` lands on one target
  measures what part 1 buys;
- if the integer rows take floating point by a type column, as the
  ancestor's do, rows as data are worth more than the count shows.

Beside destinations and operands, part 1 is destination-driven
generation's data destination, and `cbranch(e, label, sense)` is
already its control destination. Part 3's classes are close to Wirth's
modes and tcc's value kinds. What tables alone add is rows as data
that can be queried, listed and checked, and a needs column for pinned
registers. In the assessment's words: "As an organizing design for rcc
it is the weakest of the three. As a source of mechanisms it gives
two."

## 5. Destinations: destination-driven code generation

### 5.1 The design in its source

Dybvig, Hieb and Butler's report, "Destination-Driven Code Generation"
(Indiana University, Technical Report 302, February 1990), describes a
code generator that is one function (p. 7):

```
CG : Expression → Environment → Data Destination → Control Destination → Code
```

Its pieces:

- The *data destination* says where the value goes: `effect`, for no
  value, or a location, `ac` (the accumulator, `r0`), `stack`,
  `param_i` or `local_i` (pp. 5, 7).
- The *control destination* says where control goes next: a label, or
  a pair of labels for true and false, with `return` and `Lnext`, the
  next instruction, among the labels (pp. 6, 7, 13).
- The two destinations make four categories of expression: for effect,
  for a value, for a test, and for a value and a test (p. 7).
- Three helpers carry them: `CG_store` moves a value into its data
  destination and then jumps or branches (pp. 13-14); `CG_branch` and
  `CG_jump` write nothing for `Lnext` and a `ret` for `return` (p.
  13).
- The rules hand the destinations down. An `if` hands its test the
  pair `(Lthen, Lelse)` and both arms its own destinations (Figure 5,
  p. 9); a loop body inherits the loop head as its control destination
  (Figure 5); `not` swaps the pair (Figure 7, p. 11):

  ```
  CG [[ if Etest Ethen Eelse ]] ρ δ γ ⇒
      CG [[ Etest ]] ρ effect (Lthen , Lelse )
  Lthen :
      CG [[ Ethen ]] ρ δ γ
  Lelse :
      CG [[ Eelse ]] ρ δ γ
  ```

The paper keeps its technique apart from the rest of a back end: "the
destination-driven code generation technique is independent of the
solutions for these problems", storage allocation and instruction
selection (p. 5). Its generator for Scheme is "parameterized by a
table of machine-specific code generators for various low-level
operations" (p. 16). Its target is the VAX, with "three, two, and one
operand arithmetic instructions", of whose registers it uses "only r0"
(p. 4).

### 5.2 The design at work where it was born

Two statements on locals, worked out by hand from the paper's rules.

`x = a - b;` is an assignment for effect. Its rule hands the right
side the variable's location as its data destination (p. 10), and the
subtraction of two simple operands writes it in one three-operand
instruction (pp. 12, 14):

```
subl3	b, a, x
```

The value is never in a register: the destination reached the
instruction.

`if (a < b && c < d) s;` is a one-armed `if` for effect, its control
destination `L`, the label after it. The `if` hands its test the pair
`(Lthen, L)`; `and` hands its left operand `(L1, L)` and its right one
`(Lthen, L)` (Figure 7); `<` is a compare and `CG_branch`, which
writes the conditional branch to the label that is not the next
instruction (pp. 11, 13):

```
	cmpl	a, b
	jgeq	L
L1:
	cmpl	c, d
	jgeq	L
Lthen:
	s
L:
```

No value of `<` or of `&&` exists; each test jumps straight to where
control must go.

### 5.3 The case against, for this compiler

**rcc already has the design; what it lacks is the name and a single
type.** It carries the same information in more pieces than the three
of 2.2: `into_t`, `wanted`, `effect()` for a value not wanted,
`cbranch(e, label, sense)` for a test, and the two that travel in the
state, `s.in_place` and `s.reserved`. On aarch64 the data half is one
function already, `final_destination()`, and of the 18 aarch64
functions that take `wanted`, 16 also take `into_t` (at 4b95569).

**The queue's edges are not hand-down edges.** They come from the
bottom-up facts of chapter 3, which a top-down destination cannot
supply, and the paper never meets the hazard behind them. Its VAX has
three-operand instructions and its generator one register. Its rule
for a complex left operand and a simple right one generates the left
operand into the accumulator and then the operation (Figure 8, p. 12):

```
CG [[ binop op E S ]] ρ δ γ ⇒
   CG [[ E ]] ρ ac L
L:
   CG binop [[ op ]] ac (CG operand [[ S ]]ρ) δ γ
```

which is `sreorder()`'s rule: the right operand must be simple. A
single `dest_t` would carry the x86-64 predicates unchanged. The
widened join of 2.3 is the clearest case: no top-down design supplies
whether an operand's value is valid to a width before the operand is
generated.

**The paper's destinations are hard; rcc's are soft.** In the paper a
location destination is written unconditionally: `CG_store O A L`
writes `movl ↑O,↓A` (p. 14). In rcc a named register is written "where
the last instruction that writes the value can write that register and
no live value holds it" (`docs/backend.md:758-759`). That is the
Seventh Edition compiler's rule too, "it is desired that it appear in
reg" (`c10.c:159`). Taken literally, a hard destination would vacate
live registers and make listings longer. A unification must keep rcc's
soft rule, so the paper gives the shape and none of its rules.

**The paper's own rows break the charter in four places.**

- Its instruction selection drops `subl2 $0, A` and turns `subl2 $1,
  A` into `decl A` (Figure 4, p. 6).
- An assignment for its value is rewritten as `sequence (assign x E)
  (var x)` (p. 10), which reads the variable again from memory.
- A boolean for its value becomes `if Ebool (int 1) (int 0)`, jump
  code (p. 12), longer than `setcc` or `cset`.
- Its `return` writes `ret` where it stands, and `CG_jump` writes
  nothing for the next instruction (p. 13). `backend.md` 7.2 refuses
  that: "the jump to the next instruction is what the statement says"
  (`docs/backend.md:1739-1740`).

**The control half changes pins the queue never asked about.** An
inherited control destination makes a jump to a jump one jump. At
4b95569 the pins hold 14 unconditional and 10 conditional jumps to a
jump, in 14 of the 135 pinned listings of each target. Each is a
question for the charter: the instruction written would no longer be
the one the source spelled.

**History.** The paper names its own limits (p. 15): redundant jumps
in nested `if`s, a loop test at the bottom that would need
back-patching, and temporaries on the stack for complex operands, for
which it recommends a prepass. Recalled, not read: Chez Scheme later
moved to a compiler of several passes with a register allocator, and
V8's full-codegen, whose expression contexts Effect, AccumulatorValue,
StackValue and Test are this design, was retired in 2017. Both
retirements came with register allocators, which rcc's charter
refuses, so that history weighs little here.

**The roadmap.** The design does little for it: the hard parts of
floating point, structs, bit-fields and variadic functions are layout,
the ABIs' classification and operand forms (chapter 11).

**Cost.** The unification touches 88 functions and 2147 lines, 318 of
them naming a destination, counted at 4b95569. Over four days it would
have removed 6 of the 57 items and made 6 smaller (chapter 8).

### 5.4 What c1 already has

| Destination | The paper | The Seventh Edition c1 | rcc |
|---|---|---|---|
| effect | δ = effect | `efftab` (`c1.h:110`) | `effect()` (`src/c1-x86-64.cc:4378`, `src/c1-aarch64.cc:3145`) |
| a register | `ac`, one register, hard | `regtab` and `reg`, soft (`c10.c:155-160`) | `rvalue(e, into, wanted)`, soft, `wanted_register()` (`src/c1-x86-64.cc:468`) |
| a variable | `local_i`, `param_i`, in memory | `efftab`'s `=` rows, `sreorder()` | `into_t`, `takes()` and `taken()` (`src/c1-stack.hh:223`, `src/c1-stack.hh:232`) |
| pushed | `stack` | `sptab` | none: an argument is a hidden local |
| a test | γ = (Lt, Lf) | `cctab`, `cbranch(tree, lbl, cond, reg)` (`c11.c:544`) | `cbranch(e, label, sense)` |
| return | γ = return | `RFORCE` into `r0` (`c10.c:235-239`) | `return_stmt()` names the result register (`src/c1-statements.hh:442`) |

So tables and destinations are two halves of one thing, and Ritchie's
c1 is both: destination-driven in its driver and table-driven in its
rows. `rcexpr(tree, table, reg)` (`c10.c:176`) takes the table as the
kind of destination and `reg` as the register, `cbranch()` takes the
control destination, and `match()` chooses a row within the table the
destination named. Whatever organizes the rows still needs a
destination axis, and rcc has one in pieces.

One duplication shows plainly. On x86-64, `load()`, `literal()`,
`address_of()` and `symbol_address()` take `into_t` and not `wanted`,
so a named register reaches them only through the register stack's
order, the lowest free register. That is why `arm()` reserves the
registers below a join (`src/c1-x86-64.cc:2457-2485`). With one
`dest_t` those rows would write a named register as they write a
variable's.

### 5.5 What to take, and what to leave

The assessment's verdict is "adopt this part":

- one mandatory `dest_t` in place of `into_t *`, `wanted` and
  `s.in_place`, resolved by one `deliver()` per target, the paper's
  `CG_store` with rcc's soft rule;
- the arms of `?:`, `&&` and `||` inheriting it, in text both targets
  share.

Its sketch of the type and of the step, not compiled:

```cpp
// The destination of a value, the data destination of Dybvig, Hieb and
// Butler (backend.md 6.1): none, for a value evaluated for its effect
// (6.6); any expression register; the register a consumer names, rax
// for a return's, rcx for a shift's count; or a register variable's
// register (chapter 4), computed in as where a conversion of the
// signedness alone is over the row.  widen is the type a widening
// conversion hands down with it, so the row writes the value extended,
// movslq -16(%rbp), %rbx.  taken says a row wrote the destination.
struct dest_t
{
    enum class kind_t { effect, any, reg, variable };

    kind_t kind = kind_t::any;
    reg_t reg = expression_registers[0];
    const symbol_t *variable = nullptr;
    const type_t *as = nullptr;
    const type_t *widen = nullptr;
    bool taken = false;
};

// The control destination: a jump to label when the condition is
// sense, and a fall through otherwise, the paper's pair of labels with
// one of them the next instruction, as cbranch() takes it today
// (backend.md 7.1).
struct control_t
{
    int label;
    bool sense;
};

// v for dest, the paper's CG_store: where a row wrote the destination
// the value is the destination's; otherwise a register variable takes
// establish()'s mov, a register a consumer names the mov where it is
// free, movl %edx, %eax for a remainder returned, and any keeps v.  A
// value already in its destination writes nothing, r = c ? x : r.
value_t
deliver(state_t &s, value_t v, dest_t &dest)
{
    switch (dest.kind) {
    case dest_t::kind_t::effect: release(s, v); return { };
    case dest_t::kind_t::any: return v;
    case dest_t::kind_t::variable:
        if (!dest.taken && s.values[v.id].variable != dest.variable)
            establish(s, dest.variable, reg_of(s, v));
        dest.taken = true;
        release(s, v);
        return read(s, dest.variable);
    case dest_t::kind_t::reg: break;
    }

    const reg_t from = reg_of(s, v);
    if (from == dest.reg || !wanted_register(s, bit(dest.reg)))
        return v;

    const int width = std::max(32, s.values[v.id].valid);
    if (borrowed(s, v))
        copy(s, v, dest.reg);
    else
        move(s, from, dest.reg, width >= 64 ? 64 : 32);

    return v;
}
```

Left:

- the paper's rows, which fold `- 0`, reread an assigned variable and
  write a boolean value as jump code;
- the control destination as a parameter of every row. `cbranch()`
  already is one, and the fourth category, a value and a test, gains
  nothing on these machines: an x86-64 `mov` sets no flag, and
  aarch64's `cbz` tests the value's register;
- jump threading and inline returns, which are the charter's
  questions.

The assessment's confidence: high that the whole design does not
justify a rewrite, since 45 of the 57 items are untouched by it, and
about 0.6 to 0.7 that the part pays for itself. The measured gain is 6
items removed and 6 made smaller, 1266 of the 9602 lines of `src/` the
46 commits changed. The larger case is the roadmap: a floating-point
register, a place in memory and a pair of registers are each a new
destination. Without `dest_t` each would be threaded through about 50
functions per target, as `wanted` was: 2792719 changed the x86-64
generator by 295 lines added and 81 removed, and 249669a the aarch64
one by 218 and 90. That case is inference. The tree has one more
instance since: 5498bbf handed `wanted` to aarch64's `step()`, and at
d0b66bd 17 aarch64 functions take both `into_t` and `wanted` where
4b95569 had 16.

What would change it:

- up, if the floating-point slice or the struct slice threads a new
  parameter through more than 20 rows of a target;
- down, if merging the deliveries finds that they differ for reasons
  beyond `arm_into()`'s one missing check. Then the merge would be
  cosmetic, and removing the defaults alone (12.2, step 3) is the part
  worth having.

## 6. Operands: delayed code generation through operand descriptors

### 6.1 The design in its source

An *operand descriptor* is what a row returns: a description of where
its value is so far, not yet a register. Wirth calls it an *item*, tcc
an *SValue*. To *force* a descriptor is to write the instructions that
bring it into the form its consumer needs: Wirth's `load()`, tcc's
`gv()`. *Delay* is the time between a row's return and its consumer's
force.

**Wirth.** `ORG.Mod` is the code generator of the Oberon-07 compiler
for Wirth's RISC. Its `Item` is a mode, a type and three integers
(`ORG.Mod:21-26`):

```
    TYPE Item* = RECORD
      mode*: INTEGER;
      type*: ORB.Type;
      a*, b*, r: LONGINT;
      rdo*: BOOLEAN  (*read only*)
    END ;
```

and its comment gives the modes (`ORG.Mod:28-36`):

```
  (* Item forms and meaning of fields:
    mode    r      a       b
    --------------------------------
    Const   -     value (proc adr)  (immediate value)
    Var     base   off     -               (direct adr)
    Par      -     off0     off1         (indirect adr)
    Reg    regno
    RegI   regno   off     -
    Cond  cond   Fchain  Tchain  *)
```

A constant, a variable in memory at a base and an offset, a parameter
reached through an address in memory, a value in a register, an object
at a register plus an offset, and a condition with two chains of jumps
still to be fixed up. The procedures that carry it:

- `load(x)` forces any item into a register (`ORG.Mod:158-188`); a
  `Cond` is forced by writing 1 and 0 on its two chains
  (`ORG.Mod:181-184`).
- `Field` and `Index` are arithmetic on the item. A field adds its
  offset, `x.a := x.a + y.val` (`ORG.Mod:263`), and a constant index
  adds its product, `x.a := y.a * s + x.a` (`ORG.Mod:276`). Only a
  variable index writes code (`ORG.Mod:279-302`).
- `IntRelation` leaves out the compare with 0 when the last
  instruction set the flags, by looking at the instruction just
  written, `code[pc-1]` (`ORG.Mod:576`), and returns a `Cond`.
  `CFJump` forces it as a jump (`ORG.Mod:730-734`).

**tcc.** An `SValue` is a type, a location word `r`, a second register
`r2`, and a constant or the jump chains (`tcc.h:481-496`). `r` is a
register, or one of `VT_CONST`, `VT_LOCAL`, `VT_LLOCAL`, `VT_CMP` and
`VT_JMP`; `VT_LVAL` means "the object at that location", and
`VT_MUSTCAST` marks a narrow value whose extension is still pending
(`tcc.h:1026-1035`). The operands form a stack whose top is `vtop`,
and `gv(rc)` forces the top into a register of class `rc`
(`tccgen.c:1844-1847`): "store vtop a register belonging to class
'rc'. lvalues are converted to values."

### 6.2 The design at work where it was born

`a[3]` and `a[i] + 1` in Oberon, for a local array `a` of `INTEGER` at
offset 16 of the frame, worked out by hand from `ORG.Mod`'s procedures
and not run:

- `a` is an item `Var` with `a = 16`. A local is addressed off the
  stack pointer, `SP`, at its offset plus `frame`, the bytes the code
  has pushed since the procedure's entry (`ORG.Mod:175`,
  `ORG.Mod:753`).
- `a[3]`: `Index` with a constant index makes `a = 3 * 4 + 16 = 28`
  (`ORG.Mod:276`). The item is still `Var`, and no instruction is
  written. Its `load()` is one load off `SP` at 28 (`ORG.Mod:175`).
- `a[i]`: `Index` with a variable index loads `i`, shifts it left by 2
  (`ORG.Mod:289`) and adds `SP` to it, and the item becomes `RegI`:
  that register, with the offset still 16 (`ORG.Mod:291`,
  `ORG.Mod:297`). No instruction adds the 16.
- `a[i] + 1`: `AddOp` with a constant right operand loads the left
  item (`ORG.Mod:436`), and `RegI`'s load is one load at the register
  plus 16 (`ORG.Mod:180`); then the add of 1, which `AddOp` writes
  only for a constant that is not 0 (`ORG.Mod:437`), as rcc writes
  nothing for an identity (`backend.md` 6.4).

The offset rides in the item until an instruction reads the object, so
an element's address is never formed by an add of its own. The same
holds for a field of a record, and that is the case structs bring to
rcc (chapter 11).

### 6.3 The case against, for this compiler

**G13 forbids the move that pays in the sources.** In a one-pass
compiler, delay pays because the compiler picks the left operand's
form after it has seen the right operand. Both sources take that gain
by reading the left operand after the right operand's side effects:

- **Oberon.** `x + f(y)` compiles `f(y)` by `term(y)` (`ORP.Mod:375`)
  before `AddOp` loads `x`, `load(x); load(y)` (`ORG.Mod:438`). The
  call saves registers alone (`PrepCall`, `ORG.Mod:764-769`), and a
  variable not yet loaded is in no register.
- **tcc.** A call saves the value stack's registers (`save_regs()`,
  `tccgen.c:1394-1399`, called by the x86-64 `gfunc_call()` at
  `x86_64-gen.c:1235`), and `save_reg_upstack()` returns at once for
  anything that is no register (`tccgen.c:1414-1415`). So an lvalue on
  the value stack crosses the call unread, and the `+` loads it after
  the call.

That is the behaviour read in the sources; neither compiler was run.
rcc promises the opposite: the left operand is read first
(`docs/guarantees.md:106-108`). For `return g + bump();` rcc writes on
x86-64:

```
	movl	g(%rip), %eax
	movq	%rax, -8(%rbp)
	call	bump@PLT
	addl	-8(%rbp), %eax
```

The delayed form, `call bump@PLT; addl g(%rip), %eax`, is two
instructions shorter, and it is wrong whenever `bump` writes `g`. So
in rcc a delayed read must be made before any later operand that
evaluates anything. Two delays survive that rule:

- a right operand, consumed by the very next instruction;
- a left operand whose right operand evaluates nothing: a constant, a
  name, or a register variable's read.

rcc takes both already, by looking ahead in the tree:
`memory_operand()` (`src/c1-x86-64.cc:1320`), `binary_operand()`
(`src/c1-x86-64.cc:1595`), `compared()` (`src/c1-x86-64.cc:2313`) and
`product()` (`src/c1-x86-64.cc:3093`).

**The tree already gives what delay gives inside G13.** Wirth needs
the item because a parser of one pass cannot look ahead. rcc holds the
whole typed tree of a function, so whether an operand evaluates
anything is a property of its subtree, known before generation. Three
facts the tree does not give cheaply are facts of what was just
generated, and rcc keeps each of them already:

- the width a register is valid to, `held_t::valid`
  (`src/c1-common.hh:152-161`);
- the width at which the last instruction set ZF, `zero_flag`
  (`src/c1-x86-64.cc:137-140`);
- an address not yet formed, `address_t` (`src/c1-x86-64.cc:806-819`).

**A descriptor describes a source; the queue's problem is
destinations.** It says where a value is, not where it should go.
Wirth has no destination at all: every value goes to the next
register, `RH`, and `Store` loads and then stores (`ORG.Mod:611-624`).
tcc's `gv(rc)` names a register class only when it forces, so a
destination reaches a leaf and nothing more. On x86-64 the result of
`a + b` lands in its left operand's register; when `b` evaluates
anything, `a` must be read first, so that register is chosen before
`b` exists, and the request must still reach the row. Of the 18
expression commits the design leaves as the same work, 12 are this
destination and reservation work.

**Opting out is riskier than opting in.** Today a left operand is
loaded unless a named rule says otherwise. Under delay it stays unread
unless a rule forces it, and a row that forgets the force writes a
listing that assembles and reads an object late. The design therefore
needs a check that makes a forgotten force an error of the generator:
while a delayed read is pending, no instruction may read or write a
program object, or call, unless it is that read's own.
`check_movable()` already has that shape (`src/c1-stack.hh:30-42`),
but the check is one more invariant every row must keep.

**The force function becomes the big switch.** In tcc it holds every
form, crossed with every width, extension and class: `gv()` is 152
lines (`tccgen.c:1847-1998`), the x86-64 `load()` 201
(`x86_64-gen.c:374-574`), the arm64 `load()` 106
(`arm64-gen.c:519-624`). tcc's own comment on its flags form reads
"cannot let cpu flags if other instruction are generated"
(`tccgen.c:866`): a condition descriptor lives only until the next
instruction, as rcc's `zero_flag` does.

**aarch64 gains least.** It has no memory operand, so a descriptor of
memory there is only a load not yet written. A delayed load buys one
thing, its destination register, and aarch64 already chooses that at
the last instruction, `final_destination()`.

**The migration touches most rows.** Of the 404 function definitions
in the generator sources at 4b95569, 89 return a held value,
`value_t`; 63 return or take a descriptor struct; 90 take a
destination; 160 do at least one of the three. In place, each change
is one more step to prove inert.

### 6.4 What c1 already has

rcc has descriptors already, grown separately:

- `held_t` and `value_t`, the register form, with the width its value
  is valid to and the borrowed read of a register variable
  (`src/c1-common.hh:137-175`);
- `address_t`, a base, an index, a scale, a displacement and a label
  (`src/c1-x86-64.cc:806-819`); aarch64's adds the extension and the
  step (`src/c1-aarch64.cc:766-787`);
- `right_t`, "the immediate the instruction takes, the value held, or
  the address of an object in memory that the instruction names in
  place of a register" (`src/c1-x86-64.cc:1486-1500`;
  `src/c1-aarch64.cc:1642`), already an item of three modes, consumed
  by one row;
- `register_read_t` (`src/c1-x86-64.cc:2745-2756`), and aarch64's
  `index_t` and `step_t` (`src/c1-aarch64.cc:719-733`,
  `src/c1-aarch64.cc:755-764`);
- `lvalue_offset_t` and `named_offset_t`, an address plus a constant
  found by matching the tree (`src/c1-common.hh:68-76`,
  `src/c1-common.hh:80-87`);
- `zero_flag`.

At 4b95569, 139 of the 404 definitions return a held value or return
or take a descriptor struct. Fourteen predicates besides predict an
operand's form from the tree before it is generated: column 3 of 3.2's
list.

Two delays rcc takes already have the design's shape. `preserve()` is
the force of the borrowed form: a borrowed read is a read of a
register variable not yet made into a copy; only a write of that
variable can change it, and the tree shows every write; so the read is
not forced before a later operand, but by a copy at the variable's
write, which is what `preserve()` does (`src/c1-x86-64.cc:738-752`,
`src/c1-aarch64.cc:580-599`). It is the one delay G13 allows freely.
aarch64's stepped access, `step_t`, is a delayed write, guarded by a
look ahead at the operand generated between, `names(*between,
step.variable)` (`src/c1-aarch64.cc:1186`).

And one family grows by matching. x86-64 has five functions that each
recognise one address shape in the tree: `folded()`
(`src/c1-x86-64.cc:984`), `indexed()` (`src/c1-x86-64.cc:1017`),
`constant_indexed()` (`src/c1-x86-64.cc:1042`), `summed()`
(`src/c1-x86-64.cc:1077`) and `index_base()`
(`src/c1-x86-64.cc:1106`). The fifth came with b75c42b, after 4b95569,
and the same commit queued two address shapes that none covers
(`TODO:82-112`).

### 6.5 What to take, and what to leave

The assessment's verdict is to adopt:

- the address descriptor with its two operations, "add a constant" and
  "add an index at a scale", `plus(address_t, constant)` and
  `indexed_by(address_t, index, scale)`, which removes the family of
  address matchers and lands struct members;
- one `operand_t` as the return of every row, with `as_source()` and
  `into()`;
- the rule that a result stays where its instruction left it.

Its sketch of the descriptor, not compiled:

```cpp
// An operand as far as its row has generated it (backend.md 6.1): a
// constant, with no evaluation; an object in memory not yet read, of its
// type's width (G10), whose load extends to extend_to; an address not
// yet formed; a held value, owned or borrowed, valid as held_t says, an
// extension to extend_to pending; or the flags, where cond holds.
enum class form_t { none, constant, memory, address, held, flags };

struct operand_t
{
    form_t form = form_t::none;
    long long constant = 0;
    address_t place { };
    int_t type { 32, true };
    int extend_to = 0;
    value_t value { };
    binary_op_t cond = binary_op_t::ne;
    bool is_signed = true;
};

operand_t operand(state_t &, const expr_t &);  // the row; nothing forced
bool evaluates(const expr_t &);  // writes code before its own read
void read_now(state_t &, operand_t &, unsigned wanted, unsigned avoid = 0);
reg_t in_register(state_t &, operand_t &, unsigned wanted = 0);
reg_t written_register(state_t &, operand_t &, int valid, unsigned wanted,
                       unsigned avoid);
std::string as_source(state_t &, operand_t &, int width); // $c, d(B,I,s), %r
void into(state_t &, operand_t &, reg_t r, int valid);    // a named register
```

Its mechanisms, named in its count (chapter 8):

- **M1, the address descriptor.** It stays base + index × scale +
  displacement until forced. Adding a constant goes to the
  displacement; adding an index fills the index where the scale fits.
- **M2.** An object in memory is an instruction's source, at its own
  width.
- **M3.** Casts compose: a widening of an object in memory is its
  extending load, and a cast that keeps the bits retypes it.
- **M4.** A result stays where its instruction left it.
- **M5.** Each arm of `?:` is forced into the destination on its own
  path, and a variable's own read forced into that variable writes
  nothing.
- **M6.** One function writes every truth value from the flags form.

Left:

- delay beyond what G13 allows; rcc already delays inside G13, by
  looking ahead;
- Wirth's condition with jump chains, since `cbranch()` with labels
  does the same work on a tree;
- the expectation that this design organizes the generator. It does
  not touch `into_t`, `wanted` or the reservations, and those are 12
  of the 18 expression commits it leaves as the same work.

The assessment's confidence: about 90% that delay should be left, and
about 65% that the descriptor half pays for its migration. What would
change it:

- the first two steps prototyped on x86-64 (12.3): if the address
  descriptor does not delete more lines than it adds, leave it too;
- struct member access written both ways: if each matcher takes its
  member case in about ten lines, the descriptor's lead is small.

## 7. Selection as a pass of its own

### 7.1 The design

The fourth design takes chapter 3's finding at its word. If an edge is
a predicate that disagrees with the row it mirrors, let the predicate
run once, before any instruction is written, and let its answer be the
row. A pass over the typed tree of an expression decides, for each
node, which form it will be generated in, and records the decision. A
generator per target then writes each recorded form by a function that
asks nothing.

Of the four it is the avenue with no compiler behind it whole: its
lineage takes parts of it (7.2), and its assessment, made against the
tree at d0b66bd, takes a part too (7.9).

Two terms come from attribute grammars, recalled and not read here. An
attribute is *inherited* when it flows from a parent down to its
child, as where a value is wanted does; it is *synthesized* when it
flows from the subtree up, as what the subtree is does.

The assessment defined the design for rcc so (its sketch, not
compiled):

```cpp
struct dest_t                       // inherited: what the consumer asks
{
    enum class kind_t { value, named, variable, effect } kind = kind_t::value;
    int reg = -1;                   // named: the target's register number
    const symbol_t *variable = nullptr;
    const type_t *as = nullptr;     // variable: a row computing in another signedness
};

struct facts_t                      // synthesized: what the form is
{
    int valid = 0;                  // the width its result is valid to (6.3)
    int zero_flag = 0;              // its last instruction sets ZF at this width (7.1)
    unsigned pins = 0;              // pinned_within()
    const symbol_t *borrowed = nullptr; // a register variable's read
    bool writes_dest = false;       // its last instruction writes dest itself
};

enum class delivery_t { in_place, move, none };   // how a value reaches dest

struct node_t                       // one entry of the side table
{
    const expr_t &e;
    form_t form;                    // std::variant of the forms below
    dest_t dest;
    facts_t facts;
    const type_t *to = nullptr;     // a conversion handed down (6.1)
    std::vector< const node_t * > kids;
};
```

- **The record** lives in a side table keyed by node, so `ast.hh` and
  `parse.cc` stay as they are. Each node gets a `form`, a
  `std::variant` of the forms of its kind; a `dest_t`, inherited; a
  `facts_t`, synthesized; and an optional conversion handed down.
- **`select()`**, the pass, runs once per statement, since nothing is
  held from one statement to the next
  (`src/c1-statements.hh:577-578`). Its rules are first-fit and
  declared in order, with no search by cost.
- **`generate()`** is a `std::visit` per target over the selected
  forms, `std::visit` being the C++ call that dispatches on a
  variant's alternative and refuses to compile when one is not
  handled.
- **What stays at emission**: the register a value lands in, a spill,
  and the move into a destination where the value is elsewhere, all in
  one delivery step, destinations' `deliver()` of 5.5.

### 7.2 The design in its sources

**The Seventh Edition compiler** annotates first and matches later, in
part. An expression statement is passed through `optim()` before it is
generated (`c11.c:914-918`):

```c
		*sp = optim(*--sp);
		if (op==CBRANCH)
			cbranch(*sp, lbl, cond, 0);
		else if (op==EXPR)
			rcexpr(*sp, efftab, 0);
```

`optim()` rewrites the tree and records each node's `degree`, the
registers it needs, from the leaves up: `tree->degree = max(d1, d2)`
for `?:`, and for most binary operators `tree->degree = d1==d2?
d1+islong(tree->type): max(d1, d2)` (`c12.c:140-179`, `c12.c:262`).
The match is made later, at generation: `match()` compares `dcalc(p1,
nrleft)` and `dcalc(p2, nrleft)` against each row's degrees
(`c10.c:108`, `c10.c:131`), and `dcalc()` ends with the registers left
(`c11.c:195`):

```c
	return(p->degree <= nrleft? 20: 24);
```

So the ancestor splits the work. What the tree settles, the degree, is
recorded ahead; whether an operand fits the registers left is decided
when the code is written.

**lcc** goes further, and pays for it with an allocator. Its `gen()`
takes each tree through the steps in order:

- `prelabel()` sets register targets from the tree
  (`lcc/gen.c:421-447`), and `rtarget()` inserts a move node where a
  child must be in a pinned register (`lcc/gen.c:452-468`); x86's pins
  are in `target()` (`lcc/x86linux.md:780`);
- `rewrite()` labels the tree with the grammar's labeller and reduces
  it (`lcc/gen.c:470-478`), and `reduce()` records the chosen rule in
  each node, `p->x.inst = nt` (`lcc/gen.c:198`). That field is the
  recorded form;
- only after every tree is rewritten does `ralloc()` assign registers,
  node by node (`lcc/gen.c:494`, `lcc/gen.c:516`, `lcc/gen.c:607`);
- at emission a move whose source is its own destination is dropped,
  `moveself()` (`lcc/gen.c:378-381`).

That `lburg`'s labeller works from the leaves up, by cost, is
recalled: the labeller is generated C, and it was not read here.

### 7.3 The case against, for this compiler

**1. The design assumes the edges are tree facts, and the hardest are
not.** Of the 828 branch sites of the five generator sources at
d0b66bd, 34 need to know which registers are occupied (7.4). The four
days' work of chapter 8 went to exactly those: the consumer's named
register, the joins of `?:`, `x0` kept for last. lcc selects from the
tree alone because an allocator assigns the registers after it, and
rcc's model, a register variable by declaration order and the
expression registers used as a stack, has none
(`docs/backend.md:15-26`). So the pass has two choices, and both are
bad:

- It carries a model of the register stack. That copies the register
  half of 82 functions and 2194 lines: 49 x86-64 functions of 1325
  lines, 27 aarch64 functions of 792, and 6 of `c1-stack.hh`, 77. The
  two copies must then agree, which is a new class of edge, chapter
  3's in a new place.
- It leaves those sites at emission, and generation still asks.

**2. Corner cases move; they do not vanish.** Each entry of the queue
is a rule to add or to correct, and the pass holds the same rules.
C++'s type checker can make sure that every form has a generator; it
cannot make sure that the best form was chosen. None of the five
entries on `?:` that those four days queued is a missing case on
x86-64 (7.5).

**3. It adds a walk and a record.** Each kind of node gets a rule in
the pass and a case in each generator, three places per feature where
there are two today. Pinning the record would double the pins, from
272 listings to 544.

**4. The flat encoding is wrong at full size.** x86-64's
`conditional()` has 35 code paths and aarch64's 17 (7.5). One subtype
per path repeats one skeleton 35 times.

**5. The charter strains in one place: a pass ahead of generation is
where a selector's judgment grows.** The ancestor's own pass rewrites:
`optim()` exchanges a relational's operands by degree
(`c12.c:127-134`), and its generation turns `x + (-1)` into `x - 1`
(`c10.c:456-464`). `ancestor.md` 10 records what rcc refused of those
rewrites.

**6. No compiler in the lineage puts every decision ahead.** The
Seventh Edition compiler matches at generation against the registers
left, and it generates the second arm of `?:` into whatever register
the first arm came back in, moving it where the two differ
(`c10.c:429-443`). lcc and pcc keep an allocator.

**7. It would not have helped with most of the work.** 44 of the 51
generator commits are the same work under it (chapter 8). The distance
to the first real program is layout, the call convention and
initializers, which the design does not touch.

### 7.4 The count that settles it: tree, registers or frame

The design works to the degree the generator's decisions can be made
from the tree. The assessment counted them. A script extracts every
branch site of `src/c1-x86-64.cc`, `src/c1-aarch64.cc`,
`src/c1-stack.hh`, `src/c1-common.cc` and `src/c1-statements.hh` at
d0b66bd: every `if` condition, every `?:` and every `switch` subject,
828 in all. It tags a site as reading the generator's state when its
condition reads the state directly (`s.owner`, `s.reserved`,
`s.in_place`, `s.values`, `s.zero_flag` and the rest), through a
helper that reads it (`wanted_register()`, `takes()`, `borrowed()`,
`held_out()`, `reg_of()`, `take()` and others), or through a local set
from one; 125 sites are so tagged and 703 are not. Each tagged site
was then classified by hand, with a reason written for it, and some
sites the script left untagged were moved by hand: `widens_arms()`
under `cast()` (`src/c1-x86-64.cc:3733`), which reads
`wanted_register()` through a call the script did not follow, and the
displacements of slots, which depend on the frame. The other sites the
script left untagged were spot-checked only.

The classes:

- **T, from the tree.** The site reads the subtree, its types and
  symbols, and attributes passed down or up the tree: `takes()`,
  `borrowed()`, `into->taken`, `s.in_place`, `s.zero_flag` after a row
  (the width at which that row's last instruction set ZF, a fact of
  the form), and `s.terminated`, a fact of the statement tree.
- **M, from the tree and a model of the register stack.** The site
  needs to know which registers are held, reserved or spilled, or
  where a child's value came back.
- **D, from the frame.** A slot's displacement depends on every
  earlier spill and every earlier call's argument slots in the
  function: aarch64's `in_reach()` under `slot_operand()`
  (`src/c1-aarch64.cc:322`) and x86-64's 32-bit test in `folded()`
  (`src/c1-x86-64.cc:994`).
- **K, the register stack's own mechanics**, `take()`, a spill, a
  reload, a release: what a model would be, not a row's choice.
  Counted apart.
- **X, an assertion**, no choice. Counted apart.

| File | All | T | M | D | K | X |
|---|---|---|---|---|---|---|
| `c1-x86-64.cc` | 397 | 364 | 23 | 4 | 5 | 1 |
| `c1-aarch64.cc` | 273 | 251 | 10 | 8 | 4 | 0 |
| `c1-stack.hh` | 14 | 6 | 0 | 0 | 6 | 2 |
| `c1-common.cc` | 95 | 95 | 0 | 0 | 0 | 0 |
| `c1-statements.hh` | 49 | 47 | 1 | 0 | 0 | 1 |
| total | 828 | 763 | 34 | 12 | 15 | 4 |

The 34 sites of class M are of four kinds:

- **17 ask whether the register a consumer names is free.**
  `wanted_register()` on each target (`src/c1-x86-64.cc:470`,
  `src/c1-aarch64.cc:527`) and its uses: in `extended()`, `divide()`,
  `comparison()`, `conditional()`, `register_product()`, `unary()`,
  `widens_arms()` and `cast()` on x86-64, and in `final_destination()`
  and `conditional()` on aarch64.
- **6 ask whether a value came back where it was meant to.** The two
  moves of a join on each target (`src/c1-x86-64.cc:2654`,
  `src/c1-x86-64.cc:2666`; `src/c1-aarch64.cc:2334`,
  `src/c1-aarch64.cc:2346`), `held_out()` (`src/c1-x86-64.cc:498`) and
  a `return`'s move (`src/c1-statements.hh:449`).
- **6 are x86-64's pinned registers**: a dividend in `rdx`, a divisor
  come back in `rax` or `rdx`, a shifted value in `rcx` (`dividend()`,
  `pin_division()` and `shift()`).
- **5 read the width a value is valid to where it may be a join's**,
  which drops when an arm is moved into the join: `extended()` on each
  target, and aarch64's `index_of()` and `test_zero()`.

Of the 83 predicates, functions that answer a question and write
nothing, 76 are of class T and 7 of class M: x86-64's
`wanted_register()`, `held_out()`, `registers_of()`, `held_in()` and
`widens_arms()`, and aarch64's `wanted_register()` and
`registers_of()`. Of the 40 functions the first count of 3.2 names at
d0b66bd, `widens_free()` and `widens_arms()` included, 38 read only
the tree; `held_out()` and `widens_arms()` are the exceptions.

**Why a pass cannot make the 34 without a second register stack.** The
registers held at a node are a function of the tree walk:

- nothing is held at the start of a statement
  (`src/c1-statements.hh:577-578`);
- every owned value is spilled before the paths of `&&`, `||` and `?:`
  diverge (`src/c1-stack.hh:358`);
- every held value is spilled at a call (`src/c1-x86-64.cc:3774`,
  `src/c1-x86-64.cc:3795`; `src/c1-aarch64.cc:2935`,
  `src/c1-aarch64.cc:2956`);
- `take()` is deterministic (`src/c1-x86-64.cc:405-433`,
  `src/c1-aarch64.cc:444-466`).

So a model is possible. But it would replay every row's choice of
register, the register half of 82 functions, and the pass and the
generator would then hold two copies of one computation that must
agree. The assessment's split is the other way: tree facts ahead,
register facts at generation, in one delivery step.

### 7.5 The conditional at full size

The design's own example is a `?:`, turned by the pass into one of a
set of selected conditionals and generated with no corner case. The
assessment enumerated the code paths of `conditional()` from the code:

- **x86-64, `src/c1-x86-64.cc:2601-2676`: 35 paths.** 1 for effect; 4
  with a conversion and the paths writing a register variable; 6 with
  the paths writing a register variable and no conversion; and 24
  joins: 12 in the named register, with or without a conversion, and 4
  each for an unnamed join with a conversion, a plain join, and a join
  where a register variable was handed down and not taken.
- **aarch64, `src/c1-aarch64.cc:2284-2356`: 17 paths.** 1 for effect,
  2 with the paths writing the variable, 14 joins.

A battery of 16 functions reached 16 distinct x86-64 paths. The paths
are the product of five independent attributes:

- **context**: a value, or effect;
- **destination**: each path writes the variable, a join in the named
  register, or a join in the first arm's register;
- **conversion**: none, or widened; aarch64 had no such attribute at
  d0b66bd;
- **the first arm's delivery**: its row writes the destination, it is
  moved, a borrowed read is copied, or it is the variable's own read
  and writes nothing;
- **the second arm's delivery**: its row writes the destination, or it
  is moved.

Their product is 49 nominal combinations, 48 for a value and the one
for effect; 34 of the value's are reachable, and with the effect path
they are x86-64's 35. Encoded as attributes, the conditional has 5
forms on x86-64, effect, paths with or without a conversion, and a
join with or without one, and 3 on aarch64. The arms' deliveries go to
the one delivery step. A flat subtype per path is the wrong encoding:
it multiplies what the attributes add, and each new feature, floating
point among them, would multiply the list again.

**The five entries on `?:` queued in those four days, and what a
compiler would have pointed at:**

- a second arm moved into the join, queued by 7e78061 and closed by
  2215be8: prevented, since the arm's record has the join as its
  destination;
- a joined remainder widened and then extended, 2.3's edge, queued by
  2215be8 and closed by bb736a0: its blocker, that the cast "would
  need to know before", is gone, since what the cast needs is a
  synthesized fact of each arm;
- a `?:` into a register variable that keeps its join, queued by
  4063a5f and open at d0b66bd (`TODO:158-182`): on aarch64 the C++
  compiler points at it once the forms are types both targets share,
  and 553c54e, after d0b66bd, did that work by hand; on x86-64 nothing
  points at it;
- an arm that is the variable's own read writing it to itself, queued
  by 0730cf5 (`TODO:184-193`): unnecessary, since one delivery step
  writes nothing for a value already in its destination;
- the conditional below a cast that gets no name on aarch64, queued by
  5498bbf (`TODO:25-52`): the name is withheld by a deliberate rule,
  so nothing would point at it.

None of the five was found by a probe. Each is a correct listing with
one move too many, found by reading listings or by counting a shape
over `tests`. A probe compares results, so it cannot find one.

### 7.6 What happens to a corner case

It moves into the pass, as a rule or as a fact a form declares. What
is gained:

- one form per node, so that two consumers cannot read one shape two
  ways: fa2ca79, "a pointer plus a constant index is read one way",
  and the entry on `p[1] + i` (`TODO:82-96`) are that kind;
- each fact stated once, beside the form that makes it true, in place
  of 38 predicates of the tree that mirror rows;
- exhaustiveness across targets, through `std::visit`;
- decisions that can be listed and printed.

What is not gained:

- whether the chosen form is the best one: an extra move is a legal
  form;
- the 34 register decisions;
- the work of writing each rule.

There is a new edge to watch: a declared fact that is false of its
generator. One assertion per form can check it, as x86-64 checks
`loads_into()` against the row today: "c1: an operand missed its
register variable" (`src/c1-x86-64.cc:2947-2949`).

### 7.7 The charter

The documents refuse two things:

- an intermediate form an optimizer reasons over: "There is no IR, and
  nothing optimizes the program on the way" (`docs/shape.md:45`);
- a selector's judgment: the selector's strength reduction "changes
  the operation performed on the program's data, and that is exactly
  the thing the thesis says the compiler never does"
  (`docs/findings.md:695-698`).

They also name what a compiler may decide: "which register holds a
value, the one-to-one machine form of each IR instruction, and the
calling convention. Nothing else: no folding, no moving, no merging,
no reordering, no deleting" (`docs/llvm.md:277-280`). A record of
selected forms decides the second of the three, and nothing else. So
it is not the representation the charter refuses, on three conditions,
which the assessment states as its inference:

- it decorates the tree and never produces a new one;
- each operator is still written once per node, 6.1's "Every operator
  of the table is written exactly once per node"
  (`docs/backend.md:588-589`);
- its rules are declared and first-fit. A search by cost would make
  the form depend on a cost model, which is the judgment the documents
  avoid; `backend.md` call 10 makes the same argument for the switch,
  whose rule "has no threshold of rcc's own and no order by frequency,
  so a programmer who knows the cases knows the form"
  (`docs/backend.md:2285-2287`).

### 7.8 What c1 already has

- **Decisions from the tree, scattered.** 763 of the 828 branch sites
  are of class T, and 76 of the 83 predicates.
- **Selections already made ahead.** An `if` whose body is one jump is
  decided from the statement tree, `body_target()` and `if_jump()`
  (`src/c1-statements.hh:477-503`). A switch's dispatch is chosen from
  its case values alone (`backend.md` call 10).
- **Facts already recorded as the code is written.** `s.zero_flag` is
  the width at which the last row set ZF, and `held_t::valid` the
  width a value is valid to: facts of a form, kept in the state.
- **One assertion of the kind 7.6 asks for**, the `loads_into()`
  check.

### 7.9 What to take, and what to leave

The assessment's verdict is "adopt a part, and not the strong form":

- a selection step for conditions, then for addresses, then for the
  conditional's attributes;
- the facts declared per form;
- a printable record;
- destinations' delivery step, for the register decisions.

Left:

- "generation asks nothing";
- a model of the register stack in the pass;
- flat subtypes.

The assessment's confidence: about 85% that the full design should not
be built in rcc without an allocator, and about 60% that the partial
one pays, mostly through one form per node and exhaustiveness across
targets. What would change it:

- if the 34 register sites' outcomes turn out to follow from depth
  alone over all 272 pins, a model in the pass would be cheap;
- if each feature of the roadmap costs a third place to edit with no
  gain from `std::visit`, the partial design would not pay.

Two questions it leaves open, each with a lean: whether a dump of the
decisions should become pinned files (a dump for counting and
debugging, pinned only where a step changes decisions without changing
listings); and whether operand choices that depend on the frame belong
to the pass (no).

Three commits after d0b66bd bear on its count, each work the tree has
since done by hand:

- aca070f closes the entry on a pointer plus a constant into a
  register variable with a new function, `base_into()`; the count
  classed that entry a small local change;
- d2f0e37 closes the entry on three shapes beside a register
  variable's destination with three rules; the count classed it the
  same work;
- 553c54e writes the widened `?:` on aarch64, the case `std::visit`
  would have demanded, and with it adds `lands_in()`, `widens_free()`,
  `widens_into()` and `second_path()` to the aarch64 generator: the
  mirrors of chapter 3, now on the second target.

## 8. The count

### 8.1 What was counted, and how strictly

The three assessments of chapters 4 to 6 counted the same work, read
at 4b95569:

- the 46 commits that changed `src/c1-*` from 2026-11-15 to 4b95569;
- the 11 entries of the queue's chapter "The back end" at 4b95569.

The fourth, read at d0b66bd, counted the 51 commits to d0b66bd, the
same 46 and the five after them, and the 10 entries at d0b66bd. Four
of the 11 had left the queue by then and three had come.

Each item was given one of four classes under each design:

- **(a)** unnecessary by construction: the design's mechanism makes
  the change needless, and the mechanism is named;
- **(b)** a small local change under it: a table line, one rule, one
  case of one function;
- **(c)** the same work as now;
- **(d)** harder under it.

Each assessment read a mixed item its own way:

- **Tables** gives a mixed item two classes, the *primary*, of its
  main change, and the *strict*, the least favourable among its parts,
  and makes the strict totals its headline. Its (a) rests on four
  mechanisms: a row cannot choose its result register; a row's pinned
  result and width are declared; `match()` is pure and can be asked;
  the expander is shared between targets.
- **Destinations** classifies the design of its migration (12.2): one
  mandatory `dest_t` resolved by one `deliver()` per target, and a
  control destination inherited by the statements. Three mixed
  commits, dacc528, 2792719 and d1d1a39, count as (c), by their main
  work on x86-64.
- **Operands** counts (b) only where the work under the design is
  smaller than now, and a fix as small either way is (c). A mixed item
  is (a) only when every one of its shapes comes free, (b) when one
  case remains, and (c) otherwise. Its mechanisms are M1 to M6 of 6.5.
- **Selection** classifies the partial design of 7.9: a pass that
  records forms, destinations and facts, and one delivery step at
  emission. Its mechanisms: S1, one form per node; S2, the record
  carries the destination, so no row is generated with a defaulted
  one; S3, facts known ahead, as synthesized attributes; S4, one
  delivery step, which writes nothing for a value already in its
  destination; S5, shared rules, one text for an attribute both
  targets agree on.

Every class is a judgment of what a design nobody has built would do.
The counts and their checks against the log are verified; the classes
are inferred, each with its mechanism written on its line.

### 8.2 The totals

| Design | Items | (a) | (b) | (c) | (d) |
|---|---|---|---|---|---|
| Tables, strict | 11 entries | 3 | 5 | 3 | 0 |
| | 46 commits | 5 | 22 | 19 | 0 |
| Tables, primary | 11 entries | 4 | 4 | 3 | 0 |
| | 46 commits | 12 | 16 | 18 | 0 |
| Destinations | 11 entries | 2 | 2 | 7 | 0 |
| | 46 commits | 4 | 4 | 38 | 0 |
| Operands | 11 entries | 3 | 3 | 5 | 0 |
| | 46 commits | 10 | 2 | 34 | 0 |
| Selection | 10 entries at d0b66bd | 3 | 5 | 2 | 0 |
| | 51 commits | 4 | 3 | 44 | 0 |

Over the 57 items of the first three, side by side:

| | (a) | (b) | (c) | (d) |
|---|---|---|---|---|
| Tables, strict | 8 | 27 | 22 | 0 |
| Tables, primary | 16 | 20 | 21 | 0 |
| Destinations | 6 | 6 | 45 | 0 |
| Operands | 13 | 5 | 39 | 0 |

Over the 53 items all four classify, the 46 commits and the 7 entries
common to both queues:

| | (a) | (b) | (c) | (d) |
|---|---|---|---|---|
| Tables, strict | 8 | 24 | 21 | 0 |
| Destinations | 6 | 5 | 42 | 0 |
| Operands | 12 | 5 | 36 | 0 |
| Selection | 6 | 6 | 41 | 0 |

Two assessments weighed the commits by the lines they changed, over
different sets of files:

- tables, over `src/c1-*`, 7728 lines: by the strict class 489 in (a),
  5058 in (b) and 2181 in (c); by the primary 2768, 2821 and 2139;
- destinations, over all of `src/`, 9602 lines: 351, 915 and 8336.

Operands set apart the 30 of its commits that are expression work,
statements, data, the parser and comments aside: 10 of them (a), 2 (b)
and 18 (c). Selection tested its count against the strong form of the
design, in which the pass decides registers too: 18 of the 51 commits
touch the register model, and under that form they are harder, which
gives 3 (a), 1 (b), 29 (c) and 18 (d).

### 8.3 Item by item

T1 to T11 are the entries at 4b95569, in the queue's order. A tables
class is primary/strict; a dash is an item that assessment did not
classify.

| Item | What | Tables | Destinations | Operands | Selection |
|---|---|---|---|---|---|
| T1 | x86-64: a joined remainder widened by its consumer is moved, then extended | b/b | c | c | – |
| T2 | aarch64: a `return`'s value is moved where its place holds `x0` | a/b | b | c | – |
| T3 | x86-64: a pointer difference with a call moves its quotient out of `rax` and back | a/a | a | a | a |
| T4 | x86-64: a right operand that writes the variable keeps the `return`'s `mov` | c/c | c | c | b |
| T5 | x86-64: a row by a constant index under a variable index forms the row's address first | b/b | c | a | – |
| T6 | x86-64: a pointer plus a constant into a register variable loads the pointer elsewhere | a/a | c | b | b |
| T7 | x86-64: three shapes beside a register variable's destination still move | b/b | c | b | c |
| T8 | A `?:` into a register variable keeps its join where each arm needs its own write | b/b | b | b | b |
| T9 | x86-64: a path of `?:` whose arm is the variable's own read writes it to itself | a/a | a | a | a |
| T10 | aarch64: `cast()` stands under no comment of its own | c/c | c | c | c |
| T11 | aarch64: a call takes 510 arguments past the eighth | c/c | c | c | – |
| 4656db6 | a global is an object | c/c | c | c | c |
| bae102e | a global's initializer is an address or a string | c/c | c | c | c |
| 54e7517 | a global's initializer in braces | c/c | c | c | c |
| ba15c7a | static | c/c | c | c | c |
| b144651 | do, break, continue, goto and labels | c/c | c | c | c |
| 286bb27 | switch, by the rule of the Seventh Edition compiler | c/c | c | c | c |
| c0e65b6 | a register variable's register is its from its block's entry | c/c | c | c | c |
| f2d9da5 | one function moves a shift's count to cl, and three corrections | a/c | c | c | c |
| caf338c | the generators' opening comments list what the files write | c/c | c | c | c |
| 231ed71 | a constant added to a name's address is part of the address | b/b | c | a | c |
| 8639283 | on x86-64 a local array's element by a variable index is one operand | b/b | c | a | c |
| ebaeadc | a constant over any lvalue's address joins its displacement | b/b | c | a | c |
| 2cb9cce | alignment to 64 bytes is measured on x86-64, not adopted | c/c | c | c | c |
| 7657e36 | an if whose body is one jump is the condition's jump | c/c | c | c | c |
| d73caf3 | a cast that widens a signed object in memory is its load | b/b | c | a | c |
| 20b9c63 | on x86-64 a condition jumps on its operator's own flags | b/b | c | c | c |
| fe58d4c | on x86-64 an operand in memory is the instruction's own | b/b | c | a | c |
| 0d332c7 | on x86-64 a division and a shift choose their registers | b/b | c | c | c |
| dacc528 | a register variable is the destination of the value computed for it | a/b | c | c | c |
| 10d7127 | a dividend held in rdx moves to rax once, for a quotient | a/a | c | a | c |
| 6bd6540 | on x86-64 a remainder's value is the value in rdx | a/a | c | a | c |
| 8f80da4 | a comparison with its constant on the left is generated mirrored | c/c | c | c | c |
| a250bd6 | four more rows take their operand in memory on x86-64 | b/b | c | a | c |
| e74a817 | four more rows write a register variable's register on x86-64 | a/b | b | c | b |
| 6eb5348 | a pointer difference keeps its right operand out of rax and rdx | b/b | c | c | c |
| d2da175 | p += n on a register pointer is the lea into its register | b/b | a | c | a |
| a7460ef | on x86-64 a truth value is valid to 64 bits | b/b | c | c | c |
| 10e3817 | a constant on the left of + * & | ^ is taken as one on the right | c/c | c | c | c |
| ef96d2e | four more rows name a value in place on x86-64 | b/b | c | a | c |
| 2982987 | a two-instruction address and a signedness cast write the variable | a/b | c | c | c |
| ee9b381 | on aarch64 a truth value and a widening load extend once | b/b | c | c | a |
| d1d1a39 | six more rows write a register variable's register | a/b | c | c | c |
| 2792719 | a return's value is computed in the result register (x86-64) | a/b | c | c | c |
| 2215be8 | the arms of ?: join in the register the consumer names | a/a | b | b | b |
| 4063a5f | each path of &&, || and ?: writes a register variable's register | a/b | b | c | b |
| ce89e97 | five more rows write the register their consumer names | b/b | b | c | c |
| b0cad23 | a shift's count is computed in rcx (x86-64) | a/a | c | c | c |
| 7ea82b3 | comments of the shared sources hold for both targets | c/c | c | c | c |
| 900def9 | an if whose body ends in a jump writes no jump over its else | c/c | a | c | c |
| fa2ca79 | on x86-64 a pointer plus a constant index is read one way | b/b | c | a | a |
| 9809960 | a while writes no jump back after a body that ends in a jump | c/c | a | c | c |
| 79ccec2 | a loop and a switch place a label past them where a jump names it | c/c | c | c | c |
| 249669a | on aarch64 a return's value is computed in x0 | a/a | a | c | a |
| e89b415 | a refusal names the function it was met in | c/c | c | c | c |
| 0730cf5 | on x86-64 a widened ?: and one after a call write the variable | b/b | c | b | c |
| 4b95569 | on aarch64 a stack parameter past 16 MiB above the frame is read | c/c | c | c | c |

The items that landed or arrived after 4b95569, as the assessments
that saw them classified them; destinations classified none:

| Item | What | Tables | Operands | Selection |
|---|---|---|---|---|
| 73b224a | a loop places a label after its head only where a jump names it | c | c | c |
| b75c42b | on x86-64 an index joins a base and a displacement in one operand | b | a | c |
| bb736a0 | on x86-64 a widened `?:` extends each arm into the join | b | c | c |
| 5498bbf | on aarch64 a stored value, a step and an arm are computed in `x0` | a | c | c |
| d0b66bd | on aarch64 a call's outgoing area passes 4096 bytes | c | c | c |
| N1 | aarch64: a conditional below a cast gets no name | b | c | b |
| N2 | x86-64: a row by a constant index plus a variable index as a value forms the row's address first | a | a | a |
| N3 | A row's constant offset is added before an index no addressing form scales by | b | a | b |

### 8.4 What the totals show, and what they do not

- **No design makes anything harder**, as each reads its own design:
  none of the four found a (d) in the work counted.
- **Each removes little of what was done.** The largest count of (a)
  is 13 of 57, under operands. What the designs remove differs: 20 of
  the 53 items common to all four are (a) under at least one design,
  and 2 under all four, T3 and T9, a forgotten destination and a
  missing check.
- **Much of the work is outside every design.** 18 of the 53 are the
  same work under all four: statements, data and linkage, the parser,
  the frame, comments and messages, the constant on the left, one
  refactor, and the comment entry T10.
- **The gains cluster by design.** Under tables, behind 12 of the 16
  primary (a)s is one mechanism: each row function chooses its own
  result register. Under operands they are the address forms, the
  memory operand and the extending load. Under destinations they are
  the forgotten and duplicated destinations, and the statements'
  jumps. Under selection they are mostly one form per node and the
  destination in the record.
- **The count covers four days of polish.** It weighs each item alike,
  a comment as much as a hand-down threaded through 15 functions, and
  it does not weigh the features to come; chapter 11 does.

## 9. Three rows, four ways

### 9.1 The rows and what they must write

Each assessment wrote the same three rows under its design: `a + b`
with `b` an object in memory and the value wanted in a named register;
`r = c ? x : y` into a `register long` from two `int` arms; and `if
(p[i] & 1) break;`, a condition on an operator's flags over an indexed
operand. The sources, as functions:

```c
int
ret_sum(int a, int b)
{
    return a + b;
}

int
var_sum(int a, int b)
{
    register int r;

    r = a + b;
    return r;
}

long
chosen(int c, int x, int y)
{
    register long r;

    r = c ? x : y;
    return r;
}

int
scan(int *p, int n)
{
    int i;

    for (i = 0; i < n; i++)
        if (p[i] & 1)
            break;
    return i;
}
```

What rcc writes at d0b66bd. Row 1, `return a + b` and `r = a + b`, on
x86-64:

```
	movl	-4(%rbp), %eax
	addl	-8(%rbp), %eax
```

```
	movl	-12(%rbp), %ebx
	addl	-16(%rbp), %ebx
```

and on aarch64:

```
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	add	w0, w0, w1
```

```
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	add	w19, w0, w1
```

Row 2 on x86-64, each path writing the variable with an extending
load:

```
	cmpl	$0, -12(%rbp)
	je	.Lchosen.2
	movslq	-16(%rbp), %rbx
	jmp	.Lchosen.3
.Lchosen.2:
	movslq	-20(%rbp), %rbx
.Lchosen.3:
```

and on aarch64, where the arms join in `w0` and the extension follows
the join, the aarch64 half of the queue's entry on a `?:` that keeps
its join (`TODO:158-182`):

```
	ldr	w0, [x29, #24]
	cbz	w0, .Lchosen.2
	ldr	w0, [x29, #28]
	b	.Lchosen.3
.Lchosen.2:
	ldr	w0, [x29, #32]
.Lchosen.3:
	sxtw	x19, w0
```

Each design's text for row 2 writes instead, on aarch64, an extending
load on each path, one instruction fewer; written by hand and
assembled, not run:

```
	ldrsw	x19, [x29, #28]
	...
	ldrsw	x19, [x29, #32]
```

Row 3 on x86-64, the element tested in place:

```
	movq	-8(%rbp), %rax
	movslq	-16(%rbp), %rcx
	testl	$1, (%rax,%rcx,4)
	jne	.Lscan.4
```

and on aarch64, the element loaded and its bit tested by `cbnz`:

```
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #28]
	ldr	w0, [x0, w1, sxtw #2]
	and	w0, w0, #0x1
	cbnz	w0, .Lscan.4
```

`tst w0, #0x1; b.ne`, written by hand, would be as long as `and w0,
w0, #0x1; cbnz w0`.

The sketches below are each assessment's, given as written, and none
was compiled. Their names are the tree's where they exist and new
where the design needs them. The functions of a sketch are named for
that sketch alone; `cexpr()` and `match()` under tables are the new
expander's, not the ancestor's.

### 9.2 Row 1: `a + b`, the value wanted in a named register

**Tables.** The vocabulary first: the destination, an operand class by
degree, where a row leaves its value, and one row of the value table.

```cpp
// The destination of a value, cexpr()'s register argument: the
// register its consumer names, a return's rax or a shift's rcx, a
// register variable's, or none.  A row never chooses its result
// register; the expander does, once, for every row (backend.md 6.1).
struct dest_t
{
    unsigned named = 0;
    const symbol_t *variable = nullptr;
};

// An operand's class on x86-64, by degree: an immediate, a register
// variable's plain read, an object an instruction names in place,
// anything.  A row of class k takes an operand of k or below.
enum class shape_t { immediate, variable, memory, any };

// Where a row leaves its value: R, or where its instruction pins it,
// as pcc's NRES says.
enum class result_t { in_r, in_rax, in_rdx };

// One row of the value table, first fit (match()).
struct row_t
{
    binary_op_t op;
    shape_t right;
    std::string_view stem;
    result_t result;
    bool sets_zf;
};
```

```cpp
// The rows of + in the value table: the right operand named in place,
// $5, %ebx or -8(%rbp), Ritchie's %n,aw; then any right operand, held.
constexpr row_t add_rows[] = {
    { binary_op_t::add, shape_t::memory, "add", result_t::in_r, true },
    { binary_op_t::add, shape_t::any, "add", result_t::in_r, true },
};

// A row of two addresses for d: F, the left operand into R; the right
// operand named or held, outside R; the instruction writing R.  R is
// the destination where the left operand may hold it while the right
// one is generated: a register variable only under sreorder()'s two
// conditions, a right operand named in place that does not name it
// (c10.c:877-881).  Otherwise placed() moves the value, once.
value_t
expand(state_t &s, const row_t &row, const operation_t &b,
       const type_t &t, const dest_t &d)
{
    const int width = operation_width(t);
    const bool early = !d.variable || (row.right != shape_t::any &&
                                       !names(*b.rhs, d.variable));
    const dest_t left = early ? d : dest_t { };

    const auto l = cexpr(s, *b.lhs, left, pinned_by(*b.rhs));
    const unsigned out = held_out(s, l, *b.rhs, left.named);
    const auto r = row.right == shape_t::any ? held(s, *b.rhs, width, out) :
                                               named(s, *b.rhs, width, out);

    const reg_t into = writable(s, l, written(width, integer(t).is_signed),
                                registers_of(s, r));
    insn(s, sized(row.stem, width), { source(s, r, into), reg(into, width) });
    if (row.sets_zf)
        flagged(s, width);

    release(s, r);

    return placed(s, l, d);
}
```

and what differs on aarch64:

```cpp
// aarch64: three addresses, R written last from two sources, so no
// early F, no guard, no writable(); the classes are a 12-bit
// immediate, a bitmask and anything.
    const auto l = cexpr(s, *b.lhs, dest_t { }, 0);
    const auto r = row.right == shape_t::any ? held(s, *b.rhs, width, 0) :
                                               immediate(s, *b.rhs, width);
    const reg_t into = final_register(s, l, d);   // d where free, or l's own
    insn(s, row.stem, { reg(into, width), reg(reg_of(s, l), width),
                        source(s, r) });
```

**Destinations.** One rule, `early()`, for both kinds of destination,
and `deliver()` of 5.5 at the end:

```cpp
// Whether L of L op R may write dest before R is read, the one hazard
// of two addresses: a register a consumer names is free and stays so
// where no instruction within R pins one, held_out()'s rule; a register
// variable's register holds the variable until the instruction, so R
// is a leaf that does not name it, sreorder()'s two conditions.
bool
early(const state_t &s, const dest_t &dest, const operation_t &b)
{
    switch (dest.kind) {
    case dest_t::kind_t::reg:
        return wanted_register(s, bit(dest.reg)) && !pinned_within(*b.rhs);
    case dest_t::kind_t::variable:
        return !dest.taken && leaf_apart_from(*b.rhs, dest.variable) &&
               loads_into(*b.lhs, dest);
    default: return false;
    }
}

// L op obj for dest: L into dest's register where early() lets it, then
// the instruction with obj's place as its source; otherwise L where its
// row leaves it and deliver()'s mov after.
value_t
memory_row(state_t &s, const operation_t &b, const type_t &t, dest_t &dest)
{
    const int width = operation_width(t);
    const int valid = written(width, integer(t).is_signed);
    const auto *object = memory_operand(*b.rhs, width);

    dest_t any;
    dest_t &left = early(s, dest, b) ? dest : any;
    const auto l = rvalue_avoiding(s, *b.lhs, pinned_by(*b.rhs), left);

    const auto place = address_avoiding(
        s, *object, pins(b.op, *b.rhs) | held_out(s, l, *b.rhs, dest));

    from_memory(s, sized(arithmetic_mnemonic(b.op), width), l, place, valid,
                width, left);
    if (b.op != binary_op_t::mul)
        flagged(s, width);

    return deliver(s, l, dest);
}
```

On aarch64 there is no `early()` and no `held_out()`: the left operand
is generated for "any", the object is loaded, and
`final_destination(s, l, valid, dest)` names the `add`'s destination,
`add w0, w0, w1` or `add w19, w0, w1`. That is today's `arithmetic()`
with its two parameters made one.

**Operands.** The left operand stays a descriptor unless the right one
evaluates anything:

```cpp
// a op b, + - & | ^ in type t (backend.md 6.1).  The left operand is an
// operand, read now where the right one evaluates anything, its read
// coming first (G13).  The right one is the instruction's source: the
// immediate, the object at the operation's width (G10), or a register.
// The instruction writes the left operand's register: the one the
// consumer names, wanted, where the left operand is not held elsewhere;
// the right operand is generated with wanted left for last until then.
operand_t
operation(state_t &s, const operation_t &b, const type_t &t, unsigned wanted)
{
    const int width = operation_width(t);
    const int valid = written(width, integer(t).is_signed);
    const unsigned pinned = pinned_by(*b.rhs);

    auto l = operand(s, *b.lhs);
    if (evaluates(*b.rhs))
        read_now(s, l, pinned_within(*b.rhs) ? 0 : wanted, pinned);

    const bool owned = l.form == form_t::held && !borrowed(s, l.value);
    auto r = operand_avoiding(s, *b.rhs, pinned | (owned ? 0 : wanted));

    const reg_t left =
        written_register(s, l, valid, wanted, registers_of(s, r));
    insn(s, sized(arithmetic_mnemonic(b.op), width),
         { as_source(s, r, width), reg(left, width) });
    flagged(s, width);

    release(s, r);

    return l;
}
```

For `return a + b` with both operands in slots, `b` evaluates nothing,
so nothing is read early; `written_register()` loads `a` into `rax`,
and the instruction names `b`. For `r = a + b`, `into_t` still names
`ebx`, and `sreorder()`'s two conditions on the right operand stay,
since they concern its evaluation, which comes before the instruction.
On aarch64 `as_source()` loads the right operand, and
`final_destination()` picks the destination at the instruction.

**Selection.** The pass and the generator, apart:

```cpp
// PASS (shared text; the target answers right_form() and writes_last)
const node_t &
select_operation(pass_t &p, const binary_t &b, const expr_t &e, dest_t dest)
{
    const auto o = p.target.ordered(b);                        // 5 + x as x + 5
    const int width = operation_width(e.type);
    const unsigned own = p.target.pins(o.op, *o.rhs);
    const auto rf = p.target.right_form(o.op, *o.rhs, width);  // immediate | memory_t | held_t
    const bool to_left = !p.target.writes_last &&              // wanted_left(): x86
        dest.kind == dest_t::kind_t::named && !pinned_within(*o.rhs) && !(own & (1u << dest.reg));
    const auto &l = select(p, *o.lhs, to_left ? dest : dest_t { });
    const auto &r = select(p, *o.rhs, dest_t { });
    return p.record(e, operation_form_t { o.op, rf }, dest, { &l, &r },
                    { .valid = written(width, integer(e.type).is_signed),
                      .zero_flag = flag_operator(o.op) ? width : 0,
                      .pins = l.facts.pins | r.facts.pins | own,
                      .writes_dest = to_left || p.target.writes_last });
}
// GENERATOR, x86-64: asks the record, and the register stack
value_t
generate(state_t &s, const operation_form_t &f, const node_t &n)
{
    const auto &left = *n.kids[0], &right = *n.kids[1];
    const int width = operation_width(n.e.type);
    const unsigned out = n.facts.writes_dest ? 1u << n.dest.reg : 0;   // held_out()'s job
    const auto l = generate_avoiding(s, left, pinned_by(right.e));
    const right_t r = std::visit(overloaded {
        [&](const memory_t &m) {   // b's address after L (G13), read at b's width (G10)
            return right_t { .place = address_avoiding(s, m.object, out), .width = width }; },
        [&](const immediate_t &i) { return right_t { .immediate = imm(i.value) }; },
        [&](const held_t &) {
            return right_t { .value = generate_avoiding(s, right, out), .width = width }; },
    }, f.right);
    const reg_t to = deliver_to(s, n.dest, l, registers_of(s, r));    // M: rax if free, else L's
    insn(s, sized(arithmetic_mnemonic(f.op), width), { source(s, r, bit(to)), reg(to, width) });
    flagged(s, n.facts.zero_flag);
    release(s, r);
    return result(s, l, to, n.facts.valid);
}
```

On aarch64 `right_form()` never returns `memory_t` and `writes_last`
is true, so the `add` takes the name.

All four reach today's listings for row 1. They differ in where the
hazard of two addresses sits: an `early` guard in the expander under
tables, one `early()` for both destinations under destinations,
`evaluates()` and `read_now()` under operands, and under selection a
fact the pass records, `to_left`, with the register's freedom asked at
emission, `deliver_to()`. On aarch64 no design needs the guard.

### 9.3 Row 2: `r = c ? x : y` into a `register long`

**Tables.** A consumer asks a pure `match()` before anything is
generated, where `lands_in()` mirrors the rows by hand:

```cpp
// Whether the row that computes e, under the conversion to, writes R
// itself.  match() is pure, so a consumer asks before anything is
// generated, where lands_in() mirrors the rows by hand.
bool
writes_r(const expr_t &e, const type_t *to)
{
    const row_t *row = to ? match_conversion(e, *to) : match(e);

    return row && row->result == result_t::in_r;
}

// ?: as cexpr() compiles it (c10.c:429-443), in text both generators
// include, as c1-statements.hh is: the condition's jump, the first arm
// for the destination, the second for the register the first arm's
// value is in, a move only where an arm's value is elsewhere.  A
// widening of the conditional, to, goes to each arm, whose conversion
// row loads and extends at once.  A register variable is each path's
// destination where one arm's row writes R; else the arms join.
value_t
quest(state_t &s, const expr_t &e, const type_t *to, dest_t d)
{
    const auto &c = as< conditional_t >(e);
    const auto incoming = before_paths(s, e);
    const int no = new_label(s);
    const int end = new_label(s);

    if (d.variable && !writes_r(*c.yes, to) && !writes_r(*c.no, to))
        d.variable = nullptr;

    cbranch(s, *c.cond, no, false);

    const auto yes = arm(s, *c.yes, to, d);
    const dest_t join = d.variable || d.named ? d : dest_in(s, yes);
    placed(s, yes, join);
    jump(s, end);

    place_label(s, no);
    placed(s, arm(s, *c.no, to, join), join);

    place_label(s, end);

    return joined(s, join, to ? *to : e.type);
}
```

**Destinations.** Each arm for the same destination, the paper's
Figure 5, where one arm's row writes it:

```cpp
// e ? a : b for dest: the condition's jump and each arm for the same
// destination, the paper's figure 5, where that writes no more than a
// join: one arm's row writes the destination itself, lands_in(), the
// one predicate each target keeps.  Otherwise the arms join in one
// register, dest's where it names one and else the first arm's, and
// deliver() writes dest after the join.  before_paths() gives every
// held value one location on both paths; the assignment's preserve()
// has copied every held read of the variable.
value_t
conditional(state_t &s, const expr_t &e, dest_t &dest)
{
    const auto &c = as< conditional_t >(e);
    const auto incoming = before_paths(s, e);

    const int no = new_label(s);
    const int end = new_label(s);
    cbranch(s, *c.cond, { no, false });

    const bool paths = lands_in(*c.yes, dest) || lands_in(*c.no, dest);
    dest_t arm = paths ? dest : joining(s, dest);

    dest_t first = arm;
    const auto yes = deliver(s, rvalue(s, *c.yes, first), first);
    const int valid = s.values[yes.id].valid;
    if (!paths && arm.kind == dest_t::kind_t::any)
        arm = { .kind = dest_t::kind_t::reg, .reg = reg_of(s, yes) };
    release(s, yes);
    jump(s, end);

    place_label(s, no);
    dest_t second = arm;
    const auto other = deliver(s, rvalue(s, *c.no, second), second);
    const int other_valid = s.values[other.id].valid;
    release(s, other);

    place_label(s, end);

    return deliver(s, arrived(s, arm, std::min(valid, other_valid)), dest);
}
```

`cast()` hands the widening down with the destination, `inner.widen =
&c.arg->type`, and `deliver()` writes the extension wherever no row
did.

**Operands.** A conversion composes with its operand, and each arm is
forced into the variable on its own path:

```cpp
// A conversion of an operand to `to`, 6.3, with no instruction: a
// widening of a signed object in memory is its load extended (G10); of a
// constant, the constant of the new type; of a held value not valid to
// the width, the extension pending to the force.
operand_t
converted(state_t &s, operand_t o, const int_t &from, const int_t &to)
{
    if (to.width <= from.width || o.form == form_t::address)
        return o;

    if (o.form == form_t::constant)
        o.constant = widened(o.constant, from);
    else if (o.form == form_t::memory && from.is_signed)
        o.extend_to = to.width;
    else if (o.form == form_t::held && s.values[o.value.id].valid < to.width)
        o.extend_to = to.width;

    return o;
}

// Each arm converted as the assignment converts the conditional and
// forced into the variable's register on its own path (chapter 4); a
// read of r itself writes nothing.  preserve() and before_paths() have
// run before the first jump (6.7).
void
conditional_into(state_t &s, const expr_t &e, const int_t &to,
                 const symbol_t *r)
{
    const auto &c = as< conditional_t >(e);
    const auto incoming = before_paths(s, e);
    const int no = new_label(s);
    const int end = new_label(s);

    cbranch(s, *c.cond, no, false);
    auto yes = converted(s, operand(s, *c.yes), integer(c.yes->type), to);
    into(s, yes, variable_register(r), represented(r));
    jump(s, end);

    place_label(s, no);
    auto no_arm = converted(s, operand(s, *c.no), integer(c.no->type), to);
    into(s, no_arm, variable_register(r), represented(r));

    place_label(s, end);
}
```

The function names no register except through `into()`, so both
targets can share it. One case is not settled: when both arms are
values in one register, two calls, a join is an instruction shorter,
and taking the join needs the second arm's shape before the first path
closes, which is a predicate of the tree, as bb736a0's `widens_arms()`
is.

**Selection.** The tree is `assign(r, cast<long>(conditional<int>))`.
The pass's rule for the cast hands the widening to the arms where one
arm widens with no instruction of its own, today's `widens_arms()`,
and the conditional is then selected with `to = long`:

```cpp
// PASS
const node_t &
select_conditional(pass_t &p, const expr_t &e, dest_t dest, const type_t *to)
{
    const auto &k = as< conditional_t >(e);
    const auto &c = select_condition(p, *k.cond);
    const bool paths = dest.kind == dest_t::kind_t::variable && (to || e.type == computed_type(dest));
    const auto &yes = select_arm(p, *k.yes, paths ? dest : join_dest(dest), to);
    const auto &no = select_arm(p, *k.no, paths ? dest : join_dest(dest), to);
    if (paths && (yes.facts.writes_dest || no.facts.writes_dest))      // lands_in(), widens_free()
        return p.record(e, paths_t { delivery(yes, dest), delivery(no, dest) }, dest,
                        { &c, &yes, &no }, { .valid = represented(dest.variable), .borrowed = dest.variable });
    return p.record(e, join_t { }, dest, { &c, &yes, &no },
                    { .valid = std::min(yes.facts.valid, no.facts.valid) });   // a move lowers it: M
}
delivery_t
delivery(const node_t &arm, const dest_t &dest)
{
    if (arm.facts.writes_dest) return delivery_t::in_place;          // movslq -16(%rbp), %rbx
    if (arm.facts.borrowed == dest.variable) return delivery_t::none; // r = c ? r : y
    return delivery_t::move;                                          // a call: movslq %eax, %rbx
}
// GENERATOR, x86-64 (aarch64: the same text, its own instructions)
value_t
generate(state_t &s, const paths_t &f, const node_t &n)
{
    const auto incoming = before_paths(s, n.e);      // 6.7: one location on both paths
    const int no = new_label(s), end = new_label(s);
    generate_condition(s, *n.kids[0], no, false);    // cmpl $0, -12(%rbp); je
    path(s, *n.kids[1], f.yes);                      // movslq -16(%rbp), %rbx
    jump(s, end);
    place_label(s, no);
    path(s, *n.kids[2], f.no);                       // movslq -20(%rbp), %rbx
    place_label(s, end);
    return read(s, n.dest.variable);
}
void
path(state_t &s, const node_t &arm, delivery_t how)
{
    const auto v = generate(s, arm);
    if (how == delivery_t::move) establish(s, arm.dest.variable, reg_of(s, v));
    release(s, v);
}
```

With shared forms, `std::visit` demands `generate(paths_t)` with a
conversion on aarch64 too, which writes `ldrsw x19, [x29, #28]` on
each path; 553c54e added that by hand after d0b66bd. aarch64's
`writes_dest` facts are wider than x86-64's, since every integer row
there writes its destination last: a table of the pass per target.

All four get aarch64's extending load on each path from text both
targets share, and each keeps one decision that looks ahead into the
arms: `writes_r()`, a query of a pure `match()`, under tables;
`lands_in()` under destinations; the shape of the second arm under
operands; and the facts `writes_dest` and `borrowed`, synthesized by
the pass, under selection, whose `join_t`'s width, lowered by a move,
is a register fact left at emission.

### 9.4 Row 3: `if (p[i] & 1) break;`

**Tables.** The rows of `&` in the condition table, Ritchie's cc81:

```cpp
// The rows of & in the condition table, Ritchie's cc81, "& as in if
// ((a&b)==0)" (table.s:1454-1469): an object and an immediate, test
// $c, obj; a right operand named in place; two held values.  test
// writes no result, so no row has a destination, and each sets ZF, so
// the jump follows bare (backend.md 7.1).
enum class cc_form_t { on_object, named, held };

struct cc_row_t
{
    shape_t left, right;
    cc_form_t form;
};

constexpr cc_row_t cc_and[] = {
    { shape_t::memory, shape_t::immediate, cc_form_t::on_object },
    { shape_t::any, shape_t::memory, cc_form_t::named },
    { shape_t::any, shape_t::any, cc_form_t::held },
};

// A condition by its rows: the test, then jne or je by sense.  With
// no row, the value and cmp $0, rcexpr()'s fallback to regtab and a tst
// (c10.c:350-370).  The if is the jump to the break's target, sense 1,
// by c1-statements.hh's if_jump(), which no table touches.
void
cc_expand(state_t &s, const operation_t &b, int width, int label, bool sense)
{
    const cc_row_t *row = match(cc_and, b, width);
    if (!row) {
        value_branch(s, b, width, label, sense);
        return;
    }

    if (row->form == cc_form_t::on_object)
        condition_on(s, "test", *memory_operand(*b.lhs, width),
                     immediate_of(*b.rhs, width), width);
    else
        test(s, b, width);

    jump_if(s, sense ? binary_op_t::ne : binary_op_t::eq, true, label);
}
```

On aarch64, which tests a register with `cbz` and sets no flags, the
condition table holds the comparisons alone, and `&` takes the
fallback.

**Destinations.** The row writes its own branch for a control
destination:

```cpp
// a & b for a control destination and no value: test, which sets the
// flags the and would and writes no result, then the jump on ZF; the
// paper's binop under a pair of labels, the row writing its own branch
// where flag_branch() reads the zero_flag record today.  An object in
// memory against an immediate is the test of the object itself.
void
bit_test(state_t &s, const operation_t &b, int width, const control_t &to)
{
    const auto mnemonic = sized("test", width);
    const auto *object = memory_operand(*b.lhs, width);

    if (object && immediate(*b.rhs, width)) {
        const auto place = address(s, *object);
        insn(s, mnemonic, { immediate_of(*b.rhs, width), operand(s, place) });
        release(s, place);
    }
    else {
        dest_t any;
        const auto l = rvalue_avoiding(s, *b.lhs, pinned_by(*b.rhs), any);
        const auto r = binary_operand(s, b.op, *b.rhs, width);
        insn(s, mnemonic, { source_of(s, r, l, width), reg(reg_of(s, l), width) });
        release(s, r);
        release(s, l);
    }

    jump_if(s, to.sense ? binary_op_t::ne : binary_op_t::eq, true, to.label);
}

// The statement, shared: a body that is one jump hands the condition
// that jump's target as its control destination, the paper's figure 6.
void
if_jump(state_t &s, const if_t &i, int target)
{
    cbranch(s, i.cond, { target, true });

    if (i.no)
        statement(s, *i.no);
}
```

On aarch64 the row delivers its value to the test, the paper's
`CG_store O effect (Lt, Lf)`, and nothing changes. The paper needs the
same special case for a body that is a `break` (Figure 6, p. 10), so
`if_jump()` is a renaming, not a gain.

**Operands.** The element stays an object in memory not yet read, and
the test reads it:

```cpp
// *(p + i) at a scale: p's value the base and i the index, forced in the
// order written, p before an i that evaluates (G13); the object in
// memory, not yet read.
operand_t
element(state_t &s, const expr_t &p, const expr_t &i, const type_t &pointee)
{
    auto base = operand(s, p);
    if (evaluates(i))
        read_now(s, base, 0);
    const value_t b = in_value(s, base);

    auto index = operand(s, i);  // (long) i: i in memory, extending to 64
    const address_t a { .base = b, .index = in_value(s, index),
                        .scale = size_of(pointee) };

    return { .form = form_t::memory, .place = a, .type = integer(pointee) };
}

// & as a condition (7.1): test of the operands, an object in memory as
// the left one read by the test; the result is the flags.
operand_t
tested(state_t &s, const operation_t &b, int width)
{
    auto l = operand(s, *b.lhs);
    if (evaluates(*b.rhs))
        read_now(s, l, 0);

    auto r = operand(s, *b.rhs);
    if (l.form == form_t::memory && r.form == form_t::memory)
        read_now(s, l, 0);

    insn(s, sized("test", width),
         { as_source(s, r, width), as_source(s, l, width) });
    release(s, l);
    release(s, r);

    return { .form = form_t::flags, .cond = binary_op_t::ne };
}
```

`cbranch()` calls `tested()` and then a `jump_on()` that writes `cmp
$0` first for any form but the flags. On aarch64 `element()` keeps the
cast as the address's extension, `[x0, w1, sxtw #2]`, as `index_of()`
does now, and the descriptor is forced, since there is no flags row
for `&`.

**Selection.** x86-64's condition table, `cbranch()`'s choice today,
as the pass:

```cpp
// PASS: x86-64's condition table, cbranch()'s choice today
condition_form_t
select_condition_x86(pass_t &p, const expr_t &e)
{
    const int width = integer(e.type).width;
    if (is_bit_and(e)) {
        const auto o = ordered(as< binary_t >(e));                 // 1 & x as x & 1
        if (const auto *obj = memory_operand(*o.lhs, width); obj && immediate(*o.rhs, width))
            return test_in_place_t { select_address(p, *obj), literal(*o.rhs), width };
        return test_t { select(p, *o.lhs, { }), p.target.right_form(o.op, *o.rhs, width), width };
    }
    if (is_comparison_node(e)) return select_comparison(p, e);   // zero_compared(), compared()
    if (flag_condition(e)) return flags_of_row_t { select(p, e, { }) };
    if (const auto *obj = memory_operand(e, width)) return compare_in_place_t { select_address(p, *obj), 0, width };
    return value_against_zero_t { select(p, e, { }), width };
}
// select_address(p[i]) = indexed_t { base = load(p), index = extending_load(i), scale = 4 }:
// one address form, read by every consumer of the node (S1)
// GENERATOR, x86-64
void
generate(state_t &s, const test_in_place_t &f, int label, bool sense)
{
    const auto place = generate_address(s, f.address);  // movq -8(%rbp), %rax; movslq -16(%rbp), %rcx
    insn(s, sized("test", f.width), { imm(f.constant), operand(s, place) });  // testl $1, (%rax,%rcx,4)
    release(s, place);
    jump_if(s, sense ? binary_op_t::ne : binary_op_t::eq, true, label);       // jne: test set ZF
}
```

aarch64's condition table has no flags rows; it selects
`value_against_zero_t`. The `if` is already a decision of the tree:
its body is one jump (`src/c1-statements.hh:477-503`).

No design changes row 3's listing on either target. What they show is
the shape the choice takes: rows in a table under tables, a row that
writes its own branch under destinations, a flags form under operands,
and a variant of condition forms under selection, which is tables'
first step (12.1) with its result recorded.

## 10. How each keeps the promises

### 10.1 G13, order as written

- **Tables.** A row has no way to name an order. The expander
  generates `F` before the right operand for every row, so the
  ancestor's `SS; F` cannot be written. The one exchange left is the
  constant on the left, a normalization made before `match()`, as
  `ordered()` makes it now.
- **Destinations.** A destination changes which register a value is
  written to, never the order of evaluation. Each row generates its
  operands in the text's order. The one place a destination could
  break G13 is a variable's register written before a later operand
  reads it, and `early()` is the only gate for that, as
  `operates_in()` is today.
- **Operands.** One rule decides when a delayed read is made: before
  the first later operand that writes code before its own read. A
  constant, a name and a register variable's read write nothing before
  their own read; a dereference loads its pointer first, and a call
  writes a great deal. A row forces its operands in the order the
  source writes them, and `before_paths()` makes every pending read
  before the first jump. The rule is simple enough to trust only if
  `insn()` enforces it (6.3); without that check nothing keeps it, and
  both sources show what the default does.
- **Selection.** The pass records forms whose operands keep the tree's
  order. A form may read an operand where it stands only when the
  operand has no evaluation, a constant or a register variable's read,
  which is today's rule, and the generator visits the left operand
  first. The pass chooses a form, never an order.

### 10.2 G10, each access at its own width

- **Tables.** The class `memory` is an object of exactly the
  operation's width, `memory_operand()`'s
  (`src/c1-x86-64.cc:1311-1330`). A narrower object is `any`, loaded
  at its own width by its own row, and a conversion row reads its
  object at its own width. The rule lives in one classifier per
  target.
- **Destinations.** Accesses are written by `load()`, the memory
  operand, `test` and the extending load. A destination never changes
  their width: `ldrsw` and `movslq` read 4 bytes whatever register
  they write.
- **Operands.** A memory descriptor carries its object's type.
  `as_source()` names the object only at exactly the instruction's
  width, with no extension pending; otherwise it writes one load at
  the object's width with its extension, `movsbl` or `ldrsw`. A cast
  never narrows the read. This is today's rule, stated once.
- **Selection.** `memory_operand()` admits only an object exactly as
  wide as the operation, the record carries the object's type, and a
  promoted `char` keeps its extending load.

### 10.3 The job of `preserve()`

`preserve()` copies each held read of a register variable before the
variable is written, so that the read keeps the value it read: "G13,
an operand's side effects complete before the next operand starts, and
i = i++ + ++i has one value" (`src/c1-x86-64.cc:738-752`;
`src/c1-aarch64.cc:580-599`).

- **Tables.** It stays in the register stack. The expander calls it
  wherever a register variable is about to be written: before an early
  `F`, in `placed()`, and in the rows that operate in place. That is
  today's job at today's points.
- **Destinations.** It stays where it is called today: the assignment
  copies the held reads of the variable before it hands the variable
  down as the destination, and `before_paths()` saves them at a branch
  (`src/c1-stack.hh:341-361`). The paper has no counterpart, because
  its variables live in memory.
- **Operands.** It is the force of the borrowed form (6.4), the one
  delay G13 allows freely.
- **Selection.** Which reads are held is a fact of the tree: the
  borrowed reads that the ancestors hold as left operands. So the pass
  records "copy these" at each write, and `before_paths()`'s saves at
  a branch work the same way; the copy's register is chosen at
  emission.

The queue's entry on a right operand that writes the variable
(`TODO:68-80`) says the copy "would need the name handed to it". Under
the first three designs it stays the same work, since the copy sits
inside the right operand, off the destination's path. Under selection
it is a small local change: the outer destination reaches `preserve()`
as an inherited attribute, with no threading.

## 11. What comes next under each

The features ahead are `enum`, structs and unions in memory, local
initializers, structs by value across calls, variadic definitions,
bit-fields and floating point. `enum` is the parser's under every
design: c1 sees constants.

### 11.1 Floating point

- **Tables.** The table's best case. The ancestor shares one body
  between integer and floating rows by a type letter: `%n,aw` and
  `%nf,ad` both write `F; IB2 A2,R` (`table.s:283-287`). SSE, x86-64's
  floating-point instructions, keeps the memory operand, `addsd
  -8(%rbp), %xmm0`, so x86-64's `memory` class carries over; aarch64's
  `fadd d0, d1, d2` has `add`'s shape.
- **Destinations.** `dest_t`'s register takes a class, so a `return`
  names `xmm0` or `d0` as it names `rax`, with one constructor. The
  test destination covers a floating comparison.
- **Operands.** A register class on the held form, as tcc's `rc`;
  constants from labeled data; the conversion made by the load; flags
  with parity.
- **Selection.** Its forms are new alternatives of the variants, and
  `std::visit` makes both generators cover each one: where the design
  helps most.

None gives the rest:

- a second register stack, its `take()`, its spills and what a call
  destroys;
- the conventions' vector registers, and the count of them in `al` at
  a variadic call;
- x86-64's unordered compare, which shows only in PF, the parity flag
  (recalled), so `==` needs two jumps, as the ancestor's long compares
  do (`table.s:1358-1452`), and the width rule does not apply;
- constants in `.data` (`c10.c:497-500`);
- a `register double` on x86-64, which has no callee-saved vector
  register (recalled; `ancestor.md` 11.4).

### 11.2 A struct in memory

- **Tables.** A member is an address plus a constant. The address
  classes fold that today and would under tables. Assignment is a copy
  by size, a special case in the ancestor (`strasg()`,
  `c11.c:1066-1124`) and in pcc (`STASG`'s registers,
  `pcc/order.c:288-296`), not a row.
- **Destinations.** The design's best case: the paper's `local_i`
  destination becomes a place, an `address_t`. A struct assignment, a
  compound literal and a local brace initializer are generated into
  the object's place, and each subobject is delivered at its offset. A
  call that returns a struct passes its destination's address as the
  hidden result pointer, `rdi` or `x8` (recalled), and no copy
  follows. Member accesses are operand forms and gain nothing.
- **Operands.** Where the descriptor helps most. A member is the
  address descriptor plus an offset, Wirth's `Field`
  (`ORG.Mod:260-269`), M1; today each tree matcher, `lvalue_offset()`,
  `named_offset()`, `summed()` and `index_base()`, would need a member
  case of its own. Struct assignment is the assignment row on two
  memory descriptors, a copy sequence chosen by size (`ancestor.md`
  11.1), and a local initializer constant stores through the address
  plus an offset.
- **Selection.** A member access is one more address form, an lvalue's
  address plus a constant, which the address family already handles in
  the `&x + c` case. Assignment needs a copy form and a destination of
  kind "place". Local initializers and designators are stores into a
  place.

Not given by any: the layout, the copy sequence and its registers, and
a G10 statement on the copy's widths.

### 11.3 A struct by value

- **Tables.** The conventions' classification and the hidden pointer
  belong to the call sequence. A 16-byte value in `rax:rdx` is a value
  in two registers; the ancestor's templates spell that for a `long`,
  with `R+` and `A1+` (`table.s:108-110`), but rcc's register stack
  holds one register per value.
- **Destinations.** A small result needs a register pair as its
  destination, `rax:rdx` or `x0:x1` (recalled). An argument is already
  a hidden local, so its destination is its slot.
- **Operands.** A struct argument is copied into the hidden argument
  slot, its eightbytes loaded by their class, and a result is held in
  a register pair, tcc's `r2` (`tcc.h:485-486`).
- **Selection.** Calls need the classification per target and a place
  destination for the hidden return pointer. The design gives neither.

Not given by any: the classification of the System V and AAPCS64
conventions, the hidden result pointer, and a held form of two
registers.

### 11.4 A bit-field

- **Tables.** Rows: the ancestor's `bic` and `bis` (`table.s:258-272`,
  `table.s:1228-1260`), x86-64's `and` and `or` memory forms, and
  aarch64's `ubfx`, `sbfx` and `bfi` (recalled).
- **Destinations.** `s.f = e` delivers to a field's place by a read,
  an insert and a store; `if (s.f)` is the test row on the object with
  a mask, today's `condition_on()`.
- **Operands.** A memory descriptor with a bit offset and a width: a
  load, a shift and a mask. tcc's `gv()` handles bit-fields first
  (`tccgen.c:1853`).
- **Selection.** Its forms are the load of the storage unit, a shift
  and a mask, and a read, modify and write for a store.

Not given by any: the layout of the fields, the width of the
read-modify-write under G10, which must name the storage unit, and the
literal compound assignment to a field: the read, the operation, the
truncation to the field and the insertion (`ancestor.md` 11.3).

### 11.5 A variadic definition

Under every design, the register save area, `va_list`, `va_arg` and
the prologue are outside the design.

### 11.6 What the roadmap shows

Each design's case on the roadmap is larger than its count. A
floating-point register, a place and a pair are each a new
destination, one line of `dest_t` (5.5). A member is an address plus a
constant, one operation of the address descriptor (6.5). A floating
form is one alternative that `std::visit` makes each target cover
(7.9). Floating point is where rows as data, with a type column, would
pay most (4.5). What no design gives is the same list for all four:
layout, the conventions' classification, the call sequence, the save
area, and a second register stack.

## 12. How each would be reached

Every assessment found its design reachable in place, a step at a
time, with every pinned listing byte-identical until a deliberate
change. Each named a first step that can be proved inert alone: the
check of each is that the base compiler and the changed one write the
same listing for every `.c` under `tests`, on both targets.

### 12.1 Tables

In place, a row at a time, is possible: `match()` falls back to
today's function for any operator or context with no rows, as
`rcexpr()` falls back to `regtab`. But byte identity needs each row to
take registers in today's order, so rows must be extracted from
today's functions, not derived anew. An expander written fresh renames
registers across the pins, and every renamed listing is one more diff
to read.

Counted at 4b95569:

| Part | Lines | Fate |
|---|---|---|
| `parse.cc`, `lex.cc`, `ast.hh` | 6715 | stays |
| `c1-common.*`, `c1-stack.hh`, `c1-statements.hh` | 2240 | stays; `?:` and `&&` move in |
| x86-64: state, writing, frame, hooks, prologue, call | 863 | stays |
| x86-64: addresses | 446 | stays, as the address classes |
| x86-64: the consumer's register, pins, register variables, rows | 3296 | rewritten |
| aarch64: state, writing, frame, hooks, prologue, call | 1006 | stays |
| aarch64: addresses, constants | 926 | stays, as the classes and the constant row |
| aarch64: the destination, rows | 1597 | rewritten |

So 4893 of the two generators' 8134 lines are rewritten.

The first step: x86-64's condition table. The choice `cbranch()` makes
for a condition that is not a comparison and not `&&`, `||`, `!` or a
comma is spread over `flag_operator()`, `flag_condition()`,
`zero_compared()` and `flag_branch()` (`src/c1-x86-64.cc:3928-4086`).
It becomes a `constexpr` table of rows and a pure `match()` over the
operand classes, and `cbranch()` dispatches on a row's form to the
functions that exist.

### 12.2 Destinations

The data half is reached in place, one row at a time. The control half
is in the tree already, as `cbranch()` and `effect()`; folding them
into one function that also carries the fourth category gains nothing
on these machines. The steps, counted at 4b95569:

1. **The first step, inert alone.** On aarch64, the 16 functions that
   take both `into_t *` and `wanted`, 515 lines, take one `dest_t {
   into_t *into; unsigned wanted; }`, and `final_destination()` takes
   it. The same two values reach the same code, so no listing can
   change. At d0b66bd the functions are 17, `step()` among them.
2. The same for x86-64's 12 functions that take both; then those that
   take one only: 33 and 7 on x86-64, 12 and 2 on aarch64.
3. The defaults removed one function at a time. Each caller that
   passed nothing writes `dest_t {}`, which is inert and shows every
   omission.
4. `s.in_place` folded into `dest_t`, inert.
5. `deliver()` merges the deliveries: it removes or merges 158 lines
   in 18 functions, `negates_in()`, `negated_in()`, `arm_into()`,
   `second_path()`, `computed_into()`, `settle()`,
   `named_destination()` and `rvalue_in()` among them. It is inert but
   at `arm_into()`, which is the queue's entry on `movl %ebx, %ebx`
   and the first deliberate change.
6. The listing changes, each with its pin diff: that entry, the
   quotient moved out of `rax` and back, the `return`'s moves on
   aarch64, then the shared `conditional()` and the aarch64 half of
   the entry on a `?:` that keeps its join.

What stays: `parse.cc` (5390 lines), `lex.cc` (833),
`c1-statements.hh` (576, with `control_t` for `(label, sense)`),
`c1-common.cc` (1090, `retypes()` aside), the frame, the calls, the
data and the register stack. What changes: 88 functions, 2147 lines,
318 of them naming a destination. No rewrite is needed.

### 12.3 Operands

In place, a row at a time. What stays: `parse.cc`, `c1-statements.hh`,
`c1-stack.hh` (361 lines, where `held_t` becomes the held form), the
frame, the calls, the switches and the data. What is rewritten into
the descriptor, its address arithmetic and its force functions,
counted at 4b95569:

| File | Functions | Lines |
|---|---|---|
| `c1-x86-64.cc` | 30 | 804 |
| `c1-aarch64.cc` | 9 | 312 |
| `c1-common.cc` | 8 | 166 |
| total, of 10374 lines of the generator sources | 47 | 1282 |

The new code is an estimate, not a count: `ORG.Mod`'s item core is
about 148 lines for one RISC target with no widths; tcc's `gv()` and
its x86-64 `load()` are 353; the assessment guessed 300 to 500 lines
per target for rcc. The remaining rows change their signatures, not
their logic, and the destination mechanisms stay.

The first step: `right_t` becomes `operand_t` with a form tag, in both
generators. Its three exclusive fields become the constant, held and
memory forms; `binary_operand()`, `right_operand()`, `displacement()`,
`addend()` and aarch64's `product()` return it, and the 18 code lines
that name `right_t` switch on the form. The second: `plus(address_t,
constant)` and `indexed_by(address_t, index, scale)`, with the five
x86-64 address matchers rewritten on them, byte-identical; the two
address entries of the queue then follow as deliberate pin changes.

### 12.4 Selection

In place, one family at a time, each step leaving the 272 listings
byte-identical and the probes passing.

The first step is tables': move x86-64's condition choice into
`select_condition_x86()`, which returns a variant, and let `cbranch()`
visit it. It touches 10 functions and 219 lines: `cbranch()`,
`comparison_branch()`, `flag_branch()`, `test()`, `modified()`,
`compared()`, `condition_on()`, `flag_condition()`, `zero_compared()`
and `flag_operator()`. It is inert by construction: the same
predicates in the same order, and each alternative's code unchanged.
aarch64's condition family, `cbranch()` and `test_zero()`, is 77
lines.

The steps after it:

- the address family as `select_address()`, inert first, then the
  entry on `p[1] + i` as a deliberate pin change; this is also
  operands' second step, the one every design keeps and that structs
  build on;
- the facts declared per form, with a debug assertion against today's
  predicates;
- the conditional as `paths_t` and `join_t`, with the delivery step;
  then the entries on the variable's own read and on a `?:` that keeps
  its join, as deliberate changes.

Sizes, counted at d0b66bd:

- stays: `parse.cc` (5390 lines), and 157 functions and 2241 lines of
  the frame, the calls, the statements, the data and the register
  stack;
- moves to the pass as it is: 81 predicates, 1025 lines;
- touched at most: 150 row functions with a branch in them, 3565
  lines, an upper bound, since many of those branches only pick a
  mnemonic by width;
- thrown away: the defaulted parameters, `into_t *` (25 on x86-64, 23
  on aarch64) and `wanted` (15 and 18), replaced by reads of the
  record.

### 12.5 The documents, and a pin of the decisions

`backend.md` 6.3 has 29 rows and `aarch64.md` 6.3 has 28, each cell a
paragraph of forms, and the generator commits moved the documents with
the code: 43 of the 46 changed `backend.md`, `aarch64.md`, `cost.md`
or `ancestor.md`, 4617 lines beside 7728 in `src/c1-*`. The 51 to
d0b66bd changed 10014 lines of `src/`, 5467 of `docs/` and 48094 of
`tests/`. Each design changes the upkeep:

- **Tables.** The code's rows become the ground truth of the forms. A
  document's row can name the code rows it describes, and a script can
  check a target's rows against those names. The upkeep then moves
  from prose in four documents for every shape to one row and the
  sentence naming it. That holds only with the script; without it
  there are two tables to drift apart, as the ancestor's description
  of its classes drifted from its code. The prose that says why a
  boundary stands, most of `backend.md` chapter 4, stays.
- **Destinations.** `backend.md` states the rule twice: chapter 4's
  "The value computed for a register variable" (`docs/backend.md:281`)
  and 6.1's "The register a consumer names" (`docs/backend.md:757`),
  and 21 of its lines say "consumer names". Its calls 16 and 19 make
  the same call twice, and call 19 says so: "The cost is a second
  hand-down beside the variable's" (`docs/backend.md:2450`). b0cad23,
  the shift's count in `rcx`, changed chapter 4, 6.1, 6.3, 6.5 and
  call 19. Under `dest_t`, 6.1 holds one section, "The destination",
  with its kinds, the soft rule, `early()`'s hazard and `deliver()`;
  chapter 4 keeps only the variable's representation; the cells of 6.3
  lose their clauses about destinations and gain a column, "writes its
  destination last" or "first, under `early()`"; calls 16 and 19
  become one. A new destination is then one line of 6.1's list.
- **Operands.** One chapter states once the five forms, the force
  functions, G13's forcing rule, the width rule and "a result stays
  where its instruction left it". 6.3's tables gain two columns, the
  forms each row takes and the form it returns, and five prose
  passages become lines of a table of forms: "An object in memory as
  the operand", "A name and a constant", "A slot and an index", "A
  widening by the load" and "A condition on an object in memory". Each
  restates G13's boundary today, and `backend.md` says "no evaluation"
  on 8 lines. The documents of the destination rules do not change.
- **Selection.** Each row of 6.3 names its form and its rule. The long
  conditional sentences, "where ... and not where ...", become the
  pass's rules, written once and shared by both targets, where
  `aarch64.md` repeats them today. Named forms would let `backend.md`,
  `aarch64.md` and `cost.md` cite one vocabulary; the risk is two
  vocabularies, prose and form names, to keep in step.

If the selected record can be printed, a test can pin the decisions
beside the assembly. What that buys:

- exact counts: 3 of the 10 entries at d0b66bd are checked by "the
  count over `tests`", and the entry on a constant offset added before
  an index waits at "count first" because a scan of the listings for
  its shape matches 44 functions, many of them a local's address
  (`TODO:98-112`), where a dump of address forms counts the shape
  exactly;
- rule changes that move decisions without moving listings, made
  visible;
- a probe's failure traced to a decision.

It finds no wrong code, which is the probes' job. It costs 272 more
files, two pins to regenerate per deliberate step, and form names that
become an interface, so that a rename churns every pin.

## 13. How the four fit together

They are components, not rivals, and each organizes a different part
of the generator:

- **destinations**, where a value goes: the data destination and the
  control destination, handed down;
- **operands**, what a value is so far: its form, an address above
  all, handed up;
- **tables**, which row takes which operands, as data that can be
  asked, with its result and its needs declared;
- **selection**, when the choice is made: ahead of generation, and
  kept.

Each kept part splits along one line, the line chapter 7's count
draws:

- one destination type with one delivery step: the destination is an
  inherited attribute, a fact of the tree; the delivery step is
  register work;
- one operand description with the address operations: an address form
  is synthesized, a fact of the tree; "a value held in register r" is
  register work;
- a pure `match()` over declared rows: the match is the pass, its
  result recorded.

So the pass of chapter 7 is where the tree halves of the three kept
parts meet, and the delivery step at emission is where their register
halves meet. What the pass adds of its own is the record: decisions
kept, printable, and visited exhaustively on every target. It needs
the destination part to work.

The overlaps:

- The destination is in two of the first three. Tables' first part is
  destination-driven generation's data destination, and `cbranch(e,
  label, sense)` is already its control destination. Selection's
  record carries it as an attribute.
- The operand classes are in two. Tables' classes are close to Wirth's
  modes and tcc's value kinds: a descriptor holds the same fact,
  observed once the operand is generated, that `dcalc()` computes from
  the tree. Selection's `facts_t` holds it as a synthesized attribute.
- The Seventh Edition compiler has all four in miniature: the register
  argument, a destination; the degrees, recorded ahead by `optim()`;
  the classes, computed by `dcalc()` against the registers left; and
  the rows, matched at generation.
- Two assessments name the same first step, x86-64's condition choice
  made one table or one variant (12.1, 12.4). The address descriptor
  with its two operations is the step every design keeps: tables would
  hold it as its address classes, destinations leaves it as it is, and
  operands and selection make it their second step.

What none addresses:

- the register decisions themselves: the 34 sites of class M stay at
  emission under every design, and so do the reservations around
  x86-64's pinned registers, 201 lines;
- the hazard of two addresses, which each design keeps as one rule
  where today there are several;
- the work outside expression selection: 18 of the 53 items all four
  classify are the same work under all four;
- what the roadmap needs beyond forms: layout, the conventions'
  classification, the call sequence, the save area of a variadic
  function, and a second register stack.

No assessment found its design worth a rewrite, and each found a part
of it worth taking. Nothing on this page takes it.

## 14. The record

### 14.1 The sources and the terms under which they are quoted

**The Seventh Edition compiler.** `c10.c` to `c13.c`, `c1.h`,
`table.s` and `cvopt.c` of `usr/src/cmd/c/`, read in a Seventh Edition
distribution in the archive of The Unix Heritage Society. Where
`ancestor.md` quotes the same lines, they stand at the same file and
line. The compiler's output is that of its own binaries, from the same
distribution, run under a PDP-11 emulator. The licence is the one
`ancestor.md` 13.1 describes, Caldera International's letter of
January 23, 2002, which grants the rights to use, modify and
distribute the source code of 16-bit UNIX Versions 1 to 7 and of 32V,
and requires that redistributions of source code retain its notice,
its conditions and its disclaimer. This page quotes the sources, so it
reproduces them, as the letter gives them:

> Copyright(C) Caldera International Inc. 2001-2002. All rights
> reserved.
>
> Redistribution and use in source and binary forms, with or without
> modification, are permitted provided that the following conditions
> are met:
>
> Redistributions of source code and documentation must retain the
> above copyright notice, this list of conditions and the following
> disclaimer. Redistributions in binary form must reproduce the above
> copyright notice, this list of conditions and the following
> disclaimer in the documentation and/or other materials provided with
> the distribution.
>
> All advertising materials mentioning features or use of this
> software must display the following acknowledgement:
>
> This product includes software developed or owned by Caldera
> International, Inc.
>
> Neither the name of Caldera International, Inc. nor the names of
> other contributors may be used to endorse or promote products
> derived from this software without specific prior written
> permission.
>
> USE OF THE SOFTWARE PROVIDED FOR UNDER THIS LICENSE BY CALDERA
> INTERNATIONAL, INC. AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
> IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
> WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
> ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL, INC. BE
> LIABLE FOR ANY DIRECT, INDIRECT INCIDENTAL, SPECIAL, EXEMPLARY, OR
> CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
> SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
> BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
> LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
> NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
> SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**The paper.** R. Kent Dybvig, Robert Hieb and Tom Butler,
"Destination-Driven Code Generation", Indiana University Computer
Science Department, Technical Report 302, February 1990. It was read
whole in a PDF reproduction of the report, and its page numbers are
the report's own. It carries no licence; the page quotes its rules and
a few sentences for commentary.

**Oberon.** `ORG.Mod`, the code generator of the Oberon-07 compiler
for RISC, dated by its header 16.4.2016, 4.4.2017 and 31.5.2019, and
`ORP.Mod`, the parser that calls it, both N. Wirth's, from the Project
Oberon sources at `people.inf.ethz.ch/wirth/ProjectOberon/Sources/`.
Line numbers are those of the files with their line ends made LF. The
files carry no licence text; the page quotes a few lines for
commentary. Wirth's book "Compiler Construction" and its Oberon-0
generator were not read here.

**tcc.** `tccgen.c`, `tcc.h`, `x86_64-gen.c` and `arm64-gen.c` of the
Tiny C Compiler, from its `mob` branch through the GitHub mirror
`TinyCC/tinycc`. `tccgen.c` and `tcc.h` carry Fabrice Bellard's
copyright, and the two target files their own authors'. `tccgen.c`,
`tcc.h` and `x86_64-gen.c` are under the GNU Lesser General Public
License, version 2 or later, as their headers say; `arm64-gen.c`
permits copying and distribution provided its copyright notice and its
notice are preserved. The page quotes a few lines of `tccgen.c` and
`tcc.h` for commentary, and cites the two target files by line alone.

**lcc.** `gen.c`, the emitter shared by every target, and the machine
descriptions `x86linux.md`, `x86.md`, `mips.md` and `alpha.md`, from
the GitHub repository `drh/lcc`. lcc's terms are those of its own
licence file, which was not fetched here; the page cites the files by
line and quotes only fragments of templates.

**pcc.** The amd64 back end's `table.c`, `order.c`, `local2.c` and
`macdefs.h`, from a GitHub mirror of pcc's repository. Each file
carries a licence of the BSD kind; the page cites them by line and
reproduces none of their text.

### 14.2 What was read

The survey read, for each design, the tree's generator sources whole,
`backend.md` whole, the chapters of `aarch64.md`, `ancestor.md`,
`cost.md`, `guarantees.md` and `shape.md` it cites, the queue's
chapter "The back end", and the message of every generator commit from
2026-11-15. For tables it read the Seventh Edition's c1 and its table,
lcc's and pcc's files named above; for destinations the paper, whole;
for operands `ORG.Mod`, `ORP.Mod` and the tcc files named; for
selection the Seventh Edition's `optim()` and `dcalc()` and lcc's
`gen.c`. Every line quoted on this page was read again at its file and
line, at d0b66bd for the tree.

### 14.3 What is recalled

These facts come from outside the files read, and none was checked
here:

- that `lburg`'s labeller works from the leaves up, by cost; and the
  terms of attribute grammars;
- Chez Scheme's later compiler of several passes, and the retirement
  of V8's full-codegen in 2017;
- x86-64: that an unordered floating-point compare shows only in PF;
  that the System V convention has no callee-saved vector register;
- aarch64: `ubfx`, `sbfx` and `bfi`;
- the conventions: a struct returned in `rax:rdx` or `x0:x1`, and
  through a hidden pointer in `rdi` or `x8`;
- lcc's licence.

### 14.4 How the claims are kept

- A line of a source is quoted at its file and line, and a citation
  without a quote names the lines read. The paper is cited by page.
- A count is a script's, kept with the survey's other evidence, and
  names its commit: 4b95569 for the first three assessments, d0b66bd
  for the fourth. A count made again for this page is at d0b66bd and
  says so.
- "rcc writes" is a listing `rcc -S` wrote at d0b66bd. An earlier
  listing is a pin at the commit named.
- A form rcc does not write is a listing written by hand, and said to
  be so.
- A sketch is an assessment's C++, given as written; none was
  compiled.
- A class of chapter 8 is an assessment's judgment, with its
  mechanism; the totals are counts of those judgments.
- The queue is quoted with org's `=code=` and `~code~` written in
  backticks.
