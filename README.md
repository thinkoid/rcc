# rcc

An experimental descendant of Ritchie's C compiler, with its own
back end, bringing early C's machine-oriented programming model to
modern targets, with explicit guarantees for selected operations
that ISO C leaves undefined.

The name says where it comes from. The syntax stays near ISO C, so
existing programs compile; the promises are the project. They are
listed in `docs/guarantees.md`: each names the ISO paragraph, the
machine behaviour promised, the instruction that delivers it, and
what the optimizer loses.

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
Then `register`, honoured: a variable that is never stored. Then the
measurement, and the change of direction it led to: rcc's own back
end, c1, in place of LLVM's. Its three slices landed: the integer
types, control and calls; pointers, arrays and string literals;
register variables. Then the LLVM pipeline was retired, c1 is the
only back end, and the measurement is redone under it.
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
`fputs` through global pointers.
The shape is `docs/shape.md`; `make` builds `rcc`, `make test` runs
the oracle over `tests/corpus/`, the cost corpus over `tests/cost/`
and the error corpus over `tests/error/`, the programs rcc refuses
with their diagnostics, for each target, and then the probes of
`tests/probe/`, rcc's code against `clang`'s on the machine's own
target; `make bench` times the benchmark corpus of `tests/perf/`,
with a target's own tuned sources from `tests/perf/<target>/`, and
`make slots` times the four loops of `tests/slot/`, the cost of a
stack slot.
The queue is `TODO`.
