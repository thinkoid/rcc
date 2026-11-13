# rcc: what a stack slot costs

rcc keeps every local in a stack slot unless the programmer declares
it `register`. The bench of `cost.md` chapter 4, run on two aarch64
machines, gave the plain sources, whose locals all live in slots,
different ratios to `clang -O2` on the two, but the tuned sources
nearly the same. `make slots` measures the slot alone: one loop,
written four ways, each of its two variables in a register or in a
slot. This page explains that measurement from the ground up: the
terms, the loop and its listings on both targets, the numbers on
three aarch64 cores and one x86-64 core, what they mean for
`register`, and what they leave unexplained. It is written as a
tutorial, for a reader who knows C and has seen assembly but has not
used these machines.

## 0. tl;dr

- A local in a slot is read with a load and written with a store. A
  loop that carries it from one iteration to the next stores it in
  one iteration and loads it back in the next: a round trip through
  memory.
- The test is `s += i` a thousand million times, with `i` and `s`
  each a `register` variable or not. On aarch64 the four loops are 7,
  11, 9 and 13 instructions. On x86-64 they are 5, 7, 5 and 7, since
  one x86-64 instruction can load a slot, add to it and store it.
- A slot the loop carries adds, per iteration, 0.37 to 0.65 clock
  cycles on the Firestorm core of an Apple M1 Max, 4.00 to 5.28 on
  its Icestorm core, 6.00 to 6.62 on a Qualcomm Oryon, and 6.01 to
  6.73 on the Zen 2 core of an x86-64 desktop. The loop in registers
  takes 1, 2, 1 and 1 cycles.
- A cycle figure needs the clock a run gets. The Zen 2 core's
  frequency boost hides that clock from the script, so its figures
  are taken with the boost off (4.1, chapter 7).
- The number of loads barely moves that price; the round trip sets
  it. Two slots cost little more than one.
- The worth of `register` thus depends on the core. In this loop, a
  variable left in its slot makes the loop 1.37 to 1.65 times as long
  on Firestorm, but 6.98 to 7.59 times as long on the Oryon and 6.99
  to 7.71 times on Zen 2.
- The bench agrees in part. As plain sources, sort, strings and
  collatz have ratios of 7.82, 9.67 and 5.71 on the Oryon against
  2.54, 4.07 and 3.39 on the M1 Max; as tuned sources the three
  differ between the machines by 0.16 at most. But fib, sieve and
  matmul, whose plain sources keep every variable in a slot too, run
  level as plain sources, and the slot test does not say why.
- The numbers are clock times. The page says what a round trip costs
  on each core, not why.

## 1. The question

### 1.1 Where a local lives

rcc is a literal compiler in Ritchie's model (`cost.md`). Every local
lives in memory, one stack slot each, for the whole call: a read is a
load from the slot and a write a store to it. A local declared
`register` lives in a callee-saved register instead, one of ten on
aarch64, `x19` to `x28`, or one of five on x86-64, `rbx` and `r12` to
`r15` (`cost.md` chapter 0). The compiler never moves a variable
from its slot into a register on its own; the keyword is the
allocation. It is thus the programmer's lever, and the only one.

Leaving the keyword off has a price, and the price is the slot's
cost against the register's. That is the question this page answers.

### 1.2 What the bench showed

`cost.md` chapter 4 measures six programs, each as a plain source and
as a source tuned by hand for a literal compiler, under rcc and under
`clang -O2`. The plain sources declare nothing `register`, so every
local of theirs lives in a slot. The tuned sources declare the hot
variables `register`, among other changes.

On two aarch64 machines, an Apple M1 Max and a Qualcomm Oryon, the
tuned sources under rcc run 1.33 to 2.41 times as long as the plain
ones under `clang -O2` on the first, and 1.33 to 2.29 times on the
second. The plain sources under rcc run 1.61 to 7.42 times as long on
the first, but 1.92 to 9.67 times on the second. Sort, as a plain
source, has 2.54 on one machine and 7.82 on the other.

A tuned source differs from its plain one in more than `register`:
it walks pointers, ends its loops on `!=`, hoists invariants and
shifts where the plain one divides. So the bench cannot say which of
those changes carries the difference between the machines. The slot
test changes one thing only, where two variables live.

