# rcc: the back end

The design of rcc's own code generator: x86-64 assembly written from
the tree, registers owned by the compiler that promises what they
hold, and the system cc for the three things it does without
judgment, preprocessing, assembling and linking. This page is the
design before the code, as the guarantees were written before the
compiler. It is in the register of `findings.md`: the terms first,
then the mechanism, with the standard and the ABI cited where they
govern. The last chapter lists the calls still open; the queue,
`TODO`, holds the slices.

## 0. tl;dr

- Ritchie's machine on x86-64. Every local lives in its stack slot,
  off the frame pointer. A `register` variable lives in one of five
  callee-saved registers, by declaration order, and is nothing but
  that register; the sixth declared takes a slot and a warning.
  Expressions are evaluated left to right in the caller-saved
  registers, used as a stack. Conditions are jumps. An argument is a
  hidden local of its call.
- The instruction written is the instruction in our table. No
  selector rewrites it, no allocator rematerializes it, no late pass
  tidies it. The five findings of `findings.md` have no place to
  happen.
- The System V AMD64 convention at every call and every entry, so
  rcc's objects link with anyone's and call libc.
- Each guarantee is delivered by one instruction of the table, named
  in chapter 9.
- Integer constant expressions are folded in the parser, which
  needs the evaluator anyway for array sizes, `case` labels and
  static initializers. A constant is an immediate; `-1` costs
  nothing.
- The six calls the first draft left open are weighed in
  `design-calls.md` and made; chapter 13 records them.
- The oracle is unchanged. The cost pins become the assembly rcc
  writes. The measurement is redone under the register discipline.
- LLVM leaves the tree. Its one lasting contribution is chapter 4 of
  `cost.md` and `findings.md`: the written record of where a modern
  code generator's judgment hides, which this design was drawn
  against.

## 1. Why

`cost.md` chapter 2 holds LLVM's code generator to a literal
pipeline with twenty-one switches. `findings.md` records what the
switches do not reach: the selector's own strength reduction, a
switchless pass, an allocator that rematerializes a constant or a
frame address wherever it likes. Each is small. Together they say
the tool has a judgment of its own that no option removes, because
no one who wrote it wanted a literal compiler. Defending the thesis
against the tool is a maintenance stream that grows with every LLVM
release; writing the back end makes the thesis a table.

The charter says "using LLVM to bring early C's machine-oriented
programming model to modern targets". The model is the point and
LLVM is the means; a means that has to be fought is rewritten. What
LLVM still does for rcc after this design is what any cc does:
`cpp`, `as`, `ld`, under whatever name. Those have no opinion about
the program.

## 2. Background: Ritchie's machine

### 2.1 The PDP-11 compiler

The C compiler of Sixth and Seventh Edition UNIX was two passes,
`c0` and `c1`, with an optional peephole pass `c2`. `c0` parsed and
built expression trees; `c1` generated PDP-11 assembly from them,
table-driven, and the assembler `as` encoded it. The machine had
eight 16-bit registers: `r0` and `r1` were the expression registers,
`r2` to `r4` held the `register` variables, three of them, and any
not taken by a variable joined the expression registers; `r5` was
the frame pointer, `r6` the stack pointer, `r7` the program counter.
Every other local was a word or two on the stack, addressed off
`r5`.

Two properties of that compiler are the model this design keeps.
The first: the programmer decided what lived in a register. The
keyword `register` was not a hint to an allocator; it was the
allocation. Three variables fit, the fourth went to memory, and a
programmer who wanted the loop fast put the loop's three variables
first. The second: the compiler wrote the instruction for the
operator. `x * 4` was a multiply. There was a `c2` that would turn
some of them into shifts, and the programmer ran it or did not.

### 2.2 The register count of a tree

Generating code for an expression tree with a fixed number of
registers is the problem Sethi and Ullman solved in 1970. Each node
is labeled with the number of registers its evaluation needs: a
leaf needs one, a node whose children need `l` and `r` needs
`max(l, r)` when they differ and `l + 1` when they are equal, since
the result of the first child must be held while the second is
evaluated. The compiler then evaluates the child with the larger
label first, so the held value is the cheaper one. When a label
exceeds the registers available, a subtree's result is stored to
memory and reloaded. `c1` carried such labels, and it did evaluate
the harder child first.

