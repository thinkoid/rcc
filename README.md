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
c1 also writes aarch64 for the first two slices' constructs, the
integer types, control and calls, then pointers, arrays and string
literals; register variables remain. Its design is
`docs/aarch64.md`.
The shape is `docs/shape.md`; `make` builds `rcc`, `make test` runs
the oracle over `tests/corpus/` and the cost corpus over
`tests/cost/` for each target, both through c1, and then the probes
of `tests/probe/`, rcc's code against `clang`'s on the machine's own
target; `make bench` times the benchmark corpus of `tests/perf/`.
The queue is `TODO`.
