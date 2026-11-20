# rcc

An experimental descendant of Ritchie's C compiler, with its own
back end, bringing early C's machine-oriented programming model to
modern targets, with explicit guarantees for selected operations
that ISO C leaves undefined.

The name says where it comes from. The syntax stays near ISO C, so
existing programs compile; the promises are the project. They are
listed in `docs/guarantees.md`: each names the ISO paragraph, the
machine behaviour promised, the instruction that delivers it, and
what the optimizer loses. The choices ISO C leaves to the
implementation, per target, are `docs/choices.md`.

## Design

- A literal compiler. The instruction written is the instruction
  executed: `x / 4` divides, `a + 0` adds, a loop-invariant is
  computed every iteration. Choosing the cheaper form is the
  programmer's job, and the compiler never second-guesses it.
  `docs/cost.md` says what each construct compiles to.
- Own tree, own back end. rcc writes the machine's assembly from the
  tree and owns its registers, in Ritchie's model: every local in its
  stack slot, `register` variables in the callee-saved registers,
  expressions in the rest. The system cc preprocesses, assembles and
  links. The design is `docs/backend.md`, and the reasons are
  `docs/findings.md`. The tree compiled through LLVM's `llc` with
  FastISel and every rewriting pass switched off until c1 replaced
  it; `docs/llvm.md` is the record of that pipeline.
  `docs/ancestor.md` reads the Seventh Edition compiler's code
  generator against c1: what rcc takes from it, and what it leaves.
  `docs/designs.md` reads four organizing designs against c1, the
  Seventh Edition compiler's code tables, destination-driven code
  generation, delayed operand descriptors and selection as a pass of
  its own: where c1's edge cases come from, and what each design would
  remove and what it would cost.
- Each guarantee is delivered by emitting the plain instruction and
  nothing in between.

## Method

- The guarantees list is the conformance record, written before the
  code. The cost model is its companion, written as the pipeline was
  settled.
- The oracle: the same program under `clang -O2` and under `rcc`, on
  a corpus of the known divergence cases. Where they differ is the
  experiment's output.
- The pin: the assembly text per test program, diffed.
- The measurement: each program of a small benchmark corpus, as
  written and as tuned by hand for a literal compiler, timed under
  `clang -O2` and under `rcc`. How far the literal machine lands from
  the optimizer, and how much of the gap the programmer closes, is
  chapter 4 of `docs/cost.md`.

## Status

