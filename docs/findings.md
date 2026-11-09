# rcc: what the measurement found

The measurement of `cost.md` chapter 4 wrote six programs twice, read
their listings line by line and timed them. Its first run, now the
chapter's 4.1, was under the LLVM pipeline, the one the tree compiled
through until c1 replaced it (`llvm.md`), and every listing on this
page is that pipeline's.
Reading the listings turned up five facts about the pipeline that
the cost model did not record, and two of them contradicted what it
said. This page explains each one from the ground up: the terms, the
mechanism, the evidence, what it meant for the thesis, and the
choice it left open. It is written as a tutorial, for a reader who
knows C and has seen assembly but has not read an instruction
selector. The queue held the same five as entries with a fix and a
check each; chapter 8 says what became of them under c1.

## 0. tl;dr

- The instruction selector rewrites `x * 4` to a shift, unsigned
  `x / 4` to a shift and unsigned `x % 4` to a mask, on its own, with
  no switch to stop it. Signed division is left alone. Chapter 1 of
  the cost model says `x / 4` divides; for an unsigned `x` it did
  not.
- A pass that runs after register allocation, with no switch, removes
  a second load of a constant into a register that already holds it.
  The cost model's flag table named the machine combiner as the only
  pass without a switch; there were two.
- A negative constant was computed at run time: `-1` was the
  operator `-` applied to `1`, the parser kept it so, and the machine
  subtracted 1 from 0 every time the program reached it.
- An array size had to be a single integer token; `int a[96 * 96]`
  was refused, though ISO C takes any integer constant expression.
- A `register` pointer initialized with a local array's address was
  not held in a register. It was recomputed from the stack pointer
  with a `lea` at each use, inside a loop too.

The first two were the machine's own rules, and the thesis had to say
where it stood on them. The third and fourth were gaps in the front
end, with one fix. The fifth was the allocator's choice, inside what
the thesis conceded to it. Chapter 8 says what became of each.

## 1. Background: how to read what follows

### 1.1 The pipeline

Until the retirement, rcc's front end read C and wrote LLVM IR as text.
IR is a typed assembly language for an abstract machine with infinitely
many registers: `%t3 = mul i32 %t2, 4` multiplies the 32-bit value `%t2`
by 4 and names the result `%t3`. Every value is assigned exactly once
(this is *static single assignment*, SSA), so a name is a value, not a
place. The IR then went to `llc`, LLVM's code generator, which turned it
into machine instructions for x86-64, and `clang` linked the object. The
method (`llvm.md` chapter 3) was to run `llc` with FastISel, its
simplest instruction selector, and with every rewriting pass switched
off, so that each IR instruction became its one machine form and nothing
in between reasoned about the program.

### 1.2 Instruction selection

*Instruction selection* is the step that picks a machine instruction
for each IR instruction. LLVM has two selectors. SelectionDAG builds
a graph of each basic block and pattern-matches it, which is where
most of an optimizing compiler's algebra happens at this level.
FastISel, written for `-O0` compile speed, walks the IR one
instruction at a time and emits the instruction for it directly,
with a small fixed set of special cases. rcc forced FastISel
(`-fast-isel`) and made `llc` fail rather than fall back to
SelectionDAG (`-fast-isel-abort=3`). The special cases are the
subject of finding 2 below: they are FastISel's own code, not a pass,
so no `-disable-*` switch reaches them.

### 1.3 Passes and switches

After selection the code is a list of *machine instructions* on
virtual registers. A sequence of *passes* then runs over it: each
pass is a function from the program to the program, and most of them
improve it in one way (hoisting out of loops, removing duplicates,
reordering blocks). LLVM's `llc` exposes a `-disable-<pass>` option
for many of them, each added by the pass's author as a `cl::opt` in
the pass's own source file. rcc's flag table, in `src/main.cc` then
and in `llvm.md` 3.4 now, was the set of those options that switched
off every rewriting pass the probes found. A pass whose author added no
option cannot be switched off from the command line; it can only be left
out by building a different pipeline. Finding 3 is such a pass.

### 1.4 Registers, slots, immediates

