# rcc: the back end's open calls

`backend.md` ends with six calls it leaves open, each with the
answer the design assumed. The sixth, the names, is settled: the
front end is c0, the generator is c1, `src/c1.cc` and `src/c1.hh`.
This page takes the other five one at a time, in the register of
`findings.md`: what the call turns on, the background a reader needs
to weigh it, the evidence from the compilers on this machine and
from the record of the LLVM sprint, what each answer costs, and
where the argument lands. Two of the five land where the design
assumed. Three move, two of them because the design's answer was
weighed on the wrong scale: it counted registers where it should
have counted what the generator has to know. The last chapter says
what changes in `backend.md`.

## 0. tl;dr

| Call | The design said | Where the argument lands |
|---|---|---|
| 1. `rbp` | the sixth register variable, no frame pointer | the frame pointer, in every function: a slot's offset is known at its declaration, the prologue is one shape, the debugger needs nothing from us; five register variables |
| 2. the sixth `register` variable | a slot, with a note under `-v` | a slot, with a warning by default: a lever that silently does nothing is not a lever |
| 3. folding | integer constant expressions, in the parser | the same, and the evaluator is required anyway for static initializers and `case` labels |
| 4. the red zone | leaf functions keep their locals in it | the same: it is the convention's gift to a leaf, and every pin shows it |
| 5. arguments | each pushed as generated, popped into its register | every argument a hidden local of the call, stored to its slot as evaluated; the call sequence pushes the stack arguments from their slots right to left and loads the first six; the design's pushes had the seventh argument above the eighth |

## 1. Background: where the calls live

All five are about the frame and the convention of `backend.md`
chapters 3 to 5 and 8. In brief: a function's frame is the stack
between the return address `call` pushed and the lowest slot the
function uses; the System V convention says which registers carry
arguments (`rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`), which a callee
must preserve (`rbx`, `rbp`, `r12` to `r15`), which it may destroy
(the other nine), that `rsp` is a multiple of 16 at every `call`, and
that 128 bytes below `rsp` are a function's to use without moving
`rsp`. rcc's register discipline gives five of the callee-saved six to
`register` variables, keeps `rbp` as the frame pointer, and gives
the caller-saved nine to expressions. The calls are the corners of
that picture.

## 2. Call 1: `rbp`, a frame pointer or a sixth register

### 2.1 The terms

A *frame pointer* is a register that holds a fixed address inside the
current frame for the whole call, so that locals are addressed off
it and the frame can be found from outside. On the PDP-11 it was
`r5`; on x86 it is `rbp`, and the classic prologue is `push %rbp;
mov %rsp,%rbp`. The push saves the caller's `rbp`, so the frame
pointers form a chain: each frame's saved `rbp` points at the
caller's frame, and a debugger or a profiler walks the chain to
print a backtrace with no other information.

The alternative is to address everything off `rsp`, which works when
the frame's size is fixed and known to the compiler, and to describe
the frame to the debugger in a table instead: the *call frame
information* (CFI) of DWARF, emitted by the assembler from `.cfi`
directives (`.cfi_startproc`, `.cfi_def_cfa_offset 16` after a push,
`.cfi_offset %rbx, -16`, `.cfi_endproc`) into the `.eh_frame`
section, which C++ exceptions also use to unwind. A debugger reads
the table and finds every frame without a chain. A sampling
profiler can too (`perf record --call-graph dwarf`), at the price of
copying stack to decode later, where the chain can be walked in the
kernel at sample time (`--call-graph fp`).

The frame pointer went out of fashion on x86-64 because the
architecture has only sixteen registers and `rsp`-relative addressing
costs nothing: gcc and clang both omit it when optimizing and keep it
at `-O0` for debuggability. The ABI itself calls it optional: `rbp` is
"callee-saved register; optionally used as frame pointer" in its
register table, and a footnote of section 3.2.2 says the conventional
use "may be avoided by using `%rsp` (the stack pointer) to index into
the stack frame". Some distributions have lately turned it back on by
default across their packages, for the profilers.

