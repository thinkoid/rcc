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
  that register; one whose register would be a sixth takes a slot and
  a warning.
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
- The oracle is unchanged. The cost pins are the assembly rcc
  writes. The measurement is redone under the register discipline.
- LLVM left the tree. Its one lasting contribution is `llvm.md` and
  `findings.md`: the written record of where a modern code
  generator's judgment hides, which this design was drawn against.

## 1. Why

`cost.md` held LLVM's code generator to a literal pipeline with
twenty-one switches, the flag table that `llvm.md` 3.4 now records.
`findings.md` records what the switches did not reach: the selector's
own strength reduction, a switchless pass, an allocator that
rematerializes a constant or a frame address wherever it likes. Each is
small. Together they say the tool has a judgment of its own that no
option removes, because no one who wrote it wanted a literal compiler.
Defending the thesis against the tool is a maintenance stream that grows
with every LLVM release; writing the back end makes the thesis a table.

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
operator, but its `c1` made an exception of a constant power of two.
`x * 4` was a shift (`pow2()`, in `c11.c`), and `x / 4` and `x % 4`
on an `unsigned` were a shift and an `and` (`optim()`, in `c12.c`).
rcc is stricter than its ancestor here: the instruction is the
operator's, and `x * 4` multiplies (6.4). `ancestor.md` chapter 2
has what else that `c1` rewrote.

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
condition and the IR had to materialize it: that pipeline's cost
row said "used as a condition, the value is then compared with 0
again", the `setcc`, `and $1`, `movzbl`, `cmp $0` chain that was 17
of the 23 instructions in sort's inner loop (`llvm.md` 5.5). Chapter
7 is `cbranch`.

## 3. Background: x86-64 and its convention

### 3.1 The registers

Sixteen 64-bit general registers: `rax`, `rbx`, `rcx`, `rdx`,
`rsi`, `rdi`, `rbp`, `rsp`, `r8` to `r15`. Each has a 32-bit name
(`eax`, `r8d`), a 16-bit name (`ax`, `r8w`) and an 8-bit name (`al`,
`sil`, `r8b`); a write to the 32-bit name clears the upper half, a
write to a narrower name does not. `rsp` is the stack pointer and is
never anything else. The flags register describes the result of the
last instruction that sets it, and the conditional instructions,
`jcc`, `setcc`, read it. `cmp` and `test` set it and write no
result; `add`, `sub`, `and`, `or`, `xor` and `neg` set it from the
result they write. `imul` and the divisions leave ZF undefined, a
shift by a count of 0 leaves the flags as they were, and `mov`,
`movsx`, `movzx`, `lea` and `not` set none (7.1).

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
  and the ABI leaves the upper bits unspecified; the callee extends.
- The result returns in `rax` (`eax` for a 32-bit type). A `char` or
  `short` result is in the register's low bits, and the ABI leaves
  the bits above it unspecified, as for an argument; the caller
  extends. `gcc` and `clang` write nothing before `ret` and extend
  after the `call`, `movsbl %al, %eax`.
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
has released, so sibling blocks share. A later declaration of the
enclosing block does not take a closed inner block's register: its
object is live from the entry into its block (6.2.4p6), so through
the inner block, and a `goto` back into that block would write the
inner variable's register while the later one holds its value. It
takes the register above those live and above every one the closed
inner blocks of its block took. A `register` variable whose register
would be past the five gets a slot and is an ordinary local, as
6.7.2p12 allows
("the extent to which such suggestions are effective is
implementation-defined"), and as the PDP-11 did with the fourth; the
compiler says so with a warning, `register variable 'k' in a stack
slot: the five registers are taken`, which a conforming program may
receive and `-w` silences. The extent is recorded in
the cost model, as 4p10 asks (`design-calls.md` chapter 3).

A `register` parameter is moved from its argument register to its
callee-saved register in the prologue, one `mov`; the argument
register is caller-saved and would not survive the first call. A
`register` parameter past the sixth arrived on the stack and is
loaded from its slot above the return address into its register. The
moves and the ordinary parameters' stores are written in parameter
order.

A register variable is nothing but its register. No slot, no load,
no store, no `phi`, no join of arms, no patching of a loop's back
edge: an assignment to it is a `mov` into the register, a read of it
is the register as an operand. G5 for a register variable needs no
unwritten slot any more. Before its first write the register holds
what it held when the declaration was reached: what the prologue
left in it for the first variable to take it, and what the last
variable to hold it left for a later one. Nothing writes the
register until the program does, so every read before the first
write sees the same value. The one exception is an unwritten `char`,
`short` or `unsigned` variable: the representation below claims more
bits than the type has, so its declaration extends the register in
place, `movsbl %bl, %ebx` for a `char`. The value of the type, in the
low bits, is unchanged. The function's epilogue restores every register
of the set the function used, from the pushes of its prologue.

The register holds the variable's value extended by its signedness
to 32 bits, by the width rule of chapter 6.3: an `unsigned` written
at 32 bits is valid to 64, and a `long` or a pointer is the register
whole. A `register char` is thus its `movsbl`-extended value, as the
PDP-11's `movb` into a register sign-extended.

A read is the register as an operand where the instruction only
reads it: `cmpl %ebx, %r13d`. Where the instruction would write its
destination, the register is first copied into an expression
register, `movl %ebx, %eax; subl %r12d, %eax`, so the variable does
not change. An assignment is the `mov` into the register: a constant
is the immediate, `movl $0, %r12d`, and a narrow store is the
extending `mov`, `movsbl %al, %ebx`. A compound assignment and `++`
or `--` operate on the register in place, `addl $1, %r13d`, and a
narrow one is re-extended after. A compound assignment generates its
right operand first and then operates on the register, so no read of
the variable is held across the right operand: `r += (r = 2)` is
`movl $2, %ebx; addl %ebx, %ebx`.

A read still held when its variable is written would see the new
value, but G13 promises the old one. So every held read of the
variable is copied into an expression register before the write;
`i + i++` thus adds the old `i` to itself. A branch and a call ask
more than the copy, and 6.7 says what.

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
chapter 6 and chapter 8 say how the generator chooses around them,
and moves a value when it cannot.

**`rsp`.** The stack pointer, and nothing else.

## 5. The frame

A function's frame holds, from high addresses to low:

1. The return address, pushed by `call`.
2. The caller's `rbp`, pushed by the prologue; `rbp` then points at
   it, and everything below is addressed as a negative displacement
   off `rbp`.
3. The register-variable registers the function uses, pushed in the
   prologue, popped in reverse in the epilogue. Each push carries its
   place in the unwind table, `.cfi_offset %rbx, -24` for the first
   and 8 lower for each next. The registers pushed are those the
   function's register variables take by chapter 4, its register
   parameters among them: the count the parser records with the
   definition, known before the body is generated, since the slots
   lie below them.
4. One slot per local and per parameter, each of its type's size at
   its type's alignment, in declaration order, every block's locals
   included, none shared: G12, "a local's storage is one stack slot
   for the whole activation". A parameter's slot receives the
   argument register in the prologue, so every later read is a load
   from the slot. A parameter that arrived on the stack has its slot
   where the caller put it, above the return address, at a positive
   displacement off `rbp`. A local array's slot is its size at its
   element's alignment, its displacement the array's lowest address;
   the ABI's alignment of 16 for an array of 16 bytes or more is not
   applied, since nothing rcc writes relies on it.
5. One slot per argument of each call the function makes, the hidden
   locals of chapter 8, allocated as the call is met.
6. The spill area for expression values when the nine registers are
   exhausted (chapter 6) and for values held across a call (chapter
   8), sized by the deepest expression in the function.

The slots are assigned as the generator meets them, each at the next
offset below the last; the first lies below the pushes, at
`-8k - size` with `k` registers pushed. Their total is the frame's size,
written into the prologue's `sub $N,%rsp` after the body is generated
and rounded up so that `rsp` is a multiple of 16 at every `call` the
function makes: `8k + N` is a multiple of 16. A function that makes
no call puts a frame of up to 128 bytes in the red zone and writes no
`sub`; the 128 are measured from `rsp` after the pushes, so a leaf's
pushes do not count against them. The ABI reserves the
128 bytes below `rsp` for exactly this (`design-calls.md` chapter
5), and every leaf pin of `tests/cost` shows the shape. A leaf with
a larger frame writes the `sub`, as any other function does. The
prologue is `push %rbp; mov %rsp,%rbp`, then the pushes of 3, then
the `sub` if any; the one epilogue at `.Lret` writes `leaq
-8k(%rbp), %rsp` when the prologue wrote a `sub`, so that `rsp` is
back at the pushes, then pops the registers of 3 and ends in `leave;
.cfi_def_cfa %rsp, 8; ret` (`leave` is `mov %rbp,%rsp; pop %rbp`).
The prologue carries `.cfi_startproc`, `.cfi_def_cfa_offset 16`,
`.cfi_offset %rbp, -16` and `.cfi_def_cfa_register %rbp`, and the
function `.cfi_endproc`, so that an unwinder has the table and a
debugger that wants the chain has the chain. The directive after
`leave` is part of the table: `leave` restores the caller's `rbp`,
the canonical frame address is `rsp + 8` from there to `ret`, and
without the directive the table still computes it from `rbp`, which
is wrong between `leave` and `ret`.

A frame is at most 2147483632 bytes, 2 GiB less 16, since a slot is
a 32-bit displacement off `rbp` and the frame, a multiple of 16, is
the 32-bit immediate of `sub`. The parser refuses a single local over
the limit at its declaration, with its position. A sum of smaller
locals that passes the limit is refused by c1, which has no position
to give.

## 6. Expressions

### 6.1 The rule

An expression is generated for its value into a register, left to
right, by one recursive walk of the tree. For a binary node: the
left child is generated into register `L`; the right child is
generated as the instruction's source operand `R`, in one of three
forms: the immediate of a constant the instruction takes (6.4), the
operand of an object in memory (below), or a register, into which
it is generated; the instruction for the operator is written as
`op R, L`; `R` is freed; the result is `L`. Every operator of the
table is written exactly once per node, and nothing looks at a
node's neighbours, with one exception. A node whose right operand's
top operator is a division or a shift by a variable count generates
its left operand outside the registers that instruction pins (6.5).
The machine pins those registers, and what the node chooses is a
register's name, not the order of evaluation.

The left-first order is G13 and is not negotiable for register
pressure: `f() + g()` calls `f` first. A commutative operator's
operands are not swapped either, so `4 * x` writes the 4 first; the
instruction is the same `imul` and the order of the listing is the
order of the source.

**An object in memory as the operand.** A right operand that is the
read of an object in memory, a name that is no register variable or
a dereference, of exactly the operation's width, is no value in a
register: the instruction names the object. Its operand is any form
the load row reads: a slot, a label off `rip`, a base with a
displacement, an index (6.3). `x + y` on two locals is `movl
-4(%rbp), %eax; addl -8(%rbp), %eax`, and `x / d` is `movl -4(%rbp),
%eax; cltd; idivl -8(%rbp)`. The rows that take the form are `+`,
`-`, `&`, `|`, `^`, `*`, `/` and `%` (6.3, 6.5), the comparisons
(6.3, 7.1) and `&` as a condition, `testl -8(%rbp), %eax` (7.1).

The reads keep G13's order. The left operand is read first; the
right operand's address is generated where its load would be, after
the left operand, and the instruction reads the object, with nothing
evaluated between. The address's registers are held to the
instruction as any held value is (6.7). The instruction reads the
bytes the load would read, in one access at the object's width (G10,
`shape.md` chapter 5). So a `char` or a `short`, which a cast
promotes, keeps its extending load; an `int` widened to 64 bits keeps
the extending load of 6.3; a narrowing cast keeps its row; and no
object is read wider or narrower than its load would read it.

