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
- Locals live in memory, one stack slot each, for the whole call.
  `register` is not honoured yet; when it is, it will be the
  programmer's lever on the allocator.

## 1. The rule

The front end emits one IR instruction per operation in the source,
in the written order, and the back end selects one machine
instruction, or the machine's fixed sequence, per IR instruction.
Between the two, nothing reasons about the program.

What the compiler may do:

- Allocate registers. Which value lives where, and when a value is
  spilled, is the allocator's. A local declared in the source lives
  in its own stack slot regardless.
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
| `a + b`, `a - b` | `add`, `sub` | `add`, `sub`, or `lea` for a three-operand add |
| `a * b` | `mul` | `imul` |
| `a / b`, `a % b` | `sdiv`, `srem`, `udiv`, `urem` | `cltd; idiv` or `xor %edx; div`; the quotient from `eax`, the remainder from `edx` |
| `a << n`, `a >> n` | `shl`, `ashr`, `lshr` | `shl`, `sar`, `shr` by `cl`; the count masked by the instruction |
| `a < b` and the rest | `icmp`, `zext` | `cmp`, `setcc`, `and $1`, `movzbl`; used as a condition, the value is then compared with 0 again, since the tree holds an `int` and the emitter tests it as one |
| `a && b`, `a \|\| b` | branches, `phi` | `cmp $0`, conditional jumps; the short-circuit value set before the jump |
| `a ? b : c` | branches, `phi` | `cmp $0`, conditional jumps; never `cmov` |
| `if`, `while`, `for` | `icmp ne 0`, `br` | `cmp $0`, conditional and unconditional jumps, blocks in the written order |
| `f(a, b)` | `call` | the convention's argument moves, `call` |
| `(T) a` | `trunc`, `sext`, `zext`, or nothing | `movsbl`, `movzbl`, `cltq`, or nothing; a `char` load is `movzbl` from the slot and then the extension, two instructions |
| a `char` or `short` parameter or result | `i32` in the signature, `trunc` at entry, `sext`/`zext` before `ret` and before the call, `trunc` after it | a byte or word store at entry, `movsbl`/`movzbl` or `movswl`/`movzwl` before the call and before `ret` |
| `a += b` | load, convert, op, convert, store | the row of the op, between a load and a store |
| `p + n`, `p - n`, `a[i]` | `sext` of the index to 64 bits, `getelementptr` of the pointee type; for `p - n`, the index negated with `sub 0` first | feeding a load or a store, the access's scaled-index address, `(%rax,%rcx,4)`, for a size of 1, 2, 4 or 8; on its own, `shl` for a power-of-two size or `imul` for any other, then `add`; the negation of `p - n` is `xor` and `sub`; a constant index is widened at run time too, `mov $n` and `movslq` |
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
- **Locals are memory.** Every local is loaded before use and
  stored after assignment, every time. A narrow parameter's slot is
  its C width, though the parameter arrives in 32 bits. Temporaries
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
| `-disable-machine-cse` | twice |
| `-disable-machine-sink` | logical |
| `-disable-branch-fold` | block-order, cgp, early-taildup, if |
| `-disable-tail-duplicate` | conditional, early-taildup, if, select |
| `-disable-early-taildup` | early-taildup |
| `-disable-block-placement` | ten rows |

Without FastISel, the folding selector forwards a stored local to
its next read and merges operations, so nearly every listing
shrinks; unused is the one row it leaves alone. Without
`-relocation-model=pic`, the literal's address in string is a
`movabs` with an absolute relocation.

The other flags change no listing the corpus can produce:
`-disable-lsr` because locals are memory, so even a pointer walk has
no induction variable in a register to reduce; `-no-stack-coloring`
because no lifetime marker is emitted, so no two slots may share
(the G12 oracle program shows the sharing under `clang -O2`); the
rest because the pass has nothing to do on these constructs, or is
off by default on x86. They stay in the table: each holds back a
pass that exists, and a later slice may give it something to do.
