# rcc: the guarantees (draft 1)

This list is the conformance record. A program that stays inside ISO C
runs the same under rcc as under any conforming compiler. Where
ISO C says "undefined", this list says what happens. Each entry gives
the ISO paragraph, cited from C23 (ISO/IEC 9899:2024, N3220
numbering) where the text is there, and from an earlier edition only
where it is not; draft 1 needed no earlier edition. Then the promise,
the machine behaviour the promise names, the IR that delivers it, and
what the optimizer loses.

Control cuts both ways. The compiler does what the source says, the
foolish included. It never reasons from "this cannot happen" to "this
code is dead". The first objection is "UB lets the compiler catch
mistakes". The answer: a sanitizer catches mistakes; a promise is not a
mistake, and the compiler has no standing to help.

Checked on LLVM 23.1.3, `opt -O2` on hand-written IR. The
three probes are at the end.

## Integers

**G1. Signed arithmetic wraps.** ISO 6.5.1p5: an exceptional condition
is undefined. Promise: `+`, `-`, `*`, unary `-`, `++`, `--` on signed
types compute modulo 2^N; `-INT_MIN` is `INT_MIN`. Machine: two's
complement add, sub, mul discard the carry. IR: `add`, `sub`, `mul`,
`shl` without `nsw`. The one exception is the quotient `INT_MIN /
-1`, which follows G4: the target's instruction, wrapping on aarch64
and faulting on x86. Lost: `a + 1 > a` is `a != INT_MAX` instead of
`1`; induction variables lose the no-wrap fact that strength reduction
and vectorization rest on. Mitigation: machine-width indices, `!=`
loop tests, and the grant (G15).

**G2. Narrowing to a signed type truncates.** ISO 6.3.1.3p3:
implementation-defined, or a signal. Promise: the low N bits, read as
two's complement. IR: `trunc`. Lost: nothing; every compiler does
this. Written down so the record is complete.

**G3. Shift counts are taken modulo the width; signed left shift
wraps; signed right shift is arithmetic.** ISO 6.5.8p3: a count that is
negative or at least the width is undefined; p4: a signed left shift
that overflows is undefined; p5: right shift of a negative value is
implementation-defined. Promise: `E1 << E2` is `E1 * 2^(E2 mod W)`
modulo 2^W; `E1 >> E2` shifts in sign bits for signed `E1`. Machine:
x86 and aarch64 both mask the count to the width in the instruction.
IR: `shl`, `ashr`, `lshr` on `and %n, W-1`; no `nsw`, `nuw`, `exact`.
Lost: one `and` for a non-constant count, folded into the shift on
both targets. Rejected alternative: a count at or beyond the width
yields 0 or the sign; that needs a compare and a select on every
target and no machine does it.

**G4. Division does what the target's division instruction does.**
ISO 6.5.6p6, p7: a zero divisor is undefined, and so is a quotient
that does not fit. Promise: the result of `/` and `%` is the result of
the target's integer division instruction, on every operand. The
compiler adds no decision of its own. Machine: x86 `idiv` faults on a
zero divisor and on `INT_MIN / -1`; aarch64 `sdiv` yields 0 for a
zero divisor and wraps on `INT_MIN / -1`, no fault. The program does
what the machine it runs on does. IR: LLVM's `sdiv` and `srem` are
undefined on both cases, so the mid-end would otherwise reason from
"the divisor is not zero" and delete checks or hoist the division.
The front end guards every division with a non-constant divisor: a
compare and a branch, and in the cold branch the target's behaviour
reproduced (aarch64: quotient 0, remainder the dividend; x86: the
division executed as is, so the fault happens). A constant divisor
needs no guard. Lost: the compare and branch, the same on every
target. Rejected: inline assembly for the division itself, which
blocks constant folding and strength reduction.

**G5. An uninitialized object holds a value.** ISO 6.3.2.1p2: reading
an uninitialized automatic whose address is never taken is undefined;
otherwise the representation is indeterminate. Promise: the object
holds some value of its type, and every read yields that value until
a write touches it. Which value is not promised, and the compiler
does not choose one. Machine: the register or stack slot holds what
was there. IR: the front end never writes `undef` or `poison` and
never emits `noundef` on parameters or returns. After mem2reg a read
before any write becomes `undef` anyway, and LLVM may give `undef` a
different value at each use; `freeze` on every load of an automatic
pins it to one. LLVM folds the `freeze` away where it can prove the
value defined. Lost: the folds `freeze` blocks on the loads it does
not fold away; measured, not guessed. Rejected: a zero store per
automatic at entry, what `-ftrivial-auto-var-init=zero` ships. It is
a value the compiler invents.