Some operands keep their register. A shift's count goes to `cl`
(6.5). A compound assignment's right operand is generated before the
object is read, `x += e` being `e`, then the read of `x` (G13), and
x86 has no instruction with two memory operands, so `e` keeps its
register in every context. A call's argument is a hidden local
(chapter 8). `x * 4` stays `imul $4, L, L` after `x`'s load: the
three-operand form with a memory source, `imull $4, -4(%rbp), %eax`,
is not taken. A register variable as the right operand is its
register (chapter 4), and a global the file only declares is reached
through the register its address is loaded into, the GOT's load
staying, `addl (%rcx), %eax` (chapter 10).

The rule is the Seventh Edition compiler's. Its table's row `%n,aw`
in `cr40`, the rows of `+`, `-`, `|`, `&~` and `<<`, takes any left
operand and a right operand of the class `a`, addressable, and the
type `w`, a word; it writes `F`, the left operand into the register,
then `IB2 A2,R`, the instruction with the right operand's address,
`add _y,r0`. `cr42`, the multiply, takes the row whole, and `cr43`,
the division and the remainder, has its own: `F`, then `T`, the sign
extension of the register pair, then `I A2,R-`, the `div` of the
right operand's address. `cc60`, the relationals under the condition
codes, takes it for a compare. The class `a` is the degree 16, as
the table's translator, `cvopt.c`, writes the letter, and an operand
is of it when `dcalc()`, in `c11.c`, gives it a degree of 16 or less:
a name, a register, a constant, `&x` and an indirection through a
name are 12 or less. `notcompat()` admits as a `w` an `int`, an
`unsigned` or a pointer and no `char`.

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
promotion casts before any operator sees them, as today. `R` is the
right operand in one of the three forms of 6.1, and where it is an
object in memory the row's instruction names it and no load row is
written for it.