rcc does not reorder. G13 promises left to right: "operands and
arguments are evaluated left to right, and an operand's side effects
complete before the next operand starts". So a node's left child is
always evaluated first and its result held while the right child is
evaluated, whatever the labels say. The labels still serve one
purpose: to know the depth of held values, which is how many
registers the expression needs, and when it exceeds the set, which
held value to store. Chapter 6 gives the rule. On x86-64 the set is
nine registers and a C expression that holds nine values at once is
written by nobody; the spill path exists for completeness and is
exercised by one test.

### 2.3 Value code and jump code

An expression in C is evaluated for one of two reasons: for its
value, as in `x = a < b`, or for a decision, as in `if (a < b)`.
`c1` had two generators, `rcexpr` for a value in a register and
`cbranch` for a jump on the truth of an expression, and the second
never materialized a 0 or 1: `if (a < b)` was a compare and a
conditional branch to the else label, and `if (a && b)` was two
compares and two branches. This is the distinction the LLVM pipeline
could not make for rcc, because the tree holds an `int` for every
condition and the IR had to materialize it: `cost.md` chapter 3's
"used as a condition, the value is then compared with 0 again", the
`setcc`, `and $1`, `movzbl`, `cmp $0` chain that is 17 of the 23
instructions in sort's inner loop. Chapter 7 is `cbranch`.

## 3. Background: x86-64 and its convention

### 3.1 The registers

Sixteen 64-bit general registers: `rax`, `rbx`, `rcx`, `rdx`,
`rsi`, `rdi`, `rbp`, `rsp`, `r8` to `r15`. Each has a 32-bit name
(`eax`, `r8d`), a 16-bit name (`ax`, `r8w`) and an 8-bit name (`al`,
`sil`, `r8b`); a write to the 32-bit name clears the upper half, a
write to a narrower name does not. `rsp` is the stack pointer and is
never anything else. The flags register holds the result of the last
`cmp`, `test` or arithmetic instruction, and the conditional
instructions, `jcc`, `setcc`, read it.

Three instructions pin registers. `idiv` and `div` divide the
128-bit pair `rdx:rax` (or `edx:eax`) by their one operand, leaving
the quotient in `rax` and the remainder in `rdx`; `cltd` and `cqto`
sign-extend `rax` into `rdx` beforehand, and `xor %edx,%edx` zeroes
it for an unsigned divide. A shift by a variable count takes the
count in `cl` only.

### 3.2 The System V AMD64 ABI

The convention every Unix on x86-64 follows, and the one `clang`
and `gcc` compile to, so an rcc object links with theirs. For the
types rcc has, integers and pointers:

- The first six integer or pointer arguments go in `rdi`, `rsi`,
  `rdx`, `rcx`, `r8`, `r9`, in that order; further arguments go on
  the stack, right to left, so the seventh is at the lowest address.
  A `char` or `short` argument is passed in the register's low bits,
  and the ABI leaves the upper bits undefined; the callee extends.
- The result returns in `rax` (`eax` for a 32-bit type).
- `rbx`, `rbp`, `r12`, `r13`, `r14`, `r15` are callee-saved: a
  function that uses one restores it before returning. The other
  nine, `rax`, `rcx`, `rdx`, `rsi`, `rdi`, `r8` to `r11`, are
  caller-saved: a call may destroy them.
- At the `call` instruction `rsp` is a multiple of 16; so on entry,
  after the return address is pushed, `rsp + 8` is.
- The 128 bytes below `rsp` are the red zone: a signal handler will
  not touch them, so a function that calls nothing may use them
  without moving `rsp`.
- A call to a variadic function sets `al` to an upper bound on the
  number of vector registers used for arguments; rcc has no floating
  point yet and sets it to 0.

### 3.3 Two-address form and AT&T syntax

x86 arithmetic is two-address: `add %rcx,%rax` adds `rcx` into
`rax`, destroying `rax`'s old value. There is no three-operand add
except through `lea`, and the one three-operand form rcc writes is
`imul $3,%eax,%eax`, multiply by an immediate into a destination.
This shapes the expression rule of chapter 6: the register of the
left operand is the register of the result.

rcc writes AT&T syntax, which `as` reads and `objdump` prints:
source first, destination last, `%` before a register, `$` before an
immediate, `(%rax,%rcx,4)` for a base, an index and a scale,
`-8(%rbp)` for a base and a displacement, `sym(%rip)` for a
PC-relative address. The suffix (`l`, `q`, `b`, `w`) gives the
width where no register operand does.

## 4. The register discipline

Sixteen registers, four classes, fixed at design time.