## 2. The terms

### 2.1 A slot and a round trip

A *stack slot* is a fixed place in the function's frame, addressed
off the frame pointer: `[x29, #24]` on aarch64, `-12(%rbp)` on
x86-64. A *load* copies a slot into a register, and a *store* copies
a register into a slot. A *round trip* through a slot is a store,
then a load of the same address: the value goes out to memory and
comes back.

### 2.2 A loop-carried value and its chain

A *loop-carried value* is one that each iteration computes from what
the previous iteration left. In

```c
for (i = 0; i != 1000000000; ++i)
    s += i;
```

both variables are: `++i` makes the next `i` from this one, and
`s += i` makes the next `s` from this one.

The *chain* of a loop-carried value is the sequence of instructions
from one iteration's value to the next's. Each instruction of the
chain uses the value the one before it produced. With `i` in a
register the chain is one instruction, the add of 1 to the register.
With `i` in a slot it is a load, the add and a store, and the next
iteration's load reads what this store wrote. Every iteration's chain
thus holds one round trip.

### 2.3 Clock cycles per iteration

The test times each run of a loop in CPU time, user plus system, in
milliseconds. Divided by the thousand million iterations, the time is
nanoseconds per iteration. Multiplied by the core's clock frequency,
it is *clock cycles per iteration*: 330 ms at 3036 MHz is 1.00 cycle
an iteration.

The script reads the frequency from the kernel, in
`/sys/devices/system/cpu/cpu<n>/cpufreq/scaling_cur_freq`, before the
first timed run and after the last. A cycle count is thus only as
true as that frequency: the page takes each core to run at the clock
the kernel reports. On the Zen 2 core that holds only with the
frequency boost off (4.1).

The listings and the clock are all this page uses. No maker's
document was consulted, so the page says nothing about how a core is
built and claims no mechanism behind any number.

## 3. The loop and its four variants

### 3.1 The sources

The four sources are in `tests/slot`. `rr.c` is the baseline:

```c
int
main(void)
{
    register int i;
    register unsigned s = 0;
    for (i = 0; i != 1000000000; ++i)
        s += i;
    return (s >> 8) & 255;
}
```

The other three drop `register` from one variable or both:

| source | `i` | `s` |
|---|---|---|
| `rr.c` | register | register |
| `sr.c` | slot | register |
| `rs.c` | register | slot |
| `ss.c` | slot | slot |

The sum is `unsigned`, since it overflows an `int`. Modulo 2^32 it is
0xb5e49b00. Its low byte is 0, as is that of a sum never added to,
so the exit status is its second byte, 155: a loop that did not run
cannot pass for one that did.

The listings below are `./rcc -S -target aarch64` and `./rcc -S
-target x86-64` of each source, cut to the loop: from its head label
to the branch back.

### 3.2 Reading an aarch64 listing

The destination comes first: `add w19, w19, #1` adds 1 to `w19`. A
`w` register is the low 32 bits of the `x` register of the same
number. `[x29, #24]` is the address 24 bytes above the frame pointer
`x29`; `ldr` loads from it and `str` stores to it.

Each loop has three labels. `.Lmain.2` is the head, where the test
`i != 1000000000` is; `.Lmain.3` is the step `++i`; `.Lmain.4`,
outside the loop, is the code after it. `b.eq` branches when the
compare found the two equal, and `b` always branches.

rcc builds the constant 1000000000 in a register on every iteration,
in two instructions: `mov w0, #51712` writes 0xca00, and `movk w0,
#0x3b9a, lsl #16` writes 0x3b9a into bits 16 to 31 and keeps the
rest. 0x3b9aca00 is 1000000000.

### 3.3 rr: both in registers

```
.Lmain.2:
	mov	w0, #51712
	movk	w0, #0x3b9a, lsl #16
	cmp	w19, w0
	b.eq	.Lmain.4
	add	w20, w20, w19
.Lmain.3:
	add	w19, w19, #1
	b	.Lmain.2
```

