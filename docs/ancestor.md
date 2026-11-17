# rcc: the ancestor's code generator

rcc calls itself "an experimental descendant of Ritchie's C compiler".
This page reads the ancestor's code generator against the
descendant's. A survey read the Seventh Edition compiler's second
pass, `c1`, and its code table whole, against rcc's own c1, and asked
what the ancestor still has that the descendant could use. It found
one surprise, six things rcc already took, five mechanisms rcc takes,
three it leaves alone, and a longer list its rules exclude. All five
it takes are in the tree. Each is explained here from the ground up,
with the ancestor's own lines quoted at their file and line in the
compiler's directory, `usr/src/cmd/c/`. It is written as a tutorial,
in the register of `findings.md`, for a reader who knows C and has
seen assembly but has not read a table-driven code generator and does
not know the PDP-11. Chapter 13 says where the sources came from and
under what licence.

## 0. tl;dr

- The survey read the ancestor's c1 whole: its four C files, its
  header, its code table and the program that compiles the table. It
  read the optional peephole pass, `c2`, whole too, and the parser,
  `c0`, in part, and it read rcc's c1 against them.
- The surprise: the ancestor was not a literal compiler. Its c1 wrote
  `x * 8` as a shift and an unsigned `x / 8` as a shift, dropped `x +
  0`, gathered `x + 1 + 2` into `x + 3`, computed the harder operand
  first and moved a postfix `++` past the rest of its expression. Its
  c2 did none of these. So rcc is stricter than its ancestor, by its
  own rules, and "what Ritchie's c1 did, c1 may do" is a necessary
  test and not a sufficient one.
- What rcc already took: the register model, jump code for
  conditions, the three contexts with the memory forms of the effect
  context, `pswitch`, the constant subsumed in an address, and
  autoincrement as aarch64's indexed forms.
- What rcc takes from the ancestor and has in the tree:
  - an operand in memory is the instruction's own, `addl -8(%rbp),
    %eax`, on x86-64 (chapter 4), where a division and a shift also
    choose their registers (4.6);
  - a condition jumps on the flags its operator's instruction set,
    with no `cmp $0`, on x86-64 (chapter 5);
  - a cast that widens a signed object in memory is its load,
    `movslq -4(%rbp), %rax`, on both targets (chapter 6);
  - a `register` variable is the destination of the value computed
    for it, `addl -16(%rbp), %ebx` and `add w19, w0, w1`, on both
    targets (chapter 7);
  - an `if` whose body is one jump is the condition's jump to the
    jump's target, on both targets (chapter 8).
- Left alone: a step for its value as the memory form, `!` over a
  comparison as a value, and x86's short forms for 0 and 1
  (chapter 9).
- Ruled out: the table mechanism itself, and every rewrite and
  reorder of the ancestor that literalness or left-to-right
  evaluation excludes (chapter 10).
- For the slices not written yet, the ancestor's rows for structs,
  bit-fields and floating point say what carries to today's two
  machines and what does not (chapter 11).

## 1. Background: how to read the ancestor

### 1.1 The three passes

The Seventh Edition's C compiler is three programs:

- `c0` reads C, parses it, and writes expression trees and statement
  code to temporary files.
- `c1` reads the trees and writes PDP-11 assembly text. Its header
  calls it "C compiler, part 2" (`c10.c:4`).
- `c2`, the "C object code improver" (`c20.c:3`), reads that text and
  rewrites it instruction by instruction: a *peephole* pass, one that
  looks at a few adjacent instructions at a time. It was optional:
  `cc` ran it on `-O`, which is recalled, since the driver is not
  among the files read.

c0's header says how `cc` ran it (`c00.c:5-9`):

```c
 *
 * Called from cc:
 *   c0 source temp1 temp2 [ profileflag ]
 * temp1 gets most of the intermediate code;
 * strings are put on temp2, which c1 reads after temp1.
```

The shape of the statements is c0's work. c0 writes the labels and
the branches of `if`, `while` and the rest (`c02.c:346-411`), so c1
reads an expression, a label, a branch, or a conditional branch on
an expression (`getree()`, `c11.c:902-918` and `c11.c:1003-1019`).
c1's whole business is the expression.

### 1.2 The PDP-11 in a page

Each fact below is shown by the compiler's own lines where it can
be. What comes from outside these files is marked recalled.

**Registers.** Eight 16-bit registers, `r0` to `r7`. `r6` is the
stack pointer, `sp`, and `r7` the program counter, `pc` (`backend.md`
2.1; the table writes `jsr pc,` for a call). In this compiler `r5` is
the frame pointer: `optim()` turns an automatic variable into an
offset from register 5 (`c12.c:22-26`), written `-10(r5)`. The
`register` variables take `r4`, `r3` and `r2` in declaration order:
c0 starts `regvar` at 5 (`c02.c:110`), and `goodreg()` gives out
`--regvar` while it is at least 3 (`c03.c:691-694`); a fourth
variable is an automatic one (`c03.c:414-416`). The expression
registers are `r0` up to the one below the last variable's
(`c11.c:1021-1022`).

**Two addresses, and memory on either side.** `add src,dst` adds src
into dst, the source first, as in AT&T syntax. Either operand may be
a register, a name in memory or an offset from a register, and a
source may be a constant, `$5`. So `x = y;` is one instruction, `mov
_y,_x`, the `efftab` row `move3` (`table.s:862-865`). A C name takes
a leading underscore in the assembly: C's `regtab` (`c1.h:111`) is
the table's `_regtab` (`table.s:8`). A number is octal unless it ends
in a point (recalled), and the table bears it out: its `106.` is the
operator 106, `LOAD` (`c1.h:245`).

**Condition codes.** Four bits, N, Z, V and C, which nearly every
instruction sets from its result (recalled). A conditional branch
reads them: `jeq`, `jne`, `jlt` and the rest (`branchtab`,
`c13.c:364-385`). `tst x` sets them from `x`, and `mov` sets them
from the value it moves (both recalled). c2 relies on the second: it
removes a `tst` of the place the last `mov` wrote (2.3).

**Autoincrement.** `(r2)+` is the operand at the address in `r2`, and
`r2` steps past it after the access; `-(r2)` steps `r2` back first
(recalled). c1 writes them for `*p++` and `*--p` on a register
pointer (`c12.c:439-445`, printed by `pname()`, `c11.c:106-112`). The
stack is the same form on `sp`: a push is `mov r0,-(sp)` and a pop
`mov (sp)+,r0`.

**No `and`.** `bic src,dst` clears in dst the bits set in src, and
`bis` sets them (recalled). So c1 writes `a & b` as `a &~ ~b`
(`c12.c:86-90`: "replace a&b by a ANDN ~ b"), and tests a mask with
`bit`, an and that only sets the condition codes (`c13.c:334`).

**`div` takes an even-odd register pair.** It divides the 32-bit
number in `R` and `R+1`, `R` even, by its source, and leaves the
quotient in `R` and the remainder in `R+1` (recalled). The compiler
shows each part of it. The row's comment says "/ / R must be odd on
integers" (`table.s:397`), and `oddreg()` makes the register odd
(`c11.c:295-306`). The row loads the dividend into the odd register;
the macro `T` writes `sxt`, which fills a register with the N bit
(recalled), into the one below (`c10.c:725-738`); the instruction,
`I A2,R-`, names the even one (`table.s:402`); and the quotient is
taken from the even register (`c10.c:523-526`). `mul` into an odd
register keeps the low 16 bits of the product (recalled; "/ * -- R
must be odd on integers", `table.s:379`).

### 1.3 The code table

The table is a file of assembler source, `table.s`, 1601 lines long.
It is written in a notation of its own, which a small program,
`cvopt.c`, turns into the data c1 reads.

A *row* is one or more operand patterns, each a line beginning with
`%`, and a *body*: the instructions to write, with macro letters
where the operands and the registers go. Patterns on consecutive
lines share the body below them. The start of the rows for `+`
(`table.s:274-287`):

```
/ +, -, |, &~, <<
cr40:
%n,z
	F

%n,1
	F
	I'	R

%[add1:]
%n,aw
%nf,ad
	F
	IB2	A2,R
```

`/` starts a comment. `cr40:` labels the row list. The entry `40.;
cr40` of `regtab` (`table.s:19`) points the operator 40, `PLUS`
(`c1.h:174`), at it, and the entries of `-`, `<<`, `&~` and `|` point
at the same list (`table.s:20-28`). `%[add1:]` names the body of the
row below it, so that another list reuses the body by that name, as
the multiply's does (`table.s:381-383`). `cvopt.c` turns each pattern
into four bytes, a degree and a type for each operand
(`cvopt.c:249`), followed by a pointer to the body, and c1 reads them
as a `struct optab` (`c1.h:78-84`).

A pattern says what each operand must be: a class letter, then type
letters, then optionally `*`. The class is a bound on the operand's
*degree*, c1's measure of how hard the operand is to reach, which
`dcalc()` computes (`c11.c:157-196`):

| Class | Degree | The operands with a degree at most that |
|---|---|---|
| `z` | 4 | the constant 0 |
| `1` | 5 | the constant 1 |
| `c` | 8 | a positive constant |
| `r` | 9 | a `register` variable |
| `i` | 12 | a name, `&x`, a negative constant, `*` of a name |
| `a` | 16 | the same: an *addressable* operand, one an instruction names in place |
| `e` | 20 | an expression whose registers fit in those left |
| `n` | 63 | anything |

The classes' degrees are `cvopt.c`'s (`cvopt.c:204-244`), and the
operands' are `dcalc()`'s cases (`c11.c:166-195`), with 20 for an
expression whose own degree fits in the registers left and 24 for
one that does not. Of the 664 operands the table's 332 patterns name,
`n` takes 361, `a` 176 and `e` 70; `i` takes none.

`match()` takes the first row of the list that fits
(`c10.c:136-151`):

```c
	for (opt = table->tabp; opt->tabdeg1!=0; opt++) {
		if (d1 > (opt->tabdeg1&077)
		 || (opt->tabdeg1 >= 0100 && (p1->op != STAR)))
			continue;
		if (notcompat(p1, opt->tabtyp1, op)) {
			continue;
		}
		if ((opdope[op]&BINARY)!=0 && p2!=0) {
			if (d2 > (opt->tabdeg2&077)
			 || (opt->tabdeg2 >= 0100) && (p2->op != STAR) )
				continue;
			if (notcompat(p2,opt->tabtyp2, 0))
				continue;
		}
		return(opt);
	}
```

An operand fits a class when its degree is at most the class's;
`&077` takes off the bit that `*` adds. A class followed by `*` has
0100 added (`cvopt.c:246-247`), and the operand must then be an
indirection, a `*p`, or the row is skipped. So the classes nest: the
constant 0 fits `z`, `1`, `c`, `a`, `e` and `n`, and a row list runs
from its special cases to its general one. cr40 takes `x + 0` in its
first row and anything at all in `%n,n`.

`cvopt.c`'s own comment gives other numbers (`cvopt.c:34-39`):

```
	z  -> 4
	c     10
	a     14
	e     20
	n     63
	*	+0100
