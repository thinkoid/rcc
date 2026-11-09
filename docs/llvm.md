# rcc: the LLVM sprint

rcc's first back end was LLVM's. This page is the record of that
sprint: why LLVM was chosen, what a literal pipeline on `llc` is and
how far it carried the project, where the code generator's own
judgment showed through and how each case was met, what the
measurement said, the three ways out that were weighed, and why the
back end is now rcc's own (`backend.md`). It is written as a
tutorial, in the register of `findings.md`, for a reader who knows C
and has seen assembly but has not been inside a code generator. It
keeps what the sprint taught, so that the knowledge did not leave the
tree with the LLVM pipeline: the cost model's flag table, its
chapter on what the machine did on its own and its verification by
flags live here, in chapters 3 and 5.

## 0. tl;dr

- LLVM was chosen for three reasons: an own front end a few thousand
  lines long against a dependency, not a fork; textual IR as the
  interface, which moves slowly and diffs as text; and the
  guarantees expressed as flags the front end withholds from an
  optimizer. Each reason held. The third turned out to describe a
  document, not a pipeline.
- The first turn: the guarantees needed no optimizer to be true.
  They are the machine's own behaviour, reached by emitting the plain
  instruction and nothing in between. So the optimizer was never run,
  and `llc` was held to one instruction per IR instruction by
  FastISel and twenty-one switches. The cost model was written beside
  the guarantees to say what that costs.
- The literal pipeline carried two slices, `register`, an oracle of
  divergence programs, a cost corpus of 48 pinned listings and a
  measurement of six programs in days. It works.
- Its judgment showed through in eight places, each small: a skipped
  unused result, an implicit definition that lost G5 for a register
  variable, a folded compare of a value with itself, the same fold on
  a test of 0, a strength reduction by a power of two, a switchless
  late pass, an allocator that rematerializes a frame address, and a
  materialized condition. Four were defended by the emitter, three
  could only be recorded, one was rcc's own.
- Three ways out: patch LLVM, which is a fork; write machine IR
  after selection, which works and pins the project to an unstable,
  target-specific format; write the back end. The third was chosen,
  because the model is the point and a tool that has to be fought is
  rewritten.
- LLVM stays as what any cc is to a compiler that writes assembly:
  `cpp`, `as` and `ld`, which have no judgment about the program. Its
  lasting contribution is the written record of where a modern code
  generator's judgment hides, which the new back end is drawn
  against.

## 1. Why LLVM

### 1.1 The charter

"An experimental descendant of Ritchie's C compiler, using LLVM to
bring early C's machine-oriented programming model to modern
targets, with explicit guarantees for selected operations that ISO C
leaves undefined." Three things are in that sentence: a lineage, a
means, and a list of promises. The list came first (`guarantees.md`,
`rationale.md`), before any code. The means came second, and this
chapter is why it was LLVM.

### 1.2 Three design calls

**Own tree, LLVM as a dependency.** The alternative was a fork of
Clang. Clang's semantic analysis is ISO through and through, and a
compiler whose promises differ from ISO's at seventeen points fights
it at every one of them, then carries the merge treadmill forever. A
front end near Seventh Edition C is a few thousand lines; the
original `c0` was that size. Nothing in the goal paid for being
inside the LLVM tree.

**Depend on the IR, not the C++ API.** The front end writes LLVM IR
as text and hands the file to `llc` and `clang`, as a compiler hands
assembly to `as` and `ld`. No build-time dependency beyond an
installed LLVM; the IR text changes slowly between releases and is
diffed as text in a test suite, where the C++ API changes every
release. This call held to the end and is why the retirement is
cheap: nothing in rcc links LLVM.