The machine has sixteen general registers. Values that do not fit in
them, or that the allocator decides not to keep in them, live in
*stack slots*, addressed off the stack pointer as `-0x8(%rsp)`. In
rcc every ordinary local has a slot and is loaded from it at each
read and stored to it at each write (`cost.md` chapter 2, the first
row); under that pipeline a `register` local was an SSA value, and
the allocator kept it in a register. An *immediate* is a constant
encoded inside the instruction itself: in `shl $0x2,%eax` the 2 is an
immediate, and in `mov $0x4,%ecx` the 4 is one, loaded into a register
because the `idiv` instruction takes its divisor only from a register.

### 1.5 Reading a listing

The listings below are what `objdump -d` printed for the object
that pipeline built, in AT&T syntax: source operand first, destination
last, `%` before registers, `$` before immediates. Each function begins
with its parameters being stored to their slots and loaded back, which
is the literal cost of a local (`cost.md` chapter 2) and is the same in
every listing; the instructions after that are the construct's own. A
trailing `nop` line pads the function to an alignment and means nothing.

## 2. The selector rewrites a multiply and an unsigned division by a power of two

### 2.1 The terms

*Strength reduction* replaces an operation by a cheaper one that
gives the same result. The classic cases are on a *power of two*, a
number 2^k, because of how binary numbers work:

- Multiplying by 2^k appends k zero bits: `x * 4` is `x << 2`.
- Dividing an unsigned number by 2^k drops its low k bits: `x / 4`
  is `x >> 2`.
- The remainder of an unsigned number by 2^k is those low k bits:
  `x % 4` is `x & 3`.

On x86-64 an `imul` takes a few cycles and a `div` of a 32-bit value
several times that; a shift or an `and` takes one. So every
optimizing compiler does these three rewrites, and the cost
model's chapter 1 says rcc does not: "`x / 4` divides; `x >>= 2`
shifts; choosing between them is the programmer's job."