Seven instructions: `mov`, `movk`, `cmp` and `b.eq` for the test,
`add w20, w20, w19` for `s += i`, `add w19, w19, #1` for `++i`, and
the `b` back. `i` is in `w19` and `s` in `w20`. No instruction loads
or stores. Each chain is one instruction: `i`'s is the add of 1 to
`w19`, and `s`'s the add of `w19` to `w20`.

### 3.4 sr: the counter in a slot

```
.Lmain.2:
	ldr	w0, [x29, #24]
	mov	w1, #51712
	movk	w1, #0x3b9a, lsl #16
	cmp	w0, w1
	b.eq	.Lmain.4
	ldr	w0, [x29, #24]
	add	w19, w19, w0
.Lmain.3:
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Lmain.2
```

Eleven instructions. `i` is in the slot `[x29, #24]`; `s` stays in
`w19`. The loop loads `i` three times: for the test, for `s += i`,
and for `++i`. It stores `i` once, the `str` after the add of 1.
That store is what the next iteration loads back, three times. `i`'s
chain is the third `ldr`, the `add` and the `str`, and its round trip
runs from the `str` to the next iteration's loads.

### 3.5 rs: the sum in a slot

```
.Lmain.2:
	mov	w0, #51712
	movk	w0, #0x3b9a, lsl #16
	cmp	w19, w0
	b.eq	.Lmain.4
	ldr	w0, [x29, #24]
	add	w0, w0, w19
	str	w0, [x29, #24]
.Lmain.3:
	add	w19, w19, #1
	b	.Lmain.2
```

Nine instructions. `s` is in the slot `[x29, #24]`; `i` stays in
`w19`. The loop loads `s` once and stores it once: `ldr`, the `add`
of `w19`, `str`. That is `s`'s chain, and the next iteration's `ldr`
reads what this `str` wrote.

### 3.6 ss: both in slots

```
.Lmain.2:
	ldr	w0, [x29, #16]
	mov	w1, #51712
	movk	w1, #0x3b9a, lsl #16
	cmp	w0, w1
	b.eq	.Lmain.4
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	add	w1, w1, w0
	str	w1, [x29, #20]
.Lmain.3:
	ldr	w0, [x29, #16]
	add	w0, w0, #1
	str	w0, [x29, #16]
	b	.Lmain.2
```

Thirteen instructions. `i` is in `[x29, #16]` and `s` in `[x29,
#20]`. The loop loads `i` three times, as `sr` does, and `s` once:
four loads. It stores each once: two stores. Each slot has its own
round trip per iteration, so the loop carries two. `s`'s chain reads
`i` from its slot, the second `ldr`, but `i`'s chain never reads `s`.

### 3.7 The x86-64 listings

The listings are in AT&T syntax: the source operand first, the
destination last, `%` before a register and `$` before an immediate.
`-12(%rbp)` is the address 12 bytes below the frame pointer `rbp`.
`.p2align 5` is a directive, not an instruction: it aligns the loop
head to 32 bytes (`cost.md` 4.2). The compare takes 1000000000 as an
immediate, so the test is two instructions, `cmpl` and `je`.

`rr`, both in registers:

```
	.p2align 5
.Lmain.2:
	cmpl	$1000000000, %ebx
	je	.Lmain.4
	addl	%ebx, %r12d
.Lmain.3:
	addl	$1, %ebx
	jmp	.Lmain.2
```

`sr`, the counter in a slot:

```
	.p2align 5
.Lmain.2:
	movl	-12(%rbp), %eax
	cmpl	$1000000000, %eax
	je	.Lmain.4
	movl	-12(%rbp), %eax
	addl	%eax, %ebx
.Lmain.3:
	addl	$1, -12(%rbp)
	jmp	.Lmain.2
```

`rs`, the sum in a slot:

```
	.p2align 5
.Lmain.2:
	cmpl	$1000000000, %ebx
	je	.Lmain.4
	addl	%ebx, -12(%rbp)
.Lmain.3:
	addl	$1, %ebx
	jmp	.Lmain.2
```

`ss`, both in slots:

```
	.p2align 5
.Lmain.2:
	movl	-4(%rbp), %eax
	cmpl	$1000000000, %eax
	je	.Lmain.4
	movl	-4(%rbp), %eax
	addl	%eax, -8(%rbp)
.Lmain.3:
	addl	$1, -4(%rbp)
	jmp	.Lmain.2
```

