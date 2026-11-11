# rcc: the cost model

What each construct compiles to. `guarantees.md` says what a program
means where ISO C says nothing; this page says what it costs, on
every construct. The two together are the contract: the compiler
does what the source says, and only that.

## 0. tl;dr

- The instruction written is the instruction executed. `x / 4`
  divides; `x >>= 2` shifts; `a + 0` adds. Choosing between them is
  the programmer's job, and the compiler never second-guesses it.
- The compiler decides three things of its own: the register that
  holds a value, by the discipline of `backend.md` chapter 4; the
  machine form of each operator, by the table of chapter 2; and the
  calling convention, System V AMD64. Nothing else, but a convention
  of the target like the stack's 16 bytes at a call: every loop head
  is aligned to 32 bytes (4.2).
- Every statement is written, its value used or not. `a + 0;` adds
  and `a / b;` divides, and the division can fault.
- Locals live in memory, one stack slot each, for the whole call. A
  local or parameter declared `register` lives in one of five
  callee-saved registers, `rbx` and `r12` to `r15`, and is never
  stored. A sixth live at once takes a slot, with a warning. The
  keyword is the allocation, as it was on the PDP-11.
- Chapter 4 holds the measurement under c1: the plain source runs
  2.33 to 8.29 times as long under rcc as under `clang -O2`, and the
  source tuned by hand for rcc 1.59 to 2.37 times. As first written,
  matmul's tuned `main` held ten `register` variables live, put five
  in slots and ran 7.54 times as long. Under the LLVM pipeline the
  two ranges were 1.49 to 8.43 and 1.33 to 4.40.

## 1. The rule

c1 writes the instruction for each operator of the tree, in the
written order: one machine instruction, or the machine's fixed
sequence, per operator (`backend.md` 6.1). Between the parser and the
assembler, nothing reasons about the program. The parser folds an
integer constant expression to the number it spells, since it is a
spelling and not an operation on data (`backend.md` 6.4); a variable
never folds.

What the compiler may do:

- Allocate registers, by the discipline, not by an allocator. A
  `register` variable takes the next free one of the five at its
  declaration and keeps it to the end of its block. An expression
  value takes the lowest free one of the nine caller-saved registers,
  and when none is free the oldest held value is spilled
  (`backend.md` chapter 4 and 6.2). A local declared in the source
  lives in its own stack slot regardless, unless it is declared
  `register`. The extent to which `register` is effective, which
  6.7.2p12 leaves implementation-defined and 4p10 asks to be
  documented, is five variables live at once.
- Select the machine form, by the table. A `+` is an `add`; a 32-bit
  signed division is `cltd; idivl`; a comparison is `cmp` and a
  `setcc` for a value, `cmp` and a conditional jump for a condition;
  a subscript is the access's scaled-index address, `(%rax,%rcx,4)`.
  The form is fixed per operator and target, and listed in chapter 2.
- Follow the calling convention: argument registers, the stack
  frame, callee-saved registers.
- Align every loop head to 32 bytes, `.p2align 5`, a convention of
  the target like the stack's 16 bytes at a call (4.2).

What the compiler never does:

- Fold. `a + 0` is an add, `x * 3` is a multiply, `x / 4` is a
  division, `a == a` is a compare.
- Move. A loop-invariant expression is computed every iteration. A
  constant used in a loop is materialized in the loop.
- Merge. The same expression written twice is computed twice.
- Reorder. Instructions come out in the written order; blocks come
  out in the written order.
- Delete. Nothing: a statement whose value is unused is computed,
  and every `return` jumps to the epilogue, the last one included.
- Convert a branch to a conditional move or the reverse.

## 2. What each construct compiles to

x86-64; aarch64 rows to follow when the target is wired. Each row is
the form c1 writes, in the text of the pin of the cost program named
in its last column, whose opening comment says what its listing must
show. A row with no program is stated from `backend.md` 6.3; no pin
shows it yet.

