# rcc: the cost model

What each construct compiles to. `guarantees.md` says what a program
means where ISO C says nothing; this page says what it costs, on
every construct. The two together are the contract: the compiler
does what the source says, and only that.

## 0. tl;dr

- The instruction written is the instruction executed. `x / 4`
  divides; `x >>= 2` shifts; `a + 0` adds. Choosing between them is
  the programmer's job, and the compiler never second-guesses it.
- The compiler decides three things of its own: which register
  holds a value, the one-to-one machine form of each IR instruction,
  and the calling convention. Nothing else.
- The back end is LLVM's `llc` with FastISel and every rewriting
  pass switched off. The flag table is in `main.cc`; chapter 2 lists
  it with what each flag holds back.
- A computation whose result is unused is not emitted. That is the
  selector's own rule and has no switch.
- Locals live in memory, one stack slot each, for the whole call. A
  local or parameter declared `register` is never stored: it is an
  SSA value, and the allocator keeps it in a register. The keyword is
  the programmer's lever on the allocator.
- Chapter 6 measures what the costs add up to: on six programs, the
  plain source runs 1.49 to 8.43 times as long under rcc as under
  `clang -O2`, and the source tuned by hand for rcc 1.33 to 4.40
  times.

## 1. The rule

The front end emits one IR instruction per operation in the source,
in the written order, and the back end selects one machine
instruction, or the machine's fixed sequence, per IR instruction.
Between the two, nothing reasons about the program.

What the compiler may do:

- Allocate registers. Which value lives where, and when a value is
  spilled, is the allocator's. A local declared in the source lives
  in its own stack slot regardless, unless it is declared `register`.
- Select the machine form. An IR `add` is an x86 `add`, or the
  three-operand `lea` when the result goes to a third register, the
  same addition in the encoding that spares a copy, not a fold; a
  32-bit signed division is `cltd; idivl`; a comparison is `cmp` and
  a `setcc` or a conditional jump. The form is fixed per instruction
  and target, and listed in chapter 3.
- Follow the calling convention: argument registers, the stack
  frame, callee-saved registers.

What the compiler never does:

- Fold. `a + 0` is an add, `x * 3` is a multiply, `x / 4` is a
  division, `a == a` is a compare.
- Move. A loop-invariant expression is computed every iteration. A
  constant used in a loop is materialized in the loop.
- Merge. The same expression written twice is computed twice.
- Reorder. Instructions come out in the written order; blocks come
  out in the written order.
- Delete. Except an instruction whose result nothing reads, which
  the selector drops; see chapter 4.
- Convert a branch to a conditional move or the reverse.

## 2. The pipeline

`llc`, LLVM 23.1.3, with the flags below. Most are hidden options,
not a stable interface; a new LLVM is checked against chapter 5
before the version in `main.cc` is bumped.

| Flag | What it holds back |
|---|---|
| `-O1` | the real register allocator; `-O0` allows only the fast one, which spills across every block. `llc`'s own default is `-O2`; the corpus tells the two apart on no row, and `-O1` is written so the level is stated, not inherited |
| `-fast-isel` | FastISel: the instruction written is the instruction selected; no folding of `a + 0`, `x * 3`, `x / 4` |
| `-fast-isel-abort=3` | never fall back to the folding selector; fail loudly instead |
| `-relocation-model=pic` | clang links position-independent executables |
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

Why `-O1` and not `-O0`: LLVM ties its register allocators to the
level. At `-O0` only the fast allocator is permitted, and it spills
every value to the stack at every block boundary, so a loop reloads
its counter and its parameters from memory each iteration. At `-O1`
the greedy allocator runs, and with FastISel forced and the passes
above held back, the selection is as literal as at `-O0` and the
registers are real. One pass has no switch, the machine combiner; on
the probes it did nothing to a literal chain of adds and multiplies.

## 3. What each construct compiles to

x86-64; aarch64 rows to follow when the target is wired.

