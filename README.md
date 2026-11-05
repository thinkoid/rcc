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

- Own tree. LLVM is a dependency, not a fork.
- Textual IR out. `clang` assembles and links. No build-time
  dependency beyond an installed LLVM.
- Each guarantee is a flag or metadata the front end withholds: no
  `nsw`, no `inbounds`, no `!tbaa`, and so on. The full list is the
  last table of the guarantees document.

## Method

- The guarantees list is the conformance record, written before the
  code.
- The oracle: the same program under `clang -O2` and under `rcc`, on
  a corpus of the known divergence cases. Where they differ is the
  experiment's output.
- The pin: the IR text per test program, diffed.

## Status

The first slice: `int`, the scalar operators, `if`, `while`, `for`,
`return`, calls. The shape is `docs/shape.md`; `make` builds `rcc`,
`make test` runs the oracle over `tests/corpus/`. The queue is
`TODO`.