```

The code is what runs, and in the code `c` is 8 and `a` 16. `cvopt`
prints each degree in octal (`cvopt.c:249`); read as octal, the
comment's 10 and 14 are 8 and 12, the code's `c` and `i`. The comment
may describe an older set of classes; that is an inference.

The type letters (`flag()`, `cvopt.c:309-354`, and `notcompat()`,
`c11.c:198-223`):

- none: a word or a byte, that is a `char`, an `int`, an `unsigned`
  or a pointer;
- `w`: a word, an `int`, an `unsigned` or a pointer;
- `b` a `char`, `i` an `int`, `u` an `unsigned` or a pointer, `l` a
  `long`, `f` a `float` or a `double`, `d` a `double`, `s` a struct;
- `p` after one of them: a pointer to it.

`aw` thus reads "an addressable word", and `nf` "anything floating".

A body is copied to the output, and c1's `cexpr()` expands its macro
letters as it goes (`c10.c:509-797`; `cvopt.c:11-32` lists them):

- `F`: compile the left operand first, into the row's register `R`.
  `S`: the right operand. `H`: the whole tree. A suffix changes where
  the value goes: `*` compiles the address under an indirection, so
  that the row can write `#1(R)`; `S` pushes the value instead, by
  `sptab`; `C` compiles it for the condition codes, by `cctab`; `1`
  puts it in `R1`, the register above `R`. `SS` is thus "the right
  operand, pushed", and `F1*` "the left operand's address, into
  `R1`".
- `I`: the operator's instruction, the first column of `instab`
  (`c13.c:299-353`): `add` for `+`, `mov` for `=`, `cmp` for a
  comparison. `I'`: the second column, the form for a constant: `inc`
  for `+ 1`, `clr` for `= 0`, `tst` for a comparison with 0.
- `A1`, `A2`: the left and the right operand as an instruction's
  operand: `_x`, `-10(r5)`, `$5`, `(r2)+` (`pname()`,
  `c11.c:40-121`). A `'` or `''` after it marks the first or the
  second use of an operand that steps a register, so that `(r2)+`
  steps once; a `+` after it is the second word of a `long`.
- `B1`, `B2`: `b` when that operand is a `char` and `f` when it is
  floating, so that `movB1` is `movb` for a `char`
  (`c10.c:561-583`). `BE` writes `b` when either operand is a `char`,
  `BF` writes `f` when the tree is floating.
- `R`: the row's register. `R-` and `R+`: the registers below and
  above it. `R1`: the second register.
- `#1`, `#2`: the constant offset folded out of the left or the
  right operand's address, `4` in `4(r2)` (`c10.c:699-720`).
- `T`: the high word of a dividend, `sxt` or `clr` of `R-`
  (`c10.c:722-738`). `V`: the carry or the high word of a `long`.
  `Z`: a bit-field's mask. `X0`, `X1`: the branches of a `long`
  comparison.
- Anything else is copied as it stands: `mov`, `jsr pc,`, `(sp)+`, a
  comma.

### 1.4 One row, read slowly

`x + y`, with `x` and `y` external `int` names, through cr40's
`%n,aw`:

1. `match()`. `x` and `y` each have the degree 12, a name's.
   `%n,z` fails, since 12 is above 4, and `%n,1` fails the same way.
   `%n,aw` fits: 12 is at most 63 on the left and at most 16 on the
   right, and an `int` is a word.
2. `F`. c1 compiles `x` into `R`, here `r0`. `x` is a leaf, and the
   rows of `LOAD` write it, cr106's `%a,n`, `movB1 A1,R`
   (`table.s:92-94`): `mov _x,r0`.
3. `I`: the instruction for `+`, `add` (`c13.c:312`).
4. `B2`: nothing, since `y` is neither a `char` nor floating. Under
   the row's second pattern, `%nf,ad`, it writes `f`, and the same
   body is a floating add.
5. A tab, then `A2`, the operand `_y`; the comma is copied; `R` is
   `r0`.

So `x + y` is, worked out by hand from the rows:

```
mov	_x,r0
add	_y,r0
```

The right operand never enters a register: its address is the
instruction's operand. Chapter 4 is about exactly that. When the
right operand is not addressable, the later rows take over. `%n,e`
computes it into the second register first, `F`, `S1` and `IBF
R1,R` (`table.s:297-301`), and `%n,n` computes it before the left one
and pushes it (`table.s:310-315`):

```
%[add5:]
%n,n
%nf,nf
	SS
	F
	IBF	(sp)+,R
```

### 1.5 The four tables

`c1.h` declares the tables (`c1.h:109-113`):

```c
struct	table	cctab[];
struct	table	efftab[];
struct	table	regtab[];
struct	table	sptab[];
struct	table	lsptab[1];
```

Each maps operators to row lists, and each serves one purpose, which
the comments of `table.s` name:

- `regtab`, "compile to register" (`table.s:2`): the value in a
  register.
- `efftab`, "compile for side effects" (`table.s:824`): the effect
  alone, with a promise, "Also set condition codes properly (except
  for ++, --)" (`table.s:825`).
- `cctab`, "set condition codes" (`table.s:1263`): the condition
  codes and no value.
- `sptab`, "expression to -(sp)" (`table.s:1519`): the value pushed,
  for an argument.

