# rcc: the guarantees

The conformance record. Inside ISO C, rcc aims to be a conforming
compiler: what it does not compile yet is `shape.md` chapter 11, and
its known defects are `TODO`. Where ISO C says "undefined", this list
says what happens, and it fixes two choices ISO C leaves to the
implementation: G2's narrowing (6.3.1.3p3) and G3's right shift of a
negative value (6.5.8p5). Each entry gives the paragraph (C23, N3220
numbering), the promise, and the instruction that delivers it on
x86-64 (`backend.md` chapter 9); aarch64's are the table of
`aarch64.md` chapter 9. One rule: whatever behaviour the platform has,
no decision of the compiler's own. The compiler does what the
source says, the foolish included. rcc is a literal compiler
(`cost.md`): no optimizer runs, so most of these
promises are the machine's own behaviour, reached by writing the
plain instruction and nothing in between. Arguments, alternatives, and
the probes of what an optimizer would have done: `rationale.md`.

## Integers

**G1. Signed arithmetic wraps.** 6.5.1p5. `+`, `-`, `*`, `++`, `--` and
unary `-` on signed types compute modulo 2^N; `-INT_MIN` is `INT_MIN`.
Instruction: `add`, `sub`, `imul`, `shl`, `neg`; the machine
computes modulo 2^N.

**G2. Narrowing to a signed type truncates.** 6.3.1.3p3. The low N
bits, as two's complement. Instruction: a narrow store, `movb`,
`movw` or `movl`, or the narrower name of the register; nothing is
computed.

**G3. Shifts.** 6.5.8p3, p4, p5. The count is taken modulo the width; a
signed left shift wraps; a signed right shift is arithmetic.
Instruction: `shl`, `sar`, `shr` by `cl` or by an immediate, the count
as written, which the instruction masks to the width; on aarch64
`lsl`, `asr`, `lsr` by a register, which the instruction masks, or by
an immediate the generator masks to the width, since the
instruction's immediate cannot hold more: `x << 33` on an `int` is
`shll $33, %eax` on x86-64 and `lsl w0, w0, #1` on aarch64. A
constant count that is a multiple of the width of the promoted left
operand is a shift by 0, the identity, and writes no instruction
(`cost.md` chapter 0): `x << 32` on an `int` and `x >> 64` on a
`long` are `x`, and `x << 33` keeps its instruction.

**G4. Division does what the target's instruction does.** 6.5.6p6, p7.
x86 `idiv` faults on a zero divisor and on `INT_MIN / -1`; aarch64
`sdiv` yields 0 and wraps. Instruction: `idiv` or `div`, whatever
the divisor; the machine raises `#DE` on a zero divisor and on
`INT_MIN / -1`, and nothing between the source and the instruction
reasons about the divisor.

**G5. An uninitialized object holds a value.** 6.3.2.1p2. Some value of
its type, the same at every read until a write; which one, the compiler
does not choose. Instruction: the slot's bytes, or the register's
content, as nothing wrote them. Nothing in the generator touches
either before the program writes, so every read before the first
write sees the same value. A `register` variable read before a
write, at a read in its own initializer too (6.2.1p7), is its
register as its declaration finds it: as the prologue left it where
no variable of the function held that register before, and
otherwise as the last variable to hold it left it. The one
instruction G5 costs is the extension of an unwritten `char`, `short`
or `unsigned` register variable in its own register at its
declaration, `movsbl %bl, %ebx` for a `char`, which leaves the value
of its type as it was.

## Pointers and memory

**G6. Pointer arithmetic is address arithmetic.** 6.5.7p9, p10. `p + n`
is `p + n * sizeof *p` modulo 2^N; `p - q` is the address difference
divided by the size, for any two pointers. The type constraints of
6.5.7p2, p3 stand, but for GNU C's arithmetic on a `void *`, a step
of a byte, which rcc takes with a warning (`shape.md` chapter 5): no
arithmetic on a function pointer, and `p - q` only on pointers to
compatible types; rcc refuses the
rest with an error. Instruction: `lea`, `add` or `sub` on the 64-bit
registers, `imul` by the size; the difference is `sub`, `cqto` and
`idiv` by the size. The difference costs a division (`cost.md`).

**G7. Pointer comparison is address comparison.** 6.5.9p6. Addresses
compared as unsigned integers, any two pointers the type constraints
of 6.5.9p2 and 6.5.10p2 admit; rcc refuses the rest with an error.
ISO C defines `==` and `!=` between pointers to different objects
already (6.5.10p7), so the promise adds `<`, `<=`, `>` and `>=`.
Instruction: `cmp` of the 64-bit addresses, and the unsigned `jcc` or
`setcc`.

**G8. A load reads the bytes at the address.** 6.5.7p9. Whichever
object the bytes belong to, however the address was computed.
Instruction: `mov (L), L`, whatever computed `L`. Nothing reasons
about which object an address points into, so the `mov` reads the
address computed.

