# rcc: the cost model

What each construct compiles to. `guarantees.md` says what a program
means where ISO C says nothing; this page says what it costs, on
every construct. The two together are the contract: the compiler
does what the source says, and only that.

## 0. tl;dr

- The work the source writes is the work executed. Every load,
  store, call and operation is made, in the written order, and nothing
  is inferred: `x / 4` divides, `x * 4` multiplies, `x % 1` divides,
  `x >>= 2` shifts, and a loop invariant is computed at every
  iteration. Choosing the cheaper way to do the work is the
  programmer's job, and the compiler never second-guesses it. An
  operator whose constant operand makes it the identity does no work
  and writes no instruction: `a + 0`, `a - 0`, `a | 0`, `a ^ 0`,
  `a << 0`, `a >> 0`, `a * 1`, `a / 1` and `a & -1`, and `0 + a`,
  `1 * a`, `0 | a`, `0 ^ a` and `-1 & a`. Its other operand is still
  evaluated, with every load, call and fault in it, and converted as
  the operator's type asks: `f() + 0` calls `f`, and `c + 0` on a
  `char` is the extending load. `a * 0` is no identity, since its
  value is not `a`'s.
- The compiler decides three things of its own: the register that
  holds a value, by the discipline of `backend.md` chapter 4; the
  machine form of each operator, by the table of chapter 2; and the
  calling convention, System V AMD64 on x86-64 and the AAPCS64 on
  aarch64. Nothing else, but a convention of the target like the
  stack's 16 bytes at a call: on x86-64 every loop head is aligned to
  32 bytes, and on aarch64 none is (4.2).
- Every statement is written, its value used or not. `a + 0;` is the
  load of `a`, and `a / b;` divides, and the division can fault.
- Locals live in memory, one stack slot each, for the whole call. A
  local or parameter declared `register` lives in one of five
  callee-saved registers on x86-64, `rbx` and `r12` to `r15`, or one
  of ten on aarch64, `x19` to `x28`, and has no slot of its own; its
  register is stored to a spill slot only to save a read of it held
  across a branch that writes it (`backend.md` 6.7). One whose
  register would be a sixth on x86-64, an eleventh on aarch64, takes a
  slot, with a warning. The keyword is the allocation, as it was on
  the PDP-11.
- Chapter 4 holds the measurement under c1. On x86-64 the plain
  source runs 2.33 to 8.21 times as long under rcc as under `clang
  -O2`, and the source tuned by hand for rcc 1.60 to 2.24 times. As
  first written, matmul's tuned `main` held ten `register` variables
  live, put five in slots and ran 7.54 times as long. Under the LLVM
  pipeline the two ranges were 1.49 to 8.43 and 1.33 to 4.40. A
  stack slot that a loop carries adds 6.01 to 6.81 clock cycles an
  iteration on the Zen 2 core of an x86-64 desktop (4.3).
- On aarch64 the plain source runs 1.61 to 7.42 times as long on an
  Apple M1 Max and 1.92 to 9.67 times on a Qualcomm Oryon, and the
  tuned source 1.22 to 2.41 and 1.33 to 2.29 times. A stack slot that
  a loop carries from one iteration to the next adds 0.37 to 0.65
  clock cycles an iteration on the M1 Max's Firestorm core, and 6.00
  to 6.62 on the Oryon (4.3).

## 1. The rule

c1 writes the instruction for each operator of the tree, in the
written order: one machine instruction, or the machine's fixed
sequence, per operator (`backend.md` 6.1). Between the parser and the
assembler, nothing reasons about the program. The parser folds an
integer constant expression to the number it spells, since it is a
spelling and not an operation on data (`backend.md` 6.4); a variable
never folds. An identity of chapter 0 is no operator of the tree c1
reads: the parser leaves its node out and keeps its other operand, of
the operator's type, so a program compiles to the listing of its twin
with each identity's operand converted in its place, `(int) c` for
`c + 0` on a `char` (`add-zero`). A compound assignment is no
identity: `x += 0` is a read and a store the source writes, and it
writes its operator with them, `addl $0, -4(%rbp)` on x86-64 and
`add w0, w0, #0` between the `ldr` and the `str` on aarch64.
A typedef name is another spelling of its type and
writes nothing: a program compiles to the listing of its twin with
every type spelled out (`typedef` in the cost corpus). An enumerated
type is another spelling of the integer type compatible with it, and
an enumeration constant of its number: a program compiles to the
listing of its twin with the integer types and the numbers spelled
out (`enum`). A static assertion declares nothing and writes
nothing: a program compiles to the listing of its twin without its
assertions (`static-assert`). A binary constant is the spelling of its
number as a hexadecimal one is (`binary-constant`). Neither do
`restrict`, `inline` on a declaration, `__extension__` and the
attributes rcc drops, which are licences for an optimizer or
requests for a warning (`gnu-declarations`); `__asm__` after a
declarator changes the name a call writes and nothing else.
`volatile` writes nothing either: it asks that every access written
be made, and every access of every object is, so a program compiles
to the listing of its twin without the qualifier (`volatile`). Nor
does `inline` on the definition of a program's own `static`
function: it suggests that calls be fast, and each call is written
as it is either way, so a program compiles to the listing of its
twin without the specifier (`static-inline`). One whose body holds
what rcc does not compile yet is kept uncompiled and writes nothing
at all (`static-inline-not-yet`). Nor does
`__builtin_expect(e, c)` of a constant `c`: it is `e` converted to
`long`, so a program compiles to the listing of its twin with
`(long)(e)` in its place (`builtin-expect`). Nor does the attribute
`noinline`: it asks that calls of a function not be inlined, and c1
inlines none, so a program compiles to the listing of its twin
without it (`noinline`). An alias declares a symbol of its target's
address, which the assembler gives it, and costs no instruction: its
lines follow the data (`alias`). Arithmetic on a pointer to `void`,
which GNU C takes, counts bytes, so a program compiles to the listing
of its twin with `char *` in place of each `void *`
(`void-arithmetic`). `return E;` of a void `E` in a void function,
which GNU C takes, is `E;` and `return;`, so a program compiles to
the listing of its twin written so (`void-return`). A pointer to
`void` and a pointer to a function converted as if by assignment,
which GNU C takes, are the conversion a cast makes, so a program
compiles to the listing of its twin with the cast written
(`void-function-pointer`). Nor does the attribute `always_inline`,
which rcc drops with a warning: it asks that each call be inlined, c1
inlines none and writes the call, so a program compiles to the
listing of its twin without it (`always-inline`).

What the compiler may do:

- Allocate registers, by the discipline, not by an allocator. A
  `register` variable takes the next free one of the five, ten on
  aarch64, at its declaration and keeps it to the end of its block. A
  later declaration of a block takes one above every register its
  closed inner blocks took, since its object is live from the entry
  into its block (6.2.4p6). An expression value takes the lowest free
  one of the nine caller-saved registers, fifteen on aarch64, and when
  none is free the oldest held value is spilled (`backend.md` chapter
  4 and 6.2, `aarch64.md` chapter 4). On x86-64, where a division pins
  `rax` and `rdx` and a shift by a variable count `rcx`, a divisor and
  a shift's left operand take the lowest free one outside the
  registers their instruction pins, and so does a value held while
  such an instruction is generated as a node's right operand, while
  one is free (`backend.md` 6.5). A local declared in the source lives
  in its own stack slot regardless, unless it is declared `register`.
  An object at file scope lives at its label: one the file defines is
  reached PC-relative at each access, and one it only declares has its
  address loaded from the GOT at each access (`backend.md` chapter
  10). A block's `static` object lives at a label of its own, as one
  the file defines. The extent to which `register` is effective, which
  6.7.2p12 leaves implementation-defined and 4p10 asks to be
  documented, is five registers on x86-64 and ten on aarch64, taken by
  the rule above.
