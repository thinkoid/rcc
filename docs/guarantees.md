# rcc: the guarantees

The conformance record. Inside ISO C, rcc is a conforming compiler.
Where ISO C says "undefined", this list says what happens: the
paragraph (C23, N3220 numbering), the promise, and the instruction
that delivers it (`backend.md` chapter 9). One rule: whatever
behaviour the platform has, no decision of the compiler's own. The
compiler does what the source says, the foolish included. rcc is a
literal compiler (`cost.md`): no optimizer runs, so most of these
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
as written; the instruction masks the count to the width, on x86 and
on aarch64.

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
register as the prologue left it. The one instruction G5 costs is
the extension of an unwritten `char`, `short` or `unsigned` register
variable in its own register at its declaration, `movsbl %bl, %ebx`
for a `char`, which leaves the value of its type as it was.

## Pointers and memory

**G6. Pointer arithmetic is address arithmetic.** 6.5.7p9, p10. `p + n`
is `p + n * sizeof *p` modulo 2^N; `p - q` is the address difference
divided by the size, for any two pointers. Instruction: `lea`, `add`
or `sub` on the 64-bit registers, `imul` by the size; the difference
is `sub`, `cqto` and `idiv` by the size. The difference costs a
division (`cost.md`).

**G7. Pointer comparison is address comparison.** 6.5.9p6. Addresses
compared as unsigned integers, any two pointers. Instruction: `cmp`
of the 64-bit addresses, and the unsigned `jcc` or `setcc`.

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
Instruction: the same `mov`, at the lvalue's width and extension.

**G11. A misaligned access is performed.** 6.3.2.3p7. The access
happens at the address. Instruction: the same `mov`; x86 has one
`mov` for every alignment.

**G12. A local's storage is one stack slot for the whole activation.**
6.2.4p2. A block-scoped local keeps its slot after its block ends,
within the call. Instruction: the frame of `backend.md` chapter 5,
in which every local's slot is a displacement off `rbp`, assigned at
its declaration, none shared and none reused within the call.

## Control

**G13. Left to right.** 6.5.1p2, p3. Operands and arguments are
evaluated left to right, and an operand's side effects complete before
the next operand starts; `i = i++ + ++i` has one value. The one choice
the machine never made; the written order is the order. An assignment
operator's left operand is evaluated first, for its place: the
address, and any side effect in computing it. A compound assignment
then evaluates its right operand and reads the object as part of its
operation, the read, the operator and the store with nothing between:
`x += e` is `e`, then the `add` on `x`. So `x += (x = 2)` leaves 4
where `x = x + (x = 2)` leaves 3: the second names `x` as an operand of
`+`, and reads it first.

**G14. A loop runs until its condition says stop.** 6.8.6.1p4. `for
(;;);` hangs. Instruction: the test at the top of the loop, a `cmp`
and a `jcc` out, and a `jmp` back; nothing reasons about the test.

**G15. Withdrawn.** The number is reserved. It was the grant: a
per-declaration attribute by which the programmer handed an
optimizer `nsw` and `inbounds` on one variable. A literal compiler
has no optimizer to hand a flag to, so there was nothing to grant.
`rationale.md` keeps the design.

**G16. A call goes out as written.** 6.5.3.3p7, 6.3.2.3p8. The
arguments are passed as the call site's type says, per the ABI, and the
call is made; what the callee does with them is another business.
Nothing is added, dropped or rewritten. Instruction: the
convention's argument moves and `call f@PLT`, with the call site's
types (`backend.md` chapter 8). Nothing between the source and the
instruction rewrites a call.

## Storage

**G17. A string literal is a writable object.** 6.4.5p7. Each literal
written is its own array of static storage, distinct from every other
literal however spelled, and a store through a pointer into it writes
the byte, as under Ritchie's compiler. Instruction: a `.string`
under a label of its own in `.data`, one per literal written, never
in `.rodata`, so nothing marks it read-only and nothing merges it.

## Not in this list

Implementation-defined behaviour, fixed per target in a separate
table. Library undefined behaviour. Threads, out of scope for now.
Floating point, the target's IEEE 754, no fast-math flags.

## What the front end never writes

There is no IR, and nothing to withhold from the assembler.
`rationale.md` keeps the list of what the LLVM front end withheld
from an optimizer, as the record of the argument.
