# rcc: the guarantees

The conformance record. Inside ISO C, rcc is a conforming compiler.
Where ISO C says "undefined", this list says what happens: the
paragraph (C23, N3220 numbering), the promise, and the IR that
delivers it. One rule: whatever behaviour the platform has, no
decision of the compiler's own. The compiler does what the source
says, the foolish included. rcc is a literal compiler (`cost.md`): no
optimizer runs, so most of these promises are the machine's own
behaviour, reached by emitting the plain instruction and nothing in
between. Arguments, alternatives, and the probes of what an optimizer
would have done: `rationale.md`.

## Integers

**G1. Signed arithmetic wraps.** 6.5.1p5. `+`, `-`, `*`, `++`, `--` and
unary `-` on signed types compute modulo 2^N; `-INT_MIN` is `INT_MIN`.
IR: `add`, `sub`, `mul`, `shl` without `nsw`; the machine wraps.

**G2. Narrowing to a signed type truncates.** 6.3.1.3p3. The low N
bits, as two's complement. IR: `trunc`.

**G3. Shifts.** 6.5.8p3, p4, p5. The count is taken modulo the width; a
signed left shift wraps; a signed right shift is arithmetic. IR: `shl`,
`ashr`, `lshr` with the count as written, no `nsw`, `nuw`, `exact`;
the instruction masks the count on x86 and on aarch64.

**G4. Division does what the target's instruction does.** 6.5.6p6, p7.
x86 `idiv` faults on a zero divisor and on `INT_MIN / -1`; aarch64
`sdiv` yields 0 and wraps. IR: `sdiv`, `srem`, `udiv`, `urem`, plain,
whatever the divisor; the literal selector emits the instruction, and
nothing between the source and the instruction reasons about the
divisor.

**G5. An uninitialized object holds a value.** 6.3.2.1p2. Some value of
its type, the same at every read until a write; which one, the compiler
does not choose. IR: no `poison` or `noundef`; `freeze` on every load
of an automatic, which the literal selector lowers to nothing. A
`register` variable that may be read before a write has one slot,
never written. It is read once, at its declaration, or at a read in
its own initializer (6.2.1p7): a `load` and a `freeze`. That value
is the variable's at every read until a write.

## Pointers and memory

**G6. Pointer arithmetic is address arithmetic.** 6.5.7p9, p10. `p + n`
is `p + n * sizeof *p` modulo 2^N; `p - q` is the address difference
divided by the size, for any two pointers. IR: `getelementptr` without
`inbounds`, `nusw`, `nuw`; the difference as `ptrtoint`, `sub`, `sdiv`
without `exact`. The difference costs a division (`cost.md`).

**G7. Pointer comparison is address comparison.** 6.5.9p6. Addresses
compared as unsigned integers, any two pointers. IR: `icmp` on
pointers.

**G8. A load reads the bytes at the address.** 6.5.7p9, and LLVM's
provenance rule (LangRef, Pointer Aliasing Rules). Whichever object the
bytes belong to, however the address was computed. IR: a plain
`getelementptr` and a plain `load`. No alias analysis runs, so no
provenance is reasoned about, and the load reads the address
computed.

**G9. An access through a null pointer is performed at address 0.**
6.5.4.2p4. A fault, or the bytes at 0 where it is mapped. IR:
`null_pointer_is_valid` on every definition; no `nonnull`, no
`dereferenceable`.

**G10. Memory is bytes.** 6.5.1p7 and the union rules. A load through
any lvalue reads the bytes at the address as the lvalue's type. IR: no
`!tbaa`.

**G11. A misaligned access is performed.** 6.3.2.3p7. The access
happens at the address. IR: align what you can prove. A declared
object, or a member reached through one, carries the natural alignment;
a pointer of unknown origin carries `align 1`.

**G12. A local's storage is one stack slot for the whole activation.**
6.2.4p2. A block-scoped local keeps its slot after its block ends,
within the call. IR: no `llvm.lifetime.start` or `end`, and the back
end's stack colouring is off.

## Control

**G13. Left to right.** 6.5.1p2, p3. Operands and arguments are
evaluated left to right, and an operand's side effects complete before
the next operand starts; `i = i++ + ++i` has one value. The one choice
the machine never made; the written order is the order.

**G14. A loop runs until its condition says stop.** 6.8.6.1p4. `for
(;;);` hangs. IR: no `mustprogress`, `willreturn` or
`llvm.loop.mustprogress`.

**G15. Withdrawn.** The number is reserved. It was the grant: a
per-declaration attribute by which the programmer handed an
optimizer `nsw` and `inbounds` on one variable. Under the literal
back end nothing reads a flag, so there was nothing to grant.
`rationale.md` keeps the design.

**G16. A call goes out as written.** 6.5.3.3p7, 6.3.2.3p8. The
arguments are passed as the call site's type says, per the ABI, and the
call is made; what the callee does with them is another business.
Nothing is added, dropped or rewritten. IR: the call, with the call
site's type. Nothing between the source and the instruction rewrites
a call.

## Storage

**G17. A string literal is a writable object.** 6.4.5p7. Each literal
written is its own array of static storage, distinct from every other
literal however spelled, and a store through a pointer into it writes
the byte, as under Ritchie's compiler. IR: `private global [N x i8]`,
neither `constant` nor `unnamed_addr`, so nothing marks it read-only
and nothing may merge it.

## Not in this list

Implementation-defined behaviour, fixed per target in a separate
table. Library undefined behaviour. Threads, out of scope for now.
Floating point, the target's IEEE 754, no fast-math flags.

## What the front end never writes

The literal selector reads none of these. The table holds for the IR
as a document, and for the day the IR is fed to an optimizer.

| Never emitted | Promise |
|---|---|
| `nsw`, `nuw` on `add`, `sub`, `mul`, `shl` | G1, G3 |
| `exact` on `sdiv`, `udiv`, `ashr`, `lshr` | G3, G6 |
| `poison`, `noundef`, `undef` | G5 |
| `inbounds`, `nusw`, `nuw` on `getelementptr` | G6 |
| `nonnull`, `dereferenceable`; no `null_pointer_is_valid` | G9 |
| `!tbaa`; `noalias` except from `restrict` | G10 |
| `!range`, `!align`; `align` beyond what is proven | G5, G11 |
| `llvm.lifetime.start`, `llvm.lifetime.end` | G12 |
| `mustprogress`, `willreturn`, `llvm.loop.mustprogress` | G14 |
| `constant`, `unnamed_addr` on a string literal | G17 |

The grant that was G15 would have been the one exception.