**G9. An access through a null pointer is performed at address 0.**
6.5.4.2p4. A fault, or the bytes at 0 where it is mapped.
Instruction: the same `mov`; the machine faults, or reads page zero
where it is mapped.

**G10. Memory is bytes.** 6.5.1p7 and the union rules. A load through
any lvalue reads the bytes at the address as the lvalue's type.
Instruction: the same `mov`, at the lvalue's width and extension. A
spill slot is the generator's, no object of the program, and its read
at a row's width is `backend.md` 6.2's.

**G11. A misaligned access is performed.** 6.3.2.3p7. The access
happens at the address. Instruction: the same `mov`; x86 has one
`mov` for every alignment.

**G12. A local's storage is one stack slot for the whole activation.**
6.2.4p2. A block-scoped local in a slot keeps the slot after its
block ends, within the call; a `register` local has no address
(6.5.4.2p1), so no pointer reaches it after its block. Instruction:
the frame of `backend.md` chapter 5, in which every local's slot is a
displacement off `rbp`, assigned at its declaration, none shared and
none reused within the call.

## Control

**G13. Left to right.** 6.5.1p2, p3, 6.7.11p20, p24. Operands and
arguments are evaluated left to right, and an operand's side effects
complete before the next operand starts; `i = i++ + ++i` has one
value. The one choice the machine never made; the written order is the
order. An assignment operator's left operand is evaluated first, for
its place: the address, and any side effect in computing it. A
compound assignment then evaluates its right operand and reads the
object as part of its operation, the read, the operator and the store
with nothing between: `x += e` is `e`, then the `add` on `x`. So, with
`x` 1 before, `x += (x = 2)` leaves 4 where `x = x + (x = 2)` leaves
3. The first stores 2, then reads `x` as 2 and adds the assignment's
2. The second names `x` as an operand of `+`, so it reads 1 first,
then stores 2, then stores 1 + 2. An initializer list is evaluated in
its written order, which 6.7.11p24 leaves unspecified: each
initializer is evaluated and stored into its subobject before the next
is evaluated, so `struct point q = { f(), g() };` calls `f`, stores
`q.x`, then calls `g`. An initializer a later one overrides (p20) is
evaluated and stored all the same, which the paragraph's footnote lets
an implementation leave out: `{ .x = f(), .x = g() }` calls `f` and
stores its value, then calls `g` and stores over it. GNU C's statement
expression is one operand: its statements run where it stands, after
the operands to its left and before those to its right, so `f() + ({
g(); 1; }) + h()` calls `f`, then `g`, then `h`, and a jump out of it
leaves the operands to its right unevaluated. A variable length
array's size is evaluated where its declaration is reached, after the
declarators before it (6.8.1p3, p4).

**G14. A loop runs until its condition says stop.** 6.8.6.1p4. `for
(;;);` hangs. Instruction: the test at the top of the loop and a
`jcc` out, and a `jmp` back; nothing reasons about the test.

**G15. Withdrawn.** The number is reserved. It was the grant: a
per-declaration attribute by which the programmer handed an
optimizer `nsw` and `inbounds` on one variable. A literal compiler
has no optimizer to hand a flag to, so there was nothing to grant.
`rationale.md` keeps the design.

**G16. A call goes out as written.** 6.5.3.3p7, 6.3.2.3p8. The
arguments are passed as the call site's type says, per the ABI, and the
call is made; what the callee does with them is another business.
Nothing is added, dropped or rewritten. Instruction: the
convention's argument moves and `call f@PLT`, or `call *d(%rbp)`
through a pointer, with the call site's types (`backend.md` chapter
8). A call through a pointer converted to another function type is
made with that type, and nothing checks it against the function the
pointer came from. Nothing between the source and the instruction
rewrites a call.

## Storage

**G17. A string literal is a writable object.** 6.4.5p7. Each literal,
adjacent literals joined first (6.4.5p5), is its own array of static
storage, distinct from every other literal however spelled, and a
store through a pointer into it writes the byte, as under Ritchie's
compiler. Instruction: a `.string` under a label of its own in
`.data`, one per joined literal, never in `.rodata`, so nothing marks
it read-only and nothing merges it.

**G18. A record's assignment copies its object, unit by unit.**
6.5.17.2p2. `a = b` on two structs or unions of one type copies the
right operand's object into the left's: inline, in units of the
type's alignment, 1, 2, 4 or 8 bytes, of which its size is a
multiple, from the lowest address up, each unit loaded and then
stored before the next is loaded, padding included, with no call.
The left operand's place is evaluated first, then the right's (G13).
Only an exact overlap is defined (6.5.17.2p3); any other overlap is
copied in the same order, and nothing checks for it, so `a = a`
writes every unit over itself. Up to eight units are written out;
above that, a loop copies them, in the same order. Instruction, on
x86-64: per unit the plain load of its width into a scratch
register, `movzbl`, `movzwl`, `movl` or `movq`, and the store,
`movb`, `movw`, `movl` or `movq`, the unit's offset in each place's
displacement; over eight units one index from minus the size up to
0, the `add` that steps it the loop's test, `jne` back. On aarch64:
per unit the plain load of its width into `x17`, `ldrb`, `ldrh` or
`ldr`, and its store, `ldrh w17, [x0]; strh w17, [x29, #28]`; over
eight units a loop of four instructions whatever the size, `ldr x17,
[x1], #8; str x17, [x0], #8; subs x16, x16, #1; b.ne` after `mov x16,
#n`; no `bl` (`aarch64.md` 6.6).