| Source | IR | Machine |
|---|---|---|
| a local | `alloca`, a load at each read, a store at each write | a stack slot, `mov` in and out |
| a parameter | stored to its slot at entry, loaded at each read | the argument register, then the slot |
| a `register` local | the value itself, no `alloca`, no load, no store; a `phi` at a join or a loop header; without an initializer, one `alloca` never written and one `load` and `freeze` at the declaration (`register-undef`) | a register, no `mov` to or from the stack; across a loop, the same register; without an initializer, one `mov` from the frame at the declaration |
| a `register` parameter | the argument itself | the argument register, as it arrived |
| `a + b`, `a - b` | `add`, `sub` | `add`, `sub`, or `lea` for a three-operand add |
| `a * b` | `mul` | `imul` |
| `a / b`, `a % b` | `sdiv`, `srem`, `udiv`, `urem` | `cltd; idiv` or `xor %edx; div`; the quotient from `eax`, the remainder from `edx` |
| `a << n`, `a >> n` | `shl`, `ashr`, `lshr` | `shl`, `sar`, `shr` by `cl`; the count masked by the instruction |
| `a < b` and the rest | `icmp`, `zext`; two operands of one name, a `register` variable or a constant, the right one `freeze`d first (`register-compare`) | `cmp`, `setcc`, `and $1`, `movzbl`; used as a condition, the value is then compared with 0 again, since the tree holds an `int` and the emitter tests it as one; the `freeze` is nothing for a register, one `mov` for a constant |
| `a && b`, `a \|\| b` | branches, `phi` | `cmp $0`, conditional jumps; the short-circuit value set before the jump |
| `a ? b : c` | branches, `phi` | `cmp $0`, conditional jumps; never `cmov` |
| `if`, `while`, `for` | `icmp ne 0`, `br`; a condition of the constant 0, the 0 `freeze`d first (`zero-test`) | `cmp $0`, conditional and unconditional jumps, blocks in the written order; for the constant 0, a `cmp` of two registers each cleared by an `xor` |
| `f(a, b)` | `call` | the convention's argument moves, `call` |
| `(T) a` | `trunc`, `sext`, `zext`, or nothing | `movsbl`, `movzbl`, `cltq`, or nothing; a `char` load is `movzbl` from the slot and then the extension, two instructions |
| a `char` or `short` parameter or result | `i32` in the signature, `trunc` at entry, `sext`/`zext` before `ret` and before the call, `trunc` after it | a byte or word store at entry, `movsbl`/`movzbl` or `movswl`/`movzwl` before the call and before `ret` |
| `a += b` | load, convert, op, convert, store | the row of the op, between a load and a store |
| `p + n`, `p - n`, `a[i]` | `sext` of the index to 64 bits, `getelementptr` of the pointee type; for `p - n`, the index negated with `sub 0` first; a constant index is an `i64` constant | feeding a load or a store, the access's scaled-index address, `(%rax,%rcx,4)`, for a size of 1, 2, 4 or 8, and a constant index the displacement, `0x1(%rax)`; on its own, `shl` for a power-of-two size or `imul` for any other, then `add`; the negation of `p - n` is `xor` and `sub`, of a constant too |
| a constant under an implicit conversion | the constant in the target type, no instruction | an immediate of the width; `long x = 2` is one `movq $2`; an explicit cast of a constant, `(long) 2`, is the `(T) a` row |
| `++p`, `--p`, `p++`, `p--` | `getelementptr` by the `i64` constant 1 or -1 | `add` of the size, `add $0x4` or `add $-0x4` |
| `p - q` | `ptrtoint`, `sub`, `sdiv` by the element size | `sub`, `cqto`, a 64-bit `idiv`; a division even for a size of 4, since G6 promises any two pointers and a shift would floor where the division truncates; for a size of 1 no division, so `(char *) p - (char *) q` is the cheap spelling |
| `p < q` and the rest | `icmp ult`, `ugt`, `ule`, `uge`, `eq`, `ne` on `ptr`, `zext` | `cmp` of the 64-bit addresses, an unsigned `setcc`, `setb` for `<`, then `and $1`, `movzbl` |
| `if (p)`, `p == 0` | `icmp ne` or `eq` against `null` | `cmp $0x0` and the jump or the `setcc`; as a condition, one comparison, since the pointer's own compare is the condition |
| `*p` | `load`, `store`; `align 1` unless the address is a declared object (G11) | `mov` through the register; x86 has one `mov` for every alignment |
| `&x` | the `alloca` | `lea` of the slot |
| `(T *) p`, `(long) p`, `(T *) l` | nothing, `ptrtoint`, `inttoptr`; an `int` extended with `sext` first | nothing; `cltq` for an `int` |
| `"abc"` | a `private global` array of `i8`, neither `constant` nor `unnamed_addr` (G17); its address | a rip-relative `lea`, relocated against `.data`, since a private global has no symbol; the bytes in `.data`, writable |

## 4. What the machine does on its own

- **An unused result is not computed.** FastISel skips an IR
  instruction whose result nothing reads and which has no side
  effect. `a + 0;` as a statement emits nothing. A division with an
  unused result emits nothing, so it cannot fault. This is the one
  departure from "the instruction written is the instruction
  executed", and it has no switch.