| Source | Instruction | Note |
|---|---|---|
| a constant | an immediate operand, or `mov $c, L` when it must be held; `movabsq $c, L` for a 64-bit constant no 32-bit immediate holds | integer constant expressions folded in the parser, chapter 6.4 |
| a local, a parameter | `mov slot(%rbp), L`; as a right operand of the operation's width, `slot(%rbp)` itself (6.1) | a `char` is `movsbl` or `movzbl`, a `short` `movswl` or `movzwl`; a signed object under a cast to 64 bits is the extending load of the row of `(T) a`, `movslq`, `movsbq` or `movswq` |
| a `register` variable | the register, as an operand the instruction reads; copied into `L`, `mov reg, L`, where the instruction writes `L` | no instruction for a read |
| a global the file defines | `mov sym(%rip), L`; `sym(%rip)` wherever a row writes a local's `slot(%rbp)`: the store, `mov L, sym(%rip)`, and the memory forms of 6.6, `addl $1, sym(%rip)`, and the right operand of 6.1, `addl sym(%rip), %eax` | PC-relative, chapter 10; a char is `movsbl` or `movzbl`, a short `movswl` or `movzwl`, and a signed object under a cast to 64 bits the row of `(T) a`, `movslq sym(%rip), L64` |
| a global the file only declares | `movq sym@GOTPCREL(%rip), L`, its address from the GOT, then the row of `*p` through `L`: `movl (L), L`, `movl R, (L)`, `addl $1, (L)`, and `addl (L), %eax` as a right operand | chapter 10; each access the source writes loads the address once, so `n = n + 1` loads it twice |
| `a + b`, `a - b` | `add R, L`, `sub R, L`; an object in memory as `R`, `addl -8(%rbp), %eax` (6.1) | G1: the machine wraps |
| `a * b` | `imul R, L`, `imull -8(%rbp), %eax` for an object in memory | G1; the same instruction for signed and unsigned, since the low bits agree |
| `a / b`, `a % b` | `mov L, %rax` if needed; `cltd` or `xor %edx,%edx`; `idiv R` or `div R`, `idivl -8(%rbp)` for an object in memory; the result from `rax` or `rdx` into `L` | G4; the pinned pair saved around if held, and the registers of a divisor's address moved out of it, 6.5 |
| `a << b`, `a >> b` | `mov R, %rcx` if needed; `shl %cl, L`, `sar %cl, L`, `shr %cl, L`; by an immediate when `b` is a constant | G3: the instruction masks the count |
| `a & b`, `a \| b`, `a ^ b` | `and`, `or`, `xor`, an object in memory as `R` as for `add` | |
| `-a`, `~a` | `neg L`, `not L` | G1 for `-INT_MIN` |
| `!a` | `cmp $0, L`; `sete %Lb`; `movzbl %Lb, L` | as a value; as a condition, chapter 7 |
| `a < b` and the rest, as a value | `cmp R, L`, `cmpl -8(%rbp), %eax` for an object in memory; `setcc %Lb`; `movzbl %Lb, L` | `setl`, `setle`, `setg`, `setge`, `sete`, `setne` signed; `setb`, `setbe`, `seta`, `setae` unsigned and for pointers (G7) |
| `a && b`, `a \|\| b`, `a ? b : c`, as a value | jump code of chapter 7 into `L`, with `mov $0, L` and `mov $1, L` on the arms | no `setcc` chain |
| `x = e` | `mov L, slot` or `mov L, reg`, the value staying in `L`; a constant goes as the immediate straight into a register variable, `movl $0, %r12d`, or into memory when the assignment is evaluated for its effect alone, `movl $0, -12(%rbp)` (6.6) | a `char` store is `movb %Lb` |
| `x op= e` | the address of `x` first, then `e`; in the effect context, the op on the object, `addl %eax, -12(%rbp)`, but for `*=`, `/=` and `%=` (6.6); in a value context, the load row, the op row, the store row, after `e`; on a register variable, the op on the register in place, after `e` | `*=`, `/=` and `%=` are always `e`, then the load, the op and the store: `imul` has no memory destination and `idiv` none at all |
| `++x`, `x++`, `--x`, `x--` | in the effect context, the op on the object, `addl $1, -8(%rbp)`, and no old value (6.6); in a value context, the load, `add $1` or `sub $1`, the store, the post forms keeping the old value in another register | `add $size` for a pointer |
| `(T) a` | `movslq L, L64` for `int` to `long`, `movsbl` or `movzbl` from `Lb` for `char` to `int`, each only when `L` is not already valid to the new width (the width rule below); a signed object in memory under a cast that widens it is its load extended to the new width, `movslq -4(%rbp), L64` for an `int` to `long`, `movsbq` and `movswq` for a `signed char` and a `short`, and no instruction after it (a widening by the load, below); nothing for `long` to `int` (the 32-bit name is the value); nothing between pointer types; an integer to a pointer as to `unsigned long`, so `movslq` for an `int` (6.3.2.3p5); nothing for a pointer to `long` or `int` | G2: truncation is reading the low bits |
| `&x` | `lea slot(%rbp), L` or `lea sym(%rip), L`; for a global the file only declares, the load from the GOT, `movq sym@GOTPCREL(%rip), L`; `&x` plus a constant is one `lea`, the name and constant below | refused on a `register` variable, 6.5.4.2p1 |
| `*p` | `mov (L), L` at the pointee's width and extension: `movsbl`, `movzbl`, `movswl`, `movzwl`, `movl`, `movq`; under a cast that widens a signed pointee to 64 bits, `movsbq`, `movswq`, `movslq` | G8, G9, G10, G11: one `mov`, any address |
| `p[i]`, `a[i]` | `mov (L,R,s), L` for a size of 1, 2, 4 or 8, the pointer in `L` (`lea sym(%rip), L` for a global array the file defines, the GOT's load for one it declares), the index in `R`; on a local array the slot is the displacement and `rbp` the base, `movl -24(%rbp,R,4), L`, and no instruction forms the array's address (a slot and an index, below); a constant index is the displacement `c*s(L)`, and on an array reached by its name the name and constant below, the slot's or the label's own operand, `movl -20(%rbp), L`, `movl table+12(%rip), L` | one instruction, the machine's addressing form; the index is 64 bits by the parser's cast, written by the row of `(T) a`: an `int` in memory is its load, `movslq -4(%rbp), R64`, one in a register `movslq R, R64`, and nothing is written for an `unsigned` or a `long` |
| `p[i]` of another size | the `p + n` row, and the sum is the base `L` of the access; a constant index of 0 adds nothing, the bare base of the `&p[i]` row; on an array reached by its name a constant index is the name and constant below, so `m[1][2]` is one operand, and through a pointer a constant index under a constant is one displacement, `movl 20(L), L` for `p[1][2]` on `int (*p)[3]` | only an array has such a size: `m[i][j]` on `int m[2][3]` is `imul $12`, `add`, then `(L,R,4)` |
| `&p[i]`, `&*p` | `lea (L,R,s), L`, `lea -24(%rbp,R,4), L` on a local array, and `lea c*s(L), L` for a constant index; `&*p` is `p`, no instruction, and a subscript by a constant 0 is the bare base, so `&p[0]` is `p` and no instruction as well (`aarch64.md` chapter 15, call 8); a row of an array of arrays decays the same way, `m[0]` to `m`; on an array reached by its name, the name and constant below, one `lea` of the folded operand, `leaq -20(%rbp), L` for `&a[3]` | one instruction, the addressing form, where 6.5.4.2p3's "as if `+`" would spell the same address as the `p + n` row |
| `p + n`, `p - n` | `+`: `add R, L` for a size of 1, `lea (L,R,s), L` for 2, 4 or 8, `imul $s, R, R; add R, L` for any other, and on a local array of a size 1, 2, 4 or 8 one `lea -24(%rbp,R,4), L`, a slot and an index below; `-`: `sub R, L` for 1, `imul $s, R, R; sub R, L` for any other; a constant `n` is the immediate, `add $c*s, L` or `sub $c*s, L`, and on a name's address the name and constant below, one `lea`, `leaq -20(%rbp), L` for `a + 3` | G6; the index widened as for `p[i]` |
| `p - q` | `sub R, L`; for a size other than 1, `mov $s, R; cqto; idiv R`, the result from `rax` | G6: a division, not a shift, since any two pointers; the size is held in a register, since `idiv` takes no immediate; a size of 1 is the `sub` alone |
| `a`, `"abc"`, `m[i]` as pointers | `lea slot(%rbp), L`, or the address of a global array, `lea sym(%rip), L` or the GOT's load; `lea .Ls<n>(%rip), L`; an element of array type, its address by the `&p[i]` row, or by the `p + n` row for a size that is not a scale (12 bytes: `imul $12`), but for `m[0]`, the bare base at any size, and for a constant index on an array reached by its name, the name and constant below, `leaq -12(%rbp), L` for `m[1]` | 6.3.2.1p3: an array decays to its address; a literal's bytes in `.data`, chapter 10 |
| `f`, `&f` as a value | `leaq f(%rip), L` for a function the file defines; for one it only declares, the load from the GOT, `movq f@GOTPCREL(%rip), L` | 6.3.2.1p4: a function's name converts to a pointer to the function, and `&f` is the same value; one instruction either way, chapter 10; a call by the name forms no address, chapter 8 |
| `f(a, b)` | chapter 8 | |
| `sizeof` | an immediate | folded in the parser |

**A name and a constant.** An lvalue's address plus a constant is
the lvalue's own address with the constant added to its
displacement, and no instruction forms the intermediate address. The
shape is a dereference, an address or a sum of the lvalue's address
and an integer constant, the address being the decay of an array
lvalue (6.3.2.1p3) or `&` of an lvalue, and the constant added or
subtracted after its multiplication by the size (6.5.7p9); with no
sum the constant is 0, so `*a` is the operand `a[0]` is and `*&x` is
`x`. An object the generator reaches by its name, a local's or a
parameter's slot or the label of a global the file defines or of a
string literal, is thus one operand at that address plus the
constant, and no instruction forms the object's address. The lvalue
under the decay or the `&` may be such a dereference in its turn, so
`m[1][2]` is one operand. With `int a[8]` at `-32(%rbp)`, `int
table[8]` a global the file defines and `int m[2][3]` at `-24(%rbp)`:

- `a[3]` is `movl -20(%rbp), L` and `movl R, -20(%rbp)`; `table[3]` is
  `movl table+12(%rip), L`; `m[1][2]` is `movl -4(%rbp), L`; `"abc"[1]`
  is `movsbl .Ls0+1(%rip), L`;
- `&a[3]` and `a + 3` are `leaq -20(%rbp), L`, `table + 3` is `leaq
  table+12(%rip), L`, and `m[1]` as a pointer is `leaq -12(%rbp), L`;
- `a[3] += e;` and `a[3]++;` are the memory forms of 6.6 on the folded
  operand, `addl %eax, -20(%rbp)`, as on a scalar local.

Any other lvalue's address is the one the table forms for it, a base,
a base and an index, or a slot and an index (below), and the constant
joins that form's displacement. The lvalue's address is generated
once, in the order written (G13), and the constant has no
evaluation. With `char (*p)[8]` loaded into `rax` and `i` into `rcx`:

- `p[i][2]` is `movsbl 2(%rax,%rcx,8), L`, and `&p[i][2]` and `p[i] +
  2` are `leaq 2(%rax,%rcx,8), L`, with no `leaq` of the row's
  address first;
- `q[1][2]` on `int (*q)[3]` is `movl 20(%rax), L` after `q`'s load,
  the row's 12 bytes and the column's 8 one displacement, though 12 is
  no scale;
- on a local `char m[4][8]` at `-32(%rbp)`, `m[i][2]` is `movsbl
  -30(%rbp,%rax,8), L`;
- a global the file only declares is reached through its address
  from the GOT, a base: `other[3]` is `movl 12(%rax), L` and
  `other[1][2]` on an `extern int other[][3]` `movl 20(%rax), L`, one
  instruction after the load.

On a slot the constant is any constant, negative or past the array's
end: `a[-1]` is `movl -36(%rbp), L`, the address by G6's wrapping
arithmetic, and the access reads what is there (G8); a displacement no
32-bit field holds takes the rows of the table. On a label the offset
folds from 0 to the object's size in bytes, one past the end included
(6.5.7p9), `leaq table+32(%rip), L` for `table + 8`: the linker
resolves a label and an offset, and one outside the object it may not
reach, where the rows of the table always link. Any other offset of a
label takes those rows, `table[-1]` being `leaq table(%rip), %rax;
movl -4(%rax), L`; an inner sum within the object still folds, so
`table + 8 + 3` is `leaq table+32(%rip), L; addq $12, L`. An address
that is no lvalue's address plus a constant takes the rows of the
table too: a pointer object's value, `p[3]` and a parameter declared
as an array among them, and anything under a cast other than the
decay. Over any other lvalue a sum no 32-bit displacement holds is
the lvalue's address formed by `lea`, and the constant the
displacement off it. The sum is no fold of the program's data: both
of its terms are fixed before the program runs, and for an object of
static storage it is the address constant of 6.6p9 and p11, which a
static initializer writes as `.quad table+12` (chapter 10).

The rule is the Seventh Edition compiler's, in its `c12.c`.
`acommute()` flattens a sum into a list of its operands, and where the
list ends in a constant after `&x` of a static or external name, it
adds the constant to the name's offset and drops it from the list,
under the comment `/* subsume constant in "&x+c" */`; the `*` over the
remaining `&x` then cancels, and the operand is the name and its
offset, `_x+12`. `&x` of an automatic name never reaches those lines:
`optim()` has made the name an offset from `r5`, the frame pointer, and
`unoptim()` has rewritten its `&` as the sum of `r5` and that offset,
so the constant is added to the offset with the other constants of the
list, and the `*` over `r5` plus a constant is the operand
`offset(r5)`.

**A slot and an index.** The addressing form `d(B,I,s)` holds a
base, an index and a displacement at once, so a local array's
element by a variable index is one operand: `rbp` the base, the slot
the displacement, the index scaled by the size. With `int a[6]` at
`-24(%rbp)`, `a[i]` is the index's load, `movslq -28(%rbp), %rax` for
an `int i` at `-28(%rbp)`, and `movl -24(%rbp,%rax,4), L`; `a[i] = v`
stores through the same operand, `a[i] += e;` and `a[i]++;` are the
memory forms of 6.6 on it, and `&a[i]` and `a + i` are one `leaq
-24(%rbp,%rax,4), L`. No
instruction forms the array's address, so the index is the one
operand evaluated, where it stood (G13). The array is the name and
constant above, so `m[1][i]` on `int m[2][3]` at `-24(%rbp)` is
`-12(%rbp,R,4)`. What the form does not hold takes the rows of the
table: a size that is not 1, 2, 4 or 8, `m[i]` of 12 bytes; a
pointer, `p[i]`; `a - i`, since the form only adds its index; a
global array, since a PC-relative operand takes no index; and a slot
plus a constant no 32-bit displacement holds.

A `char` or `short` lvalue is loaded with the extending `mov` of its
signedness, to 64 bits under a cast that widens a signed one to
`long`, `movsbq`, and stored with the narrow `mov`; that is the whole
of narrowing and widening on x86, where under the IR pipeline it was
`trunc`, `sext` and `zext` nodes.

The width rule. Each held value carries a width: the width to which
its register's bits are the value extended by its type's
signedness. The instruction that wrote the register sets it:

- a load of a `char` or `short` leaves it valid to 32, and the load
  of a signed object that a cast widens to 64 bits valid to 64;
- a 32-bit write clears the upper half, which is the extension of an
  unsigned value and not of a signed one, so an `unsigned` written
  at 32 bits is valid to 64 and an `int` to 32;
- a narrowing cast writes nothing and leaves the bits above its
  width stale;
- a call's result is valid to its own type's width: 8 for a `char`,
  16 for a `short`, 32 for an `int` or an `unsigned`, 64 for a
  `long` or a pointer, since the ABI leaves the bits above a result
  unspecified (3.2).

A widening cast writes its `movs` or `movz` only when the value is
not already valid to the new width: `(int) c` after the load of `c`
writes nothing, `(long) i` on a register variable is `movslq %ebx,
%rax` and on an `int` in memory its load, `movslq -4(%rbp), %rax`
(below), and the promotion of a `char` call result, `f() + 1`, is
`movsbl %al, %eax` after the `call`. A `return` of a `char` or a
`short` writes no extension: the ABI asks none of the callee, so
`return c / 4 + s` in a function returning `char` ends in the `addl`
and the jump to the epilogue. This is knowledge of what the
generator's own instruction left in the register, not of the
program's data. The PDP-11 compiler had the same knowledge built in:
its `movb` into a register sign-extended, and a `char` to `int`
conversion of a register value cost nothing.

**A widening by the load.** A cast that widens a signed object in
memory makes the object's load the extending one, to the cast's width,
and writes nothing after it. `(long) i` on an `int` at `-4(%rbp)` is
`movslq -4(%rbp), %rax`, not `movl -4(%rbp), %eax; movslq %eax, %rax`,
and `(long) c` on a `signed char` is `movsbq`, not `movsbl` and
`movsbq %al, %rax`; a plain `char` follows the target's signedness.
The object is any the load row reads: a local's or a parameter's slot,
a global's label or its address from the GOT, or what a dereference
designates, an element by a subscript among them. The parser's
widening of a subscript's index and of a pointer's addend is such a
cast, so `p[i]` on an `int i` in a slot is `movslq -12(%rbp), %rcx`
and the access. The load reads the bytes the load at 32 bits reads,
one access at the object's width (G10, `shape.md` chapter 5), into the
register the value takes in any case, and leaves it valid to the
cast's width. An unsigned object needs no such load: `movl` and
`movzbl` leave it valid to 64 bits already. A `char` promoted to `int`
was loaded extended already. A register variable is no object in
memory, so its read is extended into a fresh register, `movslq %ebx,
%rax`. A cast over any other operand, a call's result, an arithmetic
value, a comparison or another cast, extends the value in its register
by the width rule.

The rule is the Seventh Edition compiler's. `match()`, in its
`c10.c`, tries a code table's row for an operator again with the
conversion over an operand dropped, so that a row whose instructions
convert as they read takes the unconverted operand; its comment's
example is `int = double` as `movf` and `movfi` with no converted
value in a register. The conversion's own rows convert an operand in
memory in the instruction that reads it: in `table.s`, an `int` to a
`float` is `movif A1,R` for an addressable word and `movif #1(R),R`
through a pointer, and `movif R,R` only for a value already in a
register (`cr51`).

`&&`, `||` and `?:` evaluated for a value have two paths into one
register. Before the first divergence every held value is given a
location that both paths share (6.7), so both leave the register map
as they found it, whatever their operands do.

### 6.4 Constants

The parser folds integer constant expressions: the operators 6.6p3
does not exclude, applied to the operands 6.6p8 allows, which are
integer constants, character constants, `sizeof` and casts to integer
types, with the type the expression has in C and the value the
instruction would compute: a signed result the type cannot hold wraps
(G1), a shift count is masked (G3), and a division with no value, by
zero or of the most negative value by -1, is left unfolded for the
instruction to trap on (G4), so folding changes no program. 6.6p4,
which makes an out-of-range constant expression a constraint
violation, governs where the grammar requires one, the size of an
array, and the wrapped value is refused there. A variable never folds:
`x * 1` multiplies, `x - x` subtracts, `x + 0` adds. The rule "the
instruction written is the instruction executed" is about operations
on the program's data; a constant expression is the spelling of a
number, which ISO C says "can be evaluated during translation" (6.6p2)
and which Ritchie's `c0` evaluated. So `-1` is the immediate `$-1`,
`p[-1]` is the displacement `-4(L)`, and `int a[96 * 96]` has 9216
elements. This closes findings 4 and 5 of `findings.md`, and the
evaluator is not an addition: the size of an array (6.7.7.3p4), the
label of a `case` (6.8.5.3p3) and every expression in a static
initializer (6.7.11p5) require one, and the last of these cannot
compile without it, since there is no instruction to compute a
static's initial bytes at (`design-calls.md` chapter 4).

A constant operand of an arithmetic instruction is an immediate when
the instruction has an immediate form, which on x86-64 every one of
the table does but `idiv`, `div` and the variable shift: `x * 4` is
`imul $4, L, L`, `x + 1` is `add $1, L`, and `x / 4` is `mov $4, R;
cltd; idiv R`. The multiply stays a multiply, the division a
division, however the constant factors: there is no code in the
table that looks at a constant's value.

### 6.5 The pinned instructions

A division needs its dividend in `rax` and writes `rdx`; a shift by
a variable count needs its count in `cl`. A value takes the lowest
free register when it is generated, before the generator meets the
instruction that consumes it, so a value would stand in a pinned
register when the instruction comes, and moves would repair the
choice. Two rules choose the registers' names up front instead. Each
is a reservation in `take()`: the reserved registers are left for
last, and taken when no other is free, so a reservation is an
avoidance, not a pin. Neither changes the order of evaluation, nor
the order and the count of the reads and writes of any object (G13).

**A node chooses for its own operands.** The node of a pinned
instruction knows its registers:

- a divisor is generated with `rax` and `rdx` reserved, and so is
  the address of a divisor in memory, its base and its index;
- the left operand of a shift by a variable count is generated with
  `rcx` reserved, so that the count finds it; the count is generated
  with `rax` reserved, in the value and the effect context alike, and
  takes `rcx` whatever a node around the shift reserves, since the
  instruction consumes it;
- the place of `/=` and `%=`, the address the object is read and
  written through, is generated with `rax` and `rdx` reserved, and
  the place of `<<=` and `>>=` by a variable count with `rcx`;
- a dividend reloaded from its spill slot (6.2) is reloaded into
  `rax`, after `rax` is vacated, and not into another register to be
  moved again; a shift's left operand reloaded so avoids `rcx`.

The right operand of every compound assignment on an object in
memory is generated with `rax` reserved, so that the object's load
takes it and the result is in the register a `return` or an
enclosing expression takes first. `x /= e` is thus the load of `e`
into `ecx`, the load of `x` into `eax`, `cltd`, `idivl %ecx` and the
store, and `p[i] /= d` loads `p` into `rcx`, `i` into `rsi`, `d`
into `edi` and the element into `eax`, and moves nothing (`pinned`).

The rule is the Seventh Edition compiler's. Its `cexpr()`, in
`c10.c`, chooses the register of a node's value from the node's own
operator before it expands the table's row, by `oddreg()` in
`c11.c`. A division, a remainder, their assignments, the pointer
difference and an unsigned right shift work on an even-odd pair, and
take the odd register of the first pair at or above the first free
register; a long shift takes the pair's even register. The row loads
the dividend into the odd register and divides the pair, and the
row's end steps a quotient back to the even register, where `div`
leaves it.

**A node holds out what its right operand pins.** This is the one
exception to 6.1's "nothing looks at a node's neighbours", and the
machine is its reason: x86 pins these registers, and the node chooses
a register's name, not the order. A node whose right operand's top
operator is a division generates its left operand with `rax` and
`rdx` reserved, and an assignment so generates its place; a shift by
a variable count as the top operator does the same with `rcx`.

- A division is `/` and `%`, `/=` and `%=` for their value, and
  `p - q` for a size other than 1; a shift by a variable count is
  `<<` and `>>`, and `<<=` and `>>=`, by a count that is no constant.
- The top operator is read through a cast, which converts in its
  operand's register and pins nothing.
- The node is any binary operator that holds its left operand while
  its right one is generated: the arithmetic and bitwise operators,
  the comparisons, the addition of a subscript or of a pointer and an
  integer, and an assignment, simple or compound, for its place.
- A register variable is not steered: its read is its register
  (chapter 4). Nor is a call: it spills every held value on entry
  (6.7), and its result is in `rax` by the convention.

So `a + b / c` loads `a` into `ecx` and `b` into `eax`, divides by
`c`'s slot, `cltd; idivl -12(%rbp)`, and adds, `addl %eax, %ecx`
(`div-busy`), and `a[i] = (x >> 16) % 256` on a global array forms
the address in `rcx` and `rsi` and shifts and divides `x` in `eax`
(`pinned`). The rule has no counterpart in the Seventh Edition
compiler, since the PDP-11's `div` takes any even-odd pair: `oddreg()`
takes the pair above the registers the enclosing nodes hold, and no
held value is in its way.

**When the moves are written.** The rule reads the top operator
alone, so a programmer applies it to the source: a division or a
shift deeper in the right operand changes nothing, and `a + (b / c +
1)` holds `a` in `rax`. A reservation is not met when no other
register is free, and a value that is itself pinned, a call's result
or a quotient, is where its own instruction left it. Then, before the
pinned instruction, each of the pinned registers that holds a live
value other than the operand is moved to a free register by `mov`, or
the oldest held value is spilled when none is free (6.2); the
operands are moved into place, the instruction is written, the result
is moved into `L`, and the displaced values stay where they went. The
generator's map of which value is in which register is updated and
nothing else cares. Every instruction written is a real move of a
real value and shows in the listing; the cost model's row for a
division with `rax` and `rdx` busy says what it costs (`div-busy`).
The remainder's move from `rdx` into the dividend's register, and the
quotient's when the dividend is not in `rax`, are 6.1's "the result
is `L`", and the rules do not remove them.

A divisor that is an object in memory is the division's operand,
`idivl -8(%rbp)` (6.1). The registers of its address, a base and an
index, are live values as any other: one in `rax` or `rdx` moves out
first with the values held there, and the operand is formed after the
moves, from the registers they went to. A call's result in `rax` as
the base, `a % f()[i]`, moves out so, `movq %rax, %rsi`, and `a`,
spilled at the call, is reloaded into `rax`, then `cltd; idivl
(%rsi,%rcx,4)` (`pinned`).

### 6.6 The effect context

Ritchie's `c1` had four tables (`c1.h`): `regtab` for a value in a
register, `cctab` for the condition codes, `efftab` for an expression
evaluated for its effect alone, and `sptab` for a value pushed on the
stack as an argument; `lsptab`, declared beside them, is `sptab` for
an argument stored to `(sp)` rather than pushed. c1 has the first
three contexts as its three entry points, and no counterpart of
`sptab`: an argument is a hidden local of its call, stored to its
slot (chapter 8; 13, call 5; `design-calls.md` chapter 6), so nothing
is pushed as it is evaluated. The three entry points:

- `rvalue(e)`, the value in a register, by this chapter;
- `cbranch(e, label, sense)`, a jump on the truth of `e` and no
  value, chapter 7.1;
- `effect(e)`, the side effects of `e` and no value: an expression
  statement, a declaration's initializer and `for`'s step.

The effect context reaches through the operators that discard a
value: the operand of a cast to void, the arms of a conditional
whose value is not wanted, a void one or one in the effect context,
and the left operand of a comma. `(void) (i = 0);` and `c ? (i = 1) :
(i = 2);` thus store each constant in one instruction.

The comma's left operand is in the effect context wherever the comma
stands: "The left operand of a comma operator is evaluated as a void
expression" (6.5.18p2), and a void expression "is evaluated for its
side effects" (6.3.2.2p1). Its right operand takes the comma's own
context: `effect()` under the effect context, `cbranch()` as a
condition, its value otherwise. So the step of `for (i = 0, j = 0; i
< n; i++, j += 2)` is `addl $1, -8(%rbp)` and `addl $2, -12(%rbp)`,
what the two statements `i++; j += 2;` are.

In the effect context, a plain assignment of a constant to an object
in memory is one instruction. The lvalue's address is evaluated
first, then the constant is written into it, `movb $122, (%rax)`,
and no register is held. G13 holds, since the constant has no
evaluation.

The write is the store row of an initializer, `initialize()`: a
constant the immediate holds is `mov $c` of the object's width into
the object, `movl $0, -12(%rbp)`, and anything else is its value in
a register and the store. A 64-bit constant no sign-extended 32-bit
immediate holds is thus `movabsq` into a register and the store, as
in 6.3. Three writers share the row:

- a declaration's initializer, into the local's slot; a register
  variable's is the immediate into its register already (chapter 4);
- an argument, into its slot (chapter 8);
- the effect context's assignment of a constant, through the
  lvalue's address.

The effect context also takes the memory forms, as Ritchie's
`efftab` did. Every PDP-11 arithmetic instruction took a memory
operand, so `x++;` was `inc x` and `x += 5;` was `add $5, x`, and
x86 has the same forms. An object in memory is a slot, a global the
file defines, `addl $1, counter(%rip)`, or an object through a
register, a pointer's or a declared global's address from the GOT,
`addl $1, (%rax)`. On an object in memory, at the object's width:

- `x++;`, `x--;`, `++x;` and `--x;` are `add $1` or `sub $1` on the
  object, `addl $1, -8(%rbp)`, `addq $4` for a pointer to 4 bytes,
  and no old value is copied;
- `x += e;`, `-=`, `&=`, `|=` and `^=` are the right operand in a
  register, or the immediate, then the operator on the object,
  `addl %eax, -12(%rbp)`, `addb %al, -9(%rbp)`;
- `x <<= e;` and `>>=` take the count in `cl`, or the immediate, and
  shift the object, `sar` for a signed object and `shr` for an
  unsigned one;
- a pointer's `+=` and `-=` multiply the index by the size in a
  register first, then add it to the object or subtract it.

The memory form serves a condition too, as `cctab` took `efftab`'s
rows: `while (--n)` is `subl $1, -4(%rbp)` and a `je` on its flags,
and `if (x += e)` the `addl` on the object and a `je` (7.1).

This is the form of the operator on its operand, not a fold: the
operation is written once, on the object, and nothing is left out.
`*=`, `/=` and `%=` keep the right operand, then the load, the
operation and the store, since `imul` has no memory destination and
`idiv` none at all. The object's width writes the bytes the promoted
operation and the narrow store write: the low bits of `add`, `sub`,
`and`, `or`, `xor` and `shl` do not depend on the bits above them,
`sar` on a signed object and `shr` on an unsigned one fill as the
promoted shift does, and x86 masks a count to 5 bits at 8, 16 and 32
bits alike (G1, G2, G3). The address is evaluated first and held, the
right operand next, and the operand is formed last, since the right
operand may spill the address's base or index (G13). A register
variable is operated on in its register already (chapter 4); its
postfix step is the prefix form, which copies no read. Any other
expression is evaluated by `rvalue()` and its value discarded.

The value context differs, since the value is wanted in a register.
In `a = b = 5` the value of `b = 5` is wanted, so the 5 is loaded
once, `movl $5, %eax`, and stored twice; `y = x += 5` and `a[i++]`
are the load, the operation and the store, and `i++` copies the old
`i`. The order is the memory form's: the address of `x`, then the
right operand, then the load of `x`, the operation and the store, so
`y = x += e` reads `x` after `e`, as `x += e;` does. A constant right
operand has no evaluation, so `y = x += 5` is the load, `addl $5,
%eax`, the store.

### 6.7 Held values at a branch and at a call

A value is held from the instruction that produces it to the one
that consumes it. There are two kinds:

- **An owned value** is in an expression register, or in a spill
  slot once it is spilled (6.2). Nothing else names its location.
  The base and the index of an address that an instruction is to
  name, a store's or an object's in memory as a right operand (6.1),
  are owned values from their generation to that instruction.
- **A borrowed read** is a read of a register variable (chapter 4):
  the variable's own register as an operand, and no instruction. It
  is the value read only for as long as the variable is not written.

In straight-line code the write keeps a borrowed read true: it
copies every held read of its variable first, `movl %ebx, %eax`
ahead of the `addl $1, %ebx` of `i++`. Nothing is predicted there,
since the write sees what is held. At two points of an expression
that is not enough, and each has an invariant of its own.

**A branch.** `&&`, `||` and `?:` have two paths that meet again.
The invariant: at the first divergence, every held value has a
location that is the same on every path. It is established once,
before the first jump is written, and all that is generated for the
operands, nested branches included, runs inside it. The write
cannot establish it later. A copy made on one path moves the value
on that path alone, and the other path may already be written: the
false exit of `a && b` leaves from inside `a`'s jump code.

So before the first divergence:

- an owned value in a register is spilled, and its slot is its
  location on every path;
- a borrowed read that some path writes is saved: the variable's
  register is stored to a spill slot, `movq %rbx, -24(%rbp)`, and
  the value is an owned, spilled one from there on;
- a borrowed read that no path writes stays borrowed, and the
  variable's register is its location on every path. `x + (c ? 1 :
  2)` on a `register int x` stores nothing.

The last two cases turn on what the paths write. That is known, not
estimated: a register variable has no address (6.5.4.2p1), so only
an assignment, a `++` or a `--` that names it can write it, and the
tree shows every one. The walk that looks for them visits every
kind of node that has operands. A path the program does not take at
run time counts all the same, so a write in one arm saves the read
on both.

A condition generated as jump code (7.1) for a statement has nothing
held. Inside an expression it is always an operand of one of these
three, and runs inside the invariant they established. The result
is not an incoming value: both paths write it into one register
(7.2).

The invariant is checked where it could be broken. The incoming
values are marked at the divergence and unmarked where the paths
meet, and an instruction that would move a marked value, a copy, a
spill or a reload, is an error of the generator, reported at that
instruction. A gap in the walk thus stops the compilation; it does
not reach a listing.

**A call.** The `call` instruction destroys the nine expression
registers. The invariant: when the call sequence begins, no
expression register holds a value. It is established after the last
argument is stored and before the first instruction of the call
sequence (chapter 8), by spilling what is held then. That point
sees what the arguments themselves produced, and predicts nothing.
In `x + f(x++)` on a `register int x`, the argument's write copies
the held read of `x` into `eax`, and the copy is spilled there:

```
	movl	%ebx, %eax
	movl	%ebx, %ecx
	addl	$1, %ebx
	movl	%ecx, -12(%rbp)
	movq	%rax, -24(%rbp)
	movl	-12(%rbp), %edi
	call	f@PLT
	movq	-24(%rbp), %rcx
	addl	%eax, %ecx