### 2.2 The evidence

The same function under four compilers:

```
clang -O0                       gcc -O2
<leaf>:                         <leaf>:
  push   %rbp                     lea    0x2(%rdi,%rdi,1),%eax
  mov    %rsp,%rbp                ret
  mov    %edi,-0x4(%rbp)
  mov    -0x4(%rbp),%eax        rcc, LLVM pipeline
  add    $0x1,%eax              <leaf>:
  mov    %eax,-0x8(%rbp)          mov    %edi,-0x8(%rsp)
  mov    -0x8(%rbp),%eax          mov    -0x8(%rsp),%eax
  shl    $1,%eax                  add    $0x1,%eax
  pop    %rbp                     mov    %eax,-0xc(%rsp)
  ret                             mov    -0xc(%rsp),%eax
                                  shl    $1,%eax
                                  ret
```

`clang -O0` keeps the chain and pays three instructions for it;
`gcc -O2` and rcc's LLVM pipeline address off `rsp`. (Both listings
also show `y * 2` as `shl $1`, finding 2 of `findings.md`, in two
compilers' `-O0`-class code.) Every pin in `tests/cost` is of the
`rsp` form; `gcc -O2 -fno-omit-frame-pointer` on a function with
calls shows the price in a non-leaf: `push %rbp; mov %rsp,%rbp` on
top of `push %rbx`, and `leave` before `ret`.

What a sixth register is worth to rcc is in the tuned programs of
the measurement as first written for the LLVM pipeline, counted per
function:

| program | function | `register` variables declared | most live at once |
|---|---|---|---|
| matmul | `main` | 10 | 10 |
| collatz | `main` | 7 | 7 |
| sieve | `main` | 6 | 5 |
| sort | `main` | 5 | 5 |
| strings | `main` | 4 | 4 |
| strings | `length`, `copy` | 2 each | 2 each |
| fib | `fib` | 3 | 3 |

c0 counts the live ones: a block's variables leave the count at its
end, so sieve's marking walker is gone before its outer counter and
strike walker are declared. By the live count two functions exceed
five, matmul's `main` and collatz's, and a sixth register would keep
one more variable out of a slot in each. In collatz it is x, the
step loop's value, while steps stays in its slot; in matmul it is
pc, which the inner loop does not touch, while bcol and the inner
loop's pa, pb and s stay in theirs. Sieve, sort, strings and fib fit
in five. That is what the sixth register is worth: one variable in
each of two programs of six, and it fits neither of them whole.
The two were since re-tuned to five, and `cost.md` chapter 4 holds
both numbers.

### 2.3 What it means

The question is not whose register `rbp` is. It is what the
generator has to know when it writes an instruction, and here the
frame pointer wins on three counts.

**A slot's offset is known at the declaration.** With a frame
pointer, the first local is `-8(%rbp)` the moment the generator
meets its declaration, the second `-12(%rbp)`, whatever the frame
turns out to hold below them: the spill area, the argument slots of
chapter 6, anything sized only after the body is generated. Off
`rsp` a slot is addressed from the bottom of the frame, so its
offset is the frame's total size minus its distance from the top,
and the total is not known until the body has been generated. The
generator would have to write the body into a buffer and patch the
offsets, or write symbolic displacements for the assembler to
resolve, and either way the listing is harder to read and the
generator carries a second pass. The design assumed `rsp` because
LLVM's pipeline did, and LLVM's pipeline has a frame-finalization
pass; c1 is one pass.

**The prologue is one shape.** `push %rbp; mov %rsp,%rbp; sub
$N,%rsp` opens every function and `leave; ret` closes it (`leave` is
`mov %rbp,%rsp; pop %rbp`). Inside the body `rsp` may then move,
pushes around a call, pushes of stack arguments, without disturbing
a single offset, since every slot is off `rbp`. The `.cfi` table is
three directives after the prologue and nothing more, however the
body moves `rsp`.

