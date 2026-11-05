# rcc: the guarantees

The conformance record. Inside ISO C, rcc is a conforming compiler.
Where ISO C says "undefined", this list says what happens: the
paragraph (C23, N3220 numbering), the promise, the IR that delivers it,
what the optimizer loses. One rule: whatever behaviour the platform
has, no decision of the compiler's own. The compiler does what the
source says, the foolish included. Arguments, alternatives and probes:
`rationale.md`.

## Integers

**G1. Signed arithmetic wraps.** 6.5.1p5. `+`, `-`, `*`, `++`, `--` and
unary `-` on signed types compute modulo 2^N; `-INT_MIN` is `INT_MIN`.
IR: `add`, `sub`, `mul`, `shl` without `nsw`. Lost: `a + 1 > a` no
longer folds to 1; induction variables lose the no-wrap fact behind
vectorization.

**G2. Narrowing to a signed type truncates.** 6.3.1.3p3. The low N
bits, as two's complement. IR: `trunc`. Lost: nothing.

**G3. Shifts.** 6.5.8p3, p4, p5. The count is taken modulo the width; a
signed left shift wraps; a signed right shift is arithmetic. IR: `shl`,
`ashr`, `lshr` on `and %n, W-1`; no `nsw`, `nuw`, `exact`. Lost: one
`and` for a non-constant count.

**G4. Division does what the target's instruction does.** 6.5.6p6, p7.
x86 `idiv` faults on a zero divisor and on `INT_MIN / -1`; aarch64
`sdiv` yields 0 and wraps. IR: a division by a non-constant divisor is
guarded by a compare and branch whose cold side reproduces the target,
since LLVM's `sdiv` is undefined on both. Lost: the compare and branch.

**G5. An uninitialized object holds a value.** 6.3.2.1p2. Some value of
its type, the same at every read until a write; which one, the compiler
does not choose. IR: no `undef`, `poison` or `noundef`; `freeze` on
every load of an automatic. Lost: the folds `freeze` blocks.

## Pointers and memory

**G6. Pointer arithmetic is address arithmetic.** 6.5.7p9, p10. `p + n`
is `p + n * sizeof *p` modulo 2^N; `p - q` is the address difference
divided by the size, for any two pointers. IR: `getelementptr` without
`inbounds`, `nusw`, `nuw`; the difference as `ptrtoint`, `sub`, `sdiv`
without `exact`. Lost: `p < p + n` no longer folds to `n > 0`; G1's
loss, for pointers.

**G7. Pointer comparison is address comparison.** 6.5.9p6. Addresses
compared as unsigned integers, any two pointers. IR: `icmp` on
pointers. Lost: nothing beyond G6.

**G8. A load reads the bytes at the address.** 6.5.7p9, and LLVM's
provenance rule (LangRef, Pointer Aliasing Rules). Whichever object the
bytes belong to, however the address was computed. IR: the pointer add
is the integer round trip, `inttoptr (add (ptrtoint p), n)`, wherever
the front end cannot prove the arithmetic stays inside a declared
object; a plain `getelementptr` where it can. Lost: alias analysis and
scalar evolution stop at an `inttoptr`, so loops over the round trip do
not vectorize; the project's first published number. `restrict` and the
grant (G15) give it back per site.

**G9. An access through a null pointer is performed at address 0.**
6.5.4.2p4. A fault, or the bytes at 0 where it is mapped. IR:
`null_pointer_is_valid` on every function; no `nonnull`, no
`dereferenceable`. Lost: `if (p == 0)` after `*p` is no longer deleted.

**G10. Memory is bytes.** 6.5.1p7 and the union rules. A load through
any lvalue reads the bytes at the address as the lvalue's type. IR: no
`!tbaa`. Lost: type-based alias analysis; `restrict` gives it back per
site.

**G11. A misaligned access is performed.** 6.3.2.3p7. The access
happens at the address. IR: align what you can prove. A declared
object, or a member reached through one, carries the natural alignment;
a pointer of unknown origin carries `align 1`. Lost: the alignment
folds.

**G12. A local's storage is one stack slot for the whole activation.**
6.2.4p2. A block-scoped local keeps its slot after its block ends,
within the call. IR: no `llvm.lifetime.start` or `end`. Lost: stack
colouring.

## Control

**G13. Left to right.** 6.5.1p2, p3. Operands and arguments are
evaluated left to right, and an operand's side effects complete before
the next operand starts; `i = i++ + ++i` has one value. The one choice
the machine never made; the written order is the order. Lost: nothing.

**G14. A loop runs until its condition says stop.** 6.8.6.1p4. `for
(;;);` hangs. IR: no `mustprogress`, `willreturn` or
`llvm.loop.mustprogress`. Lost: nothing.

**G15. The grant.** A per-declaration attribute (`_Nowrap`,
provisional) on an integer or pointer variable says arithmetic on it
does not wrap and stays in bounds. IR: `nsw`, `nuw`, `inbounds` on
operations whose result is stored to that object, nowhere else. A wrong
grant costs the ISO cost, at one named place.

**G16. A call goes out as written.** 6.5.3.3p7, 6.3.2.3p8. The
arguments are passed as the call site's type says, per the ABI, and the
call is made; what the callee does with them is another business.
Nothing is added, dropped or rewritten. IR: where the call site's type
differs from the callee's declared type in the module, the callee is
passed through `call ptr asm "", "=r,0"`, out of instcombine's reach.
Lost: at those sites, an indirect call and no inlining.

## Not in this list

Implementation-defined behaviour, fixed per target in a separate
table. Library undefined behaviour. Threads, out of scope for now.
Floating point, the target's IEEE 754, no fast-math flags.

## What the front end never writes

| Never emitted | Promise |
|---|---|
| `nsw`, `nuw` on `add`, `sub`, `mul`, `shl` | G1, G3 |
| `exact` on `sdiv`, `udiv`, `ashr`, `lshr` | G3, G6 |
| unguarded `sdiv`, `srem` with a non-constant divisor | G4 |
| `undef`, `poison`, `noundef` | G5 |
| `inbounds`, `nusw`, `nuw` on `getelementptr` | G6 |
| `nonnull`, `dereferenceable`; no `null_pointer_is_valid` | G9 |
| `!tbaa`; `noalias` except from `restrict` | G10 |
| `!range`, `!align`; `align` beyond what is proven | G5, G11 |
| `llvm.lifetime.start`, `llvm.lifetime.end` | G12 |
| `mustprogress`, `willreturn`, `llvm.loop.mustprogress` | G14 |
| a direct `call` whose function type differs from the callee's | G16 |

The grant (G15) is the one exception.
