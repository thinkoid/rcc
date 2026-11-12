# rcc: `*p++` on a register pointer

The design of one row of the aarch64 generator, written before its
code: where the page says "today", it means the generator without
the row. On aarch64 a load or a store can step its base register in
the same instruction. This page says what those two addressing forms
are, where they enter the design of `docs/aarch64.md`, what one rule
in the generator uses them, and the one condition the order of
evaluation puts on that rule. The last chapter lists the calls and
their answers.

The C citations are N3220. Every listing marked "rcc" was written by
the compiler. The two forms were assembled by `clang` and run on an
aarch64 Linux machine; the output is in chapter 2.

## 0. tl;dr

- aarch64 has two addressing forms that write their base register:
  `[x1], #d` makes the access and then adds `d`; `[x1, #d]!` adds `d`
  and then makes the access.
- `*p++` on a `register` pointer is three instructions for a load by
  the rules that carry from x86-64. With the post form it is one,
  `ldrb w0, [x19], #1`.
- The form belongs to the address, not to the load. One new case in
  the function that already builds every dereference's address
  serves loads, stores and both ends of `*p++ = *q++`.
- The step moves from the address's evaluation to the access. For a
  load the two are adjacent. For a store the right operand lies
  between them, so the form is taken only when the right operand
  does not name the pointer. This is the whole of what G13 asks.
- Only a `register` pointer qualifies: the form writes a register.
- The Seventh Edition compiler had this row, under the same
  condition, for the PDP-11's two autoincrement modes.

## 1. The terms

- **An access** is one load or one store. `c = *p` makes one. `&*p`
  makes none.
- **An address** is what an access goes through: the operand in
  square brackets. It is distinct from a pointer's value held in a
  register, which is a number like any other.
- **A row** is the instruction the generator writes for one operator
  in one context. `docs/aarch64.md` 6.3 is the table of rows.
- **A register variable** is a local declared `register` that the
  compiler placed in a callee-saved register, `x19` to `x28` on
  aarch64. It has no address (6.5.4.2p1), so only an expression that
  names it can read or write it.
- **A slot** is the stack storage of any other local, at a
  displacement off `x29`.
- **G13** is the guarantee that operands are evaluated left to right
  (`docs/guarantees.md`). Chapter 5 quotes it.
- **A pin** is the committed assembly text of a test program, which
  `make test` diffs against what the compiler writes.

## 2. What 2.3 is, and the two forms that join it

Chapter 2 of `docs/aarch64.md` is the background on the machine. It
holds no decision. It states what the instruction set offers, so
that the later chapters can cite a fact instead of restating it. Its
section 2.3, "The addressing forms and their reach", is the list of
operands a load or a store accepts, and how far each reaches. Five
of them leave the base as it was:

| Form | Reads from |
|---|---|
| `[x1]` | the address in `x1` |
| `[x1, #d]` | `x1 + d`, `d` a constant |
| `[x1, x2]` | `x1 + x2` |
| `[x1, x2, lsl #k]` | `x1 + (x2 << k)`, `k` the access's own size |
| `[x1, w2, sxtw #k]`, `uxtw` | `x1` plus the 32-bit `w2`, extended and shifted |

None of the five changes `x1`. The machine has two more, which do:

| Form | Name | What it does |
|---|---|---|
| `[x1], #d` | post-indexed | the access at `x1`, then `x1 = x1 + d` |
| `[x1, #d]!` | pre-indexed | `x1 = x1 + d`, then the access at the new `x1` |

`d` is a signed 9-bit number, -256 to 255, in bytes, not scaled by
the access size. Every load and store of 2.2's table takes both
forms. Run on the machine, with `p` pointing at the `c` of
`"abcdef"`:

```
post load:  read 'c', p moved +1
pre load:   read 'd', p moved +1
pre load -1: read 'b', p moved -1
post store: "abXdef", p moved +1
pre store:  "abXYef", p moved +1
```