**The debugger and the profiler need nothing from us.** The chain
walks itself, in `gdb` and in `perf` at sample time, and a mistake
in a `.cfi` directive no longer hides a frame. The directives are
still written, so that the unwinder has them too, but nothing
depends on their being right on the first day.

Ritchie's `c1` kept `r5` for the first and second of these reasons:
its expression stack pushed and popped through a body and the frame
pointer made that harmless. The same reasons hold here in the same
form. The cost is one register variable, five instead of six, which
2.2 priced at one variable in each of collatz and matmul as first
written, and three instructions per function, the `push`, the `mov`
and the `leave`, two in a leaf that keeps its slots in the red zone
and needs no `sub`.

### 2.4 Where it lands

The frame pointer. `rbp` opens every function, leaf included; the
register variables are `rbx`, `r12`, `r13`, `r14`, `r15`, five, and
the sixth declared takes a slot, call 2. Every slot and every
argument slot is a negative displacement off `rbp`, known when it is
declared. `backend.md` chapters 4 and 5 change accordingly.

## 3. Call 2: the sixth `register` variable

### 3.1 The terms

6.7.2p12: "A declaration of an identifier for an object with
storage-class specifier `register` suggests that access to the
object be as fast as possible. The extent to which such suggestions
are effective is implementation-defined." Two things follow. The
compiler may put the sixth variable in a slot; Annex J.3 lists the
extent as the first item of implementation-defined behaviour, and
4p10 says an implementation "shall be accompanied by a document that
defines all implementation-defined ... characteristics", so the rule
must be written down, which the cost model does. And a program that
declares six is a conforming program, so the compiler may not
refuse it: whatever it says, it says as a warning, not an error.

Three answers are possible for what it says. Nothing, as every
modern compiler says about `register` (they ignore the keyword
entirely, and `clang -O0` puts a `register` variable in a slot like
any other). A note when asked, under a verbose flag. A warning by
default, naming the variable and the count.

### 3.2 The evidence

The table of 2.2: in the tuned programs, written by a programmer who
knew the LLVM pipeline would honour every `register`, two of six
`main` functions exceeded five live at once, until the two were
re-tuned to five. Under the discipline, matmul's `main` had five
variables in slots and collatz's two, while sieve's six are never
more than five live at once; `cost.md` chapter 4 reads the slots in
the listings and the timing. If the
compiler says nothing, the programmer learns it from the listing or
the timing, or not at all; a programmer who
wrote `register` on a loop's variable and got a slot has asked for a
thing and been refused in silence.

The PDP-11 compiler said nothing, and its programmers knew the
machine had three. That knowledge was tribal. It is written in the
cost model, `cost.md` chapter 1, but a cost model is read once and a
diagnostic is read at the moment it matters.

### 3.3 What it means

The thesis calls `register` the programmer's lever on the allocator.
A lever that moves and does nothing, silently, is not a lever; it is
a decoration, which is what the keyword became everywhere else. The
one argument for silence is noise: programs written for other
compilers that scatter `register` over everything. Those programs
are rare now, the warning fires only past the fifth variable in one
function, and a programmer who wants silence can remove the keyword
from the variable they care least about, which is the decision the
model asks of them anyway.

### 3.4 Where it lands

A slot, with a warning by default: `register variable 'k' in a stack
slot: the five registers are taken`. The design's answer, a note
under `-v`, moves. The warning is not an error, and a `-w`-style
switch silences it for the programs that want it silenced.

## 4. Call 3: folding integer constant expressions

### 4.1 The terms

`findings.md` chapter 4 sets out what a constant expression is and
why `-1` is an operator applied to a constant. This chapter adds the
fact that decides the call: ISO C requires an evaluator of integer
constant expressions in several places, and a compiler that has none
cannot compile them at all:

- The size of an array, 6.7.7.3p4: a fixed-size array is one whose
  size is an integer constant expression.
- The label of a `case`, 6.8.5.3p3: "shall be an integer constant
  expression".
- The value of an enumeration constant and the width of a bit-field,
  6.7.3.3 and 6.7.3.2.