The first slice: the integer types, the scalar operators, `if`,
`while`, `for`, `return`, calls, through the literal pipeline. The
second slice: pointers and `void *`, arrays as locals and
parameters, subscripts, `&` and `*`, pointer arithmetic and
comparison, casts, `sizeof`, and string and character literals.
Then `register`, honoured: a variable that has no slot of its own, its
register stored to a spill slot only to save a read of it held across
a branch that writes it (`docs/backend.md` 6.7). Then the measurement,
and the change of direction it led to: rcc's own back end, c1, in
place of LLVM's. Its three slices landed: the integer types, control
and calls; pointers, arrays and string literals; register variables.
Then the LLVM pipeline was retired, c1 is the only back end, and the
measurement is redone under it.
c1 also writes aarch64, for the constructs of the same three slices:
the integer types, control and calls; pointers, arrays and string
literals; register variables. Its design is `docs/aarch64.md`, and
its measurement, on two machines, is in chapter 4 of `docs/cost.md`
beside x86-64's. `docs/slot.md` explains what a stack slot costs on
three aarch64 cores and one x86-64 core.
Then a program with its headers: `typedef`, `extern` at file scope,
structs, unions, enums and floating types declared and not yet used,
and the GNU C of a system header read as it is, so that a program
includes `stdio.h` and calls `printf`. Then the rest of the headers:
the declarator in parentheses, the types a header declares for
another compiler and the `static inline` bodies it defines,
`volatile` dropped as `const` is, `_Atomic` and `_Bool` declared,
floating constants taken by the lexer, a typedef of an array of a
size rcc cannot compute, and the scope a parameter list opens, so
that 57 of 58 headers of the C library and of POSIX parse on each
target; the one left is `clang`'s `tgmath.h`. A function is a value
and a call goes through a pointer to one, on both targets, so a
program passes its own comparison to `qsort` and its own handler to
`signal`. Then the objects at file scope: a program defines, declares
and uses its globals, with integer constants as initializers, and
reaches the C library's, so `header-stdout` writes to `stdout`. Their
initializers take address constants and strings, and an array takes
its size from another declaration, so `header-months` keeps a table
of names in an array of `char` and reaches `stdout`, `putchar` and
`fputs` through global pointers. An initializer in braces takes
arrays to any depth, with C's brace elision, so `header-calendar` is
K&R's day of the year with its two tables. Then `static`: a function
and an object of internal linkage, and a block's object of static
storage under a label of its own, so `header-getch` keeps K&R's
buffer of `getch` and `ungetch` private to its file. Then the jumps:
`do`, `break`, `continue`, `goto` and labels; `header-itoa` is K&R's
`itoa`, `reverse` and `trim`. Then `switch`, compiled by the Seventh
Edition compiler's rule: a direct table, a chain of compares or a
hash, chosen from the case values alone; `header-count` is K&R's
count of digits, white space and other characters. An `if` whose
body is one jump is the condition's jump to its target, as that
compiler's first pass wrote it, `jne .Llow.3` for `if (n & 1) break;`.
A cast that widens a signed object in memory is the object's load,
extending: `movslq -4(%rbp), %rax`, `ldrsw x0, [x29, #16]`, as that
compiler's tables converted an operand in the instruction that read
it. On x86-64 a condition whose operator's own instruction sets the
zero flag jumps on it with no `cmp`, `testl $1, -4(%rbp); jne
.Llow.3` for `if (n & 1) break;` and `subl $1, -4(%rbp); je` for
`while (--n)`, as that compiler's `cctab` compiled a condition. On
x86-64 an object in memory as an operator's right operand is the
instruction's own operand, `addl -8(%rbp), %eax`, `idivl -8(%rbp)`,
and a condition on an object in memory and a constant is the `cmp`
or the `test` of the object, `cmpl $5, -4(%rbp)`, as that compiler's
table named an addressable operand in the instruction. On x86-64 a
division and a shift by a variable count choose the registers of
their own operands, and a node whose right operand is one holds its
left operand out of the registers the instruction pins, so `a + b /
c` loads `a` into `ecx` and `b` into `eax` and writes no move, as
that compiler's `oddreg()` chose a division's registers before its
operands. A value assigned to a `register` variable is computed in
its register: a load is `movl -12(%rbp), %ebx` and `ldr w19, [x29,
#24]`, an operation on aarch64 is `add w19, w0, w1`, and on x86-64
`r = a + b` is `movl -12(%rbp), %ebx; addl -16(%rbp), %ebx` when the
right operand is a leaf that does not name the variable, as that
compiler's `move3` and `sreorder()` wrote it.
An element by a
constant index is the object's own operand, `movl table+12(%rip),
%eax`, the constant folded into the address as that compiler
subsumes the constant of `&x+c`. On x86-64 a local array's element
by a variable index is one operand too, the slot the displacement of
the indexed form, `movl -24(%rbp,%rax,4), %eax`, and on both targets
a constant over any lvalue's address joins that address's
displacement, `movsbl 2(%rax,%rcx,8), %eax` for `p[i][2]`.
Then `enum`: an enumeration is the integer type `clang` makes
compatible with it, and each constant its number, so `header-escapes`
names K&R's escapes by a switch on an `enum escapes` and counts the
days of a year over an `enum months`. Then unary `+`: the promotion
of its operand and no instruction of its own, so `+c` on a `char` is
the extending load `movsbl -1(%rbp), %eax` and `+i` on an `int` the
load alone. Then a function declared in a block, and an object
declared `extern` in one: the name is the block's, and the entity the
one of its linkage, so `header-squeeze` calls K&R's `squeeze`,
defined after the block that declares it, and the C library's
`strlen` without its header. Then `?:` with two `void` operands, a
void conditional generated for its effect as one whose value is
dropped, so `header-collatz` takes each step of its walk by
`n % 2 ? triple(&n) : halve(&n)`. Then `_Static_assert`, and
`static_assert` through `assert.h`: a declaration that declares
nothing and writes nothing, and an error with its string where its
constant is 0. Then the encoding prefixes `L`, `u` and `U` on
character constants and string literals, and `u8` on string
literals: the source's UTF-8 decoded into the elements of `wchar_t`,
`char16_t` and `char32_t`, each a `.long` or a `.short`, so
`header-wide` counts the letters of `L"crème brûlée"` and passes it
to `wcslen`. Then structs and unions laid out: a member list read as
declarations, and each record laid out by the rule `clang` has on
both targets, so `sizeof`, `_Alignof` and `offsetof` of one are its
layout's, which the `layout` probe checks against `clang`'s. Then
their objects: a local's slot and a global's data at the record's
size and alignment, so a program takes the address of a struct and
passes it on. Then the members: `p->m` is the dereference of `p`
plus the member's offset, the tree's own nodes, and each generator
reads the offset into the access, `movl 8(%rax), %eax` and `ldr w0,
[x0, #8]`; `header-words` counts the words of a text in K&R's binary
tree of section 6.5, and `header-lookup` keeps the table of names of
section 6.6. Then the initializers of records of static storage: a
list walked against the members, a union's first, with the elision
an array's list has, folded into the object's data, so
`header-keywords` counts the keywords of a text in the table of
section 6.3, and the data of the `struct-data` pin is the bytes
`clang` writes on both targets. Then a record's assignment in the
parser, `a = b` on two objects of one record, which stands where a
value is discarded, which each generator copies inline, unit by
unit of the record's alignment, lowest address first (G18). Then the
variable argument lists in the parser: `va_list` as the target's
convention has it, and `va_arg`, `va_start`, `va_copy` and `va_end`
built of the tree's own nodes, so a function that takes a list reads
it, which the `lists` probe checks against `clang`'s. Then a
definition's `va_start` in both generators: the prologue stores
every argument register, general and vector, into the function's
save area, and the list is built from it (G19), so a list made by
rcc's code is read by `clang`'s and by the C library's `vsnprintf`,
which the `variadics` probe checks; on aarch64 a list handed to a
call is the address of a copy the caller makes.
The shape is `docs/shape.md`; `make` builds `rcc`, `make test` runs
the oracle over `tests/corpus/`, the cost corpus over `tests/cost/`
and the error corpus over `tests/error/`, the programs rcc refuses
with their diagnostics, for each target, then the driver's command
line, `tests/driver`, and then the probes of
`tests/probe/`, rcc's code against `clang`'s on the machine's own
target; `make test-long` is `make test` with the programs of the
oracle's long rows run too, those whose run lasts the time limit,
which `make test` only diffs; `make bench` times the benchmark
corpus of `tests/perf/`,
with a target's own tuned sources from `tests/perf/<target>/`, and
`make slots` times the four loops of `tests/slot/`, the cost of a
stack slot.
The queue is `TODO`.
