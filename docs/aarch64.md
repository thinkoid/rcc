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

c0 carries too, but for three facts it must now take from the target
(chapter 11): the count of register variables, the signedness of
plain `char`, and the frame limit.

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

A function's address and a string's are formed PC-relative in two
instructions, `adrp x0, sym` for the 4 KiB page and `add x0, x0,
:lo12:sym` for the offset in it. A call is `bl f`; the linker makes
the PLT entry when `f` is in a shared library, and no `@PLT` is
written.

## 4. The register discipline

Thirty-one registers, five classes, fixed at design time.

**Register variables: `x19` to `x28`.** Ten, the whole callee-saved
set but the frame pointer. A function's `register` variables take them
in order of declaration, the first `x19`. Everything else of
`backend.md` chapter 4 holds with the names changed: a variable keeps
its register to the end of its block, sibling blocks share, an
eleventh live at once takes a slot and the warning, `register
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
- An assignment is the `mov` into the register, `mov w20, w0`, or
  `mov x20, x0` for a `long` or a pointer. A constant is the
  immediate, `mov w20, #0`. A narrow one is the extension, `sxtb w19,
  w0` for a `signed char`, `uxtb w19, w0` for a `char`. A
  declaration's initializer is the same. Without one nothing is
  written (G5), but a narrow or an `unsigned` variable is extended in
  its own register, `sxtb w19, w19` or `mov w19, w19`, so that the
  register holds the representation above.
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
  unspecified. The moves come in parameter order among the stores of
  the other parameters. One past the eighth is loaded straight into
  its register from where the caller stored it, `ldrsb w19, [x29,
  #64]`.

**The frame pointer: `x29`.** Set in every function's prologue, leaf
included, and every slot is a displacement off it.

**The link register: `x30`.** Stored in the frame record by every
prologue and loaded by the epilogue, leaf included. A leaf could keep
its return address in the register and store nothing. It does not:
one prologue shape, and the record is the chain a debugger walks.

**Expression registers: `x0` to `x7`, `x9` to `x15`.** Fifteen, the
caller-saved set without `x8`, used as a stack: the generator takes
the lowest free one. `x0` is first, so a return value tends to be
where it is wanted. Nine held any expression on x86; fifteen do here,
and the spill path is the same code.

**The scratch pair: `x16`, `x17`.** Never an expression register and
never held across a node. `x16` takes an address the instruction
cannot reach (chapter 5.3). `x17` carries the bytes of a copy from
memory to memory, which the machine has no instruction for: a stack
argument on its way from its slot to the outgoing area, and a stack
parameter on its way to its slot. Both are dead at every `bl` by the
convention, which is what suits them to a use that never spans one.

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
   `str`. They are counted from the tree before the body, as on
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
parameter stays where the caller pushed it. One that lies 16 MiB or
more above `x29`, where the `add` into `x16` does not reach (5.3), is
a not-yet; only a frame within a few bytes of its limit puts one
there.

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
When `L` is a register variable's read, the result takes a fresh
expression register, `op N, L, R`, and that is the whole of what x86's
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
commutative operator's operands are not swapped.

### 6.2 Depth and spill

Unchanged. A spill is `str x0, [x29, #d]` and a reload `ldr x0, [x29,
#d]`, 8 bytes. Fifteen registers run out at sixteen values held at
once.

### 6.3 The table

For a 32-bit operation (`w` names) or a 64-bit one (`x` names).

| Source | Instruction | Note |
|---|---|---|
| a constant | an immediate where the instruction encodes it; else `mov L, #c`, with `movk` for each further 16 bits a wide one needs | 6.4 |
| a local, a parameter | the load of 2.2 from `[x29, #d]` | through `x16` beyond reach, 5.3 |
| a `register` variable | the register, as a source operand; as `L` of a row that writes its result, the result in a fresh `N`, `sub w0, w19, w20`, `lsl w0, w19, #3`, `add x0, x19, w20, sxtw #2` for `p + n`, `sub x0, x19, x20` for `p - q` before its `sdiv` | no instruction, and no copy (6.1) |
| `a + b`, `a - b` | `add L, L, R`, `sub L, L, R` | G1 |
| `a * b` | `mul L, L, R` | G1; a constant factor is first loaded, `mov R, #4` |
| `a / b` | `sdiv L, L, R`, `udiv L, L, R` | G4: no trap, 2.6 |
| `a % b` | `sdiv N, L, R; msub L, N, R, L`; for `L` a register variable's read, `msub` is the last to read `R`, so its result takes the register `R` leaves, as in every row (6.1), and the quotient's register is taken and freed as before: `sdiv w1, w19, w0; msub w0, w1, w0, w19` | the machine has no remainder; `N` is taken and freed |
| `a << b`, `a >> b` | `lsl L, L, R`, `asr`, `lsr`; a constant count is the immediate, masked to the width | G3; the count in any register |
| `a & b`, `a \| b`, `a ^ b` | `and`, `orr`, `eor` | |
| `-a`, `~a` | `neg L, L`, `mvn L, L` | |
| `!a` | `cmp L, #0; cset L, eq`; for an operand with stale upper bits, `tst L, #0xff` in place of the `cmp` (7.1) | as a value |
| `a < b` and the rest, as a value | `cmp L, R; cset L, lt` | `lt`, `le`, `gt`, `ge`, `eq`, `ne` signed; `lo`, `ls`, `hi`, `hs` unsigned and for pointers (G7) |
| `a && b`, `a \|\| b`, `a ? b : c`, as a value | jump code into `L`, `mov L, #1` and `mov L, #0` on the arms | never `csel`: 6.5.16p5, the arm not chosen is not evaluated |
| `x = e` | the store of 2.2 to the slot; for a register variable the `mov` into its register, `mov w20, w0`, `mov x20, x0`, a constant the immediate, `mov w20, #0`, a narrow type the extension, `sxtb w19, w0` | 6.6 for a constant; chapter 4 |
| `x op= e`, `++x`, `x++` | the load, the operation, the store, after `e` for `x op= e`; a register variable in place, after `e`, its register `L` and the destination, `add w19, w19, w0`, `add w19, w19, #1`, a narrow one re-extended after, `sxtb w19, w19`, and `%=` with the quotient in a register of its own, `sdiv w0, w19, w20; msub w19, w0, w20, w19`; `x++` on a register variable for its value is G13's copy and the step, `mov w0, w19; add w19, w19, #1`; a pointer's `++` and `--` step by the size, an immediate where `add` encodes it, `add L, L, #4` for an `int *`, and otherwise held in a register | no memory form, 6.6; chapter 4 |
| `(T) a` | `sxtw L, L` for `int` to `long`; `sxtb`, `sxth`, `uxtb`, `uxth` from a narrow type; each only when `L` is not already valid to the new width; `mov wL, wL` for an `unsigned` not valid to 64; nothing between pointer types and for a pointer to `long`; an integer to a pointer as to `unsigned long`, so `sxtw` for an `int` | G2; the width rule |
| `&x` | `add L, x29, #d` | two `add`s beyond 4095, 5.3 |
| `*p` | the load of 2.2 from `[L]` | G8 to G11 |
| `*p++`, `*p--`, `*++p`, `*--p`, `*(p += c)`, `*(p -= c)` on a `register` pointer | the access by a form of 2.3 that steps the variable's register, the borrowed read the base: the post-indexed form for `p++` and `p--`, `ldrb w0, [x19], #1` and `ldrb w0, [x19], #-1`, and the pre-indexed one for the others, `ldrb w0, [x19, #1]!`, `ldr w0, [x19, #16]!` for `*(p += 4)` on an `int *`; a store the same, `strb w0, [x19], #1`, so `*p++ = *q++` is `ldrb w0, [x20], #1; strb w0, [x19], #1`; a compound assignment or a `++` through the pointer, two accesses through one address, steps once, the post form at the store, `ldrb w0, [x19]; add w0, w0, #5; strb w0, [x19], #1`, the pre form at the load; a read of the pointer still held is copied before the access that steps, `mov x0, x19`. The step is its own instructions and the access goes through the old or the new value, as the rows of `x++` and `*p` have it, in three cases: a store whose right operand names the pointer, since G13 completes the step before that operand, `*p++ = *p` being `mov x0, x19; add x19, x19, #1; ldrb w1, [x19]; strb w1, [x0]`; a step out of the form's reach, `*(p += 64)` on an `int *` being `add x19, x19, #256; ldr w0, [x19]`; and a pointer in a slot, whose step is a load, an `add` and a store | `autoincrement.md`; call 9 |
| `p[i]`, `a[i]` | the load from `[L, R, lsl #k]`, `[L, R]` for a size of 1; from `[L, wR, sxtw #k]` or `[L, wR, uxtw #k]` for an index held at 32 bits; a constant index is `[L, #c*s]` when the displacement encodes, a negative one within 256 bytes the unscaled displacement, `[L, #-4]`; a constant index the access does not encode (2.3) is an index like any other, held in a register by the constant row of 6.4 and read `[L, R, lsl #k]` | one instruction; the index's extension is the form's, below (call 5) |
| `&p[i]`, `&*p` | the address of `*E` as a value, and no access made (6.5.4.2p3): a bare base is its own value, so `&*p` is `p`, and so is `&p[0]`, a subscript by a constant 0, whose access is `[L]` as `*p`'s is, no instruction (call 8); any other subscript's address is the row of `p + n`: `&p[3]` is `add L, L, #12` for an `int *`, and `&p[i]` is `add L, L, R, lsl #k`, or `add L, L, wR, sxtw #k` for an index held at 32 bits; a row of an array of arrays decays the same way, `m[0]` to `m` | the same address by `add`, the machine having no `lea` |
| `p + n`, `p - n` | `add L, L, R` or `sub` for a size of 1; `add L, L, R, lsl #k` or `sub` for 2, 4 or 8; the index held at 32 bits as for `&p[i]`; `mov N, #s; mul R, R, N` first for any other size; a constant `n` is the immediate `#c*s` when it encodes, and otherwise the product held in a register by 6.4, `add L, L, R`; for `n + p` the result is in `L`, the integer's register: a plain register keeps the source's order, `add L, L, R`, and the shifted or extended operand, which the instruction takes last only, is `add L, R, wL, sxtw #2` | G6 |
| `p - q` | `sub L, L, R`; for a size other than 1, `mov R, #s; sdiv L, L, R` | G6: a division |
| `"abc"` | `adrp L, .Ls<n>; add L, L, :lo12:.Ls<n>` | two instructions, chapter 10 |
| `f(a, b)` | chapter 8 | |

The width rule holds as written. Its facts on this machine:

- a narrow load leaves the value valid to 32;
- a write to a `w` name clears the upper half, so an `unsigned`
  written at 32 is valid to 64 and an `int` to 32;
- a narrowing cast writes nothing;
- a call's result is valid to its own type's width and no more: 8 for
  a `char`, 16 for a `short`, since the convention leaves the rest
  unspecified (chapter 3). The cast that widens it writes the
  extension.

And a `return` of a `char` or a `short` writes no extension: the
convention asks none of the callee, and G16 adds nothing.

**The index.** The integer operand of a subscript or of pointer
arithmetic is 64 bits by the parser's cast, on both targets. On
x86-64 the cast's instruction is written, `movslq` for an `int`, and
the load follows. This machine's addressing form takes a 32-bit index
and extends it itself, so the cast is looked through wherever it
stands, the source's own `p[(long) i]` included, since the tree does
not tell the two apart. The cast's operand is generated and what is
held decides the operand:

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
or 8 is multiplied in, and there the cast writes its `sxtw` before
the `mul`, as it does anywhere else.

So `p[i]` for an `int i` is one instruction, and for a `register int
i` it names the variable's register and nothing is copied:

```
	ldr	w0, [x1, w20, sxtw #2]
```

On x86-64 a `long` index is the programmer's lever against the
`movslq`. Here an `int` index and a `long` one cost the same.

### 6.4 Constants

c0's folding is unchanged, and is the same on both targets: a
division with no value stays unfolded, and on this machine the
instruction left to compute it yields a number instead of a trap.

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
the registers the operands are in. The state's `reserved` mask,
`vacate()`, `move()` and `pinned()` exist in the x86-64 generator
only.

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

## 7. Conditions and control

### 7.1 Jump code

`cbranch(e, label, sense)` as in `backend.md` 7.1:

- A comparison: `cmp L, R` and `b.cc label`, the condition or its
  inverse by `sense`.
- `!e`, `&&`, `||`: the same shapes.
- `a, b`: `effect(a)`, then `cbranch(b, label, sense)`.
- Any other expression: its value into `L`, then the test of zero,
  `cbnz L, label` or `cbz L, label`, one instruction (call 3).

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

As `backend.md` 7.2, with `b` for `jmp`. `return e` leaves `e` in
`x0`, and a `mov` puts it there when it is held in another register:
`mov w0, w1` at 32 bits, and on the `x` names for a `long` or a
pointer. `x op= e` for its value is such a case: the result is in
the object's register, taken above `e`'s, so a `return` of it ends
in `mov w0, w1`. Every `return` jumps to the one epilogue. `main`
returns 0 on reaching its `}`, `mov w0, #0`.

**No alignment directive before a loop head**: measured, not
adopted. `.p2align 5` on x86-64 was measured on one processor and
adopted as "a convention of the target" (`backend.md` 13, call 8). On
this target `.p2align 4`, `5` and `6` were measured on two cores, a
Qualcomm Oryon and the Firestorm core of an Apple M1 Max. The best,
`.p2align 5`, nets -0.5% on the Oryon and -0.3% on Firestorm over the
tuned sources, and `clang -O2` aligns no loop head here (`cost.md`
4.2, call 7).

## 8. Calls

Every argument is a hidden local of its call, as before.

1. Every held value is spilled.
2. Each argument in turn is written to its slot by the store row of
   6.6.
3. The call sequence. What the arguments left held in an expression
   register is spilled first (`backend.md` 6.7). With more than
   eight arguments, `sub sp, sp, #A` opens the outgoing area, `A`
   being 8 bytes for each argument past the eighth, rounded up to
   16. Each of those is copied from its slot at its own width, `ldr
   w17, [x29, #44]; str w17, [sp]`, the ninth at `[sp]`, the tenth
   at `[sp, #8]`. A `char` is `ldrb` and `strb`, and the seven bytes
   above it in the area stay as they were: they are padding to the
   convention (chapter 3). The first eight are loaded from their
   slots into `x0` to `x7`, a narrow one extended by its load. `bl
   f`. `add sp, sp, #A`.
4. The result is in `x0`, valid to its type's width (6.3).

An area of 4096 bytes or more, which the immediate of `sub` does not
encode, is a not-yet: a call takes 510 arguments past the eighth. No
unwind directive is written around the area, since the canonical
frame address is computed from `x29` there.

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
| G16 a call goes out as written | chapter 8 |
| G17 a string literal is writable | `.data`, chapter 10 |

G4 is the row to read twice. `guarantees.md` already says "x86 `idiv`
faults on a zero divisor and on `INT_MIN / -1`; aarch64 `sdiv` yields
0 and wraps". A program that relies on the trap is relying on x86,
and rcc does not write a check to give this machine one: that would
be a decision of the compiler's own.

The misalignment the machine does refuse is `sp`'s, which is the
generator's to keep and never the program's.

## 10. Statics and literals

As `backend.md` chapter 10: one `.text`, one `.data` block of
literals after the last function, `.Ls<n>` and `.string`, never
merged, never read-only. A literal's address is the `adrp` and `add`
pair. A function is `.globl`, `.type f, @function` and `.size`. No
alignment directive is needed before a function: every instruction
is 4 bytes, and the assembler aligns `.text` to 4. The listing opens
and closes as on x86-64: `.file` with the source's name and `.text`
first, `.section .note.GNU-stack,"",@progbits` last.

## 11. c0 and the driver

c0 learns the target, for three facts:

- **The register variables**, 5 or 10: the count c0 assigns from and
  the number in its warning.
- **Plain `char`**, signed on x86-64 and unsigned on aarch64.
  6.2.5p20: "The implementation shall define char to have the same
  range, representation, and behavior as either signed char or
  unsigned char." The platform defined it, and rcc follows the
  platform (call 2). A program that needs a signed byte writes
  `signed char`.
- **The frame limit**, 2 GiB less 16 on x86-64 and 16 MiB less 16
  on aarch64 (5.2): the largest frame. c0 refuses a local over it at
  its declaration, where its position is known. The generator holds
  the same limit for the frame as a whole.

They live in one small description, `target_t`, that c0, the
generators and the driver read: the name, the name `clang` takes after
`--target=`, the three facts. Everything else about a target is in its
generator.

The driver takes `-target x86-64` or `-target aarch64`, and the
default is the machine rcc was built on. With `-S` either target's
text can be written on either machine. With `-c` or a link the
driver hands `clang` the matching `--target`. The `clang` of either
machine assembles the other target's pins, so one machine checks
both sets of pins; only the oracle's runs and the probes need the
machine's own target (call 6).

## 12. The code

The x86-64 generator is one file, and every function in it writes
x86 mnemonics. Two ways to add a target:

- **One walk over a target interface.** The walk is shared and each
  instruction goes through a table or an object the target supplies.
  The two machines disagree on the operand count, on memory operands
  and on pinned registers, so the interface is either the union of
  both or the least of both. It is a second, smaller LLVM, and the
  first one was retired for the judgment it kept between the tree
  and the instruction.
- **Two generators.** `src/c1-x86-64.cc`, today's `c1.cc` renamed,
  and `src/c1-aarch64.cc`, each read whole against its own design
  page. `c1.hh`'s `generate()` takes the target and calls one.

The second is the answer (call 1), with one qualification.
Some of c1 holds no instruction: the integer view of a type, the
tree predicates (`writes()`, `registers_in()`, `is_comparison()`),
the label names, the literals' `.string` block. That part is shared
from the start, in a file of its own. The register stack, `spill()`,
`hold()`, `reg_of()` and the rest, is written once, in
`src/c1-stack.hh`. Each generator includes it inside its own
namespace and so compiles it against its own registers. The stack
writes three instructions, the store to a spill slot, the reload
from one and the `mov` of a register into another; each generator
defines them before the include. `take()` stays in each generator,
since x86's has the reservation, and so do x86's pins. The contract
is one: the held values and the two save points of `backend.md` 6.7.

## 13. Verification

- **The oracle.** `clang -O2` on this machine gives the twelve corpus
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
  - `constant-bits`, a constant by its bits (6.4);
  - `stack-parameter`, the stack parameter's copy (5.2);
  - `large-frame`, a frame of 4096 bytes or more (5.2).

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
  twelve arguments, and variadic callees. The memory probe runs every
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
  truncated quotient itself.

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

Seven were open in the first draft of this page, and an eighth and a
ninth came later. The answers:

1. **Two generators**, `src/c1-x86-64.cc` and `src/c1-aarch64.cc`,
   each read whole against its own page (chapter 12). The
   instruction-free part is shared from the start; the register
   stack is written once, in `src/c1-stack.hh`, which each generator
   includes.
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
   `long` index is a lever on one of them only.
6. **`-target`, and both pin sets checked on one machine** (chapter
   11). The cost is a driver option and a `--target` passed to
   `clang`.
7. **No loop-head alignment**: measured, not adopted (7.2). On two
   cores the best directive nets half a percent or less over the
   tuned sources, and `clang -O2` aligns no loop head on this target
   (`cost.md` 4.2).
8. **The address of a subscript by a constant 0 is the pointer, and
   no instruction** (6.3), on both targets. Two readings meet here.
   6.5.4.2p3 has `&p[0]` "as if the & operator were removed and the
   [] operator were changed to a + operator", and `p + 0` adds
   (`backend.md` 6.4: `x + 0` adds), so `&p[0]` would be an `add` of
   0. Against it, `p[0]` and `*p` are one access on either machine,
   `movl (%rax), %eax` and `ldr w0, [x0]`: the constant is the
   displacement of the addressing form, and a displacement of 0 is
   the bare base. The address of one access is thus the address of
   the other, and `&*p` is `p`. The page takes the second, since no
   `+` was written. The cost is that `&p[0]` and `p + 0`, one value,
   are two listings. `subscript-address` pins the row on both
   targets, and `subscript-row` a row of an array of arrays, `m[0]`.
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

Decided by the machine, listed so that nothing is decided silently:

- the frame record at the bottom and positive displacements (5.1);
- the save area directly above the frame record, where its `stp` and
  `ldp` reach it in a frame of any size (5.2);
- a stack parameter copied to its slot by the prologue (5.2);
- slots in declaration order, a far one through `x16` (5.3);
- the remainder as `sdiv` and `msub` (6.3);
- G4 without a trap (chapter 9).