```

The values held on entry to the call are spilled sooner, before the
first argument is evaluated. They are known to cross the call, and
spilling them first leaves every expression register to the
arguments: with `a` still in `rax`, the division in `a + f(b / c)`
would have to move it out of the way (6.5).

A borrowed read needs nothing at a call. Its register is
callee-saved (3.2), and the callee cannot write a variable that has
no address.

**One operation saves a held value**, whichever kind it is: the
spill of 6.2 for an owned one, the store of the variable's register
for a borrowed read. A branch uses both halves and a call the
first.

## 7. Conditions and control

### 7.1 Jump code

`cbranch(e, label, sense)` writes code that jumps to `label` when
`e` is true (sense 1) or false (sense 0), and falls through
otherwise, without producing a value:

- `a < b` and the rest: `L` and `R` as in chapter 6, `cmp R, L`,
  then `jcc label` with the condition or its inverse by `sense`;
  `jl`, `jge` and the rest signed, `jb`, `jae` and the rest unsigned
  and for pointers. An object in memory as `a` and a constant the
  instruction takes as `b` are the `cmp` of the object itself,
  `cmpl $5, -4(%rbp)` (a condition on an object in memory, below).
- `!e`: `cbranch(e, label, !sense)`.
- `a && b`, jump on false: `cbranch(a, label, 0); cbranch(b, label,
  0)`. Jump on true: `cbranch(a, skip, 0); cbranch(b, label, 1);
  skip:`. `||` is the mirror. Short circuit is the shape of the code;
  the second operand is not evaluated when the first decides (6.5.14p4,
  6.5.15p4).
- `a, b`: `effect(a)`, then `cbranch(b, label, sense)` (6.6).
- An operator whose own instruction sets the zero flag from the
  condition's value, and `== 0` or `!= 0` over one: that instruction,
  then `jne` or `je`, with no `cmp` (the flags of the operator's
  instruction, below).
- Any other expression: its value into `L` by chapter 6, `cmp $0, L`
  (or `test L, L`, the same flags), `jne` or `je`. A pointer compares
  against `$0` the same way (G9 has nothing to say about a test). The
  read of an object in memory alone is the `cmp $0` of the object,
  `cmpl $0, -4(%rbp)` (below).
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

**The flags of the operator's instruction.** A condition holds when
its value compares unequal to 0, and an instruction that sets ZF from
the result it computes has made that test: ZF is 1 exactly when the
result is 0. So on four rows the jump follows the operator's own
instruction, `je` or `jne` by `sense`, and no `cmp` is written:

- `a + b`, `a - b`, `a | b`, `a ^ b` and `-a` on integers: the value
  by its row of 6.3, then the jump. `if (a + b)` on two parameters is
  `addl -8(%rbp), %eax; je .Ladd.2`, the right operand the
  instruction's (6.1).
- `a & b`: both operands as the row of `&` generates them, then `test
  R, L` and the jump. `test` sets the flags the `and` would and writes
  no result, so a register variable as `L` is read in its register
  and not copied, `testl -16(%rbp), %ebx`, an object in memory as `R`
  is the instruction's operand, `testl -8(%rbp), %eax`, and a
  constant is the instruction's immediate, `testl $1, %eax`. An object
  in memory as `L` with a constant `R` is `testl $1, -4(%rbp)` (a
  condition on an object in memory, below).
- `x += e`, `-=`, `&=`, `|=`, `^=`, `++x` and `--x`: the form
  `effect()` writes for the statement (6.6), then the jump. On an
  object in memory that is the memory form, so `while (--n)` is
  `subl $1, -4(%rbp); je .Lcount.3`, and `subb $1, -1(%rbp)` on a
  `char`; on a register variable it is the operation in place,
  `addl %eax, %ebx` (chapter 4). The object is read once and written
  once, in the order of 6.6, as the value's row reads and writes it
  (G13). A postfix step is no row: its value is the old one, which
  the instruction's flags do not describe.
- `e == 0` and `e != 0`, where `e` is one of these: the same code,
  the jump's sense by the operator, so `if ((a & m) == 0)` is `testl
  -8(%rbp), %eax; jne`. `<`, `<=`, `>` and `>=` against 0 keep the `cmp`:
  a jump of this rule reads ZF alone.

The instruction is written at the condition's width, the width of the
test above: `addl` for an `int`, `addq` for a `long`, the memory form
at the object's width. Where the two differ, or where the row writes
anything after its operator's instruction, the `cmp` stays. A
`register char`'s step is `addl $1, %ebx` and the re-extension
`movsbl %bl, %ebx` after it, so its test is `cmpb $0, %bl`; `x += l`
on a `register int` and a `long l` is the operation on a 64-bit copy
and the `mov` back, then `cmpl $0, %ebx`. A pointer keeps the `cmp`,
since `p + n`, `p - n` and `p - q` are `lea`, `imul` or a division
(6.3), and so do `*`, `/`, `%`, the shifts, `~`, a cast, `*=`, `/=`,
`%=`, `<<=` and `>>=`, by the table below.

c1 knows the flags as it knows the width a register is valid to
(6.3). It keeps, for the last instruction written, the width of the
result from which that instruction set ZF, or none. The rows state it
after their instruction. Every instruction written after it withdraws
it, a move of the result, a reload from a spill slot or a narrow
value's re-extension, and so does a label, which another path may
reach with flags of its own. `cbranch()` writes the bare jump only
where the record holds at the condition's width, and the `cmp`
elsewhere. A memory form under a condition has no value to compare,
so where it leaves no record c1 stops with an error of the generator.

The flags are Intel's, from the "Flags Affected" paragraph of each
instruction in the Intel 64 and IA-32 Architectures Software
Developer's Manual, volume 2:

| Instruction | ZF, by the manual | Row |
|---|---|---|
| `add` | "The OF, SF, ZF, AF, CF, and PF flags are set according to the result." | `+`, `+=`, `++x` |
| `sub` | "The OF, SF, ZF, AF, PF, and CF flags are set according to the result." | `-`, `-=`, `--x` |
| `and`, `or`, `xor` | "The OF and CF flags are cleared; the SF, ZF, and PF flags are set according to the result." | `\|`, `^`, `&=`, `\|=`, `^=` |
| `neg` | "The OF, SF, ZF, AF, and PF flags are set according to the result." | `-a` |
| `test` | "The SF, ZF, and PF flags are set according to the result", the `AND` of its operands, which it does not write | `&` |
| `imul` | "The SF, ZF, AF, and PF flags are undefined." | none: `*` and `*=` keep the `cmp` |
| `idiv`, `div` | "The CF, OF, SF, ZF, AF, and PF flags are undefined." | none |
| `shl`, `sar`, `shr` | "The SF, ZF, and PF flags are set according to the result. If the count is 0, the flags are not affected." | none: a count the machine masks to 0 leaves the flags of the instruction before |
| `not`, `lea`, `mov`, `movsx`, `movzx` | "None." | none |
| `inc`, `dec` | "The OF, SF, ZF, AF, and PF flags are set according to the result." | none: c1 writes `add $1` and `sub $1` |

The rule is the Seventh Edition compiler's `cctab`, the table of its
four that compiles a condition for the condition codes and no value
(6.6). Its `cbranch()`, in `c11.c`, compiles a condition that is no
`&&`, `||`, `!` or comma by `cctab` and branches on the codes.
`rcexpr()`, in `c10.c`, drops the 0 of a relational with 0 under
`cctab`, so `x == 0` is the code of `x` and the branch. `&` is the
PDP-11's `bit`, the row `cc81`, under the comment `& as in "if
((a&b)==0)"`. `cexpr()` tries `efftab` first for a condition, since
"efftab will generate the right code unless the operator is a shift
or postfix ++ or --", so an assignment operator's effect form and its
codes serve. The rows `rest` write the value of `+`, `-`, `|` and
unary `-` and branch with no `tst`, and any condition no row takes is
its value by `regtab` and a `tst`. rcc takes the rows whose x86
instruction leaves ZF right. It adds `^`, which `cctab` has no row
for, and leaves out `*` and `/`, which `rest` takes; a dereference,
which `rest` takes too, since a PDP-11 `mov` sets the codes; and `<`
and the other relationals with 0, whose branches read more than the
zero code. The optional peephole pass `c2` removed a `tst` of the place
the `mov` before it wrote (`c21.c`). That is not taken: an x86 `mov`
sets no flag, and rcc has no peephole pass.

