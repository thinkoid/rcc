# rcc: the second target, aarch64

The design of c1 for aarch64, before the code, as `backend.md` was
written before c1. It is in that page's shape and register: the terms
first, then the mechanism, with the standard and the ABI cited where
they govern. `backend.md` stays the design of the x86-64 generator;
this page says what carries over unchanged, what is x86's own and has
no counterpart, and what the new machine asks that the old one did
not. The last chapter lists the calls and their answers. The C
citations are N3220; the ABI citations are the AAPCS64, release
2026Q3, by section title. Every instruction form on this page was
assembled by both `clang` and GNU `as`, and every behaviour stated of
the machine was run on an aarch64 Linux machine.

The three slices of chapter 14, integers, control and calls, then
pointers, arrays and string literals, then register variables, are in
the tree with their pins and their probes. The measurement is taken,
on two machines, and `cost.md` chapter 4 has its tables.

## 0. tl;dr

- The model carries whole. Every local in its slot off the frame
  pointer, `register` variables in callee-saved registers by
  declaration order, expressions left to right in the caller-saved
  registers as a stack, conditions as jumps, every argument a hidden
  local of its call.
- The register variables are ten, `x19` to `x28`: the machine's
  number. The extent is a property of the target, which c0 reads.
- Four parts of the x86-64 design have no counterpart: the pinned
  instructions (6.5), the memory forms of the effect context (6.6),
  the red zone, and the pushes. Each was the shape of an x86
  instruction, not of the model.
- The frame is turned over. The frame record sits at the bottom, `x29`
  points at it, and every slot is a positive displacement off `x29`.
  The encoding decides this: a positive displacement reaches 16 KiB
  for an `int`, a negative one 256 bytes.
- `sp` does not move inside a body, and is a multiple of 16 whenever
  memory is accessed through it. The hardware checks it.
- A slot, an immediate or an address the instruction cannot encode
  costs an instruction, and the cost model says which. A slot beyond
  reach is one `add` into `x16`; the programmer's lever is to declare
  the scalars before the arrays.
- G4 on this machine: `sdiv` and `udiv` do not trap. `x / 0` is 0 and
  `INT_MIN / -1` is `INT_MIN`. The guarantee is unchanged, "what the
  target's instruction does"; the answer is different.
- Plain `char` is unsigned, as the platform has it.
- The oracle carries: `clang -O2` gives the same twelve statuses on this
  machine as on x86-64. The pins are per target.
- A subscript by an `int` is one instruction, `ldr w0, [x1, w2, sxtw
  #2]`: the addressing form extends the index itself (6.3).
- `*p++` on a `register` pointer is one instruction, `ldrb w0, [x19],
  #1`: the addressing form steps the pointer itself (2.3, 6.3).
- The nine calls of chapter 15 are answered.
- The measurement is taken on an Apple M1 Max and on a Qualcomm Oryon
  (`cost.md` chapter 4). Loop-head alignment is measured, not
  adopted: c1 writes no alignment directive on this target (7.2).

## 1. What carries, and what does not

The model is Ritchie's, and it was never x86's. Chapter 2 of
`backend.md` describes it on the PDP-11: the programmer decides what
lives in a register, the compiler writes the instruction for the
operator, an expression's value is held in a register while the next
operand is evaluated, a condition is a jump. c1 for aarch64 keeps
all of it, and keeps c1's three entry points, `rvalue()`, `cbranch()`
and `effect()`, the width rule, the spill rule, the hidden local of an
argument, the left-to-right order of G13 and the label names.

c0 carries too, but for five facts it must now take from the target
(chapter 11): the count of register variables, the signedness of
plain `char` and of `wchar_t`, the frame limit, and the variable
argument list.

What does not carry is what `backend.md` built around the shape of
x86 instructions:

- **Two-address form** (3.3). `add %rcx, %rax` destroys its left
  operand, so a register variable read as a left operand is first
  copied. aarch64 is three-address, `add w0, w19, w1`, and the copy
  is gone (chapter 4).
- **Pinned instructions** (6.5). `idiv` wants `rax` and `rdx`, a
  variable shift wants `cl`. Every aarch64 instruction takes any
  register. The exchange of 6.5, the reservation of `rax` around a
  shift count, and the rule that a call or a division generates
  outside an outer reservation have nothing to do here.
- **Memory operands** (6.6). `addl $1, -8(%rbp)` is one x86
  instruction. aarch64 is a load-store machine: only a load reads
  memory and only a store writes it. The memory forms of the effect
  context are x86's (chapter 6.6).
- **The red zone** (5). The AAPCS64 has none (chapter 3).
- **Pushes** (5, 8). `push` moves `rsp` by 8. On aarch64 `sp` must be
  a multiple of 16 at every access through it, so nothing is pushed
  one register at a time (chapter 5).

## 2. Background: the machine

### 2.1 The registers

Thirty-one 64-bit general registers, `x0` to `x30`. Each has a 32-bit
name, `w0` to `w30`, for its low half; there are no 16-bit or 8-bit
names. A write to the `w` name clears the upper half, as a write to
`eax` clears the upper half of `rax`, so the width rule of
`backend.md` 6.3 holds here unchanged. Register number 31 is the
stack pointer `sp` in an instruction that addresses memory or adjusts
the stack, and the zero register, `wzr` or `xzr`, in most others: it
reads as zero and a write to it is discarded. `x29` is the frame
pointer and `x30` the link register: `bl f` writes the return address
into `x30` and jumps, `ret` jumps to the address in `x30`. No
instruction pushes a return address.

### 2.2 Load-store, three-address

An arithmetic instruction names a destination and two sources, all
registers: `add w0, w1, w2` writes `w1 + w2` into `w0` and changes
neither source. Memory is reached by `ldr` and `str` and their narrow
forms only:

| Access | Load | Store |
|---|---|---|
| `char`, signed | `ldrsb w0, [..]` | `strb w0, [..]` |
| `char`, unsigned | `ldrb w0, [..]` | `strb w0, [..]` |
| `short`, signed | `ldrsh w0, [..]` | `strh w0, [..]` |
| `short`, unsigned | `ldrh w0, [..]` | `strh w0, [..]` |
| `int`, `unsigned` | `ldr w0, [..]` | `str w0, [..]` |
| `long`, a pointer | `ldr x0, [..]` | `str x0, [..]` |

A narrow load extends to 32 bits by its mnemonic, as `movsbl` and
`movzbl` do. `stp` and `ldp` store and load a pair of registers at
adjacent 8-byte addresses in one instruction.

### 2.3 The addressing forms and their reach

An address operand is one of:

- `[x1]`, a base;
- `[x1, #d]`, a base and a constant displacement;
- `[x1, x2]`, a base and an index;
- `[x1, x2, lsl #k]`, a base and an index shifted by the access's
  own size: `#2` for an `int`, `#3` for a `long`, and no other
  amount. `ldr w0, [x1, x2, lsl #3]` does not assemble.
- `[x1, w2, sxtw #k]` and `[x1, w2, uxtw #k]`, a base and a 32-bit
  index that the form itself extends to 64 bits, signed or unsigned,
  and then shifts. `k` is again the access's own size, or absent.

`add` and `sub` take the last operand too, since the machine has no
`lea`: `add x0, x1, w2, sxtw #2` is the address the form above reads
from.

The displacement is where this machine differs most from x86, whose
`-8(%rbp)` takes any 32-bit number. Here it is one of two encodings:

| Encoding | Range | For a 4-byte access |
|---|---|---|
| unsigned, scaled by the access size | 0 to 4095 times the size, a multiple of the size | 0 to 16380 |
| signed, unscaled | -256 to 255 | -256 to 255 |

The reach upward is 4095 bytes for a `char`, 8190 for a `short`,
16380 for an `int`, 32760 for a `long`; the reach downward is 256
bytes for all of them. The assembler picks the encoding from the
number, so c1 writes `ldr w0, [x29, #d]` for both, and must know
when `d` has neither. Chapter 5 is built on this table.

None of the five forms changes its base. Two more do, and each makes
the access and writes `x1 + d` into the base, in its own order:

- `[x1], #d`, post-indexed: the access at `x1`, then `x1 + d` into
  `x1`;
- `[x1, #d]!`, pre-indexed: `x1 + d` into `x1`, then the access at
  the new `x1`.

`d` is the second encoding of the table, signed and unscaled, -256 to
255 whatever the access's size: `ldr x0, [x1, #255]!` assembles, and
`ldr w0, [x1, #256]!` does not. Neither form takes an index, so `ldr
w0, [x1, x2]!` does not assemble either. Every load and store of 2.2
takes both, the store of `wzr` included, `strb wzr, [x1], #1`. A load
or a store whose data register is its base is unpredictable to the
architecture. `clang` refuses `ldr x19, [x19], #8`, "unpredictable LDR
instruction, writeback base is also a source", and GNU `as` assembles
it with a warning, "unpredictable transfer with writeback". c1 never
writes one. The base is a register variable's register, and a load's
data register an expression register; a store whose source is the
base's variable names it in its right operand, so the store keeps the
step out of the access (6.3). The run on the machine is
`autoincrement.md` chapter 2.

### 2.4 Immediates

An x86 instruction takes a 32-bit immediate, and `backend.md` 6.4
could say "there is no code in the table that looks at a constant's
value". Here an instruction is 32 bits whole, and each class has its
own short encoding:

| Instructions | Immediate |
|---|---|
| `add`, `sub`, `cmp` | 0 to 4095, or that shifted left by 12 |
| `and`, `orr`, `eor`, `tst` | a "bitmask": a run of ones, rotated, repeated across the register. `#0xff` is one, `#5` is not |
| `lsl`, `asr`, `lsr` | 0 to 31 on a `w` register, 0 to 63 on an `x` |
| `mov` | 16 bits at one of the positions 0, 16, 32, 48, the rest zeros (`movz`) or ones (`movn`), or a bitmask (`orr` from the zero register): three instructions under one name, and the assembler picks; `movk` writes 16 more bits and keeps the rest |
| `mul`, `sdiv`, `udiv`, `str` | none |

So c1 must look at a constant's value, for one question: does this
instruction encode it. The answer changes the count of instructions
and never the operation (chapter 6.4).

### 2.5 Flags, branches, and the test of zero

`cmp w0, w1` sets the flags, and `b.lt label` and the rest branch on
them: `b.lt`, `b.le`, `b.gt`, `b.ge` signed, `b.lo`, `b.ls`, `b.hi`,
`b.hs` unsigned, `b.eq`, `b.ne`. `cset w0, lt` writes 1 or 0 into a
register from the flags, in one instruction where x86 needs `setl`
and `movzbl`. `b label` is the unconditional jump.

Two instructions test a register and branch without the flags:
`cbz w0, label` branches when the register is zero and `cbnz` when it
is not.

### 2.6 Division

`sdiv w0, w1, w2` and `udiv` divide, into any register. There is no
remainder instruction: a remainder is the quotient multiplied back
and subtracted, for which the machine has `msub w0, w3, w2, w1`,
`w1 - w3 * w2`.

Neither division traps. Run on the machine:

| Expression | Result |
|---|---|
| `7 / 0` | 0 |
| `INT_MIN / -1` | `INT_MIN` |
| `7 % 0`, by `sdiv` and `msub` | 7 |
| `INT_MIN % -1` | 0 |
| `7u / 0` | 0 |

### 2.7 Shifts

`lsl w0, w1, w2`, `asr` and `lsr` take the count in any register and
use its low 5 bits on a `w` register and its low 6 on an `x`, which is
the mask x86 applies at 32 and 64 bits. The count's register is named
at the width of the value shifted: `lsl x0, x1, w2` does not
assemble, and `lsl x0, x1, x2` reads only the low 6 bits of `x2`, so a
32-bit count needs no extension.

## 3. Background: the convention

The AAPCS64, the convention Linux on aarch64 follows and `clang` and
`gcc` compile to. For the types rcc has, integers and pointers:

- The first eight arguments go in `x0` to `x7`. Further arguments go
  on the stack, each in a slot of 8 bytes, the ninth at `[sp]` at the
  call and the next above it.
- "Any part of a register or a stack slot that is not used for an
  argument (padding bits) has unspecified content at the callee entry
  point" (Parameter passing rules). So the callee extends a `char` or
  a `short` argument. `clang -O2` stores a `char` stack argument with
  `strb w10, [sp]` and its callee reads it with `ldrb w8, [sp]`.
- The result returns in `x0`, "in the same registers as would be used
  for such an argument" (Result return), and so with the same
  unspecified upper bits: the caller extends a narrow result. `clang
  -O2` compiles `char f(char c) { return c + 1; }` to `add w0, w0, #1;
  ret`, and a caller of a `short` function to `bl g; sxth w8, w0`.
- `x19` to `x28` are callee-saved, and so are `x29` and `sp`. `x0` to
  `x15` are caller-saved. `x8` carries the address of a result
  returned in memory, which rcc does not have yet.
- `x16` and `x17`, named IP0 and IP1, may be changed by the linker's
  code between a `bl` and the function it reaches; between calls
  "they can also be used within a routine to hold intermediate
  values" (General-purpose Registers). `x18` is the platform's and is
  left alone.
- The stack: "at any point at which memory is accessed via SP, the
  hardware requires that SP mod 16 = 0" (Universal stack
  constraints), and at a call it must be so as well. Linux arms the
  check: a function that subtracts 8 from `sp` and stores through it
  dies of SIGBUS.
- No red zone: "No thread is permitted to access (for reading or for
  writing) the inactive region" of its stack (the same section), and
  the inactive region is everything below `sp`. A leaf has a frame
  like any other function.
- The frame record: "a frame record of two 64-bit values on the
  stack", the caller's `x29` at the lower address and the value of
  `x30` on entry at the higher, with `x29` pointing at it. "The
  location of the frame record within a stack frame is not
  specified" (The Frame Pointer).
- A variadic callee takes its arguments as any other does. There is
  no counterpart of `al`.
- Plain `char` is the unsigned byte (Arithmetic types).

A string's address, and the address of a function the file defines,
are formed PC-relative in two instructions, `adrp x0, sym` for the 4
KiB page and `add x0, x0, :lo12:sym` for the offset in it. A function
the file only declares has its address loaded from the GOT, `adrp x0,
:got:f` and `ldr x0, [x0, :got_lo12:f]` (chapter 10). A call is `bl
f`; the linker makes the PLT entry when `f` is in a shared library,
and no `@PLT` is written. A call through a pointer is `blr` on the
register that holds it.

## 4. The register discipline

Thirty-one registers, five classes, fixed at design time.