| Source | Machine | Program |
|---|---|---|
| a local | a slot of its own off `rbp` for the whole call; a load at each read, `movl -8(%rbp), %eax`, and a store at each write, `movl %eax, -8(%rbp)`; a constant stored by a statement or an initializer is the one store of the immediate, `movl $0, -8(%rbp)` | `local`, `while` |
| a parameter | its argument register stored to its slot in the prologue, `movl %edi, -4(%rbp)`; then the local's row | `parameter` |
| a `register` local | the next of `rbx`, `r12` to `r15`, by declaration order, pushed after `rbp` in the prologue and popped before `leave`; no slot, no load, no store; a read is the register as an operand, `cmpl %ebx, %r13d`; an instruction that writes its destination works on a copy, `movl %r12d, %eax; addl %r13d, %eax`, and the assignment moves the result back, `movl %eax, %r12d`; a constant assigned is the immediate, `movl $0, %r12d`; across a loop, the same register | `register-loop` |
| a `register` local without an initializer | the register as the prologue left it; nothing writes it until the program does, so every read sees one value (G5) | `register-undef` |
| a `register` parameter | moved from its argument register to its register in the prologue, `movl %edi, %ebx`; then the register | `register-parameter` |
| the frame | `pushq %rbp; movq %rsp, %rbp` in every function and `leave` in the one epilogue; a function that calls nothing keeps its slots in the red zone below `rsp` and writes no `sub`; one that calls subtracts its frame, rounded so that `rsp` is a multiple of 16 at the `call`, `subq $16, %rsp` | `local`, `call` |
| `a + b`, `a - b` | `addl %ecx, %eax`, `subl %ecx, %eax`; the left operand's register is the result's | `add-sub`, `parameter` |
| `a + 0` | `addl $0, %eax`: a constant operand is the immediate, whatever its value | `add-zero` |
| `a * b` | `imull %ecx, %eax`; by a constant, `imull $3, %eax, %eax` | `mul`, `mul-three` |
| `a / b`, `a % b` | `cltd; idivl %ecx` signed, `xorl %edx, %edx; divl %ecx` unsigned; the quotient in `eax`, the remainder moved from `edx`, `movl %edx, %eax` | `div-rem`, `udiv-urem` |
| `x / 4`, `x / 7` | the divisor held in a register, `movl $4, %ecx`, since `idiv` takes no immediate, then `cltd; idivl %ecx`; no shift, no multiply by a magic constant | `div-four`, `div-seven` |
| `a + b / c`, a division with `rax` and `rdx` busy | the values held there moved out to free registers, `movq %rdx, %rsi; movq %rax, %rdi`, the dividend moved into `eax`, then `cltd; idivl %esi` (`backend.md` 6.5) | `div-busy` |
| `a << n`, `a >> n` | the count in `ecx`, `shll %cl, %eax`, `sarl %cl, %eax`, `shrl %cl, %eax`; the instruction masks the count | `shift` |
| `a < b` and the rest, as a value | `cmpl %ecx, %eax; setl %al; movzbl %al, %eax`, `sete` for `==`; no jump | `compare` |
| `x == x` on a `register` variable | `cmpl %ebx, %ebx`: the compare is written | `register-compare` |
| `a < b` as a condition | `cmpl %ecx, %eax` and a jump on the inverse to the else label, `jge .Lchoose.2`; no value | `if` |
| `a && b`, `a \|\| b` | jump code: each operand compared with 0, `cmpl $0, %eax`, and a jump, `je .Lboth.2` for `&&`, `jne` past the right operand for `\|\|`; as a value, `movl $1, %eax` and `movl $0, %eax` on the two paths | `logical` |
| `a ? b : c` | the condition by jump code, `cmpl $0, %eax; je .Lconditional.2`, each arm into `eax`, a `jmp` past the second arm; never `cmov` | `conditional`, `select` |
| `if`, `while`, `for` | the condition by jump code at the top, where it is written, blocks in the written order; a loop's head is aligned to 32 bytes, `.p2align 5` before `.Lcount.2` in `while` and `.Lsum.2` in `for`, the gap filled by the assembler with multi-byte `nop`s; a loop jumps back from its bottom, `jmp .Lcount.2` in `while`, and `for`'s step has its own label before the jump; an empty arm is still tested, `je .Lempty_if.2` to the label that follows it in `cgp`; a join that holds only the jump back stays one block | `if`, `while`, `for`, `block-order`, `cgp`, `early-taildup` |
| `if (0)`, `!0` | `if (0)` is a test, `movl $0, %eax; cmpl $0, %eax; je .Lnever.2`, and the body is kept; `!0` is the constant 1, folded in the parser | `zero-test` |
| `return e` | `e` into `eax`, then `jmp .L<function>.1` to the one epilogue, `jmp .Lsum.1` in `for`, the last statement included | every program |
| a statement whose value is unused | written: `a + 0;` is `addl $0, %eax`, `a / b;` is `cltd; idivl %ecx`, and the division can fault | `unused` |
| `f(a, b)` | each argument evaluated into a register and stored to a slot of its own, `movl %eax, -12(%rbp)`, a constant argument in one store of the immediate, `movl $1, -4(%rbp)` in `stack-args`; the slots loaded into `edi`, `esi` and on; `call f@PLT`; the result in `eax` | `call` |
| a seventh and an eighth argument | stored to their slots as the first six are, a constant in one instruction, `movl $7, -28(%rbp)`, then pushed from their slots right to left, a narrow one loaded into `eax` and pushed from `rax`, `movl -28(%rbp), %eax; pushq %rax`; `subq $8, %rsp` first for an odd count; the bytes added back after the call, `addq $16, %rsp` | `stack-args` |
| `(T) a` | `int` to `long`, `movslq %eax, %rax`; `char` to `int` after the extending load, nothing; `int` to `unsigned`, nothing; to `char`, the narrow store `movb %al, -5(%rbp)`, and the next read is the extending load, `movsbl -5(%rbp), %eax` or `movzbl` | `cast` |
| a `char` or `short` parameter or result | stored at its width in the prologue, `movb %dil, -1(%rbp)`, `movw %si, -4(%rbp)`, and loaded with its extension, `movsbl`, `movswl`; a result not already valid to 32 bits extended before the jump to the epilogue, `movsbl %al, %eax`, `movzbl %al, %eax`; an argument stored at its width and loaded extended into its register, `movsbl -5(%rbp), %edi` | `narrow` |
| `a += b` | as a statement, the op on the object at its width, `addl %eax, -12(%rbp)`, on a `char` `addb %al, -9(%rbp)`; for its value, and for `*=`, `/=` and `%=` always, `b` first, then the load, the op, the store; on a `char`, `b` into `ecx`, the extending load `movsbl -9(%rbp), %eax`, the op in `int`, the narrow store `movb %al, -9(%rbp)` | `compound` |
| a constant | the immediate, `addl $408, %eax`, a constant expression folded first, `12 * 34` to 408; a 64-bit constant no 32-bit immediate holds is `movabsq $81985529216486895, %rcx` where it is written, in a loop at every iteration | `constant-hoisting`, `constant-in-loop` |
| a constant under an implicit conversion | the constant in the target type, an immediate of its width, stored: `long x = 2` is `movq $2, -8(%rbp)`, `long x = -1` is `movq $-1, -8(%rbp)`; `(long) 2` is the same `long` constant | |
| a loop-invariant expression | computed every iteration: `a / b` in the body is its two loads and its `idivl`, and `12 * 34` its `addl $408` | `invariant` |
| an expression written twice | computed twice: `a * b + a * b` is two `imull`, and `12 * 34 + 12 * 34` two `addl $408` | `twice` |
| more held values than nine registers | the oldest held value spilled, `movq %rax, -32(%rbp)`, and reloaded when it is consumed, `movq -32(%rbp), %rax` | `spill` |
| `p + n`, `m[i]` as a pointer | the index widened to 64 bits, `movslq %ecx, %rcx`, then `leaq (%rax,%rcx,4), %rax` for a size of 2, 4 or 8, and `imulq $12, %rcx, %rcx; addq %rcx, %rax` for a size that is not a scale | `gep`, `gep-stride` |
| `p - n` | the index widened, `imulq $4, %rcx, %rcx; subq %rcx, %rax`; for a size of 1, the `sub` alone | |
| `++p`, `--p`, `p += 2`, `p -= 2` | the size times the constant, an immediate: as a statement, on the pointer's slot, `addq $4, -8(%rbp)`, `subq $8, -8(%rbp)`; for its value, between the load and the store, `subq $8, %rax` | `ptr-step` |
| `a[i]`, `p[i]` | the access's own address: the array's `leaq -24(%rbp), %rax`, the index widened, `movslq %ecx, %rcx`, then `movl (%rax,%rcx,4), %eax` or `movl %edx, (%rax,%rcx,4)`; a constant index is the displacement, `movsbl 1(%rax), %eax` | `subscript`, `string` |
| `p - q` | `subq %rcx, %rax`; for a size other than 1, `movq $4, %rcx; cqto; idivq %rcx`, a division even for a size of 4, since G6 promises any two pointers and a shift would floor where the division truncates; for a size of 1 the `sub` alone, so `(char *) p - (char *) q` is the cheap spelling | `ptr-diff`, `ptr-diff-char` |
| `p < q` and the rest | `cmpq %rcx, %rax`, the unsigned `setb %al`, `movzbl %al, %eax` | `ptr-compare` |
| `if (p)`, `p == 0` | `cmpq $0, %rax`, then `je` as a condition, or `sete %al; movzbl %al, %eax` as a value | `ptr-null` |
| `*p` | `movl (%rax), %eax`, `movl %ecx, (%rax)`; x86 has one `mov` for every alignment | `deref` |
| `&x` | `leaq -8(%rbp), %rax` | `address-of` |
| `(T *) p`, `(long) p`, `(T *) l` | nothing; `movslq %eax, %rax` for an `int` | `ptr-int`, `void-pointer` |
| `"abc"` | `leaq .Ls0(%rip), %rax`; the bytes a `.string` under a label of their own in `.data`, writable, after the last function | `string` |

