# rcc: the guarantees, the reasoning

Draft 1 of the list, kept as the record of the decisions: the
arguments, the rejected alternatives, the probes. The list itself is
`guarantees.md`, one page; this document is where its entries come
from and is not the record of conformance.

This list was the conformance record. A program that stays inside ISO C
is meant to run the same under rcc as under any conforming compiler;
what rcc does not compile yet is `shape.md` chapter 11. Where
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
probes are at the end.

Draft 1 assumed the IR would be handed to LLVM's optimizer, and the
guard of G4, the round trip of G8 and the laundering of G16 were
designed to survive it. The pipeline since became literal (`cost.md`,
`llvm.md` chapter 2): no optimizer runs, those three mechanisms are
not emitted, and the arguments below stand as the record of what the
optimizer would have done to the plain instruction, which is why it
does not run.

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

**G7. Pointer comparison is address comparison.** ISO 6.5.9p6: the
relational operators, only within one object; 6.5.10p7 defines `==`
and `!=` between pointers to different objects, equal where one
"happens to immediately follow" the other. Promise: `<`, `<=`, `>`,
`>=`, `==`, `!=` compare addresses as unsigned integers, any two
pointers the type constraints of 6.5.9p2 and 6.5.10p2 admit. IR:
`icmp` on pointers, which LLVM already defines as address
comparison. Lost:
`&a + 1 == &b` no longer folds to 0 for two distinct objects, the
one-past-the-end address included (`tests/corpus/g7-ptr-equal.c`: 1
under rcc, 0 under `clang -O2`); for `<` and the other relational
operators, nothing beyond G6.

**G8. A load reads the bytes at the address.** Which object the bytes
belong to is not the compiler's concern. ISO 6.5.7p9: `p + n` is
defined only within "the same array object", "provided they exist";
provenance is the subject of TS 6010. LLVM has its own provenance,
LangRef, Pointer Aliasing Rules: "A pointer value formed from a
scalar `getelementptr` operation is based on the pointer-typed
operand of the `getelementptr`"; "A pointer value formed by an
`inttoptr` is based on all pointer values that contribute (directly
or indirectly) to the computation of the pointer's value"; and,
under Allocated Objects, "If a pointer that is not based on the
object tries to read or write to the object, it is undefined
behavior." The gep rule says nothing about `inbounds`: withholding
the flag removes the no-wrap promise, not the object. Probe 4 shows
the fold. So the promise comes in tiers:

- (a) Arithmetic that stays inside the object the pointer came from:
  plain `getelementptr`, honoured, alias analysis unimpaired.
- (b) Crossing objects through an integer in the source (`uintptr_t`
  arithmetic): `ptrtoint`, `inttoptr`, honoured by LLVM's rule.
- (c) Crossing objects with pointer arithmetic alone (`p + (q - p)`
  used to reach `q`'s object): honoured only if the pointer add is
  emitted as `inttoptr (add (ptrtoint p), n)`.

Delivering (c) costs in three parts: alias analysis cannot decompose
an `inttoptr` result, so a load through one may alias any object
whose address has escaped; scalar evolution does not model
`inttoptr`, so loop access analysis cannot prove the dependences a
vectorizer needs, and a loop over `a[i]` with the index as the round
trip does not vectorize; and the front end can keep the plain gep
only where it can prove the arithmetic stays inside a declared object
(a constant index within a declared array, a member through a
declared object): gep what you can prove, the sibling of G11.
Everything else takes the round trip, and the programmer buys the
optimizer back per site: `restrict` for aliasing, the grant (G15) for
bounds.

All three tiers are promised. (c) is the premise of the list, and
the measurement prices it rather than decides it: the corpus
compiled with the round trip everywhere, then with "gep what you can
prove", and what the vectorizer and alias analysis still do in each,
is the published cost. "Gep what you can prove" weakens nothing:
arithmetic that provably stays inside one object cannot reach
another, so the plain gep and the round trip mean the same thing
there.

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
to a pointer type with stricter alignment is undefined. Promise: the
access happens at the address, and the machine does what it does.
Machine: x86 performs ordinary loads and stores at any alignment, at
a penalty across a cache line; aarch64 performs them in normal memory
and faults only for exclusives, device memory and the stack pointer.
IR: the `align` on a load or store is a promise LLVM reasons from
(it folds `(uintptr_t)p & 3`, feeds known bits, and picks aligned
vector moves), and overestimating it is undefined behaviour in the
LangRef. Rule: align what you can prove. An access to a declared
object, or to a member reached through one, carries the natural
alignment, which the ABI makes true. An access through a pointer of
unknown origin, a parameter, a cast, an integer, carries `align 1`.
Lost: the alignment-arithmetic folds; on a strict-alignment target
(not ours) an `align 1` word access becomes byte loads. Scalar
accesses on x86-64 and aarch64 are the same instruction either way,
and vectorized loops over a pointer parameter already use unaligned
vector moves under clang, which can prove no more than the scalar
alignment. Rejected: natural alignment everywhere, a promise the
front end has no grounds to make.

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
operand's side effects complete before the next operand starts.**
ISO 6.5.1p2, p3: unsequenced side effects on one object are
undefined; the order of evaluation of operands and of arguments is
unspecified. These are two things. For the side effects the machine
does something definite, whatever the emitted code does, and the
compiler has no licence to reason from "cannot happen". For the
order there is no machine behaviour to copy: the source does not say
and the IR must, so the front end decides at every site whether the
list says so or not. This is the one promise on the list that fixes
a choice the machine never made. The choice is between an order
written down once, which the programmer can read, and an order
chosen silently per site, which is K&R's position (section 2.12:
neither operand order nor argument order is specified; the PDP-11
compiler pushed arguments right to left) and ISO's. Control means
the written order. Left to right is the natural order of the writing
system and needs no explanation. Promise: `i = i++ + ++i` has one
value, and the order says which. IR: emitted in that order; LLVM
reorders only where unobservable. Lost: nothing at the IR level; the
ABI occasionally wants one more register move.

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