**A condition on an object in memory.** Where the value is not
wanted, in `cbranch()` alone, three conditions read an object in
memory in the instruction that tests it, at the object's width, and
no register holds the object:

- a comparison whose left operand is the read of an object in memory
  and whose right operand is a constant the instruction takes as an
  immediate: `cmpl $5, -4(%rbp)` and the jump;
- the read of an object in memory alone, `if (x)`: `cmpl $0,
  -4(%rbp)`, and `cmpb $0, -1(%rbp)` on a `char`, since a condition
  is tested at its operand's own width (above);
- `&` of an object in memory and such a constant: `testl $1,
  -4(%rbp)`, which is the flags row of `&`, so the jump follows it
  with no `cmp`, under `== 0` and `!= 0` too.

The object is any the load row reads (6.1). The instruction reads the
left operand, so the form holds only where nothing is evaluated
between the object's load and the instruction: the right operand is a
constant, which has no evaluation (G13). `if (g < f())` keeps the
load of `g` before the call, since `f` may write `g`, and so does `g
== h()`. A comparison for its value keeps the load, since `setcc`
writes a register in any case; a `char` or a `short` that the
comparison or the `&` promotes keeps its extending load; and a
constant no immediate holds is held in a register, so its left
operand keeps the load too.

The rows are the Seventh Edition compiler's. In `cc60`, the
relationals under the condition codes, the row `%a,z`, an addressable
operand and the constant 0, writes `tst A1`, the test of the operand
where it is, `tst _x`, and `tstb` on a `char`; `%aw,aw`, two
addressable words, writes `cmp A2,A1`, the compare of the two operands
where they are, a constant among them. In `cc81`, `&` under the
condition codes, `%a,a` writes `bit A2,A1`, `bit $1,_x`. The PDP-11's
`cmp` and `bit` took two operands in memory, and its compiler
exchanged a relational's operands by their degree and mapped the
operator (`optim()`, in `c12.c`). x86 has no such instruction and rcc
does not reorder, so here the left operand is the object and the right
the constant.