## 3. Verification

Two scripts, both run by `make test`. `tests/oracle` runs first: it
builds each divergence program of `tests/corpus/` under `clang -O2`
and under `./rcc`, compares the exit statuses with
`tests/corpus/expected`, and diffs the assembly `./rcc -S` writes
against the pin, `tests/corpus/<name>.s`. `tests/costs` runs after
it and pins this page.

Each program under `tests/cost/` is one row of chapter 2 or one rule
of chapter 1, and its opening comment says what its listing must
show. The pin beside it, `<name>.s`, is the text `./rcc -S` writes,
whole. The script assembles each listing with `clang -c`, to prove
that it assembles. A row passes when the fresh listing equals the
pin; the diff of a failed row is in `build/cost/log`. A new or
changed listing is read by hand against its row of chapter 2 before
`tests/costs --pin` adopts it. A change to `c1.cc` that moves a cost
thus shows as a red row, and the row names the construct whose cost
moved.

## 4. The measurement

The measurement says how far a literal machine lands from the
optimizer, and how much of that gap the programmer closes by hand.
Each program in `tests/perf/` comes as two sources: `<name>.c`, the
plain program, and `<name>-tuned.c`, the same program written for a
literal compiler. Each is built four times:

- `clang -O2 <name>.c`, the plain source under the optimizer: the
  baseline, ratio 1.00.