- Every expression in the initializer of an object with static
  storage duration, 6.7.11p5: `int x = 3 * 4;` at file scope is
  twelve bytes of `.data` holding the value 12, and there is no
  instruction to compute it at, since no code runs before `main`.

rcc has none of these yet (`static` and file-scope variables are
"not yet"), and each will need the evaluator when it arrives. So the
evaluator exists in c0 regardless. The call is only whether it also
runs on a constant expression where a non-constant one would do: in
`long x = -1`, in `p[-1]`, in `x * (2 * 3)`.

### 4.2 The evidence

Under the LLVM pipeline, from `findings.md`:

```
long x = -1                     long x = 2
  xor    %eax,%eax                movq   $0x2,-0x10(%rsp)
  sub    $0x1,%eax
  cltq
  mov    %rax,-0x10(%rsp)
```

Three instructions against one, at every execution. Ritchie's `c0`
folded: when it built a node whose two operands were constants, it
built the constant instead. Every C compiler since has.

### 4.3 What it means

The literal rule is "the instruction written is the instruction
executed", and it is a rule about the program's data: `x * 4`
multiplies `x`, a value the compiler does not know. A constant
expression has no data in it; it is a spelling of a number, and ISO
C says so in 6.6p2, "can be evaluated during translation rather than
runtime". Folding it does not change which instructions operate on
data; it changes how a number is written in the listing, `$-1`
instead of `xor; sub; cltq`.

What is not folded matters as much. A node with one constant
operand stays: `x * 1` is `imul $1`, `x + 0` is `add $0`, `0 * x` is
a multiply, `x - x` is a subtract. The evaluator fires only when
every leaf under a node is a constant, and the result takes the type
C gives the expression, so `-1` is an `int` and `-1L` a `long`, and
then meets the implicit conversion like any constant. A value
outside the type's range is a constraint violation (6.6p4) and is
diagnosed, not wrapped.

### 4.4 Where it lands

Fold, as the design says: integer constant expressions wherever they
occur, constants only, in c0, with the one evaluator that the array
size, the `case` label and the static initializer require. The cost
model's constant row then reads as it should: a constant is an
immediate.

One sentence of 4.3 was revised when the evaluator landed: an
out-of-range result is not diagnosed but wraps, as G1 makes the
instruction wrap, and a division with no value stays unfolded for G4's
trap. The fold is the spelling of a number only while the number is
the one the instruction computes; `2147483647 + 1` in an ordinary
expression is the run-time overflow of 6.5p5, which G1 defines, and
not the constant expression of 6.6p4, which the grammar names only
where it requires one. There, in an array size, the wrapped value is
refused.

## 5. Call 4: the red zone

### 5.1 The terms

The System V AMD64 ABI, section 3.2.2, reserves the 128 bytes below
`rsp`: "The 128-byte area beyond the location pointed to by `%rsp`
is considered to be reserved and shall not be modified by signal or
interrupt handlers. Therefore, functions may use this area for
temporary data that is not needed across function calls. This area
is known as the red zone." A function
that calls nothing, a *leaf*, can keep its locals there and never
move `rsp`, saving the `sub` of the prologue and the `add` of the
epilogue. A function that calls something cannot, because `call`
pushes the return address into the zone and the callee's frame
overwrites it. Kernel code compiles with `-mno-red-zone`, since an
interrupt there arrives on the current stack.

### 5.2 The evidence

Every leaf under `clang -O0`, `gcc -O0` and rcc's LLVM pipeline uses
the zone: the `leaf` listings of 2.2 address `-0x4(%rbp)` or
`-0x8(%rsp)` with no `sub`. All 48 pins of `tests/cost` are of leaf
functions or of functions whose call shape is the row's subject, and
the leaf ones all show the zone.

### 5.3 What it means

Two prologue shapes exist whatever the call: a leaf whose slots
exceed 128 bytes must move `rsp` anyway, so "every function moves
`rsp`" buys one shape only for functions under 128 bytes, at two
instructions per call of every small leaf. The zone is the
convention's gift to exactly the functions a literal compiler's
programmer writes most, the small ones, and taking it keeps every
re-pinned leaf listing comparable to its LLVM-era pin, so that the
diff of the re-pinning is about the constructs and not about the
prologue.