**Register variables: `x19` to `x28`.** Ten, the whole callee-saved
set but the frame pointer. A function's `register` variables take them
in order of declaration, the first `x19`. Everything else of
`backend.md` chapter 4 holds with the names changed: a variable keeps
its register to the end of its block, sibling blocks share, a later
declaration of a block takes a register above its closed inner
blocks', one whose register would be past the ten takes a slot and
the warning, `register
variable 'k' in a stack slot: the ten registers are taken`. The extent
is implementation-defined (6.7.2p12) and is now a number per target,
which the cost model records for each.

The representation is the same: the register holds the value
extended to 32 bits by its signedness, a `long` or a pointer whole.
The rules of reading and writing differ in one place, and for the
better:

- A read is the register as an operand, and no instruction.
- **No copy before an instruction that writes.** On x86 the left
  operand's register is the destination, so `a - b` on two register
  variables is `movl %ebx, %eax; subl %r12d, %eax`. Here the
  destination is named apart: `sub w0, w19, w20`, the result in a
  fresh expression register, the variables untouched.
- An assignment's value is computed in the register, the variable
  being a consumer that names its register (6.1): the row that
  computes it writes the variable's register, `ldr w19, [x29, #24]`,
  `add w20, w20, w21`, `cset w19, lt`, `sxtw x19, w0`, wherever the
  row writes its destination only after it has read every operand and
  its last instruction leaves the representation above. An address
  that the page and its offset form is two writes and reads no
  operand, `adrp x19, buf; add x19, x19, :lo12:buf`, and so is the
  GOT's, `adrp x19, :got:f; ldr x19, [x19, :got_lo12:f]`, and a slot
  beyond the reach of an `add`, `add x19, x29, #4096; add x19, x19,
  #1928`. A pointer plus a constant is two writes where the pointer's
  own row can write the register, the `add` of the constant reading
  nothing else: `p = q[1]` on an `int (*q)[3]` in memory is `ldr x19,
  [x29, #24]; add x19, x19, #12`, and no expression register is taken.
  A pointer whose row writes no register variable keeps its register,
  `p = &f(x)[1]` being `add x19, x0, #4` after the `bl`, and the
  variable's own read is the base already, `p = &p[1]` the one `add
  x19, x19, #4`. The pointer of a sum by an integer that is no
  constant is generated elsewhere, since the integer could read the
  variable after the pointer's row wrote it. An lvalue's address plus
  a constant is the same: the GOT's load of an array the file only
  declares, or the pointer a dereference reads, writes the register,
  `p = &declared[3]` being `adrp x19, :got:declared; ldr x19, [x19,
  :got_lo12:declared]; add x19, x19, #12` and `p = &m[1][2]` on `int
  (*m)[3]` `ldr x19, [x29, #24]; add x19, x19, #20`; where the
  constants sum to 0, `p = &m[1][-3]`, the load is the whole value and
  nothing is moved after it. A row by a variable index takes the
  register too, `add x19, x0, x1; add x19, x19, #8` for `p = &m[i][2]`,
  since its `add` is its own instruction whichever register it writes.
  An operation whose right operand reads nothing the left operand's
  row could overwrite is the same, a constant, held or an immediate,
  or another register variable's read: the left operand's row writes
  the register and the operation is on it, `r = x + 7` being `ldr w19,
  [x29, #24]; add w19, w19, #7`, `r = x * 4` `ldr w19, [x29, #24]; mov
  w0, #4; mul w19, w19, w0`, and `r = x % 3` `sdiv w1, w19, w0; msub
  w19, w1, w0, w19` after the load, `msub` reading every source before
  it writes. These are x86-64's two conditions on the right operand
  (`backend.md` chapter 4) for a machine whose instruction reads both
  sources first: R is generated after the left operand's row has
  written the register, so R must not read the variable, and `r = x +
  r` keeps `ldr w0, [x29, #24]; add w19, w0, w19`; any other right
  operand keeps the left operand elsewhere. `r = r + x`, on the
  variable's own read, is the one `add` already. The operation is in
  the type the variable's value is computed in: an `int` one widened
  into a `long`, `l = i + 5`, keeps its `sxtw x19, w0`, and a `long`
  one narrowed into an `int` and one into a narrow variable their own
  registers. A constant left operand is the constant row into the
  register, `mov w20, #5; sub w20, w20, w19` for `r = 5 - s`, and a
  constant of two instructions is two writes of it that read no
  operand, as an address of a page and its offset is, `mov w20,
  #22136; movk w20, #0x1234, lsl #16` before the `sub`; `r = 5 - r`,
  whose right operand is the variable, keeps `mov w0, #5; sub w19, w0,
  w19`. A difference of pointers at a size other than 1 is the same:
  its `sub` writes the register and its `sdiv` divides it in place,
  `w = q - a` on a `register int *q` into a `register long w` being
  `sub x20, x19, x0; mov x0, #4; sdiv x20, x20, x0`, both pointers
  read by the `sub` and the size's `mov` writing an expression
  register.
  A conversion that changes the signedness alone is written
  there too: an object under it is loaded at the extension of the
  conversion's type, `ldr w19, [x29, #32]` for `register unsigned u =
  i`, and any other value of 32 bits or more is its own row's, `add
  w19, w0, w1` for `u = a + b` on two `int`s, since a write of a `w`
  register clears the upper half; a second such conversion under the
  first hands the variable on as the first does, `v = 5 + (int) u`
  into a `register unsigned v` being `ldr w19, [x29, #24]; add w19,
  w19, #5`. The value of a step on an object in
  memory, `r = x++`, is the load of `x` into the register, `ldr w19,
  [x29, #24]`, and the step after it on a fresh register and the
  store, `add w0, w19, #1; str w0, [x29, #24]`, except where the
  address reads the variable's register, `r = a[r]++`, whose store
  goes through that register after the load; and `r = s++` on another register
  variable is G13's copy into `r`'s register, `mov w20, w19`, before
  the `add`. The value of a prefix step on an object of 32 bits or
  more, `r = ++x`, is the load into the register, the step on it and
  the store from it, `ldr w19, [x29, #24]; add w19, w19, #1; str w19,
  [x29, #24]`, but not where the address of the object reads the
  variable's register; a narrower object's sum is no value of its
  type, so `r = ++c` on a `char` keeps the step in an expression
  register and the re-extension, `uxtb w19, w0`, after the store, as
  a widened one keeps its `sxtw x19, w0`.
  The variable is written after the row has read every
  operand, so no condition on them is needed, where x86-64 takes two
  (`backend.md` chapter 4). Any other value is computed elsewhere and
  moved in, `mov w20, w0`, or `mov x20, x0` for a `long` or a pointer:
  another register variable's read, `mov w19, w20`; a call's result,
  in `x0` by the convention; a step's whose address reads the
  variable, or that a conversion widens after the load; a load
  through a register pointer stepped into the same pointer, since a
  load's data register cannot be its stepped base (2.3); and a value
  converted by a narrowing or a widening to a narrow or an `unsigned`
  variable, whose extension is the move, `sxtb w19, w0` for a `signed
  char`, `uxtb w19, w0` for a `char`, `mov w19, w0` for an `unsigned`
  from a `long`. The variable's own read is its register's value
  already and writes nothing, as `backend.md` chapter 4 has it: `r =
  r`, a narrow `c = c` among them, and an arm `r` of `r = c ? x : r`,
  of a register pointer too, which leaves the conditional one path
  past the condition's branch, whatever the other arm is (`backend.md`
  7.2): `cbnz w0, .L3` over `ldr w19, [x29, #28]` for `r = c ? r : x`,
  and for `r = c ? f(x) : r` `cbz w0, .L3` over the `bl` and the
  `mov w19, w0` after it, the move into the register of any arm whose
  row writes no register, below; a narrow variable re-extended after an
  operation in place is a write of new bits and keeps its extension. A
  constant is the immediate, `mov w20, #0`. `&&`,
  `||` and `?:` are written on two paths, and each path writes the
  register, by the rule and the edge of `backend.md` chapter 4: `r = a
  && b` is `mov w19, #1` and `mov w19, #0` after the jump code, and `r
  = c ? x : y` each arm by its own row, `ldr w19, [x29, #28]`, a
  constant by the constant row, and a `?:` of another pointer type into
  a pointer variable a load into `x19` on each path, `add x19, x19,
  #20` after the join for `q = &(c ? m : n)[1][2]`. More arms' rows
  write the register here than on x86-64, since a row writes its
  destination with its last instruction: an operation, a quotient's
  `sdiv` among them, as well as a constant, an object in memory, a
  truth value and a step's value. So a first arm whose row does not, a
  call's result, is moved in on its own path, `mov w19, w0` after the
  `bl`, where the second arm's row writes the register, and `r = c ?
  f(x) : a / b` is that move and `sdiv w19, w0, w1`. One the
  assignment widens, `l = c ? x : y` into a `long`, hands the
  conversion to each arm, `ldrsw x19,
  [x29, #28]` and `ldrsw x19, [x29, #32]`, a constant the constant of
  the variable's type, `mov x19, #-1`, and two `char`s, `l = c ? a :
  b`, the promoted load at the width on each path, `ldrb w19, [x29,
  #28]` (6.3). Two arms whose rows write no register, `r = c ? f(x) :
  f(x + 1)`, or each of which needs an extension of its own, `l = c ?
  a + b : a - b` on `int`s, join as for any consumer and keep the
  `mov` or the extension after the join, `sxtw x19, w0`. A
  declaration's initializer is the same.
  Without one nothing is written (G5), but a narrow or an `unsigned`
  variable is extended in its own register, `sxtb w19, w19` or `mov
  w19, w19`, so that the register holds the representation above.
- A compound assignment and `++` operate in place, `add w21, w21,
  #1`, and a narrow one is re-extended after, `sxtb w19, w19`. `%=`
  keeps the quotient in a register of its own, since `msub` still
  reads the variable: `sdiv w0, w19, w20; msub w19, w0, w20, w19`. An
  operation wider than the variable, `int += long`, is made on the
  read's extension in a fresh register and moved in, `sxtw x1, w19;
  add x1, x1, x0; mov w19, w1`, and an `unsigned` one that a 64-bit
  operation wrote whole is re-established, `add x19, x19, x0; mov
  w19, w19`.
- G13's copy stays: a read still held when its variable is written is
  copied into an expression register first. A postfix `++` or `--`
  for its value is such a write, and the copy is the value: `i++` is
  `mov w0, w19; add w19, w19, #1`. The two invariants of
  `backend.md` 6.7, at a branch and at a call, carry as written; a
  borrowed read is the common case here, since no instruction copies
  one.
- An access through a `register` pointer stepped by `++`, `--` or a
  compound assignment by a constant makes the step itself, by a form
  of 2.3 that writes the variable's register: `*p++` on a `char *`
  is `ldrb w0, [x19], #1`, where the rules above write three
  instructions, the copy, the step and the access through the copy
  (`autoincrement.md` chapter 3). The access writes the variable,
  so a read of it still held is copied first, as before any write.
  The row and its conditions are 6.3.
- A `register` parameter is moved from its argument register in the
  prologue, by the extending form for a narrow one, `sxth w19, w1`
  for a `short`, since the convention leaves its upper bits
  unspecified. One past the eighth is loaded straight into its
  register from where the caller stored it, `ldrsb w19, [x29, #64]`.
  The moves and the loads come in parameter order among the stores of
  the first eight, as on x86-64 (`backend.md` chapter 4): a load
  follows those stores.

**The frame pointer: `x29`.** Set in every function's prologue, leaf
included, and every slot is a displacement off it.

**The link register: `x30`.** Stored in the frame record by every
prologue and loaded by the epilogue, leaf included. A leaf could keep
its return address in the register and store nothing. It does not:
one prologue shape, and the record is the chain a debugger walks.

**Expression registers: `x0` to `x7`, `x9` to `x15`.** Fifteen, the
caller-saved set without `x8`, used as a stack: the generator takes
the lowest free one. `x0` is first, so a return value tends to be
where it is wanted, and a `return` names it, so that its value is
computed there where the last instruction can write it (6.1). Nine
held any expression on x86; fifteen do here, and the spill path is
the same code.

**The scratch pair: `x16`, `x17`.** Never an expression register and
never held across a node. `x16` takes an address the instruction
cannot reach (chapter 5.3), from `x29` for a slot and from `sp` for an
argument in the outgoing area (chapter 8), the page of a global the
file defines for its access (chapter 10), the callee of a call
through a pointer, loaded from its slot for the `blr` (chapter 8),
and the count of a record copy's loop (6.6). `x17` carries the bytes
of a copy from memory to memory, which the machine has no instruction
for: a stack argument on its way from its slot to the outgoing area,
a stack parameter on its way to its slot, and each unit of a record's
copy (6.6). Both are dead at every `bl` by the convention, which is
what suits them to a use that never spans one.

`x8` and `x18` are not used. `sp` is the stack pointer, and nothing
else.

## 5. The frame

### 5.1 The choice the encoding makes

`backend.md` puts the frame pointer at the top of the frame and every
slot at a negative displacement, known the moment the slot is
declared. Mirrored here, that reaches 256 bytes (2.3): the first
array of a function would put every later local beyond the
instruction. The positive displacement reaches 16 KiB for an `int`.
So the frame is turned over: the record at the bottom, `x29` pointing
at it, the slots above it, each at the next higher offset as the
generator meets it. An offset is still known at the declaration, and
the total is still known only after the body. `gcc` lays its frames
out this way.

From low addresses to high:

1. The frame record, 16 bytes at `[x29]`: the caller's `x29`, then
   `x30`.
2. The register-variable registers the function uses, 8 bytes each
   from `[x29, #16]`, stored in pairs by `stp` and an odd last one by
   `str`. Their count is the parser's, known before the body, as on
   x86, since the slots lie above them.
3. One slot per parameter and per local, each of its type's size at
   its type's alignment, in declaration order, none shared (G12).
4. One slot per argument of each call, as the call is met.
5. The spill slots, as they are needed.

The total, `T`, is rounded up to 16. Above it lie the arguments the
caller stored for a ninth parameter and on.

### 5.2 The prologue and the epilogue

One shape for every function:

```
sum:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -32
	.cfi_offset w20, -24
	str	x21, [x29, #32]
	.cfi_offset w21, -16
	...
.Lsum.1:
	.cfi_def_cfa wsp, 48
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldr	x21, [x29, #32]
	.cfi_restore w21
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
```

This listing was assembled and run under a `clang`-compiled `main`,
with a callee that walked the frame chain through it, and stepped by
instruction under `gdb`, which printed `main` in its backtrace at
every one.

The epilogue's directives keep the unwind table true at each of its
instructions. From the prologue's `.cfi_def_cfa_register w29` the
table computes the canonical frame address from `x29`, and the `ldp`
gives `x29` back to the caller; so the epilogue names `sp` first, at
its label, where `sp` equals `x29`. Once `sp` has moved up, the saved
registers lie below it, and the convention has no red zone (chapter
3): a signal's frame may overwrite them. So each `.cfi_restore` says
that its register holds the caller's value again, and
`.cfi_def_cfa_offset` follows each `add` with what is left of the
frame.

The listing's function has three register variables, and their
registers are the save area of 5.1: directly above the frame record,
from `[x29, #16]`, and below the first slot. The prologue stores them
right after `mov x29, sp`, two at a time by `stp` and an odd last one
by `str`:

- one register is `str x19, [x29, #16]`;
- two are `stp x19, x20, [x29, #16]`;
- three are that pair and `str x21, [x29, #32]`.

Each store is followed by a `.cfi_offset` for each register it
stores, at its place counted from the canonical frame address, `16 +
8i - T` for the register `i` places past `x19`: `w19` is at -32 in
the listing's frame of 48 bytes, and at -16 in a frame of 32. The
epilogue loads them back by the same pairs in the same order, after
`.cfi_def_cfa wsp, T` and before the frame record, each load followed
by a `.cfi_restore` for each register it loads. A function with no
register variable has no save area, and its first slot is at `[x29,
#16]`. The area is in reach of its `stp` and `ldp` whatever the
frame's size: their displacement is signed and scaled by 8, -512 to
504, and the last pair of the ten registers is at `[x29, #80]`.

`sp` equals `x29` from the prologue to the epilogue, but inside a
call that passes arguments on the stack (chapter 8). It is a multiple
of 16 throughout, since `T` is. The immediate of `sub` is 12 bits, or
12 bits shifted by 12, so a frame of 4096 bytes or more is opened in
two instructions, `sub sp, sp, #110592; sub sp, sp, #48`, the upper
part first, and in one when its low 12 bits are zero. Each `sub` is
followed by its `.cfi_def_cfa_offset`, with the part of the frame
opened so far: a frame of 8208 bytes is `sub sp, sp, #8192;
.cfi_def_cfa_offset 8192; sub sp, sp, #16; .cfi_def_cfa_offset 8208`.
The epilogue's `add`s close it in the same order. A frame over 16 MiB
less 16 is a not-yet, refused as `backend.md` chapter 5 has it: a
single local at its declaration by the parser, a sum of smaller ones
by the generator.

`stp x29, x30, [sp, #-48]!` would open a frame of up to 512 bytes in
one instruction where this shape takes two. It is not used: a second
prologue shape chosen by the frame's size.

A parameter of the first eight is stored from its argument register to
its slot, `str w0, [x29, #40]`, at its width. A parameter past the
eighth lies above the frame at `[x29, #T + 8(i - 8)]`, `i` its
position counted from 0: the ninth parameter, `i` = 8, is at `T`, and
the tenth at `T + 8`. `T` is not known while the body is generated. So
the prologue, which is written after the body, copies it to a slot of
its own: `ldr w17, [x29, #T]; str w17, [x29, #44]`. The copies follow
`mov x29, sp` and the save area's stores, and precede the stores of
the first eight. Every parameter is then a slot at a known offset, at
two instructions for each one past the eighth. On x86 such a
parameter stays where the caller pushed it.

A variadic definition that calls `va_start` has the area the list is
built from (`shape.md` chapter 4), the convention's register save
area: 200 bytes, the function's first slot, aligned to 16, and written
first after the save area's stores, before the copies of the
parameters past the eighth and the stores of the first eight. `x0` to
`x7` go from its start by four `stp`, `stp x0, x1, [x29, #16]` to `stp
x6, x7, [x29, #64]`, the named parameters' among them, `q0` to `q7`
from 64 by four `stp` of `q` registers, `stp q0, q1, [x29, #80]` to
`stp q6, q7, [x29, #176]`, every register whatever the caller passed,
since a list handed to the C library may carry a `double` in a vector
register; and at 192 the address of the first argument the caller left
on the stack past the named ones, `x29` plus `T` and 8 for each named
parameter past the eighth, formed in `x16`: `add x16, x29, #272; str
x16, [x29, #208]`. The parameters' stores and copies read the argument
registers and write none of them, so the area holds the registers as
they arrived whatever comes after it. The `stp` of `q` registers takes
a displacement scaled by 16 up to 1008, and the area, at the bottom of
the frame, is in its reach.

### 5.3 A slot beyond reach

A slot's displacement is encodable for its access when it is a
multiple of the access size no greater than 4095 times it (2.3). The
frame pointer is at the bottom, so no displacement is negative.

A slot beyond that is reached through `x16`, in one more instruction:

```
	add	x16, x29, #36864
	ldr	w0, [x16, #24]
```

The `add` takes the displacement's upper 12 bits, shifted, and the
access takes the low 12. Its address as a value, for `&x` or an
array's decay, is `add x0, x29, #36864; add x0, x0, #24`, and one
`add` when either part is zero.

An upper part of 16 MiB or more is past the `add`'s immediate. It is
held in `x16` by the constant row of 6.4 and added, and the access
takes the low 12 bits as before:

```
	mov	x16, #16777216
	add	x16, x29, x16
	ldr	x17, [x16]
```

No slot lies there, since the frame is under 16 MiB (5.2); a
parameter past the eighth does, when the frame is near its limit, and
the prologue's copy reaches it so.

A global the file defines is reached by the same mechanism with the
symbol's page in place of the upper part: `adrp x16, counter`, then
the access through `[x16, :lo12:counter]` (chapter 10).

This is a cost the programmer controls, and the cost model gives it a
row: slots are laid out in declaration order, so a function that
declares `int a[9216]` three times and its counters after them puts
every counter beyond reach, and one that declares the counters first
does not. The generator does not sort the slots. Sorting is a
judgment about the program, and the order of declaration is the
programmer's to choose, as it is for which three variables got the
PDP-11's registers.

## 6. Expressions

### 6.1 The rule

Unchanged from `backend.md` 6.1 in what it says: the left child into
register `L`, the right into `R`, one instruction for the operator,
`R` freed, the result in `L`. The instruction is written `op L, L, R`.
Where x86-64 names an object in memory as the right operand in place
of `R` (`backend.md` 6.1), this target has nothing to take: a
load-store machine has no instruction with a memory operand, so every
operand is loaded into `R` by the load of 2.2. When `L` is a register
variable's read, the result takes a fresh expression register,
`op N, L, R`, and that is the whole of what x86's
`writable()` copy becomes. `R` is freed before `N` is taken, in the
rule's order, so `N` may be `R`'s register: `x * 4` is `mov w0, #4;
mul w0, w19, w0`. A row of two instructions frees `R` after the last
that reads it, the remainder's `msub` (6.3). A postfix `++` or `--`
for its value on an object in a slot is the same case: the load leaves
the old value in `L`, the new value takes a fresh register, `ldr w0,
[x29, #16]; add w1, w0, #1; str w1, [x29, #16]`, and x86's copy of the
old one is gone. On a register variable it is not the same case. The
variable is written in place, so G13's copy of chapter 4 stays and is
the value: `mov w0, w19; add w19, w19, #1`. Left first is G13; a
commutative operator's operands are not swapped, but for a constant.

A value is computed in the register its consumer names (`backend.md`
6.1). Two consumers name one here: a register variable (chapter 4) and
a `return`, which names `x0`. The return's name goes down to the
instruction that last writes the value, through a cast's operand, `&`
of a subscript, the comma's right operand, an assignment's right
operand and each arm of `?:`. That instruction writes `x0` where no
other value holds it: an operation, a compound assignment's included,
a shift, the `sdiv` or the `msub` of a division, `neg` and `mvn`, an
extension, the `cset` of a comparison and of `!`, and the `add`, `sub`
or `sdiv` of pointer arithmetic and of a pointer difference. It names
its destination apart from its sources and reads every source before
it writes, so a source in `x0` is no obstacle. A left operand reloaded
after a branch or a call is `ldr x1, [x29, #24]; add w0, w1, w0`, the
right operand read in `x0`. A remainder by a call's result is `sdiv
w2, w1, w0; msub w0, w2, w0, w1`: the `sdiv` writes the quotient's
register, taken outside both operands, so it overwrites no source of
the `msub`. No copy precedes an instruction that writes, so `return r
- p[i]` is `sub w0, w19, w0`, and a remainder returned as a `long` is
`msub w0, w2, w1, w0; sxtw x0, w0`.

One thing generated before that instruction is kept out of `x0`: the
place of an assignment, of a compound assignment or of a step on an
object in memory, whose address the store reads after the value. The
place is generated with `x0` left for last, the state's reservation,
which `take()` honours as an order of preference (6.5), so that the
value takes `x0`. `return p[i] = x` is `ldr x1, [x29, #16]; ldr w2,
[x29, #24]; ldr x0, [x29, #32]; str x0, [x1, w2, sxtw #3]`; `return
p[i] += x` writes `add x0, x3, x0` before the store; and `return
p[i]++` loads the old value into `w0`, `ldr w0, [x1, w2, sxtw #2]; add
w3, w0, #1`, the new value in a register of its own. A call's result
within the place is in `x0` by the convention, and the name stops at
the store: `return p[f(i)] = x` is `str w1, [x2, w0, sxtw #2]; mov w0,
w1`.

A conditional takes the name at the top of the returned value: its
arms join in `x0`, and each arm is generated for it, so that its last
instruction writes `x0` where it can. `return c ? p[i]++ : b` is the
first arm's `ldr w0, [x1, w2, sxtw #2]` and the second arm's `ldr w0,
[x29, #20]`, with no move. An arm whose value is elsewhere, a register
variable's or one whose place holds a call's result, is moved there.

Below a cast, a comma or an assignment a conditional takes the name
by the rule and the edge of `backend.md` 6.1, read for this machine's
rows. Under a cast that widens it, the conversion goes to each arm
with the name where one arm at least widens with no instruction of its
own: a constant, `mov x0, #-5`, an object in memory by its extending
load, `ldrsw x0, [x29, #32]`, a truth value, or a register variable's
read, whose copy into the join its extension replaces, `sxtw x0, w19`.
An extension here writes any register from any other, so the other
arm's extension writes `x0` from wherever its value is: `return a ?
(*p)++ : b` as a `long` is `sxtw x0, w0` after the step and `ldrsw x0,
[x29, #32]`, with nothing after the join. A consumer that names no
register has the same conversion handed to each arm, joined in the
first arm's register, as `backend.md` 6.1 has it: `h = c ? a : b` on
two `signed char`s into a `long` in memory is `ldrsb x0, [x29, #20]`
and `ldrsb x0, [x29, #21]`, the store after the join. Otherwise, and
below any other cast, a comma or an assignment, the arms join in `x0`
where each takes the name by its row, with no instruction of its own
or with the copy it needs in any case: a constant, a load, a call's
result, a truth value, an operation on integers, a register
variable's read, an
assignment or a step whose place holds no call. `return (long) (c ?
(*p)++ : b)` on two `char`s is the old value's `ldrb w0, [x1]`
extended on its own path, `sxtw x0, w0`, and the second arm's `ldrb
w0, [x29, #32]`, valid to 64 bits, with nothing after the join. Two
arms that store through a call's result, which holds `x0` at the
store, keep the join in their one register: `return (long) (c ? (*g()
= a) : (*h() = b))` is `str w1, [x0]` on each path and `sxtw x0, w1`
after the join, where the name would move each arm into `x0`.

At the top of the returned value a conditional joins in `x0` whatever
its arms, and two such arms are each moved there, `mov w0, w1`, where
a join in their register and the one move after it is an instruction
fewer. The rule does not serve there: the join it falls back to is
the name's own here, and the arms it does not vouch for may take `x0`.
`return c ? (*g() = a) : b` joined in the first arm's register would
move the second arm into it and then into `x0`, an instruction more
than the name's one move.

The moves left before the branch to the epilogue are thus two:

- a register variable's value, `mov w0, w19`, which is the
  instruction;
- the value of an assignment or of a step whose place holds a call's
  result.

A constant on the left of a comparison or of `+`, `*`, `&`, `|` or
`^` is taken on the right, as on x86-64 (`backend.md` 6.1): a
comparison is mirrored, `5 < x` is `ldr w0, [x29, #16]; cmp w0, #5`
and `b.le`, the constant `cmp`'s immediate, and `0 == p` is `cmp x0,
#0`; the other five commute, `5 + x` is `ldr w0, [x29, #16]; add w0,
w0, #5` and `7 & x` the load and `and w0, w0, #0x7`. A constant the
instruction does not encode, and `mul`'s, which takes no immediate,
is taken on the right too, and held after the right operand, `mov w1,
#5000; cmp w0, w1`, `ldr w0, [x29, #16]; mov w1, #4; mul w0, w0, w1`
for `4 * x`. It costs its `mov` in either order, and no instruction
here names an object in memory, so the exchange never costs an
instruction; on the right the constant is held across nothing, so `5
* f()` is `bl f` and the `mov` after it, where the source's order
would hold the 5 across the call in a spill slot. x86-64 keeps the
order for a constant no immediate holds, and for one whose order is
the operation on a register variable's register (`backend.md` 6.1).
`-`, `/`, `%` and the shifts keep their order.

### 6.2 Depth and spill

Unchanged. A spill is `str x0, [x29, #d]` and a reload `ldr x0, [x29,
#d]`, 8 bytes. Fifteen registers run out at sixteen values held at
once.

### 6.3 The table

For a 32-bit operation (`w` names) or a 64-bit one (`x` names). A
row's last instruction writes `L`, or the register a consumer names:
a register variable's where the row leaves its representation
(chapter 4), and a return's `x0` where no other value holds it (6.1).

| Source | Instruction | Note |
|---|---|---|
| a constant | an immediate where the instruction encodes it; else `mov L, #c`, with `movk` for each further 16 bits a wide one needs | 6.4 |
| a local, a parameter | the load of 2.2 from `[x29, #d]` | through `x16` beyond reach, 5.3 |
| a global the file defines | `adrp x16, sym`, then the load or the store of 2.2 through `[x16, :lo12:sym]`, wherever a row reads or writes a slot: `ldr w0, [x16, :lo12:sym]`, `str w0, [x16, :lo12:sym]` | two instructions an access, chapter 10 |
| a global the file only declares | `adrp L, :got:sym; ldr L, [L, :got_lo12:sym]`, its address from the GOT, then the row of `*p` through `L`: `ldr w0, [L]`, `str w1, [L]` | chapter 10; each access the source writes loads the address once |
| a `register` variable | the register, as a source operand; as `L` of a row that writes its result, the result in a fresh `N`, `sub w0, w19, w20`, `lsl w0, w19, #3`, `add x0, x19, w20, sxtw #2` for `p + n`, `sub x0, x19, x20` for `p - q` before its `sdiv` | no instruction, and no copy (6.1) |
| `a + b`, `a - b` | `add L, L, R`, `sub L, L, R`; a constant on the left of `+` taken on the right (6.1), `5 + x` the load and `add w0, w0, #5` | G1 |
| `a * b` | `mul L, L, R`; a constant on the left taken on the right (6.1), `4 * x` the load, `mov w1, #4` and `mul w0, w0, w1` | G1; a constant factor is first loaded, `mov R, #4` |
| `a / b` | `sdiv L, L, R`, `udiv L, L, R` | G4: no trap, 2.6 |
| `a % b` | `sdiv N, L, R; msub L, N, R, L`; for `L` a register variable's read, `msub` is the last to read `R`, so its result takes the register `R` leaves, as in every row (6.1), and the quotient's register is taken and freed as before: `sdiv w1, w19, w0; msub w0, w1, w0, w19`; a return's `msub` writes `x0` with `R` in it, `sdiv w2, w1, w0; msub w0, w2, w0, w1` (6.1) | the machine has no remainder; `N` is taken and freed |
| `a << b`, `a >> b` | `lsl L, L, R`, `asr`, `lsr`; a constant count is the immediate, masked to the width | G3; the count in any register |
| `a & b`, `a \| b`, `a ^ b` | `and`, `orr`, `eor`; a constant on the left as for `+` | |
| `-a`, `~a` | `neg L, L`, `mvn L, L` | |
| `!a` | `cmp L, #0; cset L, eq`; for an operand with stale upper bits, `tst L, #0xff` in place of the `cmp` (7.1) | as a value |
| `a < b` and the rest, as a value | `cmp L, R; cset L, lt`; a constant on the left the mirrored comparison (6.1), `5 < x` the load and `cmp w0, #5; cset w0, gt` | `lt`, `le`, `gt`, `ge`, `eq`, `ne` signed; `lo`, `ls`, `hi`, `hs` unsigned and for pointers (G7) |
| `a && b`, `a \|\| b`, `a ? b : c`, as a value | jump code into `L`, `mov L, #1` and `mov L, #0` on the arms; the arms of a returned `?:` join in `x0`, each arm generated for it and moved there where its value is elsewhere, `ldr w0, [x1, w2, sxtw #2]` for a postfix step's old value, and under a cast that widens the conditional each arm extended into `x0` where one widens free, `ldrsw x0, [x29, #32]` (6.1); into a register variable each path writes its register, `mov w19, #1` and `mov w19, #0` for `r = a && b`, where an arm's row writes it for `?:`, the other arm's value moved in on its own path, `mov w19, w0` after a call, and under a conversion that widens the conditional each arm extended into it, `ldrsw x19, [x29, #28]` (chapter 4) | never `csel`: 6.5.16p5, the arm not chosen is not evaluated |
| `x = e` | the store of 2.2 to the slot, a return's value written into `x0` by `e`'s last instruction, the place generated with `x0` left for last, `add w0, w1, w0; str w0, [x29, #24]` and `ldr x0, [x29, #32]; str x0, [x1, w2, sxtw #3]` (6.1); for a register variable the last instruction of the row that computes the value writes its register, `add w19, w0, w1`, `ldr w19, [x29, #24]`, where the row writes its destination only after it has read every operand, `adrp x19, buf; add x19, x19, :lo12:buf` for an address in two instructions, and through a conversion that changes the signedness alone, `ldr w19, [x29, #32]` for `register unsigned u = i`, and otherwise the `mov` into it, `mov w20, w0`, `mov x20, x0`, a narrow type's the extension, `sxtb w19, w0`; a constant the immediate, `mov w20, #0` | 6.6 for a constant; chapter 4 |
| `x op= e`, `++x`, `x++` | the load, the operation, the store, after `e` for `x op= e`; for a return's value the place generated with `x0` left for last, the operation writing `x0`, `add x0, x3, x0`, and the postfix form's load, `ldr w0, [x1, w2, sxtw #2]; add w3, w0, #1` (6.1); a register variable in place, after `e`, its register `L` and the destination, `add w19, w19, w0`, `add w19, w19, #1`, a narrow one re-extended after, `sxtb w19, w19`, and `%=` with the quotient in a register of its own, `sdiv w0, w19, w20; msub w19, w0, w20, w19`; `x++` on a register variable for its value is G13's copy and the step, `mov w0, w19; add w19, w19, #1`; a pointer's `++` and `--` step by the size, an immediate where `add` encodes it, `add L, L, #4` for an `int *`, and otherwise held in a register | no memory form, 6.6; chapter 4 |
| `(T) a` | `sxtw L, L` for `int` to `long`; `sxtb`, `sxth`, `uxtb`, `uxth` from a narrow type; each only when `L` is not already valid to the new width; a signed object in memory under a cast that widens it is the load of 2.2 extended to the new width, `ldrsw x0, [x29, #16]` for an `int` to `long`, `ldrsb x0` and `ldrsh x0` for a `signed char` and a `short`, its displacement scaled by the object's size as the plain load's is, so through `x16` where that one would be (5.3), and no instruction after it, as `backend.md` 6.3's widening by the load has it, the object read under casts that keep its bits included, `ldrsw x0, [x29, #16]` for `(long) (int) u`, the plain `ldr w0` for `(unsigned long) (unsigned) i` and the `ldrb w0` for `(long) (char) c`, valid to 64, and over the promotion of a `char` or a `short` the one load extended to the outer width, `ldrsb x0, [x29, #16]` for `(long) (int) c` on a `signed char` and the `ldrb w0` or `ldrh w0`, valid to 64, on an unsigned one, a signed object under an unsigned widening the one load into the `w` register, `ldrsb w0, [x29, #16]` for `(long) (unsigned) c`, whose write of the `w` register clears the upper half, the `unsigned`'s zero extension, but a call's result keeping its `mov w0, w0`, `sxtb w0, w0; mov w0, w0` for a `signed char` one and `mov w0, w0` for an `int` one, whose upper half the convention leaves undefined, and a narrowing cast under the widening keeps its load and the extension after it, `ldr x0; sxtw x0, w0` for `(long) (int) l`; `mov wL, wL` for an `unsigned` not valid to 64; nothing between pointer types and for a pointer to `long`; an integer to a pointer as to `unsigned long`, so `sxtw` for an `int` | G2; the width rule |
| `&x` | `add L, x29, #d`; for a global the file defines `adrp L, sym; add L, L, :lo12:sym`, as a string literal's; for one it only declares, the load from the GOT; `&x` plus a constant is the same `add` or pair with the constant in it, the name and constant below | two `add`s beyond 4095, 5.3 |
| `*p` | the load of 2.2 from `[L]` | G8 to G11 |
| `*p++`, `*p--`, `*++p`, `*--p`, `*(p += c)`, `*(p -= c)` on a `register` pointer | the access by a form of 2.3 that steps the variable's register, the borrowed read the base: the post-indexed form for `p++` and `p--`, `ldrb w0, [x19], #1` and `ldrb w0, [x19], #-1`, and the pre-indexed one for the others, `ldrb w0, [x19, #1]!`, `ldr w0, [x19, #16]!` for `*(p += 4)` on an `int *`; a store the same, `strb w0, [x19], #1`, so `*p++ = *q++` is `ldrb w0, [x20], #1; strb w0, [x19], #1`; a compound assignment or a `++` through the pointer, two accesses through one address, steps once, the post form at the store, `ldrb w0, [x19]; add w0, w0, #5; strb w0, [x19], #1`, the pre form at the load; a read of the pointer still held is copied before the access that steps, `mov x0, x19`. The step is its own instructions and the access goes through the old or the new value, as the rows of `x++` and `*p` have it, in three cases: a store whose right operand names the pointer, since G13 completes the step before that operand, `*p++ = *p` being `mov x0, x19; add x19, x19, #1; ldrb w1, [x19]; strb w1, [x0]`; a step out of the form's reach, `*(p += 64)` on an `int *` being `add x19, x19, #256; ldr w0, [x19]`; and a pointer in a slot, whose step is a load, an `add` and a store | `autoincrement.md`; call 9 |
| `p[i]`, `a[i]` | the load from `[L, R, lsl #k]`, `[L, R]` for a size of 1; from `[L, wR, sxtw #k]` or `[L, wR, uxtw #k]` for an index held at 32 bits; a constant index is `[L, #c*s]` when the displacement encodes, a negative one within 256 bytes the unscaled displacement, `[L, #-4]`; a constant index the access does not encode (2.3) is an index like any other, held in a register by the constant row of 6.4 and read `[L, R, lsl #k]`; on an array reached by its name a constant index is the name and constant below, the slot's own access, `ldr w0, [x29, #28]`, or the row of a global scalar the file defines, `adrp x16, table+12; ldr w0, [x16, :lo12:table+12]`; through a cast of a sum the sum's bytes, a constant's at any stride, `ldr w0, [x0, #1]` for `*(int *) (c + 1)` on a `char *c` and `ldrb w0, [x0, #2]` for `p[1].x` at offset 0 of a record of 2 bytes, an index's at a stride of 1 or the access's size, `mov x1, #4097; ldr w0, [x0, x1]` for `*(int *) (c + 4097)`; an element of a row by a variable index the access does not scale, the index's product added to the row's base and the row's constant the access's displacement past the sum, `ldr w0, [x0, #36]` for `p[1][i][0]` on `int (*p)[3][3]`, or with no constant the product the register offset, `ldr w0, [x0, x1]` for `p[i][0]` on `int (*p)[3]` (the lvalue and constant below) | one instruction; the index's extension is the form's, below (call 5) |
| `&p[i]`, `&*p` | the address of `*E` as a value, and no access made (6.5.4.2p3): a bare base is its own value, so `&*p` is `p`, and so is `&p[0]`, a subscript by a constant 0, whose access is `[L]` as `*p`'s is, no instruction (call 8); any other subscript's address is the row of `p + n`: `&p[3]` is `add L, L, #12` for an `int *`, and `&p[i]` is `add L, L, R, lsl #k`, or `add L, L, wR, sxtw #k` for an index held at 32 bits; a row of an array of arrays decays the same way, `m[0]` to `m`; on an array reached by its name, the name and constant below, the slot's `add L, x29, #d+12` or the label's pair, `adrp L, table+12; add L, L, :lo12:table+12` | the same address by `add`, the machine having no `lea` |
| `p + n`, `p - n` | `add L, L, R` or `sub` for a size of 1; for a power of two the index shifted by its log, `k`: `add L, L, R, lsl #k` or `sub` up to 16, the index held at 32 bits read by the extended operand as for `&p[i]`, `add x0, x0, w1, sxtw #4` for `p + i` on records of 16 bytes, and above 16, which the extended operand does not shift by, its 4 at most, `R` the index whole, extended by its load or by `sxtw`, `ldrsw x1, [x29, #24]; add x0, x0, x1, lsl #5` on records of 32 bytes; `mov N, #s; mul R, R, N` first for any other size; a constant `n` is the immediate `#c*s` when it encodes, and otherwise the product held in a register by 6.4, `add L, L, R`; on a name's address a constant `n` is the name and constant below, the address formed once, `add x0, x29, #28` for `a + 3`; for `n + p` the result is in `L`, the integer's register: a plain register keeps the source's order, `add L, L, R`, and the shifted or extended operand, which the instruction takes last only, is `add L, R, wL, sxtw #2` | G6 |
| `p - q` | `sub L, L, R`; for a size other than 1, `mov R, #s; sdiv L, L, R` | G6: a division |
| `"abc"` | `adrp L, .Ls<n>; add L, L, :lo12:.Ls<n>`; an element by a constant, the name and constant below, `adrp x16, .Ls0+1; ldrb w0, [x16, :lo12:.Ls0+1]` | two instructions, chapter 10 |
| `f`, `&f` as a value | `adrp L, f; add L, L, :lo12:f` for a function the file defines; for one it only declares, the load from the GOT, `adrp L, :got:f; ldr L, [L, :got_lo12:f]` | two instructions either way, chapter 10; a call by the name forms no address, chapter 8 |
| `f(a, b)` | chapter 8 | |

**A name and a constant.** The rule of `backend.md` 6.3 holds as
written: an object reached by its name plus an integer constant, 0 for
`*a` and `*&x`, is one operand, the constant folded into it, and no
instruction forms the object's address first. With `int a[8]` at
`[x29, #16]` and `int table[8]` a global the file defines:

- `a[3]` is the slot's own access, `ldr w0, [x29, #28]`, reached
  through `x16` beyond reach by 5.3; `&a[3]` and `a + 3` are `add x0,
  x29, #28`, two `add`s beyond 4095;
- `table[3]` is the row of a global scalar the file defines, `adrp
  x16, table+12; ldr w0, [x16, :lo12:table+12]`, and `&table[3]` and
  `table + 3` the label's pair, `adrp x0, table+12; add x0, x0,
  :lo12:table+12`.

The `adrp` and the `:lo12:` carry the same addend, so the page is the
element's: `adrp x16, table` with `:lo12:table+12` reads the wrong
page whenever the array's start and the element lie on two. GNU as
and clang take the addend in both, the relocation is against the
symbol plus it, and a `static char big[8192]` read at 4095, 4096 and
8191 runs right (the globals probe). The `:lo12:` form of a load is
scaled by the access's size, and the element's offset is a multiple
of it, as the object's address is (chapter 10). A slot's displacement
plus the constant is reached when it is from 0 to 16 MiB less 1, the
reach of 5.3's `add` into `x16`. A label's offset folds from 0 to the
object's size, one past the end included, as on x86-64: the linker
resolves the label and the offset, and one outside the object it may
not reach, where the rows of the table always link.

A name's address plus a constant under a cast to another pointer
type, `*(int *) ((char *) &x + 4)`, the shape of a structure's member,
is the same operand, but the access is then no element's, and its
size need not divide the offset. A slot's operand is taken where 5.3's
forms encode it: the scaled one for a multiple of the access's size,
the unscaled 9 bits for any other, `ldr w0, [x29, #25]` for `+ 1` on a
`long x[2]` at `[x29, #24]`, also past `x16`'s add of the upper bits;
an offset no multiple of the size and out of the 9 bits keeps the
address formed, `add x0, x29, #281; ldr w0, [x0]`, since `ldr w0,
[x29, #281]` does not assemble. A label's scaled `:lo12:` form needs
the label plus the offset a multiple of the access's size at link
time, so it is taken only where the object is aligned to the size and
the offset a multiple of it: `*(short *) ((char *) words + 6)` on an
`int words[8]` is `adrp x16, words+6; ldrsh w0, [x16,
:lo12:words+6]`, but `*(int *) ((char *) words + 6)`, and `*(int *)
(bytes + 4)` on a `char bytes[16]`, which aligns to 1, keep the page's
`adrp` and `add` before `ldr w0, [x0]`: the linker rejects the other,
"relocation truncated to fit: R_AARCH64_LDST32_ABS_LO12_NC", where the
label lies at no multiple of 4, and takes it where it lies there by
chance. Anything else, a
negative displacement off `x29` and `table[-1]` among them, takes the
rows of the table: `adrp x0, table; add x0, x0, :lo12:table; ldr w0,
[x0, #-4]`. No form of 2.3 has a base, an index and a displacement at
once, so a local array's element by a variable index forms the
array's address first, `add x0, x29, #16`, where x86-64 makes the
slot the displacement of the indexed operand (`backend.md` 6.3, a
slot and an index).

Any other lvalue's address plus a constant is the rule's where that
address is a base and a displacement: the base is generated once and
the constants are one displacement, where the access encodes it
(2.3). `p[1][2]` on `int (*p)[3]` is `ldr w0, [x0, #20]` after `p`'s
load, with no `add` of the row's 12 bytes first; `other[1][2]` on an
`extern int other[][3]` is the GOT's load and `ldr w0, [x0, #20]`. An
address with an index has no form with a displacement, so `m[i][2]`
on `char (*m)[8]` forms the row's address, `add x0, x0, w1, sxtw #3`,
and reads `ldrb w0, [x0, #2]`; a sum the access does not encode takes
the rows of the table too. The row's own constant goes past the sum:
the index's product is added to the pointer's base and the constant
is the access's displacement with the column's, so `p[1][i][0]` on
`int (*p)[3][3]` is `mov x2, #12; mul x1, x1, x2; add x0, x0, x1;
ldr w0, [x0, #36]` after `p`'s load, with no `add` of the 36, and
`m[i][4]` on a local `char m[4][5]` at `[x29, #20]` adds the product
to the frame pointer, `add x0, x29, x0`, and reads `ldrb w0, [x0,
#24]`. A label's constant is its offset in the page: `rows[i][2]` on a
global the file defines is `adrp x0, rows+2`, the add of the product
and `ldrb w0, [x0, :lo12:rows+2]`, with no add of `:lo12:` before the
index. The constants under the row's index, through its casts, join
its own: `((int (*)[3]) (c + 4))[i][2]` on a `char *c` is `mov x2,
#12; mul x1, x1, x2; add x0, x0, x1; ldr w0, [x0, #12]` after `c`'s
load, and `(p + 1)[i][2]` on `int (*p)[3]` reads `[x0, #20]`, with no
`add` of the 4 or the 12 first. A row by a constant only takes them
the same way: a member array's element, `p->a[3]` on a member `int
a[8]` at 32, the decay of `*(int (*)[8]) ((char *) p + 32)` plus 12,
is `ldr w0, [x0, #44]` after `p`'s load, the array's 32 and the
element's 12 one displacement, and `p->t[2].h` reads `[x0, #98]`.
Constants above the index join it as well, a member array's element
under the record's index, `p[i].a[3]` on records of 168 bytes, being
the product's add and `ldr w0, [x0, #44]`; and on a label the total is
the page's offset, the member's byte sum over the index included,
`tab[i].m` at 8 being `adrp x0, tab+8`, the product's add and `ldr w0,
[x0, :lo12:tab+8]`, `tab[i].a[3]` the same at `tab+44`. A label keeps
its pair where the total is no multiple of the access's
size or its object is aligned to less,
`((int (*)[3]) ((char *) cells + 2))[i][1]` being `adrp x0, cells+2;
add x0, x0, :lo12:cells+2`, the product's add and `ldr w0, [x0, #4]`.
With no constant at all, a product of a size that is no power of two
is the access's register offset, `p[i][0]` on `int (*p)[3]` being
`ldr w0, [x0, x1]` after the `mul`, but not an index the add shifts,
since the access's form shifts by its own size alone: `add x0, x0, w1,
sxtw #4; ldr x0, [x0]` for `p[i].a` on records of 16 bytes and `add
x0, x0, x1, lsl #5` before it on records of 32; and so are constants
that cancel the column,
`((int (*)[1]) (c + 4))[i][-1]` being `ldr w0, [x0, w1, sxtw #2]`,
where nothing is evaluated between the address and its access; one
form holds a register offset or a displacement, never
both, so `p[i][1]` adds the product and reads `[x0, #4]`, and an
address held across an assignment's right operand keeps to one
register, `p[i][0] = f()` adding the product before the call. Across a
call the base and the index are that one register, and the constants
under the place stay the access's displacement, as a spill costs a
register and a constant nothing: `p->a[i] += f()` on a member `int
a[8]` at 32 is `add x0, x0, x1, lsl #2` before the call and `ldr w1,
[x2, #32]` and `str w1, [x2, #32]` after it, and on a local array the
slot's displacement rides the same way, `add x0, x29, w0, sxtw #2` and
`[x2, #32]`. A
constant out of reach after the sum, a label's offset outside its
object, and the address as a value keep the rows of the table. The
address as a value is the base and one
`add` of the displacement, where `add` encodes it (2.4): `&p[1][2]`
and `p[1] + 2` are `add x0, x0, #20` after `p`'s load, and a
displacement of 0, `p[1] - 3`, is the base and no instruction, as
`&*p` is `p`. A displacement `add` does not encode takes the rows of
`p + n`, but for an access with nothing evaluated between it and its
address, which then holds two registers at no cost, the displacement
is held and read as the register offset, unscaled, `mov x1,
#2147483648; ldrb w0, [x0, x1]` for `a[1][0]` on `char
(*a)[2147483648]`. An access through a cast of a sum of a stride of 1
or of the access's own size reads the sum's bytes as its offset the
same way: `*(int *) (c + 1)` on a `char *c` is `ldr w0, [x0, #1]`,
and `*(int *) (c + 4097)`, which no displacement reaches, `mov x1,
#4097; ldr w0, [x0, x1]`; the add stays where it encodes the constant,
`*(int *) (c + 257)` being `add x0, x0, #257; ldr w0, [x0]`, and where
the address is held across a right operand, `*(int *) (c + 4097) = v`.
A constant over the cast, or a subscript of it, joins the constants
under it: the pointer is read down through its casts and its sums by a
constant to a base, each constant scaled by its own sum's stride, and
their bytes with the access's own are its one displacement where it
reaches after the sum, so `((int *) (c + 1))[3]` is `ldr w0, [x0,
#13]` after `c`'s load and `((int *) ((char *) ((short *) (c + 1) + 2)
+ 2))[1]` `ldr w0, [x0, #11]`, with no `add`. A variable index on the
way is added to the base, in the order written, and the constants
ride past the sum, since one form holds an index or a displacement,
never both: `*(short *) ((char *) p + 4 + i * 4 + 2)` is the product's
`add x0, x0, w1, sxtw` and `ldrsh w0, [x0, #6]`; where the access's
own operand is the index, `((int *) (c + 1))[i]`, the constant keeps
its `add` before `[x0, w1, sxtw #2]`, the same count. A total out of
reach is held once and read as the register offset, unscaled, where
that is fewer instructions than the parts take at the least: one
constant's row, or one `add` for two or more that do not cancel, and
one instruction for the access's own part out of its reach. So
`((int *) (c + 1))[4097]` is `mov x1, #16389; ldr w0, [x0, x1]`
where the parts are `add x0, x0, #1; mov x1, #4097` before `[x0, x1,
lsl #2]`, and `((int *) (p + 2000000000))[400000000]` is `mov x1,
#41984; movk x1, #0xd693, lsl #16` before `[x0, x1]`, two where the
parts are five. The total can need more `movk` than its parts, so the
count decides: `((int *) (c + 1))[255]`, 1021 bytes, keeps `add x0,
x0, #1` and `[x0, #1020]`, the same count with no register more, and
`((int *) (c + 65536))[4]` keeps its one `add` where 65552 held is
two. Under a cast, or at a size no form scales by, the pointer's value
may be formed whole, the total one `add`, and that bounds the count
too: `*(int *) ((char *) p + 1052972 + -300)` keeps `add x0, x0,
#1052672` before `[x0]`, where the total held is two.
As a value the address is the same walk: two or more constants
are one `add` of their sum where its immediate encodes it,
`&((int *) (c + 1))[3]` being `add x0, x0, #13` and
`&((int *) (c + 4))[-1]`, whose constants sum to 0, `c` and no
instruction; a variable index on the way is added first, in the order
written, and the constants once after it, `add x0, x0, w1, sxtw; add
x0, x0, #6` for `(short *) ((char *) p + 4 + i * 4 + 2)`; into a
register pointer the base's row and the `add` write the variable, `ldr
x19, [x29, #24]; add x19, x19, #13`. A sum no `add` immediate encodes
is held and added, `mov x1, #32777; add x0, x0, x1` for
`&((long *) (c + 1))[4097]`, where that is fewer instructions than the
parts' rows, three there; `0x10000 + 16` keeps its parts, two `add`s,
where the sum held would be three. A name's address keeps its label's
pair with the part within the object, as above. The walk goes through
a row's layer too, the decay of `*q` or `&*q` being `q`: `&p[1][2]` on
`int (*p)[3]` is `add x0, x0, #20` after `p`'s load, and `&((int
(*)[3]) (c + 4))[i][2]` on a `char *c` adds the product to `c`, `mov
x2, #12; mul x1, x1, x2; add x0, x0, x1`, and then the one `add x0,
x0, #12`, the 4 under the row with the column's 8. A row on a slot or
a label under its index takes its constants, under the index and
above it, into the name's own address, then adds the product:
`&((int (*)[3]) ((char *) m + 4))[i][1]` on a local `int m[4][3]` at
`[x29, #20]` is `add x0, x29, #28` and the product's add, and on a
global `cells` the label's pair at `cells+8`. A total out of 5.3's
reach or outside the label's object keeps the rows of `p + n`. One
constant over any other row is the row of `p + n` or `p - n` after
the sum, as over any pointer, `sub x0, x0, #4` for `p[i] - 1`; a 0
there is an identity, and the row's address alone (6.4).

A store's address is generated before its right operand and held
across it (G13), but a constant index reads nothing, so its `mov` is
written after the right operand, at the access: `p[5000] = f()` on an
`int *p` spills `p` alone across the call, `bl f; mov x1, #5000; ldr
x2, [x29, #24]; str w0, [x2, x1, lsl #2]`, and `p[5000] = v` is `mov
x2, #5000` after `v`'s load. A compound assignment holds the constant
the same way, its load and store after the right operand both reading
it. A variable index is read before the right operand, as written;
where that operand holds a call that would spill both the base and the
index, the two are summed first and one register is spilled, `add x0,
x0, w1, sxtw #2` before the call and `str w0, [x1]` after it, but not
where either is a register variable's read or spilled already. With no
call the two registers are held, the form being one instruction fewer
than the sum.

The width rule holds as written. Its facts on this machine:

- a narrow load leaves the value valid to 32, and the load of a
  signed object that a cast widens to 64 bits, `ldrsw x0`, valid to
  64;
- a write to a `w` name clears the upper half, so an `unsigned`
  written at 32 is valid to 64 and an `int` to 32;
- a narrowing cast writes nothing;
- the value of a compound assignment or a step at the object's own
  width is the operation's write, so `return *p += e` on an `unsigned
  *p` widened to `long` ends in `str w0, [x1]` with nothing after it,
  where an operation in `long`, `*p += l`, keeps `mov w0, w0`;
- a call's result is valid to its own type's width and no more: 8 for
  a `char`, 16 for a `short`, since the convention leaves the rest
  unspecified (chapter 3). The cast that widens it writes the
  extension;
- a truth value, the 0 or 1 of a comparison, of `!` and of `&&` and
  `||`, is valid to 64: `cset` and the `mov w0, #1` and `mov w0, #0`
  of the jump code write a `w` name, and 0 and 1 extend alike by
  either signedness. So `(long) (a < b)` is `cset w0, lt` and nothing
  after it, and `register long l = a < b` is `cset w19, lt`. A 32-bit
  operation on it records 32 again, as on any value: `(long) ((a < b)
  + 1)` is `add w0, w0, #1; sxtw x0, w0`.

And a `return` of a `char` or a `short` writes no extension: the
convention asks none of the callee, and G16 adds nothing.

**The index.** The integer operand of a subscript or of pointer
arithmetic is 64 bits by the parser's cast, on both targets. On x86-64
the cast's instruction is written, `movslq` for an `int`, the index's
own load when it is in memory, and the access follows. This machine's
addressing form takes a 32-bit index and extends it itself, so the
cast is looked through wherever it stands, the source's own `p[(long)
i]` included, since the tree does not tell the two apart. The cast's
operand is generated and what is held decides the operand:

- valid to 64 bits, a `long` or an `unsigned` written at 32: `[L, R,
  lsl #k]`, as the width rule writes nothing for the cast;
- valid to 32 bits and signed, an `int`: `[L, wR, sxtw #k]`;
- valid to 32 bits and unsigned, as an `unsigned` call result is:
  `[L, wR, uxtw #k]`;
- narrower and signed, as a `signed char` call result is: the cast's
  instruction extends it to 32 first, `sxtb w2, w2`, and the form
  does the rest;
- narrower and unsigned, as a plain `char` call result is on this
  target: the cast's instruction is `uxtb w2, w2`, which leaves it
  valid to 64 bits, so the form is `lsl`.

`&p[i]` and `p + n` form the same address by `add` or `sub` with the
same operand, `add x0, x1, w2, sxtw #2`. A size that is not 1, 2, 4
or 8 is multiplied in, and there the cast is written before the
`mul`, as anywhere else: the extending load, `ldrsw x1, [x29, #16]`,
for an `int` in memory, and `sxtw` for one in a register.

So `p[i]` for an `int i` is one instruction, and for a `register int
i` it names the variable's register and nothing is copied:

```
	ldr	w0, [x1, w20, sxtw #2]
```

On x86-64 a `long` index is the programmer's lever against the
`movslq` of an `int` index held in a register; an `int` in memory is
one `movslq`, its load, as a `long` is one `movq`. Here an `int`
index and a `long` one cost the same.

### 6.4 Constants

c0's folding is unchanged, and is the same on both targets: a
division with no value stays unfolded, and on this machine the
instruction left to compute it yields a number instead of a trap.
An identity's node is left out on both alike (`backend.md` 6.4), so
`x + 0` and `x * 1` are `x`'s load alone, and `p[i] + 0` over a row
is the row's address with no `add x0, x0, #0`.

A constant operand is an immediate when the instruction, as the
source writes it, has an encoding for the value:

- `x + 1` is `add w0, w0, #1`; `x + 5000` is `mov w1, #5000; add w0,
  w0, w1`.
- `x & 255` is `and w0, w0, #0xff`; `x & 5` is `mov w1, #5; and w0,
  w0, w1`.
- `x * 4` is `mov w1, #4; mul w0, w0, w1`, and `x / 4` is `mov w1,
  #4; sdiv w0, w0, w1`. The multiply stays a multiply and the
  division a division, as on x86; each costs the `mov` its
  instruction has no immediate for.
- A negative constant keeps its operator. `x + -1` is written `add
  w0, w0, #-1` and `x == -1` is `cmp w0, #-1`. The assembler encodes
  the first as `sub w0, w0, #1` and the second as `cmn w0, #1`, the
  same result and the same flags: the machine has no negative
  immediate, and these are its spellings of the two operations. c1
  writes the operator the source has.
- A constant that must be held is one `mov` when the instruction
  encodes it (2.4): 16 bits at one position hold it or its
  complement, `mov w0, #5000`, or it is a bitmask, `mov x0,
  #0xffff0000ffff0000`. Any other 32-bit value is `mov` and one
  `movk`. Any other 64-bit one starts from zeros or from ones,
  whichever leaves fewer of its four 16-bit parts to write, zeros on
  a tie, and takes one `movk` for each part left, four instructions
  at most: `-100000L` is `mov x0, #-34464; movk x0, #0xfffe, lsl
  #16`. A `mov` is written with the value it leaves in the register,
  in hex for a bitmask, and otherwise in decimal, read as a signed
  number of the register's width whatever the constant's type: the
  `unsigned` 0xffffec78 is `mov w1, #-5000`. A `movk` is written with
  its 16 bits in hex.

  Counting the 16-bit parts that differ from zeros, or from ones,
  gives the most a constant costs, and that count is made by eye. The
  bitmask only lowers it: `0x80000001` is one `mov` and `0x80000002`
  two, which the eye does not tell apart.

The count is a fact of the encoding, and the cost model gets a row
for it: a constant costs nothing, one instruction, or two, by its
bits.

### 6.5 The pinned instructions

There are none. `divide()` and `shift()` write their instruction on
the registers the operands are in. `pins()`, `pinned_by()`,
`vacate()`, `move()` and `pinned()` exist in the x86-64 generator
only. The state's `reserved` mask is here for one use: the place of an
assignment or of a step whose value a `return` names is generated
with `x0` left for last (6.1).

### 6.6 The effect context

The three contexts stay, and `effect()` still reaches through a cast
to void, a conditional's arms and a comma, whose left operand is in
this context wherever the comma stands. What it finds there is less,
because the machine has less:

- **No memory forms.** `x++;` on a slot is `ldr w0, [x29, #40]; add
  w0, w0, #1; str w0, [x29, #40]`. `x += e;` is `e`, then the load,
  the operation and the store. `backend.md` took the memory forms
  because "x86 has the same forms" as the PDP-11; this machine does
  not, and three instructions are what the statement costs on it. A
  post form is its prefix form in this context, in one register.
- **A constant into memory goes through a register.** `str` takes no
  immediate. `x = 5;` is `mov w0, #5; str w0, [x29, #40]`. The one
  store row of `initialize()` stays one function, serving a
  declaration's initializer, an argument's slot and the effect
  context's assignment, and its body is these two instructions.
- **Zero** is the exception the machine offers: `str wzr, [x29, #40]`
  stores zero in one instruction, since the zero register is an
  operand like any other (call 4). Through a stepped `register`
  pointer it is the same store with the form of 6.3: `*p++ = 0;` is
  `strb wzr, [x19], #1`.

A register variable is operated on in its register, as before.

**A record's assignment is a copy.** `a = b` on two structs or unions
of one type stands only where its value is discarded, and is the right
operand's object copied into the left's in units of the type's
alignment, 1, 2, 4 or 8 bytes, which tile the object, padding
included, lowest address first. Each unit is the plain load of its
width into `x17`, `ldrb`, `ldrh` or `ldr`, the store's mirror, and its
store, before the next unit is loaded; a unit is bits and no value, so
no load extends it. Nothing is called. The left's place is evaluated
first and the right's after it (G13), the left's held across the
right's as a store's address is across its right operand: one
register, stored to its spill slot once where the right holds a call,
`bl f; str x0, [x29, #16]; bl g` and the reload before the first
store, and a constant index written after it.

A place keeps its own form where every unit reaches (2.3), each unit's
offset joining the displacement: a slot, `ldrh w17, [x0]; strh w17,
[x29, #28]` and the same at `#2` and `#30`; a base and a displacement,
`str w17, [x0, #8]` for a member; a row's page, whose `:lo12:` carries
the offsets. A label of one or two units is its page and its access
through `x16` for each, `adrp x16, corner; ldr w17, [x16,
:lo12:corner]`; beyond two its address is formed once, `adrp x0,
first; add x0, x0, :lo12:first`, two instructions and then one a unit.
A slot or a base whose units leave the reach is its address formed
once, and an index is added to its base once, `add x0, x0, w1, sxtw
#2`, each unit then a displacement off the sum; an element of a size
the add's shift does not take, more than 16 bytes or no power of two,
is the row of `p + n` with its `mul`. A register pointer's step is its
own `add`, `add x19, x19, #4`, after the copy for `p++` and before it
for `++p`, since no one of the accesses may make it. `a = a` is the
copy onto itself, written out.

More than eight units are a loop of four instructions whatever the
size: the two addresses in registers of the copy's own and the count
of units in `x16`, `mov x16, #9`, then `ldr x17, [x1], #8; str x17,
[x0], #8; subs x16, x16, #1; b.ne`, each post-indexed access stepping
its address. An index from 0 holds the same registers and is a fifth
instruction a unit, its `cmp`.

**A local record a list initializes is zeroed first.** The
declaration zeroes the whole slot, padding included, before the first
listed value is evaluated, and the statements the parser writes after
it store the listed values, each an assignment of this context (G20).
The units and the loop are the copy's, by the one rule of
`backend.md` 6.6. Each unit is the store of the zero register at its
width, `str wzr, [x29, #16]; str wzr, [x29, #20]` for an 8-byte
record of alignment 4, and takes no other register; a slot whose
units leave the reach is its address formed once, as the copy's is,
each unit a displacement off it. More than eight units are the copy's
loop with one address, the slot's address formed first and the count
after it, as the copy orders them, then three instructions whatever
the size: `add x0, x29, #16; mov x16, #25`, then `str xzr, [x0], #8;
subs x16, x16, #1; b.ne`.

## 7. Conditions and control

### 7.1 Jump code

`cbranch(e, label, sense)` as in `backend.md` 7.1:

- A comparison: `cmp L, R` and `b.cc label`, the condition or its
  inverse by `sense`; a constant on the left first taken as `R`, the
  comparison mirrored (6.1), so `if (5 < x)` is `cmp w0, #5` and
  `b.le`.
- `!e`, `&&`, `||`: the same shapes.
- `a, b`: `effect(a)`, then `cbranch(b, label, sense)`.
- Any other expression: its value into `L`, then the test of zero,
  `cbnz L, label` or `cbz L, label`, one instruction (call 3). This
  covers the conditions that x86-64 jumps on the flags of their
  operator's instruction (`backend.md` 7.1): `cbz` and `cbnz` test a
  register and read no flag, so this target has nothing to take.

x86 tests a narrow operand at its own width, `cmpb $0, %al`. This
machine compares whole registers only. A value valid to 32 bits is
tested at 32, which a loaded `char` always is. One with stale upper
bits, the operand of a narrowing cast, is tested by `tst w0, #0xff`
and `b.ne`: the low bits and nothing else. `!` as a value has the
same test, with `cset` after it.

`if (a != 0)` and `if (a)` thus differ in the listing, as they differ
in the source: the first is the comparison row, `cmp w0, #0; b.eq`,
and the second the test.

### 7.2 Statements

As `backend.md` 7.2, with `b` for `jmp`. A `?:` path that writes
nothing has no `b` over it: the condition's branch of 7.1 goes to the
join, inverted where the first path is the empty one, `cbnz` for
`cbz`, `b.lt` for `b.ge` and `b.ne` for the `tst`'s `b.eq`. `return
e` leaves `e` in
`x0`: the return names it, and the last instruction of `e`'s row
writes it where no other value holds it, a conditional at the top of
`e` joining its arms there (6.1). A `mov` puts it there when it is
held in another register: `mov w0, w19` for a register variable at 32
bits, and on the `x` names for a `long` or a pointer. Every `return`
jumps to the one epilogue. `main` returns 0 on reaching its `}`, `mov
w0, #0`, by the rule of `backend.md` 7.2.

`do`'s condition at the bottom branches back to the body's label on
the condition, `b.lt .Lcount.2` or `cbnz`, by jump code (7.1). `break`,
`continue` and `goto` are each one `b`, to the label `backend.md` 7.2
names, and a label of the source is one more label of the function.
An `if` whose body is one of them, or a `return` with no value, is the
condition's branch to its target, by the rule of `backend.md` 7.2:
`if (n & 1) break;` is `and w0, w0, #0x1; cbnz w0, .Llow.3`. A
`switch` is its dispatch, 7.3, then its body, as on x86-64.

**No alignment directive before a loop head**: measured, not
adopted. `.p2align 5` on x86-64 was measured on one processor and
adopted as "a convention of the target" (`backend.md` 13, call 8). On
this target `.p2align 4`, `5` and `6` were measured on two cores, a
Qualcomm Oryon and the Firestorm core of an Apple M1 Max. The best,
`.p2align 5`, nets -0.5% on the Oryon and -0.3% on Firestorm over the
tuned sources, and `clang -O2` aligns no loop head here (`cost.md`
4.2, call 7).

### 7.3 The switch

The rule of `backend.md` 7.3, the Seventh Edition compiler's, chooses
the form from the cases alone, in the code both generators share, so a
switch takes the same form on both targets; the instructions are this
machine's rows.

**Direct.** `day()` of `switch-direct`, cases 1, 2, 3 and 5:

```
	ldr	w0, [x29, #16]
	sub	w0, w0, #1
	cmp	w0, #4
	b.hi	.Lday.6
	adrp	x16, .Lday.8
	add	x16, x16, :lo12:.Lday.8
	ldr	x16, [x16, w0, uxtw #3]
	br	x16
```

The `sub` and the `cmp` take their constant as an immediate where the
instruction encodes it (2.4), and hold it by the constant row of 6.4
where it does not: the least `int` as the first value is `mov w1,
#-2147483648; sub w0, w0, w1`. `b.hi` is the unsigned jump above
`range`. The table's address is the `adrp` and `add` pair a literal's
is, into `x16`, which holds nothing across a node (chapter 4). The
load reads the entry by the form of 2.3 that scales the index and
extends a 32-bit one unsigned, `uxtw #3`, so no instruction extends it
first, and a 64-bit one whole, `[x16, x0, lsl #3]`; `br x16` jumps. A
register variable is read where it is only read, `cmp w19, #3` and
`ldr x16, [x16, w19, uxtw #3]`, and the `sub` writes a fresh register
(6.1). The table is the text of `backend.md` 7.3.

**Simple.** `kind()` of `switch-simple`, cases written 10, 0, -1 and
100:

```
	cmp	w0, #-1
	b.eq	.Lkind.4
	cbz	w0, .Lkind.3
	cmp	w0, #10
	b.eq	.Lkind.2
	cmp	w0, #100
	b.eq	.Lkind.5
	b	.Lkind.6
```

A case of 0 takes the test of zero, `cbz`, one instruction: the
Seventh Edition compiler tested a 0 with `tst` where it compared any
other value, and `cbz` is this machine's test (7.1, call 3). Any other
case is the comparison row, `cmp` and `b.eq`, its constant held first
by the constant row where `cmp` does not encode it.

**Hash.** `reason()` of `switch-hash`, twelve cases, a table of 5:

```
	mov	w1, #5
	udiv	w2, w0, w1
	msub	w0, w2, w1, w0
	adrp	x16, .Lreason.16
	add	x16, x16, :lo12:.Lreason.16
	ldr	x16, [x16, w0, uxtw #3]
	br	x16
```

The size is held, as the divisor of `x / 7` is, since `udiv` takes no
immediate. `udiv` writes the quotient into a register taken for it,
and `msub` the remainder into the value's, as the remainder of `%` is
(6.3); a register variable's remainder goes to a fresh register. The
remainder indexes the table, and the buckets compare the quotient,
`cmp w2, #40` and `b.eq`, or `cbz w2` for a quotient of 0. No register
is pinned (6.5), so nothing moves.

## 8. Calls

Every argument is a hidden local of its call, as before.

1. Every held value is spilled.
2. The callee of a call through a pointer is evaluated first and
   written to a slot of 8 bytes of its own, taken before it is
   evaluated, by the store row of 6.6, `str x0, [x29, #32]`. Then each
   argument in turn is written to its slot by the store row of 6.6. A
   `va_list` argument, which the convention passes as the address of a
   copy the caller makes, a record of more than 16 bytes, takes a slot
   of 32 bytes when its turn comes and is copied into it by the copy
   of 6.6, four `ldr x17` and `str x17` units, so that an argument
   after it that reads or steps the list leaves the copy as it was.
3. The call sequence. What the arguments left held in an expression
   register is spilled first (`backend.md` 6.7). With more than
   eight arguments, `sub sp, sp, #A` opens the outgoing area, `A`
   being 8 bytes for each argument past the eighth, rounded up to
   16; an area of 4096 bytes or more is opened in two parts, as the
   frame is (5.2), `sub sp, sp, #4096; sub sp, sp, #16`. Each of
   those is copied from its slot at its own width, `ldr w17, [x29,
   #44]; str w17, [sp]`, the ninth at `[sp]`, the tenth at `[sp,
   #8]`. A `char` is `ldrb` and `strb`, and the seven bytes above it
   in the area stay as they were: they are padding to the convention
   (chapter 3). An argument past its store's reach (2.3), 4096 bytes
   above `sp` for a `strb`, 8192 for a `strh`, 16384 for a `str` of a
   `w` register and 32768 of an `x`, is stored through `x16` formed
   from `sp`, as a far slot's address is from `x29` (5.3): `add x16,
   sp, #4096; strb w17, [x16]`. The first eight are loaded from their
   slots into `x0` to `x7`, a narrow one extended by its load. A
   list's argument is its copy's address, `add x1, x29, #272` into its
   register, or past the eighth `add x17, x29, #272; str x17, [sp]`.
   `bl f` for a named function. Through a pointer, the callee's slot is
   loaded into `x16` after the argument registers, `ldr x16, [x29,
   #32]`, and `blr x16`. `x16` is the register the convention lets a
   call destroy, and nothing is held in it across a node (chapter 4).
   A callee's slot beyond the load's reach forms its address in `x16`
   first, as any far slot does (5.3), and `x16` is loaded through it,
   `add x16, x29, #36864; ldr x16, [x16, #3192]` beside a local of
   40000 bytes. `add sp, sp, #A`, in the parts that opened it.
4. The result is in `x0`, valid to its type's width (6.3).

A call through a pointer is made with the types of the pointer's
pointee (G16). `p(x)`, `(*p)(x)` and `(**p)(x)` are one listing, and
`(*f)(x)` and `(&f)(x)` on a function's name form the address and call
through it, as `backend.md` chapter 8 has it.

An area of 16 MiB or more, past the two parts' reach, is a not-yet:
a call takes 2097150 arguments past the eighth. No unwind directive
is written around the area, since the canonical frame address is
computed from `x29` there.

The outgoing area is opened and closed around the call, as x86's
pushes are added back after theirs. The alternative, an area at the
bottom of the frame sized by the function's largest call, would put
the frame record above it at an offset unknown until the body is
done, for the sake of a call with nine arguments.

## 9. What each guarantee compiles to

| Guarantee | Instruction |
|---|---|
| G1 signed arithmetic wraps | `add`, `sub`, `mul`, `lsl`, `neg`: modulo 2^N |
| G2 narrowing truncates | `strb`, `strh`, `str w`, or the `w` name; nothing is computed |
| G3 shifts | `lsl`, `asr`, `lsr`: the instruction masks the count to the width |
| G4 division is the instruction's | `sdiv`, `udiv`: 0 for a zero divisor, `INT_MIN` for `INT_MIN / -1`, and no trap |
| G5 an uninitialized object holds a value | a slot's bytes or a register's content, unwritten until the program writes, `cmp w19, w19`; but a narrow or an `unsigned` register variable is extended in its own register, `sxtb w19, w19`, which leaves the value of its type as it was (chapter 4) |
| G6 pointer arithmetic is address arithmetic | `add`, `sub` on `x` registers, `mul` by the size; `p - q` is `sub`, `sdiv` |
| G7 pointer comparison is address comparison | `cmp` of `x` registers, the unsigned conditions |
| G8 a load reads the bytes at the address | the load of 2.2, whatever computed the address |
| G9 null access at address 0 | the same load; the machine faults |
| G10 memory is bytes | the same load, at the lvalue's width and extension |
| G11 misaligned access performed | the same load; the machine performs it, as the corpus program shows, 52 on both targets |
| G12 one slot for the activation | chapter 5 |
| G13 left to right | chapter 6's rule and chapter 8's order; for a register variable, chapter 4's copy of a held read, `mov w0, w19` before `add w19, w19, #1`, and its save before a branch, `str x19, [x29, #32]` |
| G14 a loop runs until its condition says stop | a test and a jump |
| G16 a call goes out as written | chapter 8: the convention's argument moves with the call site's types, then `bl f`, or `ldr x16` from the callee's slot and `blr x16` through a pointer |
| G17 a string literal is writable | `.data`, chapter 10 |
| G18 a struct assignment copies its object | each unit `ldrb`, `ldrh` or `ldr` into `x17` and its store, lowest address first, unrolled to eight units and a loop of four instructions beyond, 6.6; no `bl` |
| G19 variable arguments are read where the convention put them | the area of 5.2 at entry, `stp x0, x1` to `stp q6, q7` and the stacked arguments' address; `va_arg` the test of `__gr_offs` and the load at the slot; a list argument the address of a 32-byte copy, chapter 8 |
| G20 a local's initializer list writes every byte of its object | the slot zeroed first, each unit of the copy the store of `wzr` or `xzr`, `str wzr, [x29, #16]`, lowest address first, unrolled to eight units and a loop of three instructions beyond, `str xzr, [x0], #8; subs x16, x16, #1; b.ne`, 6.6; then the listed values' stores, in the list's order |

G4 is the row to read twice. `guarantees.md` already says "x86 `idiv`
faults on a zero divisor and on `INT_MIN / -1`; aarch64 `sdiv` yields
0 and wraps". A program that relies on the trap is relying on x86,
and rcc does not write a check to give this machine one: that would
be a decision of the compiler's own.

The misalignment the machine does refuse is `sp`'s, which is the
generator's to keep and never the program's.

## 10. Statics and literals

As `backend.md` chapter 10: one `.text`, one `.data` block of literals
after the last function, `.Ls<n>` and `.string`, never merged, never
read-only, and a wide literal aligned to its element, under `.long` or
`.short`, which an element's read by a constant index, `ldr w0, [x16,
:lo12:.Ls2+4]`, needs for its scaled offset. A literal's address is
the `adrp` and `add` pair. A function is `.type f, @function` and
`.size`, and `.globl` first for one of external linkage; a `static`
function has none, and a call of it is `bl f`, as of any function. No
alignment directive is needed before a function: every instruction is
4 bytes, and the assembler aligns `.text` to 4. The listing opens and
closes as on x86-64: `.file` with the source's name and `.text` first,
`.section .note.GNU-stack,"",@progbits` last. An alias is written as
on x86-64, after the data, `.globl a`, `.type a, @function` for a
function and `a = t`.

A function's address as a value has the two rows of `backend.md`
chapter 10. A function the file defines, wherever its body stands,
is the `adrp` and `add` pair, `adrp x0, f; add x0, x0, :lo12:f`. A
function the file only declares is loaded from the GOT, `adrp x0,
:got:f; ldr x0, [x0, :got_lo12:f]`: GNU ld refuses the PC-relative
pair against a libc function in a position-independent executable,
`R_AARCH64_ADR_PREL_PG_HI21 ... which may bind externally`, and the
GOT's entry is the address the library itself takes of the function.

An object follows the same rule. One the file defines, by a definition
anywhere in the file, is reached by the far slot's mechanism with the
symbol's page (5.3): `adrp x16, counter`, then the access through
`[x16, :lo12:counter]`, two instructions an access, and `x16` holds
nothing across them. A `static` object and a block's are such
objects, a block's under its label, `adrp x16, next.count` and
`[x16, :lo12:next.count]`. Its address as a value is the pair a literal's
is, `adrp x0, counter; add x0, x0, :lo12:counter`, and an array the
file defines is indexed by a variable after that pair, as a local
array is after its `add`; by a constant it is the element's own pair,
`adrp x16, table+12` and `[x16, :lo12:table+12]`, the name and
constant of 6.3. The `:lo12:` form of a load or a store is scaled by
the access's size, so the object's low 12 bits must be a multiple of
it: the object is aligned to its type at its definition, and an
element's offset is a multiple of the element's size. One the file only
declares has its address loaded from the GOT, as a declared function's
is, and each access is the indirection through that register. GNU ld
refuses the PC-relative pair against `stdout`, as against a libc
function.

The objects' data is the text of `backend.md` chapter 10, the same on
both targets: `.data` for those with an initializer, `.bss` for the
others, each under `.type`, `.size` and `.p2align`, and `.globl` for
one of external linkage alone, a block's static object under the
label of `backend.md` chapter 10, with
`.byte`, `.short`, `.long`, `.quad`, `.string`, `.ascii` or `.zero`,
which GNU as and clang take on this machine as on x86-64. A
brace-enclosed list is the same text too: each element it gives in
the directive of its form at its width, in subscript order, and a
`.zero` for each run of elements it leaves out, so `int m[2][3] = {
{ 1 }, 4, 5 };` is `.long 1`, `.zero 8`, `.long 4`, `.long 5` and
`.zero 4` on both targets. An address
constant is the same `.quad n`, `.quad a+12`, `.quad .Ls0+1`: the
assembler leaves an `R_AARCH64_ABS64`, which the linker of a
position-independent executable makes an `R_AARCH64_RELATIVE` for a
symbol the program defines and leaves against the symbol for one in a
shared library, `.quad stdout`, `.quad putchar`; the loader fills in
both. The one difference is the alignment of an array: the AAPCS64
has no rule like x86-64's 16 for an array of 16 bytes or more, so an
array takes its element's, and an array of `char` a string
initializes has none.

## 11. c0 and the driver

c0 learns the target, for five facts:

- **The register variables**, 5 or 10: the count c0 assigns from and
  the number in its warning.
- **Plain `char`**, signed on x86-64 and unsigned on aarch64.
  6.2.5p20: "The implementation shall define char to have the same
  range, representation, and behavior as either signed char or
  unsigned char." The platform defined it, and rcc follows the
  platform (call 2). A program that needs a signed byte writes
  `signed char`.
- **`wchar_t`**, `int` on x86-64 and `unsigned int` on aarch64, 32
  bits on both, as `clang` has it: the type of `L'x'` and the element
  of `L"..."`, 6.4.4.5p15 and 6.4.5p6. `char16_t` and `char32_t` are
  `unsigned short` and `unsigned int` on both.
- **The frame limit**, 2 GiB less 16 on x86-64 and 16 MiB less 16
  on aarch64 (5.2): the largest frame. c0 refuses a local over it at
  its declaration, where its position is known. The generator holds
  the same limit for the frame as a whole.
- **The variable argument list**, 7.16p4, the convention's, which the
  C library reads: on x86-64 an array of one 24-byte record of the
  offsets into the register save area and the addresses of the areas,
  so a parameter of it is a pointer; on aarch64 a 32-byte record of
  the stack area's address, the tops of the register areas and the
  offsets below them, which a call passes as the address of a copy.
  c0 makes the record and builds `va_start` and `va_arg` from it
  (`shape.md` chapter 5).

They live in one small description, `target_t`, that c0, the
generators and the driver read: the name, the name `clang` takes after
`--target=`, the five facts, and the alignment the convention asks of
a global array, which c1 reads (chapter 10). Everything else about a
target is in its generator.

The driver takes `-target x86-64` or `-target aarch64`, and the
default is the machine rcc was built on. With `-S` either target's
text can be written on either machine. With `-c` or a link the
driver hands `clang` the matching `--target`. The `clang` of either
machine assembles the other target's pins, so one machine checks
both sets of pins; only the oracle's runs and the probes need the
machine's own target (call 6).

## 12. The code

A generator's functions write its machine's mnemonics. A second
target is added in one of two ways:

- **One walk over a target interface.** The walk is shared and each
  instruction goes through a table or an object the target supplies.
  The two machines disagree on the operand count, on memory operands
  and on pinned registers, so the interface is either the union of
  both or the least of both. It is a second, smaller LLVM, and the
  first one was retired for the judgment it kept between the tree
  and the instruction.
- **Two generators.** `src/c1-x86-64.cc` and `src/c1-aarch64.cc`,
  each read whole against its own design page. `c1.hh`'s
  `generate()` takes the target and calls one.

The second is the answer (call 1), with one qualification: what is
the same text for both machines is written once.

- **What holds no instruction**: the integer view of a type, the
  tree predicates (`writes()`, `names()`, `is_comparison()`), the
  label names, the literals' `.string` block. It is
  `src/c1-common.cc`, compiled once.
- **The register stack**, `spill()`, `hold()`, `reg_of()` and the
  rest, is `src/c1-stack.hh`. It writes three instructions, the
  store to a spill slot, the reload from one and the `mov` of a
  register into another; each generator defines them before the
  include. `take()` stays in each generator, since x86's has the
  reservation, and so do x86's pins. The contract is one: the held
  values and the two save points of `backend.md` 6.7.
- **The statements**, the expression statement, the block, `if`,
  `while`, `for` and `return`, are `src/c1-statements.hh`. Each is a
  shape of labels and jumps around the generator's own functions,
  with no instruction of its own (7.2). Where the machines differ,
  each generator defines a constant: `loop_alignment`, 5 on x86-64
  and 0 here (7.2), and `result_register`, `rax` and `x0`. A
  `return`'s value reaches its register by the stack's `mov`. A
  declaration, `decl_stmt()` with the `register_decl()` it calls,
  stays in each generator, since the store row of its initializer
  takes each machine's own form of the slot's address.

The stack and the statements are generator text that names each
target's registers and state. Each generator includes the two inside
its own namespace, the statements after its expressions, and so
compiles them against its own registers and state.

## 13. Verification

- **The oracle.** `clang -O2` on this machine gives the corpus
  programs the statuses it gives them on x86-64, the second column
  of `tests/corpus/expected` unchanged. The file is shared until a
  row differs.
- **The pins** live under a directory per target,
  `tests/corpus/<target>/<name>.s` and `tests/cost/<target>/<name>.s`,
  and both targets have one in each corpus. Both scripts take
  `--target <target>`, by default the machine's own. The text diff,
  and the assembly by `rcc -target <target> -c`, run for any target
  on any machine; the oracle's programs run only on the machine's
  own target. A program that uses a construct whose slice is not in
  the tree, and has no pin for the target, shows "not yet" in its row
  and is neither passed nor failed. The headers of `tests/oracle`
  and `tests/costs` state the rule. No program of either corpus shows
  it on this target.
- **The cost corpus** reaches this target's rows. Three programs are
  aimed at x86's numbers:
  - `spill`'s ten nested operands exhaust nine registers, not
    fifteen. Its sibling `spill-fifteen` holds seventeen values, more
    than either target's registers.
  - `stack-args`'s seven and eight arguments are all in registers
    here. Its sibling `stack-args-nine` passes nine and ten.
  - `div-busy` has no pinned register to be busy here, and needs no
    sibling: its aarch64 pin shows the row, as those of `div-rem`
    and `udiv-urem` show the remainder.

  The rows this target asks for have programs of their own:
  - `far-slot`, a slot beyond reach (5.3);
  - `far-stack-parameter`, a stack parameter past the `add`'s reach
    (5.3);
  - `constant-bits`, a constant by its bits (6.4);
  - `stack-parameter`, the stack parameter's copy (5.2);
  - `large-frame`, a frame of 4096 bytes or more (5.2);
  - `outgoing-area`, an outgoing area of 4096 bytes or more and an
    argument past its store's reach (chapter 8).

  The subscript by an `int` and by a `long` has `subscript` and
  `subscript-long`, and the subscript's address `subscript-address`
  and `subscript-row` (6.3).

  The register variables of chapter 4 have the programs x86-64's
  have, pinned for this target as well: `register-loop`,
  `register-undef`, `register-parameter` and `register-compare`. The
  access that steps a register pointer has `register-walk`, a copy, a
  length and a compare in the register idiom, pinned for both targets
  (6.3).
- **The ABI probes** are `tests/probes`, over the probes of
  `tests/probe/`: rcc's code against `clang -O0`'s, function by
  function over a table of values, run on the machine's own target.
  The calls probe pairs the two three ways: `clang`'s code calls
  rcc's, rcc's calls `clang`'s, and rcc's calls rcc's. Its cases
  include narrow arguments and results both ways, calls of none to
  twelve arguments, and variadic callees. The indirect probe pairs
  them the same three ways through pointers to functions: `clang`'s
  main calls rcc's functions through the pointers they return, rcc's
  code calls `clang`'s, of its library and of the main, through
  pointers, and rcc's calls rcc's. It calls through a local, a
  parameter, a register variable, an array element, a call's result,
  a conditional and a cast, with none to twelve arguments, narrow
  arguments and results, and a variadic pointee; it checks that the
  callee is evaluated before the arguments, and that a function's
  address, of one the file defines and of one it only declares,
  equals the main's and the library's. The memory probe runs every
  integer type loaded and stored through a pointer, aligned and not,
  the address of locals in frames beyond the reach of 5.3, arrays and
  string literals, and compares the memory each function writes byte
  by byte, guard bytes included. The pointers probe runs pointer
  arithmetic and the subscript over pointees of 1, 2, 4 and 8 bytes
  and rows of 3, 12 and 40, with an index of each integer type as a
  variable, as a call's result and as a constant on either side of
  what an immediate or a displacement holds, and compares the memory
  in the same way. It compares `p - q` with `clang`'s except in one
  place: for a negative difference that is no multiple of a size of
  2, 4 or 8, `clang -O0` shifts, and the probe's main computes the
  truncated quotient itself. The globals probe runs the objects at file
  scope of chapter 10, of every integer type and pointers to them,
  each one the file defines and one it only declares, which its
  library defines: read and written, a compound assignment and `++`
  for the effect and for the value, an element by a constant and by a
  variable index, a row of an array of arrays, the address taken and
  passed, a pointer dereferenced and stepped, and an object read
  before and after a call that writes it. The library is linked twice,
  once renamed, so each build has its own copy of every object; the
  main fills both copies alike before each check and compares every
  object byte by byte after it, so a store too wide or to the wrong
  object shows. It checks the initialized objects against their
  initializers, and a tentative definition written twice against
  zero, before anything writes them. The initializers probe runs the
  data of chapter 10's address constants and strings: a pointer
  initialized by each spelling of an address constant, of an object
  or a function the file defines and of one its library defines, and
  each shape of an array of `char` a string initializes. An address
  differs between the two builds, so the main compares what it
  designates, the address against its own build's object, the bytes
  of a string past its end included, a call through the pointer, and
  the comparisons each build's code makes of the address in its data
  with the one it forms. Its library's objects, initialized by
  `clang`, are read by rcc's code, and arrays declared without their
  size are completed by a later declaration, by the other file and by
  the end of the file.

  Three probes run the register variables. The registers probe runs
  a register variable of each integer type and a register pointer to
  four pointees: initialized, read by every operator on either side,
  assigned, tested, its read held across the paths of a branch and
  across a call, a parameter in each position, in blocks, beside a
  frame of 40000 bytes and in an expression deeper than the
  expression registers. Its functions that use one to ten registers
  prove the save area, and an unwritten narrow variable is run over
  registers filled with nine patterns. The inplace probe runs every
  compound assignment on each integer type and five pointees, as a
  statement and for its value, with a constant, a slot, a register
  variable and a call as the right operand, and `++` and `--` either
  side. It reads a narrow variable back at its promoted width too,
  `(x + 1) * 3L`, since a widening cast extends from the type's own
  bits and would hide a re-extension the write skipped. The walk probe
  runs the accesses of 6.3 that step a register pointer, over eight
  pointees, `char` to `long` and a pointer: as a load and as a store,
  both ends of `*p++ = *q++`, a compound assignment and a `++`
  through the pointer, steps at and past the form's reach, and the
  accesses the form does not take. Each of its functions returns the
  pointer's final offset beside what it read, so a step of the wrong
  size or at the wrong time shows. It compares with `clang`'s build
  except in one place: for a store whose right operand names the
  stepped pointer, `clang -O0` evaluates that operand before the left
  operand's step, where G13 evaluates the left operand first, so the
  probe's main compares rcc's result with a function of its own that
  makes the same accesses in G13's order.
- **`make test`** runs both scripts for every target with a pin
  directory, and then the probes.
- **The measurement** is taken on two machines, an Apple M1 Max and a
  Qualcomm Oryon, and `cost.md` chapter 4 has a table for each beside
  x86-64's. Two tuned sources are this target's own, in
  `tests/perf/aarch64/`. matmul's is the shared source as first
  written, ten registers live. strings' writes its loops with the
  `*p++` of 6.3; it was written for this machine after the generator
  existed, which "as first written" does not cover. Loop-head
  alignment is measured, not adopted (`cost.md` 4.2). `make slots`
  measures the cost of a stack slot on each core (`cost.md` 4.3,
  `slot.md`).

## 14. The slices

0. **The ground.** `target_t`; c0 takes the count, `char` and the
   frame limit from it; the driver's `-target`; `c1.cc` renamed; the
   shared file split out; the pins moved. No aarch64 instruction yet.
   Check: every x86-64 pin unchanged. The slice is in the tree.
1. **Integers, control, calls.** The frame, the prologue, chapters 6
   to 8. Check: the slice's pins read against this page, and the
   three probes. The slice is in the tree.
2. **Pointers, arrays, string literals.** The rows of 6.3 from `&x`
   to `"abc"`, the index under the table, and chapter 10. Check: the
   slice's pins read against this page, and the memory and pointers
   probes. The slice is in the tree.
3. **Register variables.** Chapter 4, the save area of 5.1 and 5.2,
   the rows of 6.3 that a register variable changes, and the access
   that steps a register pointer. Check: the slice's pins read against
   this page, and the registers, inplace and walk probes. The slice is
   in the tree.

Each slice is followed by its pins, read against this page, and its
documents, as the x86-64 slices were. Then the measurement, which
`cost.md` chapter 4 records.

## 15. The calls

Seven were open in the first draft of this page, and the eighth to
the thirteenth came later. The answers:

1. **Two generators**, `src/c1-x86-64.cc` and `src/c1-aarch64.cc`,
   each read whole against its own page (chapter 12). The
   instruction-free part is shared from the start; the register
   stack and the statements are written once, in `src/c1-stack.hh`
   and `src/c1-statements.hh`, which each generator includes.
2. **Plain `char` follows the platform**, unsigned here (chapter
   11). One source means two things on two machines, as with every C
   compiler, and a program that needs a signed byte says `signed
   char`. Signed everywhere would be a decision of the compiler's
   own, and would break the oracle's comparison with `clang` on any
   program that reads a `char` above 127.
3. **`cbz` and `cbnz` are the test of zero** (7.1). It is the
   machine's instruction for "compares unequal to 0", and it
   rewrites nothing: an explicit `a != 0` still compares. The
   Seventh Edition compiler's code table keyed rows on a zero
   operand in the same way, `tst` for the test.
4. **`wzr` is the source of a zero store** (6.6), on the same
   ground; the same table has `clr` for a zero. The cost is a second
   row for one constant: `x = 0;` is one instruction and `x = 1;`
   two.
5. **The index's extension is written inside the form** (6.3):
   `ldr w0, [x1, w2, sxtw #2]` reads `p[i]` for an `int i` in one
   instruction. The first draft kept x86-64's row for the three
   slices, `sxtw x2, w2` and then the load. But base, extended index
   and scale are one operand on this machine, as base, index and
   scale are one on x86-64, and a program written for the machine
   gets the machine's addressing form. The cost is that the two
   targets' listings differ for the same subscript, and that the
   `long` index is a lever on one of them only. The extending load of
   6.3 leaves the index alone: its extension is the form's, so no
   instruction is left for the load to absorb, and the index's load
   stays `ldr w1`.
6. **`-target`, and both pin sets checked on one machine** (chapter
   11). The cost is a driver option and a `--target` passed to
   `clang`.
7. **No loop-head alignment**: measured, not adopted (7.2). On two
   cores the best directive nets half a percent or less over the
   tuned sources, and `clang -O2` aligns no loop head on this target
   (`cost.md` 4.2).
8. **The address of a subscript by a constant 0 is the pointer, and no
   instruction** (6.3), on both targets. 6.5.4.2p3 has `&p[0]` "as if
   the & operator were removed and the [] operator were changed to a +
   operator", and `p + 0` is an identity, which writes no instruction
   (`backend.md` 6.4), so `&p[0]`, `p + 0` and `p` are one listing.
   `p[0]` and `*p` are one access on either machine,
   `movl (%rax), %eax` and `ldr w0, [x0]`. `subscript-address` pins
   the row on both targets, and `subscript-row` a row of an array of
   arrays, `m[0]`. On an array reached by its name the constant 0 is
   the name and constant of 6.3 with nothing to add: `a[0]` and `*a`
   are the slot's own access, and `&a[0]` and `a + 0` are one listing,
   the array's address.
9. **An access through a stepped `register` pointer makes the step**
   (6.3), by the form of 2.3 that writes its base: `*p++` is `ldrb
   w0, [x19], #1`, and `*(p += c)` takes the pre-indexed form when
   the step's bytes are in the form's reach, since "The expression ++E
   is equivalent to (E+=1)" (6.5.4.1p2). The rules carried from
   x86-64 write three instructions, the copy, the step and the access
   through the copy. But the base and its step are one operand on this
   machine, as base, extended index and scale are (call 5), and a
   program written for the machine gets the machine's form. The form
   is a case of the address, not a row of the load or of the store,
   so one rule serves both ends of `*p++ = *q++`. The cost is that the
   two targets' listings part for the same source, and that one store
   has two rows: one whose right operand names the pointer keeps the
   step before that operand, as G13 has it. `register-walk` pins the
   row on both targets, and `autoincrement.md` has the argument.
10. **A `register` variable is not kept across `longjmp`**, `volatile`
    or not, as on x86-64 (`backend.md` chapter 13, call 9): the
    variable is in its callee-saved register (chapter 4), which
    `longjmp` restores to its value at `setjmp`. Every other local is
    in its slot and keeps the value last stored. The cost is a
    departure from 7.13.2.1p3, which keeps the value of a volatile
    local of the function that called `setjmp`.
11. **A `register` variable is the destination of the value computed
    for it** (chapter 4), as on x86-64 (`backend.md` chapter 13, call
    16): the row that computes the value writes the variable's
    register, `add w19, w0, w1` and `ldr w19, [x29, #24]`, wherever
    the row writes its destination only after it has read every
    operand: an address in two instructions writes it twice, `adrp
    x19, buf; add x19, x19, :lo12:buf`, and reads none. An instruction
    here names its destination apart from its sources, so the variable
    is written after every operand is read, and the two conditions
    x86-64 takes from the Seventh Edition compiler's `sreorder()` are
    not needed. A conversion that changes the signedness alone is no
    boundary, on both targets: `register unsigned u = i` is the one
    load, `ldr w19, [x29, #32]`, and `u = a + b` on two `int`s `add
    w19, w0, w1`. The cost is that
    the two targets' listings part for the same source: `r = a + (b +
    c)` and `r = a - r` write the variable here and keep the `mov` on
    x86-64. On both targets each path of `&&`, `||` and `?:` writes
    the register, `mov w19, #1` and `mov w19, #0` for `r = a && b`, a
    first arm whose row writes no register moved in on its own path,
    and under a conversion that widens the conditional each arm
    extended into it; here the arms' rows that write it are more, `r =
    c ? f(x) : a / b` being `mov w19, w0` and `sdiv w19, w0, w1`.
12. **A constant on the left is taken on the right** (6.1), of a
    comparison and of `+`, `*`, `&`, `|` and `^`, as on x86-64
    (`backend.md` chapter 13, call 18): `5 < x` is `ldr w0, [x29,
    #16]; cmp w0, #5` and `b.le`, `0 == (c & 1)` the `and` and `cmp
    w0, #0`, and `5 + x` the load and `add w0, w0, #5`. A constant the
    instruction does not encode, and `mul`'s, is taken on the right
    too, since it is held in a register in either order and no
    instruction here names an object in memory; on the right it is
    held across nothing, so `5 * f()` spills nothing. The cost is that
    the two targets' listings part for a constant x86-64 holds:
    `0x100000000L < x` and `0x100000000L + x` load `x` first here and
    the constant first there.
13. **A `return`'s value is computed in `x0`** (6.1), as on x86-64
    (`backend.md` chapter 13, call 19): the return names `x0`, and the
    instruction that last writes the value writes it where no other
    value holds it, `ldr x1, [x29, #24]; add w0, w1, w0` for `x + (c ?
    1 : 2)` and `sdiv w2, w1, w0; msub w0, w2, w0, w1` for a remainder
    by a call's result. An instruction here names its destination
    apart from its sources and reads every source before it writes, so
    the name goes to the row's last instruction and not to its left
    operand: a source in `x0` is no obstacle, and a call within an
    operand, whose result is in `x0`, stops nothing. One thing is
    generated with `x0` left for last, the place of an assignment or
    of a step, so that the stored value, the operation of `+=` and a
    postfix step's old value take it: `return p[i] = x` is `ldr x0,
    [x29, #32]; str x0, [x1, w2, sxtw #3]`. The arms of a returned
    conditional are generated for `x0`, and below a cast, a comma or
    an assignment where `backend.md` 6.1's rule and edge allow it,
    read for this machine's rows. The cost is a second hand-down
    beside the variable's, the state's reservation, and a boundary: a
    call's result within the place holds `x0` at the store, two arms
    through a call's result keep their join below a cast, and at the
    top of the returned value they are each moved into `x0`, the one
    place the rule does not reach.

Decided by the machine, listed so that nothing is decided silently:

- the frame record at the bottom and positive displacements (5.1);
- the save area directly above the frame record, where its `stp` and
  `ldp` reach it in a frame of any size (5.2);
- a stack parameter copied to its slot by the prologue (5.2);
- slots in declaration order, a far one through `x16` (5.3);
- the remainder as `sdiv` and `msub` (6.3);
- G4 without a trap (chapter 9).
