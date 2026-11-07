# rcc

An experimental descendant of Ritchie's C compiler, using LLVM to
bring early C's machine-oriented programming model to modern targets,
with explicit guarantees for selected operations that ISO C leaves
undefined.

The name says where it comes from. The syntax stays near ISO C, so
existing programs compile; the promises are the project. They are
listed in `docs/guarantees.md`: each names the ISO paragraph, the
machine behaviour promised, the IR that delivers it, and what the
optimizer loses.

## Design

- A literal compiler. The instruction written is the instruction
  executed: `x / 4` divides, `a + 0` adds, a loop-invariant is
  computed every iteration. Choosing the cheaper form is the
  programmer's job, and the compiler never second-guesses it.
  `docs/cost.md` says what each construct compiles to.
- Own tree. LLVM is a dependency, not a fork.
- Textual IR out. `llc` selects instructions one for one and
  allocates registers, with every rewriting pass switched off;
  `clang` links. No build-time dependency beyond an installed LLVM.
- Each guarantee is delivered by emitting the plain instruction and
  nothing in between. The front end withholds every flag an
  optimizer would reason from: no `nsw`, no `inbounds`, no `!tbaa`.
  The full list is the last table of the guarantees document.

## Method

- The guarantees list is the conformance record, written before the
  code. The cost model is its companion, written as the pipeline was
  settled.
- The oracle: the same program under `clang -O2` and under `rcc`, on
  a corpus of the known divergence cases. Where they differ is the
  experiment's output.
- The pin: the IR text per test program, diffed.
- The measurement: each program of a small benchmark corpus, as
  written and as tuned by hand for a literal compiler, timed under
  `clang -O2` and under `rcc`. How far the literal machine lands from
  the optimizer, and how much of the gap the programmer closes, is
  chapter 6 of `docs/cost.md`.

## Status

The first slice: the integer types, the scalar operators, `if`,
`while`, `for`, `return`, calls, through the literal pipeline. The
second slice: pointers and `void *`, arrays as locals and
parameters, subscripts, `&` and `*`, pointer arithmetic and
comparison, casts, `sizeof`, and string and character literals.
Then `register`, honoured: a variable that is never stored. The shape is
`docs/shape.md`; `make` builds `rcc`, `make test` runs the oracle
over `tests/corpus/` and the cost corpus over `tests/cost/`, and
`make bench` times the benchmark corpus of `tests/perf/`. The queue
is `TODO`.