- **A compare of one value with itself is folded.** FastISel rewrites
  an `icmp` whose two operands are one IR value to its answer
  (`FastISel::optimizeCmpPredicate`, `FastISel.cpp`). The x86
  selector then emits a constant for the value and an unconditional
  jump for a branch (`X86FastISel::X86SelectCmp` and
  `X86FastISel::X86SelectBranch`, `X86FastISel.cpp`). No option gates
  it. A slot local escapes it: its two reads are two loads, two
  values. The emitter freezes the right operand of a compare whose
  two operands have one name, so the operands differ and the `cmp` is
  emitted. The `freeze` is a `COPY`: for a register the allocator
  merges it away, `cmp %eax,%eax`; a constant takes one `mov` into a
  second register. A comparison, a condition and `!` write their
  `icmp` through one helper, so a test of a constant is a compare
  too: `if (0)` and `!0` compare 0 with a frozen 0, a `cmp` of two
  registers each cleared by an `xor`, and the body of the `if` is
  kept (`zero-test`).
- **Locals are memory.** Every local is loaded before use and
  stored after assignment, every time. A narrow parameter's slot is
  its C width, though the parameter arrives in 32 bits. A `register`
  local or parameter is the exception: it is never stored and lives
  in a register, spilled as a temporary is. A `register` local that
  may be read before a write costs one frame slot, never written, and
  one load at its declaration: a value defined by nothing would be an
  implicit definition, which the allocator need not keep between two
  reads, so each read could see another register (G5). Temporaries
  within an expression are registers. The allocator may spill a
  temporary across a call or a block.
- **An access computes its own address.** A `getelementptr` that
  feeds a load or a store is selected as the access's address, base,
  index and scale, `(%rax,%rcx,4)`, when the element size is 1, 2, 4
  or 8; the slot's offset of a local array is the displacement. A
  `getelementptr` that feeds anything else, or has another size, is
  computed on its own: the index scaled by `shl` for a power of two
  and by `imul` otherwise, then an `add`. Either form is the machine
  form of one IR instruction, as `lea` is of an `add`, not a fold:
  the scaled add is still performed, by the access itself, and
  nothing is simplified away.
- **The frame.** The prologue and epilogue, callee-saved registers,
  stack alignment, the red zone for a leaf's locals: the convention's,
  not the source's. No frame pointer is kept.
- **Unreachable blocks are dropped.** The emitter opens a block after
  every `return` for whatever follows; when nothing does, `llc`
  removes it, and no flag keeps it. Code the source cannot reach is
  not code.

## 5. Verification

The oracle pins the IR; `tests/costs`, which `make test` runs after
it, pins the machine code. Each program under `tests/cost/` is one
row of chapter 3, one rule of chapters 1 and 4, or one flag of
chapter 2, and its opening comment says what its listing must show.
The pin beside it, `<name>.s`, is the object's disassembly by
`objdump -d --no-show-raw-insn --no-addresses`, from the `.text`
line on, relocations included so a call names its callee. A row
passes when the fresh listing equals the pin; the diff of a failed
row is in `build/cost/log`. A new or changed listing is read by hand
against its row before `tests/costs --pin` adopts it. An LLVM
upgrade is checked by running the corpus under the new `llc`: a row
that differs names the construct whose cost moved.

Which rows catch which flag, measured by dropping one flag at a
time:

| Flag dropped | Rows that change |
|---|---|
| `-fast-isel` with its abort | every row but unused |
| `-relocation-model=pic` | string |
| `-disable-cgp` | cgp, early-taildup |
| `-disable-constant-hoisting` | constant-hoisting |
| `-disable-machine-licm` | constant-in-loop, invariant |
| `-disable-machine-cse` | twice, zero-test |
| `-disable-machine-sink` | logical |
| `-disable-branch-fold` | block-order, cgp, early-taildup, if, register-undef |
| `-disable-tail-duplicate` | conditional, early-taildup, if, select |
| `-disable-early-taildup` | early-taildup |
| `-disable-block-placement` | twelve rows |

Without FastISel, the folding selector forwards a stored local to
its next read and merges operations, so nearly every listing
shrinks; unused is the one row it leaves alone. Without
`-relocation-model=pic`, the literal's address in string is a
`movabs` with an absolute relocation.

The other flags change no listing the corpus can produce:
`-disable-lsr` because a counter in a slot is no induction variable,
and a `register` counter that feeds a compare and an add, never an
address, gives the pass nothing to reduce (the `register` loop row
is the same with the flag dropped); `-no-stack-coloring`
because no lifetime marker is emitted, so no two slots may share
(the G12 oracle program shows the sharing under `clang -O2`); the
rest because the pass has nothing to do on these constructs, or is
off by default on x86. They stay in the table: each holds back a
pass that exists, and a later slice may give it something to do.

## 6. The measurement

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
The numbers are bound to the machine, so nothing is pinned. The table
is one run on an x86-64 desktop, Zen 2, with LLVM 23.1.3, on
2026-11-07.