### 7.2 Statements

- `if (e) s1 else s2`: `cbranch(e, .Lelse, 0); s1; jmp .Lend;
  .Lelse: s2; .Lend:`. Without `else`, no `jmp` and one label. When
  `s1` is one jump, a `break`, a `continue`, a `goto L` or a `return`
  with no value, bare or alone in braces, the `if` is `cbranch(e,
  target, 1)` to that jump's target, then `s2`: no `jmp` and no label.
  `if (n & 1) break;` is `testl $1, -4(%rbp); jne .Llow.3` (7.1).
  The rule is the first pass's of the Seventh Edition compiler,
  `statement()` in its `c02.c`, which takes the bare jump; rcc takes
  it in braces as well, since the braces write nothing. A `return e`
  is not one jump: `e` is code of its own.
- `while (e) s`: `.p2align 5; .Lcond: cbranch(e, .Lend, 0); s; jmp
  .Lcond; .Lend:`. The condition is at the top, where it is written,
  and tested once per iteration plus once; no rotation, no
  duplicated test at the bottom. The directive aligns the head to 32
  bytes, and the assembler fills the gap with multi-byte `nop`s (13,
  call 8).
- `do s while (e);`: `.p2align 5; .Lbody: s; .Lcond: cbranch(e,
  .Lbody, 1); .Lend:`. The body comes first and the condition at the
  bottom, where it is written, tested once per iteration (6.8.6.3p1);
  the jump back is taken while it holds. `.Lbody` is the loop's head
  and takes the directive as `while`'s and `for`'s heads do.
- `for (i; e; n) s`: `i; .p2align 5; .Lcond: cbranch(e, .Lend, 0); s;
  .Lstep: n; jmp .Lcond; .Lend:`.
- `break` and `continue`: `jmp` to a label of the innermost loop around
  them. `break` jumps to its `.Lend`, past the loop (6.8.7.4p2).
  `continue` jumps to the loop's continuation (6.8.7.3p2): `.Lcond` in
  `while` and `do`, `.Lstep` in `for`. Each loop places all three of its
  labels whether or not a jump names them. c1 keeps the loops and the
  switches around the statement it generates as a stack of their labels;
  a `switch` takes a `break`, to the label past its body, and no
  `continue`, which goes on to the loop around it (7.3). A statement
  boundary holds no value (6.7), so the jump is the whole statement.
- `goto L` and `L:`: `jmp` to the label and the label where it is
  written. A label of the source is one more label of the function,
  `.L<function>.<n>`, numbered when c1 first meets it, at its definition
  or at a `goto` to it; no listing shows the C name. It is not aligned,
  even when a `goto` jumps back to it: the directive is the compiler's
  for the loops the language names, and a loop built of `goto` is laid
  out as written. A `goto` into a block skips the initializers before
  its label, and the objects they would initialize hold some value of
  their type (G5). A label may stand before a declaration and
  before a block's closing brace (6.8.3p1), and is then the null
  statement it is translated as followed by (p2).
- `e ? a : b` for its value: `cbranch(e, .Lelse, 0)`, `a` into `L`,
  `jmp .Lend`, `.Lelse:` `b` into the same `L`, `.Lend:`. Never
  `cmov`: the arm not chosen is not evaluated (6.5.16p5), and the
  jump says so.
- `return e`: `e` into `L`, `mov L, %eax` if `L` is not `rax`, `jmp
  .Lret`, where `.Lret` is the one epilogue (a name in this
  document; the listing names it `.L<function>.1`, the first label a
  function takes). `return` jumps to the epilogue every time, the
  last statement of the body included: the
  jump to the next instruction is what the statement says, and
  removing it was the optional peephole pass's (`c2`) work on the
  PDP-11, which rcc has no counterpart of. A fall off the end of a
  non-`void` function reaches `.Lret` with `rax` as it is, which is
  G5's "some value of its type"; the `%ret.indeterminate` slot of the
  IR pipeline is gone. `main` is the exception: reaching its closing
  `}` returns 0 (5.1.2.3.4p1), so `main` writes `movl $0, %eax`
  before the epilogue unless its last statement is a `return`. A
  `void` function whose last statement is not a `return` falls into
  the epilogue's label, with no `jmp`.
- Blocks and labels are written in source order. A label is `.L`,
  the function's name and a number counted from 1 in each function,
  `.L<function>.<n>`, `.Lsum.1`. The name keeps the labels of a file
  unique, since an identifier holds no dot. The first label a
  function takes is its epilogue's, `.L<function>.1`. Nothing is
  reordered after the labels.
- `switch (e) s`: `e` into `L`, the dispatch of 7.3, then `s` in
  source order, each `case` and `default` label where it is written,
  and the label past the body.

### 7.3 The switch

The parser converts each case's constant to the promoted type of the
controlling expression (6.8.5.3p5): `int`, `unsigned`, `long` or
`unsigned long`. c1 sorts the cases by value in that type's order;
`ncase` is the count of the cases, and `range` the last value less the
first, as an unsigned number of the type's width. The form of the
dispatch follows from the cases alone:

1. **No case.** The value of `e` is computed, since it is written, and
   a `jmp` goes to the default, or past the body when there is none.
2. **Direct**, when `range > 0` and `range <= 3 * ncase`. The first
   value is subtracted from `L` when it is not 0; the result is
   compared unsigned with `range`, and a value above it jumps to the
   default. Any other jumps through a table of `range + 1` addresses:
   each value's entry is its case's label, and a value no case holds
   has the default's.
3. **Simple**, otherwise when `ncase < 10`. A compare and a jump if
   equal for each case, in the order of the values, then a `jmp` to
   the default.
4. **Hash**, otherwise. The table's size, `tabs`, is the `i` from
   `ncase / 4` to `ncase / 2` whose `i * worst` is least, `worst` being
   the most cases whose values, read unsigned, leave one remainder
   modulo `i`; on a tie the first such `i`. `L` is divided unsigned by
   `tabs`, and the jump goes through a table of `tabs` addresses by the
   remainder, to a bucket. Each bucket compares the quotient with
   `value / tabs` of each case whose remainder it is, in the order of
   the values, then jumps to the default.

The default of a switch that has none is the label past the body.
The rule is the Seventh Edition compiler's, `pswitch` in its `c11.c`,
and what rcc adds is for its wider types and its machines: the type is
the promoted one, not the PDP-11's 16-bit `int`, so the order, the
range and the unsigned reading of the hash are that type's, and every
instruction of the dispatch is at its width; the search for `tabs`
starts above any product, where the PDP-11's started at 077777, so a
switch of any size finds one; and the instructions are each machine's
rows, below.

**Direct.** `day()` of the cost program `switch-direct`, cases 1, 2, 3
and 5 of an `int` and a default, `ncase` 4 and `range` 4:

```
	movl	-4(%rbp), %eax
	subl	$1, %eax
	cmpl	$4, %eax
	ja	.Lday.6
	leaq	.Lday.8(%rip), %rcx
	jmp	*(%rcx,%rax,8)
	.data
	.p2align 3
.Lday.8:
	.quad	.Lday.2
	.quad	.Lday.3
	.quad	.Lday.4
	.quad	.Lday.6
	.quad	.Lday.5
	.text
```

The subtraction is the `sub` row of 6.3, the constant an immediate, or
held by the constant row when no 32-bit immediate holds it: `movabsq
$-9223372036854775808, %rcx; subq %rcx, %rax`. The compare is the
comparison row and `ja` its unsigned jump. The dispatch is an
operation of the unsigned type of the width, since the compare reads
the index so, and the addressing form reads the index at 64 bits. The
width rule of 6.3 makes it valid to 64: after `subl`, a 32-bit write
of an unsigned value, it writes nothing; with a first value of 0 and
no `subl`, an `int` loaded valid to 32 bits takes `movl %eax, %eax`
before the `leaq`, as `digit()` of the same program shows. A register
variable is read by the compare and by the jump, `cmpl $3, %ebx`, and
copied into `L` for the `subl`, or for the width rule's `movl`, since
neither may write the variable (chapter 4). The jump goes through the
entry, `jmp *(%rcx,%rax,8)`, the table's address formed by `leaq` as a
literal's is.

**Simple.** `kind()` of `switch-simple`, cases written 10, 0, -1 and
100 and a default, `range` 101:

```
	movl	-4(%rbp), %eax
	cmpl	$-1, %eax
	je	.Lkind.4
	cmpl	$0, %eax
	je	.Lkind.3
	cmpl	$10, %eax
	je	.Lkind.2
	cmpl	$100, %eax
	je	.Lkind.5
	jmp	.Lkind.6
```

Each compare is the comparison row with the case's constant, an
immediate or the constant row's register, and `je`. The Seventh Edition
compiler tested a case of 0 with `tst`, its test of zero; this
machine's test of zero is the same `cmpl $0` (7.1).

**Hash.** `reason()` of `switch-hash`, twelve status codes from 200 to
503, `range` 303; its comment works the size out, 5:

```
	movl	-4(%rbp), %eax
	movl	$5, %ecx
	xorl	%edx, %edx
	divl	%ecx
	leaq	.Lreason.16(%rip), %rcx
	jmp	*(%rcx,%rdx,8)
	.data
	.p2align 3
.Lreason.16:
	.quad	.Lreason.17
	.quad	.Lreason.18
	.quad	.Lreason.19
	.quad	.Lreason.20
	.quad	.Lreason.21
	.text
.Lreason.17:
	cmpl	$40, %eax
	je	.Lreason.2
	cmpl	$80, %eax
	je	.Lreason.8
	cmpl	$100, %eax
	je	.Lreason.12
	jmp	.Lreason.14
```

The division is the unsigned division row of 6.3 with the pins of 6.5:
the size held in a register as the divisor of `x / 7` is, kept out of
`rax` and `rdx`, and both results kept, the quotient in `eax` and the
remainder in `edx`. The remainder is valid to 64 bits, since `divl`
wrote it at 32, and indexes the table. The quotient is held across the
jump and into the buckets; nothing on any path writes it.

**The table.** As the Seventh Edition compiler wrote it: in `.data`,
where the dispatch stands in the function's text, aligned to 8, under
a label of the function, an address per entry, then `.text` again. The
assembler leaves a relocation for each address and the loader fills it
in, as it does a global's address constant (chapter 10).

**Labels and the body.** A `case` or `default` label is one more label
of the function, numbered when c1 meets the switch, in the order the
labels are written; the label past the body follows, then the table's
and the buckets'. The dispatch comes first, then the body in source
order, each label where it is written, in a nested block or a loop of
the body as well, so Duff's device is the body as written. Control
falls from one case into the next unless a statement leaves. No value
is held at a case label. A `break` in the body jumps past it, and a
`continue` goes on with the loop around the switch (7.2).

## 8. Calls

`f(a1, ..., an)`, with the arguments evaluated left to right (G13).
Every argument is a hidden local of the call: a slot of its own in
the frame, written as the argument is evaluated and read back by the
call sequence.

A call names its function or goes through a pointer to one. The tree
says which: the callee of the first is the function's name, and the
callee of the second is any expression that is a pointer to a
function (`shape.md` chapter 4). The callee of a call through a
pointer is a hidden local of its call as every argument is.

1. Every expression register holding a value of the enclosing
   expression (`x` in `x + f(a, b)`) is stored to a spill slot; the
   register is freed, and the arguments find every expression
   register free.
2. The callee of a call through a pointer is evaluated first, since
   the source writes it first (G13): a slot of 8 bytes, taken before
   the callee is evaluated, written by the store row of 6.6,
   `movq %rax, -24(%rbp)`. A call that names its function has no
   such slot. Then each argument in turn is written to its argument
   slot by the store row of 6.6, the slot taken before the argument is
   evaluated. A constant is the immediate into its slot, `movl $1,
   -4(%rbp)`; anything else is evaluated into a register by chapter
   6 and stored, `mov L, -24(%rbp)`. Arguments are evaluated in
   source order, each complete before the next begins, and a call
   inside an argument finds every expression register free and
   clobbers nothing that matters.
3. The call sequence. Whatever the arguments left held in an expression
   register is spilled first (6.7), so nothing is held in a register
   from here to the `call`. If the arguments past the sixth are an odd
   number, `sub $8,%rsp`, so that the `call` lands on a multiple of 16.
   The arguments past the sixth are pushed from their slots right to
   left, so that the seventh is lowest, as the ABI wants (3.2.3): an
   8-byte slot in one instruction, `pushq -40(%rbp)`. A slot narrower
   than 8 bytes is not pushed directly, since `pushq` would read the
   bytes above it: it is loaded, extended, into `eax` and pushed from
   `rax`, `movl -28(%rbp), %eax; pushq %rax`. The first six are loaded
   from their slots into `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`, a
   narrower one extended to 32 bits by the load. `al` is set to 0 for a
   variadic callee, `movl $0, %eax`, a function or the pointee of a
   pointer. `call f@PLT` for a named function, every named call through
   the PLT, chapter 10; `call *-24(%rbp)` through a pointer, the
   machine's call through memory, straight from the callee's slot, with
   no register. The pushed bytes and the pad are added back.
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
A `char` or `short` result is not extended by the callee before
`ret`: the ABI leaves the bits above a narrow result unspecified, and
`gcc` and `clang` extend it in the caller. So the caller holds it
valid to its type's width, and the width rule extends it where it is
used wider (6.3), `movsbl %al, %eax` after the `call`. G16: the call
is made with the call site's types and the convention's registers,
and nothing is added, dropped or rewritten. A call through a pointer
takes its types from the pointer's pointee, whatever function the
pointer came from: a pointer cast to another function type calls with
the cast's types, and nothing checks them against the function's.

`p(x)`, `(*p)(x)` and `(**p)(x)` are one call and one listing: `*` on
a pointer to a function is the function designator, which converts
back to the pointer, and writes no instruction. `(*f)(x)` and
`(&f)(x)` on a function's name are calls through a pointer: the source
wrote an address, so the address is formed by the row of chapter 10,
stored to the callee's slot, and called through. `f(x)` and `(f)(x)`
name the function.

The callee's prologue: `push %rbp; mov %rsp,%rbp`, push the
register-variable registers it uses, `sub` its frame unless it is a
leaf in the red zone, store each parameter's argument register to its
slot or move it to its callee-saved register if `register`. The
epilogue at `.Lret`: pop the registers, `leave`, the directive of
chapter 5, `ret`.

## 9. What each guarantee compiles to

The instruction clause of each guarantee in `guarantees.md` is its
row here, in the guarantee's voice.

| Guarantee | Instruction |
|---|---|
| G1 signed arithmetic wraps | `add`, `sub`, `imul`, `shl`, `neg`: the machine computes modulo 2^N |
| G2 narrowing truncates | a narrow store `movb`, `movw`, `movl`, or the narrower register name; nothing is computed |
| G3 shifts | `shl`, `sar`, `shr` by `cl` or an immediate: the instruction masks the count to the width |
| G4 division is the instruction's | `idiv`, `div`: `#DE` on a zero divisor and on `INT_MIN / -1`, by the machine |
| G5 an uninitialized object holds a value | a slot's bytes, or a register's content, unwritten until the program writes; nothing in the generator touches either, but for the extension of an unwritten `char`, `short` or `unsigned` register variable in its own register, which leaves the value of its type as it was |
| G6 pointer arithmetic is address arithmetic | `lea`, `add`, `sub` on 64-bit registers, `imul` by the size; `p - q` is `sub`, `cqto`, `idiv` |
| G7 pointer comparison is address comparison | `cmp` of the 64-bit addresses, in registers or one in its slot (6.1), the unsigned `jcc` and `setcc` |
| G8 a load reads the bytes at the address | `mov (L), L`, whatever computed `L` |
| G9 null access at address 0 | the same `mov`; the machine faults or reads page zero |
| G10 memory is bytes | the same `mov` with the lvalue's width and extension |
| G11 misaligned access performed | the same `mov`; x86 has one `mov` for every alignment |
| G12 one slot for the activation | chapter 5: every local's slot, none shared, none reused within the call |
| G13 left to right | chapter 6's rule and chapter 8's order |
| G14 a loop runs until its condition says stop | chapter 7's `while`: a test and a jump, and no code that reasons about the test |
| G16 a call goes out as written | chapter 8: the convention's argument moves with the call site's types, then `call f@PLT`, or `call *d(%rbp)` through a pointer |
| G17 a string literal is writable | `.data`, chapter 10 |

G15 stays withdrawn. The never-emitted table, the IR flags the
front end withheld from an optimizer that never ran, has no
counterpart: there is no IR, and there is nothing to withhold from
`as`. `rationale.md` keeps the table as the record of the
argument.

## 10. Statics and literals