**The guarantees live in what the front end withholds.** LLVM's IR
carries flags and metadata that license the optimizer's reasoning:
`nsw` says a signed add does not overflow, `inbounds` says a pointer
stays inside its object, `!tbaa` says two accesses of different types
do not alias, `poison` says a value may be anything. Each guarantee
of the list is the absence of one of them: G1 is `add` without `nsw`,
G6 is `getelementptr` without `inbounds`, G10 is no `!tbaa`, G5 is
`freeze` instead of `poison`. The never-emitted table, at the end of
`guarantees.md` until the retirement and of `rationale.md` still, is
this call written out. The first plan was to emit
that IR and hand it to `opt -O2`: an optimizer that reasons only from
what it is told, told nothing, would be a conforming compiler for
rcc's promises. G15, since withdrawn, was the complement: a
per-declaration grant by which the programmer handed `nsw` and
`inbounds` back to the optimizer on one variable.

### 1.3 What LLVM promised

Targets: x86-64 today and aarch64 by changing a triple. A register
allocator of production quality. An instruction selector that knows
every addressing mode and every instruction form. An assembler and a
linker. And the IR itself as a thinking tool: a typed, explicit,
single-assignment language in which "what does this construct
compile to" has a written answer before any machine is chosen.

## 2. The first turn: from the optimizer to the literal pipeline

### 2.1 What the optimizer would have needed

The rationale records three mechanisms designed for draft 1 of the
guarantees to survive `opt -O2`. The guard of G4: a compare and a
branch around every division by a non-constant divisor, because
LLVM's `sdiv` by zero is undefined behaviour in the IR's own
semantics and the optimizer may reason from it. The round trip of
G8: `ptrtoint` and `inttoptr` around a pointer whose provenance the
optimizer might otherwise track, so a load reads the bytes at the
address and not the object the optimizer believes is there. The
laundering of G16: a call made through an empty inline `asm`, so the
optimizer cannot see the callee and rewrite the call. Each of the
three is code the source did not write, added to stop a reasoner
from reasoning.

### 2.2 The realization

Every guarantee in the list is a statement about the machine. Two's
complement wraps. `idiv` faults on zero. A `mov` reads the bytes at
the address, whatever they belong to. A loop runs until its test
says stop. The guards exist only because something between the
source and the machine reasons about the program; take that thing
away and the plain instruction delivers the promise with nothing
added. So the optimizer was not held off with flags; it was not run.
The commit that did it says: "rcc stops handing its IR to an
optimizer. The instruction written is the instruction executed." The
guard of G4, the mask of G3 and the round trip of G8 left the emitter
the same day, and the "lost" clauses left the guarantees: nothing is
lost to an optimizer that does not run.

This is the point at which the project became a literal compiler,
and the cost model (`cost.md`) was written as the second document:
if the compiler does only what the source says, the programmer needs
to know what each thing the source says costs.

### 2.3 Why `llc` still

`llc` is LLVM's code generator alone: it reads IR and writes an
object, running no mid-level optimizer. It had a selector built for
`-O0` that takes each IR instruction as it comes, FastISel, and a
switch on nearly every rewriting pass after selection. Forcing the
one and throwing the others gave a pipeline in which one IR
instruction became one machine instruction, and LLVM's register
allocator, the one thing the project wanted from a code generator,
still ran. That pipeline is chapter 3.

## 3. The literal pipeline

### 3.1 How `llc` is built

A code generator is a sequence of passes over the function. In
`llc` they fall into five groups. First, IR-level preparation passes
that rewrite the IR for the selector's benefit (`codegenprepare`,
loop strength reduction, constant hoisting). Second, instruction
selection, which turns each IR instruction into machine instructions
on an unbounded supply of virtual registers. Third, machine-level
optimizations on virtual registers (machine CSE, machine LICM,
sinking, peephole, the combiner). Fourth, register allocation, which
maps virtual registers to the machine's and inserts spills, then the
frame is laid out. Fifth, late passes on physical registers (branch
folding, tail duplication, block placement, scheduling, late
cleanup) and the assembly printer.

Most passes that rewrite the program carry a `-disable-<pass>`
option, a `cl::opt` the pass's author added, and `llc` honours it on
the command line. The flag table of `main.cc` was the set of those
options, found by probing: a construct was compiled, its listing
read, and where a pass had done something the source did not say,
the option that held it back was found and added. The table was
verified on LLVM 23.1.3, and every flag was a hidden option, not a
stable interface.