| program | `clang -O2 <name>.c` | `rcc <name>.c` | `rcc <name>-tuned.c` | `clang -O2 <name>-tuned.c` |
|---|---|---|---|---|
| collatz | 292 (1.00) | 2377 (8.14) | 494 (1.69) | 291 (1.00) |
| fib | 260 (1.00) | 388 (1.49) | 347 (1.33) | 266 (1.02) |
| matmul | 284 (1.00) | 2171 (7.64) | 940 (3.31) | 279 (0.98) |
| sieve | 288 (1.00) | 1843 (6.40) | 770 (2.67) | 299 (1.04) |
| sort | 275 (1.00) | 1634 (5.94) | 1209 (4.40) | 280 (1.02) |
| strings | 281 (1.00) | 2370 (8.43) | 760 (2.70) | 270 (0.96) |

The plain source under rcc runs 1.49 to 8.43 times as long as under
`clang -O2`. The tuned source under rcc runs 1.33 to 4.40 times as
long. The tuned source under `clang -O2` runs 0.96 to 1.04 times as
long as the plain one.

**collatz.** The tuned source tests the parity with `& 1` and halves
with `>> 1`, where the plain one writes `% 2` and `/ 2`. The value is
a signed `long`, so only the programmer knows it is never negative;
an unsigned value would not show the division, since the selector
rewrites an unsigned `/` and `%` by a power of two on its own. The
step loop of `rcc collatz.c` has an `idiv` for the test and a second
on the even path, and moves the value and the count to and from their
slots. The step loop of `rcc collatz-tuned.c` has an `and $0x1` and a
`sar $1` in their place and no stack operand. The ratio falls from
8.14 to 1.69, the largest fall in the table.

**fib.** The tuned source declares the parameter and the two partial
sums `register`. `rcc fib.c` builds the frame and stores the
parameter to its slot on every call, the base case included, and
loads it back for the test, the return and each call's argument.
`rcc fib-tuned.c` tests the argument register and returns from the
base case before any frame is built; on the recursive path it keeps n
in `rbx` and the first sum in `rbp`, both callee-saved, across the
second call. The ratio falls from 1.49 to 1.33. The work is the
calls, and `clang -O2 fib.c` keeps one of the two recursive calls and
turns the other into a loop.

**matmul.** The tuned source computes the row bases of a and c once
per row, walks one pointer along a's row and another down b's column,
and keeps the sum in a register, stored to c once per element. The
inner loop of `rcc matmul.c` has four `imul`, three for the indices
`i * n`, `k * n` and `i * n` again and one for the product, twelve
loads of i, j, k and n from their slots, a store of k, and a load and
a store of the element of c. The inner loop of `rcc matmul-tuned.c`
has one `imul`, the product, the two loads of the elements, and no
stack operand. Its step `pb += n` moves 96 into a register, extends
and shifts it at each iteration: n is a `register` variable
initialized with a constant, so it is the constant. The ratio falls
from 7.64 to 3.31.

**sieve.** The tuned source walks a pointer in the marking loop and
in the strike loop, up to an end computed once per repetition, with
the walker, the end, the limit and the count in registers. The strike
loop of `rcc sieve.c` loads j and n for the test, j for the store and
j and i for the step, and stores j back: five loads and a store of
slots for each byte struck. The strike loop of `rcc sieve-tuned.c`
compares the walker with the end, stores the byte through the walker
and adds i, extended by a `movslq`, with no stack operand. The ratio
falls from 6.40 to 2.67.

**sort.** The tuned source walks two pointers down for the shift,
steps with `--q`, loads each element it compares once, and keeps the
key, the index, the walkers and the element in registers. The shift
loop of `rcc sort.c` loads j five times and the key once from their
slots, stores j back, and loads the element `a[j]` twice, for the test
and for the move. The shift loop of `rcc sort-tuned.c` has no stack
operand and one load of the element, at a displacement of -4 from q.
Of the 23 instructions an iteration runs, 17 are the two tests and
the `&&` that joins them: each test sets a byte, masks and widens it,
and the joined value is masked, widened and compared with 0 once more
(the `a && b` row of chapter 3). One more is a `lea` of the array's
address for the test `p != a`. The ratio falls from 5.94 to 4.40, the
least fall in the table after fib.

**strings.** The tuned source walks pointers in `length` and `copy`,
returns the length as the difference of two `char` pointers, tests
the byte `copy` has just stored, and halves the length with `>> 1`.
`length` in `rcc strings.c` loads the pointer and the index from
their slots at each test and the index again at the step, and stores
it back. `copy` in `rcc strings.c` loads s twice, d once and i four
times from their slots for each byte, and the source byte twice. In
`rcc strings-tuned.c` neither function has a stack operand: `length`
steps a register and returns one `sub`, and `copy` loads each byte
once, stores it and tests it. The `idiv` of `n / 2` in `main` is a
`sar`. The ratio falls from 8.43 to 2.70. `clang -O2` turns `length`
into a call of `strlen` in both its builds.