`lsptab` is `sptab` under another name: `match()` reads its rows as
`sptab`'s (`c10.c:99-100`), and a row's `-(` writes no `-` for it
(`c10.c:684-689`), so the first argument goes to `(sp)` and saves a
pop (`comarg()`'s comment, `c10.c:1016-1022`). The four lists hold
50, 14, 26 and 7 entries.

`rcexpr()` drives them. It asks the table it was given, and when no
row fits it falls back (`c10.c:162-170`):

```c
 * If the table called for is cctab, sptab, or efftab,
 * and tree can't be done using the called-for table,
 * another try is made.
 * If the tree can't be compiled using cctab, regtab is
 * used and a "tst" instruction is produced.
 * If the tree can't be compiled using sptab,
 * regtab is used and the register is pushed on the stack.
 * If the tree can't be compiled using efftab,
 * just use regtab.
```

rcc keeps `regtab`, `efftab` and `cctab` as its three contexts and
has no counterpart of `sptab` (3.3).

### 1.6 Reading the listings on this page

- **rcc's listings** are what `rcc -S` writes, with `-target x86-64`
  or `-target aarch64`, cut to the construct's own lines. The x86-64
  ones are in AT&T syntax: source first, destination last. Each
  function first stores its parameters to their slots, from
  `-4(%rbp)` down on x86-64 and from `[x29, #16]` up on aarch64, and
  every read of a parameter reads its slot (`backend.md` chapter 5).
- **Earlier listings** show what rcc wrote before a mechanism of
  chapters 4 to 8 was built, and the text says so in the past tense.
  They are real listings: for chapters 4, 5, 6 and 8 from the
  compiler as it stood before all four, and for chapter 7 from the
  compiler just before its own.
- **Listings written by hand** show a form rcc does not write, one
  it leaves alone: chapter 9's, and aarch64's `tst` in 5.4. Each is
  said to be so where it stands.
- **PDP-11 listings** are worked out by hand from the rows, and said
  to be so. None was run.
- **Counts** of instructions and bytes are `llvm-mc`'s encoding of
  each snippet. Chapter 13.4 says how every count was made.
- **Pins** are the listings the tree keeps for its test programs, in
  the cost corpus and the oracle's corpus, and every change is diffed
  against them (`backend.md` chapter 12). A mechanism's reach is the
  pins it moves.

## 2. The surprise: the ancestor was not literal

### 2.1 The rule rcc keeps

`cost.md` puts it first: "The instruction written is the instruction
executed. `x / 4` divides; `x >>= 2` shifts; `a + 0` adds."
(`cost.md` chapter 0). The parser folds an integer constant
expression, the spelling of a number, and nothing else (`backend.md`
6.4). The second rule is G13: "Operands and arguments are evaluated
left to right, and an operand's side effects complete before the next
operand starts" (`guarantees.md`, G13).

The ancestor kept neither rule, and its c1, not its peephole pass,
broke them.

### 2.2 What the ancestor's c1 rewrote

The ancestor's c1 rewrote the tree before it chose rows, and it had
rows that leave work out. Each of the following is c1's.

**A multiply by a power of two is a shift.** `rcexpr()` calls
`pow2()` on every multiply (`c10.c:320-325`):

```c
	/*
	 * Try to change * to shift.
	 */
	case TIMES:
	case ASTIMES:
		tree = pow2(tree);
```

`pow2()` (`c11.c:496-510`) makes a multiply by 2^k a left shift by k.
Worked out by hand from cr40, `x * 8` is `mov _x,r0; ash $3,r0`.

**An unsigned division or remainder by a power of two is a shift or
a mask** (`c12.c:188-191`):

```c
	case MOD:
	case ASMOD:
		if (tree->tr1->type==UNSIGN && ispow2(tree))
			return(pow2(tree));
```

**A division or a multiply-assign by 1 is its left operand**
(`c12.c:182-187`), **and so is a shift by 0** (`c12.c:208-210`).

**`+ 0` goes.** `acommute()`, which flattens a sum into a list of its
operands, drops a constant 0 from the list (`c12.c:634-640`):

```c
	if (op==PLUS || op==OR) {
		/* toss out "+0" */
		if (acl.nextl>0 && (t1 = isconstant(*t2)) && t1->value==0
		 || (*t2)->op==LCON && (*t2)->lvalue==0) {
			acl.nextl--;
			t2--;
		}
```

The table leaves out the rest. cr40's first row, `%n,z`, has the body
`F` alone (`table.s:276-277`), so `x + 0` is the load of `x`.
`efftab`'s `ci70` gives `x += 0` no body at all (`table.s:1098-1104`):

```
/ =+
ci70:
%n*,z
%a,z

%a,1
	I'B1	A1
```

The blank line closes the two patterns' body before anything is in
it, so `x += 0;` and `*p += 0;` write nothing; that is worked out by
hand through `cvopt.c`.

**Constants are gathered out of a sum** (`c12.c:620-626`), so `x + 1
+ 2` is `x + 3`:

```c
	if (!flt) {
		/* put constants together */
		for (i=acl.nextl; i>0; i--) {
			if (t2[0]->op==CON && t2[-1]->op==CON) {
				acl.nextl--;
				t2--;
				const(op, &t2[0]->value, t2[1]->value);
```

**A multiply or an `&` by 0 is the constant** (`c12.c:653-657`):

```c
	} else if (op==TIMES || op==AND) {
		t1 = acl.llist[acl.nextl];
		if (t1->op==CON) {
			if (t1->value==0)
				return(t1);
```

Read from the code and not run: `f() * 0` is 0, and `f` is not
called.

**`x + -1` is `x - 1`** (`c10.c:456-464`).

**The harder operand goes first.** `insert()` sorts a commutative
chain's operands by degree (`c12.c:978-986`), so `a + f(b)` calls `f`
before it reads `a`. A comparison's operands are swapped when the
right one is harder (`c12.c:127-134`). And a row of class `n,n`
computes its right operand before its left one, `SS` before `F`
(`table.s:310-315`, in 1.4).

**A postfix `++` moves past its expression** (`c10.c:916-919`):

```c
/*
 * Delay handles postfix ++ and -- 
 * It observes that "x + y++" is better
 * treated as "x + y; y++".
```

### 2.3 What c2 did

c2 rewrote instructions, not operators. It reads c1's assembly text
line by line and knows each instruction by its mnemonic. Its work:

- a jump to a jump is retargeted, a conditional branch over a `jbr`
  is reversed, code after an unconditional jump is removed, and a
  jump to the next instruction is removed (`c20.c:474-526`);
- a `mov` into a register that already holds the value is removed,
  and `mov $0,x` becomes `clr x` (`c21.c:23-43`);
- `bic $-1,x` becomes `clr x`, and a `bic $0` or a `bis $0` goes
  (`c21.c:78-91`);
- a `tst` of the place the last `mov` wrote is removed
  (`c21.c:126-137`);
- a branch on a compare of two known constants is removed or made
  unconditional (`c21.c:181-203`);
- `dec r; jne L` becomes `sob r,L` (`c21.c:248-267`).

To c2 a `mul` is one more instruction that writes its destination
(`c21.c:73`, among `add` and `sub`). None of 2.2's rewrites is c2's.

c2's condition-code work is that one case (`c21.c:126-137`):

```c
	case TST:
		singop(p);
		repladdr(p, 0, flt);
		source(regs[RT1]);
		if (equstr(regs[RT1], ccloc)) {
			p->back->forw = p->forw;
			p->forw->back = p->back;
			p = p->back;
			nrtst++;
			nchange++;
		}
		continue;
```

`ccloc` is the place whose value the condition codes hold. One line
sets it, after a `mov` (`c21.c:59`), and every other instruction
clears it (`c21.c:97`, `c21.c:120`, `c21.c:178`, `c21.c:205`).
Chapter 5 comes back to it.

### 2.4 Two orders of one statement

`delay()` changes what a program computes. The program:

```c
int
main(void)
{
	int x = 10, y = 1;

	y = x + y++;
	return y;
}
```

ISO C leaves `y = x + y++` undefined. 6.5.1p2 makes a side effect on
a scalar object undefined when it is unsequenced relative to a
different side effect on the same object, and `++` and `=` both
write `y` with nothing to order them. rcc defines it: under G13 the
step completes inside its operand. On aarch64 the step's store, the
fourth line, comes before the add:

```
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	add	w2, w1, #1
	str	w2, [x29, #20]
	add	w0, w0, w1
	str	w0, [x29, #20]
```

The ancestor's `sdelay()` takes the step out of the tree and leaves
the name in its place (`c10.c:960-962`). `delay()` then compiles `y =
x + y` (`c10.c:946-949`), and `rcexpr()` compiles the step after it,
for its effect (`c10.c:333-337`). Worked out by hand from the rows,
with `x` and `y` as external names:

```
mov	_x,r0
add	_y,r0
mov	r0,_y
inc	_y
```

A listing in `delay()`'s order was written by hand for aarch64, from
rcc's own, and both were assembled and run on an aarch64 Linux
machine:

- rcc's listing exits with 11. `y++` leaves `y` at 2 and gives 1, so
  `y` is `10 + 1`.
- `delay()`'s order exits with 12. `y` is `10 + 1`, and then `y++`
  makes it 12.

### 2.5 What it means

Every rewrite of 2.2 is c1's, and rcc's rules exclude every one: the
algebra by literalness, the reorders and `delay()` by G13. So the
lens "what Ritchie's c1 did, c1 may do" is necessary and not
sufficient. rcc is stricter than its ancestor here, and that is the
project's rule. A mechanism of the ancestor is a candidate when it
also passes both rules. Chapters 4 to 9 are those that do; chapter
10 lists those that do not.

## 3. What the descendant already took

Six mechanisms rcc shares with its ancestor besides those of
chapters 4 to 8, each named here with the place it lives.

### 3.1 The register model

The ancestor gave the `register` variables `r4`, `r3` and `r2` in
declaration order and made a fourth one automatic (1.2); every other
local was an offset from `r5`. rcc keeps the model: five
callee-saved registers on x86-64 and ten on aarch64, by declaration
order, every other local in its slot off the frame pointer, and the
expression registers used as a stack (`backend.md` chapter 4,
`aarch64.md` chapter 4, `src/c1-stack.hh`). What rcc does not keep
is `SETREG`: the ancestor gave the registers no variable took to the
expressions (`c11.c:1021-1022`), where rcc's register classes are
"fixed at design time" (`backend.md` chapter 4). On x86-64 an
expression register taken from the callee-saved set would cost its
save and its restore (`backend.md` 3.2).

### 3.2 Jump code

The ancestor's `cbranch()` (`c11.c:544-621`) writes a compare and a
conditional branch for a condition and never a value: `&&`, `||` and
`!` are the shape of the branches, and a comma's left operand is
compiled for its effect. rcc's `cbranch()` is the same function
(`backend.md` 7.1, `cbranch()` in `src/c1-x86-64.cc`). Its last case
is the ancestor's fallback of 1.5, a `cmp $0` where the ancestor
wrote a `tst`; the cases before it on x86-64 are chapter 5's. For
the value of a comparison the ancestor wrote jump code too, `clr` on
one arm and `mov $1` on the other (`c10.c:420-428`); rcc writes
`setcc` and `movzbl`, or `cset`, with no branch (`backend.md` 6.3).

### 3.3 The three contexts and the memory forms

`regtab`, `efftab` and `cctab` are rcc's three entry points,
`rvalue()`, `effect()` and `cbranch()`, in each generator
(`backend.md` 6.6). `sptab` has none, since an argument is a hidden
local of its call, stored to its own slot (`backend.md` chapter 8).
The effect context takes the memory forms (`backend.md` 6.6), as the
ancestor's `efftab` did: its `ci70` writes `x++;` as `inc _x`
(`table.s:1103-1104`), and rcc writes `addl $1, -12(%rbp)` for
`i++;` in `bump()` of chapter 9.3. aarch64 has no memory form, being
a load-store machine (`aarch64.md` chapter 1).

### 3.4 `pswitch`

The ancestor's `pswitch()` (`c11.c:361-449`) chooses a switch's
dispatch from its case values alone: a jump through a table when
`range>0 && range <= 3*ncase` (`c11.c:381`), a chain of compares when
`ncase<10` (`c11.c:399`), and a hash otherwise. rcc takes the rule
whole, widened to its types (`backend.md` 7.3 and call 10;
`direct_switch()`, `simple_switch()` and `hash_switch()` in
`src/c1-statements.hh`).

### 3.5 The constant subsumed in an address

`acommute()` adds the constant of `&x + c` to the name's offset
(`c12.c:647-652`):

```c
		/* subsume constant in "&x+c" */
		if (op==PLUS && t2[0]->op==CON && t2[-1]->op==AMPER) {
			t2--;
			t2[0]->tr1->offset =+ t2[1]->value;
			acl.nextl--;
		}
```

so `*(&x + 4)` is the operand `_x+4`, and no instruction forms the
address. rcc's "name and a constant" is the same rule on a slot or a
label: "`a[3]` is `movl -20(%rbp), L`" (`backend.md` 6.3;
`lvalue_address()` and `folded()` in `src/c1-x86-64.cc`).

### 3.6 Autoincrement

The ancestor wrote `*p++` on a register pointer as `(r2)+`, the
access that steps its base (`c12.c:439-445`). x86-64 has no such
form. aarch64 has two, and rcc takes them: "`*p++` on a `register`
pointer is one instruction, `ldrb w0, [x19], #1`" (`aarch64.md`
chapter 0, and its call 9; `autoincrement.md`).

## 4. An operand where it lives (C1)

### 4.1 The terms

An operand is *addressable* when an instruction can name it in place
with an addressing form, and nothing has to compute it first: a
local's slot `-8(%rbp)`, a global `g(%rip)`, a constant. An x86-64
arithmetic instruction takes one operand in memory: `addl -8(%rbp),
%eax` reads the slot and adds it, in one instruction. aarch64 is a
load-store machine: "only a load reads memory and only a store writes
it" (`aarch64.md` chapter 1).

### 4.2 What the ancestor does

cr40's `%n,aw` (1.4): when the right operand is addressable, its
address is the instruction's operand. The division has the same row
(`table.s:397-402`):

```
/ / R must be odd on integers
cr43:
%n,aw
	F
	T
	I	A2,R-
```

Worked out by hand from it, with the pair of 1.2, `x / d` is:

```
mov	_x,r1
sxt	r0
div	_d,r0
```

A comparison's `%n,aw` reuses cr40's body (`table.s:1328-1330`). A
test of a name against 0 is the name's own operand: `cctab`'s
comparisons start with `%a,z` (`table.s:1299-1303`), which reuses
`efftab`'s first body, `I'B1 A1` (`table.s:851-854`), and `I'` of a
test is `tst`. Worked out by hand, `if (x)` is `tst _x; jeq L`.

The row asks for a word on the right, `aw`. A `char` is not taken in
place: it fails `w`, goes to `%n,e`, and is loaded first.

### 4.3 What rcc wrote before

```c
int
add(int x, int y)
{
	return x + y;
}

int
divm(int x, int d)
{
	return x / d;
}
```

`add()` and `divm()` on x86-64 loaded each operand into a register:

```
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	addl	%ecx, %eax
```

```
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	cltd
	idivl	%ecx
```

A test of a parameter, `if (x)` in `test()` of chapter 5's program,
was a load and then a compare:

```
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	je	.Ltest.2
```

### 4.4 What rcc writes

On x86-64 a right operand that is the read of an object in memory, of
the operation's width, is the instruction's own operand (`backend.md`
6.1). `add()` and `divm()`:

```
	movl	-4(%rbp), %eax
	addl	-8(%rbp), %eax
```

```
	movl	-4(%rbp), %eax
	cltd
	idivl	-8(%rbp)
```

In a condition, where no value is wanted, an object in memory against
a constant is the instruction on the object (`backend.md` 7.1). So
`test()` is:

```
	cmpl	$0, -4(%rbp)
	je	.Ltest.2
```

On aarch64 the loads are the machine's only way to its operands, and
`add()` and `divm()` are as they were:

```
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	add	w0, w0, w1
```

### 4.5 What it buys and what it costs

Per place, one instruction, one register and two bytes:

| Function | Instructions | Bytes |
|---|---|---|
| `add()` | 3 → 2 | 8 → 6 |
| `divm()` | 4 → 3 | 9 → 7 |
| `less()`, `x < y` as a value | 5 → 4 | 14 → 12 |
| `addg()`, `x + g` on a global | 3 → 2 | 11 → 9 |
| `test()`, `if (x)` | 3 → 2 | 8 → 6 |