Signed division is different. C says the quotient truncates toward
zero (6.5.6p7: "the algebraic quotient with any fractional part
discarded"), so `-7 / 4` is `-1`. An arithmetic right shift floors:
`-7 >> 2` is `-2`. The shift is wrong for a negative dividend unless
the compiler first adds `2^k - 1` to a negative `x`, which is the
sign fixup `clang -O2` emits around its `sar` for a signed `x / 4`.
FastISel does not do that fixup; it rewrites a signed division only
when the IR marks it `exact`, which rcc never writes.

### 2.2 The observation

One program, seven functions, three with a power of two, two signed
for contrast, two with 3 for contrast:

```c
int mul_four(int x) { return x * 4; }
unsigned udiv_four(unsigned x) { return x / 4; }
unsigned urem_four(unsigned x) { return x % 4; }
int sdiv_four(int x) { return x / 4; }
int srem_four(int x) { return x % 4; }
int mul_three(int x) { return x * 3; }
unsigned udiv_three(unsigned x) { return x / 3; }
```

The IR is what the source says. `mul_four` writes `%t3 = mul i32
%t2, 4`, `udiv_four` writes `%t3 = udiv i32 %t2, 4`. The listing does
not:

```
<mul_four>:
	mov    %edi,-0x8(%rsp)
	mov    -0x8(%rsp),%eax
	shl    $0x2,%eax
	ret

<udiv_four>:
	mov    %edi,-0x8(%rsp)
	mov    -0x8(%rsp),%eax
	shr    $0x2,%eax
	ret

<urem_four>:
	mov    %edi,-0x8(%rsp)
	mov    -0x8(%rsp),%eax
	and    $0x3,%eax
	ret

<sdiv_four>:
	mov    %edi,-0x8(%rsp)
	mov    -0x8(%rsp),%eax
	mov    $0x4,%ecx
	cltd
	idiv   %ecx
	ret

<srem_four>:
	mov    %edi,-0x8(%rsp)
	mov    -0x8(%rsp),%eax
	mov    $0x4,%ecx
	cltd
	idiv   %ecx
	mov    %edx,%eax
	ret

<mul_three>:
	mov    %edi,-0x8(%rsp)
	mov    -0x8(%rsp),%eax
	imul   $0x3,%eax,%eax
	ret

<udiv_three>:
	mov    %edi,-0x8(%rsp)
	mov    -0x8(%rsp),%eax
	mov    $0x3,%ecx
	xor    %edx,%edx
	div    %ecx
	ret
```

Three rewrites: `mul` by 4 is `shl $2`, `udiv` by 4 is `shr $2`,
`urem` by 4 is `and $3`. The two signed functions divide, as the
cost corpus row `div-four` pins and as chapter 1 claims. The two
functions with 3 multiply and divide, since 3 is no power of two.

### 2.3 Where it happens

In `FastISel.cpp`, the selector's generic part shared by every
target. `FastISel::selectBinaryOp` handles every two-operand
arithmetic instruction. When the second operand is a constant
integer (`dyn_cast<ConstantInt>`), it takes the *register-immediate*
path, and on the way:

```cpp
    // Transform "urem x, pow2" -> "and x, pow2-1".
    if (ISDOpcode == ISD::UREM && isa<BinaryOperator>(I) &&
        isPowerOf2_64(Imm)) {
      --Imm;
      ISDOpcode = ISD::AND;
    }
```

and the function it then calls, `FastISel::fastEmit_ri_`:

```cpp
  // If this is a multiply by a power of two, emit this as a shift left.
  if (Opcode == ISD::MUL && isPowerOf2_64(Imm)) {
    Opcode = ISD::SHL;
    Imm = Log2_64(Imm);
  } else if (Opcode == ISD::UDIV && isPowerOf2_64(Imm)) {
    // div x, 8 -> srl x, 3
    Opcode = ISD::SRL;
    Imm = Log2_64(Imm);
  }
```

Just above the `urem` case is the signed one, guarded by `isExact()`:
a `sdiv exact` by a power of two becomes `sra`. rcc never writes
`exact` (it is a promise that the division has no remainder, which
the optimizer would reason from), so signed division is never
rewritten. This is why the `div-four` row, written with `int`, never
saw the rewrite: it tested the one case FastISel leaves alone.

None of this is a pass. It is the selector deciding which
instruction to emit for `mul`, the same place that decides `add`
becomes `add`. There is no `cl::opt` around it, and `-O0`, the
level FastISel was written for, performs it too.

### 2.4 What it means

Two statements in the record were false for these cases. Chapter
1's "`x / 4` divides" held for a signed `x` and failed for an
unsigned one. The table's rows gave `imul` for `a * b` and `div` for
the unsigned `a / b` and `a % b`, for every operand; the rows needed
a qualification for a constant power of two. The measurement itself
was written around the fact: `tests/perf/collatz.c` declares its
value a signed `long` so that `x / 2` and `x % 2` divide and the tuned
version has a shift and a mask to replace them with. Had the plain
program used `unsigned`, the selector would have done the programmer's
tuning for it, and the row would have measured nothing.

### 2.5 The choice

Two ways to settle it, the same two as for the compare fold that
the cost model recorded (`llvm.md` 5.3).

**Defeat it in the emitter.** The rewrite fires on
`dyn_cast<ConstantInt>` of the operand: the operand has to *be* a
constant in the IR. The emitter already uses this against the
compare fold: when both operands of a compare are one name, the
right one is passed through `freeze` first, and FastISel sees two
values. The same move applies here. A constant power of two as the
operand of `mul`, `udiv` or `urem` would be written as `%c = freeze
i32 4` and the instruction would use `%c`. FastISel then takes its
register-register path: the constant is loaded into a register with
one `mov`, and the instruction is `imul %ecx` or `div %ecx`. The
cost is that one `mov`, and the immediate form `imul $4,%eax,%eax`
becoming the register form; for `div` the constant was going to a
register anyway, so there the cost is nothing. The rule stays
exactly as chapter 1 states it.

**Record it as the machine's rule.** The cost model's chapter "What
the machine does on its own", now `llvm.md` chapter 5, recorded the
unused-result rule as the one departure from "the instruction
written is the instruction executed", and the compare fold as the
second, defeated. A third bullet would have said: a multiply, an
unsigned division or an unsigned remainder by a constant power of
two is selected as a shift or a mask. Chapter 1 and the rows would
have been qualified. The programmer loses nothing in power, since the
shift and the mask are what they would write; what is lost is the
simplicity of the rule.

The entry in `TODO` asked for the decision and named the check
either way: cost rows for `x * 4`, unsigned `x / 4` and unsigned
`x % 4`, pinning `imul` and `div`, or pinning the rewrite as the
machine's rule. Chapter 8 says how the retirement settled it.

## 3. A pass with no switch drops a constant already in its register

### 3.1 The terms

*Register allocation* assigns the IR's unbounded virtual registers to
the machine's sixteen. When a value is needed in two places and
keeping it in a register in between is costly, the allocator may
*rematerialize* it: compute it again at the second place instead of
carrying it. A constant is the cheapest value to rematerialize; a
`mov $2,%ecx` costs one instruction and no memory, so the allocator
freely writes the `mov` again wherever the constant is needed.

Once allocation is done, the registers are physical, and a *late*
pass can look at the result. *Dominance* is the relation such a pass
reasons with: block A dominates block B when every path from the
function's entry to B goes through A. If A loads 2 into `ecx` and
nothing on any path from A to B writes `ecx`, then at B `ecx` is
already 2, and a second `mov $2,%ecx` in B is redundant.

### 3.2 The observation

The Collatz step, taken out of `tests/perf/collatz.c`:

```c
int step(int x)
{
    if (x % 2 != 0)
        return 3 * x + 1;
    return x / 2;
}
```

The IR has two constants 2, one in the `srem` of the condition and
one in the `sdiv` of the last return, in two different blocks. Each
should be a `mov $0x2,%ecx` before its `idiv`. The listing:

```
<step>:
	mov    %edi,-0x8(%rsp)
	mov    -0x8(%rsp),%eax
	mov    $0x2,%ecx
	cltd
	idiv   %ecx
	cmp    $0x0,%edx
	setne  %al
	and    $0x1,%al
	movzbl %al,%eax
	cmp    $0x0,%eax
	je     <step+0x2b>
	mov    -0x8(%rsp),%eax
	imul   $0x3,%eax,%eax
	add    $0x1,%eax
	ret
	mov    -0x8(%rsp),%eax
	cltd
	idiv   %ecx
	ret
```

The second `idiv %ecx`, after the `ret` of the first return, has no
`mov $0x2,%ecx` before it. It divides by the `ecx` that the first
block loaded, nine instructions earlier, across a branch.

`llc` can print the program before and after any pass
(`-print-before=` and `-print-after=`). Before the pass named
`machine-latecleanup` the function has two `MOV32ri 2` into `$ecx`,
one before each `IDIV32r`; after it, one. Every pass before it had
left both.

### 3.3 Where it happens

`MachineLateInstrsCleanup.cpp` describes itself in its header:

> This simple pass removes any identical and redundant immediate or
> address loads to the same register. The immediate loads removed
> can originally be the result of rematerialization, while the
> addresses are redundant frame addressing anchor points created
> during Frame Indices elimination.

It walks the blocks in reverse post-order, remembers in each block
which register was last defined by which immediate load, carries
that knowledge into a block from its predecessors when they agree,
and deletes a load whose register already holds the same immediate.
It was written to tidy up after rematerialization, which is the
scenario above exactly: the allocator wrote the `mov $2` twice, and
the pass noticed the second was redundant.

It is added unconditionally in `TargetPassConfig.cpp`:

```cpp
void TargetPassConfig::addMachineLateOptimization() {
  // Cleanup of redundant immediate/address loads.
  addPass(&MachineLateInstrsCleanupID);
```

and its source file declares no `cl::opt`. The `-disable-*` switches
in rcc's table are each one such option; this pass has none, and
`llc` 23.1.3 offers no way to leave it out short of building the
pass pipeline by hand.

### 3.4 What it means

The cost model's flag table said: "One pass has no switch, the
machine combiner; on the probes it did nothing to a literal chain of
adds and multiplies." That sentence was wrong in count. There were
two passes without a switch, and this one did something.

Whether what it does crosses the thesis's line is the question. The
tl;dr of `cost.md` said the compiler decides three things of its
own: which register holds a value, the one-to-one machine form of
each IR instruction, and the calling convention (`llvm.md` 3.6). The
constant 2 is a value. Whether it sits in `ecx` across the branch or is
loaded again is a question of which register holds it, and of whether a
register holds it at all, which is the allocator's domain by the
thesis's own words. On that reading the pass does not add a fourth
thing; it finishes the first. What it never does is change an
instruction that operates on the program's data: no `idiv` became a
shift, no add was removed. Every instruction the program computes with
is still there.

Chapter 1's "the same expression written twice is computed twice" is
also still true: the two divisions both execute. What is computed
once is the loading of a constant, which the IR does not even write
as an instruction (a constant in IR is an operand, not a
computation; the `mov` is the machine form of using it).

### 3.5 The choice

**Record it.** The flag table's text names the pass beside the
machine combiner; the chapter on what the machine does on its own
gets a bullet saying a constant loaded into a register on every path
to a later use of the same constant is not loaded again; a cost row
pins one `mov` for one constant used in two blocks, the first
dominating the second. That was the entry's fix as queued.

**Build the pipeline without it.** `llc` can stop before a pass
(`-stop-before=machine-latecleanup`) and write the program as
*machine IR* (MIR, a textual form of the post-allocation code), and
a second `llc` can start after that pass (`-start-after=`) and
finish. The driver would run `llc` twice with a MIR file between.
It would make every instruction the IR implies appear, at the cost
of a second process, a second file format in the pipeline and a
dependency on MIR's stability across LLVM versions, for one `mov $2`.

The first reading above argues for recording.

## 4. A negative constant is a negation at run time

### 4.1 The terms

C has no negative constants. The grammar of an integer constant
(6.4.4.2) is a digit sequence and an optional suffix; there is no
sign. So `-1` in a program is two tokens, the unary `-` operator and
the constant `1`, and its value is "the negative of its (promoted)
operand" (6.5.4.3p3). The same holds for `~0`, `1 << 4` and
`96 * 96`: each is an expression built from constants, and ISO C
calls such an expression a *constant expression*, one that "can be
evaluated during translation rather than runtime, and accordingly
may be used in any place that a constant may be" (6.6p2). An
*integer constant expression* (6.6p8) is the integer-typed case,
built from integer and character constants, `sizeof` and casts
between arithmetic types.

*Constant folding* is a compiler evaluating such an expression and
substituting the result. Ritchie's `c0` did it, in a few lines of
the expression builder: when both operands of an arithmetic node are
constants, build the constant instead of the node. It is the oldest
compiler optimization there is, and it is not what the thesis's rule
is about. "The instruction written is the instruction executed" is a
rule about operations on the program's data: `x / 4` divides `x`, a
value the compiler does not know. `-1` has no data in it. It is a
spelling of a number.

### 4.2 The observation

```c
long minus_one(void) { long x = -1; return x; }
long two(void) { long x = 2; return x; }
int before(int *p) { return p[-1]; }
int after(int *p) { return p[1]; }
```

The IR for `minus_one` was `%t1 = sub i32 0, 1` and then `%t2 =
sext i32 %t1 to i64`: the parser, before it folded constant
expressions, built a unary node for `-`, the emitter wrote unary
minus as a subtraction from zero (as the cost model's row for `p -
n` said), and the `int` result was then widened to `long` by the
implicit conversion. `two` wrote the constant `2` as an `i64`
operand and nothing else. The listings, of that parser's output:

```
<minus_one>:
	xor    %eax,%eax
	sub    $0x1,%eax
	cltq
	mov    %rax,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	ret

<two>:
	movq   $0x2,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	ret

<before>:
	mov    %rdi,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	xor    %ecx,%ecx
	sub    $0x1,%ecx
	movslq %ecx,%rcx
	mov    (%rax,%rcx,4),%eax
	ret

<after>:
	mov    %rdi,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	mov    0x4(%rax),%eax
	ret
```

`long x = -1` was three instructions, `xor`, `sub`, `cltq`, where
`long x = 2` was a single store of an immediate. `p[-1]` cleared a
register, subtracted 1, sign-extended it to 64 bits and used it as a
scaled index, four instructions, where `p[1]` was one load with the
displacement `0x4` folded into the address. In a loop that walked
backwards with `p[-1]` the negation was inside the loop.

### 4.3 What it means

The cost model's constant row said "a constant under an implicit
conversion: the constant in the target type, no instruction". That
was true of `2` and false of `-1` until the parser folded constant
expressions; since then it is true of both. Nothing in the table
warned of it, since it had no row for unary minus or complement at
all. The measurement's tuned programs avoided negative constants
without saying so.

The question it asked of the project was where the literal rule
stops. Three answers were possible. The rule covers every operator in
the source, so `-1` subtracts: then the cost model needs a row saying
so, and a programmer who wants `-1` for free cannot have it in C's
spelling. The rule covers operators on data, and a constant expression
is folded in the parser before any IR exists: then `-1` is the immediate
`-1`, `96 * 96` is 9216, and the emitter never sees an operator with two
constant operands. Or the rule is left as it is and the gap is recorded.
Ritchie's compiler took the second answer, and so does the ISO text in
spirit: a constant expression "can be evaluated during translation".

### 4.4 The choice

Fold integer constant expressions in the parser, and only those:
the operators 6.6p3 does not exclude, applied to the operands 6.6p8
allows, which are integer constants, character constants, `sizeof`
and casts to integer types. A variable never folds, so `x * 1` still
multiplies and `x - x` still subtracts; the rule on data is
untouched. A signed result the type cannot hold wraps, as the
instruction would (G1). 6.6p4 makes an out-of-range constant
expression a constraint violation ("shall evaluate to a constant
that is in the range of representable values for its type"), and it
governs where the grammar requires a constant expression, the size
of an array; there the evaluator refuses the wrapped value. The
result is typed as the expression would be (`-1` is an `int`, `-1L`
a `long`) and then goes through the same implicit conversion as any
constant.

The same evaluator settles the next finding.

## 5. An array size must be one integer literal

### 5.1 The terms

An array declarator `D[e]` with `e` an integer constant expression
declares an array of fixed size; with any other `e` it declares a
*variable length array* (6.7.7.3p4), a conditional feature rcc does
not have. The standard's test is the expression's form, not the
token count: `int a[96 * 96]` is a fixed-size array of 9216
elements, as is `int a[sizeof (long) * 8]` or `int a[N]` with `N` a
macro expanding to `(1 << 10)`.

### 5.2 The observation

```c
int f(void) { int a[96 * 96]; return a[0]; }
```

```
arr.c:1:21: error: not yet: array size other than an integer constant
```

Before the evaluator, the parser accepted a lone integer token as
the size and refused anything else with that `not yet`, which it no
longer writes. `tests/perf/matmul.c` wanted `N * N` and wrote
`9216`. A macro expanding to a literal worked, since the preprocessor
had substituted it before the parser ran; a macro expanding to an
expression did not.

### 5.3 The fix

The evaluator of finding 4, applied at the size. A size that
evaluates is a fixed array; a size that does not evaluate, because
it names a variable, stays refused, since the variable length array
is a feature the project has not taken on. The check: `int a[96 *
96]` has 9216 elements, and `int a[n]` with `n` a variable is still
refused.

## 6. A register pointer to a local array is a `lea` at each use

### 6.1 The terms

A local array is an `alloca` in the IR, and after selection a *frame
index*, a symbolic "slot number k of this function's frame" that a
later pass (frame index elimination) turns into a displacement off
the stack pointer. Its address is not a value that exists anywhere;
it is computed by `lea -0x24(%rsp),%reg`, *load effective address*,
which adds the displacement to `rsp` and stores the sum in a register
without touching memory.

A `register` variable in rcc was then an SSA value (1.4): no slot,
no load, no store, kept in a register by the allocator. When its
initializer is a local array's address, the SSA value *is* the frame
address, and the allocator decides how to make it available at each
use. A `lea` from `rsp` is the cheapest possible rematerialization,
one instruction, no memory, no dependency on anything the loop
writes, so the allocator rematerializes rather than reserving a
register for it across the loop.

### 6.2 The observation

```c
int sum(void)
{
    int a[8];
    register int *p = a;
    register int *first = a;
    register int s = 0;
    for (p = a + 8; p != first; )
        s = s + *--p;
    return s;
}
```

```
<sum>:
	lea    -0x24(%rsp),%rcx
	add    $0x20,%rcx
	xor    %eax,%eax
	lea    -0x24(%rsp),%rdx
	cmp    %rdx,%rcx
	setne  %dl
	and    $0x1,%dl
	movzbl %dl,%edx
	cmp    $0x0,%edx
	je     <sum+0x2c>
	mov    -0x4(%rcx),%edx
	add    $0xfffffffffffffffc,%rcx
	add    %edx,%eax
	jmp    <sum+0xb>
	ret
```

The loop header is the second `lea`: `first` is recomputed into
`rdx` on every iteration, then compared with `p` in `rcx`. `p` and
`s` are in registers across the loop, as the row promises; `first`
is not, though it was declared `register` just the same. Writing `p
!= a` instead of `p != first` gives the same `lea` and `cmp`; the
`register` declaration bought nothing here.

### 6.3 What it means

The cost model's row for a `register` local said "a register, no
`mov` to or from the stack; across a loop, the same register". For a
variable that held a local's address, it was a `lea` from the frame
at each use, which is no `mov` to or from the stack but is not "the
same register" either. It is the allocator's choice, and squarely
inside "which register holds a value", the first of the three things
the thesis concedes. It costs one instruction per use, the same one
the non-`register` spelling costs, so the keyword is neither a gain
nor a loss on such a variable. The row should have said so, with a
cost program pinning the `lea` in the loop. No decision was open.

## 7. The findings together

The five sort into two kinds.

**Rules of the machine** (2, 3, 6). The selector's strength
reduction, the late cleanup's dropped `mov` and the allocator's
`lea` are things LLVM's code generator does with no option to stop
it, below the level the IR describes. The thesis draws a line
through them. The `lea` and the dropped constant load are the
allocator's business, "which register holds a value", and the
record needs a row and a bullet, nothing more. The strength
reduction is not: it changes the operation performed on the
program's data, and that is exactly the thing the thesis says the
compiler never does. The cost model treated one such rule, the
unused result, as a recorded departure, and another, the compare
fold, as defeated by the emitter (`llvm.md` 5.1 and 5.3). The
power-of-two rewrite had to be placed in one of those two columns,
and the pattern of the compare fold, a `freeze` on the constant
operand, was available at the cost of one `mov`.

**Gaps in the front end** (4, 5). The parser did no constant
evaluation, so a negative constant was a run-time operation and an
array size had to be one token. Both were closed by one evaluator of
integer constant expressions, in the parser, applied before any IR
was written. That evaluator is not an optimization of the program's
operations; it is the translation of a spelling into the number it
spells, which the standard places at translation time and which the
compiler this one descends from did from its first version.

## 8. What became of the five

The retirement of the LLVM pipeline settled the three findings that
were the machine's, and the evaluator had already closed the two
that were the front end's. The measurement of `cost.md` chapter 4
is redone under c1, with `collatz.c` signed and `matmul.c` writing
its size out as before; the run under the pipeline is its 4.1.

**Finding 2, the strength reduction**, has no place to happen. c1
writes the instruction of its table for the operator, whatever the
constant: `x * 4` is `imull $4, %eax, %eax`, and an unsigned `x / 4`
is `movl $4, %ecx; xorl %edx, %edx; divl %ecx`, the remainder of
`x % 4` then moved from `edx` (`backend.md` 6.4). No code in the
table looks at a constant's value. No cost row pins the three yet;
the queue holds the entry.

**Finding 3, the switchless late pass**, has no counterpart: no pass
runs after c1, and the assembler encodes what c1 wrote. A constant
used in two blocks is loaded in each, so the Collatz step's second
`idivl %ecx` has its own `movl $2, %ecx`.

**Findings 4 and 5, the unfolded constant and the one-token array
size**, were closed by the evaluator of `backend.md` 6.4, in the
parser, before c1 replaced the pipeline. `long x = -1` is `movq $-1,
%rax` and the store, `p[-1]` is the displacement of one load, `movl
-4(%rax), %eax`, and `int a[96 * 96]` has 9216 elements. 6.6p3
restricts the operators the evaluator folds and 6.6p8 the operands;
it wraps a signed result the type cannot hold, and refuses the
wrapped value only where 6.6p4 governs, the size of an array.

**Finding 6, the rematerialized frame address**, has no place to
happen either, since a `register` variable is its register and no
allocator chooses. `register int *first = a;` is one `leaq
-56(%rbp), %rax` into an expression register at the initialization,
moved into its register, `movq %rax, %r12`; after it the variable is
the register, so the loop's `p != first` is `cmpq %r12, %rbx`.