**G16. A call goes out as written.** ISO 6.5.3.3p7: a function "defined
with a type that is not compatible with the type (of the expression)
pointed to by the expression that denotes the called function" is
undefined to call; 6.3.2.3p8 says the same of a converted pointer.
Promise: the arguments are passed as the call site's type says, per the
ABI, and the call is made. What lands there and what the callee does
with it is another business. If the call writes into the kernel, that
is what was asked. Nothing is added, dropped or rewritten. Machine: the
caller lays out the registers and slots the call site's type assigns
and jumps; the callee reads the ones its own type names; nothing checks
that the two agree. An extra argument is ignored, a missing one reads
what the register held, a wider one is read in its low part, a `void`
callee called for a value yields what the return register held. IR: the
`call` names the call site's function type. The LangRef says the type
"shall be the signature of the function being called" and attaches no
consequence to a mismatch; instcombine (`transformConstExprCastCall`)
rewrites a direct call to a visible callee of another type when the
operands cast losslessly: extra arguments are dropped, missing ones are
added as `getNullValue`, a value of the compiler's own (probe 5).
Indirect calls are left as written, but inlining makes indirect calls
direct, so the source cannot tell which calls are safe. Rule: where the
call site's type differs from the callee's declared type in the module,
the front end launders the callee through an empty inline asm, `call
ptr asm "", "=r,0"(ptr @g)`, and calls through the result. The
optimizer sees an opaque pointer; the linker sees the symbol. Where the
types agree, including a callee declared only in another unit, nothing
changes. Lost: at those sites only, an address load and an indirect
call instead of a direct one, and no inlining there. Rejected: a
promise for indirect calls only, since inlining decides which calls
stay indirect, not the source; and leaving the rewrite, since the zero
is a value the compiler invents, G5's objection.

## Storage

**G17. A string literal is a writable object.** ISO 6.4.5p7: "It is
unspecified whether these arrays are distinct provided their
elements have the appropriate values", and a program that modifies
one is undefined. Promise: each literal, adjacent literals joined
first (6.4.5p5), is its own array of static storage, distinct from
every other literal however spelled, and a store through a pointer
into it writes the byte, as under Ritchie's compiler. Machine: the
bytes lie in a writable section, and a store is a store. IR: the
literal's global without `constant` and without `unnamed_addr`.
Lost: equal literals merged into one array; the read-only section,
where a stray store faults; and the fold of a load from a literal to
its initializer, by which `clang -O2` returns the initializer's `'a'`
after a store of `'z'` and drops the store
(`tests/corpus/g17-string-store.c`: 122 under rcc, 97 under
`clang -O2`).

## Not in this list

- Implementation-defined behaviour is fixed per target, but in no
  table yet (`char` signedness, widths, bit-field layout, `>>` of
  negatives is in G3). Not promises, just fixed.
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
| `!range`, `!align` metadata; `align` above what the front end can prove | G5, G11 |
| `llvm.lifetime.start`, `llvm.lifetime.end` | G12 |
| `mustprogress`, `willreturn`, `llvm.loop.mustprogress` | G14 |
| a direct `call` whose function type differs from the callee's | G16 |
| `constant`, `unnamed_addr` on a string literal's global | G17 |

The grant (G15) is the one exception, and it is written in the source.

## The oracle and the pin

Per the README: the same program under `clang -O2` and under
rcc on a corpus of the known UB-divergence cases; where the two
differ is the experiment's output. The IR text per test program,
diffed, was the pin under the LLVM pipeline; the assembly text is the
pin now. The first corpus entry per guarantee is the
`opt -O2` probe that shows the fold the promise removes.

## Probes (LLVM 23.1.3, `opt -O2 -S`)

1. G9: `load i32, ptr null` in a function marked `null_pointer_is_valid`
   survives as a load; in an unmarked function the body folds to
   `ret i32 undef`.
2. G6: `icmp ult ptr %p, (gep i32, %p, %n)` survives; with `inbounds`
   it folds to `icmp sgt i64 %n, 0`.
3. G1: `(a + 1 > a) ? 1 : 0` folds to `a != INT_MAX`; with `nsw` it
   folds to `1`.
4. G8: two allocas `a` and `b`, a store of 3 to `b`'s address
   computed from `a` by the address difference, then a load of `b`.
   With the address as `getelementptr i8, ptr %a, %d` the function
   folds to `ret 2`, the earlier store to `b`, the store through the
   gep kept beside it; with the address as `inttoptr (add (ptrtoint
   %a), %d)` it folds to `ret 3`.
5. G16: direct calls to a visible `i32 g(i32)`. With an extra
   argument the call is rewritten to `g(1)`; with none, to `g(0)`;
   with an `i64` argument, and a call expecting a value from a `void`
   callee, left as written. An indirect call through a parameter is
   left as written in every shape; the same call in a helper inlined
   with `@g` as the argument folds to `ret i32 0`. With `@g` passed
   through `call ptr asm "", "=r,0"` the call survives as an indirect
   call. A module that only declares `g` with the call site's type
   sees no rewrite: the mismatch is visible only beside the
   definition, or under LTO.