The count of loads is the same: the load is part of the instruction.
What that saves in time was not measured.

`backend.md` 6.1 names three forms of a right operand: the immediate
of a constant the instruction takes, the operand of an object in
memory, or a register, into which it is generated. c1's own comment
on the right operand names the same three, "the immediate the
instruction takes, the value held, or the address of an object in
memory that the instruction names in place of a register" (`right_t`
in `src/c1-x86-64.cc`). Against the rules:

- G13 holds. The left operand is read first; the right operand's
  address is generated where its load would be, and the instruction
  reads the object with nothing evaluated between (`backend.md` 6.1).
- Literalness holds: the same instruction, in its other operand form,
  and the object is read once, at its width.
- `backend.md` 6.7, the rule for values held across a branch or a
  call, holds. The base and the index of the operand's address are
  owned values from their generation to the instruction, and no new
  kind of value appears.

Where the form does not apply:

- the operand must be of the operation's width, so a `char` or a
  `short` promoted and an `int` widened to 64 bits keep their
  extending load, as the ancestor's `w` keeps a `char` out;
- a shift's count goes to `cl`;
- a compound assignment's right operand keeps its register, since `x
  += e` reads `e` before `x` (G13);
- a global the file only declares is reached through its address
  from the GOT, the global offset table, which is a load of its own;
- in a condition an object is compared in place only against a
  constant, which has no evaluation: `g < f()` loads `g` before the
  call, since `f` may write `g`.

Four rows beside the rule still load their operand: pointer
arithmetic, a cast that changes signedness alone, a comparison with a
constant for its value, and a multiply by a constant. `return x < 5;`
is `movl -4(%rbp), %eax; cmpl $5, %eax` where `if (x > 5)` is `cmpl
$5, -4(%rbp)`, and `x * 4` is the load and `imull $4, %eax, %eax`.
With the constant written first, `if (5 < x)` loads the 5, `movl $5,
%eax; cmpl -4(%rbp), %eax`. The queue holds the four rows and the
constant written first.

The form moved 54 x86-64 pins, 3224 instruction lines to 3072: 78
loads of a right operand, 65 of a left one, three moves of a divisor,
and a spill fewer in two programs. No aarch64 listing moved.

### 4.6 The registers of a division

A division on x86-64 is pinned. `idiv` divides `edx:eax` and leaves
the quotient in `eax` and the remainder in `edx` (`backend.md` 3.1).
c1 gave a value the lowest free register when it generated it, so a
value stood in a pinned register before c1 met the division, and
moves repaired the choice (`backend.md` 6.5). Two statements showed
the cost:

```c
int
fill(int i, unsigned seed)
{
	int a[8];

	a[i] = (seed >> 16) % 256;
	return a[i];
}

int
div_busy(int a, int b, int c)
{
	return a + b / c;
}
```

`a[i] = (seed >> 16) % 256` is the line of `tests/perf/matmul.c`
that fills its matrices. G13 evaluates the place first, so `i` took
`rax`, the first register. The division then wanted `rax` for its
dividend and `rdx` free, where the constant divisor had just been
loaded, and both moved. Twelve instructions:

```
	movl	-4(%rbp), %eax
	movslq	%eax, %rax
	movl	-8(%rbp), %ecx
	shrl	$16, %ecx
	movl	$256, %edx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	%ecx, %eax
	xorl	%edx, %edx
	divl	%esi
	movl	%edx, %ecx
	movl	%ecx, -40(%rbp,%rdi,4)
```

`return a + b / c` is the cost program `div-busy`. `a` took `rax`,
the divisor `c` landed in `rdx`, and both moved again. Eleven
instructions:

```
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	movl	-12(%rbp), %edx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	%ecx, %eax
	cltd
	idivl	%esi
	movl	%eax, %ecx
	addl	%ecx, %edi
	movl	%edi, %eax
```

Two rules choose the registers' names before the instruction is met
(`backend.md` 6.5). Each is a reservation: the registers reserved
are taken last, when no other is free, so a reservation is an
avoidance and not a pin. Neither changes the order of evaluation, nor
any read or write of an object.

**A node chooses for its own operands.** A divisor, and the address
of a divisor in memory, is generated with `rdx` held out as `rax` is;
the left operand of a shift by a variable count with `rcx` held out;
the place of `/=` and `%=` with `rax` and `rdx` held out, and that of
`<<=` and `>>=` with `rcx`. This is the principle of the ancestor's
`oddreg()`, which `cexpr()` calls before a row compiles either
operand (`c10.c:444`), so that the division's node picks its pair by
what `div` needs (`c11.c:295-306`):

```c
		case DIVIDE:
		case MOD:
		case ASDIV:
		case ASMOD:
		case PTOI:
		case ULSH:
		case ASULSH:
			reg++;

		case TIMES:
		case ASTIMES:
			return(reg|1);
```

**A node holds out what its right operand pins.** A node whose right
operand's top operator is a division generates its left operand with
`rax` and `rdx` held out, and an assignment so generates its place; a
shift by a variable count does the same with `rcx`. The top operator
is read through a cast, which pins nothing. This is the first of two
exceptions to 6.1's rule that nothing looks at a node's neighbours,
the second being chapter 7's. The machine pins the registers, and the
node chooses a register's name, not an order. The rule has no
counterpart in the ancestor. The PDP-11's `div` takes any even-odd
pair, and `oddreg()` starts the pair above the registers already held:
worked out from its lines, with `r0` held a division takes `r2` and
`r3`, and nothing held moves. On x86-64 the pair is `rax` and `rdx`
and no other, so the node above the division keeps them free.

`fill()`'s statement is seven instructions:

```
	movslq	-4(%rbp), %rcx
	movl	-8(%rbp), %eax
	shrl	$16, %eax
	movl	$256, %esi
	xorl	%edx, %edx
	divl	%esi
	movl	%edx, -40(%rbp,%rcx,4)