- `rcc <name>.c`, the plain source under rcc: what literalness costs
  an unmodified program.
- `rcc <name>-tuned.c`, the tuned source under rcc: the thesis
  number.
- `clang -O2 <name>-tuned.c`, the tuned source under the optimizer:
  whether hand tuning hurts it.

The tuned source makes the changes this page asks of the programmer:
`register` on the hot variables, pointer walks instead of subscripts,
`!=` loop ends, invariants hoisted, and a shift or a mask where the
programmer knows the divisor is a power of two and the value is not
negative. The two sources of a program return the same exit status.
Each program fills its local data from a linear congruential
generator written in the program, runs its work in a repeat loop, and
folds its result to a byte as the exit status, so nothing is dead and
nothing can be computed in closed form; the listing of each `clang
-O2` build has its loops.

The metric is user CPU time, the least of five runs, in milliseconds
and as the ratio to the baseline. `make bench` runs `tests/bench`,
which builds the four binaries, refuses a row whose four builds
disagree on the exit status, and times each run under `timeout 120`.
The numbers are bound to the machine, so nothing is pinned. `make
bench` was run three times, and the table is the run whose cells sum
least, on an x86-64 desktop, Zen 2, with clang 23.1.4, on 2026-11-09.
c1 aligns every loop head to 32 bytes (4.2), so the table is measured
at that layout: a hot loop's time still moves with the code before it,
but only where no loop head intervenes.