A translation unit's output is one assembly file with a `.text`
section for its functions and a `.data` section for its objects
with static storage. A string literal is a labeled array of bytes in
`.data`, one label per joined literal, never merged, never in
`.rodata`: G17. The label is `.Ls<n>`, numbered from 0 in the order
the literals are met, by a counter of its own in each file; two equal
literals written twice are two labels. The literals the functions name
are numbered first; those the objects' initializers point into follow
them, in the order of the definitions and of the elements in each,
so an initializer's literal moves no function's. The bytes are one `.string`
directive, which appends the terminating zero (6.4.5p6): printable
ASCII as itself with `"` and `\` escaped, `\n`, `\t` and `\r` as the C
escapes, and every other byte as a three-digit octal escape. All the
literals of a file are one `.data` block after its last function,
before the note section, with no alignment directive, since a `char`
array aligns to 1. A literal's address is `lea .Ls<n>(%rip), L`.

A function is `.type f, @function` and its label, and ends with
`.size`. One of external linkage is `.globl f` first. A `static`
function has internal linkage (6.2.2p3) and no `.globl`, so its symbol
is local to the file's object: the assembler binds the file's
references to it, and the linker binds no other file's. That is the
whole difference in the listing. A call of it is the named call,
`call f@PLT`, which the linker resolves to the local symbol directly,
and its address is the PC-relative row of a function the file
defines, below.

An object the file defines is a labeled area after the literals. The
objects with an initializer are one `.data` block, and those without
one are one `.bss` block after it, each in the order of the
definitions; `int n = 0;` is in `.data`, since the source wrote the
initializer, and `int n;` is in `.bss`. Each object is written so:

- `.globl n` for an object of external linkage, and nothing for a
  `static` one, at file scope or in a block, whose symbol is local to
  the file's object;
- `.type n, @object` and `.size n, 4`, the symbol's kind and its size
  in the symbol table, which the linker and a debugger read;
- `.p2align k`, the alignment of its type, `k` its power of two, when
  it is above 1; a `char` has none; an array of 16 bytes or more is
  aligned to 16, `.p2align 4`, as the psABI's "Aggregates and Unions"
  asks of a global array, since code another compiler writes for its
  declaration may assume it, and a local array keeps its element's;
- the label, then the initializer's data, below, or `.zero` with the
  object's size.

c0 folds the initializer into the subobjects it gives a value
(6.7.11p18): each scalar, and each array of `char` a string
initializes, with its byte offset in the object, its type and its
value. A scalar's initializer, in braces or not, is one subobject at
offset 0. A brace-enclosed list is one for each element it gives, in
subscript order and at any depth, with the braces of a subaggregate
written, left out or left out in part (p21). The data are each
subobject's value at its own width, in the order of the offsets, and
a `.zero` for each run of bytes no subobject holds, between them and
after the last, so the bytes written are the object's size:
`int m[2][3] = { { 1 }, 4, 5 };` is `.long 1`, `.zero 8`, `.long 4`,
`.long 5` and `.zero 4`. An object a list initializes is in `.data`
whatever its values, so `int z[3] = {};` is `.zero 12` there. Each
value has one of three forms, and each form has its data:

- A constant: an integer constant expression converted to the
  scalar's type, a null pointer, or an integer cast to a pointer. It
  is `.byte`, `.short`, `.long` or `.quad` with the number, by the
  scalar's size, read unsigned for an unsigned type: `.long 42`,
  `.quad 4096`, `.byte 255` for an `unsigned char`.
- An address constant (6.6p9, p11): a symbol and a byte offset, one
  `.quad`. The symbol is a global or a function, one the file defines
  or one it only declares, or a string literal's label: `.quad n` for
  `&n`, `.quad a+12` for `&a[3]` of an `int` array, `.quad a+8` for
  `a + 2`, `.quad on_signal`, `.quad stdout`, `.quad .Ls0+1` for
  `"abc" + 1`. The offset is the constant times the pointee's size, as
  a subscript's is. An address cast to an integer of 8 bytes is the
  same `.quad`; to a narrower integer it is an error, since the loader
  fills in no field narrower than a pointer. The assembler leaves a
  relocation, and in a position-independent executable the linker
  leaves the word to the loader: a relative relocation for a symbol
  the program defines, the symbol's own for one in a shared library.
  The object is in `.data` with the other initialized ones, writable,
  since the loader writes it. `char *names[] = { "Jan", "Feb" };` is
  two literals and `.quad .Ls0` and `.quad .Ls1`.
- A string literal for an array of `char`, `signed char` or `unsigned
  char` (6.7.11p15), the object or an element of an array of arrays,
  in braces or not: the array's own bytes at its offset, in the
  escapes of the literals' block, and no literal of its own. `.string`
  appends the terminating zero when the array has room for it, and
  `.ascii` appends none when the literal fills the array; the elements
  past the literal's are part of the run of zeros after it. `char s[]
  = "abc";` is `.size s, 4` and `.string "abc"`, `char s[8] = "abc";`
  is `.string "abc"` and `.zero 4`, and `char s[3] = "abc";` is
  `.ascii "abc"`. `char names[][4] = { "Jan", "Feb" };` is `.string
  "Jan"` and `.string "Feb"`. `char *p = "abc";` is two objects: the
  literal in the literals' block, and the pointer, `.quad .Ls0`.

An object the file only declares writes nothing.

An object a block declares `static` has static storage duration
(6.2.4p3) and is one more object the file defines: in the `.data` or
the `.bss` block with the others, in the order of the definitions,
without `.globl`, since it has no linkage. Its initializer takes every
form a global's does, and is written once in its data; its
declaration writes no instruction. Its label is the function's name
and its own, `next.count` for `count` in `next()`, and from the second
object of that name in the function on, a number after them, counting
from 1: `next.count.1`. No C identifier holds a `.`, so the label is no
other symbol's, and two functions, or two blocks of one function, do
not share one. c0 gives the symbol the label as its assembler name,
which c1 writes wherever it writes the symbol.

A global the file defines, `static` or not, a block's among them, is
reached PC-relative, a slot with another spelling: `movl
counter(%rip), %eax`, `addl $1, counter(%rip)`, `movl
next.count(%rip), %eax`,
`leaq counter(%rip), %rax`. An array the file defines is indexed by a
variable after its address is formed, `leaq table(%rip), %rax`,
since a PC-relative operand takes no index, where a local array's
slot is the displacement of the indexed operand (6.3); by a
constant it is its label and the element's offset, `movl
table+12(%rip), %eax`, the name and constant of 6.3.

A function's address as a value has two rows, and so does an
object's. A function the file defines, wherever in the file its body
stands, is reached PC-relative, as a literal is, `leaq f(%rip), L`. A function the file
only declares may be in a shared library, which a PC-relative
reference does not reach from a position-independent executable:
GNU ld refuses `R_X86_64_PC32` against a libc function there. Its
address is loaded from the GOT, `movq f@GOTPCREL(%rip), L`, the
entry the dynamic linker writes, which holds the address the library
itself takes of the function, so the two compare equal. The symbol is
the name `assembly_name()` gives, as a call's is. An object follows
the same rule: one the file defines, by a definition anywhere in the
file, is PC-relative, and one it only declares has its address loaded
from the GOT, `movq stdout@GOTPCREL(%rip), L`, and each access is the
indirection through that register, as `*p` is. GNU ld links a
PC-relative reference to a libc object on x86-64 by a copy relocation
and refuses it on aarch64; the GOT is the one form both link.

The file opens with `.file` and the source's name without its
directory, `.file "local.c"`, and then `.text`. It closes with
`.section .note.GNU-stack,"",@progbits`, the empty note section that
marks the stack not executable.

Every address is PC-relative (`sym(%rip)`) but a declared function's
and a declared object's, which come from the GOT, and every named
call goes through the PLT,
so the object links into a position-independent executable, which is
the default link on the platforms rcc runs on, and into a fixed one
as well.

## 11. The driver and the pipeline

`rcc file.c` runs the system cc's preprocessor, tokenizes, parses,
folds, generates assembly text, and then runs the system cc's
assembler on it for `-c` and its linker for a link; the system cc is
`clang`, which the driver runs as `clang -E`, `clang -c` and `clang`.
`-S` writes the assembly to a file; standard output for `-o -`, as
every cc does, is queued (`TODO`). No `llc`, no IR, no flag table. The
`# line "file"` markers of the preprocessor still give the diagnostics
their positions. The one warning, chapter 4's for a sixth live
`register` variable, is printed in the error's format and changes no
exit status; `-w` silences it. `-target x86-64` or `-target aarch64`
names the target, and the default is the machine rcc was built on.
For another machine's target the driver hands each of the three
`clang` runs `--target=` and the target's triple.

## 12. Verification

- **The oracle**, `tests/oracle` over `tests/corpus`: the same
  program under `clang -O2` and under `rcc`, exit statuses compared,
  and the text `rcc -S` writes diffed against the program's pin.
- **The cost corpus**, `tests/costs` over `tests/cost`: the pin of
  each program is the text `rcc -S` writes, and the script assembles
  it with `rcc -c` to prove that it assembles. Each pin is read by
  hand against its target's design, this document's table or
  `aarch64.md`'s, before it is adopted, and chapter 2 of `cost.md` is
  written from the pins.
- **Where the pins live**: under a directory per target beside their
  programs, `tests/corpus/<target>/<name>.s` and
  `tests/cost/<target>/<name>.s`, written by
  `tests/oracle --target <target> --pin` and
  `tests/costs --target <target> --pin`. Both targets have a
  directory in each corpus. A program that uses a construct the
  target's generator does not handle yet, and has no pin for the
  target, shows "not yet" and is neither passed nor failed, as the
  scripts' headers say.
- **The ABI**: `g12-block-slot` is the corpus program that calls
  into libc, `printf` with `al` set, and it passes. The probes,
  `tests/probes` over `tests/probe/`, run rcc's code against `clang
  -O0`'s on the machine's own target, function by function over a
  table of values, and the calls probe pairs the two three ways:
  `clang`'s code calls rcc's, rcc's calls `clang`'s, and rcc's calls
  rcc's.
- **`make test`** runs both scripts for every target with a pin
  directory, and then the probes.
- **The measurement**, `tests/bench`: redone under c1, and recorded
  in `cost.md` chapter 4 beside the LLVM pipeline's; the registers
  each tuned function takes are in `design-calls.md` 2.2.

## 13. The calls

Six were open in the first draft of this page, and the rest came
later. `design-calls.md` weighs the first five one chapter each; the
answers:

1. **`rbp`** is the frame pointer in every function, and the
   register variables are five. A slot's offset is known at its
   declaration, the prologue is one shape, the debugger needs
   nothing from us; by the live count as first written the sixth
   register would keep one more variable out of a slot in each of
   collatz and matmul, and fit neither whole.
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
   `src/c1-x86-64.cc` and `src/c1.hh`.
7. **The effect context takes the memory forms**, as Ritchie's
   `efftab` did: `x += 5;` is `addl $5, x` and `x++;` is `addl $1, x`.
8. **Every loop head is aligned to 32 bytes**, `.p2align 5`, a
   convention of the target; the measurement is `cost.md` 4.2, where
   64 bytes was measured too and not adopted.
9. **A `register` variable is not kept across `longjmp`**, `volatile`
   or not. c0 drops `volatile` (`shape.md` chapter 5), so `register
   volatile int x;` is in its callee-saved register (chapter 4), and
   `longjmp` restores that register to its value at `setjmp`. Every
   other local is in its slot and keeps the value last stored. The
   cost is a departure from 7.13.2.1p3, which keeps the value of a
   volatile local of the function that called `setjmp`.
10. **A switch is compiled by the rule of the Seventh Edition
    compiler**, taken whole (7.3): a direct jump through a table, a
    compare chain in the order of the values, or a hash, chosen from
    the case values alone, the same on both targets. The rule has no
    threshold of rcc's own and no order by frequency, so a programmer
    who knows the cases knows the form. The cost is a table in `.data`
    for the loader to relocate, and in the hash a division at every
    dispatch.
11. **An `if` whose body is one jump is the condition's jump** (7.2),
    on both targets: a `break`, a `continue`, a `goto` or a `return`
    with no value, and the `if` jumps to its target when the condition
    holds, with no label of its own. The rule is the first pass's of
    the Seventh Edition compiler, which takes the bare jump; rcc takes
    it alone in braces as well, since the braces write nothing. The
    boundary is one level of braces and a jump with no code: `return
    e` and a jump in two pairs of braces keep the jump over the body.
12. **A cast that widens a signed object in memory is its load**,
    extended to the cast's width (6.3), on both targets: `(long) i`
    is `movslq -4(%rbp), %rax` and nothing after it, and the parser's
    widening of an index is such a cast. The Seventh Edition compiler
    converts an operand in memory in the instruction that reads it, as
    its `match()` lets a row do; rcc takes the widening of a signed
    integer alone. The boundary is the read: a register variable, and
    any operand that is no read of an object, keep the extension after
    the value.
13. **On x86-64 a condition jumps on its operator's own flags** (7.1):
    where the operator's instruction sets ZF from the condition's
    value, `je` or `jne` follows it and no `cmp` is written, so `while
    (--n)` is `subl $1, -4(%rbp)` and the jump. The rows are the
    Seventh Edition compiler's `cctab`, taken where Intel's manual
    says the instruction sets ZF: rcc adds `^` and leaves out `*`, `/`
    and a dereference. aarch64 tests a register with `cbz`, no flags,
    and has nothing to take. The cost is a record the generator keeps,
    the width at which the last instruction set ZF, which any
    instruction or label after it withdraws.
14. **On x86-64 an operand in memory is the instruction's own** (6.1):
    a right operand that reads an object of the operation's width is
    named by the instruction, `addl -8(%rbp), %eax`, and in a
    condition an object against a constant is the instruction on the
    object, `cmpl $5, -4(%rbp)`. The rule is the Seventh Edition
    compiler's row `%n,aw`; that compiler also exchanged a
    relational's operands, and rcc does not reorder. aarch64 has no
    instruction with a memory operand, and nothing to take. The load
    stays beyond the rule's boundary: for a `char` promoted, an `int`
    widened, a compound assignment's right operand and a shift's
    count.
15. **On x86-64 a division and a shift by a variable count choose
    their registers** (6.5). A node chooses for its own operands, as
    the Seventh Edition compiler's `oddreg()` chose a division's pair
    from its operator. A node whose right operand's top operator is
    one holds its left operand out of the registers that instruction
    pins; that rule has no counterpart in the ancestor, whose `div`
    takes any even-odd pair. The cost is the one exception to 6.1's
    "nothing looks at a node's neighbours", and a boundary: a division
    deeper in the right operand changes nothing.