### 5.4 Where it lands

Yes: a function that makes no call and whose slots and spill area fit
in 128 bytes keeps them in the red zone and never moves `rsp`. Every
other function subtracts its frame once in the prologue and adds it
once in the epilogue.

## 6. Call 5: how arguments reach their registers

### 6.1 The terms

Three facts collide at a call. G13 says the arguments are evaluated
left to right, each complete before the next begins. The convention
says the first six go in `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`, which
are also six of the nine expression registers, and that the
arguments beyond them "are pushed on the stack in reversed
(right-to-left) order" (section 3.2.3), so that at the `call` the
seventh is at `(%rsp)` and the eighth at `8(%rsp)`; and that "the
stack needs to be 16 (32 or 64) byte aligned immediately before the
call instruction is executed" (section 3.2.2). And any argument may
itself contain a call,
which destroys every caller-saved register, the six included.

So an argument's value, once computed, must be kept somewhere safe
until the `call`, through the evaluation of every argument after it;
and at the `call` the six values must be in the six registers,
whichever registers they are in now. Moving `n` values from `n`
registers to `n` other registers at once, where the sets overlap, is
the *parallel move* (or parallel copy) problem: the moves have to be
ordered so that no destination is overwritten before it is read, and
a cycle (`rdi` must go to `rsi` and `rsi` to `rdi`) needs an
exchange or a scratch register. x86 has `xchg`, and the problem is
classical and small for six.

### 6.2 The evidence

Three compilers on three calls:

```
                clang -O0               gcc -O0                 rcc, LLVM pipeline
nested:         mov -0x4(%rbp),%edi     mov %eax,%edi           mov 0x8(%rsp),%edi
g(x) + h(x)     call g                  call g                  call g
                mov %eax,-0x8(%rbp)     mov %eax,%ebx           mov %eax,%ebx
                mov -0x4(%rbp),%edi     mov -0x14(%rbp),%eax    mov 0x8(%rsp),%edi
                call h                  mov %eax,%edi           call h
                mov %eax,%ecx           call h                  add %ebx,%eax
                mov -0x8(%rbp),%eax     add %ebx,%eax
                add %ecx,%eax

args:           call g                  call g                  call g
g(g(x) + 1)     mov %eax,%edi           add $0x1,%eax           lea 0x1(%rax),%edi
                add $0x1,%edi           mov %eax,%edi           call g
                call g                  call g

many:           mov $0x5,%r9d           push $0x7               mov $0x5,%r9d
eight(x,1,..,7) movl $0x6,(%rsp)        push $0x6               push $0x7
                movl $0x7,0x8(%rsp)     mov $0x5,%r9d           push $0x6
                call eight              ...                     call eight
                                        call eight              add $0x10,%rsp
                                        add $0x10,%rsp
```

Three things to read off. In `nested`, the value of `g(x)` has to
survive the call of `h`: `clang -O0` stores it to a slot, gcc and
LLVM keep it in `rbx`, a callee-saved register, which in rcc's
discipline belongs to a register variable and is not available for
this. In `many`, the two stack arguments: clang reserves the space
in the prologue and stores each at its offset (`movl $0x6,(%rsp)`),
so `rsp` is constant through the body; gcc and LLVM push them, right
to left so the seventh lands lowest, and `add $0x10,%rsp` after the
call. In `args`, the nested call's result goes straight from `rax`
to `rdi` with its `add` on the way.

### 6.3 The bug in the design

`backend.md` chapter 8 says every argument is pushed as it is
generated and the first six popped into their registers in reverse,
and that "arguments past the sixth stay on the stack in the
right-to-left order the ABI wants, which the pushes in source order
have produced". That is wrong. Pushing `a7` then `a8` puts `a8` at
the lower address, and the ABI wants `a7` there. Left-to-right
evaluation and the stack layout the ABI wants are in conflict for
stack arguments, and the pushes cannot do both. gcc and LLVM
resolve it by evaluating stack arguments right to left, which G13
forbids rcc; clang's reserved area resolves it by storing each
argument to its own offset in whatever order it is computed.