### 3.2 Two selectors

LLVM has two instruction selectors. SelectionDAG builds a graph of
each basic block and pattern-matches it; its x86 lowering alone is
sixty-six thousand lines, and that is where an optimizing compiler's
instruction-level algebra lives: it will forward a stored local to
its next load, merge operations, fold constants, choose between a
multiply and a shift. FastISel, 2400 lines of generic code and 4200
for x86, was written so that `-O0` compiles quickly: it walks the IR
one instruction at a time and emits the instruction for it, falling
back to SelectionDAG for anything it does not handle. `-fast-isel`
forces it; `-fast-isel-abort=3` makes `llc` fail rather than fall
back, so that a listing is never silently the other selector's. On
the cost corpus, dropping these two flags changed every row but one:
the folding selector shrinks nearly every listing.

### 3.3 `-O1`, not `-O0`

LLVM ties its register allocators to the optimization level. At
`-O0` only the fast allocator is permitted, and it spills every value
to the stack at every block boundary, so a loop reloads its counter
from memory each iteration and no `register` variable could ever be
a register. At `-O1` the greedy allocator runs. With FastISel forced
and the rewriting passes held back, selection at `-O1` was as
literal as at `-O0` and the registers were real. This is the single
most important fact of the sprint about LLVM's code generator: the
literal selection and the good allocator can be had together, and
the price is the list of switches.

### 3.4 The flag table

What each flag held back, as the cost model recorded it. The flags
are LLVM 23.1.3's.

| Flag | What it holds back |
|---|---|
| `-O1` | the real register allocator; `-O0` allows only the fast one, which spills across every block |
| `-fast-isel` | FastISel: the instruction written is the instruction selected; no folding of `a + 0`, `x * 3`, `x / 4` |
| `-fast-isel-abort=3` | never fall back to the folding selector; fail loudly instead |
| `-relocation-model=pic` | `clang` links position-independent executables |
| `-disable-cgp` | no IR rewriting before selection |
| `-disable-lsr` | no loop strength reduction |
| `-disable-constant-hoisting` | constants stay where they are written |
| `-disable-machine-licm` | nothing moves out of a loop |
| `-disable-machine-cse` | nothing is computed once that was written twice |
| `-disable-machine-sink`, `-disable-postra-machine-sink` | nothing moves down a branch |
| `-disable-machine-dce` | no dead-instruction removal past the selector's own |
| `-disable-peephole` | no instruction rewriting |
| `-disable-x86-lea-opt` | no `lea` rewriting |
| `-disable-x86-domain-reassignment` | no domain changes |
| `-disable-early-ifcvt` | no branch to conditional move |
| `-x86-cmov-converter=false` | no conditional move to branch |
| `-enable-misched=false` | no reordering |
| `-disable-copyprop` | no copy propagation |
| `-disable-branch-fold` | branches stay as written |
| `-disable-tail-duplicate`, `-disable-early-taildup` | no block duplication |
| `-disable-block-placement` | blocks stay in the written order |
| `-disable-post-ra` | no post-allocation scheduling |
| `-no-stack-coloring` | one slot per local, never shared |

### 3.5 Which rows caught which flag

The cost corpus was the table's proof. Each program under
`tests/cost` held one construct, one rule or one flag, and its pin
was the object's disassembly by `objdump`. Dropping one flag at a
time and watching which pins changed gave the map below; a flag that
changed no row stayed in the table because the pass it held back
exists and a later slice might have given it something to do.

| Flag dropped | Rows that changed |
|---|---|
| `-fast-isel` with its abort | every row but `unused` |
| `-relocation-model=pic` | `string` |
| `-disable-cgp` | `cgp`, `early-taildup` |
| `-disable-constant-hoisting` | `constant-hoisting` |
| `-disable-machine-licm` | `constant-in-loop`, `invariant` |
| `-disable-machine-cse` | `twice`, `zero-test` |
| `-disable-machine-sink` | `logical` |
| `-disable-branch-fold` | `block-order`, `cgp`, `early-taildup`, `if`, `register-undef` |
| `-disable-tail-duplicate` | `conditional`, `early-taildup`, `if`, `select` |
| `-disable-early-taildup` | `early-taildup` |
| `-disable-block-placement` | twelve rows |