They belong in 2.3 for the reason the other five are there: they
are operands of the same instructions, and the generator must know
their reach. 2.3 gains the two rows, the range of `d`, and one
restriction: the register loaded or stored may not be the base.
`clang` refuses `ldr x19, [x19], #8` with "unpredictable LDR
instruction, writeback base is also a source", and GNU `as` assembles
it with a warning.

2.3 does not say when the forms are used. That is a row of chapter
6, as it is for the other five.

## 3. Where an address is made today

`pointed()` is not new. It is in `src/c1-aarch64.cc` since slice 2,
and it builds the address of every dereference. The parser gives the
generator three tree shapes for what looks alike in the source:

| Source | Tree |
|---|---|
| `*p` | deref over the name `p` |
| `p[i]` | deref over an add of `p` and `i` (6.5.3.2p2) |
| `*p++` | deref over a postfix `++` of the name `p` |

`pointed()` takes the operand of the deref and returns an
`address_t`: a held base, with an index or a displacement when there
is one. It has two cases today:

```c++
address_t
pointed(state_t &s, const expr_t &p)
{
    if (!is< binary_t >(p) || as< binary_t >(p).op != binary_op_t::add)
        return { .base = rvalue(s, p) };

    // an add of a pointer and an integer: the pointer the base, the
    // integer the index or the displacement
    ...
}
```

The subscript is the second case. Every other pointer expression is
the first: its value is computed into a register by `rvalue()`, and
that register is the base. `*p++` falls there. `rvalue()` of `p++`
writes the step as instructions of its own and yields the old value,
and the access goes through that.

Three callers use the address `pointed()` returns: `load()` for a
value, `store()` for an assignment, and `fetch()` followed by
`store()` for a compound assignment or a `++` through the pointer.
None of them knows what shape the address has. This is why the
subscript needed no row in the load and none in the store.

What `*p++` costs today, `register char *p`:

rcc, x86-64, `int c = *p++;`:

```
	movq	%rbx, %rax
	addq	$1, %rbx
	movsbl	(%rax), %eax
```

rcc, aarch64, before this row, the same rules carried
(`docs/aarch64.md` chapter 4: the read is copied before the variable
is written):

```
	mov	x0, x19
	add	x19, x19, #1
	ldrb	w0, [x0]
```

x86-64 has no instruction that does better. aarch64 does:

```
	ldrb	w0, [x19], #1
```

## 4. The rule

One case joins `pointed()`, before the other two:

> When the pointer expression is a `++` or a `--` of a `register`
> pointer, the address is the variable's register as the base, and
> the step is the form's write: the post form for `p++` and `p--`,
> the pre form for `++p` and `--p`.

| Source | Access |
|---|---|
| `*p++` | `[x19], #s` |
| `*p--` | `[x19], #-s` |
| `*++p` | `[x19, #s]!` |
| `*--p` | `[x19, #-s]!` |

`s` is the size of the object pointed to. An access is made only to
a scalar, of 1, 2, 4 or 8 bytes, so `s` is always in the form's
reach.

`address_t` gains the step: which variable, by how much, before or
after. `operand()`, which prints an address, prints the form. A
caller that goes through one address twice, or that generates an
operand between the address and its access, tells the address so;
chapter 6 has both.

The access now writes a variable, so it is preceded by what precedes
every write of a register variable: `preserve()` copies each read of
`p` that an enclosing expression still holds. In `p[0] + *p++` the
left operand's load has consumed its read of `p`, and nothing is
held. In `(long) p + *p++` the left operand is the read itself, held
while the right operand is generated, and it is copied first, `mov
x0, x19`.

## 5. How it composes

Take `*p++ = *q++`, both pointers `register char *`. The generator's
`assign()` does three things in order, and each uses a function that
exists:

1. The left operand's address. `address()` sees a deref and calls
   `pointed()`, which returns "base `x19`, post, +1". No instruction
   is written.