### 6.4 The three shapes

**Push everything** (the design's). Each argument's value pushed
when computed, so it is safe from later calls; the six popped into
their registers at the end. Uniform and one instruction each way,
but wrong past six: the pushes put the eighth argument below the
seventh, and no order of pops puts it back without registers. The
seventh argument onward needs another mechanism, and then there are
two.

**Straight into the register when safe.** An argument that is a
variable or a constant, with no later argument containing a call, is
moved into its register directly; anything else takes a safe path.
Cheapest code on the common call, which is why every compiler's
`many` looks the same, but two paths through the generator and a
look-ahead over the later arguments to pick between them, and a
parallel move at the end for the general case.

**Every argument a hidden local of the call.** Each argument is
evaluated left to right into a register, exactly as the initializer
of `int t = a1;` would be, and stored to a slot of its own in the
frame, off `rbp`. When the last is stored, the call sequence reads
them back: the stack arguments pushed from their slots right to
left, `pushq -24(%rbp)`, one instruction each, after a `sub $8,%rsp`
when their count is odd so that the `call` lands on sixteen; the
first six loaded from their slots into `rdi` to `r9`; `call`; the
pushed bytes added back. Nothing is held in a register across the
evaluation of a later argument, so a call inside an argument
clobbers nothing that matters, and no parallel move is needed,
because every value comes from memory and the registers are free.
A value the enclosing expression holds, `x` in `x + f(a, b)`, is
saved to a spill slot around the call by the general rule of
chapter 6 of `backend.md`.

### 6.5 What it means

The third shape is Ritchie's model applied to arguments: a value the
programmer did not put in a register lives in memory, and the
listing shows it. Its cost is one store and one load per argument,
the same count as a push and a pop, and the frame pointer makes it
trivial: argument slots are locals the source did not name, with
offsets known when the call is met, and the pushes of the stack
arguments move `rsp` without disturbing them. The seventh-argument
bug of 6.3 is gone, since the stack arguments are pushed from their
slots in the order the ABI wants rather than the order the source
evaluates them. The parallel move of the design's alternative is
gone, since no value is in a register when the call sequence
begins.

What is paid: on `nested`, `g(x) + h(x)`, the argument `x` of each
call is loaded from its slot, stored to the argument slot, and
loaded again into `rdi`, where gcc and the LLVM pipeline load it
once. On `fib`, the call-heaviest program of the measurement, that
is two instructions per call. The cost model gets a row for a call
with register arguments, one with stack arguments and one with a
call inside an argument, and the measurement will say what the
shape costs. A direct path for an argument that is a variable or a
constant can be added as a measured step later, if the number says
it should; it changes the call sequence and nothing else.

### 6.6 Where it lands

Every argument a hidden local of the call. Chapter 8 of `backend.md`
is rewritten to say so, chapter 5 gains the argument slots, and the
call rows of the cost model follow the first pins.

## 7. What changes in `backend.md`

- Chapter 0 and chapter 4, the register discipline: five register
  variables, `rbx` and `r12` to `r15`; `rbp` the frame pointer; the
  sixth declared takes a slot with a warning.
- Chapter 5, the frame: the classic prologue and `leave`, every slot
  a displacement off `rbp` known at its declaration, argument slots
  for the calls the function makes, the red zone for a leaf's slots
  with no `sub`.
- Chapter 6.4, constants: unchanged, and a sentence that the
  evaluator is the one the array size, the `case` label and the
  static initializer require.
- Chapter 8, calls: rewritten to the shape of 6.4 and 6.5 here.
- Chapter 13: the six calls, each in one line, pointing here.
- The cost model, rewritten from the new pins: the
  implementation-defined extent of `register`, five live at once,
  recorded in `cost.md` chapter 1, and the warning in its chapter 0;
  the call rows of 6.5 in its chapter 2.