`rr` is five instructions: the test, `addl %ebx, %r12d` for `s += i`,
`addl $1, %ebx` for `++i`, and the `jmp` back. `i` is in `ebx` and
`s` in `r12d`.

An x86-64 instruction can take a slot as an operand. `addl %ebx,
-12(%rbp)` reads the slot, adds `ebx` and writes the sum back, in
one instruction; the aarch64 loop needs `ldr`, `add` and `str` for
the same. So `rs` is five instructions, as `rr` is, though it reads
and writes `s`'s slot on every iteration. `sr` is seven: two `movl`
loads of `i` for the test and for `s += i`, and `addl $1,
-12(%rbp)` for `++i`. `ss` is seven too, with `addl %eax, -8(%rbp)`
for the sum and `addl $1, -4(%rbp)` for the counter.

The instruction count thus hides the slot on x86-64. The accesses to
the slots are the same as on aarch64, and so are the round trips:
each `addl` to a slot writes what the next iteration reads.

### 3.8 The counts together

| variant | aarch64 instructions | loads / stores | x86-64 instructions | reads / writes of a slot | round trips |
|---|---|---|---|---|---|
| rr | 7 | 0 / 0 | 5 | 0 / 0 | 0 |
| sr | 11 | 3 / 1 | 7 | 3 / 1 | 1 |
| rs | 9 | 1 / 1 | 5 | 1 / 1 | 1 |
| ss | 13 | 4 / 2 | 7 | 4 / 2 | 2 |

The counts are per iteration. A round trip is one store and the
loads of the next iteration that read it.

## 4. The numbers

### 4.1 How they were taken

Four cores, on three machines:

- an Apple M1 Max, with clang 23.1.3. It has two kinds of core: its
  Firestorm cores ran at 3036 MHz and its Icestorm cores at 2064 MHz.
- a Qualcomm Oryon, with clang 23.1.1, at 3418 MHz.
- an x86-64 desktop, Zen 2, with clang 23.1.4, at 3493 MHz with the
  frequency boost off.

The three machines ran with the cpufreq governor `performance`, the
two aarch64 machines on mains. The kernel reported the same frequency
before and after every series on the aarch64 cores, and 3493 MHz
before and after the series of record on the Zen 2 core. The timed
binaries are those `make slots` builds, rcc's builds of the four
sources of chapter 3 for the machine's target.

In its ordinary state the Zen 2 desktop runs with its frequency
boost on. There the times repeat, but the frequency the kernel
reports before the first run is that of the idle core, not the clock
a run gets: at that reading `rr` comes out at 0.84 and 0.78 cycles an
iteration in two series, under one, though each iteration's add to
`i` waits for the one before. The Zen 2 figures are thus taken with
the boost off. With the boost on the times are shorter, but their
ratios to `rr`'s lie within 1% of those with it off.

The four variants ran in fifteen rounds, once each a round, in an
order rotated by the round. Every run was pinned to one core and
timed in user plus system CPU time; the least of the fifteen is kept.
Each series ran twice, and the second's least lay within 0.2% of the
first's in every cell. The tables are the first series.

### 4.2 The table

The least time of each variant, in milliseconds, for 10^9
iterations:

| variant | Firestorm | Icestorm | Oryon | Zen 2 |
|---|---|---|---|---|
| rr | 330 | 970 | 294 | 287 |
| sr | 453 | 3394 | 2072 | 2208 |
| rs | 543 | 2909 | 2051 | 2006 |
| ss | 537 | 3527 | 2232 | 2213 |

The same in clock cycles per iteration, with the cycles above `rr`'s
in parentheses:

| variant | Firestorm | Icestorm | Oryon | Zen 2 |
|---|---|---|---|---|
| rr | 1.00 | 2.00 | 1.00 | 1.00 |
| sr | 1.38 (+0.37) | 7.01 (+5.00) | 7.08 (+6.08) | 7.71 (+6.71) |
| rs | 1.65 (+0.65) | 6.00 (+4.00) | 7.01 (+6.00) | 7.01 (+6.01) |
| ss | 1.63 (+0.63) | 7.28 (+5.28) | 7.63 (+6.62) | 7.73 (+6.73) |