## Pointers and memory

**G6. Pointer arithmetic is address arithmetic.** ISO 6.5.7p9, p10:
defined only within an array object and one past it. Promise: `p + n`
is the address `p + n * sizeof *p` modulo 2^N; `p - q` is the address
difference divided by the size, for any two pointers of the type.
Machine: an add. IR: `getelementptr` without `inbounds`, `nusw` or
`nuw`; the difference as `ptrtoint`, `sub`, `sdiv` without `exact`.
Lost: `p < p + n` no longer folds to `n > 0` (probe 2); the pointer
form of the induction-variable facts of G1. Same mitigations.

**G7. Pointer comparison is address comparison.** ISO 6.5.9p6: only
within one object. Promise: `<`, `<=`, `>`, `>=`, `==`, `!=` compare
addresses as unsigned integers, any two pointers. IR: `icmp` on
pointers, which LLVM already defines as address comparison. Lost:
nothing beyond G6.

**G8. A load reads the bytes at the address.** Which object the bytes
belong to is not the compiler's concern. ISO: 6.5.7p9 ("provided they
exist"); provenance is the subject of TS 6010. LLVM has its own
provenance: a `getelementptr` result is "based on" its base pointer,
and a load through a pointer not based on the object it reaches is
undefined in LLVM with or without `inbounds`. Only the integer round
trip launders it: a pointer from `inttoptr` is based on every pointer
that contributed to the integer. So the promise comes in tiers:

- (a) Arithmetic that stays inside the object the pointer came from:
  plain `getelementptr`, honoured, alias analysis unimpaired.
- (b) Crossing objects through an integer in the source (`uintptr_t`
  arithmetic): `ptrtoint`, `inttoptr`, honoured by LLVM's rule.