```

The index is in `rcx` and the dividend in `eax` from the start, the
divisor in `esi`, and nothing moves before the `divl`. The index's
load is chapter 6's extending load. The remainder is stored from
`edx`, where the division leaves it, as the ancestor's row of a
remainder returns the odd register of its pair (`backend.md` 6.5).

`div_busy()` is six:

```
	movl	-4(%rbp), %ecx
	movl	-8(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	addl	%eax, %ecx
	movl	%ecx, %eax
```

The divisor is the operand of `idivl -12(%rbp)` by 4.4, `a` is in
`ecx` by the second rule, and nothing moves before the division.

| Statement | Instructions | Bytes |
|---|---|---|
| `a[i] = (seed >> 16) % 256;`, with chapter 6 | 12 → 8 | 35 → 25 |
| `return a + b / c;`, with 4.4 | 11 → 6 | 26 → 14 |

The second rule reads the top operator alone, so a programmer applies
it to the source: "a division or a shift deeper in the right operand
changes nothing, and `a + (b / c + 1)` holds `a` in `rax`"
(`backend.md` 6.5). Where no other register is free, the moves are
written as they were. The two rules moved three x86-64 pins. aarch64
has no pinned instruction (`aarch64.md` 6.5), and no aarch64 listing
moved.

### 4.7 Where it stands

C1 and the division's two rules are in rcc on x86-64. aarch64 has
nothing to take.

## 5. The condition codes an instruction already set (C2)

### 5.1 The terms

x86-64's *flags* hold facts about the last result: ZF, the zero
flag, says whether it was 0. "The flags register describes the result
of the last instruction that sets it" (`backend.md` 3.1). A
conditional jump reads them. So after `andl %ecx, %eax` the flags
already say whether the result is 0, and a `cmpl $0, %eax` before the
jump asks a question the machine has answered. `testl %ecx, %eax`
sets the flags from `eax & ecx` and writes no register (`backend.md`
7.1, which quotes Intel's manual for each instruction's flags).

### 5.2 What the ancestor does

The ancestor's `cctab` does five things.

**1. A comparison with 0 loses its compare.** `rcexpr()` replaces it
by its left operand under `cctab` (`c10.c:198-200`):

```c
	if (opdope[tree->op]&RELAT && tree->tr2->op==CON && tree->tr2->value==0
	 && table==cctab)
		tree = atree = tree->tr1;
```

`cbranch()` then adds 200 to the operator, so that `branchtab`'s
entries for a test against 0 choose the branch (`c11.c:612-614`;
`c13.c:375-384`). Worked out by hand, `if (a + b < 0)` is `mov
_a,r0; add _b,r0; jge L`.

**2. `&` becomes `bit`.** Under `cctab` the and-not of 1.2 turns back
into an and for the flags, `TAND` (`c10.c:256-261`):

```c
	case ANDN:
		if (table==cctab) {
			tree->op = TAND;
			tree->tr2 = optim(tnode(COMPL, tree->type, tree->tr2));
		}
		break;
```

`cc81` writes it with `move3`'s body (`table.s:1454-1457`), under the
comment `/ & as in "if ((a&b) ==0)"`. Worked out by hand, `if (x &
m)` is `bit _m,_x; jeq L`.

**3. `cctab` tries `efftab` first**, so an assignment or a prefix step
under a condition is its memory form, and the jump reads the flags
the form set. The comment (`c10.c:474-484`):

```c
	/*
	 * The following peculiar code depends on the fact that
	 * if you just want the codition codes set, efftab
	 * will generate the right code unless the operator is
	 * a shift or
	 * postfix ++ or --. Unravelled, if the table is
	 * cctab and the operator is not special, try first
	 * for efftab;  if the table isn't, if the operator is,
	 * or the first match fails, try to match
	 * with the table actually asked for.
	 */
```

and the test (`c10.c:490-494`):

```c
	if (table!=cctab || c==INCAFT || c==DECAFT || tree->type==LONG
	 || c==ASRSH || c==ASLSH || c==ASULSH
	 || (opt = match(tree, efftab, r, 0)) == 0)
		if ((opt=match(tree, table, r, 0))==0)
			return(-1);
```

Worked out by hand, `while (--n)` is `dec _n; jeq L`, `L` the loop's
end. A postfix step is left out, as `efftab`'s promise says, "except
for ++, --" (`table.s:825`): its condition is the old value, and the
step sets the flags from the new one.

**4. The arithmetic operators compute into a register with no
`tst`.** `cctab`'s row `rest` compiles the tree itself, `H`, and the
operator's own instruction leaves the flags (`table.s:1512-1516`):

```
/ set codes right
rest:
%n,n
%nf,nf
	H
```

**5. Anything else falls back** to `regtab` and a `tst` of the
register (1.5; `c10.c:366-367`). This is rcc's rule for any condition
no row takes: "its value into `L` by chapter 6, `cmp $0, L`"
(`backend.md` 7.1).

c2's part was the `tst` after a `mov` (2.3). On x86-64 a `mov` sets
no flags (`backend.md` 7.1), so c2's case has no counterpart there.
The condition-code work worth taking is c1's.

### 5.3 What rcc wrote before

```c
int
flags(int x, int m)
{
	if (x & m)
		return 1;
	return 0;
}

int
nonzero(int a, int b)
{
	if (a + b)
		return 1;
	return 0;
}

int
count(int n)
{
	int s = 0;

	while (--n)
		s += n;
	return s;
}

int
added(int x, int e)
{
	if (x += e)
		return 1;
	return 0;
}

int
test(int x)
{
	if (x)
		return 1;
	return 0;
}

int
regflags(int m)
{
	register int x = m;

	if (x & m)
		return 1;
	return 0;
}
```

On x86-64, `flags()` computed the `&`, then compared it with 0:

```
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	andl	%ecx, %eax
	cmpl	$0, %eax
	je	.Lflags.2
```

`count()`'s `--n` was the load, the step and the store, then the
compare:

```
	movl	-4(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -4(%rbp)
	cmpl	$0, %eax
	je	.Lcount.3
```

`added()` the same for `x += e`:

```
	movl	-8(%rbp), %ecx
	movl	-4(%rbp), %eax
	addl	%ecx, %eax
	movl	%eax, -4(%rbp)
	cmpl	$0, %eax
	je	.Ladded.2
```

`regflags()` copied the register variable before the `and`, which
would write it (`backend.md` chapter 4), then compared:

```
	movl	-12(%rbp), %eax
	movl	%ebx, %ecx
	andl	%eax, %ecx
	cmpl	$0, %ecx
	je	.Lregflags.2
```

### 5.4 What rcc writes

On x86-64 the jump reads the flags of the instruction the source
wrote (`backend.md` 7.1). `flags()` is `test`, the ancestor's `bit`:

```
	movl	-4(%rbp), %eax
	testl	-8(%rbp), %eax
	je	.Lflags.2
```

Its right operand is the instruction's own as well, by chapter 4, as
it is in `nonzero()` and `regflags()`. `count()` is the memory form
under the condition, the ancestor's `dec _n; jeq L`:

```
	subl	$1, -4(%rbp)
	je	.Lcount.3
```

`added()`:

```
	movl	-8(%rbp), %eax
	addl	%eax, -4(%rbp)
	je	.Ladded.2
```

and `regflags()`, where `test` writes no register and so needs no
copy of the variable:

```
	testl	-12(%rbp), %ebx
	je	.Lregflags.2
```

`nonzero()`, `if (a + b)`, keeps its `addl` and has no compare. `if
(n & 1)` on a parameter is `testl $1, -4(%rbp)` and the jump, in
chapter 8's listing, which is `cctab`'s `bit $1,_n`.

On aarch64 the test of zero is one instruction already, `cbz`
(`aarch64.md` 2.5, call 3), in `flags()`:

```
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	and	w0, w0, w1
	cbz	w0, .Lflags.2
```

and in `count()`:

```
	ldr	w0, [x29, #16]
	sub	w0, w0, #1
	str	w0, [x29, #16]
	cbz	w0, .Lcount.3
```

Which x86-64 instructions set ZF is Intel's manual's, which
`backend.md` 7.1 quotes in a table, and the rows' limits follow from
it. `add`, `sub`, `and`, `or`, `xor` and `neg` set ZF from their
result; `imul` and `idiv` leave it undefined; `not` and `lea` set no
flag; a shift by a count of 0 leaves the flags as they were. So a
pointer, `*`, `/`, `%`, the shifts, `~` and a cast keep the `cmp`,
and so does a postfix step, whose value is the old one; the ancestor
left the postfix step out as well. A jump of this rule reads ZF
alone, so `<`, `<=`, `>` and `>=` against 0 keep the `cmp` too. c1
records the width at which the last instruction written set ZF, and
any instruction or label after it withdraws the record, so the jump
is bare only when nothing came between (`backend.md` 7.1). With the
constant written first, `if (0 == (c & 1))` keeps the compare that
`if ((c & 1) == 0)` loses; the queue holds it.

aarch64 has the gain already. `and w0, w0, w1; cbz w0` is two
instructions, and `tst w0, w1; b.eq`, written by hand, would be two as
well: 4 instructions and 16 bytes either way, the loads included.

### 5.5 What it buys and what it costs

On x86-64:

| Function | Instructions | Bytes |
|---|---|---|
| `flags()`, `if (x & m)` | 5 → 3 | 13 → 8 |
| `nonzero()`, `if (a + b)` | 5 → 3 | 13 → 8 |
| `count()`, `while (--n)` | 5 → 2 | 14 → 6 |
| `added()`, `if (x += e)` | 6 → 3 | 16 → 8 |
| `regflags()`, `if (x & m)` on a register variable | 5 → 2 | 12 → 5 |

The counts of `flags()`, `nonzero()` and `regflags()` include chapter
4's operand in memory. `count()` and `added()` also hold one register
fewer.

`backend.md` 7.1 has the rows: an operator whose instruction sets the
flags, and a memory form under a condition. The flags are knowledge
of the same kind as the width rule's, "knowledge of what the
generator's own instruction left in the register, not of the
program's data" (`backend.md` 6.3). `flagged()` keeps the record
after each instruction that sets the flags, and `flag_branch()`
writes the bare jump where it holds (`src/c1-x86-64.cc`). G13 and
literalness hold: the instruction is the one the source wrote, and
the jump reads its flags with nothing between. 6.7 holds, since a
condition holds no values. The rows moved one pin, `if-jump` on
x86-64, 203 instruction lines to 198, and no aarch64 listing; the
cost program `condition-flags` pins each row and the rows that keep
the `cmp`.

### 5.6 Where it stands

C2 is in rcc on x86-64. aarch64 has nothing to take: `cbz` and `cbnz`
are its test of zero, and `tst` saves no instruction.

## 6. The conversion made by the load (C3)

### 6.1 The terms

`(long) i`, for an `int i`, widens 32 bits to 64 by copying the sign
bit up: *sign extension*. x86-64 does it in a register, `movslq %eax,
%rax`, and the same mnemonic loads: `movslq -4(%rbp), %rax` reads 32
bits from memory and extends them, in one instruction. aarch64 has
`sxtw x0, w0` in a register and `ldrsw x0, [x29, #16]`, a load that
extends. What the two extending loads do is recalled; `llvm-mc`
assembles both.

### 6.2 What the ancestor does

`match()` tries a row a second time with a conversion taken out of
the tree, to find one whose instruction makes the conversion itself
(`c10.c:111-123`):

```c
		/*
		 * If a subtree starts off with a conversion operator,
		 * try for a match with the conversion eliminated.
		 * E.g. int = double can be done without generating
		 * the converted int in a register by
		 * movf double,fr0; movfi fr0,int .
		 */
		if (opdope[p2->op]&CNVRT && (nocvt&NOCVR)==0
			 && (opdope[p2->tr1->op]&CNVRT)==0) {
			tree->tr2 = p2->tr1;
			if (opt = match(tree, table, nrleft, NOCVL))
				return(opt);
			tree->tr2 = p2;
```

An `int` to floating conversion, for one, has a row whose load
converts (`table.s:723-726`):

```
/ int -> float
cr51:
%aw,n
	movif	A1,R
```

Worked out by hand, `(double) i` for a name `i` is `movif _i,r0`:
the load is the conversion, where an operand that is not addressable
is computed first and then converted in its register, `movif R,R`
(`table.s:732-734`).

rcc does the same for a `char`. The promotion is a cast over the
name, the load extends, and the cast writes nothing: "`(int) c` after
the load of `c` writes nothing" (`backend.md` 6.3).

### 6.3 What rcc wrote before

```c
long
widen(int i)
{
	return i;
}

int
get(int *a, int i)
{
	return a[i];
}
```

On x86-64, `widen()` loaded, then extended:

```
	movl	-4(%rbp), %eax
	movslq	%eax, %rax
```

and `get()` did the same to the index before the access:

```
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %ecx
	movslq	%ecx, %rcx
	movl	(%rax,%rcx,4), %eax
```

On aarch64, `widen()` loaded, then extended:

```
	ldr	w0, [x29, #16]
	sxtw	x0, w0
```

### 6.4 What rcc writes

A cast that widens a signed object in memory is its load
(`backend.md` 6.3, "A widening by the load"). `widen()` on x86-64:

```
	movslq	-4(%rbp), %rax
```

`get()` on x86-64:

```
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	movl	(%rax,%rcx,4), %eax
```

and `widen()` on aarch64:

```
	ldrsw	x0, [x29, #16]
```

`get()` on aarch64 extends inside the addressing form, as it did
(call 5):

```
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	ldr	w0, [x0, w1, sxtw #2]
```

### 6.5 What it buys and what it costs

One instruction per place. On x86-64 that is two bytes, 6 → 4, and
13 → 11 in `get()`; on aarch64 four, 8 → 4.

The row of `(T) a` in `backend.md` 6.3 writes the extending load when
the operand is a signed object in memory: a slot, a global's label or
its address from the GOT, or what a dereference designates.
`extending_load()` and `cast()` in each generator write it. A `signed
char` and a `short` take `movsbq` and `movswq` on x86-64, `ldrsb` and
`ldrsh` into an `x` register on aarch64. An unsigned object, a
register variable and a cast over anything that is no read keep their
rows. G13 and the held values are unchanged: the load reads the bytes
it read before, in one access at the object's width, into the same
register. Literalness holds: the conversion is still an instruction's
work, as it is for a `char`.

A cast over a cast is no read, so the rule does not see the object
under it. `(long) (char) c` on a `char` in memory is `movsbl
-1(%rbp), %eax; movsbq %al, %rax`, where `(long) c` is the one
`movsbq -1(%rbp), %rax`.

The rule moved 18 x86-64 pins in 44 places, 1337 instruction lines to
1293, and 6 aarch64 pins in 8 places, 499 to 491.

### 6.6 Where it stands

C3 is in rcc on both targets.

## 7. A register variable as the destination (C5)

### 7.1 The terms

A `register` variable is nothing but its register: "an assignment to
it writes the register, a read of it is the register as an operand"
(`backend.md` chapter 4). The *destination* is the register an
instruction writes. The question is where the value assigned is
computed: in the variable's own register, so that no `mov` follows,
or in whichever register the register stack gives it, with a `mov`
into the variable after.

### 7.2 What the ancestor does

For a plain load, `efftab`'s `move3` writes the right operand
straight into the left (`table.s:862-865`):

```
%[move3:]
%a,aw
%ab,a
	IBE	A2,A1
```

so `r = y` with `r` in `r4` is `mov _y,r4`. For an operation,
`sreorder()` rewrites the tree, by its comment (`c10.c:836-837`):

```c
 * Moreover, expressions like "reg = x+y" are best done as
 * "reg = x; reg =+ y" (so long as "reg" and "y" are not the same!).
```

It takes an assignment to a name. It refuses at once when the name is
no register variable, when it is a `char`, or when the right side is
floating. The operators it rewrites are `+`, `-`, `&`, `&~`, `|`, `^`
and the two shifts, and `*` only by a power of two, which `pow2()`
first makes a shift (`c10.c:860-875`):

```c
		case ASSIGN:
			if (p1->class != REG||p1->type==CHAR||isfloat(p->tr2))
				return(0);
			if (p->op==ASSIGN) switch (p->tr2->op) {
			case TIMES:
				if (!ispow2(p->tr2))
					break;
				p->tr2 = pow2(p->tr2);
			case PLUS:
			case MINUS:
			case AND:
			case ANDN:
			case OR:
			case EXOR:
			case LSHIFT:
			case RSHIFT:
```

It refuses in two more cases (`c10.c:876-881`):

```c
				p1 = p->tr2->tr2;
				if (xdcalc(p1, 16) > 12
				 || p1->op==NAME
				 &&(p1->nloc==p->tr1->nloc
				  || p1->regno==p->tr1->nloc))
					return(0);
```

- the right operand is not a leaf, a name, a constant or `*` of a
  name, or it is a `char`: `xdcalc()` gives it a degree above 12
  (`c11.c:140-155`);
- the right operand names the variable, as itself or as the base
  register of its address, `*r` or `r->f`.

Otherwise it compiles `reg = x` for its effect, and nothing when `x`
is the variable itself (`c10.c:884-887`), and makes the node `reg =+
y` (`c10.c:888-889`). Worked out by hand, `r = x + y` with `r` in
`r4` is `mov _x,r4; add _y,r4`. The variable holds `x` between the two
instructions, and the two conditions make sure that nothing reads it
there.

### 7.3 What rcc wrote before

```c
int
f(int a, int b)
{
	register int r;

	r = a + b;
	return r;
}
```

On x86-64 the sum was computed in `eax`, its right operand the
instruction's own by chapter 4, and moved:

```
	movl	-12(%rbp), %eax
	addl	-16(%rbp), %eax
	movl	%eax, %ebx
```

and on aarch64 in `w0`, and moved:

```
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	add	w0, w0, w1
	mov	w19, w0
```

A declaration's initializer, `register int x = m;` in `regflags()` of
chapter 5, was loaded and moved, on x86-64:

```
	movl	-12(%rbp), %eax
	movl	%eax, %ebx
```

and on aarch64:

```
	ldr	w0, [x29, #24]
	mov	w19, w0
```

### 7.4 What rcc writes

The variable's register is the destination of the value assigned to
it, where the machine lets the last instruction be the one that
writes it, and a declaration's initializer is the same (`backend.md`
chapter 4, `aarch64.md` chapter 4). A load goes into the register on
both targets, an extending load included. `regflags()`'s initializer
on x86-64:

```
	movl	-12(%rbp), %ebx
```

and on aarch64:

```
	ldr	w19, [x29, #24]
```

On aarch64 the row that computes the value writes the variable by its
last instruction, wherever that instruction is the row's only write
of its destination. An instruction there names its destination apart
from its sources, so the variable is written after every operand is
read, and the rule puts no condition on the operands. `f()`:

```
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	add	w19, w0, w1
```

On x86-64 an instruction of two addresses writes its left operand. So
the operation is the left operand loaded into the variable's register
and the instruction written on it, the right operand where it lives
(chapter 4):

```
	movl	-12(%rbp), %ebx
	addl	-16(%rbp), %ebx
```

The variable holds `a` before the right operand is read. So x86-64
takes the ancestor's two conditions on the right operand. It is a
leaf: a constant, the read of a named object, a register variable or
`*` of a named pointer, read through the parser's conversions, so
that a promoted `char` is one. And it does not name the variable, as
itself or as the base of its address. `operates_in()` in
`src/c1-x86-64.cc` asks both, the second by `names()` in
`src/c1-common.cc`. The operators are `+`, `-`, `&`, `|`, `^`, `*`
and the shifts, on the variable's own type. rcc takes `*` by any
operand, where the ancestor took a power of two alone: `imul` writes
any register, and the PDP-11's `mul` wants an odd one, "for integer
multiply, R must be odd" (`table.s:570`).

The two targets thus part for the same source. Two shapes the
conditions refuse, and a third function for the left operand:

```c
int g(void);

int
nested(int a, int b, int c)
{
	register int r;

	r = a + (b + c);
	return r;
}

int
named(int a, int b)
{
	register int r = b;

	r = a - r;
	return r;
}

int
called(int a)
{
	register int r;

	return r = g() + a;
}
```

x86-64 computes `nested()` and `named()` in `eax` and moves the
result, as it did:

```
	movl	-12(%rbp), %eax
	movl	-16(%rbp), %ecx
	addl	-20(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, %ebx
```

```
	movl	-12(%rbp), %eax
	subl	%ebx, %eax
	movl	%eax, %ebx
```

aarch64 writes the variable in both:

```
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	ldr	w2, [x29, #32]
	add	w1, w1, w2
	add	w19, w0, w1
```

```
	ldr	w0, [x29, #24]
	sub	w19, w0, w19
```

On x86-64 the left operand has a boundary too. It must reach the
variable's register by an instruction it has in any case: its load,
its constant, the operation again, the variable itself, which is
there already, or the copy of another register variable's read
(`loads_into()` in `src/c1-x86-64.cc`). Any other left operand, a
call's result or a division, leaves the whole assignment computed
elsewhere and moved. `called()` on x86-64:

```
	call	g@PLT
	addl	-12(%rbp), %eax
	movl	%eax, %ebx
```

Moved into `ebx` before the `addl`, the call's result would leave the
assignment's value in the variable's register, and the `return` would
copy it back into `eax`: one instruction more (`backend.md` chapter
4). On aarch64 the `add` writes the variable, `add w19, w0, w1`, and
the `return` reads it back, `mov w0, w19`.

Other rows keep the `mov`, by `backend.md` chapter 4 and `aarch64.md`
chapter 4:

- on both targets, a call's result, the value of `&&`, `||` and `?:`,
  which two paths write, another register variable's read, and a
  value converted into a narrow or an `unsigned` variable;
- on x86-64, also a division and a remainder, whose registers are
  pinned (4.6), `-` and `~`, a comparison's value, and an address
  formed by `lea`;
- on aarch64, also an address formed by two instructions, a step's
  value, which its row writes before its store, and a load that steps
  the variable's own register as its base, which cannot also be the
  load's data register.

The queue holds the rows still to decide.

### 7.5 What it buys and what it costs

Per place where the rule applies, one instruction:

| Case | x86-64 | aarch64 |
|---|---|---|
| `r = a + b` | 3 → 2 instructions, 8 → 6 bytes | 4 → 3, 16 → 12 |
| `register int x = m;` | 2 → 1, 5 → 3 | 2 → 1, 8 → 4 |
| `r = a + (b + c)` | unchanged, 5 and 13 | 6 → 5, 24 → 20 |
| `r = a - r` | unchanged, 3 and 7 | 3 → 2, 12 → 8 |

The rule is the second exception to 6.1's rule that nothing looks at a
node's neighbours (`backend.md` chapter 13, call 16): the assignment
hands its register to the row of its right side. G13 holds. On aarch64
the variable is written after every operand is read, and on x86-64 the
two conditions make sure that nothing reads it between its first write
and the instruction. `preserve()` copies the held reads of the
variable before that first write, so `x = r + (r = a + b)` adds the
old `r` (`backend.md` chapter 4). Literalness holds: every operation
the source writes is still its instruction, and the assignment is that
instruction's destination.

The step's count, over the cost corpus, the oracle's corpus and the
benchmark programs: 7 x86-64 listings changed, 943 instruction lines
to 915, and 11 aarch64 listings, 1827 to 1789; no listing of the
oracle's corpus changed. Four cost pins moved on each target,
`condition-flags`, `register-blocks`, `register-compare` and
`register-loop`, and the cost program `register-destination` pins the
rule.

### 7.6 Where it stands

C5 is in rcc on both targets: by the ancestor's rule on x86-64, and
with no condition on the operands on aarch64.

## 8. An `if` whose body is a jump (C6)

### 8.1 The terms

`if (n & 1) break;` has one statement for a body, and the statement
is a jump. Jump code writes a branch past the body for when the
condition is false, and then the body's jump. The same `if` can be
one branch: to the jump's target, for when the condition is true.

### 8.2 What the ancestor does

The ancestor's c0 does it, as it lays out the statement
(`c02.c:369-384`):

```c
			case BREAK:
				o2 = brklab;
				goto simpif;

			case CONTIN:
				o2 = contlab;
			simpif:
				chconbrk(o2);
				cbranch(np, o2, 1);
			hardif:
				if ((o=symbol())!=SEMI)
					goto syntax;
				if ((o1=symbol())==KEYW && cval==ELSE) 
					goto stmt;
				peeksym = o1;
				return;
```

A body that is `break`, `continue`, a `goto` to a label, or a
`return` with no value (`c02.c:349-374`) is one `cbranch()` to its
target, with the sense true (`c02.c:377`). An `else` then follows the
branch directly (`c02.c:381-382`). Worked out by hand, `if (n & 1)
break;` is `bit $1,_n; jne L`, `L` the loop's end.

This is c0's work, the first pass's. In rcc the layout of a statement
is c1's (`src/c1-statements.hh`).

### 8.3 What rcc wrote before

```c
int
low(int n)
{
	while (n) {
		if (n & 1)
			break;
		n = n >> 1;
	}
	return n;
}

void
early(int *p)
{
	if (*p)
		return;
	*p = 1;
}

int
elsebreak(int n)
{
	int s = 0;

	while (n) {
		if (n & 1)
			break;
		else
			s = s + 1;
		n = n >> 1;
	}
	return s;
}
```

On x86-64, `low()`'s `if` branched past the body, and the body
jumped:

```
	movl	-4(%rbp), %eax
	andl	$1, %eax
	cmpl	$0, %eax
	je	.Llow.4
	jmp	.Llow.3
.Llow.4:
```

With an `else`, the `if`'s own jump over the `else` followed the
`break`'s, and no path reached it:

```
	movl	-4(%rbp), %eax
	andl	$1, %eax
	cmpl	$0, %eax
	je	.Lelsebreak.4
	jmp	.Lelsebreak.3
	jmp	.Lelsebreak.5
.Lelsebreak.4:
```

On aarch64:

```
	ldr	w0, [x29, #16]
	and	w0, w0, #0x1
	cbz	w0, .Llow.4
	b	.Llow.3
.Llow.4:
```

### 8.4 What rcc writes

The condition's jump goes to the body's target, taken when the
condition holds (`backend.md` 7.2; `body_target()` and `if_jump()`,
beside `if_stmt()` in `src/c1-statements.hh`). `low()` on x86-64:

```
	testl	$1, -4(%rbp)
	jne	.Llow.3
```

Its `testl $1, -4(%rbp)` is chapter 5's `test` on chapter 4's
operand. This chapter's part is the one `jne` in place of the `je`,
the `jmp` and the label. `elsebreak()` the same, with the `else` body
straight after the jump:

```
	testl	$1, -4(%rbp)
	jne	.Lelsebreak.3
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -8(%rbp)
```

and `low()` on aarch64:

```
	ldr	w0, [x29, #16]
	and	w0, w0, #0x1
	cbnz	w0, .Llow.3
```

The body is the jump bare, or alone in one pair of braces, since the
braces write nothing; the ancestor takes the bare keyword only. Any
other body keeps the general shape: "`if (e) s1 else s2`: `cbranch(e,
.Lelse, 0); s1; jmp .Lend; .Lelse: s2; .Lend:`" (`backend.md` 7.2).
In two pairs of braces, `if (n & 1) { { break; } }`, the jump keeps
it, `je` past the body and then the `jmp`. A `return e` is no jump,
since `e` is code of its own; with an `else` it is followed by a jump
over the `else` part that no path reaches, and the queue holds it.

### 8.5 What it buys and what it costs

Counted on the jumps alone, from the condition's jump to the label
that goes, since the listings above differ in more, by chapters 4 and
5:

| Case | Instructions | Bytes |
|---|---|---|
| x86-64 | 2 → 1 | 4 → 2 |
| x86-64, with an `else` | 3 → 1 | 6 → 2 |
| aarch64 | 2 → 1 | 8 → 4 |

One label fewer each time.

7.2's `if` has the row: a body that is one jump is the condition's
jump to its target, so `if_stmt()` looks at its body. G13,
literalness and 6.7 hold: a statement boundary holds no value, and
the jump that goes is the body's own. The rule moved three listings
on each target, in six places: 437 instruction lines to 431 on
x86-64, and 465 to 459 on aarch64.

### 8.6 Where it stands

C6 is in rcc on both targets.

## 9. The three left alone

### 9.1 A step for its value as the memory form (C4)

The ancestor writes a postfix step for its value on an object in
memory as the read, then the step on the object (`table.s:120-124`):

```
/ ++,-- postfix
cr32:
%a,1
	movB1	A1',R
	I'B1	A1''
```

and a compound assignment for its value as the operation on the
object, then a read of it (`table.s:434-439`):

```
/ =+, =-, =|, =&~
cr70:
%[addq1:]
%aw,aw
	I	A2,A1'
	mov	A1'',R
```

Worked out by hand for names, the first row writes `b++` as `mov
_b,r0; inc _b`, and the second writes `x += y` as `add _y,_x; mov
_x,r0`.

rcc writes `j = i++` as the load, a copy, the step in a register, and
the two stores:

```c
int
post(int i)
{
	int j;

	j = i++;
	return i + j;
}
```

```
	movl	-4(%rbp), %eax
	movl	%eax, %ecx
	addl	$1, %eax
	movl	%eax, -4(%rbp)
	movl	%ecx, -8(%rbp)
```

On aarch64 it is three instructions with no copy, and there is
nothing to take:

```
	ldr	w0, [x29, #16]
	add	w1, w0, #1
	str	w1, [x29, #16]
	str	w0, [x29, #20]
```

The memory form, written by hand, is 3 instructions where there are
5, and 10 bytes where there are 14, and it holds one register fewer:

```
	movl	-4(%rbp), %eax
	addl	$1, -4(%rbp)
	movl	%eax, -8(%rbp)
```

It reads `i` twice: once for the value and once inside the `addl`.
The source writes one read. rcc's c1 "writes every access of every
object" (`shape.md` chapter 5), and c0 drops `volatile`, so every
object is one whose accesses are made as written; a device register
can tell a second read from one. The other side: `i++;` as a
statement is already the memory form, `addl $1, -12(%rbp)` in
`bump()` of 9.3. The survey's pattern for the postfix form finds 3
x86-64 pins in 5 places, and its pattern for the compound form 16
pins in 33 places, an upper bound, since it also matches `x = x +
c`.

rcc leaves C4 alone: the form reads an object twice where the source
reads it once.

### 9.2 `!` over a comparison as a value (C9)

The ancestor turns `!(a < b)` into `a >= b` (`c12.c:463-468`):

```c
	case EXCLA:
		if ((opdope[subtre->op]&RELAT)==0)
			break;
		tree = subtre;
		tree->op = notrel[tree->op-EQUAL];
		break;
```

rcc writes the comparison's value, then the `!` of it:

```c
int
notless(int a, int b)
{
	return !(a < b);
}
```

On x86-64:

```
	movl	-4(%rbp), %eax
	cmpl	-8(%rbp), %eax
	setl	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	sete	%al
	movzbl	%al, %eax
```

On aarch64:

```
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	cmp	w0, w1
	cset	w0, lt
	cmp	w0, #0
	cset	w0, eq
```

Written by hand, the inverse comparison is 3 instructions on x86-64
where there are 6 from the compare on, and 2 on aarch64 where there
are 4:

```
	cmpl	-8(%rbp), %eax
	setge	%al
	movzbl	%al, %eax
```

rcc leaves C9 alone. No program of either corpus has the shape, and
as a condition rcc already turns the sense: "`!e`: `cbranch(e, label,
!sense)`" (`backend.md` 7.1).

### 9.3 x86's short forms for 0 and 1 (C7)

`instab`'s second column (1.3) is the ancestor's form for a constant
0 or 1. Its first lines (`c13.c:299-302`):

```c
struct instab instab[] {
	LOAD,	mov,	tst,
	ASSIGN,	mov,	clr,
	EQUAL,	cmp,	tst,
```

and its steps (`c13.c:312-315`):

```c
	PLUS,	add,	inc,
	ASPLUS,	add,	inc,
	MINUS,	sub,	dec,
	ASMINUS,sub,	dec,
```

`clr` for `= 0`, `tst` for a test of 0, and `inc` and `dec` for a
step of 1. x86-64 has three such forms (recalled): `xorl %eax, %eax`
zeroes a register, `incl` steps by 1, and `testl %eax, %eax` tests
for 0. rcc writes the general forms: `cmpl $0, %eax` for a test of a
register, in `notless()` of 9.2, and for `return 0;` and `i++;`:

```c
int
zero(void)
{
	return 0;
}

void
bump(int *p)
{
	int i = 0;

	i++;
	*p = i;
}
```

```
	movl	$0, %eax
```

```
	movl	$0, -12(%rbp)
	addl	$1, -12(%rbp)
```

Written by hand, the short forms are `xorl %eax, %eax`, `incl
-12(%rbp)` and `testl %eax, %eax`. They save bytes and nothing else:
5 → 2, 4 → 3 and 3 → 2, and no instruction and no register. `xorl`
also writes the flags, which `movl` does not (`backend.md` 7.1), so
it could not stand between an instruction whose flags a condition
reads and its jump (chapter 5). The survey's patterns find `movl $0`
into a register in 28 x86-64 pins (89 places), a step of 1 in 40
(96) and `cmpl $0` of a register in 14 (41), overlapping.

aarch64 took its zero forms on the ancestor's ground, `cbz` for the
test and `wzr` for the store (calls 3 and 4), and there they save an
instruction: "`x = 0;` is one instruction and `x = 1;` two"
(`aarch64.md` chapter 15).

rcc leaves C7 alone: the gain is in bytes only, the cost model
counts instructions, and `xorl` writes the flags that `movl` does
not.

## 10. What the rules exclude

Each mechanism below is the ancestor's c1's, and one of rcc's rules
excludes it.

**Literalness**, "the instruction written is the instruction
executed":

- a multiply by a power of two as a shift (`c10.c:320-325`,
  `c11.c:496-510`), and an unsigned division and remainder by a power
  of two as a shift and a mask (`c12.c:188-191`);
- `acommute()`'s algebra: constants gathered (`c12.c:620-633`), `+ 0`
  dropped (`c12.c:634-640`), `* 0`, `& 0` and `* 1` simplified
  (`c12.c:653-666`), and constants distributed over a sum
  (`c12.c:706-777`, `c12.c:960-977`);
- the rows that write nothing for a 0, cr40's `%n,z` and `ci70`'s
  empty body (2.2);
- `x + -1` as `x - 1` (`c10.c:456-464`), and `x - c` as `x + -c`
  (`c12.c:65-74`);
- a division and a multiply-assign by 1, and a shift by 0, dropped
  (`c12.c:182-187`, `c12.c:208-210`), and `& ~0` dropped
  (`c12.c:167-170`);
- a comparison of a comparison with 0 folded into the inner one
  (`c12.c:247-260`), and `-(-x)` and `~~x` cancelled
  (`c12.c:473-474`, `c12.c:506-507`);
- a comparison of two widened `int`s made on the `int`s
  (`c11.c:594-601`);
- an unsigned comparison with 0 folded to a branch or to nothing,
  `jbr` and `/nop` in `branchtab` (`c13.c:381-384`);
- a bare name as a statement writing nothing (`c10.c:348-349`), where
  rcc's `*p;` is a load (`shape.md` chapter 5);
- `(int) l` read as the low word of `l` in memory (`c12.c:352-366`);
- a `char` compared with a small constant at the byte's width
  (`c12.c:135-138`).

**G13**, left to right:

- every reorder by degree: `insert()`'s sort (`c12.c:978-986`), the
  comparison's swap (`c12.c:127-134`), and the rows that compute the
  right operand first (`table.s:310-315`);
- `delay()`'s postfix step moved past its expression
  (`c10.c:927-951`);
- `sreorder()`'s other rewrite, "x + (reg = y)" done as "reg=y;
  x+reg" (`c10.c:831-832`).

**How arguments reach their registers** (`design-calls.md` chapter
6), and G13: `sptab` and `lsptab`, and `comarg()`'s walk from the
last argument to the first (`c10.c:280-289`).

**The table mechanism itself.** rcc keeps its rows as code: each
generator's functions are its table, read against its page.
`match()`, `cvopt` and the macro interpreter, a matcher over rows held
as data, are not taken. What the rows say, row by row, is this page's
subject.

**The machines.** Two forms have nothing to stand on:

- `*(reg + &x)` as `_x(rN)` (`c12.c:454-460`). On x86-64 a
  PC-relative operand takes no index, and an absolute address does
  not link into a position-independent executable (`backend.md`
  chapter 10); aarch64 has no such form.
- `efftab`'s move from memory to memory, `mov _y,_x`
  (`table.s:862-865`). Neither machine has one: on x86-64 that is
  recalled, and aarch64 is load-store.

## 11. For the constructs not written yet

Structs, unions, enums and floating point are not yet in rcc
(`shape.md` chapter 11), and bit-fields come with structs. The
ancestor's rows for each say what carries to today's two machines
and what does not.

### 11.1 Structs

`strasg()` copies a struct by words (`c11.c:1066-1124`). One word is
copied as an `int` and two as a `long` (`c11.c:1093-1096`):

```c
	if (nwords==1)
		setype(tp, INT);
	else if (nwords==sizeof(int))
		setype(tp, LONG);
```

Up to six words are an unrolled run of `mov (r1)+,(r0)+`
(`c11.c:1109-1112`), and more than six a loop (`c11.c:1114-1121`).
Both addresses go first into `r0` and `r1` (`ci116`,
`table.s:1591-1600`).

What carries:

- a copy as a fixed sequence chosen by the size alone, as `pswitch`
  chooses its form by the cases alone;
- a struct of 1, 2, 4 or 8 bytes moved as that integer;
- pairs of `movq` through a register on x86-64, and `ldp` and `stp`
  pairs on aarch64 (`aarch64.md` 2.2);
- a loop past a bound, and no call to `memcpy`: a call the source
  never wrote would be the compiler's own decision.

What does not carry:

- returning a struct through one static buffer
  (`c11.c:1074-1084`), which is not reentrant. The ABIs return up to
  16 bytes in `rax:rdx` or `x0:x1`, and anything larger through a
  hidden pointer, `rdi` on x86-64 and `x8` on aarch64 (recalled);
- pushing a struct argument by words (`c10.c:1032-1065`). The
  arguments ruling and the ABIs' classification of arguments decide
  that instead;
- on x86-64, `rep movsb`, which would be one more pinned instruction,
  on `rsi`, `rdi` and `rcx` (recalled).

### 11.2 Unions and enums

Both are c0's alone. c0 declares a union's members with `MOU`
(`c03.c:200`, `c03.c:233-241`), and an enumerator is a constant of
type `int` (`c03.c:448-455`). c1 sees offsets and constants, and
`c1.h` has no union and no enum among its types (`c1.h:283-297`).

### 11.3 Bit-fields

c0 packs fields into a word, or a byte for a `char` field, and never
lets one straddle a boundary (`align()`, `c03.c:618-660`); an `int`
field reads as `unsigned` (`c01.c:136-137`). In c1:

- a read is a shift and a mask (`FSEL`, `c12.c:391-397`);
- a write is `bic` of the mask, then `bis` of the value
  (`lvfield()`, `c12.c:554-595`, and the rows `cr16` and `ci16`,
  `table.s:258-272` and `table.s:1228-1260`);
- the assignment of 0 or of all ones is the `bic` or the `bis` alone
  (`FSELA`, `c12.c:147-157`);
- in the effect context the outer select is dropped
  (`c10.c:202-213`).

These carry: the shift and the mask (on aarch64 `ubfx` and `sbfx`,
recalled), the clear and the set (x86-64's memory forms; on aarch64
the load, `bfi` and the store, recalled), the dropped select, and the
two constant cases.

What does not carry is a compound assignment done on the whole word.
The ancestor's comment admits it (`c12.c:550`):

```c
 * Sadly, overflow is undetected on =+ and the like.
```

A carry out of the field runs into the next field. The literal form
is the read, the operation, the truncation to the field, and the
insertion. Unsigned-only fields do not carry either: a field
declared `signed int` must extend its sign when it is read.

### 11.4 Floating point

In the ancestor:

- every operation is in `double`: c0 makes a `float` result
  `double` (`c01.c:225-226`), and a `float` parameter as well
  (`c02.c:655-656`);
- a constant is placed in `.data` under a label at its first use
  (`c10.c:497-500`);
- a conversion is made by the load: `movif` loads an integer as a
  floating value (`table.s:723-726`), and `movof` a `float` as a
  `double` (`table.s:96-97`), both recalled;
- a branch on a floating comparison first writes `cfcc`
  (`c11.c:618-619`), which copies the floating unit's condition
  codes to the processor's (recalled);
- a `double` never goes in a register numbered 4 or above
  (`c10.c:679`).

What carries:

- a constant as labeled data, loaded PC-relative: SSE, x86-64's
  floating-point instructions, takes no immediate, and aarch64's
  `fmov` encodes only a few values (recalled);
- the conversion made by the load, chapter 6's principle;
- a compare straight into the flags. On x86-64 an unordered result
  shows only in PF, the parity flag, so `==` and `!=` test PF as
  well; aarch64's `fcmp` sets its flags, NZCV, directly (both
  recalled).

What does not carry:

- every operation in `double`. The ABIs pass a `float` as a `float`
  (recalled), and literalness wants the operation of the type;
- `cfcc`;
- the register model as it stands. The System V x86-64 convention
  has no callee-saved floating register (recalled), so a `register
  double` there takes a slot and the warning, while aarch64's `d8`
  to `d15` are callee-saved (recalled), room for eight. That is a
  decision for the floating slice.

## 12. The findings together

| Mechanism | Targets | Gain | Pins | Where it stands |
|---|---|---|---|---|
| C1, an operand where it lives | x86-64 | one instruction, one register and two bytes per place | moved 54, 3224 instruction lines to 3072 | in the tree |
| The division's two register rules | x86-64 | 12 → 8 instructions in `matmul`'s statement, with C3; 11 → 6 in `a + b / c`, with C1 | moved 3 | in the tree |
| C2, the flags an instruction already set | x86-64 | two or three instructions per condition of 5.5, C1's operand included, and a register in two of the five | moved 1, 203 instruction lines to 198 | in the tree |
| C3, the conversion made by the load | both | one instruction per place; 2 bytes on x86-64, 4 on aarch64 | moved 18 x86-64 (44 places), 6 aarch64 (8) | in the tree |
| C5, a register variable as the destination | both | one instruction per place; on aarch64 also where x86-64's two conditions refuse | moved 4 per target; 7 x86-64 listings, 943 instruction lines to 915, and 11 aarch64, 1827 to 1789 | in the tree |
| C6, an `if` whose body is a jump | both | one jump and one label per place; two jumps with an `else` on x86-64 | moved 3 per target, 6 places | in the tree |
| C4, a step for its value as the memory form | x86-64 | two instructions, four bytes, one register | 3 (5 places); at most 16 (33) | left alone |
| C9, `!` over a comparison as a value | both | three instructions on x86-64, two on aarch64 | none | left alone |
| C7, the short forms for 0 and 1 | x86-64 | bytes only: 3, 1 and 1 | 28 (89), 40 (96), 14 (41) | left alone |

For a mechanism in the tree, the pins are those it moved. For the
others, they are the places the survey's pattern finds in the pins as
they stand. The labels C1 to C9 are the survey's numbering; its C8 is
`delay()`, which G13 rules out (2.4).

## 13. The record

### 13.1 The sources and their licence

The ancestor is the C compiler of the Seventh Edition of the UNIX
system, from the GitHub mirror `v7unix/v7unix`, directory
`v7/usr/src/cmd/c/`:

- `c00.c` to `c05.c` and `c0.h`: c0, the parser;
- `c10.c` to `c13.c`, `c1.h` and `table.s`: c1, the code generator
  and its table, with `cvopt.c`, which compiles the table's notation;
- `c20.c`, `c21.c` and `c2.h`: c2, the optional peephole pass.

The mirror carries no licence file. Its README names its source, a
Seventh Edition distribution in the archive of The Unix Heritage
Society. The archive's note on copyrights says that Caldera provided
a BSD-style source licence for the Seventh Edition, among other early
systems, in 2002, and points at the licence: Caldera International's
letter of January 23, 2002. The letter grants the rights to use,
modify and distribute the source code of 16-bit UNIX Versions 1 to 7
and of 32V. It requires that redistributions of source code retain
its copyright notice, its list of conditions and its disclaimer. This
page quotes the sources, so it reproduces them, as the letter gives
them:

> Copyright(C) Caldera International Inc. 2001-2002. All rights
> reserved.
>
> Redistribution and use in source and binary forms, with or without
> modification, are permitted provided that the following conditions
> are met:
>
> Redistributions of source code and documentation must retain the
> above copyright notice, this list of conditions and the following
> disclaimer. Redistributions in binary form must reproduce the above
> copyright notice, this list of conditions and the following
> disclaimer in the documentation and/or other materials provided
> with the distribution.
>
> All advertising materials mentioning features or use of this
> software must display the following acknowledgement:
>
> This product includes software developed or owned by Caldera
> International, Inc.
>
> Neither the name of Caldera International, Inc. nor the names of
> other contributors may be used to endorse or promote products
> derived from this software without specific prior written
> permission.
>
> USE OF THE SOFTWARE PROVIDED FOR UNDER THIS LICENSE BY CALDERA
> INTERNATIONAL, INC. AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
> IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
> WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
> ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL, INC. BE
> LIABLE FOR ANY DIRECT, INDIRECT INCIDENTAL, SPECIAL, EXEMPLARY, OR
> CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
> SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
> BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
> LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
> (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
> USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
> DAMAGE.

### 13.2 What was read

The survey read whole `c1.h`, `cvopt.c`, `table.s`, `c10.c`, `c11.c`,
`c12.c`, `c13.c`, `c2.h`, `c20.c`, `c21.c` and `c04.c`. It read
`c01.c`, `c02.c` and `c03.c` in part, at the lines this page cites,
and did not read `c00.c` past its header, `c05.c` or `c0.h`. Of rcc
it read the documents this page cites, and the functions of `src/` it
names. Every line quoted here was read again at its file and line.

### 13.3 What is recalled

These facts come from outside the files, and none was checked here:

- the PDP-11: the four condition codes and which instructions set
  them, `tst` and `mov` among them; octal numbers; autoincrement's
  order; `bic` and `bis`; `div`'s pair and `mul`'s odd register;
  `sxt`; what `movif`, `movof` and `cfcc` do;
- that `cc` ran c2 on `-O`;
- x86-64: what the extending load `movslq` does; the short forms for
  0 and 1; that `mov` has no form from memory to memory; `rep
  movsb`'s registers; PF and the unordered compare; that SSE takes no
  floating-point immediate;
- aarch64: what `ldrsw` and `tst` do; `ubfx`, `sbfx` and `bfi`;
  `fmov`'s immediates; `fcmp` and NZCV;
- the ABIs: the return of a struct, a `float` passed as a `float`,
  and the callee-saved floating registers of each.

Which x86-64 instructions set ZF, `test`, `mov` and `xor` among them,
is not recalled: `backend.md` 7.1 quotes Intel's manual for each.

### 13.4 How the claims are kept

- A line of the ancestor is quoted at its file and line, and a
  citation without a quote names the lines read.
- "rcc writes" is a listing `rcc -S` wrote, at the tree this page
  describes. An earlier listing is the compiler's from before the
  mechanisms of chapters 4, 5, 6 and 8 were built, or, in chapter 7,
  from just before its own.
- A form rcc does not write is a listing written by hand, and said
  to be so: the forms it leaves alone, chapter 9's and aarch64's
  `tst` in 5.4.
- A PDP-11 listing is worked out by hand from the rows; none was run.
- The two orders of 2.4 were assembled and run on an aarch64 Linux
  machine, the one in `delay()`'s order written by hand.
- Instructions and bytes are `llvm-mc`'s encodings of each snippet,
  LLVM 23.1.3, a snippet being lines cut from a listing or written by
  hand. The pins and listings a mechanism in the tree moved are the
  counts of its step. For the others, the survey's script counts the
  pins of the cost corpus and of the oracle's corpus, 125 on each
  target, by the pattern each form changes. The table's figures are
  a script's count of `table.s`.
- The one citation of ISO C is 6.5.1p2, N3220's numbering.