`-disable-lsr` changed nothing because a counter in a slot is no
induction variable, and a `register` counter that never feeds an
address gives the pass nothing to reduce. `-no-stack-coloring`
changed nothing because no lifetime marker was ever emitted, so no
two slots could share. Without `-relocation-model=pic` the string
literal's address became a `movabs` with an absolute relocation.
The parser's folding of constant expressions later removed the
witnesses of `invariant` and `twice`, whose `12 * 34` became the
immediate 408, so neither row changed any more when its flag was
dropped; the table is the measurement as it was run.

### 3.6 The three things the compiler may do

The cost model's rule drew the line the pipeline was held to. The
compiler decides which register holds a value, the one-to-one
machine form of each IR instruction, and the calling convention.
Nothing else: no folding, no moving, no merging, no reordering, no
deleting, no conversion between a branch and a conditional move.
Under "the machine form of one instruction" two things were accepted
that look like folds and are not: `lea` for an add whose result goes
to a third register, and the scaled-index addressing mode
`(%rax,%rcx,4)` for a `getelementptr` that feeds a load, since the
scaled add is still performed, by the access itself. The register
discipline of `backend.md` is drawn against this list: it keeps the
three things and takes the first of them, the registers, into rcc's
own hands.

## 4. How far it went

In days, on the literal pipeline: the first slice, the integer
types, the scalar operators, `if`, `while`, `for`, `return`, calls,
through the driver, with the oracle running the first divergence
programs; the second slice, pointers, `void *`, arrays, subscripts,
`&` and `*`, pointer arithmetic and comparison, casts, `sizeof`,
string and character literals, with G17 added for the writable
literal; the cost corpus, one program per row of the cost model with
its listing pinned; `register`, honoured as a variable that is never
stored, with structured SSA (`phi` at two-arm joins and loop
headers) built by the emitter; the measurement of six programs in
plain and tuned form. At the end `make test` ran 9 oracle rows and
48 cost rows green and `make bench` built and agreed on all six.

Two facts about the convention were learned on the way and carry
over to the new back end unchanged. A `char` or `short` parameter
travels as `int`: the ABI leaves the upper bits of the register
undefined and the callee extends, so the signature says `i32` and
the callee truncates at entry. A function with no calls keeps its
locals in the red zone below `rsp`, and every pin of a leaf function
shows `-0x8(%rsp)` with no `sub`.

The method also carries over whole: a document written before the
code, a corpus of one program per promise or per row, a pin read by
hand against its row before it is adopted, and an oracle that runs
the same program under `clang -O2` and under rcc and compares exit
statuses. None of this is LLVM's; all of it was built on LLVM's
pipeline and is what the new back end is verified with.

## 5. Where the judgment showed

Eight places, in the order they were met. Each is told as the
pipeline presented it: the construct, what the listing showed, where
in LLVM it happened, and how rcc answered. The last five are the
findings of the measurement and have their own page, `findings.md`;
here they are summarized.

### 5.1 An unused result is not computed

`a + 0;` as a statement emitted nothing, and a division whose result
nothing reads emitted nothing, so it could not fault. The selector's
driver loop (`SelectionDAGISel.cpp`, `isFoldedOrDeadInstruction`)
walks a block bottom-up and skips an instruction that writes no
memory, is no terminator, and whose result no later instruction has
asked for, before FastISel sees it; there is no switch. This was
recorded in the cost model as the one departure from
"the instruction written is the instruction executed", and left
there: nothing a programmer writes for its effect is affected, and
the alternative, a fake use of every result, would have cost an
instruction per statement. Under the new back end the question does
not arise: the generator writes the instruction for the node, and a
statement's value is simply not stored.