**G19. The variable arguments are read where the convention put
them.** 7.16.1.4p3, 7.16.1.1p2. A variadic definition that calls
`va_start` stores every argument register, general and vector, into
its save area at entry, whatever the caller says it passed in vector
registers, so a list the function hands to the C library reads each
argument where the convention placed it, a floating one among them,
which rcc has no type for. `va_start` writes the convention's list
from the area. `va_arg` takes the next 8-byte slot, the area's while
a general register is left and then the caller's stack, and loads it
at the type's width: a type unlike the promoted argument's reads the
slot's bytes as that type (G10), so a `char` reads the low byte of
the promoted `int`, and a read past the last argument reads what the
slot holds (G5). A list handed to a function is the convention's.
Instruction, on x86-64: after the named parameters' stores, `movq`
of `%rdi` to `%r9` into the area's 0 to 40 and `movaps` of `%xmm0` to
`%xmm7` into 48 to 160, with no test of `%al`, then `leaq 16(%rbp),
%rax`, 8 more for each named parameter past the sixth, stored at
176; `va_start` four stores, `movl $8, -216(%rbp)` for one named
parameter, `movl $48`, the address loaded from 176 and the area's
`leaq`; `va_arg` `cmpl $40` on `gp_offset` and `ja` to the stack's
path, there the address stepped by `addq $8`, here `addl $8` on the
offset and the area plus the offset less 8, then the load of the
type's width; a list handed on is the address of the caller's own
record, which the callee's `va_arg` advances (`backend.md` chapter 5).
On aarch64: at entry, before anything of the function's own, four
`stp` of `x0` to `x7` and four `stp` of `q0` to `q7` into the area,
then the address of the stacked arguments past the named ones, `add
x16, x29, #272; str x16, [x29, #208]`; `va_arg` the test of
`__gr_offs`, the step of the offset or of `__stack`, and the load of
the slot; a list handed on is copied by G18's four units into a slot
of the call's own and passed as that slot's address, `add x1, x29,
#272`, so the callee's `va_arg` advances the copy (`aarch64.md` 5.2,
chapter 8).

**G20. A local's initializer list writes every byte of its object.**
6.7.11p20, p22; 6.2.6.1p6. Every byte of a local object initialized
by a list is a listed value or zero, padding included, as an object of
static storage holds the same list; C23 leaves the padding
unspecified where a list is not empty. The declaration zeroes the
whole object first, in G18's units from the lowest address up, before
the first initializer is evaluated, whatever the list names; then the
listed values are stored in the list's order (G13). A braced list for
a subobject, or a value of another member of a union, that overrides
a value stored before zeroes the subobject's bytes again first, in
units of its alignment, at most 8 bytes, written out at any count, so
each byte holds what an object of static storage holds for the list.
Instruction, on
x86-64: per unit the store of the immediate 0 at its width into the
slot, `movb`, `movw`, `movl` or `movq`, `movl $0, -8(%rbp)`, with no
register; over eight units G18's loop of one index, `movq $-200,
%rax; movq $0, (%rbp,%rax,1); addq $8, %rax; jne` back
(`backend.md` 6.6). On aarch64: per unit the store of the zero
register at its width, `strb wzr`, `strh wzr`, `str wzr` or `str
xzr`, `str wzr, [x29, #16]`, with no other register; over eight
units a loop of three instructions whatever the size, `str xzr,
[x0], #8; subs x16, x16, #1; b.ne` after `add x0, x29, #16; mov x16,
#25` (`aarch64.md` 6.6).

**G21. A `_Bool` reads as its byte.** 6.2.6.1p5, 6.2.6.2p1. A
`_Bool` is one byte, read and written at its width as `unsigned char`
is. A conversion to `_Bool` stores 0 or 1 (6.3.1.2), and so does
every operator that stores into one, whose result is converted. A
`_Bool` assigned to a `_Bool` is copied as it is. A read yields the
byte the object holds, so one holding another value, written through
another type (G10) or never written (G5), reads as that byte until a
store replaces it: 2 is true in a condition, 2 in arithmetic, and
unequal to 1. Instruction: `movzbl` on x86-64, `ldrb` on aarch64, as
for `unsigned char`; no test, no mask.

## Not in this list

Implementation-defined behaviour beyond G2 and G3: fixed per
target, not promised, and gathered in `choices.md`. Library
undefined behaviour. Threads, out of scope for now. Floating point,
the target's IEEE 754, no fast-math flags.

## What the front end never writes

There is no IR, and nothing to withhold from the assembler.
`rationale.md` keeps the list of what the LLVM front end withheld
from an optimizer, as the record of the argument.