- (c) Crossing objects with pointer arithmetic alone (`p + (q - p)`
  used to reach `q`'s object): honoured only if every pointer add is
  emitted as the integer round trip, which makes alias analysis give
  up on nearly all pointer arithmetic.

**Decision:** draft promises (a) and (b). (c) is the first measurement
of the project: the corpus compiled both ways, and what the optimizer
loses when every `gep` is `inttoptr (add (ptrtoint p), n)`.

**G9. An access through a null pointer is performed at address 0.**
ISO 6.5.4.2p4. Machine: page 0 is unmapped on every target we care
about, so the access faults with SIGSEGV; where page 0 is mapped the
bytes there are read. IR: the `null_pointer_is_valid` function
attribute on every function (probe 1: the load survives `-O2`; without
the attribute the function folds to `ret undef`); no `nonnull`, no
`dereferenceable` on parameters. Lost: `if (p == 0)` after `*p` is no
longer deleted. Nothing else.

**G10. Memory is bytes; a load through any lvalue reads the bytes at
the address as the lvalue's type.** ISO 6.5.1p7 (effective type) and
the union rules. Promise: casting a pointer and reading through it,
or writing one union member and reading another, is a load of the
bytes. IR: no `!tbaa` metadata, ever. Lost: type-based alias
analysis, the fact that lets an `int` store and a `float` load be
reordered. `restrict` gives it back per site (`noalias` on the
parameter is emitted for `restrict` and nothing else).

**G11. A misaligned access is performed.** ISO 6.3.2.3p7: converting
to a pointer type with stricter alignment is undefined. Machine: x86
tolerates any alignment for ordinary loads; aarch64 tolerates it for
ordinary loads and faults for exclusive and some SIMD forms. IR: the
`align` on a load is a fact LLVM reasons from (it folds `(uintptr_t)p
& 3` and selects aligned vector moves). Draft rule: an access through
a pointer that came from a cast or from integer arithmetic carries
`align 1`; an access through a declared object or a pointer to one
carries the natural alignment. Lost: aligned vector loads in loops
over cast-derived pointers. **Decision:** the rule above, or natural
alignment everywhere and "the machine does what it does" (which
licenses LLVM to fold alignment tests and is a weaker promise).

**G12. A local's storage is one stack slot for the whole activation.**
ISO 6.2.4p2: a pointer to an object past its lifetime is
indeterminate. Promise: within one call, a block-scoped local keeps
its slot after its block ends; a pointer to it still reads the slot.
Nothing is promised after the function returns. IR: no
`llvm.lifetime.start` and `llvm.lifetime.end` markers, which are what
lets stack colouring share slots. Lost: larger frames in functions
with many block-scoped locals that have their address taken.

## Control

**G13. Operands are evaluated left to right, arguments too, and an
operand's side effects complete before the next operand starts.** ISO
6.5.1p2, p3: unsequenced side effects are undefined; argument order is
unspecified. Promise: `i = i++ + ++i` has one value and this list says
which. Machine: nothing; this is emission order. IR: emitted in that
order; LLVM reorders only where unobservable. Lost: nothing at the IR
level; the ABI occasionally wants one more register move. Note: K&R
left the order open on purpose, so the compiler could choose the
cheaper one. That cost is gone on a register machine with LLVM behind
the front end. **Decision:** include it; it is the one promise ISO
withholds that costs nothing.

**G14. A loop runs until its condition says stop.** ISO 6.8.6.1p4:
a loop without observable effects may be assumed to terminate.
Promise: `for (;;);` hangs. IR: no `mustprogress` function attribute,
no `llvm.loop.mustprogress` metadata; LLVM assumes progress only when
told. Lost: nothing.

**G15. The grant.** The licence as something written in the source.
A per-declaration attribute (name open; `_Nowrap` as a placeholder)
on an integer or pointer variable says: arithmetic on this object
does not wrap and stays in bounds. The front end emits `nsw`, `nuw`,
`inbounds` on operations whose result is stored to that object, and
nowhere else. The programmer grants per site; the default stays the
promise; the cost of a wrong grant is the ISO cost, at one named place.

## Not in this list

- Implementation-defined behaviour is fixed per target in a separate
  table (`char` signedness, widths, bit-field layout, `>>` of
  negatives is in G3). Not promises, just fixed.
- Calls through a mismatched function type (ISO 6.5.3.3p7). The call
  goes out per the ABI of the pointer's type. What the callee makes of
  it is the callee's affair and not deliverable past the ABI; LLVM
  rewrites a direct call through a cast when it thinks it safe. Open.
- Library undefined behaviour (overlapping `strcpy`, bad `printf`
  formats): the library's, not the compiler's.
- Threads and the memory model: out of scope for now.
- Floating point: the target's IEEE 754, no fast-math flags, nothing
  to promise.

## What the front end never writes

The IR vocabulary rcc withholds, as a checklist for the audit
of emitted IR:

| Never emitted | Promise |
|---|---|
| `nsw`, `nuw` on `add`, `sub`, `mul`, `shl` | G1, G3 |
| `exact` on `sdiv`, `udiv`, `ashr`, `lshr` | G3, G6 |
| unguarded `sdiv`, `srem` with a non-constant divisor | G4 |
| `undef`, `poison`, `noundef` | G5 |
| `inbounds`, `nusw`, `nuw` on `getelementptr` | G6 |
| `nonnull`, `dereferenceable`, `!nonnull`; function without `null_pointer_is_valid` | G9 |
| `!tbaa`; `noalias` except from `restrict` | G10 |
| `!range`, `!align` metadata | G5, G11 |
| `llvm.lifetime.start`, `llvm.lifetime.end` | G12 |
| `mustprogress`, `willreturn`, `llvm.loop.mustprogress` | G14 |

The grant (G15) is the one exception, and it is written in the source.

## The oracle and the pin

Per the README: the same program under `clang -O2` and under
rcc on a corpus of the known UB-divergence cases; where the two
differ is the experiment's output. The IR text per test program,
diffed, is the pin. The first corpus entry per guarantee is the
`opt -O2` probe that shows the fold the promise removes.

## Probes (LLVM 23.1.3, `opt -O2 -S`)

1. G9: `load i32, ptr null` in a function marked `null_pointer_is_valid`
   survives as a load; in an unmarked function the body folds to
   `ret i32 undef`.
2. G6: `icmp ult ptr %p, (gep i32, %p, %n)` survives; with `inbounds`
   it folds to `icmp sgt i64 %n, 0`.
3. G1: `(a + 1 > a) ? 1 : 0` folds to `a != INT_MAX`; with `nsw` it
   folds to `1`.