### 5.2 An implicit definition loses G5

The first `register` variable without an initializer was emitted as
`freeze i32 undef`, once, at its declaration: the IR's way of saying
"some value, the same at every read". FastISel selects `undef` as
`IMPLICIT_DEF`, a pseudo-instruction that defines a register with
no particular content, and LLVM's handling of implicit definitions
marks every use of that register as undefined, so the allocator need
not keep anything live between two reads. Two reads, two registers,
two values: `if (x < 10) return 1; if (x >= 10) return 2; return 3;`
could return 3. G5 says the uninitialized object holds one value.
The answer was one `alloca` that nothing writes and one `load` and
`freeze` at the declaration; the loaded value is then an ordinary SSA
value the allocator must keep. One instruction, a `mov` from the
frame, bought the guarantee back. The cost row `register-undef` pins
it. The new back end needs none of this: a register variable is a
register, and a register nobody writes holds what it held.

### 5.3 A compare of a value with itself is folded

`register int x = n; return x == x;` compiled to `mov $1,%al`.
`FastISel::optimizeCmpPredicate` (`FastISel.cpp`) tests whether the
two operands of an `icmp` are the same IR value and, if so, rewrites
the predicate to its constant answer; the x86 selector then emits
the constant, and for a branch an unconditional jump. A slot local
escaped it, since its two reads were two loads and two values; a
`register` variable, being one SSA value, did not. Two answers were
possible: accept it as the machine's rule, or make the compare
happen. `a == a` is a compare was judged more in the spirit of the
project, and the emitter freezes the right operand when the two
operands have one name, so FastISel sees two values. `freeze` of a
non-poison value is the value; it selects as a `COPY`, which the
allocator merges away for a register (`cmp %eax,%eax`) and which
costs one `mov` for a constant. Cost row `register-compare`.

### 5.4 The same fold on a test of 0

`if (0)` compiled to an unconditional jump and, before the parser
folded constant expressions, `!0` to a constant: the emitter's test
of a condition and its `!` compared the value with 0, and when the
value was the constant 0 the two operands were one `Value*` again. The
three sites that write an `icmp`, a comparison, a condition and `!`,
were given one helper that applies the same freeze, so a test of a
constant is a compare too: `cmp` of two registers each cleared by an
`xor`, and the body of the `if` kept. Cost row `zero-test`. Once the
parser folded constant expressions, `!0` was the constant 1 before any
IR was written, and only `if (0)` still reached the helper. Under the
new back end the code for a constant condition is the code for a
condition, an immediate loaded and compared, and nothing looks at its
value.

### 5.5 A condition is materialized

Sort's inner loop ran 23 instructions, of which 17 were the two
tests and the `&&` that joins them: each compare set a byte, masked
it, widened it to an `int`, and the joined value was masked, widened
and compared with 0 once more before the jump. This one is rcc's
own, not LLVM's. The tree holds an `int` for every condition, as C
says, and the emitter tested it as an `int`: `icmp`, `zext` to
`i32`, `icmp ne 0`, `br`. FastISel selected exactly what was written.
A jump-code generator, Ritchie's `cbranch`, that emits a compare and
a conditional jump and never a value, could have been written at
the IR level too, and is chapter 7 of `backend.md`. The sprint's
lesson here is about the emitter's design, and it transfers.

### 5.6 The selector strength-reduces by a power of two

`x * 4` is `shl $2`; for an unsigned `x`, `x / 4` is `shr $2` and
`x % 4` is `and $3`. Two sites in FastISel's generic code,
`fastEmit_ri_` and `selectBinaryOp`, test the constant operand with
`isPowerOf2_64` and change the opcode; signed division is left alone
unless the IR marks it `exact`, which rcc never writes, which is why
the cost row for `x / 4`, written with `int`, never saw it. No
switch: this is the selector's own code, not a pass. The fix in the
pattern of 5.3, a `freeze` on the constant so that `dyn_cast<ConstantInt>`
fails, was available at the price of one `mov`. `findings.md`
chapter 2.