2. The right operand's value. `rvalue()` sees a deref and calls
   `load()` on `pointed()` of `q++`, which returns "base `x20`, post,
   +1". `load()` prints it: `ldrb w0, [x20], #1`.
3. The store through the address of step 1: `strb w0, [x19], #1`.

Two instructions. rcc writes six for it on x86-64 today, a copy, a
step, a copy, a step, the load and the store, and the carried rules
write the same six on aarch64. No function was told about
`*p++ = *q++`; the rule was applied twice.

`**pp++`, with `register char **pp`, composes the other way. The
inner deref is a load through "base `x19`, post, +8": `ldr x0,
[x19], #8`. Its value is the base of the outer deref, the first case
of `pointed()`: `ldrb w0, [x0]`.

What does not match stays as it is. `p++[2]` is a deref over an add
whose pointer operand is the step, so it is the subscript's case,
with the step computed first. `*p++` with `p` a pointer to an array
makes no access, so there is no form to carry the step.

## 6. G13, and why the step's position matters

This is the part the summary compressed too far. The argument has
four steps.

**Step 1: what the form changes is when the step happens.** Today
`p++` is evaluated where the address is evaluated, and its `add` is
written there. With the form, no instruction is written at that
point. The step happens inside the access instruction, wherever that
instruction is written.

**Step 2: for a load, that is the same place.** `load()` evaluates
the address and writes the access at once. Nothing is generated
between them, so nothing can observe the difference.

**Step 3: for a store, it is not.** An assignment evaluates its left
operand's address, then its right operand, then stores. The right
operand lies between the address and the access. G13 says where the
step belongs:

> An assignment operator's left operand is evaluated first, for its
> place: the address, and any side effect in computing it.

The step is such a side effect. It must be complete before the right
operand starts. ISO C leaves the order open, "The evaluations of the
operands are unsequenced" (6.5.17.1p3), and makes a program that
depends on it undefined (6.5.1p2). rcc promises an order, and the
compiler keeps it today. `int i = 1; a[i++] = i;`, both built on
x86-64:

| Compiler | `a[1]` after | Meaning |
|---|---|---|
| rcc | 2 | the right operand saw `i` after the step |
| `clang -O2` | 1 | it saw `i` before |

So a store that takes the post form moves the step past the right
operand. In `*p++ = e` that is invisible exactly when `e` neither
reads nor writes `p`.

**Step 4: the tree says whether it does.** `p` has no address, so
`e` can touch it only by naming it. One walk over `e` looks for the
name, as `writes()` already walks an expression for writes of a
variable (`backend.md` 6.7). The rule, complete:

- A load takes the form always.
- A store takes the form when its right operand does not name the
  variable. `*p++ = c`, `*p++ = *q++` and `*p++ = f(x)` qualify.
- A store whose right operand names it is today's row: the step
  first, as its own instructions, and the store through the copy of
  the old value. `*p++ = *p` and `*p++ = p - base` are such stores.

A compound assignment and a `++` through the pointer, `*p++ += e`
and `(*p++)++`, make two accesses through one address: the load of
the old value and the store of the new one. The step may happen
once. The pre form steps at the first access and the second uses the
plain base; the post form uses the plain base first and steps at the
last. rcc, aarch64:

```
	ldrb	w0, [x19]          // *p++ += 5
	add	w0, w0, #5
	strb	w0, [x19], #1
```

The right operand of a compound assignment lies between the address
and both accesses, so the same condition applies to it.

## 7. Only a register pointer

The form writes a register. A pointer in a slot is not in one: its
step is a load, an `add` and a store, three instructions the form
cannot absorb. rcc today, aarch64, `char *p` an ordinary parameter:

```
	ldr	x0, [x29, #16]
	add	x1, x0, #1
	str	x1, [x29, #16]
	ldrb	w0, [x0]
```

This stays. It is also the model's point: the programmer who wants
the one-instruction walk declares the pointer `register`, and the
cost model says so.