The cycles above `rr`'s are computed from the times before rounding,
so a difference of two rounded cells can miss them by 0.01.

### 4.3 What the table shows

- **The loop in registers.** `rr` takes one cycle per iteration on
  Firestorm, on the Oryon and on Zen 2, and two on Icestorm. An
  iteration of seven instructions thus takes one cycle on two of the
  three aarch64 cores, and the x86-64 iteration of five takes one on
  Zen 2.
- **The round trip's price.** A slot the loop carries adds 6.00 to
  6.62 cycles an iteration on the Oryon, 6.01 to 6.73 on Zen 2, 4.00
  to 5.28 on Icestorm, and 0.37 to 0.65 on Firestorm. On the Oryon
  and on Zen 2 one slot costs at least six times what the whole loop
  in registers does.
- **The count of loads barely matters.** `sr` reads its slot three
  times an iteration and `rs` once, and each carries one round trip.
  `sr` costs 0.07 cycle more than `rs` on the Oryon, 0.71 more on
  Zen 2 and 1.00 more on Icestorm, but 0.27 less on Firestorm. The
  round trip sets the price; the loads beside it move it little.
- **Two slots cost little more than one.** `ss` carries two round
  trips, but costs only 0.55 cycle more than `sr` on the Oryon, 0.27
  more on Icestorm and 0.02 more on Zen 2, and on Firestorm 0.02 less
  than `rs`. The second slot does not double the price.

## 5. What it means

### 5.1 For the bench

The rcc columns of the two aarch64 tables of `cost.md` chapter 4, in
ms, with the ratio to `clang -O2` of the plain source:

| program | M1 Max: `clang -O2` | M1 Max: rcc plain | M1 Max: rcc tuned | Oryon: `clang -O2` | Oryon: rcc plain | Oryon: rcc tuned |
|---|---|---|---|---|---|---|
| collatz | 330 | 1120 (3.39) | 466 (1.41) | 288 | 1644 (5.71) | 453 (1.57) |
| fib | 282 | 454 (1.61) | 679 (2.41) | 244 | 469 (1.92) | 558 (2.29) |
| matmul | 362 | 2685 (7.42) | 740 (2.04) | 362 | 2584 (7.14) | 615 (1.70) |
| sieve | 472 | 2114 (4.48) | 642 (1.36) | 438 | 2065 (4.71) | 582 (1.33) |
| sort | 373 | 949 (2.54) | 495 (1.33) | 337 | 2634 (7.82) | 448 (1.33) |
| strings | 266 | 1083 (4.07) | 535 (2.01) | 250 | 2418 (9.67) | 492 (1.97) |

The bench is `make bench`, unpinned, the least of five runs in user
plus system CPU time, with the tuned matmul and strings of
`tests/perf/aarch64`. The Oryon's `rcc strings.c` cell is one low
timing of its five; the other fourteen timings of the three runs
read 2500 to 2510 ms (`cost.md` chapter 4).

- **Tuned.** 1.33 to 2.41 on the M1 Max, 1.33 to 2.29 on the Oryon.
  No row's tuned ratio differs between the machines by more than
  0.34, matmul's.
- **Plain.** sort, strings and collatz run 2.78, 2.23 and 1.47 times
  as long on the Oryon as on the M1 Max, though `clang -O2` runs all
  three faster on the Oryon.

The slot test finds a cost that splits the two machines the same
way: a carried slot costs the Oryon about six cycles an iteration and
the M1 Max's Firestorm core under one. It does not show that this
cost is what splits the plain rows; chapter 6 says why not.

On x86-64 the table of `cost.md` chapter 4 has the plain ratios of
sort, strings and collatz at 8.00, 7.57 and 7.86, and a carried slot
costs the Zen 2 core about six cycles, as it costs the Oryon. The
listings of the two targets differ, so the match shows no more than
the one between the aarch64 machines.

### 5.2 For `register`

The keyword is the allocation, and its worth is the slot's price on
the core that runs the program. In this loop a variable left without
it makes the loop 1.37 to 1.65 times as long on Firestorm, 3.00 to
3.64 times on Icestorm, 6.98 to 7.59 times on the Oryon, and 6.99 to
7.71 times on Zen 2. The source is the same on all four, and the
listing on the three aarch64 cores; only the time differs.