### 5.7 A pass with no switch drops a reload of a constant

`MachineLateInstrsCleanup`, added unconditionally after register
allocation (`TargetPassConfig.cpp`), removes a load of an immediate
into a register that already holds it on every path. Its source
declares no `cl::opt`. In the Collatz step, the second `idiv` by 2
divided by the `ecx` loaded for the first, across a branch. This is
"which register holds a value" and was arguable as inside the three
things, but it also meant the flag table's "one pass has no switch,
the machine combiner" was wrong in count, and that the count could grow
with any release. `findings.md` chapter 3.

### 5.8 The allocator rematerializes a frame address

`register int *first = a;` with `a` a local array is a `lea` from
`rsp` at each use, inside the loop too; the allocator recomputes the
address rather than keeping it, since a `lea` is the cheapest value
to rematerialize. Inside the three things, recordable as a row, and
a reminder that `register` under this pipeline was a request to an
allocator, not an allocation. `findings.md` chapter 6.

Two more findings of the measurement were the front end's: a
negative constant was a subtraction at run time, and an array size
had to be one token, both for want of a constant-expression
evaluator in the parser. They are `findings.md` chapters 4 and 5 and
are closed by `backend.md` chapter 6.4, whichever back end.

## 6. What the measurement said

Six programs, each as written and as tuned by hand for a literal
compiler, under `clang -O2` and under rcc, milliseconds of user time,
minimum of five runs, on one x86-64 desktop with LLVM 23.1.3:

| program | clang -O2 plain | rcc plain | rcc tuned | clang -O2 tuned |
|---|---|---|---|---|
| collatz | 292 (1.00) | 2377 (8.14) | 494 (1.69) | 291 (1.00) |
| fib | 260 (1.00) | 388 (1.49) | 347 (1.33) | 266 (1.02) |
| matmul | 284 (1.00) | 2171 (7.64) | 940 (3.31) | 279 (0.98) |
| sieve | 288 (1.00) | 1843 (6.40) | 770 (2.67) | 299 (1.04) |
| sort | 275 (1.00) | 1634 (5.94) | 1209 (4.40) | 280 (1.02) |
| strings | 281 (1.00) | 2370 (8.43) | 760 (2.70) | 270 (0.96) |

The plain source ran 1.5 to 8.4 times as long under the literal
pipeline as under the optimizer. Hand tuning, `register` on the hot
variables, pointer walks, `!=` loop ends, invariants hoisted by the
programmer, a shift where the programmer knows the divisor, brought
it to 1.3 to 4.4, and cost the optimizer nothing. The gap that
remained was in the places the pipeline chose: sort's 4.4 is the
materialized condition of 5.5, matmul's 3.3 is the slot loads of the
locals the programmer did not declare `register`. The measurement
is chapter 4 of `cost.md`; its purpose, to say how far a literal
machine lands from the optimizer and how much of the gap the
programmer closes by hand, is served. The chapter now holds the run
under c1, and this pipeline's table is its 4.1.

## 7. The three ways out

Reading the listings for the measurement produced the five
findings, and the findings produced the question: is the project
fighting its tool? The ledger that answered it:

| component | lines |
|---|---|
| FastISel, generic | 2377 |
| FastISel, x86 | 4225 |
| FastISel, aarch64 | 5162 |
| SelectionDAG, core | 125947 |
| x86 lowering for SelectionDAG | 65919 |

The judgment being fought was about twenty lines of FastISel's
generic file, one unconditional `addPass`, and the allocator's
rematerialization. Small. But each release can add to it, no option
reaches the selector's own code, and no one who wrote the code
generator wanted a literal compiler. Three ways out:

**Patch LLVM.** Add the missing switches: a `cl::opt` around the
power-of-two rewrite, one around the compare fold, a
`-disable-machine-latecleanup` in the pattern of the others. Each is
a few lines. But the x86 selector is created inside the target
library and cannot be replaced from outside; the switches mean a
patched LLVM build, which is the fork the first design call rejected,
and rcc would then require it. Sending the switches upstream was
weighed and declined: a `-disable-*` for the late pass would likely
be taken, the FastISel ones would need an argument no one upstream
has a use for, and a release carrying them would be months away in
any case.

**Write machine IR.** `llc` can read a function as MIR, the textual
form of the machine instructions on virtual registers, and resume
after any pass (`-start-after=finalize-isel`). It was tried: the MIR
of a probe, fed back, produced the identical object. rcc would do
its own selection into x86 opcodes and LLVM would allocate registers
and encode; the late pass could be skipped by stopping before it and
restarting after. Declined as a narrowing selection on shifting
grounds: MIR is target-specific opcode by opcode, documented as an
unstable format, and would pin rcc to an LLVM version more tightly
than the flag table did, for the benefit of an allocator whose
choices (5.7, 5.8) were themselves on the list.

**Write the back end.** Emit assembly text and own the registers.
Ritchie's `c1` on x86-64: every local in its slot, `register`
variables in the callee-saved registers, expressions in the rest,
conditions as jumps, the System V convention at calls. Three to five
thousand lines for x86-64, a second table for aarch64. Every one of
the eight places in chapter 5 has no place to happen. `backend.md`.

## 8. The decision

The project cares about one thing most: Ritchie's programming model,
in which the programmer decides what is in a register and the
compiler writes the instruction for the operator. The charter's
"using LLVM" is a means, important as long as it benefits; a tool
that has to be fought is rewritten. The MIR path would have kept
LLVM for the allocator, and the allocator was half of what was being
fought. The own back end was chosen, early enough that the emitter
it replaces is a third of the tree and two weeks old. LLVM remains
in the pipeline as any cc remains in a compiler that writes
assembly: `cpp`, `as`, `ld`. Those have no opinion about the program.

## 9. What LLVM left behind

The sprint was not a detour. Four things it produced are the ground
the new back end stands on.

- **The map of where judgment hides.** Chapter 5 here and
  `findings.md` are, together, a written account of what a modern
  code generator does to a program even when told to do nothing: the
  selector's convenience rewrites, the switchless passes, the
  allocator's rematerialization, the materialization of a condition.
  The register discipline, the constant rule and the jump code of
  `backend.md` each answer one entry of that account.
- **The IR as a thinking tool.** Writing each construct as IR first
  forced the question "what does this mean, exactly" before "what
  instruction is this": the never-emitted table is a list of every
  assumption an optimizer makes, and each guarantee is the denial of
  one. That list is the meaning half of the project and does not
  change with the back end.
- **The method.** Documents before code; one program per row; pins
  read by hand; an oracle against `clang -O2`. The oracle stays
  exactly as it is, and `clang -O2` stays as the compiler rcc is
  measured against.
- **The three things.** The rule that a compiler may choose the
  register, the machine form and the convention, and nothing else,
  was written to hold `llc` to a line. It is now the specification
  the generator is written to meet, with the first of the three
  taken in hand.

## 10. The record

At the retirement of the LLVM pipeline:

- the emitter, `emit.cc`, and the flag table of `main.cc` left the
  tree; `-S` writes c1's assembly, and `llc` is no longer run;
- the cost model's flag table, its chapter on what the machine did
  on its own and its verification by flags left `cost.md`, and their
  content is chapters 3 and 5 here;
- the table of `cost.md` was rewritten from c1's pins;
- the IR clauses of `guarantees.md` became instruction clauses;
- `shape.md`'s pipeline, emitter and driver chapters were rewritten;
- the pins of c1 moved up a directory over the LLVM pins, the two
  scripts lost their `--native` mode, and `make test` runs both
  corpora through c1;
- `findings.md` stayed as the detailed record of the five findings,
  and closes with what became of them;
- `rationale.md` stayed as the argument for the guarantees, whose
  probes of what `clang -O2` does are the reason there are guarantees
  at all.

Git keeps the emitter.