- Select the machine form, by the table. On x86-64 a `+` is an `add`;
  a 32-bit signed division is `cltd; idivl`; a comparison is `cmp` and
  a `setcc` for a value, `cmp` and a conditional jump for a condition;
  a condition whose operator's own instruction sets the zero flag from
  its value is that instruction and the jump on its flags, `subl $1,
  -4(%rbp); je` for `while (--n)` and `testl $1, -4(%rbp); jne` for
  `if (n & 1)`, as the Seventh Edition compiler's `cctab` compiled a
  condition (`backend.md` 7.1); a subscript is the access's
  scaled-index address, `(%rax,%rcx,4)`, and on a local array the slot
  is its displacement off `rbp`, `-24(%rbp,%rax,4)`, with no
  instruction forming the array's address. An object reached by its
  name plus a constant is one operand, the constant in its
  displacement, `movl -20(%rbp), %eax` for `a[3]` on a local `int
  a[8]` and `movl table+12(%rip), %eax` for `table[3]` on a global the
  file defines, as the Seventh Edition compiler's `&x+c` was
  (`backend.md` 6.3). Any other lvalue's address plus a constant is
  that address with the constant in its displacement, `movsbl
  2(%rax,%rcx,8), %eax` for `p[i][2]` on a `char (*p)[8]`, and on
  aarch64 where the address is a base and a displacement, `ldr w0,
  [x0, #20]` for `p[1][2]` on an `int (*p)[3]` and `add x0, x0, #20`
  for its address. A cast that widens a signed object in memory is the
  object's load, extending to the cast's width, `movslq -4(%rbp),
  %rax` for `(long) i` and `ldrsw x0, [x29, #16]` on aarch64, as the
  Seventh Edition compiler's table converted an operand in the
  instruction that read it (`backend.md` 6.3). On x86-64 an object in
  memory as the right operand of an operation of its width is the
  instruction's own operand, `addl -8(%rbp), %eax` and `idivl
  -8(%rbp)`, after the left operand's load; and as a condition an
  object in memory tested alone, compared with a constant or `&` a
  constant is the `cmp` or `test` of the object itself, `cmpl $5,
  -4(%rbp)` and `testl $1, -4(%rbp)`; as the Seventh Edition
  compiler's table named an addressable operand in the instruction
  (`backend.md` 6.1, 7.1). On aarch64, which has no instruction with
  a memory operand, every operand is loaded. The form is fixed per
  operator and target, and listed in chapter 2.
- Follow the calling convention: argument registers, the stack
  frame, callee-saved registers.
- Align every loop head to 32 bytes on x86-64, `.p2align 5`, a
  convention of the target like the stack's 16 bytes at a call
  (4.2). On aarch64 it aligns none: alignment there is measured, not
  adopted (4.2, `aarch64.md` 7.2). A loop is one the language names,
  `while`, `do` or `for`: a loop built of `goto` is laid out as
  written, and no label of the source is aligned.
- Choose a `switch`'s dispatch by the rule of the Seventh Edition
  compiler (`backend.md` 7.3), from the case values alone, the same on
  both targets. With `ncase` the count of the cases and `range` the
  greatest value less the least, read unsigned at the promoted type's
  width:
  - no case: the value, and a jump to the default or past the body;
  - **direct** when `range > 0` and `range <= 3 * ncase`: the same
    instructions whatever the value, the subtraction of the first
    value when it is not 0, a compare, a jump to the default, the
    table's address and the jump through it, four to six on x86-64
    and six or seven on aarch64, and the loads of a constant the
    instruction does not encode; and a table of `range + 1`
    addresses, 8 bytes each, in `.data`;
  - **simple** otherwise when `ncase < 10`: a compare and a jump per
    case in the order of the values, so the k-th smallest value costs
    k of each, and a value no case holds costs all of them and a jump;
    a constant the compare does not encode costs its loads first;
  - **hash** otherwise: an unsigned division by a table size of
    `ncase / 4` to `ncase / 2`, a jump through a table of that size,
    and the compares of the quotient in one bucket, as many as the
    most cases one bucket holds, at most.

  The form is thus the programmer's to choose by the values: cases
  close together are a table, few cases a chain, and many spread
  cases a division.

What the compiler never does:

- Fold. `x * 3` is a multiply, `x / 4` is a division, `a * 0` is a
  multiply, `a == a` is a compare. An identity, which does no work,
  writes nothing (chapter 0).
- Move. A loop-invariant expression is computed every iteration. A
  constant used in a loop is materialized in the loop.
- Merge. The same expression written twice is computed twice.
- Reorder. Instructions come out in the written order; blocks come
  out in the written order.
- Delete. Nothing: a statement whose value is unused is computed,
  and every `return` jumps to the epilogue, the last one included.
- Convert a branch to a conditional move or the reverse.

## 2. What each construct compiles to

A table per target. Each row is the form c1 writes, in the text of
the target's pin of the cost program named in its last column, whose
opening comment says what its listing must show on each target. A
row with no program is stated from `backend.md` 6.3; no pin shows it
yet.

### 2.1 x86-64

| Source | Machine | Program |
|---|---|---|
| a local | a slot of its own off `rbp` for the whole call; a load at each read, `movl -8(%rbp), %eax`, and a store at each write, `movl %eax, -8(%rbp)`; a constant stored by a statement or an initializer is the one store of the immediate, `movl $0, -8(%rbp)` | `local`, `while` |
| a parameter | its argument register stored to its slot in the prologue, `movl %edi, -4(%rbp)`; then the local's row; one past the sixth read where the caller pushed it, above the return address, with no copy, `movsbl 32(%rbp), %ecx` for a `char` and `addq 40(%rbp), %rax` for a `long`, its slot the right operand of the `addq` | `parameter`, `stack-parameter` |
| a `register` local | the next of `rbx`, `r12` to `r15`, by declaration order, pushed after `rbp` in the prologue and popped before `leave`; no slot, no load, and no store but the save of a read of it held across a branch that writes it, `movq %rbx, -24(%rbp)` (`backend.md` 6.7); a read is the register as an operand, `cmpl %ebx, %r13d`; an instruction that writes its destination works on a copy, `movl %ebx, %eax; subl %r12d, %eax`, but where it computes a value assigned to the variable, below, `addl %r13d, %r12d` for `s = s + i`, and where an operator that commutes has a right operand in a register and no free register is named, which the instruction writes, the variable its source, `addl %ebx, %eax` for `s = r + f(x)` (`backend.md` 6.1); a constant assigned is the immediate, `movl $0, %r12d`; across a loop, the same register | `register-loop`, `register-parameter`, `register-copy` |
| a `register` local without an initializer | the register as its declaration finds it, from the prologue or from an earlier block's variable; nothing writes it until the program does but the extension of a `char`, `short` or `unsigned` one in its own register at its declaration, `movsbl %bl, %ebx` or `movl %ebx, %ebx`, which keeps the value of its type, so every read sees one value (G5) | `register-undef`, `register-destination` |
| a `register` parameter | moved from its argument register to its register in the prologue, `movl %edi, %ebx`; then the register | `register-parameter` |
| `r = e` on a `register` variable | the value computed in the variable's register: a load, `movl -12(%rbp), %ebx`; an address, `leaq defined(%rip), %rbx`, `leaq -12(%rbp), %rbx`, `leaq -28(%rbp,%rax,4), %rbx` for `&a[i]`, `leaq (%rax,%rcx,4), %rbx` for `p = q + n`, and the GOT's `movq declared@GOTPCREL(%rip), %rbx`; an operation `+`, `-`, `&`, `\|`, `^`, `*` or a shift whose right operand is a leaf that does not name `r`, the left operand loaded into the register and the instruction written on it, `movl -12(%rbp), %ebx; addl -16(%rbp), %ebx`, `addl $5, %ebx`, `addl %ebx, %r12d` for a register variable, `addl (%rax), %ebx` after `p`'s load, `addl %eax, %ebx` after a `char`'s `movsbl -13(%rbp), %eax`, an element by a constant index, `addl table+12(%rip), %ebx`, and `addl 12(%rax), %ebx` after `p`'s load; another register variable's read copied, under conversions that write nothing where it is valid to the width, `movl %ebx, %r12d; addl $5, %r12d` for `r = c + 5` on a register `char c`, and the extension written into the register, `movslq %ebx, %r12; addq $5, %r12` for `r = i + 5L` on a register `int`; `r = x * 4` on a register variable `imull $4, %ebx, %r12d`; `r = r + b` the instruction alone, `addl -16(%rbp), %ebx`; pointer arithmetic by `addq` or `subq` the same, `p = p + 1` the `addq $4, %rbx` alone, `p = q - 1` `movq -16(%rbp), %rbx; subq $4, %rbx`, `p = p - n` `imulq $4, -24(%rbp), %rax; subq %rax, %rbx`, and `p += n` as `p = p + n`, `leaq (%rbx,%rax,4), %rbx` at a size of 2, 4 or 8, and at a size of 1 an `n` in memory the operand, `addq -24(%rbp), %rbx`; `r = -x` `movl -12(%rbp), %ebx; negl %ebx`, `~x` `notl %ebx`, `r = -r` the `negl` alone; a comparison's value `setl %bl; movzbl %bl, %ebx` after the `cmp`, into an `int`, a `long` or an `unsigned long`, `sete %bl` after `cmpl $5, -12(%rbp)`; `r = a + (b + c)`, `r = a - r`, `p = q + p[1]` and a division computed in `eax` or `rax` and moved, `movl %eax, %ebx`, `movq %rax, %rbx`; an initializer the same, `movl -12(%rbp), %ebx; imull -16(%rbp), %ebx`; through a conversion that changes the signedness alone the same, `movl -12(%rbp), %ebx` for `register unsigned r = i`, `movsbl -9(%rbp), %ebx` for `register char r = uc`, `movl -12(%rbp), %ebx; addl -16(%rbp), %ebx` for `r = a + b` on two `int`s into an `unsigned`, `movl -12(%rbp), %ebx; addl $5, %ebx` for `v = 5 + (int) u` on an `unsigned u`, and `setl %bl; movzbl %bl, %ebx` for `r = a < b`; an `unsigned` operation widened, whose 32-bit write is the widening, the same, `movl -12(%rbp), %ebx; imull -16(%rbp), %ebx` for `r = a * b` into an `unsigned long`; any other conversion that narrows or widens keeps the `mov`, `movq -16(%rbp), %rax; movl %eax, %ebx` for `r = (int) l`; a sum whose left operand is an address, `leaq characters(%rip), %rbx; addq -16(%rbp), %rbx` for `p = characters + n`, `leaq rows(%rip), %rbx; imulq $12, -16(%rbp), %rax; addq %rax, %rbx` for `p = rows + n`, for `p = &rows[i]` and for `p = &rows[i][0]` and `p = rows[i]` into an `int *`, and two `addq` after the `leaq` for `p = characters + i + i`; `p = &*q` and `p = &q[0]` the load of `q`, `movq -16(%rbp), %rbx`; `r = !x` `sete %bl; movzbl %bl, %ebx`; `r = x++` the load into the register, `movl -12(%rbp), %ebx`, then `movl %ebx, %eax; addl $1, %eax; movl %eax, -12(%rbp)`, `r = ++x` the load, the step and the store on the register, `movl -12(%rbp), %ebx; addl $1, %ebx; movl %ebx, -12(%rbp)`, `r = ++c` on a `char` the step in `eax` and `movsbl %al, %ebx`, and `r = s++` on a register variable `movl %ebx, %r12d` before `addl $1, %ebx`; a value extended into the register, `movslq %eax, %rbx` for `r = a + b` into a `long` from two `int`s, `movslq %ebx, %r12` for `r = i` from a register `int`, `movslq -16(%rbp), %rbx` and `movslq -20(%rbp), %rbx` on the two paths of `l = c ? x : y` into a `long` and `movslq -16(%rbp), %rbx` and `movq $-1, %rbx` on those of `l = c ? x : -1`, `movsbq -13(%rbp), %rbx` and `movsbq -14(%rbp), %rbx` on those of `l = c ? a : b` on two `char`s, the promoted load extended to the variable's width; `q = &(c ? m : n)[1][2]` into a register `int *` from two pointers to rows `movq -24(%rbp), %rbx` and `movq -32(%rbp), %rbx` on the paths and `leaq 20(%rbx), %rbx`, a pointer's conversion writing nothing; `u = i + ub` `movl -12(%rbp), %ebx; addl -16(%rbp), %ebx` and `u = c + b` on a `char` `movsbl -9(%rbp), %ebx; addl -16(%rbp), %ebx`; `r = a && b` and `r = a || b` `movl $1, %ebx` and `movl $0, %ebx` on the two paths, `r = c ? x : y` each arm into the register, `movl -16(%rbp), %ebx` and `movl -20(%rbp), %ebx`, and `r = c ? r + 1 : 0` `addl $1, %ebx` and `movl $0, %ebx`, with no `mov` after the join, an arm that is the variable's own read nothing on its path, whatever the other arm is, the condition's one jump going to the join past the other path with no `jmp`, `jne .Lchosen_own_first.3` over `movl -16(%rbp), %ebx` for `r = c ? r : x`, `je` over the load for `r = c ? x : r`, and `je` over the `call` and `movl %eax, %ebx` for `r = c ? f(x) : r`, and `r = r` nothing at all, and `r = c ? f(x) : 0` and `r = c ? f(x) : p[i]++` the call's result moved in on its own path, `movl %eax, %ebx`, and the second arm's row into the register, `movl $0, %ebx` and the step's load `movl (%rax,%rcx,4), %ebx`; the `mov` kept for `r = a[r]++`, whose address reads the variable, for `p = characters + *p`, whose right operand names it, and after the join of `r = c ? f(x) : a / b`, where neither arm's row writes the variable (`backend.md` chapter 4) | `register-destination`, `register-blocks` |
| `r += e`, `++r` on a `register` variable | the operation on the register in place, `addl $5, %ebx` and `addl $1, %ebx`, no load and no store (`backend.md` chapter 4) | `register-rules` |
| a narrow `register` variable | a value moved in by the extending `mov`, `movsbl %al, %ebx` after a call; a step on it re-extended after, `addl $1, %ebx; movsbl %bl, %ebx`; unwritten, extended in its own register at its declaration, `movsbl %bl, %ebx` (`backend.md` chapter 4) | `register-rules` |
| a held read of a `register` variable that is then written | copied into an expression register before the write, `movl %ebx, %ecx` before `addl $1, %ebx` in `i + i++` (G13); before the paths of `&&`, `\|\|` and `?:` of which one writes it, the register stored, `movq %rbx, -24(%rbp)`, and the read reloaded where the paths meet, `movq -24(%rbp), %rax` (`backend.md` 6.7) | `register-rules` |
| a `register` parameter past the sixth | loaded from its slot above the return address into its register, `movl 16(%rbp), %ebx`, after the stores of the six; the moves of `register` parameters and the stores of the others in parameter order, `movl %edi, -20(%rbp); movl %esi, %ebx; movl %edx, -24(%rbp); movl %ecx, %r12d` (`backend.md` chapter 4) | `register-rules` |
| the epilogue of a function with pushes and a `sub` | `leaq -8k(%rbp), %rsp` before the pops, `leaq -8(%rbp), %rsp; popq %rbx; leave` for one push; a frame without a `sub` pops at once (`backend.md` chapter 5) | `register-rules` |
| a sixth `register` variable live | a slot and the warning, `movl %eax, -52(%rbp)`; a later block's variable takes `rbx` again, `imull $2, -44(%rbp), %ebx` (`backend.md` chapter 4) | `register-rules` |
| the frame | `pushq %rbp; movq %rsp, %rbp` in every function and `leave` in the one epilogue; a function that calls nothing keeps its slots in the red zone below `rsp` and writes no `sub`; one that calls subtracts its frame, rounded so that `rsp` is a multiple of 16 at the `call`, `subq $16, %rsp` | `local`, `call` |
| `a + b`, `a - b` | `addl -8(%rbp), %eax`, `subl -8(%rbp), %eax`, the right operand's slot the instruction's operand after the left operand's load; the left operand's register is the result's | `add-sub`, `parameter` |
| an object in memory as the right operand | of the operation's width, the instruction's own operand, and no load of its own: in `add`, `addl -24(%rbp), %eax` for a local, `addl defined(%rip), %eax` for a global the file defines, `addl (%rcx), %eax` after the GOT's load, `movq declared@GOTPCREL(%rip), %rcx`, and after `p`'s, and `addl -40(%rbp,%rcx,4), %eax` after `movslq -20(%rbp), %rcx` for `a[i]`; `subl`, `andl`, `orl`, `xorl`, `imull`, `idivl` and `cmpl` the same; an `int` under the cast to `unsigned` of `u + i` the same, `addl -8(%rbp), %ecx`, and `cmpl`, `imull` and `divl -8(%rbp)` in `signedness`; the load kept for a `char` promoted, `movsbl -5(%rbp), %ecx; addl %ecx, %eax`, an `int` widened, `movslq -12(%rbp), %rcx; addq %rcx, %rax`, a compound assignment's right operand into an object in memory, `movl -8(%rbp), %eax; addl %eax, -4(%rbp)`, where into a register variable it is the operand, `addl -16(%rbp), %ebx`, `andq -24(%rbp), %rbx` on a `long`, a shift's count, `movl -8(%rbp), %ecx; shll %cl, %eax`, and an index pointer arithmetic scales, `movq -40(%rbp), %rdx; leaq (%rax,%rdx,4), %rax`; a global read before a call on the right, loaded, spilled across the call and reloaded, `movq -8(%rbp), %rcx; cmpl %eax, %ecx` (`backend.md` 6.1) | `operand` |
| an object in memory compared with a `register` variable | the object the `cmp`'s own operand and the variable's register its source, `cmpl %ebx, -12(%rbp)`, then the jump or the `setcc`, and no load; an expression on the right keeps the object's load, `movl -12(%rbp), %eax; movl %ebx, %ecx; addl $1, %ecx; cmpl %ecx, %eax` for `x == r + 1` (`backend.md` 6.1) | `operand` |
| an identity, `a + 0`, `a * 1`, `a & -1` and the rest of chapter 0 | no instruction: the other operand's row alone, `movl -4(%rbp), %eax` for `a + 0`, `movsbl -1(%rbp), %eax` for `c + 0` on a `signed char` and `movslq -4(%rbp), %rax` for `i + 0L`, the extension kept; `a * 0` is no identity, `imull $0, -4(%rbp), %eax`, a constant operand the immediate whatever its value | `add-zero` |
| `a * b` | `imull -8(%rbp), %eax`; by a constant, the three-operand form with the object in memory as its source and no load, `imull $3, -4(%rbp), %eax`, by a power of two the same, `imull $4, -4(%rbp), %eax` and no shift, and `imulq $1000, -16(%rbp), %rcx` for a `long`; on a value in a register `imull $3, %eax, %eax`; on a register variable the three-operand form with its register as the source, `imull $4, %ebx, %ecx`, and `imull $4, %ebx, %eax` for a return; `4 * x` as `x * 4` is, `imull $4, -4(%rbp), %ecx` (`backend.md` 6.1) | `mul`, `mul-three`, `operand`, `return-register`, `power-of-two` |
| `a / b`, `a % b` | `cltd; idivl -8(%rbp)` signed, `xorl %edx, %edx; divl -8(%rbp)` unsigned, the divisor's slot the operand; the quotient in `eax`, the remainder the value in `edx`, which a return moves, `movl %edx, %eax`, and a store or a next row takes from there (`backend.md` 6.5) | `div-rem`, `udiv-urem` |
| `a % b` on a `register` variable | the left operand copied from its register to where the division wants it, `movl %ebx, %eax`, then the row above, `cltd; idivl %ecx; movl %edx, %eax`, and `xorl %edx, %edx; divl %ecx` unsigned; the variable is not written | `register-remainder` |
| `x / 4`, `x / 7` | the divisor held in a register, `movl $4, %ecx`, since `idiv` takes no immediate, then `cltd; idivl %ecx`, and on an `unsigned` `xorl %edx, %edx; divl %ecx`, `x % 4` then `movl %edx, %eax`; no shift, no mask, no multiply by a magic constant | `div-four`, `div-seven`, `power-of-two` |
| `a + b / c`, a division as a node's right operand | the node's left operand held outside `rax` and `rdx`, `movl -4(%rbp), %ecx`, the dividend loaded into `eax`, `cltd; idivl -12(%rbp)`, then `addl %eax, %ecx`, or under a `return` `addl %ecx, %eax`, which writes the quotient's `rax`, and no move; the division read through a cast; the same under a comparison, `cmpl %eax, %ecx`, a subscript, `movl (%rcx,%rax,4), %eax` after `movslq %eax, %rax`, and an assignment's place, `a[i] = (x >> 16) % 256` with the address in `rcx` and `rsi` and the store from `edx`; a divisor and its address loaded outside `rax` and `rdx` too, `movq -16(%rbp), %rsi; idivl (%rsi)` for `*p`; a divisor's address in `rax`, a call's result, moved out, `movq %rax, %rsi`, and the dividend reloaded into `rax`, `movq -16(%rbp), %rax`, for `x % f()[i]` (`backend.md` 6.5) | `div-busy`, `pinned`, `operand` |
| a division with `rax` and `rdx` busy | where the rule does not reach, a division deeper in the right operand, `a + (b / c + 1)`, or where no other register is free: the values held there moved out to free registers, `movq %rax, %rsi`, the dividend moved into `eax`, then `cltd; idivl -12(%rbp)`; the quotient moved into the dividend's register, `movl %eax, %ecx`; with no register free the oldest held value spilled first, `movq %rcx, -32(%rbp); movq %rax, %rcx` (`backend.md` 6.5) | `div-busy` |
| `a << n`, `a >> n` | the count in `ecx`, `shll %cl, %eax`, `sarl %cl, %eax`, `shrl %cl, %eax`; the instruction masks the count; the left operand loaded outside `rcx`, and a node's left operand or an assignment's place held outside it while the shift is generated: `b[i] = x << n` on a local array is `movslq -4(%rbp), %rax; movl -8(%rbp), %edx; movl -12(%rbp), %ecx; shll %cl, %edx`, and no move (`backend.md` 6.5); the count's last instruction writes `ecx` where it is free, `movsbl %al, %ecx` after a call with a `char` result and `movl %edx, %ecx` for the old value of `n++`, and a count that is a call's `int` result or is generated while `rcx` holds a live value is moved in, `movl %eax, %ecx` (`backend.md` 6.1) | `shift`, `pinned` |
| `-a`, `~a` | `negl %eax`, `notl %eax`; widened after, `-(unsigned) a` to `unsigned long` is the `negl` alone, since the 32-bit write cleared the upper half, and `-a` to `long` is `negl %eax; movslq %eax, %rax`; the parser folds `-1` to a constant, so it is no instruction of this row | `negate` |
| `+a` | no instruction of its own: the promotion of `a` alone, as `(int) a` writes it; a `char` or a `signed char` its extending load, `movsbl -1(%rbp), %eax`, an `unsigned short` `movzwl -2(%rbp), %eax`, and an `int` or a wider type its load alone, `movl -4(%rbp), %eax`, `movq -8(%rbp), %rax`; a `register` variable its register, `movl %ebx, %eax` for a return; as an operand the same, `movsbl -5(%rbp), %ecx; addl %ecx, %eax` for `a + +b` | `unary-plus` |
| `a < b` and the rest, as a value | `cmpl -8(%rbp), %eax; setl %al; movzbl %al, %eax`, `sete` for `==`; no jump; an object in memory against a constant the `cmp` of the object and no load, `cmpl $5, -4(%rbp); setl %al; movzbl %al, %eax`, `setae` on an `unsigned` (`backend.md` 6.1); the value 0 or 1 valid to 64 bits, so `(long) (a < b)` and `(unsigned long) (a < b)` write nothing after the `movzbl`, nor `(long) !a` after its `movzbl` or `(long) (a && b)` after its arms' `movl` (`backend.md` 6.3) | `compare`, `operand` |
| `x == x` on a `register` variable | `cmpl %ebx, %ebx`: the compare is written | `register-compare` |
| `a < b` as a condition | `cmpl -8(%rbp), %eax` and a jump on the inverse to the else label, `jge .Lchoose.2`; no value | `if` |
| a condition on an object in memory and a constant | the `cmp` or `test` of the object itself, at its width, and no load: `cmpl $5, -4(%rbp); jge .Lless_constant.2` for `x < 5`, `cmpl $0, -4(%rbp); je .Lnonzero.2` for `if (x)`, `cmpb $0, -1(%rbp)` on a `char`, and `testl $1, -4(%rbp); je .Lbit.2` for `x & 1`; through a pointer the same after its load, `cmpl $0, (%rax)` (`backend.md` 7.1) | `operand`, `if-jump` |
| a comparison with a constant on the left | the mirrored comparison, the constant the immediate on the right, and no load of it: `5 < x` on an `int` in memory is `cmpl $5, -4(%rbp); jle .Lconstant_left.2`, as `x > 5` is, and `jbe` on an `unsigned`; on a register variable `cmpl $5, %ebx`; for its value `cmpl $5, -4(%rbp); setg %al`, and no load; `0 == p` is `cmpq $0, -8(%rbp); jne .Lnull_left.2`; `0 == (c & 1)` jumps on the `test`, `testl $1, -4(%rbp); jne .Lzero_equal.2`; a constant no 32-bit immediate holds keeps the order of the source (`backend.md` 6.1) | `operand`, `condition-flags` |
| a constant on the left of `+`, `*`, `&`, `\|` or `^` | taken as the constant on the right is, the operands' order exchanged, and no load of it: `5 + x` is `movl -4(%rbp), %eax; addl $5, %eax`, as `x + 5` is, `4 * x` `imull $4, -4(%rbp), %ecx`, `7 & x`, `7 \| x` and `7 ^ x` the load and `andl $7`, `orl $7` and `xorl $7`, and `5 + c` on a `char` `movsbl -5(%rbp), %ecx; addl $5, %ecx`; on a register variable `movl %ebx, %eax; addl $5, %eax`; `5 + f()` the `call` and `addl $5, %eax`, with no spill; into a register variable `imull $4, -12(%rbp), %ebx` for `r = 4 * a`, `addl $5, %ebx` alone for `r = 5 + r` and `movl %ebx, %r12d; addl $5, %r12d` for `r = 5 + c` on a register `char c`; `if (1 & x)` `testl $1, -4(%rbp); je .Lcommuted_bit.2`; held first where it keeps the order, a constant no 32-bit immediate holds, `movabsq $4294967296, %rax; addq -8(%rbp), %rax`; `5 - x`, `40 / x` and `1 << x` keep the constant first (`backend.md` 6.1) | `operand` |
| a condition whose operator's instruction sets the zero flag: `a + b`, `a - b`, `a \| b`, `a ^ b`, `-a`; `a & b`; `x += e`, `-=`, `&=`, `\|=`, `^=`, `++x`, `--x`; `== 0` and `!= 0` over one | the operator's instruction and `je` or `jne` on its flags, no `cmp`: `addl -8(%rbp), %eax; je .Ladd.2`, and `negl %eax; je .Lneg.2`; `&` as `test`, no value written, `testl -8(%rbp), %eax`, `testl $1, -4(%rbp)` on a parameter in its slot, and `testl -16(%rbp), %ebx` on a register variable, which is not copied; the statement's form on the object, `addl %eax, -12(%rbp)`, `addb %al, -9(%rbp)` on a `char`, `addl %eax, counter(%rip)` and `subl $1, counter(%rip)` on a global, the operation in place on a register variable, `addl -16(%rbp), %ebx` with `b`'s slot its operand; `(a & m) == 0` is `testl -8(%rbp), %eax; jne .Lequal_zero.2`; the `cmp` stays after `imull`, `shll %cl` and a postfix step's copy of the old value, after a `register char`'s re-extension, `movsbl %bl, %ebx; cmpb $0, %bl`, and for `(a + b) < 0`, `addl -8(%rbp), %eax; cmpl $0, %eax; jge .Lnegative.2` (`backend.md` 7.1) | `condition-flags`, `if-jump` |
| `a && b`, `a \|\| b` | jump code: each operand compared with 0 in its slot, `cmpl $0, -4(%rbp)`, and a jump, `je .Lboth.2` for `&&`, `jne` past the right operand for `\|\|`; as a value, `movl $1, %eax` and `movl $0, %eax` on the two paths | `logical` |
| `a ? b : c` | the condition by jump code, `cmpl $0, -4(%rbp); je .Lconditional.2`, each arm into `eax`, a `jmp` past the second arm; never `cmov`; the arms join in the register the consumer names, a return's `eax`, a remainder moved there on its arm, `movl %edx, %eax`, a postfix step's copy written there, `movl %esi, %eax`, and no move after the join, and with no consumer naming one in the first arm's register, the second arm's load written there, `movl -16(%rbp), %edx` beside a remainder, and the store from it; under a widening each arm extended into the join and nothing after it, `movslq %edx, %rax` for a remainder returned as a `long` and the second arm's `movslq -12(%rbp), %rax`, `movq $4, %rax` for a constant, but two quotients joined in `eax` and `movslq %eax, %rax` after the join (`backend.md` 6.1) | `conditional`, `select`, `div-busy` |
| `a ? b : c` for its effect | two `void` operands, or two whose values are dropped: the condition by jump code, each arm for its effect, a `jmp` past the second arm and no join, `cmpl $0, -4(%rbp); je .Lpick.2; call f@PLT; jmp .Lpick.3; .Lpick.2: call g@PLT; .Lpick.3:`; a constant assigned in an arm its one store, `movl $1, x(%rip)`; a void conditional as an arm the same inside the arm, its end label followed by the outer arm's `jmp` | `conditional-effect` |
| `a ?: b` | GNU C's `a ? a : b` with `a` evaluated once: `a` into the register the arms join in, compared with 0 there and taken by a `jne` past `b`, which is written into the same register, `call f@PLT; cmpl $0, %eax; jne .Lcalled.2; call g@PLT` for `return f() ?: g()`, with no slot and no second read; `movl -4(%rbp), %eax; cmpl $0, %eax` for an int in memory, `cmpq $0, %rax` for a pointer, `movsbl -1(%rbp), %eax` before the `cmpl` for a `char`, converted before its test, and no `cmp` where the condition's instruction set the zero flag; into a register variable the variable's register on both paths, `movl -12(%rbp), %ebx; cmpl $0, %ebx; jne` and `movl -16(%rbp), %ebx`, but where `b` reads the variable; as a condition the join, then `cmpl $0, %eax; je`; for its effect `a`'s jump code past `b`, `call f@PLT; cmpl $0, %eax; jne .Ldiscarded.2; call g@PLT` | `conditional-omitted-operand` |
| `a, b` | no instruction of its own: `a` as the statement `a;` would be written, `addl $1, -8(%rbp)` for `i++`, then `b` as the value, the condition or the effect the comma is | `comma` |
| `if`, `while`, `for` | the condition by jump code at the top, where it is written, blocks in the written order; a loop's head is aligned to 32 bytes, `.p2align 5` before `.Lcount.2` in `while` and `.Lsum.2` in `for`, the gap filled by the assembler with multi-byte `nop`s; a loop jumps back from its bottom, `jmp .Lcount.2` in `while`, and `for`'s step follows the body under no label but where a `continue` names one; an empty arm is still tested, `je .Lempty_if.2` to the label that follows it in `cgp`; a join that holds only the jump back stays one block; an `if` whose body is one jump, a `break`, a `continue`, a `goto` or a `return` with no value, bare or alone in braces, is the condition's jump to the jump's target, taken when the condition holds, `testl $1, -4(%rbp); jne .Llow.3` for `if (n & 1) break;`, and an `else` part follows it, with no `jmp` and no label (`backend.md` 7.2) | `if`, `while`, `for`, `block-order`, `cgp`, `early-taildup`, `if-jump` |
| `if (0)`, `!0` | `if (0)` is a test, `movl $0, %eax; cmpl $0, %eax; je .Lnever.2`, and the body is kept; `!0` is the constant 1, folded in the parser | `zero-test` |
| `do s while (e);` | the body first, under the loop's head, aligned to 32 bytes, `.p2align 5` before `.Lcount.2`; the condition by jump code at the bottom, where it is written, under a label only where a `continue` names it, and the jump back while it holds, `jl .Lcount.2`; one test an iteration; a label past the test where a `break` jumps to it | `do` |
| `break`, `continue` | one `jmp` each, nothing held, `jmp .Ltally.7` for the `break` that ends a case; as an `if`'s body, the `if`'s one jump: `break` past the loop, `jg .Lsum.4`, `continue` to the condition in `while` and `do` and to the step in `for`, `je .Lsum.3` | `break-continue`, `if-jump` |
| `goto`, a label | one `jmp` to the label, and the label where it is written, `.L<function>.<n>` numbered as every label of the function is, when first met, at its definition or at a `goto`; the listing shows no C name; never aligned, so a loop built of `goto` has no `.p2align`, `jmp .Lcount.2` back to `.Lcount.2`; as an `if`'s body, the `if`'s one jump, `jge .Lcount.3` | `goto`, `if-jump` |
| `switch`, direct | the value less the first case's, `subl $1, %eax`, compared unsigned with the range, `cmpl $4, %eax; ja .Lday.6` to the default; the table's address and the jump through it, `leaq .Lday.8(%rip), %rcx; jmp *(%rcx,%rax,8)`; the table in `.data`, `.p2align 3` and a `.quad` per value from the first to the last, the default's label where no case holds the value; a first value of 0 is no `subl`, and an `int` then takes the width rule's `movl %eax, %eax` before the `leaq` | `switch-direct` |
| `switch`, simple | per case, in the order of the values, `cmpl $-1, %eax; je .Lkind.4`, a 0 as `cmpl $0, %eax`; then `jmp .Lkind.6` to the default | `switch-simple` |
| `switch`, hash | the unsigned division by the table's size, `movl $5, %ecx; xorl %edx, %edx; divl %ecx`; the jump through the table by the remainder, `leaq .Lreason.16(%rip), %rcx; jmp *(%rcx,%rdx,8)`; in each bucket the quotient compared with each of its cases' value divided by the size, `cmpl $40, %eax; je .Lreason.2`, then `jmp .Lreason.14` to the default | `switch-hash` |
| a switch's body | the labels where they are written, nothing between one case's statements and the next label, so control falls through, `addl $1, -8(%rbp)` then `.Lscore.3:` and `addl $2, -8(%rbp)`; a `break` is `jmp .Lscore.7` past the body; a default in the middle is one more label | `switch-fall` |
| `return e` | `e` computed in `eax`, which the return names, where its last instruction can write it, what is generated before that instruction outside `rax`: `movq -16(%rbp), %rcx; movslq -20(%rbp), %rdx; movl %ebx, %eax; subl (%rcx,%rdx,4), %eax` for `r - p[i]` on a `register int r`, `movslq %edx, %rax` for a remainder returned as a `long`, `setl %al` after `cmpl %eax, %ecx` for `a < b / c`, the quotient of `(a + b / c) / d` left in `eax`, and an assignment's value, a `+=`'s load and a postfix step's copy of the old value in `eax`, their place in `rcx` and `rdx`, and a register variable's read that the right operand writes copied or reloaded there, `movq -24(%rbp), %rax; addl %ecx, %eax` for `r + (c ? r++ : 2)`, a pointer plus a call's result written into `rax`, `addq -24(%rbp), %rax` for `p + f(x)` on a `char *p`, `p` read in its spill slot and not reloaded, and for a register pointer `p` `movq %rbx, %rax` and `addq %rcx, %rax` for `p + (i + (p = t, 0))`, `subq %rbx, %rax` before `cqto; idivq %rcx` for `p - (p = t)`; moved in otherwise, `movl %edx, %eax` for a remainder, `movl %ecx, %eax` after `r - f(x)`, and `movl %edx, %eax` after `r - p[i / 2]`, a division within the right operand, but a quotient left in `rax`, `idivq %rsi` last for `p - h(q)`, and an operator that commutes writing its right operand's `rax`, `addl %ebx, %eax` for `r + f(x)` and `addl -24(%rbp), %eax` for `r + (r = f(x))`, the copy read in its spill slot (`backend.md` 6.1, 6.5); then `jmp .L<function>.1` to the one epilogue, `jmp .Lsum.1` in `for`, the last statement included; a `return` with no value as an `if`'s body is the `if`'s one jump, `jne .Learly.1` | every program, `return-register`, `if-jump` |
| a statement whose value is unused | written: `a + 0;` is the load of `a`, `movl -4(%rbp), %eax`, `a / b;` is `cltd; idivl -8(%rbp)`, and the division can fault | `unused` |
| `f(a, b)` | each argument evaluated into a register and stored to a slot of its own, `movl %eax, -12(%rbp)`, a constant argument in one store of the immediate, `movl $1, -4(%rbp)` in `stack-args`; the slots loaded into `edi`, `esi` and on; `call f@PLT`; the result in `eax` | `call` |
| a seventh argument and on | stored to their slots as the first six are, a constant in one instruction, `movl $7, -28(%rbp)`, then pushed from their slots right to left, each loaded into `eax` and pushed from `rax`, `movl -28(%rbp), %eax; pushq %rax`, a `char` by its extending load, `movsbl -33(%rbp), %eax`; `subq $8, %rsp` first for an odd count; the bytes added back after the call, `addq $16, %rsp` for one or two, `addq $32, %rsp` for three or four | `stack-args`, `stack-args-nine` |
| `p(a, b)` | the callee first: `p` loaded and stored to a slot of its own, `movq %rax, -24(%rbp)`, a hidden local as each argument is; then the arguments as for `f(a, b)`; the call through the slot, `call *-24(%rbp)`, the machine's call through memory, with no register; the result in `eax` | `call-pointer` |
| `p(1, ..., 9)` | the callee's slot written first, `movq %rax, -16(%rbp)`, then the arguments' and the pushes as for a named call, `subq $8, %rsp` and three `pushq %rax`; `call *-16(%rbp)`, which the pushes do not move, since the slot is off `rbp`; `addq $32, %rsp` | `call-pointer-stack` |
| `(*p)(x)`, `(**p)(x)` | the listing of `p(x)`: the `*` writes no instruction | `call-pointer-star` |
| `(T) a` | `int` to `long` on an object in memory, its load and nothing after it, `movslq -12(%rbp), %rax`, and a `signed char` and a `short` the same, `movsbq -1(%rbp), %rax`, `movswq -2(%rbp), %rax`, a plain `char` as a `signed char`; through a pointer and on a global the same load, `movswq (%rax), %rax`, `movslq defined(%rip), %rax`, `movslq (%rax), %rax` after the GOT's load; an `unsigned char` its load, `movzbl -1(%rbp), %eax`, valid to 64 bits and nothing after it; under a cast that changes the signedness alone the load of the type under the widening, `(long) (int) u` `movslq -4(%rbp), %rax` and `(unsigned long) (unsigned) i` `movl -4(%rbp), %eax`, nothing after either, over a promotion the one load extended to the outer width, `movsbq -1(%rbp), %rax` for `(long) (int) c` on a `signed char` and `movzwl -2(%rbp), %eax` on an `unsigned short`, and `(long) (unsigned) c` on a `signed char` the one `movsbl -1(%rbp), %eax`, whose 32-bit write clears the upper half, with `movl %eax, %eax` after a call's `signed char` result, which no load writes, and over a narrowing cast the object read whole, `movq -8(%rbp), %rax; movslq %eax, %rax` for `(long) (int) l` (`backend.md` 6.3); a value in a register extended in it, as after `-a`'s `negl`, and a call's `signed char` result `movsbl %al, %eax` after the `call`, `movsbq %al, %rax` to a `long`; `char` to `int` after the extending load, nothing; `int` to `unsigned`, nothing; to `char`, the narrow store `movb %al, -5(%rbp)`, and the next read is the extending load, `movsbl -5(%rbp), %eax` or `movzbl` | `cast`, `widen`, `operand`, `signed-byte` |
| a `char` or `short` parameter or result | stored at its width in the prologue, `movb %dil, -1(%rbp)`, `movw %si, -4(%rbp)`, and loaded with its extension, `movsbl`, `movswl`; a result in `eax` as the operation left it, no extension before the jump to the epilogue; an argument stored at its width and loaded extended into its register, `movsbl -5(%rbp), %edi`; the result extended by the caller where it is used wider, `movsbl %al, %eax` after the `call` | `narrow` |
| `a += b` | as a statement, the op on the object at its width, `addl %eax, -12(%rbp)`, on a `char` `addb %al, -9(%rbp)`; for its value, and for `*=`, `/=` and `%=` always, `b` first, then the load, the op, the store, but for an operator that commutes on an object of the operation's width the object the op's source where `b`'s register is the one the row writes, `addq -8(%rbp), %rax` and the store for `a += f(b)`, `imull (%rax,%rcx,4), %edx` and the store for `p[i] *= e;`, and `x *= 12` on an `int` the three-operand `imull $12, -4(%rbp), %eax` and the store, a `char` keeping its extending load (`backend.md` 6.6); on a `char`, `b` into `ecx`, the extending load `movsbl -9(%rbp), %eax`, the op in `int`, the narrow store `movb %al, -9(%rbp)`; the place of `/=` and `%=` held outside `rax` and `rdx`, so `p[i] /= d` is `p` in `rcx`, `i` in `rsi`, `d` in `edi`, the element loaded into `eax`, `cltd; idivl %edi` and the store `movl %eax, (%rcx,%rsi,4)`, and no move; the place of `<<=` and `>>=` by a variable count outside `rcx`, `shll %cl, (%rax,%rdx,4)` after the count's load into `ecx` (`backend.md` 6.5); the value of an `unsigned` one at its own width, widened to `long`, the store and nothing after it, `addl %edx, %eax; movl %eax, (%rcx)` for `*p += e`, and `++*p` the same, where an operation in `long`, `*p += l`, keeps `movl %eax, %eax` | `compound`, `pinned` |
| a constant | the immediate, `addl $408, %eax`, a constant expression folded first, `12 * 34` to 408; a 32-bit constant whatever its bits, `addl $-2147483646, %eax` for `0x80000002`, and a 64-bit one a 32-bit immediate holds, `addq $-100000, %rax`; a 64-bit constant no 32-bit immediate holds is `movabsq $81985529216486895, %rcx` where it is written, in a loop at every iteration | `invariant`, `constant-hoisting`, `constant-in-loop`, `constant-bits` |
| a constant under an implicit conversion | the constant in the target type, an immediate of its width, stored: `long x = 2` is `movq $2, -8(%rbp)`, `long x = -1` is `movq $-1, -8(%rbp)`; `(long) 2` is the same `long` constant | |
| a loop-invariant expression | computed every iteration: `a / b` in the body is the load of `a` and `idivl -8(%rbp)`, and `12 * 34` its `addl $408` | `invariant` |
| an expression written twice | computed twice: `a * b + a * b` is two `imull`, and `12 * 34 + 12 * 34` two `addl $408` | `twice` |
| more held values than nine registers | the oldest held value spilled, `movq %rax, -32(%rbp)`, and reloaded when it is consumed, `movq -32(%rbp), %rax`, or, as the left operand of an operator that commutes, read in its slot at the row's width, `addl -32(%rbp), %eax` (`backend.md` 6.2) | `spill`, `spill-fifteen`, `return-register` |
| `p + n`, `m[i]` as a pointer | the index loaded and widened to 64 bits in one, `movslq -4(%rbp), %rcx`, then `leaq (%rax,%rcx,4), %rax` for a size of 2, 4 or 8, and `imulq $12, %rcx, %rcx; addq %rcx, %rax` for a size that is not a scale, on a local array the one `leaq -28(%rbp,%rax,1), %rax` after `imulq $12, %rax, %rax`, no `leaq` of the array first; on a row by a constant index the one `leaq 12(%rax,%rcx,4), %rax` for `p[1] + i` on `int (*p)[3]` after `p`'s load and `movslq -12(%rbp), %rcx`, no `leaq` of the row first, into a register pointer `leaq 12(%rax,%rcx,4), %rbx`; at a size of 1 a `long` in memory the instruction's operand, `addq -40(%rbp), %rsi` (`backend.md` 6.1), and an index in a register on a row's address the one `leaq 8(%rax,%rcx,1), %rax` for `p[1] + i` on `char (*p)[8]`; at a size that is not a scale a `long` in memory the `imulq`'s source, `imulq $12, -16(%rbp), %rcx; addq %rcx, %rax`, and `n + p` the same, `imulq $12, -16(%rbp), %rax; addq -8(%rbp), %rax`; a constant `n` of `n + p` as of `p + n`, `3 + p` `movq -8(%rbp), %rax; addq $12, %rax`, and into a register pointer `movq -16(%rbp), %rbx; addq $12, %rbx`, but held first where no 32-bit immediate holds its product, `movq $1073741824, %rax; movq -8(%rbp), %rcx; leaq (%rcx,%rax,4), %rax`; on a local array of a size 1, 2, 4 or 8 the one `leaq -20(%rbp,%rax,4), %rax` after `movslq -4(%rbp), %rax`, no address formed first | `gep`, `gep-stride`, `operand`, `subscript-row` |
| `p - n` | the index widened, `movslq -12(%rbp), %rcx; imulq $4, %rcx, %rcx; subq %rcx, %rax`; a `long` in memory the `imulq`'s source, `imulq $4, -16(%rbp), %rcx; subq %rcx, %rax`, and `p -= n` the same, `imulq $4, -16(%rbp), %rax; subq %rax, -8(%rbp)` (`backend.md` 6.1); for a size of 1, the `sub` alone, a `long` in memory its operand, `subq -16(%rbp), %rax` | `operand` |
| `++p`, `--p`, `p += 2`, `p -= 2` | the size times the constant, an immediate: as a statement, on the pointer's slot, `addq $4, -8(%rbp)`, `subq $8, -8(%rbp)`; for its value, between the load and the store, `subq $8, %rax` | `ptr-step` |
| `a[i]`, `p[i]` | the access's own address: on a local array the index loaded and widened in one, `movslq -4(%rbp), %rax`, then the access with the slot as the displacement, `movl -24(%rbp,%rax,4), %eax` or `movl %ecx, -24(%rbp,%rax,4)`, no `leaq` first; through a pointer the pointer loaded, the index loaded and widened, `movslq -12(%rbp), %rcx`, then `movl (%rax,%rcx,4), %eax`; a `signed char` index `movsbq -9(%rbp), %rcx`; a constant index into a pointer is the displacement, `movsbl 1(%rax), %eax`, and into an array reached by its name the one access at the slot plus the offset, `movl -20(%rbp), %eax` for `m[0][1]`, no `leaq` first; a store takes that operand too, `movl $8, -16(%rbp)` for `r[1]` in `typedef` | `subscript`, `string`, `subscript-row`, `typedef`, `widen` |
| `a[i]` by a `long` | the index loaded whole, `movq -8(%rbp), %rax`, and not widened; then `movl %ecx, -28(%rbp,%rax,4)` or `movl -28(%rbp,%rax,4), %eax` | `subscript-long` |
| `&p[0]`, `&p[3]`, `&p[i]` | the address the access would read, and no access: `&p[0]` is `p` and no instruction, the bare base as `*p`'s is; `&p[3]` and `&p[i]` one `lea` of the addressing form, `leaq 12(%rax), %rax`, and `leaq (%rax,%rcx,4), %rax` after `movslq -12(%rbp), %rcx` | `subscript-address` |
| `m[0]`, `m[0][1]`, `m[1]` on `int m[2][3]`, `p[1]`, `p[1][i]` on `int (*p)[3]`, `p[1][i][0]` on `int (*p)[3][3]` | a row decays to its address, the address of a subscript, which on a local array is its slot plus the row's offset whatever the row's size: `m[0]` is the array's `leaq -24(%rbp), %rax` and no instruction after it; `m[0][1]` the one access `movl -20(%rbp), %eax`; `m[1]` the one `leaq -12(%rbp), %rax`; through a pointer the pointer's value plus the offset, `p[1]` the one `leaq 12(%rax), %rax` after `p`'s load, and an offset no 32-bit displacement holds added from a register, `movabsq $2400000000, %rcx; addq %rcx, %rax` for `p[200000000]`; a variable index into the row joins the row's address, `p[1][i]` the one `movl 12(%rax,%rcx,4), %eax` after `p`'s load and `movslq -12(%rbp), %rcx`, no `leaq` first, and `i[p[1]]` the same after `i`'s load and `p`'s (`backend.md` 6.3, a base, a displacement and an index); by an index of a size no addressing form scales by the product is the operand's index at scale 1, `p[1][i][0]` the `imulq $12, %rcx, %rcx` and the one access `movl 36(%rax,%rcx,1), %eax`, no `leaq` and no `addq`, and on a local `char m[4][5]` `movsbl -20(%rbp,%rax,1), %eax` after `imulq $5, %rax, %rax`, no address formed; the index written first the same, `i[p[1]][0]` the `imulq` and `movl 36(%rcx,%rax,1), %eax`; a constant under the row joins the displacement past a cast, `movl 12(%rcx,%rax,1), %eax` for `i[(int (*)[3]) (c + 4)][2]`, subtracted, `movl -4(%rax,%rcx,1), %eax` for `(p - 1)[i][2]`, and in a store across a call, `movl %eax, 408(%rcx)` for `((int (*)[3]) (c + 400))[i][2] = f()` after the sum's `addq`, which the call spills once, and at a size of 2, 4 or 8 the pointer and the index summed into one register before the call, `leaq (%rax,%rcx,8), %rax` and `movl %eax, 404(%rcx)` for `((int (*)[2]) (c + 400))[i][1] = f()`, one spill and one reload (`backend.md` 6.3, an address held across a right operand) | `subscript-row` |
| `p[i][2]`, `m[i][2]`, `p[1][2]`, `&p[i][2]` | a constant over a row's address is that address's displacement, and no instruction forms the row's address: `p[i][2]` on a `char (*p)[8]` the one `movsbl 2(%rax,%rcx,8), %eax` after `p`'s load and `movslq -12(%rbp), %rcx`; on a local `char m[4][8]` the slot plus 2, `movsbl -34(%rbp,%rax,8), %eax`; `p[1][2]` on an `int (*p)[3]` the one `movl 20(%rax), %eax`, the row's 12 and the column's 8; `&p[i][2]` the one `leaq 2(%rax,%rcx,8), %rax`, and `&p[1][2]` the one `leaq 20(%rax), %rax`; under a cast of the pointer the sum's bytes are the access's operand, `movl 4097(%rax), %eax` for `*(int *) (p + 4097)` on a `char *p`, `movl (%rax,%rcx,1), %eax` for `*(int *) (p + i)`, and `movl bytes+4(%rip), %eax` on a global's bytes; a subscript or a constant over such a sum joins it, `movl 13(%rax), %eax` for `((int *) (p + 1))[3]` and `movswl 2(%rax,%rcx,1), %eax` for `*(short *) ((char *) p + i * 4 + 2)`; a name's address plus a constant under a cast, the shape of a structure's member, the slot's or the label's own operand, `movl -20(%rbp), %eax` for `*(int *) ((char *) &x + 4)` and `movl words+6(%rip), %eax` for `*(int *) ((char *) words + 6)` (`backend.md` 6.3) | `subscript-displacement` |
| `q = p[1]`, `q = &p[1]`, `q = &m[1][2]`, `q = &xa[3]` into a `register` pointer | the pointer's row writes the variable's register and the `leaq` is on it, `movq -16(%rbp), %rbx; leaq 12(%rbx), %rbx`, and no expression register is taken; on a register `p` the one `leaq 12(%rbx), %r12`, and `q = &q[1]` the one `leaq 4(%rbx), %rbx`; a call's result as the pointer, `q = &f(q)[1]`, `leaq 4(%rax), %rbx` after the call; an array the unit only declares, `q = &xa[3]` and `q = xa + 3`, the GOT's load into the register, `movq xa@GOTPCREL(%rip), %rbx; leaq 12(%rbx), %rbx`; `q = &m[1][2]` on `int (*m)[3]` `movq -16(%rbp), %rbx; leaq 20(%rbx), %rbx`, and `q = &m[1][-3]`, whose constants sum to 0, the one `movq -16(%rbp), %rbx` (`backend.md` chapter 4) | `address-destination` |
| `p - q` | `subq -16(%rbp), %rax`, `q`'s slot the instruction's operand and no load of it (`backend.md` 6.1); for a size other than 1, `movq $4, %rcx; cqto; idivq %rcx`, a division even for a size of 4, since G6 promises any two pointers and a shift would floor where the division truncates; for a size of 1 the `sub` alone, so `(char *) p - (char *) q` is the cheap spelling; with a `register` variable as `p`, the variable copied into `rax` and `q` held outside `rax` and `rdx`, as a divisor is, `movq %rbx, %rax; subq %rcx, %rax`, and nothing moved after the division (`backend.md` 6.5) | `ptr-diff`, `ptr-diff-char` |
| `p < q` and the rest | `cmpq -16(%rbp), %rax`, the unsigned `setb %al`, `movzbl %al, %eax` | `ptr-compare` |
| `if (p)`, `p == 0` | as a condition the pointer's slot compared, `cmpq $0, -8(%rbp)`, then `je`; as a value the slot compared the same, `cmpq $0, -8(%rbp)`, then `sete %al; movzbl %al, %eax` | `ptr-null` |
| `*p` | `movl (%rax), %eax`, `movl %ecx, (%rax)`; x86 has one `mov` for every alignment | `deref` |
| `a = b` on two records | an inline copy in units of the record's alignment, at most 8 bytes, lowest address first, each unit's load into a scratch register then its store, padding included, no call: `movl (%rcx), %edx; movl %edx, (%rax)` and the same at 4 for an 8-byte record of alignment 4 through two pointers, `movzwl` and `movw` for a unit of 2, `movzbl` and `movb` for 1; the unit's offset in each place's displacement, a slot's `-16(%rbp)`, a label's `corner+4(%rip)`, an element's `4(%rax,%rcx,8)`, no address formed; over eight units one loop, `movq $-72, %rsi`, then `movq 72(%rcx,%rsi,1), %rdx; movq %rdx, 72(%rax,%rsi,1); addq $8, %rsi; jne`, five instructions whatever the size, a label's address and an indexed element's formed first (G18, `backend.md` 6.6) | `struct-copy` |
| a local record initialized by a list | the slot zeroed whole at the declaration, in the copy's units, lowest address first, each the store of the immediate 0 at the unit's width, `movl $0, -8(%rbp); movl $0, -4(%rbp)` for an 8-byte record of alignment 4, padding included, no register; over eight units the copy's loop of one index, `movq $-200, %rax`, then `movq $0, (%rbp,%rax,1); addq $8, %rax; jne`, four instructions whatever the size; then each listed value stored at its member's offset in the slot, in the list's order, a constant by the one store of the immediate, `movl $1, -8(%rbp)`, a call's result by its store, `movl %eax, -8(%rbp)`, before the next value is evaluated (G20, `backend.md` 6.6) | `struct-local-list-initializer`, `struct-local-list-zeroed` |
| a variadic definition that calls `va_start`, and `va_arg` | the save area, a slot of 184 bytes at an alignment of 16 after the parameters', filled after their stores: `movq %rdi, -192(%rbp)` to `movq %r9, -152(%rbp)`, `movaps %xmm0, -144(%rbp)` to `movaps %xmm7, -32(%rbp)` with no test of `%al`, and `leaq 16(%rbp), %rax; movq %rax, -16(%rbp)`; `va_start` four stores into the list, `movl $8, -216(%rbp)`, `movl $48, -212(%rbp)`, the address at 176 loaded and stored, the area's `leaq` stored; `va_arg` `cmpl $40, -216(%rbp); ja`, then `addl $8, -216(%rbp)` and the area plus the offset less 8, or the stack's address stepped by `addq $8`, and the load of the type's width, `movq (%rax), %rax`; `va_copy` the record's copy, three `movq` units; `va_end` the list's `leaq`, discarded; a list handed on its address, `leaq -216(%rbp), %rax` (G19, `backend.md` chapter 5) | `variadic` |
| `*p++` on a `register` pointer | the row of `p++` on a register variable, the read copied and the variable stepped in place, then the access through the copy, `movq %rbx, %rax; addq $1, %rbx; movsbl (%rax), %eax`; `*--q` is the step and the access through the variable, `subq $1, %r12; movsbl (%r12), %ecx`; the copy loop, `while (*p++ = *q++)`, is nine instructions, `movq %rbx, %rax; addq $1, %rbx; movq %r12, %rcx; addq $1, %r12; movsbl (%rcx), %ecx; movb %cl, (%rax); cmpb $0, %cl; je .Lcopy.3; jmp .Lcopy.2` | `register-walk` |
| `&x` | `leaq -8(%rbp), %rax` | `address-of` |
| `(T *) p`, `(long) p`, `(T *) l` | nothing; for an `int` its load, `movslq -4(%rbp), %rax` | `ptr-int`, `void-pointer` |
| `"abc"` | `leaq .Ls0(%rip), %rax`; the bytes a `.string` under a label of their own in `.data`, writable, after the last function | `string` |
| `f`, `&f` as a value | of a function the file defines, PC-relative, `leaq defined(%rip), %rax`; of one it only declares, from the GOT, `movq declared@GOTPCREL(%rip), %rax`; one instruction either way | `function-address` |
| a global the file defines | the object PC-relative, its slot's rows with `counter(%rip)` for `-8(%rbp)`: the load `movl counter(%rip), %eax`, the store `movl %eax, counter(%rip)`, the memory forms `addl %eax, counter(%rip)` and `addl $1, counter(%rip)`, the address `leaq counter(%rip), %rax`; one instruction an access | `global` |
| a global the file only declares | its address loaded from the GOT at each access, `movq counter@GOTPCREL(%rip), %rax`, then the rows of `*p`: `movl (%rax), %eax`, `movl %ecx, (%rax)`, `addl $1, (%rax)`; `counter = counter + 1` loads the address twice, the store's first; two instructions an access | `global-extern` |
| `a[i]`, `a[3]` on a global array | by a variable, the address formed, `leaq table(%rip), %rax` for one the file defines and `movq other@GOTPCREL(%rip), %rax` for one it only declares, then the access through it, `movl (%rax,%rcx,4), %eax` after `movslq -4(%rbp), %rcx`, since a PC-relative operand takes no index; by a constant, on one the file defines, the one access at the label plus the offset, `movl table+12(%rip), %eax`, and on one it only declares the GOT's load and `movl 12(%rax), %eax` | `global-array` |
| the data of a global | after the last function, the objects with an initializer in `.data` and the others in `.bss`, each under `.globl`, `.type n, @object`, `.size n, 4` and `.p2align 2` above an alignment of 1, then `.byte 120`, `.short -3`, `.long 42`, `.quad 4886718345` or `.zero 12`; `int zero = 0;` is `.long 0` in `.data`; an array of 16 bytes or more is aligned to 16, `.p2align 4` for `long pair[2]`, which the psABI asks of a global array; nothing for an object only declared | `global-data` |
| an address constant as a global's initializer | one `.quad` in `.data`, the symbol and its offset in bytes, the constant times the pointee's size: `.quad n` for `&n`, `.quad a+12` for `&a[3]`, `.quad a+8` for `a + 2`, `.quad m+24` for `&m[1][2]`, `.quad on_signal`, `.quad stdout` and `.quad putchar` for objects and functions the file only declares, `.quad .Ls1+1` for `"abc" + 1`, its literal numbered after the functions'; `.quad 4096` for `(int *) 4096` and `.quad n` for `(long) &n`; the linker or the loader fills each in | `global-address` |
| a string literal as the initializer of an array of `char` | the array's own bytes under its own label, in the escapes of the literals' block: `.string "abc"` for `char s[] = "abc"`, which is 4 bytes, `.string "abc"` and `.zero 4` for `char s[8]`, `.ascii "abc"` for `char s[3]`, which the literal fills; no alignment, but `.p2align 4` for an array of 16 bytes or more; beside them, `char *p = "abc"` is two objects, the literal `.Ls0` and `.quad .Ls0` | `global-string` |
| a wide string literal | its own array in the literals' block, aligned to its element, its elements and the zero element under one directive of their width, `.p2align 2; .Ls0: .long 104, 233, 0` for `L"hé"`, `.short 120, 55357, 56832, 0` for `u"x😀"`, the surrogate pair; as an array's initializer the same elements, `.long 97, 98, 0` and `.zero 20` for `wchar_t padded[8] = L"ab"`; an element by a constant index one load, `movl .Ls2+4(%rip), %eax`; a wide character constant its value, folded | `wide-string` |
| a brace-enclosed list as a global's initializer | in `.data` whatever its values, each element the list gives in subscript order in the directive of its form at its width, and `.zero` for each run of elements it leaves out, so the bytes are the object's size: `.long 1`, `.long 2`, `.zero 12` for `int a[5] = { 1, 2 }`; `.long 1`, `.zero 8`, `.long 4`, `.long 5`, `.zero 4` for `int m[2][3] = { { 1 }, 4, 5 }`; `.quad .Ls0`, `.quad .Ls1` for `char *names[] = { "Jan", "Feb" }`, the literals numbered after the functions'; `.string "Jan"`, `.string "Feb"` for `char months[][4]`, each row's own bytes; `.long 5` for `int n = { 5 }` and `.zero 4` for `int z = {}`; `.p2align 4` for an array of 16 bytes or more | `global-braces` |
| `static` | a static function, a static object and a block's static object are their twins of external linkage but for the `.globl` line, which they have none of: `.type hidden, @function` and the code; `call hidden@PLT` and `leaq hidden(%rip), %rax`; `movl quiet(%rip), %eax`; a block's object under the function's name and its own, `movl next.count(%rip), %eax` and `movl %eax, next.count(%rip)`; the data in `.bss` under `.type`, `.size` and `.p2align 2` | `static` |

### 2.2 aarch64

The rows of the three slices, integers, control and calls, then
pointers, arrays and string literals, then register variables
(`aarch64.md` chapter 14), in the order of 2.1.

The two rows of 2.1 with no program have none here either. Two rows
are this target's alone: a slot beyond reach and a frame of 4096
bytes or more.

| Source | Machine | Program |
|---|---|---|
| a local | a slot of its own above `x29` for the whole call; a load at each read, `ldr w0, [x29, #20]`, and a store at each write, `str w0, [x29, #20]`; zero stored by a statement or an initializer is one store of the zero register, `str wzr, [x29, #20]`, and any other constant goes through a register, `mov w0, #1; str w0, [x29, #16]` | `local`, `while`, `stack-args` |
| a parameter | its argument register stored to its slot in the prologue, `str w0, [x29, #16]`; then the local's row; one past the eighth copied in the prologue from above the frame to a slot of its own through `x17`, at its width, after `mov x29, sp` and before the stores of the first eight, `ldrb w17, [x29, #64]; strb w17, [x29, #48]` for a `char` and `ldr x17, [x29, #72]; str x17, [x29, #56]` for a `long` | `parameter`, `stack-parameter` |
| a `register` local | the next of `x19` to `x28`, by declaration order, stored to the save area above the frame record in the prologue, `stp x19, x20, [x29, #16]` and `str x21, [x29, #32]`, and loaded back before the frame record in the epilogue, `ldp x19, x20, [x29, #16]` and `ldr x21, [x29, #32]`; no slot, no load, and no store but the save of a read of it held across a branch that writes it, `str x19, [x29, #32]` (`backend.md` 6.7); a read is the register as an operand, `cmp w21, w19`; an instruction that writes its destination writes a fresh register, with no copy, `sub w0, w19, w20`, but where it computes a value assigned to the variable, below, `add w20, w20, w21` for `s = s + i`; a constant assigned is the immediate, `mov w20, #0`; across a loop, the same register | `register-loop`, `register-parameter` |
| a `register` local without an initializer | the register as its declaration finds it, from the prologue or from an earlier block's variable; nothing writes it until the program does but the extension of a `char`, `short` or `unsigned` one in its own register at its declaration, `uxtb w19, w19` for a `char`, unsigned here, or `mov w19, w19`, which keeps the value of its type, so every read sees one value (G5), `cmp w19, #10` at both tests | `register-undef`, `register-destination` |
| a `register` parameter | moved from its argument register to its register in the prologue, `mov w19, w0`; then the register | `register-parameter` |
| `r = e` on a `register` variable | the last instruction of the value's row writes the variable's register, with no condition on the operands: `ldr w19, [x29, #24]`, `add w19, w0, w1`; by a constant or another register variable the left operand's row into the register and the operation on it, `ldr w19, [x29, #24]; add w19, w19, #5` and `ldr w20, [x29, #32]; add w20, w20, w19`; `sub w19, w0, w19` for `r = a - r`, `add w19, w0, w1` after `add w1, w1, w2` for `r = a + (b + c)`, `add w19, w19, w0` for `r = r + b`, `sdiv w19, w0, w1`, and `mul w19, w0, w1` for an initializer; an address in two instructions writes it twice, `adrp x19, defined; add x19, x19, :lo12:defined`, the GOT's `adrp x19, :got:declared; ldr x19, [x19, :got_lo12:declared]`, and a slot beyond reach `add x19, x29, #4096; add x19, x19, #1928`; through a conversion that changes the signedness alone, `ldr w19, [x29, #24]` for `register unsigned r = i`, `ldrb w19, [x29, #24]` for `register unsigned char r = sc`, `add w19, w0, w1` for `r = a + b` on two `int`s into an `unsigned` and `cset w19, lt` for `r = a < b`; a narrowing or a widening keeps the `mov`, `mov w19, w0`; `r = x++` the load into the register, `ldr w19, [x29, #24]`, then `add w0, w19, #1; str w0, [x29, #24]`, `r = (*p)++` `ldr w19, [x0]` the same, `r = ++x` the load, the step and the store on the register, `ldr w19, [x29, #24]; add w19, w19, #1; str w19, [x29, #24]`, `r = ++c` on a `char` the step in `w0` and `uxtb w19, w0` after the store, and `r = s++` on a register variable `mov w20, w19` before `add w19, w19, #1`; `r = a && b` and `r = a || b` `mov w19, #1` and `mov w19, #0` on the two paths, `r = c ? x : y` each arm into the register, `ldr w19, [x29, #28]` and `ldr w19, [x29, #32]`, and `r = c ? r + 1 : 0` `add w19, w19, #1` and `mov w19, #0`, with no `mov` after the join; `r = c ? f(x) : 0` `mov w19, w0` after the call on its own path and `mov w19, #0`, and `l = c ? x : y` into a register `long` `ldrsw x19, [x29, #28]` and `ldrsw x19, [x29, #32]`, `q = &(c ? m : n)[1][2]` into a register `int *` from two pointers to rows `ldr x19` on each path and `add x19, x19, #20`, a pointer's conversion writing nothing, an arm that is the variable's own read nothing on its path, whatever the other arm is, the condition's one branch going to the join past the other path with no `b`, `cbnz w0, .Lchosen_own_first.3` over `ldr w19, [x29, #28]` for `r = c ? r : x`, `cbz` over the load for `r = c ? x : r`, and `cbz` over the `bl` and `mov w19, w0` for `r = c ? f(x) : r`, and `r = r` nothing at all; the `mov` kept for `r = a[r]++`, whose address reads the variable, `mov x19, x1`, and after the join of `r = c ? f(x) : f(x + 1)`, `mov w19, w0` (`aarch64.md` chapter 4) | `register-destination`, `register-blocks` |
| `r += e`, `++r` on a `register` variable | the operation on the register in place, `add w19, w19, #5` and `add w19, w19, #1` (`aarch64.md` chapter 4) | `register-rules` |
| a narrow `register` variable | a value moved in by the extending move, `uxtb w19, w0` for a `char` after a call; a step on it re-extended after, `add w19, w19, #1; uxtb w19, w19`; unwritten, extended in its own register at its declaration, `uxtb w19, w19` (`aarch64.md` chapter 4) | `register-rules` |
| a held read of a `register` variable that is then written | copied before the write, `mov w1, w19` before `add w19, w19, #1` in `i + i++` (G13); before the paths of `&&`, `\|\|` and `?:` of which one writes it, the register stored, `str x19, [x29, #32]`, and the read reloaded where the paths meet, `ldr x1, [x29, #32]` (`backend.md` 6.7) | `register-rules` |
| a `register` parameter past the eighth | loaded into its register from the caller's area, `ldr w20, [x29, #64]`, after the stores of the first eight; the moves and the loads of `register` parameters and the stores of the others in parameter order, `str w0, [x29, #32]; mov w19, w1; str w2, [x29, #36]; mov w20, w3` (`aarch64.md` chapter 4) | `register-rules` |
| the epilogue of a function with `register` variables | the saved registers loaded from the save area off `x29`, `ldr x19, [x29, #16]`, then the frame record and `sp`, `ldp x29, x30, [sp]; add sp, sp, #48`, whatever the frame (`aarch64.md` chapter 5) | `register-rules` |
| a sixth `register` variable live | a register, `x24`, of the ten; a later block's variable takes `x19` again, `mul w19, w19, w0` (`aarch64.md` chapter 4) | `register-rules` |
| the frame | `sub sp, sp, #32; stp x29, x30, [sp]; mov x29, sp` in every function, a leaf included, and `ldp x29, x30, [sp]; add sp, sp, #32; ret` in the one epilogue; the frame record at the bottom, the slots above it, the size rounded up to 16, `sub sp, sp, #16` for the record alone | `local`, `call`, `zero-test` |
| a slot beyond reach | one more instruction at each access: the displacement's upper part added into `x16`, `add x16, x29, #36864`, then `str w0, [x16, #20]` or `ldr w0, [x16, #20]`; the same local declared before the array is `ldr w0, [x29, #20]` | `far-slot` |
| a frame of 4096 bytes or more | opened by two `sub`s, the upper part first, `sub sp, sp, #8192` and `sub sp, sp, #16`, each followed by its `.cfi_def_cfa_offset`, and closed by two `add`s in the same order; by one, `sub sp, sp, #8192`, when its low 12 bits are zero | `large-frame` |
| `a + b`, `a - b` | `add w0, w0, w1`, `sub w0, w0, w1`; the left operand's register is the result's | `add-sub`, `parameter` |
| an object in memory as the right operand | loaded into a register as any operand, `ldr w1, [x29, #36]`, and the operation on registers, `add w0, w0, w1`, since the machine has no instruction with a memory operand; a global through `x16` or the GOT's load, `*p` and `a[i]` loaded the same | `operand` |
| an identity, `a + 0`, `a * 1`, `a & -1` and the rest of chapter 0 | no instruction: the other operand's row alone, `ldr w0, [x29, #16]` for `a + 0`, `ldrsb w0, [x29, #16]` for `c + 0` on a `signed char` and `ldrsw x0, [x29, #16]` for `i + 0L`, the extension kept; `a * 0` is no identity, `mov w1, #0; mul w0, w0, w1` | `add-zero` |
| `a * b` | `mul w0, w0, w1`; by a constant, the constant held first, `mov w1, #3`, since `mul` takes no immediate, and by a power of two the same, `mov w1, #4` and no `lsl` | `mul`, `mul-three`, `power-of-two` |
| `a / b`, `a % b` | `sdiv w0, w0, w1` signed, `udiv w0, w0, w1` unsigned; the remainder is the quotient multiplied back and subtracted, `sdiv w2, w0, w1; msub w0, w2, w1, w0`, and `udiv w2, w0, w1` first unsigned | `div-rem`, `udiv-urem` |
| `a % b` on a `register` variable | the quotient in a register taken for it, and the result in the register the right operand leaves, `sdiv w1, w19, w0; msub w0, w1, w0, w19`, `udiv` unsigned; no copy and no move, and the variable is not written | `register-remainder` |
| `x / 4`, `x / 7` | the divisor held in a register, `mov w1, #4`, since `sdiv` takes no immediate, then `sdiv w0, w0, w1`, and on an `unsigned` `udiv w0, w0, w1`, `x % 4` `udiv w2, w0, w1; msub w0, w2, w1, w0`; no shift, no mask, no multiply by a magic constant | `div-four`, `div-seven`, `power-of-two` |
| `a + b / c`, a division with values held | the instruction on the registers the operands are in, `sdiv w1, w1, w2`, then `add w0, w0, w1`, and the remainder `sdiv w3, w1, w2; msub w1, w3, w2, w1`; nothing is moved, since no register is pinned, with nine values held or under an assignment, a comparison or a subscript as under `+` | `div-busy`, `pinned` |
| `a << n`, `a >> n` | the count in any register, `lsl w0, w0, w1`, `asr w0, w0, w1`, `lsr w0, w0, w1`; the instruction masks the count; under an assignment, `lsl w2, w2, w3` on the registers the loads took | `shift`, `pinned` |
| `-a`, `~a` | `neg w0, w0`, `mvn w0, w0`; widened after, `-(unsigned) a` to `unsigned long` is the `neg` alone, since the write to the `w` name cleared the upper half, and `-a` to `long` is `neg w0, w0; sxtw x0, w0`; the parser folds `-1` to a constant, so it is no instruction of this row | `negate` |
| `+a` | no instruction of its own: the promotion of `a` alone, as `(int) a` writes it; a `signed char` its extending load, `ldrsb w0, [x29, #16]`, a plain `char`, unsigned here, `ldrb w0, [x29, #16]`, an `unsigned short` `ldrh w0, [x29, #16]`, and an `int` or a wider type its load alone, `ldr w0, [x29, #16]`, `ldr x0, [x29, #16]`; a `register` variable its register, `mov w0, w19` for a return; as an operand the same, `ldrsb w1, [x29, #20]; add w0, w0, w1` for `a + +b` | `unary-plus` |
| `a < b` and the rest, as a value | `cmp w0, w1; cset w0, lt`, `cset w0, eq` for `==`; no jump; the value valid to 64 bits, so widened to a `long` or an `unsigned long` nothing after the `cset`, and `!a`, `a && b` the same after `cset w0, eq` and `mov w0, #1` and `mov w0, #0`; into a register `long` the `cset` itself, `cset w19, lt`; after a 32-bit operation the extension again, `add w0, w0, #1; sxtw x0, w0` (`aarch64.md` 6.3) | `compare` |
| `x == x` on a `register` variable | `cmp w19, w19`: the compare is written | `register-compare` |
| `a < b` as a condition | `cmp w0, w1` and a branch on the inverse to the else label, `b.ge .Lchoose.2`; no value | `if` |
| a condition on an object in memory and a constant | the object loaded, then the row of any condition: `ldr w0, [x29, #16]; cmp w0, #5; b.ge .Lless_constant.2`, `cbz w0, .Lnonzero.2`, `ldrb w0, [x29, #16]; cbz w0` on a `char`, and `and w0, w0, #0x1; cbz w0, .Lbit.2` | `operand` |
| a comparison with a constant on the left | the mirrored comparison, the constant the immediate on the right, and no `mov` of it: `5 < x` is `ldr w0, [x29, #16]; cmp w0, #5; b.le .Lconstant_left.2`, as `x > 5` is, and `b.ls` on an `unsigned`; on a register variable `cmp w19, #5`; for its value `cmp w0, #5; cset w0, gt`; `0 == p` is `ldr x0, [x29, #16]; cmp x0, #0; b.ne .Lnull_left.2`; `0 == (c & 1)` the `and`, then `cmp w0, #0; b.ne .Lzero_equal.2` (`aarch64.md` 6.1) | `operand`, `condition-flags` |
| a constant on the left of `+`, `*`, `&`, `\|` or `^` | taken as the constant on the right is, the operands' order exchanged, and no `mov` of an immediate: `5 + x` is `ldr w0, [x29, #16]; add w0, w0, #5`, as `x + 5` is, `7 & x` the load and `and w1, w1, #0x7`, `orr` and `eor` the same; `4 * x` the load, then `mov w2, #4; mul w1, w1, w2`, and `0x100000000 + l` the load, then `mov x1, #4294967296; add x0, x0, x1`, the constant held after the right operand; on a register variable `add w0, w19, #5`; `5 + f()` `bl f; add w0, w0, #5`, with no spill; into a register variable `ldr w19, [x29, #24]; mov w0, #4; mul w19, w19, w0` for `r = 4 * a` and `add w19, w19, #5` alone for `r = 5 + r`; `if (1 & x)` the load, `and w0, w0, #0x1; cbz w0, .Lcommuted_bit.2`; `5 - x`, `40 / x` and `1 << x` keep the constant first (`aarch64.md` 6.1) | `operand` |
| a condition that x86-64 jumps on its operator's flags | the value, then the test of zero: `add w0, w0, w1; cbz w0, .Ladd.2`, `and w0, w0, #0x1` and `cbz` for `a & 1`; on an object the load, the operation and the store, then `cbz w1`; a `char`'s stale upper bits `tst w1, #0xff; b.eq`; `(a & m) == 0` the comparison, `cmp w0, #0; b.ne .Lequal_zero.2`; `cbz` and `cbnz` read no flag, so x86-64's rows have nothing to take here (`aarch64.md` 7.1) | `condition-flags` |
| `a && b`, `a \|\| b` | jump code: each operand tested for zero in one instruction and a branch, `cbz w0, .Lboth.2` for `&&`, `cbnz w0, .Leither.4` past the right operand for `\|\|`; as a value, `mov w0, #1` and `mov w0, #0` on the two paths | `logical` |
| `a ? b : c` | the condition by jump code, `cbz w0, .Lconditional.2`, each arm into `w0`, a `b .Lconditional.3` past the second arm; never `csel`; a returned conditional joins its arms in `x0`, each arm generated for it, `ldr w0, [x1, w2, sxtw #2]` for a postfix step's old value, an arm moved there where its value is elsewhere, and no move after the join; one under a cast that widens it each arm extended into the join where one widens free, `mov x0, #4` and `mov x0, #-5` returned, and `ldrsb x0, [x29, #20]` and `ldrsb x0, [x29, #21]` for two `signed char`s stored into a `long`, with no `sxtw` after the join (`aarch64.md` 6.1) | `conditional`, `select` |
| `a ? b : c` for its effect | two `void` operands, or two whose values are dropped: the condition by jump code, each arm for its effect, a `b` past the second arm and no join, `ldr w0, [x29, #16]; cbz w0, .Lpick.2; bl f; b .Lpick.3; .Lpick.2: bl g; .Lpick.3:`; a constant assigned in an arm its one store, `mov w0, #1` before `str w0, [x16, :lo12:x]`; a void conditional as an arm the same inside the arm, its end label followed by the outer arm's `b` | `conditional-effect` |
| `a ?: b` | GNU C's `a ? a : b` with `a` evaluated once: `a` into the register the arms join in, tested there by a `cbnz` past `b`, which is written into the same register, `bl f; cbnz w0, .Lcalled.2; bl g` for `return f() ?: g()`, with no slot and no second read; `ldr w0, [x29, #16]; cbnz w0` for an int in memory, `cbnz x0` for a pointer, `ldrb w0, [x29, #16]` before the `cbnz` for a `char`; into a register variable the variable's register on both paths, `ldr w19, [x29, #24]; cbnz w19` and `ldr w19, [x29, #28]`, but where `b` reads the variable; as a condition the join, then `cbz w0`; for its effect `a`'s jump code past `b`, `bl f; cbnz w0, .Ldiscarded.2; bl g` | `conditional-omitted-operand` |
| `a, b` | no instruction of its own: `a` as the statement `a;` would be written, `ldr w0, [x29, #20]; add w0, w0, #1; str w0, [x29, #20]` for `i++`, then `b` as the value, the condition or the effect the comma is | `comma` |
| `if`, `while`, `for` | the condition by jump code at the top, where it is written, blocks in the written order; no alignment directive before a loop head; a loop branches back from its bottom, `b .Lcount.2` in `while`, and `for`'s step follows the body under no label but where a `continue` names one; an empty arm is still tested, `cbz w0, .Lempty_if.2` to the label that follows it in `cgp`; a join that holds only the branch back stays one block; an `if` whose body is one jump is the condition's branch to the jump's target, as on x86-64, `and w0, w0, #0x1; cbnz w0, .Llow.3` for `if (n & 1) break;` | `if`, `while`, `for`, `block-order`, `cgp`, `early-taildup`, `if-jump` |
| `if (0)`, `!0` | `if (0)` is a test, `mov w0, #0; cbz w0, .Lnever.2`, and the body is kept; `!0` is the constant 1, folded in the parser, `mov w0, #1` | `zero-test` |
| `do s while (e);` | the body first, under the loop's head, with no alignment directive; the condition by jump code at the bottom under a label only where a `continue` names it, and the branch back while it holds, `b.lt .Lcount.2`; one test an iteration; a label past the test where a `break` jumps to it | `do` |
| `break`, `continue` | one `b` each, nothing held, `b .Ltally.7` for the `break` that ends a case; as an `if`'s body, the `if`'s one branch: `break` past the loop, `b.gt .Lsum.4`, `continue` to the condition in `while` and `do` and to the step in `for`, `b.eq .Lsum.3` | `break-continue`, `if-jump` |
| `goto`, a label | one `b` to the label, and the label where it is written, numbered as on x86-64; the listing shows no C name; a loop built of `goto` branches back, `b .Lcount.2`; as an `if`'s body, the `if`'s one branch, `b.ge .Lcount.3` | `goto`, `if-jump` |
| `switch`, direct | the value less the first case's, `sub w0, w0, #1`, compared unsigned with the range, `cmp w0, #4; b.hi .Lday.6` to the default; the table's address into `x16` and the branch through it, `adrp x16, .Lday.8; add x16, x16, :lo12:.Lday.8; ldr x16, [x16, w0, uxtw #3]; br x16`, the load extending the index; the table of x86-64's row; a first value of 0 is no `sub` and nothing else | `switch-direct` |
| `switch`, simple | per case, in the order of the values, `cmp w0, #-1; b.eq .Lkind.4`, a 0 as the test of zero, `cbz w0, .Lkind.3`; then `b .Lkind.6` to the default | `switch-simple` |
| `switch`, hash | the size held and the unsigned division, quotient and remainder, `mov w1, #5; udiv w2, w0, w1; msub w0, w2, w1, w0`; the branch through the table by the remainder, `ldr x16, [x16, w0, uxtw #3]; br x16`; in each bucket the quotient compared, `cmp w2, #40; b.eq .Lreason.2`, then `b .Lreason.14` to the default | `switch-hash` |
| a switch's body | as on x86-64: the labels where they are written, each case's load, add and store with no branch before the next label; a `break` is `b .Lscore.7` past the body | `switch-fall` |
| `return e` | `e` into `w0`: the return names `x0`, and the value's last instruction writes it where no other value holds it, a source in it included, `sub w0, w19, w0` for `r - p[i]`, `ldr x1, [x29, #24]; add w0, w1, w0` for `x + (c ? 1 : 2)`, `msub w0, w2, w1, w0; sxtw x0, w0` for a remainder returned as a `long`; the place of an assignment or of a step generated with `x0` left for last, `ldr x0, [x29, #32]; str x0, [x1, w2, sxtw #3]` for `p[i] = x`; by a `mov` when it is held in another register, a register variable's, `mov w0, w19`, and one whose place holds a call's result, `mov w0, w1` for `p[f(i)] = x` (`aarch64.md` 6.1); then the branch to the one epilogue, `b .Lsum.1` in `for`, the last statement included; a `return` with no value as an `if`'s body is the `if`'s one branch, `cbnz w0, .Learly.1` | every program, `return-register`, `if-jump` |
| a statement whose value is unused | written: `a + 0;` is the load of `a`, `ldr w0, [x29, #16]`, `a / b;` is `sdiv w0, w0, w1`, and the division does not trap | `unused` |
| `f(a, b)` | each argument evaluated into a register and stored to a slot of its own, `str w0, [x29, #24]`, a constant argument through a register, `mov w0, #1; str w0, [x29, #16]` in `stack-args`; the slots loaded into `w0`, `w1` and on, `ldr w0, [x29, #24]`; `bl f`; the result in `w0` | `call` |
| a seventh and an eighth argument | in registers as the first six are, `ldr w6, [x29, #40]` and `ldr w7, [x29, #44]`; nothing on the stack | `stack-args` |
| a ninth argument and on | stored to their slots as the first eight are, a `char` at its width, `strb w0, [x29, #48]`; after the stores the outgoing area opened below the frame, 8 bytes an argument rounded up to 16, `sub sp, sp, #16`; each copied from its slot through `x17` at its width, the ninth to `[sp]` and the tenth to `[sp, #8]`, `ldrb w17, [x29, #48]; strb w17, [sp]` for a `char`, `ldr w17, [x29, #52]; str w17, [sp, #8]`; then the first eight loaded, `bl f9`, and `add sp, sp, #16` | `stack-args-nine` |
| `p(a, b)` | the callee first: `p` stored to a slot of its own, `str x0, [x29, #32]`, a hidden local as each argument is; then the arguments as for `f(a, b)`; after the argument registers, the slot loaded into `x16` and the call through it, `ldr x16, [x29, #32]; blr x16`; the result in `w0` | `call-pointer` |
| `p(1, ..., 9)` | the callee's slot written first, `str x0, [x29, #24]`, then the arguments' and the outgoing area as for a named call, `sub sp, sp, #16` and the ninth copied to `[sp]`; `ldr x16, [x29, #24]; blr x16`; `add sp, sp, #16` | `call-pointer-stack` |
| `(*p)(x)`, `(**p)(x)` | the listing of `p(x)`: the `*` writes no instruction | `call-pointer-star` |
| `(T) a` | `int` to `long` on an object in memory, its load and nothing after it, `ldrsw x0, [x29, #24]`, and a `signed char` and a `short` the same, `ldrsb x0, [x29, #16]`, `ldrsh x0, [x29, #16]`; through a pointer and on a global the same load, `ldrsh x0, [x0]`, `adrp x16, defined; ldrsw x0, [x16, :lo12:defined]`, `ldrsw x0, [x0]` after the GOT's load; a plain `char`, unsigned here, and an `unsigned char` their load, `ldrb w0, [x29, #16]`, valid to 64 bits and nothing after it; under a cast that keeps every bit the same load, `ldrsw x0, [x29, #16]` for `(long) (int) u`, `ldr w0, [x29, #16]` alone for `(unsigned long) (unsigned) i`, `ldrb w0, [x29, #16]` for `(long) (char) c` and `ldrsb x0, [x29, #16]` for `(long) (signed char) uc`, over a promotion the one load extended to the outer width, `ldrsb x0, [x29, #16]` for `(long) (int) c` on a `signed char` and `ldrh w0, [x29, #16]` alone on an `unsigned short`, and `(long) (unsigned) c` on a `signed char` the one `ldrsb w0, [x29, #16]`, with `mov w0, w0` after a call's result, `sxtb w0, w0; mov w0, w0` for a `signed char` one, and over a narrowing cast the object read whole, `ldr x0, [x29, #16]; sxtw x0, w0` for `(long) (int) l` (`aarch64.md` 6.3); a value in a register extended in it, as after `-a`'s `neg`, and a call's `signed char` result `sxtb w0, w0` after the `bl`, `sxtb x0, w0` to a `long`; `char` to `int` after the extending load, nothing; `int` to `unsigned`, nothing; to `char`, the narrow store `strb w0, [x29, #20]`, and the next read is the extending load, `ldrb w0, [x29, #20]`, unsigned as plain `char` is | `cast`, `widen`, `operand`, `signed-byte` |
| a `char` or `short` parameter or result | stored at its width in the prologue, `strb w0, [x29, #16]`, `strh w1, [x29, #18]`, and loaded with its extension, `ldrb w0, [x29, #16]`, `ldrsh w1, [x29, #18]`; a result in `w0` as the operation left it, no extension before the branch to the epilogue; an argument stored at its width and loaded extended into its register, `ldrb w0, [x29, #20]`; the result extended by the caller where it is used wider, `uxtb w0, w0` after the `bl` | `narrow` |
| `a += b` | no memory form, as a statement or for its value: `b` first, then the load, the op, the store, `ldr w1, [x29, #24]; add w1, w1, w0; str w1, [x29, #24]`; on a `char`, the extending load `ldrb w1, [x29, #24]`, the op in `int`, the narrow store `strb w1, [x29, #24]`, and for its value the re-extension; for a return's value the op writes `w0`, `add w0, w1, w0`, the store takes it from there and the re-extension is `uxtb w0, w0`; the value of an `unsigned` one at its own width, widened to `long`, the store and nothing after it, `add w0, w2, w0; str w0, [x1]` for `*p += e`, where an operation in `long` keeps `mov w0, w0` | `compound` |
| `a = b` on two records | the copy of `aarch64.md` 6.6: each unit of the type's alignment the plain load into `x17` and its store, lowest address first, `ldrh w17, [x0]; strh w17, [x29, #28]` and the same at `#2` and `#30` for a 4-byte record of alignment 2 from an element, whose index is added once, `add x0, x0, w1, sxtw #2`; a slot's and a base's displacement carry the unit's offset; a label of one or two units its page through `x16` for each, a longer one its address formed once; more than eight units the loop of four instructions, `mov x16, #9` and `ldr x17, [x1], #8; str x17, [x0], #8; subs x16, x16, #1; b.ne`; across a call on the right the left's address spilled once | `struct-copy` |
| a local record initialized by a list | the slot zeroed whole at the declaration, in the copy's units, lowest address first, each the store of the zero register at the unit's width, `str wzr, [x29, #16]; str wzr, [x29, #20]` for an 8-byte record of alignment 4, `strh wzr` for a unit of 2, padding included, no other register; a slot whose units leave the reach its address formed once; over eight units the copy's loop with one address, `add x0, x29, #16; mov x16, #25`, then `str xzr, [x0], #8; subs x16, x16, #1; b.ne`, three instructions whatever the size; then each listed value stored at its member's offset in the slot, in the list's order, a constant through a register, `mov w0, #1; str w0, [x29, #16]`, a call's result by its store, `str w0, [x29, #16]`, before the next value is evaluated (G20, `aarch64.md` 6.6) | `struct-local-list-initializer`, `struct-local-list-zeroed` |
| `va_start`, `va_arg`, `va_copy`, `va_end`; a list argument | a definition that calls `va_start` stores its area first, `stp x0, x1, [x29, #16]` to `stp x6, x7, [x29, #64]`, `stp q0, q1, [x29, #80]` to `stp q6, q7, [x29, #176]` and the stacked arguments' address, `add x16, x29, #272; str x16, [x29, #208]` (`aarch64.md` 5.2); `va_start` the five members' stores; `va_arg` the test of `__gr_offs`, `cmp w0, #0; b.ge`, the step of the offset or of `__stack` and the load at the slot; `va_copy` the four units of the 32-byte copy; `va_end` the list's address, `add x0, x29, #224`; a list argument copied into a slot of the call's own, four `ldr x17` and `str x17`, and its address passed, `add x1, x29, #272` | `variadic` |
| a constant | the immediate where the instruction encodes it, `add w0, w0, #4095`, `add w0, w0, #4096`, the bitmask `and w0, w0, #0xff`; otherwise held in a register first, by one `mov` where it encodes, `mov w1, #5000`, `mov w1, #-5000`, `mov w1, #0x80000001`, else by a `mov` and a `movk` for each further 16 bits, `mov w1, #2; movk w1, #0x8000, lsl #16`, and from ones for `-100000L`, `mov x1, #-34464; movk x1, #0xfffe, lsl #16`; a 64-bit constant of four parts is four instructions where it is written, `mov x1, #52719` and three `movk`, in a loop at every iteration | `constant-hoisting`, `constant-in-loop`, `constant-bits` |
| a loop-invariant expression | computed every iteration: `a / b` in the body is its two loads and its `sdiv w1, w1, w2`, and `12 * 34` its `add w0, w0, #408` | `invariant` |
| an expression written twice | computed twice: `a * b + a * b` is two `mul`, `mul w0, w0, w1` and `mul w1, w1, w2`, and `12 * 34 + 12 * 34` two `add w0, w0, #408` | `twice` |
| more held values than fifteen registers | the values held in `w0` to `w7` and `w9` to `w15`, `ldr w15, [x29, #40]`, `x8` left out; then the oldest held value spilled, `str x0, [x29, #48]`, and reloaded when it is consumed, `ldr x1, [x29, #48]`; `spill`'s eleven values are all held, the last `ldr w11, [x29, #32]` | `spill`, `spill-fifteen` |
| `p + n`, `m[i]` as a pointer | the index not widened, read by the last operand of `add`, which extends and scales it, `add x0, x0, w1, sxtw #2` for a size of 4; for a size that is not a scale, the index loaded and widened in one first, `ldrsw x1, [x29, #16]`, the size held, `mov x2, #12`, since `mul` takes no immediate, then `mul x1, x1, x2` and `add x0, x0, x1` | `gep`, `gep-stride` |
| `++p`, `--p`, `p += 2`, `p -= 2` | the size times the constant, an immediate, between the load of the pointer and the store back, as a statement and for its value: `ldr x0, [x29, #16]`, `add x0, x0, #4` or `sub x0, x0, #8`, `str x0, [x29, #16]` | `ptr-step` |
| `a[i]`, `p[i]` | the access's own address: the array's `add x0, x29, #24`, the index loaded and not widened, `ldr w1, [x29, #16]`, then `str w2, [x0, w1, sxtw #2]` or `ldr w0, [x0, w1, sxtw #2]`, the operand extending and scaling it; a constant index into a pointer is the displacement, `ldrb w0, [x0, #1]`, and into an array reached by its name the slot's own access at the slot plus the offset, `ldr w0, [x29, #20]` for `m[0][1]`, no `add` first, through `x16` beyond reach; a store takes that operand too, `str w0, [x29, #24]` for `r[1]` in `typedef` | `subscript`, `string`, `subscript-row`, `typedef` |
| `a[i]` by a `long` | the index loaded whole, `ldr x1, [x29, #16]`, and read by the shifted operand, `str w2, [x0, x1, lsl #2]` or `ldr w0, [x0, x1, lsl #2]`: as many instructions as by an `int` | `subscript-long` |
| `&p[0]`, `&p[3]`, `&p[i]` | the address the access would read, and no access: `&p[0]` is `p` and no instruction, the bare base as `*p`'s is; `&p[3]` and `&p[i]` one `add`, the machine having no `lea`, `add x0, x0, #12`, and `add x0, x0, w1, sxtw #2` | `subscript-address` |
| `m[0]`, `m[0][1]`, `m[1]` on `int m[2][3]`, `p[1]`, `p[1][i]` on `int (*p)[3]`, `p[1][i][0]` on `int (*p)[3][3]` | a row decays to its address, the address of a subscript, which on a local array is its slot plus the row's offset: `m[0]` is the array's `add x0, x29, #16` and no instruction after it; `m[0][1]` the one access `ldr w0, [x29, #20]`; `m[1]` the one `add x0, x29, #28`; through a pointer the pointer's value plus the offset, `p[1]` the one `add x0, x0, #12` after `p`'s load, and an offset no immediate holds built in a register and added, `mov x1, #6144; movk x1, #0x8f0d, lsl #16; add x0, x0, x1` for `p[200000000]`; a variable index into the row after the row's address, since no form holds an index with a displacement, `p[1][i]` the `add x0, x0, #12` after `p`'s load, then `ldr w0, [x0, w1, sxtw #2]` after `i`'s; by an index of a size no addressing form scales by the product added to the pointer, `mov x2, #12; mul x1, x1, x2; add x0, x0, x1`, and the row's 36 the access's displacement, `ldr w0, [x0, #36]`, with no add of it; on a local array the product added to `x29`, `add x0, x29, x0; ldrb w0, [x0, #24]` for `m[i][4]` at `[x29, #20]`, and on a label the page's offset the access's, `adrp x0, rows+2` and `ldrb w0, [x0, :lo12:rows+2]`; with no constant at all the product the register offset, `ldr w0, [x0, x1]` for `p[i][0]`, but `p[i][0] = f()`, whose address is held across the call, the add of the product and `str w0, [x1]` | `subscript-row` |
| `p[i][2]`, `m[i][2]`, `p[1][2]`, `&p[i][2]` | a constant over a row's address is the access's displacement: `p[1][2]` on an `int (*p)[3]` the one `ldr w0, [x0, #20]` after `p`'s load, the row's 12 and the column's 8, no `add` first; by a variable row the row's address formed, since no form holds an index with a displacement, `add x0, x0, w1, sxtw #3`, then `ldrb w0, [x0, #2]`, and on a local `char m[4][8]` at `[x29, #20]` the index added to `x29`, `add x0, x29, w0, sxtw #3`, then `ldrb w0, [x0, #22]`; `&p[i][2]` the row's `add` and `add x0, x0, #2`; `&p[1][2]` the one `add x0, x0, #20`; through a cast of a `char *` sum, `*(int *) (p + 1)` the one `ldr w0, [x0, #1]` and `*(int *) (p + 4097)`, which no displacement reaches, `mov x1, #4097; ldr w0, [x0, x1]`, but its store, whose address is held across the right operand, the add of the offset and `str w1, [x0]`; a name's address plus a constant under a cast the slot's or the label's own operand where the access encodes it, `ldr w0, [x29, #28]` for `*(int *) ((char *) &x + 4)`, the unscaled `ldr w0, [x29, #25]` for `+ 1`, `adrp x16, words+6; ldrsh w0, [x16, :lo12:words+6]` on an `int words[8]`, but the address formed for an offset out of the unscaled reach and no multiple of the size, `add x0, x29, #281; ldr w0, [x0]`, and for a label not aligned to the access, `*(int *) ((char *) words + 6)` and `*(int *) (bytes + 4)` on a `char` array, the page's `adrp` and `add` before `ldr w0, [x0]`; a constant or a subscript over a cast joined with the constants under it into the one displacement, `ldr w0, [x0, #13]` for `((int *) (p + 1))[3]` on a `char *p` and `ldr w0, [x0, #11]` through two casts, and past a variable index on the way, `ldrsh w0, [x0, #6]` after the product's add for `*(short *) ((char *) p + 4 + i * 4 + 2)` | `subscript-displacement` |
| `q = p[1]`, `q = &p[1]`, `q = &m[1][2]`, `q = &xa[3]` into a `register` pointer | the pointer's row writes the variable's register and the `add` is on it, `ldr x19, [x29, #24]; add x19, x19, #12`, and no expression register is taken; on a register `p` the one `add x20, x19, #12`, and `q = &q[1]` the one `add x19, x19, #4`; a call's result as the pointer, `q = &f(q)[1]`, `add x19, x0, #4` after the call; an array the unit only declares, `q = &xa[3]` and `q = xa + 3`, the GOT's load into the register, `adrp x19, :got:xa; ldr x19, [x19, :got_lo12:xa]; add x19, x19, #12`; `q = &m[1][2]` on `int (*m)[3]` `ldr x19, [x29, #24]; add x19, x19, #20`, and `q = &m[1][-3]`, whose constants sum to 0, the one `ldr x19, [x29, #24]` (`aarch64.md` chapter 4) | `address-destination` |
| `p - q` | `sub x0, x0, x1`; for a size other than 1, `mov x1, #4; sdiv x0, x0, x1`, a division even for a size of 4, as on x86-64; for a size of 1 the `sub` alone | `ptr-diff`, `ptr-diff-char` |
| `p < q` and the rest | `cmp x0, x1`, the unsigned `cset w0, lo` | `ptr-compare` |
| `if (p)`, `p == 0` | the test of zero as a condition, `cbz x0, .Ltest.2`; `cmp x0, #0; cset w0, eq` as a value | `ptr-null` |
| `*p` | `ldr w0, [x0]`, `str w1, [x0]`; one load or one store at any alignment (G11) | `deref` |
| `*p++` on a `register` pointer | one access that steps the pointer itself, by the post-indexed form for `p++`, `ldrb w0, [x19], #1`, and the pre-indexed one for `--q`, `ldrb w1, [x20, #-1]!`; no step is an instruction of its own; the copy loop, `while (*p++ = *q++)`, is four instructions, `ldrb w0, [x20], #1; strb w0, [x19], #1; cbz w0, .Lcopy.3; b .Lcopy.2`; a pointer in a slot keeps the rows of `p++` and `*p`, its step a load, an `add` and a store | `register-walk` |
| `&x` | `add x0, x29, #20` | `address-of` |
| `(T *) p`, `(long) p`, `(T *) l` | nothing, the load alone, `ldr x0, [x29, #16]`; for an `int` its load, `ldrsw x0, [x29, #16]` | `ptr-int`, `void-pointer` |
| `"abc"` | `adrp x0, .Ls0; add x0, x0, :lo12:.Ls0`; the bytes a `.string` under a label of their own in `.data`, writable, after the last function | `string` |
| `f`, `&f` as a value | of a function the file defines, PC-relative, `adrp x0, defined; add x0, x0, :lo12:defined`; of one it only declares, from the GOT, `adrp x0, :got:declared; ldr x0, [x0, :got_lo12:declared]`; two instructions either way | `function-address` |
| a global the file defines | each access two instructions, the symbol's page into `x16`, `adrp x16, counter`, and the load or the store through it, `ldr w0, [x16, :lo12:counter]`, `str w0, [x16, :lo12:counter]`; no memory form, so `counter += v` and `counter++` are the load, the operation and the store, five instructions with their two `adrp`; the address `adrp x0, counter; add x0, x0, :lo12:counter` | `global` |
| a global the file only declares | its address loaded from the GOT at each access, `adrp x0, :got:counter; ldr x0, [x0, :got_lo12:counter]`, then the rows of `*p`: `ldr w0, [x0]`, `str w1, [x0]`; `counter = counter + 1` loads the address twice, the store's first; three instructions an access | `global-extern` |
| `a[i]`, `a[3]` on a global array | by a variable, on one the file defines, the label's row, `adrp x0, table; add x0, x0, w1, sxtw #2; ldr w0, [x0, :lo12:table]`, and on one it only declares the GOT's load, then the access of a local array's subscript, `ldr w0, [x0, w1, sxtw #2]`; by a constant, on one the file defines, the row of a global scalar with the offset in the `adrp` and the `:lo12:` alike, `adrp x16, table+12; ldr w0, [x16, :lo12:table+12]`, and on one it only declares the GOT's load and `ldr w0, [x0, #12]` | `global-array` |
| the data of a global | the text of x86-64's row, but an array takes its element's alignment whatever its size, `.p2align 3` for `long pair[2]`, since the AAPCS64 asks no more | `global-data` |
| an address constant as a global's initializer | the text of x86-64's row; the assembler's relocation is `R_AARCH64_ABS64`, which the linker of a position-independent executable makes relative for a symbol the program defines | `global-address` |
| a string literal as the initializer of an array of `char` | the text of x86-64's row, but no alignment for an array of any size | `global-string` |
| a wide string literal | the data of x86-64's row, an array of `wchar_t` aligned to its element, `.p2align 2`, whatever its size; an element by a constant index, `adrp x16, .Ls2+4; ldr w0, [x16, :lo12:.Ls2+4]`, whose scaled offset the literal's alignment makes valid | `wide-string` |
| a brace-enclosed list as a global's initializer | the text of x86-64's row, but an array takes its element's alignment whatever its size | `global-braces` |
| `static` | the twins of external linkage but for the `.globl` line, as on x86-64: `bl hidden`, `adrp x0, hidden; add x0, x0, :lo12:hidden`, `adrp x16, quiet; ldr w0, [x16, :lo12:quiet]`, and the block's object `adrp x16, next.count` with `[x16, :lo12:next.count]` | `static` |

## 3. Verification

Five scripts, in the order `make test` runs them. The first three run
for every target with a pin directory, and each takes the target as
`--target <target>`; the default is the machine's own. A pin is one
target's listing or diagnostics, so it sits under a directory named
for its target. The last two run once.

- `tests/oracle` diffs the assembly `./rcc -target <target> -S`
  writes for each divergence program of `tests/corpus/` against the
  pin, `tests/corpus/<target>/<name>.s`, and proves with
  `./rcc -target <target> -c` that it assembles. On the machine's
  own target it also builds each program under `clang -O2` and under
  `./rcc`, runs both, and compares the exit statuses with
  `tests/corpus/expected`; on another target the programs are not
  run, nor are those of a long row, as the limit's paragraph below
  tells, unless the oracle is given `--long`.
- `tests/costs` pins this page: the listing of each program of
  `tests/cost/`, as the next paragraph tells.
- `tests/errors` pins the diagnostics: for each program of
  `tests/error/`, one rcc refuses, what rcc prints on standard error
  and the exit status it leaves with,
  `tests/error/<target>/<name>.err`.
- `tests/driver` checks the driver's command line, where the output
  goes: `-S -o -` writes the assembly to the standard output, and
  `-o -` with `-c` or a link is a usage error.
- `tests/probes` checks values, on the machine's own target only:
  rcc's code against `clang -O0`'s, function by function, as told
  below.

A program that uses a construct whose slice is not in the target's
generator, and has no pin for the target, shows "not yet" in its row
of the oracle or the cost corpus, and a probe whose functions rcc
stops at so shows "not yet" in its line; neither is passed nor
failed, and the scripts' headers give the rule. Each script of
`tests/` sets the core limit to 0 before it runs anything, so a
program or a compiler that a signal kills leaves its status in the
log and no core file.

Each program under `tests/cost/` is one row of chapter 2, one rule
of chapter 1 or one row of `choices.md`, and its opening comment
says what its listing must show on each target, in a paragraph per
target. Its pin,
`tests/cost/<target>/<name>.s`, is the text
`./rcc -target <target> -S` writes, whole. The script assembles each
program with `./rcc -target <target> -c`, which hands `clang` the
target, to prove that the text assembles; no program runs, so the
run is the same on any machine. A row passes when the fresh listing
equals the pin; the diff of a failed row is in
`build/cost/<target>/log`. A new or
changed listing is read by hand against its row of chapter 2 before
`tests/costs --pin` adopts it. A change to c1 that moves a cost
thus shows as a red row, and the row names the construct whose cost
moved.

`tests/probes` runs last, on the machine's own target only. Each probe
of `tests/probe/` compiles one source by rcc and by `clang -O0`, and a
`clang` main calls each function of both builds over a table of values
and compares the results; the calls probe pairs the two builds three
ways, each calling the other and rcc calling rcc. The memory and
pointers probes compare the memory each function writes as well, byte
by byte, the first over loads and stores through pointers and the
second over pointer arithmetic and the subscript. A probe does not
compare with `clang` in three places. One is `p - q` for a negative
difference that is no multiple of a size of 2, 4 or 8: `clang -O0`
shifts, and the probe's main computes the truncated quotient itself.
Another is the walk probe's store through a stepped `register`
pointer whose right operand names the pointer: `clang -O0` evaluates
that operand before the step, where G13 makes the step first, so the
main compares with a function of its own that keeps G13's order.
The third is the padding of a local record a list initializes, which
C23 leaves unspecified where the list is not empty: the zeroing probe
checks each byte of rcc's object outside its members against zero
(G20), and reads none of `clang`'s.
A probe checks values, not costs: that rcc's code computes what
`clang`'s does, which no pin shows. The bounds probe checks G10's
width too, which few values show: the low bytes of a sum or a product
do not depend on the bytes above its operands. It places each narrow
object at the end of a page mapped beside an unmapped one, and at the
start of one, and runs each row that reads or writes it, through a
pointer and as a member at a constant offset, so that an access wider
than its object, or one below it, faults; the fault names the row. A
global's and a local's address are the program's, not the probe's to
place, so their widths are the cost pins' to show and the
comparison's fit to check.

Each program the oracle runs, and each probe program, runs under a
time limit of 10 seconds, as `timeout -k 1 10`, since a program may
hang by promise and a fault of rcc's code may keep one in a loop:
SIGTERM stops it at the limit, and SIGKILL a second later if it
ignores SIGTERM. The run then shows as "timeout": an oracle row that
expects it passes, and a probe fails. `timeout` ends with 124 after
SIGTERM and 137 after SIGKILL, and either is the limit's only where
the run lasted the limit: one function, `run_under_limit()` in
`tests/status.sh`, runs and times the program for both scripts. A run
that ends with 124 before the limit gave the status itself, which no
program of the corpus may, and the oracle fails it; one that ends
with 137 sooner was killed from elsewhere, and shows as `KILL`, a
probe as "stopped, KILL".

A row of `tests/corpus/expected` that holds "timeout" in either
column is a long row: a program of it runs until the limit stops
it, so its run lasts the limit, as G14's endless loop does under
rcc. The oracle runs the programs of a long row only when it is
given `--long`. Without it the row is checked as on another target,
by its pin and its object alone; the table shows "-" for both
statuses, the summary counts the long rows whose programs were not
run, and `--pin` leaves the row's statuses as they are. `make test`
runs the oracle without `--long`, so it waits out no long row's
limit; `make test-long` is `make test` with the oracle's `--long`.

The shell reports a run that signal n killed as 128 + n, so the oracle
shows a status above 128 by the signal's name, as `kill -l` gives it:
`SEGV` for the null load of G9, and `KILL` for a SIGKILL from
elsewhere before the limit. No program of the corpus exits above 128
by itself, so no status reads two ways. `tests/corpus/expected` names
the signal too: a row passes on a signal only when it names it, and a
row that holds a number above 128 fails, as does a status above 128
whose signal `kill -l` has no name for. A probe a signal stops shows
the name the same way, "stopped, SEGV": the name comes from one
function, `shown_status()` in `tests/status.sh`, which both scripts
read from the tree's root.

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
negative. A tuned source is written for a machine, so a target may
have its own, `tests/perf/<target>/<name>-tuned.c`, which `make
bench` takes in place of the shared `tests/perf/<name>-tuned.c`. The
two sources of a program return the same exit status. Each program
fills its local data from a linear congruential
generator written in the program, runs its work in a repeat loop, and
folds its result to a byte as the exit status, so nothing is dead and
nothing can be computed in closed form; the listing of each `clang
-O2` build has its loops.

The metric is user plus system CPU time, the least of five runs, in
milliseconds and as the ratio to the baseline. The kernel splits a
run's CPU time between user and system by sampling, so user time
alone can read far under the run's cost, while the sum only grows
with noise. On the Firestorm core of an Apple M1 Max, user time alone
read one run of a binary at 293 ms where its other runs lay between
552 and 556. `make bench` runs `tests/bench`, which builds the four
binaries, refuses a row whose four builds disagree on the exit
status, and times each run under `timeout 120`. `make slots` runs
`tests/slots`, the cost of a stack slot (4.3). The numbers are bound
to the machine, so nothing is pinned. `make bench` was run three
times on each machine, and each table is the run whose cells sum
least.

The x86-64 table was measured on an x86-64 desktop, Zen 2, with clang
23.1.4, on 2026-11-17, in user plus system time, with the cpufreq
governor `schedutil` and the frequency boost on, the load average
under 0.2 when the first run began. c1 aligns every loop head to 32
bytes on x86-64 (4.2), so the table is measured at that layout: a hot
loop's time still moves with the code before it, but only where no
loop head intervenes.

| program | `clang -O2 <name>.c` | `rcc <name>.c` | `rcc <name>-tuned.c` | `clang -O2 <name>-tuned.c` |
|---|---|---|---|---|
| collatz | 296 (1.00) | 2293 (7.75) | 473 (1.60) | 295 (1.00) |
| fib | 261 (1.00) | 608 (2.33) | 584 (2.24) | 271 (1.04) |
| matmul | 287 (1.00) | 2355 (8.21) | 572 (1.99) | 272 (0.95) |
| sieve | 289 (1.00) | 2001 (6.92) | 507 (1.75) | 296 (1.02) |
| sort | 273 (1.00) | 2164 (7.93) | 513 (1.88) | 274 (1.00) |
| strings | 277 (1.00) | 2069 (7.47) | 562 (2.03) | 277 (1.00) |

The table is the method's, and one of its cells lies more than 2%
from the same cell of both other runs: `rcc fib-tuned.c`, 584 ms
against 602 and 600, lies under them. At the medians of the three
runs its ratio is 2.28.

The plain source under rcc runs 2.33 to 8.21 times as long as under
`clang -O2`. The tuned source under rcc runs 1.60 to 2.24 times as
long. The tuned source under `clang -O2` runs 0.95 to 1.04 times as
long as the plain one.

**collatz.** The tuned source tests the parity with `& 1` and halves
with `>> 1`, where the plain one writes `% 2` and `/ 2`. The value is
a signed `long`, so only the programmer knows it is never negative.
The step loop of `rcc collatz.c` reads x from its slot three times an
iteration, for the loop test, the parity test and the step, stores it
back, and adds 1 to the count in its slot, `addl $1, -52(%rbp)`. The
loop test compares the slot itself, `cmpq $1, -48(%rbp)`. The
parity test is `movq $2, %rcx; cqto; idivq %rcx` and a compare of the
remainder with 0, `cmpq $0, %rdx; jne .Lmain.10`, and the even path
divides again. The tuned `main` holds five `register` variables live
in the step loop: total, end, v, x and steps take `rbx`, `r12`,
`r13`, `r14` and `r15d`, and rep and start, declared without the
keyword, take slots. Nothing warns. The step loop of `rcc
collatz-tuned.c` has no stack operand: it compares x with 1, `cmpq
$1, %r14`, tests the parity in its register, `testq $1, %r14`,
computes the odd step in its register, `imulq $3, %r14, %r14` and
`addq $1, %r14`, halves x in its register, `sarq $1, %r14`, and counts
in its register, `addl $1, %r15d`. The ratio falls from 7.75 to 1.60.
The head of the step loop opens a 32-byte window, at 0x11c0. As first
written, with seven `register` variables live and x and steps in
slots, the tuned ratio was 1.91, measured before c1 aligned its loop
heads.

**fib.** The tuned source declares the parameter and the two partial
sums `register`. `rcc fib.c` subtracts its frame, `subq $32, %rsp`,
on every call, the base case included, stores the parameter to its
slot, `movl %edi, -4(%rbp)`, compares the slot itself for the test,
`cmpl $2, -4(%rbp)`, and loads it back for the return and each
argument. Each argument goes through a slot of its
own, `movl %eax, -8(%rbp); movl -8(%rbp), %edi`, and the first result
is stored across the second call, `movq %rax, -16(%rbp)`, and read
back by the sum, `addl -16(%rbp), %eax`. The tuned `fib` holds its
three live, n in `ebx`, a in `r12d`
and b in `r13d`. `rcc fib-tuned.c` moves the parameter in once,
`movl %edi, %ebx`, tests and returns it there, and keeps the first
result in `r12d` across the second call; but every call, the base
case included, pushes the three registers and pops them in the
epilogue, and each argument still goes through its slot, `movl %eax,
-28(%rbp); movl -28(%rbp), %edi`. The ratio falls from 2.33 to 2.24,
the least fall in the table, and at the medians of the three runs the
plain and tuned ratios are level, 2.27 and 2.28. The work is the
calls, and `clang -O2 fib.c` keeps one of the two recursive calls and
turns the other into a loop.

**matmul.** The tuned source computes the row bases of a and c once
per row, walks one pointer along a's row and another down b's column,
and declares the sum `register`, stored to c once per element. The
inner loop of `rcc matmul.c` has four `imull`, two for `i * n` and
one for `k * n` in the three indices and one for the product, eleven
reads of i, j, k and n from their slots, four of them loads and seven
the operands of a `cmpl`, an `imull` or an `addl`, the step `addl $1,
-110632(%rbp)` on k's slot, and the product added to c's element in
place, `addl %ecx, -110592(%rbp,%rax,4)`, the slot of c the
displacement of the indexed operand. The tuned `main` holds five
`register` variables live in the inner loop: n, aend, pa, pb and s
take `ebx`, `r12`, `r13`, `r14` and `r15d`, and bend, i, arow, pc and
bcol, declared without the keyword, take slots. Nothing warns. The
inner loop of `rcc matmul-tuned.c` has no stack operand and runs 9
instructions an iteration: the test `cmpq %r12, %r13` and its jump,
the load of one element and the product with the other in place, `movl
(%r13), %eax; imull (%r14), %eax`, its add to s, `addl %eax, %r15d`,
the step `addq $4, %r13`, the step `pb += n`, `movslq %ebx, %rax; leaq
(%r14,%rax,4), %r14`, and the `jmp` back. The `leaq` scales n and
writes pb's register, so the product is the loop's one multiply,
where the plain one has four. The middle loop pays the slots: it
loads bcol for its test and compares it with bend's slot, `movq
-110688(%rbp), %rax; cmpq -110656(%rbp), %rax`, copies arow and bcol
from their slots into pa and pb, stores s through pc loaded from its
slot, and steps pc and bcol in theirs, `addq $4, -110680(%rbp)` and
`addq $4, -110688(%rbp)`. The ratio falls from 8.21 to 1.99. The head
of the inner loop opens the second half of a 64-byte line, at 0x1320,
and the loop's 29 bytes lie whole in it. As first written, with ten
`register` variables live and pc, bcol, pa, pb and s in slots, the
tuned ratio was 7.54.

**sieve.** The tuned source walks a pointer in the marking loop and
in the strike loop, up to an end computed once per repetition, and
declares the walkers, the end, the limit, the outer counter and the
count `register`. The strike loop of `rcc sieve.c` loads j and
compares it with n's slot for the test, `cmpl -65552(%rbp), %eax`,
loads j for the store and i for the step, and adds i to j's slot,
`addl %eax, -65568(%rbp)`: three loads, a compare with memory and an
add to memory for each byte struck. The tuned `main` declares six
`register` variables but holds at most five live, since the marking
walker's loop ends before the outer counter's begins: n, count and end
take `ebx`, `r12d` and `r13`, the marking walker `r14`, and after it
the outer counter i takes `r14d` and the strike walker `r15`. Nothing
warns. The strike loop of `rcc sieve-tuned.c` compares the walker with
the end, `cmpq %r13, %r15; jae .Lmain.14`, stores the byte through it,
`movb $0, (%r15)`, and adds i, widened by `movslq %r14d, %rax`, with
no stack operand. The ratio falls from 6.92 to 1.75.

**sort.** The tuned source walks two pointers down for the shift,
steps with `--q`, loads each element it compares once, and keeps the
key, the index, the walkers and the element in registers. The shift
loop of `rcc sort.c` reads j four times from its slot, once by the
compare `cmpl $0, -8220(%rbp)` and three times by a load, compares
the element with the key in its slot, `cmpl -8216(%rbp), %eax`, steps
j in its slot, `subl $1, -8220(%rbp)`, and loads the element `a[j]`
twice, for the test and for the move. The tuned
`main` holds its five live, i, key, p, q and v, in `ebx`, `r12d`,
`r13`, `r14` and `r15d`. The shift loop of `rcc sort-tuned.c` has no
stack operand and runs 10 instructions an iteration: `leaq
-8232(%rbp), %rax` for the array's address, the test `p != a`, `subq
$4, %r14` for `--q`, the one load of the element, straight into v,
`movl (%r14), %r15d`, the compare with the key, the store `movl %r15d,
(%r13)`, the move of `p = q` and the `jmp` back. The `&&` is jump
code: each test is a compare and a jump to the loop's exit, `cmpq
%rax, %r13; je .Lmain.12` and `cmpl %r12d, %r15d; jle .Lmain.12`,
four of the 10, where the LLVM pipeline set and widened a byte for
each test and for the `&&` (`llvm.md` 5.5). The ratio falls from
7.93 to 1.88; with matmul's and collatz's it is one of the three
largest falls in the table, each by more than 6.

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
`main` is a `sarl $1, %ecx`. The ratio falls from 7.47 to 2.03.
`clang -O2` turns `length` into a call of `strlen` in both its
builds.

On aarch64 `make bench` was run three times on each of two machines,
each on mains with the cpufreq governor `performance`: an Apple M1
Max, with clang 23.1.3, on 2026-11-17, and a Qualcomm Oryon, with
clang 23.1.1, on 2026-11-13. The runs are not pinned, so a table names
the machine, not the core. c1 writes no alignment directive on this
target (4.2).

aarch64 keeps ten `register` variables to x86-64's five, and an
access that steps a `register` pointer, `*p++`, is one instruction
there (`aarch64.md` 6.3). Two programs thus have a tuned source
of their own in `tests/perf/aarch64/`, and the other four take the
shared one. matmul's is the shared source as first written, with
`register` on all ten variables of the product. strings' writes the
loops of `length` and `copy` with `*p++`; it was written for this
machine after the generator existed, which the design's phrase "as
first written" does not cover (`aarch64.md` chapter 13). What each
buys was measured earlier, in user time alone. The tuned matmul ran in
740 ms against 835 with the shared source on the M1 Max, and in 615
against 666 on the Oryon. The tuned strings ran in 535 ms against 544
on the M1 Max, and in 489 to 493 against 507 on the Oryon.

The M1 Max:

| program | `clang -O2 <name>.c` | `rcc <name>.c` | `rcc <name>-tuned.c` | `clang -O2 <name>-tuned.c` |
|---|---|---|---|---|
| collatz | 330 (1.00) | 1121 (3.40) | 470 (1.42) | 330 (1.00) |
| fib | 282 (1.00) | 454 (1.61) | 679 (2.41) | 282 (1.00) |
| matmul | 362 (1.00) | 2685 (7.42) | 740 (2.04) | 361 (1.00) |
| sieve | 471 (1.00) | 2114 (4.49) | 648 (1.38) | 384 (0.82) |
| sort | 373 (1.00) | 949 (2.54) | 455 (1.22) | 373 (1.00) |
| strings | 266 (1.00) | 1087 (4.09) | 541 (2.03) | 282 (1.06) |

The Oryon:

| program | `clang -O2 <name>.c` | `rcc <name>.c` | `rcc <name>-tuned.c` | `clang -O2 <name>-tuned.c` |
|---|---|---|---|---|
| collatz | 288 (1.00) | 1644 (5.71) | 453 (1.57) | 295 (1.02) |
| fib | 244 (1.00) | 469 (1.92) | 558 (2.29) | 243 (1.00) |
| matmul | 362 (1.00) | 2584 (7.14) | 615 (1.70) | 366 (1.01) |
| sieve | 438 (1.00) | 2065 (4.71) | 582 (1.33) | 346 (0.79) |
| sort | 337 (1.00) | 2634 (7.82) | 448 (1.33) | 332 (0.99) |
| strings | 250 (1.00) | 2418 (9.67) | 492 (1.97) | 263 (1.05) |

The Oryon's table is thus of the compiler of 2026-11-13, whose
listings of collatz and strings as written and of collatz, matmul,
sieve, sort and strings as tuned differ from those of the M1 Max's.
The Oryon's `rcc strings.c` cell is one timing of its five: the other
fourteen timings of that binary in the three runs read 2500 to 2510
ms, 225 more on the same machine read 2494 to 2512 with the default
and with a fixed address layout, and 2500 against 250 is a ratio of
10.00.

On the M1 Max the plain source under rcc runs 1.61 to 7.42 times as
long as under `clang -O2`, and the tuned source 1.22 to 2.41 times.
On the Oryon the two ranges are 1.92 to 9.67 and 1.33 to 2.29. The
tuned source under `clang -O2` runs 0.79 to 1.02 times as long as the
plain one, but for strings, whose `*p++` source runs 1.06 times as
long on the M1 Max and 1.05 on the Oryon. Across the two dates, the
plain ratios of sort, strings and collatz differ the most between the
two machines, 2.54 against 7.82, 4.09 against 9.67 and 3.40 against
5.71, but their tuned ratios differ by 0.15 at most; 4.3 measures the
cost of a slot on each.

**collatz.** As on x86-64, the step loop of `rcc collatz.c` reads x
from its slot three times an iteration and stores it back; each read
is a load. The parity
test is a division and a multiply back, `mov x1, #2; sdiv x2, x0, x1;
msub x0, x2, x1, x0`, since the machine has no remainder. The count
steps by a load, an add and a store, `ldr w0, [x29, #64]; add w0, w0,
#1; str w0, [x29, #64]`, since the machine has no memory form. The
tuned `main` holds total, end, v, x and steps in `x19` to `x22` and
`w23`, the five it holds on x86-64, and its step loop has no stack
operand. The parity test is `and x0, x22, #0x1; cbz x0, .Lmain.10`.
The odd step holds the 3 in a register, `mov x0, #3; mul x0, x22,
x0`, since `mul` takes no immediate, and the even step is `asr x22,
x22, #1`. The ratio falls from 3.40 to 1.42 on the M1 Max and from
5.71 to 1.57 on the Oryon.

**fib.** The tuned `fib` holds n, a and b in `w19`, `w20` and `w21`.
It moves n in once, `mov w19, w0`, tests and returns it there, and
keeps the first result in `w20` across the second call. But every
call, the base case included, stores the three registers in its
prologue, `stp x19, x20, [x29, #16]` and `str x21, [x29, #32]`, and
loads them back in its epilogue, `ldp x19, x20, [x29, #16]` and `ldr
x21, [x29, #32]`. The base case is thus 15 instructions, where in
`rcc fib.c` it is 12, with one store and two loads of n's slot; the
path that recurses is 25 instructions, and 24 in `rcc fib.c`, whose
sum is `add w0, w1, w0` after the reload of the first result. Each
argument still goes through a slot of its own, `str w0, [x29, #40];
ldr w0, [x29, #40]`. The tuned source runs longer than the plain one
on both machines: the ratio rises from 1.61 to 2.41 on the M1 Max and
from 1.92 to 2.29 on the Oryon. On x86-64 it falls from 2.33 to 2.24.

**matmul.** `matmul.c` declares its scalars after the three arrays,
so every scalar's slot is beyond the reach of a load, and each access
forms its address in `x16` first, `add x16, x29, #110592; ldr w0,
[x16, #52]` (2.2, a slot beyond reach). The inner loop of `rcc
matmul.c` is 47 instructions, thirteen of them `add x16` and four
`mul`; the product is added to c's element by a load, an add and a
store, `ldr w3, [x0, w1, sxtw #2]; add w3, w3, w2; str w3, [x0, w1,
sxtw #2]`. This target's tuned source holds all ten variables of the
product in `x19` to `x28`, and neither its inner loop nor its middle
loop has a stack operand. The inner loop runs 9 instructions an
iteration: the test `cmp x26, x23` and its branch, the two loads,
`ldr w0, [x26]` and `ldr w1, [x27]`, the product, its add to s, the
step `add x26, x26, #4`, the step `pb += n` in one instruction, `add
x27, x27, w19, sxtw #2`, where x86-64 takes three, and the branch
back. The middle loop stores s through pc, `str w28, [x24]`, and steps
pc and bcol in their registers. The shared source, with five
`register` variables, leaves bend, i, arow, pc and bcol in slots
beyond reach; its middle loop reaches four of them, each access
through its own `add x16, x29, #110592`. The ratio falls from 7.42 to
2.04 on the M1 Max and from 7.14 to 1.70 on the Oryon.

**sieve.** `sieve.c` declares its scalars after the array too, so
each access to one goes through `add x16, x29, #65536`. The strike
loop of `rcc sieve.c` is 18 instructions: five loads, j and n for the
test, j for the store, i and j for the step, the store of j back, and
the store of the byte, `strb wzr, [x0, w1, sxtw]`, each scalar access
with its `add x16`. The tuned `main` holds n, count and end in `w19`,
`w20` and `x21`, the marking walker in `x22`, and after it the outer
counter i in `w22` and the strike walker in `x23`, as on x86-64. Its
strike loop is five instructions: the test, `cmp x23, x21; b.hs
.Lmain.14`, the store `strb wzr, [x23]`, the step `add x23, x23, w22,
sxtw`, which widens i in its operand where x86-64 writes a `movslq`,
and the branch back. The ratio falls from 4.49 to 1.38 on the M1 Max
and from 4.71 to 1.33 on the Oryon.

**sort.** `sort.c` declares its scalars after an array of 8 KiB, so
their slots are in reach, `ldr w0, [x29, #8232]`. The shift loop of
`rcc sort.c` is 20 instructions: it loads j four times and the key
once from their slots, loads the element `a[j]` twice, and steps j by
a load, a `sub` and a store. The tuned `main` holds i, key, p, q and v
in `w19`, `w20`, `x21`, `x22` and `w23`. Its shift loop has no stack
operand and is 9 instructions an iteration, one fewer than on
x86-64: `--q` and the load of the element are one instruction, `ldr
w23, [x22, #-4]!`, the pre-indexed form, which steps the pointer
before the access. The load writes v's register itself, as the row
that computes a value assigned to a register variable does
(`aarch64.md` chapter 4), and the compare and the store read `w23`.
The ratio falls from 2.54 to 1.22 on the M1 Max and from 7.82 to 1.33
on the Oryon.

**strings.** `length` in `rcc strings.c` loads the pointer and the
index from their slots at each test and steps the index by a load, an
add and a store. `copy` loads s twice, d once and i four times from
their slots for each byte, and the source byte twice. This target's
tuned source writes the two loops with `*p++`, and the post-indexed
form steps each pointer in its access. `length`'s loop is three
instructions, `ldrb w0, [x20], #1; cbz w0, .Llength.3; b .Llength.2`,
and `copy`'s is four, `ldrb w0, [x20], #1; strb w0, [x19], #1; cbz
w0, .Lcopy.3; b .Lcopy.2`. `length` returns `sub x0, x20, x19; sub x0,
x0, #1`. The `sdiv` of `n / 2` in `main` is an `asr w2, w22, #1`. The
ratio falls from 4.09 to 2.03 on the M1 Max and from 9.67 to 1.97 on
the Oryon.

### 4.1 Under the LLVM pipeline

The same programs were measured through the pipeline `llvm.md`
records, on an x86-64 desktop, Zen 2, with LLVM 23.1.3, on
2026-11-07, in user time alone, the least of five runs (`llvm.md`
chapter 6):

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
under `clang -O2`, which c1 does not touch, stays within 0.95 to
1.04 in both; the two rcc columns move, from the pipeline's ratio to
c1's:

- collatz: plain 8.14 to 7.75, tuned 1.69 to 1.60.
- fib: plain 1.49 to 2.33, tuned 1.33 to 2.24.
- matmul: plain 7.64 to 8.21, tuned 3.31 to 1.99.
- sieve: plain 6.40 to 6.92, tuned 2.67 to 1.75.
- sort: plain 5.94 to 7.93, tuned 4.40 to 1.88.
- strings: plain 8.43 to 7.47, tuned 2.70 to 2.03.

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

On x86-64 a hot loop's time moves with where its head lies. A probe
builds each of the twelve sources of `tests/perf/` four ways: as c1
writes it, with `.p2align 5` before every loop head, with `.p2align
4` or `.p2align 6` in its place, and with no directive; forty-eight
binaries, whose four listings of a source differ in their `.p2align`
lines alone. It ran on the machine of chapter 4's table, on
2026-11-17, with the cpufreq governor `schedutil` and the frequency
boost on. Fifteen rounds ran the binaries interleaved, every variant
once a round, so that each saw the same load. Every run was pinned to
one core and timed in user plus system CPU time; the least of the
fifteen is kept, the median in parentheses. The tuned rows, in ms:

| program | none | `.p2align 4` | `.p2align 5` | `.p2align 6` |
|---|---|---|---|---|
| collatz | 492 (494) | 489 (596) | 496 (513) | 488 (490) |
| fib | 622 (624) | 621 (624) | 620 (625) | 622 (623) |
| matmul | 625 (636) | 579 (580) | 591 (592) | 590 (592) |
| sieve | 437 (449) | 501 (530) | 496 (559) | 445 (458) |
| sort | 753 (755) | 752 (753) | 528 (529) | 752 (754) |
| strings | 587 (589) | 575 (577) | 582 (584) | 586 (590) |

The whole series ran twice, and the second's least lies within 1.4%
of the first's in every cell. The plain rows, whose hot loops keep
their variables in slots, are a wash: no variant moves one by more
than 2% from `.p2align 5`.

The hot loops of matmul and sort answer the same placement in
opposite ways:

- matmul's inner loop, 32 bytes, runs in 579 to 591 ms where it lies
  whole in one 64-byte line, under the three directives, and in 625
  where it spans two, with none;
- sort's shift loop, 33 bytes, runs in 752 and 753 ms where it lies
  whole in one line, with no directive and under `.p2align 4` and
  `.p2align 6`, and in 528 where it spans two, from the middle of a
  line, under `.p2align 5`.

Sieve's strike loop, 17 bytes, lies whole in one line under all four,
and at the start of one under both `.p2align 5` and `.p2align 6`, yet
runs in 496 ms under the one and 445 under the other. Collatz, fib and
strings move by 2.1% at most. Zen 2 fetches 32 bytes of code a cycle,
and its op cache and branch predictor keep their entries by code
address in 64-byte lines; which of these a given loop runs into is not
read from the numbers, and this page does not guess.

No variant wins every row. Against `.p2align 5`, the other three lose
sort by 42% to 43%, 752 and 753 ms against 528, and no alignment loses
matmul by 5.8%; but `.p2align 4` wins matmul and strings by 2.0% and
1.2%, and no alignment and `.p2align 6` run sieve in 437 and 445 ms
against 496. `.p2align 5`'s runs of sieve spread 63 ms from least to
median; in the second series, where they spread 23, the two win sieve
by 9.4% and 8.2%. Over the six rows `.p2align 5` has the least sum,
3313 ms; `.p2align 6` lies 5.1% over it, no alignment 6.1% and
`.p2align 4` 6.2%, and 5.5%, 6.6% and 6.3% in the second series. The
margin is sort's: without its row, each of the other three sums less
than `.p2align 5`, in both series.

The decision: c1 writes `.p2align 5` before every loop head
(`backend.md` 7.2, and chapter 13, call 8). It is a convention of the
target, as the stack's 16 bytes at a call are: it says where the
machine fetches well and nothing about the program. It has two costs:

- Time. The padding lies on the path into the loop, so it is executed
  once per entry of the loop: at most 31 bytes of multi-byte `nop`s,
  since the directive names no fill byte. Before the step loop of
  `rcc collatz-tuned.c` the 18 bytes from 0x11ae are two
  instructions, `cs nopw 0x0(%rax,%rax,1)` and `nopl
  0x0(%rax,%rax,1)`.
- Size. The code grows by the padding, up to 31 bytes a loop. The
  directive also raises the alignment of `.text` from 4 bytes to 32,
  so every function moves.

The x86-64 numbers of this chapter are thus a fact about c1's layout,
every loop head at offset 0 of its 32-byte window, and no longer about
where the code before a loop happens to end. The heads of the five hot
loops of chapter 4 lie at 0x11c0 in collatz, 0x1320 in matmul, 0x1240
in sieve, 0x1220 in sort and 0x1140 in strings' `length`. The code
before a head still decides which half of a 64-byte line the head
opens: the heads in sort and matmul open the second half, the other
three the first.

`.p2align 6` adds up to 96 bytes to the code of a tuned program, and
before the five hot heads its padding is 173 bytes in 20 `nop`s
against 109 in 13. The probe pins its runs to one core, which `make
bench` does not, and its tuned cells under `.p2align 5`, the binaries
of chapter 4's table, lie from 2.2% under to 6.2% over that table's
cells: the comparison is among its own columns.

`.p2align 6` loses more on sort than it gains on sieve, with more
padding. Alignment to 64 bytes is thus measured on this target, not
adopted, and a tuned ratio of chapter 4 remains a fact about one
layout.

On aarch64 the probe ran on two cores, a Qualcomm Oryon and the
Firestorm core of an Apple M1 Max. It built each of the twelve sources
four ways: as c1 writes it, and with `.p2align 4`, `.p2align 5` or
`.p2align 6` before every loop head; forty-eight binaries. Fifteen
rounds ran them interleaved, each run pinned to one core and timed in
user CPU time, the least kept, and the whole series ran twice. Over
the tuned rows, against no alignment:

- `.p2align 4` nets +0.9% on the Oryon and +0.7% on Firestorm;
- `.p2align 5` nets -0.5% and -0.3%;
- `.p2align 6` nets +1.6% and +2.0%.

No variant moves a tuned row by more than 5% on the Oryon, or by more
than 3.5% on Firestorm but matmul under `.p2align 6`, +8.8%; on
x86-64 one moves a tuned row by up to 43%. The plain rows move by
2.2% at most on the Oryon and 1.5% on Firestorm. `clang -O2` aligns
no loop head on this target: it writes one `.p2align 2` per function.

The decision: c1 writes no alignment directive on aarch64
(`aarch64.md` 7.2, and chapter 15, call 7). The best variant nets half
a percent or less, and its padding would lie on the path into every
loop: on aarch64 the padding is 4-byte `nop`s, one instruction each.
Alignment on this target is thus measured, not adopted.

### 4.3 The cost of a slot

`make slots` times one loop, `s += i` a thousand million times, in
four sources under `tests/slot/`: `rr.c` with the counter i and the
sum s both `register` variables, `sr.c` with i in a slot, `rs.c` with
s in a slot, and `ss.c` with both in slots. Each run is pinned to one
core and timed in user plus system CPU time, fifteen rounds, the least
kept, and the script turns the time into clock cycles per iteration
at the frequency the kernel reports. On the two kinds of core of the
Apple M1 Max, on the Qualcomm Oryon and on the Zen 2 core of the
x86-64 desktop, in clock cycles per iteration, with the cycles above
`rr`'s in parentheses:

| variant | Firestorm, 3036 MHz | Icestorm, 2064 MHz | Oryon, 3418 MHz | Zen 2, 3493 MHz |
|---|---|---|---|---|
| rr | 1.00 | 2.00 | 1.00 | 1.00 |
| sr | 1.38 (+0.37) | 7.01 (+5.00) | 7.08 (+6.08) | 7.81 (+6.81) |
| rs | 1.65 (+0.65) | 6.00 (+4.00) | 7.01 (+6.00) | 7.01 (+6.01) |
| ss | 1.63 (+0.63) | 7.28 (+5.28) | 7.63 (+6.62) | 7.54 (+6.54) |

The Zen 2 core ran with the frequency boost off, its clock fixed: with
the boost on, the frequency the kernel reports before the first run is
the idle core's and not the clock a run gets, though the times keep
their ratios to `rr`'s within 1%.

A slot the loop carries, stored in one iteration and loaded back in
the next, adds 0.37 to 0.65 cycles an iteration on Firestorm, 4.00 to
5.28 on Icestorm, 6.00 to 6.62 on the Oryon and 6.01 to 6.81 on
Zen 2. The count of loads barely moves that price, and two slots cost
little more than one. The listing is the same on the three aarch64
cores, so the worth of `register` is the core's: a variable left in
its slot makes this loop 1.37 to 1.65 times as long on Firestorm, 6.98
to 7.59 times on the Oryon, and 6.99 to 7.79 times on Zen 2.
`slot.md` explains the measurement from the ground up, with the
listings on both targets, and says what it leaves unexplained.