| program | `clang -O2 <name>.c` | `rcc <name>.c` | `rcc <name>-tuned.c` | `clang -O2 <name>-tuned.c` |
|---|---|---|---|---|
| collatz | 298 (1.00) | 2313 (7.76) | 473 (1.59) | 300 (1.01) |
| fib | 260 (1.00) | 607 (2.33) | 616 (2.37) | 274 (1.05) |
| matmul | 298 (1.00) | 2343 (7.86) | 605 (2.03) | 286 (0.96) |
| sieve | 300 (1.00) | 2048 (6.83) | 480 (1.60) | 307 (1.02) |
| sort | 279 (1.00) | 2313 (8.29) | 521 (1.87) | 283 (1.01) |
| strings | 287 (1.00) | 2201 (7.67) | 570 (1.99) | 286 (1.00) |

The plain source under rcc runs 2.33 to 8.29 times as long as under
`clang -O2`. The tuned source under rcc runs 1.59 to 2.37 times as
long. The tuned source under `clang -O2` runs 0.96 to 1.05 times as
long as the plain one.

**collatz.** The tuned source tests the parity with `& 1` and halves
with `>> 1`, where the plain one writes `% 2` and `/ 2`. The value is
a signed `long`, so only the programmer knows it is never negative.
The step loop of `rcc collatz.c` loads x from its slot three times an
iteration, for the loop test, the parity test and the step, stores it
back, and adds 1 to the count in its slot, `addl $1, -52(%rbp)`. The
parity test is `movq $2, %rcx; cqto; idivq %rcx` and a compare of the
remainder with 0, `cmpq $0, %rax; jne .Lmain.10`, and the even path
divides again. The tuned `main` holds five `register` variables live
in the step loop: total, end, v, x and steps take `rbx`, `r12`,
`r13`, `r14` and `r15d`, and rep and start, declared without the
keyword, take slots. Nothing warns. The step loop of `rcc
collatz-tuned.c` has no stack operand: it compares x with 1, `cmpq
$1, %r14`, tests the parity on a copy, `andq $1, %rax`, computes the
odd step with `imulq %r14, %rax` and `addq $1, %rax` and the even one
with `sarq $1, %rax`, moves the result back, `movq %rax, %r14`, and
counts in its register, `addl $1, %r15d`. The ratio falls from 7.76 to
1.59. The head of the step loop opens a 32-byte window, at 0x11e0. As
first written, with seven `register` variables live and x and steps in
slots, the tuned ratio was 1.91, measured before c1 aligned its loop
heads; the re-tuning then measured slower, and 4.2 tells why.

**fib.** The tuned source declares the parameter and the two partial
sums `register`. `rcc fib.c` subtracts its frame, `subq $32, %rsp`,
on every call, the base case included, stores the parameter to its
slot, `movl %edi, -4(%rbp)`, and loads it back for the test, the
return and each argument. Each argument goes through a slot of its
own, `movl %eax, -8(%rbp); movl -8(%rbp), %edi`, and the first result
is stored across the second call, `movq %rax, -16(%rbp)`, and loaded
back. The tuned `fib` holds its three live, n in `ebx`, a in `r12d`
and b in `r13d`. `rcc fib-tuned.c` moves the parameter in once,
`movl %edi, %ebx`, tests and returns it there, and keeps the first
result in `r12d` across the second call; but every call, the base
case included, pushes the three registers and pops them in the
epilogue, and each argument still goes through its slot, `movl %eax,
-28(%rbp); movl -28(%rbp), %edi`. The ratio rises from 2.33 to 2.37,
the only rise in the table. The work is the calls, and `clang -O2
fib.c` keeps one of the two recursive calls and turns the other into
a loop.