The cost model of `cost.md` says what a slot compiles to, a load and
a store per access, on every core; it cannot say what the slot costs
in time, since that is the core's. A gain from `register` measured on
one core thus says little of the gain on another. The bench's tuned
sources declare their hot variables `register`, and their ratios
agree on the two machines.

On x86-64 the count of instructions understates the slot: `rs` is
as many instructions as `rr`, five, but takes 7.01 cycles an
iteration on Zen 2 to `rr`'s 1.00.

## 6. What is not explained

- **fib, sieve and matmul.** Their plain sources keep every variable
  in a slot too, yet they run level on the two machines: 454 ms
  against 469, 2114 against 2065, 2685 against 2584, within 4%. The
  slot test does not say why sort, strings and collatz split and
  these three do not. Their inner loops have not been read against
  sort's and strings'.
- **The bench's core.** The bench ran unpinned, so its M1 Max column
  names the machine, not the core that ran it.
- **Firestorm's order.** On Firestorm `sr`, with three loads, costs
  less than `rs`, with one, and `ss` less than `rs`. The table
  records it; nothing here explains it.
- **x86-64.** One x86-64 core has run the test, Zen 2, and its
  cycle figures hold only with the frequency boost off. The clock a
  run gets with the boost on is not read, so the cycles of the
  machine's ordinary state are not known; its times are. No other
  x86-64 core has run the test, so whether the price differs between
  x86-64 cores as it does between aarch64 ones is not known. The
  bench on the same machine, plain 2.25 to 8.06 and tuned 1.62 to
  2.29 (`cost.md` chapter 4), ran with the boost on.
- **Two targets, one price.** The Oryon and Zen 2 run different
  loops: `rs` is nine instructions on the one, with a load and a
  store of the slot, and five on the other, with one `addl` to it.
  Yet a carried slot costs the two about the same, 6.00 to 6.62
  cycles an iteration on the Oryon and 6.01 to 6.73 on Zen 2. The
  table records it; nothing here says whether it is more than
  chance.
- **The frequency.** The cycle figures rest on the frequency the
  kernel reports. A core that ran at another clock would move its
  cycle figures in proportion; the milliseconds stand either way.
- **The mechanism.** The page says what a round trip costs on each
  core, not why it costs six cycles on one and under one on another.
  That would take a maker's document, and none was consulted.

## 7. How to run it

`make slots` runs `tests/slots` from the tree root, building rcc
first if it needs to. `make slots CORE=<n>` pins every run to core
`<n>`; without it the script takes the highest-numbered online core.
On a machine with more than one kind of core, run it once for a core
of each kind.

The script builds the four variants with rcc and `rr.c` with `clang
-O2`, runs each once, and refuses to time unless the five exit
statuses agree. It then times fifteen rounds, the four variants once
a round in an order rotated by the round, each run pinned to the core
by `taskset` and timed in user plus system CPU time, under a limit of
120 seconds. It prints the core and its `scaling_cur_freq` before and
after, and a table with, per variant, the least and the median time
in ms and the nanoseconds per iteration from the least. Where the
core has a cpufreq policy, the table also has the clock cycles per
iteration and the cycles above `rr`'s. The log of every command is
`build/slot/log`. Nothing in the script names an instruction or a
register, so it runs on either target.

Run it on a quiet machine. The script changes no setting, so a fixed
clock is the machine's own: the numbers above were taken with the
governor `performance`, the aarch64 machines on mains and the Zen 2
desktop with its frequency boost off. The cycle column is computed at
the frequency read before the first run; when the reading after the
last run differs, the clock was not fixed, and the column does not
hold.

On a machine with its frequency boost on, the reading before the
first run is the idle core's, not the clock a run gets, so the cycle
column does not hold. Turn the boost off for the run: with the
cpufreq driver `acpi-cpufreq`, write 0 to
`/sys/devices/system/cpu/cpufreq/boost` before it and 1 after. The
times are shorter with the boost on, and their ratios to `rr`'s move
by under 1% on the Zen 2 core.