## 8. Lineage

The second pass of the Seventh Edition compiler rewrites exactly
this tree. In `c12.c`, `optim()`, under `case STAR`:

```c
p = subtre->tr1;
if ((subtre->op==INCAFT||subtre->op==DECBEF)&&tree->type!=LONG
 && p->op==NAME && p->class==REG && p->type==subtre->type) {
	p->type = tree->type;
	p->op = subtre->op==INCAFT? AUTOI: AUTOD;
	return(p);
}
```

A `*` over a postfix `++` or a prefix `--` of a name of class `REG`
becomes one operand, `AUTOI` or `AUTOD`, printed `(r3)+` or `-(r3)`
(`c11.c`, `pname()`). Three things match this design:

- the condition is a register variable, `p->class==REG`;
- it is a rewrite of the address, not a row of the load, so the
  code tables use it wherever an address operand stands;
- a row with two accesses steps once. The table's row for a postfix
  `++` through an address is `movB1 A1',R` and `I'B1 A1''`: the
  first access is printed with one quote, which drops the `+` of an
  autoincrement, and the second with two, which drops the `-` of an
  autodecrement. That is chapter 6's last rule.

Two things differ. The PDP-11 had only those two modes, so `*++p`
and `*p--` got nothing; aarch64 has all four. And that compiler
applies the rewrite wherever the tree stands, a store's left operand
included, without looking at the right operand: its language
promised no order. rcc promises one, and chapter 6's condition is
the price.

## 9. Where it lands in the documents and the tests

- `docs/aarch64.md` 2.3: the two forms, as in chapter 2 above.
- Chapter 4: one bullet under the rules of reading and writing a
  register variable.
- 6.3: one row, `*p++`, `*p--`, `*++p`, `*--p`, `*(p += c)` and
  `*(p -= c)` on a register pointer.
- Chapter 15 is the list of calls: the places where the design could
  have gone two ways, each with its answer and what the answer
  costs. This is the ninth: the machine's
  form is taken, and the cost is that the two targets' listings
  part for the same source, and that one store has two rows by what
  its right operand names.
- `docs/cost.md`: a row saying what `*p++` costs on a register
  pointer and on a slot pointer, per target.
- A cost program with the string idioms, a copy, a length and a
  compare, pinned for both targets. Its x86-64 pin shows today's
  listing, since that generator does not change.
- A probe of its own, `walk`: every form as a load and as a store,
  two accesses through one address, and stores whose right operand
  names the pointer. One mutation must turn it red before any other:
  the walk for the name removed.

## 10. The calls

Three were open when this page was written. The answers:

1. **The form is a case of the address**, as chapters 3 to 5 have
   it. The alternative was a row in the load and a row in the store,
   each matching `*p++`: more code, and `*p++ = *q++` would need both
   rows to agree.
2. **`*(p += c)` and `*(p -= c)` take the pre form** when `c` times
   the size is in the form's reach. `++p` is defined as `p += 1`:
   "The expression ++E is equivalent to (E+=1)" (6.5.4.1p2). The rule
   is thus "a step by a constant the form encodes", and `*++p` is its
   case of 1. The reach is the encoding's, -256 to 255, and it is not
   symmetric: on a `register int *`, `*(p -= 64)` is `ldr w0, [x19,
   #-256]!`, and `*(p += 64)`, 256 bytes, is `add x19, x19, #256` and
   the plain load.
3. **The row is its own step of the work**, after `p++` on a register
   pointer in place, which is the row it falls back to.

Decided by the mechanism, listed so that nothing is decided silently:

- the copy of a held read of the pointer is made at the access that
  steps, so for the post form of two accesses it stands before the
  store, not before the load;
- a step of zero, `*(p += 0)`, is the pre form with `#0`, as `p + 0`
  is an `add` of 0;
- `*p++ = 0;` stores the zero register through the form, `strb wzr,
  [x19], #1`, since the zero store goes through the same address.