**matmul.** The tuned source computes the row bases of a and c once
per row, walks one pointer along a's row and another down b's column,
and declares the sum `register`, stored to c once per element. The
inner loop of `rcc matmul.c` has four `imull`, two for `i * n` and
one for `k * n` in the three indices and one for the product, eleven
loads of i, j, k and n from their slots, the step `addl $1,
-110632(%rbp)` on k's slot, and the product added to c's element in
place, `addl %edx, (%rax,%rcx,4)`. The tuned `main` holds five
`register` variables live in the inner loop: n, aend, pa, pb and s
take `ebx`, `r12`, `r13`, `r14` and `r15d`, and bend, i, arow, pc and
bcol, declared without the keyword, take slots. Nothing warns. The
inner loop of `rcc matmul-tuned.c` has no stack operand and runs 11
instructions an iteration: the test `cmpq %r12, %r13` and its jump,
the two loads, `movl (%r13), %eax` and `movl (%r14), %ecx`, the
product, `imull %ecx, %eax`, its add to s, `addl %eax, %r15d`, the
step `addq $4, %r13`, the step `pb += n`, `movslq %ebx, %rax; leaq
(%r14,%rax,4), %rax; movq %rax, %r14`, and the `jmp` back. The `leaq`
scales n, so the product is the loop's one multiply, where the plain
one has four. The middle loop pays the slots: it loads bcol and bend
for its test, `movq -110688(%rbp), %rax; movq -110656(%rbp), %rcx`,
copies arow and bcol from their slots into pa and pb, stores s
through pc loaded from its slot, and steps pc and bcol in theirs,
`addq $4, -110680(%rbp)` and `addq $4, -110688(%rbp)`. The ratio
falls from 7.86 to 2.03. The head of the inner loop opens a 32-byte
window, at 0x1380. As first written, with ten `register` variables
live and pc, bcol, pa, pb and s in slots, the tuned ratio was 7.54.

**sieve.** The tuned source walks a pointer in the marking loop and
in the strike loop, up to an end computed once per repetition, and
declares the walkers, the end, the limit, the outer counter and the
count `register`. The strike loop of `rcc sieve.c` loads j and n for
the test, j for the store and i for the step, and adds i to j's slot,
`addl %eax, -65568(%rbp)`: four loads and an add to memory for each
byte struck. The tuned `main` declares six `register` variables but
holds at most five live, since the marking walker's loop ends before
the outer counter's begins: n, count and end take `ebx`, `r12d` and
`r13`, the marking walker `r14`, and after it the outer counter i
takes `r14d` and the strike walker `r15`. Nothing warns. The strike
loop of `rcc sieve-tuned.c` compares the walker with the end, `cmpq
%r13, %r15; jae .Lmain.14`, stores the byte through it, `movb $0,
(%r15)`, and adds i, widened by `movslq %r14d, %rax`, with no stack
operand. The ratio falls from 6.83 to 1.60.