**Register variables: `rbx`, `r12`, `r13`, `r14`, `r15`.** Five of
the callee-saved set. A function's `register` variables take them
in order of declaration; the first declared gets `rbx`. A variable
keeps its register until its block ends, and a variable declared in
an inner block may take a register that an earlier block's variable
has released, so sibling blocks share. A `register` variable beyond
the five gets a slot and is an ordinary local, as 6.7.2p12 allows
("the extent to which such suggestions are effective is
implementation-defined"), and as the PDP-11 did with the fourth; the
compiler says so with a warning, `register variable 'k' in a stack
slot: the five registers are taken`, which a conforming program may
receive and a `-w`-style switch silences. The extent is recorded in
the cost model, as 4p10 asks (`design-calls.md` chapter 3).

A `register` parameter is moved from its argument register to its
callee-saved register in the prologue, one `mov`; the argument
register is caller-saved and would not survive the first call.

A register variable is nothing but its register. No slot, no load,
no store, no `phi`, no join of arms, no patching of a loop's back
edge: an assignment to it is a `mov` into the register, a read of it
is the register as an operand. G5 for a register variable needs no
unwritten slot any more: before its first write the register holds
whatever the prologue left in it, and nothing writes the register
until the program does, so every read before the first write sees
the same value. The function's epilogue restores every register of
the set the function used, from the pushes of its prologue.

**The frame pointer: `rbp`.** Pushed and set in every function's
prologue, leaf included, restored by `leave`. Every slot is a
displacement off it, known the moment the slot is declared, whatever
the frame comes to hold below; `rsp` may move inside a body, for
the pushes of a call, without disturbing an offset. The sixth
callee-saved register was weighed as a sixth register variable and
the frame pointer won on what the generator has to know
(`design-calls.md` chapter 2).

**Expression registers: `rax`, `rcx`, `rdx`, `rsi`, `rdi`, `r8`,
`r9`, `r10`, `r11`.** The caller-saved set, nine registers, used as
a stack: the generator takes the lowest free one for a new value
and frees it when the value is consumed. `rax` is taken first so
that the result of an expression statement, a return value and a
dividend tend to be where they are needed. The pinned instructions
of 3.1 and the argument registers of 3.2 are inside this set, and
chapter 6 and chapter 8 say how the generator moves around them.

**`rsp`.** The stack pointer, and nothing else.

## 5. The frame

A function's frame holds, from high addresses to low:

1. The return address, pushed by `call`.
2. The caller's `rbp`, pushed by the prologue; `rbp` then points at
   it, and everything below is addressed as a negative displacement
   off `rbp`.
3. The register-variable registers the function uses, pushed in the
   prologue, popped in reverse in the epilogue.
4. One slot per local and per parameter, each of its type's size at
   its type's alignment, in declaration order, every block's locals
   included, none shared: G12, "a local's storage is one stack slot
   for the whole activation". A parameter's slot receives the
   argument register in the prologue, so every later read is a load
   from the slot, as under the LLVM pipeline ("a parameter is written
   at entry"). A parameter that arrived on the stack has its slot
   where the caller put it, above the return address, at a positive
   displacement off `rbp`.
5. One slot per argument of each call the function makes, the hidden
   locals of chapter 8, allocated as the call is met.
6. The spill area for expression values when the nine registers are
   exhausted (chapter 6) and for values held across a call (chapter
   8), sized by the deepest expression in the function.

The slots are assigned as the generator meets them, each at the next
offset below the last; their total is the frame's size, written
into the prologue's `sub $N,%rsp` after the body is generated and
rounded up so that `rsp` is a multiple of 16 at every `call` the
function makes. A function that makes no call puts its slots in the
red zone and writes no `sub`: the ABI reserves the 128 bytes below
`rsp` for exactly this (`design-calls.md` chapter 5), and every leaf
pin of `tests/cost` shows the shape. The prologue is `push %rbp; mov
%rsp,%rbp`, then the pushes of 3, then the `sub` if any; the one
epilogue at `.Lret` pops the registers of 3 and ends in `leave;
.cfi_def_cfa %rsp, 8; ret` (`leave` is `mov %rbp,%rsp; pop %rbp`).
The prologue carries `.cfi_startproc`, `.cfi_def_cfa_offset 16`,
`.cfi_offset %rbp, -16` and `.cfi_def_cfa_register %rbp`, and the
function `.cfi_endproc`, so that an unwinder has the table and a
debugger that wants the chain has the chain. The directive after
`leave` is part of the table: `leave` restores the caller's `rbp`,
the canonical frame address is `rsp + 8` from there to `ret`, and
without the directive the table still computes it from `rbp`, which
is wrong between `leave` and `ret`.

## 6. Expressions

### 6.1 The rule

An expression is generated for its value into a register, left to
right, by one recursive walk of the tree. For a binary node: the
left child is generated into register `L`; the right child is
generated into register `R`; the instruction for the operator is
written as `op R, L`; `R` is freed; the result is `L`. Every
operator of the table is written exactly once per node, and
nothing looks at a node's neighbours.

The left-first order is G13 and is not negotiable for register
pressure: `f() + g()` calls `f` first. A commutative operator's
operands are not swapped either, so `4 * x` writes the 4 first; the
instruction is the same `imul` and the order of the listing is the
order of the source.

### 6.2 Depth and spill

The depth of an expression is the number of values held at once, the
Sethi–Ullman label under left-first evaluation: one plus the depth
of the right child when the left is a value that must be held, or
the depth of the left child, whichever is larger. When a value must
be held and no register is free, the oldest held value is stored to
the next spill slot of the frame and its register reused; when the
spilled value is consumed it is reloaded, all 8 bytes, into the
lowest free register, with `movq`. The same slots take a value held
across a call (chapter 8). The generator counts spill slots as it
goes, and the frame of chapter 5 is sized after the body is
generated. Nine registers hold any expression a programmer writes;
the path is exercised by one cost program of ten nested right
operands and otherwise never runs.

### 6.3 The table

The integer and pointer operators, for a 32-bit `int` (suffix `l`,
32-bit names) or a 64-bit `long` or pointer (suffix `q`, 64-bit
names); `char` and `short` are widened to `int` by the parser's
promotion casts before any operator sees them, as today.

| Source | Instruction | Note |
|---|---|---|
| a constant | an immediate operand, or `mov $c, L` when it must be held; `movabsq $c, L` for a 64-bit constant no 32-bit immediate holds | integer constant expressions folded in the parser, chapter 6.4 |
| a local, a parameter | `mov slot(%rbp), L` | a `char` is `movsbl` or `movzbl`, a `short` `movswl` or `movzwl` |
| a `register` variable | the register, as an operand | no instruction |
| a static | `mov sym(%rip), L` | PC-relative, chapter 10 |
| `a + b`, `a - b` | `add R, L`, `sub R, L` | G1: the machine wraps |
| `a * b` | `imul R, L` | G1; the same instruction for signed and unsigned, since the low bits agree |
| `a / b`, `a % b` | `mov L, %rax` if needed; `cltd` or `xor %edx,%edx`; `idiv R` or `div R`; the result from `rax` or `rdx` into `L` | G4; the pinned pair saved around if held, 6.5 |
| `a << b`, `a >> b` | `mov R, %rcx` if needed; `shl %cl, L`, `sar %cl, L`, `shr %cl, L`; by an immediate when `b` is a constant | G3: the instruction masks the count |
| `a & b`, `a \| b`, `a ^ b` | `and`, `or`, `xor` | |
| `-a`, `~a` | `neg L`, `not L` | G1 for `-INT_MIN` |
| `!a` | `cmp $0, L`; `sete %Lb`; `movzbl %Lb, L` | as a value; as a condition, chapter 7 |
| `a < b` and the rest, as a value | `cmp R, L`; `setcc %Lb`; `movzbl %Lb, L` | `setl`, `setle`, `setg`, `setge`, `sete`, `setne` signed; `setb`, `setbe`, `seta`, `setae` unsigned and for pointers (G7) |
| `a && b`, `a \|\| b`, `a ? b : c`, as a value | jump code of chapter 7 into `L`, with `mov $0, L` and `mov $1, L` on the arms | no `setcc` chain |
| `x = e` | `mov L, slot` or `mov L, reg`; the value stays in `L` | a `char` store is `movb %Lb` |
| `x op= e` | the load row, the op row, the store row | |
| `++x`, `x++`, `--x`, `x--` | the load, `add $1` or `sub $1`, the store; the post forms keep the old value in another register | `add $size` for a pointer |
| `(T) a` | `movslq L, L64` for `int` to `long`, `movsbl` or `movzbl` from `Lb` for `char` to `int`, each only when `L` is not already valid to the new width (the width rule below); nothing for `long` to `int` (the 32-bit name is the value); nothing between pointer types | G2: truncation is reading the low bits |
| `&x` | `lea slot(%rbp), L` or `lea sym(%rip), L` | refused on a `register` variable, 6.5.4.2p1 |
| `*p` | `mov (L), L` | G8, G9, G10, G11: one `mov`, any address |
| `p[i]` | `movslq` of the index; `mov (L,R,s), L` for a size of 1, 2, 4 or 8 | one instruction, the machine's addressing form; a constant index is a displacement |
| `p + n`, `p - n` | `movslq` of `n`; `lea (L,R,s), L` or `imul $size, R; add R, L` | G6; `sub` for `p - n` |
| `p - q` | `sub R, L`; `cqto`; `idiv` by the size | G6: a division, not a shift, since any two pointers |
| `f(a, b)` | chapter 8 | |
| `sizeof` | an immediate | folded in the parser |
| `"abc"` | `lea .Ls3(%rip), L` | the bytes in `.data`, chapter 10 |

A `char` or `short` lvalue is loaded with the extending `mov` of its
signedness and stored with the narrow `mov`; that is the whole of
narrowing and widening on x86, where under the IR pipeline it was
`trunc`, `sext` and `zext` nodes.

The width rule. Each held value carries a width: the width to which
its register's bits are the value extended by its type's
signedness. The instruction that wrote the register sets it:

- a load of a `char` or `short` leaves it valid to 32;
- a 32-bit write clears the upper half, which is the extension of an
  unsigned value and not of a signed one, so an `unsigned` written
  at 32 bits is valid to 64 and an `int` to 32;
- a narrowing cast writes nothing and leaves the bits above its
  width stale;
- a call's result is valid to 32: the callee extends a narrow
  result, and above 32 bits the convention promises nothing.

A widening cast writes its `movs` or `movz` only when the value is
not already valid to the new width: `(int) c` after the load of `c`
writes nothing, `(long) i` is `movslq`. A `return` of a `char` or a
`short` extends `eax` by the same test, so `return c / 4 + s` in a
function returning `char` ends in `movsbl %al, %eax`. This is
knowledge of what the generator's own instruction left in the
register, not of the program's data. The PDP-11 compiler had the
same knowledge built in: its `movb` into a register sign-extended,
and a `char` to `int` conversion of a register value cost nothing.

`&&`, `||` and `?:` evaluated for a value spill every held value
first, as a call does (chapter 8). Each has two paths into one
register, and with nothing held, both paths leave the register map
as they found it, whatever their operands do.

### 6.4 Constants

The parser folds integer constant expressions: the operators 6.6p3
does not exclude, applied to the operands 6.6p8 allows, which are
integer constants, character constants, `sizeof` and casts to
integer types, with the type the expression has in C and the value
the instruction would compute: a signed result the type cannot hold
wraps (G1), a shift count is masked (G3), and a division with no
value, by zero or of the most negative value by -1, is left unfolded
for the instruction to trap on (G4), so folding changes no program.
6.6p4, which makes an out-of-range constant expression a constraint
violation, governs where the grammar requires one, the size of an
array, and the wrapped value is refused there. A variable never
folds: `x * 1` multiplies, `x - x` subtracts, `x + 0` adds. The rule "the instruction written is the
instruction executed" is about operations on the program's data; a
constant expression is the spelling of a number, which ISO C says
"can be evaluated during translation" (6.6p2) and which Ritchie's
`c0` evaluated. So `-1` is the immediate `$-1`, `p[-1]` is the
displacement `-4(L)`, and `int a[96 * 96]` has 9216 elements. This
closes findings 3 and 4 of `findings.md`, and the evaluator is not
an addition: the size of an array (6.7.7.3p4), the label of a `case`
(6.8.5.3p3) and every expression in a static initializer (6.7.11p5)
require one, and the last of these cannot compile without it, since
there is no instruction to compute a static's initial bytes at
(`design-calls.md` chapter 4).

A constant operand of an arithmetic instruction is an immediate when
the instruction has an immediate form, which on x86-64 every one of
the table does but `idiv`, `div` and the variable shift: `x * 4` is
`imul $4, L, L`, `x + 1` is `add $1, L`, and `x / 4` is `mov $4, R;
cltd; idiv R`. The multiply stays a multiply, the division a
division, however the constant factors: there is no code in the
table that looks at a constant's value.

### 6.5 The pinned instructions

A division needs its dividend in `rax` and `rdx` free; a variable
shift needs its count in `cl`. The generator is holding values in
the expression registers, possibly in those three. The rule: before
the pinned instruction, each of the pinned registers that holds a
live value other than the operand is exchanged with a free register
(`xchg`, or a `mov` to a free one), the operands are moved into
place, the instruction is written, the result is moved into `L`, and
the displaced values stay where they went; the generator's map of
which value is in which register is updated and nothing else cares.
Every instruction written is a real move of a real value and shows
in the listing; the cost model's division row will say what a
division costs when `rax` or `rdx` is busy.

## 7. Conditions and control

### 7.1 Jump code

`cbranch(e, label, sense)` writes code that jumps to `label` when
`e` is true (sense 1) or false (sense 0), and falls through
otherwise, without producing a value:

- `a < b` and the rest: `L` and `R` as in chapter 6, `cmp R, L`,
  then `jcc label` with the condition or its inverse by `sense`;
  `jl`, `jge` and the rest signed, `jb`, `jae` and the rest unsigned
  and for pointers.
- `!e`: `cbranch(e, label, !sense)`.
- `a && b`, jump on false: `cbranch(a, label, 0); cbranch(b, label,
  0)`. Jump on true: `cbranch(a, skip, 0); cbranch(b, label, 1);
  skip:`. `||` is the mirror. Short circuit is the shape of the code;
  the second operand is not evaluated when the first decides (6.5.14p4,
  6.5.15p4).
- Any other expression: its value into `L` by chapter 6, `cmp $0, L`
  (or `test L, L`, the same flags), `jne` or `je`. A pointer compares
  against `$0` the same way (G9 has nothing to say about a test).
- The constants 0 and 1 as conditions, after folding: `if (0)`
  writes `cmp $0` of an immediate loaded into `L` and the jump, as
  the `zero-test` row pins today. The code for a constant condition
  is the code for a condition; nothing looks at the value.

A condition is tested at its operand's own width: `if (c)` on a
`char` is `cmpb $0, %al`. The standard's test is that the operand
"compares unequal to 0" (6.8.5.2p2 for `if`, 6.5.14p3 for `&&`,
6.5.15p3 for `||`), and it names no promotion. `!E` is defined as
`(0==E)` (6.5.4.3p5), which does promote, since `==` converts its
operands. The parser does not insert that promotion yet, and c1
compares `!c` at the `char`'s width too; the result is the same, and
the promotion is queued.

### 7.2 Statements

- `if (e) s1 else s2`: `cbranch(e, .Lelse, 0); s1; jmp .Lend;
  .Lelse: s2; .Lend:`. Without `else`, no `jmp` and one label.
- `while (e) s`: `.Lcond: cbranch(e, .Lend, 0); s; jmp .Lcond;
  .Lend:`. The condition is at the top, where it is written, and
  tested once per iteration plus once; no rotation, no duplicated
  test at the bottom.
- `for (i; e; n) s`: `i; .Lcond: cbranch(e, .Lend, 0); s; .Lstep: n;
  jmp .Lcond; .Lend:`. `continue` will jump to `.Lstep`, `break` to
  `.Lend`.
- `e ? a : b` for its value: `cbranch(e, .Lelse, 0)`, `a` into `L`,
  `jmp .Lend`, `.Lelse:` `b` into the same `L`, `.Lend:`. Never
  `cmov`: the arm not chosen is not evaluated (6.5.16p5), and the
  jump says so.
- `return e`: `e` into `L`, `mov L, %eax` if `L` is not `rax`, `jmp
  .Lret`, where `.Lret` is the one epilogue (a name in this
  document; the listing numbers it `.L<n>`, the first label a
  function takes). `return` jumps to the epilogue every time, the
  last statement of the body included: the
  jump to the next instruction is what the statement says, and
  removing it was the optional peephole pass's (`c2`) work on the
  PDP-11, which rcc has no counterpart of. A fall off the end of a
  non-`void` function reaches `.Lret` with `rax` as it is, which is
  G5's "some value of its type"; the `%ret.indeterminate` slot of the
  IR pipeline is gone. `main` is the exception: reaching its closing
  `}` returns 0 (5.1.2.3.4p1), so `main` writes `movl $0, %eax`
  before the epilogue unless its last statement is a `return`.
- Blocks and labels are written in source order. A label is `.L`
  and a function-local number; the labels of a function are unique
  and nothing is reordered after them.

## 8. Calls

`f(a1, ..., an)`, with the arguments evaluated left to right (G13).
Every argument is a hidden local of the call: a slot of its own in
the frame, written as the argument is evaluated and read back by the
call sequence.

1. Every expression register holding a value of the enclosing
   expression (`x` in `x + f(a, b)`) is stored to a spill slot; the
   register is freed. Nothing is held in a register from here to the
   `call`.
2. Each argument in turn is evaluated into a register by chapter 6,
   as the initializer of a local would be, and stored to its argument
   slot, `mov L, -24(%rbp)`. Arguments are evaluated in source order,
   each complete before the next begins, and a call inside an
   argument finds every expression register free and clobbers
   nothing that matters.
3. The call sequence. If the arguments past the sixth are an odd
   number, `sub $8,%rsp`, so that the `call` lands on a multiple of
   16. The arguments past the sixth are pushed from their slots right
   to left, so that the seventh is lowest, as the ABI wants (3.2.3):
   an 8-byte slot in one instruction, `pushq -40(%rbp)`. A slot
   narrower than 8 bytes is not pushed directly, since `pushq` would
   read the bytes above it: it is loaded, extended, into `eax` and
   pushed from `rax`, `movl -28(%rbp), %eax; pushq %rax`. The first
   six are loaded from their slots into `rdi`, `rsi`, `rdx`, `rcx`,
   `r8`, `r9`, a narrower one extended to 32 bits by the load. `al`
   is set to 0 for a variadic callee. `call f@PLT`, every call
   through the PLT, chapter 10. The pushed bytes and the pad are
   added back.
4. The result is in `rax`; it is moved into the lowest free
   expression register, and the values of step 1 are reloaded from
   their spill slots as the enclosing expression consumes them.

The shape costs one store and one load per argument, the same count
as a push and a pop, and it is the model applied to arguments: a
value the programmer did not put in a register lives in memory, and
the listing shows it. It was chosen over pushes, which put the
eighth argument below the seventh, and over a direct move into the
register, which needs a look-ahead and a parallel move
(`design-calls.md` chapter 6). A direct path for an argument that is
a variable or a constant can be added as a measured step if the
measurement says the store and the load matter.

A `char` or `short` argument to a parameter of that type keeps the
parameter's type: it is stored to its slot at its width and extended
to 32 bits by the load into its register (`movsbl -5(%rbp), %edi`).
A `char` or `short` result is extended by the callee before `ret`,
as the ABI's "the callee extends" and the IR pipeline's row both
say, and the caller reads 32 bits of it. G16: the call is made
with the call site's types and the convention's registers, and
nothing is added, dropped or rewritten.

The callee's prologue: `push %rbp; mov %rsp,%rbp`, push the
register-variable registers it uses, `sub` its frame unless it is a
leaf in the red zone, store each parameter's argument register to its
slot or move it to its callee-saved register if `register`. The
epilogue at `.Lret`: pop the registers, `leave`, the directive of
chapter 5, `ret`.

## 9. What each guarantee compiles to

The "IR:" clause of each guarantee in `guarantees.md` becomes an
instruction clause when the back end lands. The table here is the
draft of those clauses.

| Guarantee | Instruction |
|---|---|
| G1 signed arithmetic wraps | `add`, `sub`, `imul`, `shl`, `neg`: the machine computes modulo 2^N |
| G2 narrowing truncates | a narrow store `movb`, `movw`, `movl`, or the narrower register name; nothing is computed |
| G3 shifts | `shl`, `sar`, `shr` by `cl` or an immediate: the instruction masks the count to the width |
| G4 division is the instruction's | `idiv`, `div`: `#DE` on a zero divisor and on `INT_MIN / -1`, by the machine |
| G5 an uninitialized object holds a value | a slot's bytes, or a register's content, unwritten until the program writes; nothing in the generator touches either |
| G6 pointer arithmetic is address arithmetic | `lea`, `add`, `sub` on 64-bit registers; `p - q` is `sub`, `cqto`, `idiv` |
| G7 pointer comparison is address comparison | `cmp` of 64-bit registers, the unsigned `jcc` and `setcc` |
| G8 a load reads the bytes at the address | `mov (L), L`, whatever computed `L` |
| G9 null access at address 0 | the same `mov`; the machine faults or reads page zero |
| G10 memory is bytes | the same `mov` with the lvalue's width and extension |
| G11 misaligned access performed | the same `mov`; x86 has one `mov` for every alignment |
| G12 one slot for the activation | chapter 5: every local's slot, none shared, none reused within the call |
| G13 left to right | chapter 6's rule and chapter 8's order |
| G14 a loop runs until its condition says stop | chapter 7's `while`: a test and a jump, and no code that reasons about the test |
| G16 a call goes out as written | chapter 8 |
| G17 a string literal is writable | `.data`, chapter 10 |

G15 stays withdrawn. The never-emitted table of `guarantees.md`,
the IR flags the front end withheld from an optimizer that never
ran, has no counterpart: there is no IR, and there is nothing to
withhold from `as`.

## 10. Statics and literals

A translation unit's output is one assembly file with a `.text`
section for its functions and a `.data` section for its objects
with static storage. A string literal is a labeled array of bytes in
`.data`, one label per literal written, never merged, never in
`.rodata`: G17. Its address is `lea .Ls3(%rip), L`. A static
variable is a labeled area in `.data` or `.bss` with its initializer
or its zeros, reached the same way. A function is `.globl` and
`.type f, @function` unless `static`, and ends with `.size`.

Every address is PC-relative (`sym(%rip)`) and every external call
goes through the PLT, so the object links into a position-independent
executable, which is the default link on the platforms rcc runs on,
and into a fixed one as well.

## 11. The driver and the pipeline

`rcc file.c` runs the system `cpp` (or `cc -E`), tokenizes, parses,
folds, generates assembly text, and then runs `cc -c` on it for `-c`
and `cc` for a link. `-S` writes the assembly, to a file or to
standard output for `-o -`, as every cc does. No `llc`, no IR, no
flag table. The `# line "file"` markers of the preprocessor still
give the diagnostics their positions.

Until the LLVM pipeline is retired, the driver's `-native` selects
c1: `rcc -native -S` writes the assembly, `-native -c` assembles it
with the system cc, and a link links it. Without the flag the LLVM
path runs as before.

`shape.md`'s chapters on the pipeline, the emitter and the driver
are rewritten when this lands; the lexer, the types, the tree and
the parser, 3300 of the tree's 5700 lines, are untouched.

## 12. Verification

- **The oracle**, `tests/oracle` over `tests/corpus`: the same
  program under `clang -O2` and under `rcc`, exit statuses compared.
  Unchanged in script and in expectation; the corpus's `.ll` pins
  become `.s` pins of `rcc -S`.
- **The cost corpus**, `tests/costs` over `tests/cost`: the pin of
  each program becomes the text `rcc -S` writes, and the script
  assembles it with `cc -c` to prove that it assembles. No `objdump`,
  no relocations to strip. The rows are re-pinned one by one against
  this document's table, and the document's chapter 3 of `cost.md`
  is rewritten from the pins.
- **Where the pins live**: until retirement, the `.s` pins are under
  `tests/corpus/native/` and `tests/cost/native/`, written by
  `tests/oracle --native` and `tests/costs --native`, which `make
  test-native` runs; the LLVM pins keep their places, and `make
  test` runs them. At retirement the native pins move up a
  directory and replace them.
- **The ABI**: a corpus program that calls into libc (`puts`,
  `strlen`, `printf` with `al` set) and one that is called from a
  `clang`-compiled `main`, exit status compared. New, and the first
  thing to pass.
- **The measurement**, `tests/bench`: redone. The tuned programs
  declare more `register` variables than five in `main` (matmul ten,
  collatz seven, sieve six, sort five); the sixth and later take
  slots, the numbers move, and the movement is recorded as a result
  about the model. Then the tuned sources may be re-tuned to five, as
  a PDP-11 programmer tuned to three, and the chapter re-recorded
  with the register count per program.

## 13. The calls

Six were open in the first draft of this page. `design-calls.md`
weighs the first five one chapter each; the answers:

1. **`rbp`** is the frame pointer in every function, and the
   register variables are five. A slot's offset is known at its
   declaration, the prologue is one shape, the debugger needs
   nothing from us; the sixth register was worth one variable in one
   program of six.
2. **The sixth `register` variable** takes a slot, with a warning by
   default. The extent is implementation-defined (6.7.2p12) and
   documented (4p10); a lever that silently does nothing is not a
   lever.
3. **Integer constant expressions are folded** in c0, constants
   only. The evaluator is required regardless by the array size, the
   `case` label and the static initializer.
4. **A leaf keeps its slots in the red zone** and writes no `sub`;
   the ABI reserves the 128 bytes for it, and every leaf pin shows
   the shape.
5. **Every argument is a hidden local of its call**, stored to a slot
   as evaluated; the call sequence pushes the stack arguments from
   their slots right to left and loads the first six. The first
   draft's pushes put the eighth argument below the seventh.
6. **The names.** The front end is c0, the generator is c1:
   `src/c1.cc` and `src/c1.hh`.