**sort.** The tuned source walks two pointers down for the shift,
steps with `--q`, loads each element it compares once, and keeps the
key, the index, the walkers and the element in registers. The shift
loop of `rcc sort.c` loads j four times and the key once from their
slots, steps j in its slot, `subl $1, -8220(%rbp)`, and loads the
element `a[j]` twice, for the test and for the move. The tuned
`main` holds its five live, i, key, p, q and v, in `ebx`, `r12d`,
`r13`, `r14` and `r15d`. The shift loop of `rcc sort-tuned.c` has no
stack operand and runs 11 instructions an iteration: `leaq
-8232(%rbp), %rax` for the array's address, the test `p != a`, `subq
$4, %r14` for `--q`, the one load of the element, `movl (%r14),
%eax`, its copy into v, the compare with the key, the store `movl
%r15d, (%r13)`, the move of `p = q` and the `jmp` back. The `&&` is
jump code: each test is a compare and a jump to the loop's exit,
`cmpq %rax, %r13; je .Lmain.12` and `cmpl %r12d, %eax; jle
.Lmain.12`, four of the 11, where the LLVM pipeline set and widened a
byte for each test and for the `&&` (`llvm.md` 5.5). The ratio falls
from 8.29 to 1.87; with collatz's, from 7.76 to 1.59, it is one of the
two largest falls in the table.

**strings.** The tuned source walks pointers in `length` and `copy`,
returns the length as the difference of two `char` pointers, tests
the byte `copy` has just stored, and halves the length with `>> 1`.
`length` in `rcc strings.c` loads the pointer and the index from
their slots at each test and steps the index in its slot, `addl $1,
-12(%rbp)`. `copy` in `rcc strings.c` loads s twice, d once and i
three times from their slots for each byte, steps i in its slot, and
loads the source byte twice. The tuned `length` and `copy` hold two
`register` variables each, s and p, and d and s, in `rbx` and `r12`;
`main` holds four, sum, p, end and n, in `ebx`, `r12`, `r13` and
`r14d`. In `rcc strings-tuned.c` neither `length` nor `copy` has a
stack operand: `length` steps `r12` and returns one `subq %rbx,
%rax`, and `copy` loads each byte once, `movsbl (%r12), %eax`, stores
it, `movb %al, (%rbx)`, and tests it. The `idivl` of `n / 2` in
`main` is a `sarl $1, %edx`. The ratio falls from 7.67 to 1.99.
`clang -O2` turns `length` into a call of `strlen` in both its
builds.

### 4.1 Under the LLVM pipeline

The same programs were measured through the pipeline `llvm.md`
records, on an x86-64 desktop, Zen 2, with LLVM 23.1.3, on
2026-11-07:

| program | `clang -O2 <name>.c` | `rcc <name>.c` | `rcc <name>-tuned.c` | `clang -O2 <name>-tuned.c` |
|---|---|---|---|---|
| collatz | 292 (1.00) | 2377 (8.14) | 494 (1.69) | 291 (1.00) |
| fib | 260 (1.00) | 388 (1.49) | 347 (1.33) | 266 (1.02) |
| matmul | 284 (1.00) | 2171 (7.64) | 940 (3.31) | 279 (0.98) |
| sieve | 288 (1.00) | 1843 (6.40) | 770 (2.67) | 299 (1.04) |
| sort | 275 (1.00) | 1634 (5.94) | 1209 (4.40) | 280 (1.02) |
| strings | 281 (1.00) | 2370 (8.43) | 760 (2.70) | 270 (0.96) |

The movement between the two tables is a result about the model.
In the table of chapter 4 the tuned sources of collatz and matmul
are the re-tuned ones, with five `register` variables live where
the sources the pipeline ran had seven and ten. The tuned source
under `clang -O2`, which c1 does not touch, stays within 0.96 to
1.05 in both; the two rcc columns move, from the pipeline's ratio to
c1's:

- collatz: plain 8.14 to 7.76, tuned 1.69 to 1.59.
- fib: plain 1.49 to 2.33, tuned 1.33 to 2.37.
- matmul: plain 7.64 to 7.86, tuned 3.31 to 2.03.
- sieve: plain 6.40 to 6.83, tuned 2.67 to 1.60.
- sort: plain 5.94 to 8.29, tuned 4.40 to 1.87.
- strings: plain 8.43 to 7.67, tuned 2.70 to 1.99.

The tuned ratio fell for sort, sieve, strings, matmul and collatz,
whose hot loops under c1 have no stack operand, sort's with jump code
where the pipeline set a byte for each test. It rose for fib, whose
every call under c1 pushes and pops three registers and passes its
argument through a slot.

Reading that pipeline's listings turned up five facts about it that
the cost model did not record, two of them against what its rule and
its flag table said. `findings.md` explains each from the ground up,
with the listings, the LLVM source and the choice it left open, and
closes with what became of each under c1.

### 4.2 The placement of a loop

The re-tuned collatz took both slots out of its step loop and ran
slower, 565 ms to 628, with no stack operand left to blame. What
changed beside the registers is where the loop lies. c1 then wrote no
alignment directive, so a loop's head fell wherever the instructions
before it ended; clang writes `.p2align 4, 0x90` before every loop
head. A first probe put the re-tuned step loop at the eight 4-byte
offsets of a 32-byte window, with `.p2align 5` and a run of `nop`
before `.Lmain.8`, built each with `clang`, and timed each binary five
times on the same machine, the least kept:

| offset of the head in its 32-byte window | 0 | 4 | 8 | 12 | 16 | 20 | 24 | 28 |
|---|---|---|---|---|---|---|---|---|
| `rcc collatz-tuned.c`, ms | 480 | 480 | 493 | 496 | 652 | 536 | 660 | 549 |

The loop is 54 bytes. From offset 0 to 12 it spans two windows and runs
in 480 to 496 ms, a ratio of 1.64 to 1.69 against the 293 ms `clang
-O2 collatz.c` ran in then; from offset 16 on it spans three and runs
in 536 to 660. The build `make bench` timed had the head at offset 16,
628 ms, and the source as first written, with x and steps in slots, had
it at offset 14 and ran in 565. The re-tuning's gain was real; the
placement took it back, and more.

The first probe also moved matmul's inner loop alone and ran it in 811
ms at offset 0, against 599 at the offset 22 the assembler gave it.
That table measured one layout of the rest of the code, not the offset:
in the full probe below, with every head aligned, the loop at offset 0
runs level with no alignment.

The full probe built each of the twelve sources of `tests/perf/` three
ways: as c1 wrote it, with `.p2align 4, 0x90` before every loop head,
and with `.p2align 5, 0x90`; thirty-six binaries. Fifteen rounds ran
them interleaved, every variant once a round, so that each saw the
same load on a machine in use. Every run was pinned to one core and
timed in user CPU time; the least of the fifteen is kept, the median
in parentheses. The tuned rows, in ms:

| program | none | `.p2align 4` | `.p2align 5` |
|---|---|---|---|
| collatz | 666 (675) | 675 (685) | 493 (500) |
| fib | 622 (631) | 623 (631) | 623 (632) |
| matmul | 614 (620) | 624 (630) | 613 (637) |
| sieve | 478 (488) | 623 (664) | 477 (528) |
| sort | 532 (534) | 759 (765) | 530 (538) |
| strings | 643 (652) | 579 (586) | 596 (602) |

The plain rows, whose hot loops keep their variables in slots, are a
wash: the three variants of each lie within 1% of each other, but
sieve's. There `.p2align 4` is 1.3% over no alignment, 2046 ms against
2020, and `.p2align 5` 2.5%, 2071. The median of the unaligned sieve's
fifteen runs lies 42 ms over its least, so the gap is at the edge of
the spread.

In the tuned rows `.p2align 4` wins strings by 10%, 579 ms against
643, and loses sieve by 30%, 623 against 478, and sort by 43%, 759
against 532; in both losses the head lies at offset 16 of its 32-byte
window. `.p2align 5` wins collatz by 26%, 493 against 666, and
strings by 7%, 596 against 643, and against no alignment loses no row
by more than 1 ms. By the medians it trails no alignment in sieve, 528
against 488, and in matmul, 637 against 620, the two rows where its
runs spread wider than the other variants', 51 and 24 ms from median
to least.

The full probe made two observations it did not prove. Under `.p2align
4`, sort's shift loop sits whole inside one 64-byte line and is the
slow one. Under `.p2align 4`, sieve's 17-byte strike loop is the one
variant that spans two 32-byte windows. Zen 2 fetches 32 bytes of code
a cycle, and its op cache and branch predictor keep their entries by
code address in 64-byte lines; which of these a given loop runs into is
not read from the numbers, and this page does not guess.

The decision: c1 writes `.p2align 5` before every loop head
(`backend.md` 7.2, and chapter 13, call 8). It is a convention of the
target, as the stack's 16 bytes at a call are: it says where the
machine fetches well and nothing about the program. It has two costs:

- Time. The padding lies on the path into the loop, so it is executed
  once per entry of the loop: at most 31 bytes of multi-byte `nop`s,
  since the directive names no fill byte. Before the step loop of
  `rcc collatz-tuned.c` the 18 bytes from 0x11ce are two
  instructions, `cs nopw 0x0(%rax,%rax,1)` and `nopl
  0x0(%rax,%rax,1)`. The probe measured the costlier padding:
  `.p2align 5, 0x90` assembled from text pads with one-byte `nop`s,
  one instruction per byte, where clang's own objects, emitted
  without the text step, get the long ones.
- Size. The code grows by the padding, up to 31 bytes a loop. The
  directive also raises the alignment of `.text` from 4 bytes to 32,
  so every function moves.

The numbers of this chapter are thus a fact about c1's layout, every
loop head at offset 0 of its 32-byte window, and no longer about where
the code before a loop happens to end. The heads of the five hot loops
of chapter 4 lie at 0x11e0 in collatz, 0x1380 in matmul, 0x1240 in
sieve, 0x1220 in sort and 0x1140 in strings' `length`. The code
before a head still decides which half of a 64-byte line the head
opens: the heads in collatz and sort open the second half, the other
three the first.
