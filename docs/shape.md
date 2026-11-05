# rcc: the shape of the compiler

The parts, what each owns, and where they meet. The headers under
`src/` are this document in C++. A change to the shape changes both.

## 0. tl;dr

- Five parts in a line: preprocessor (external), lexer, parser,
  emitter, back end (external). One tree between parser and emitter,
  typed as it is built.
- rcc is a literal compiler. The instruction written is the
  instruction executed. The front end emits exactly what the source
  says, and the back end is LLVM's literal instruction selector with
  every rewriting pass switched off; LLVM's register allocator is the
  one thing it is asked to do well. `cost.md` is the record of what
  each construct compiles to.
- The guarantees in `guarantees.md` are the meaning half: what a
  program denotes where ISO C says nothing. Under a literal back end
  most of them are the machine's own behaviour, reached by writing
  nothing in between.
- Closed sets are `std::variant`; recursion edges are `unique_ptr`
  for a fixed arity and `vector` for a list. Types are values,
  compared structurally.
- The first error is fatal. No recovery.
- GNU make, one executable, a flat `src/`. The test is the oracle
  script over `tests/corpus/`.

## 1. The pipeline

`rcc file.c` runs `clang -E` on the file, tokenizes the result,
parses it into one tree per translation unit, writes the tree as
LLVM IR text, hands the `.ll` file to `llc` for instruction
selection and register allocation, and to `clang` to link. The
front end touches LLVM only through the IR file, and LLVM's
optimizer is never run: no mid-end pass, and no machine-level pass
that rewrites, moves, merges or deletes an instruction. The flag set
that holds it to that is the table in `main.cc` and chapter 2 of
`cost.md`.

The preprocessor is external. ISO defines it completely, nothing in
the guarantees touches it, and V7's `cc` ran `cpp | c0` the same way.
The lexer reads the `# line "file"` markers `clang -E` leaves, so
diagnostics name the source file and line. The column is the
column in the preprocessed text: after a macro use on the line it
drifts from the source, and `clang -E` carries nothing to correct
it.

## 2. The lexer

`tokenize(source, file)` returns the whole file as a vector of
tokens. A token is a kind, its spelling as a view into the source,
and a location. The lexer keeps no state and knows no names: an
identifier is an identifier, and whether it names a type is the
parser's question, answered by scope lookup. A vector gives the
parser arbitrary lookahead for free.

## 3. The types

A type is the machine's view: void, an integer of a width and a
signedness, a pointer to a type, an array of a type and a count, a
function of a result and parameters. The C spelling is not kept:
`long` and `long long` are one type on a target where both are 64
bits. The edges from a type to the types it names are
`shared_ptr< const type_t >`: a type is never written after it is
made, so sharing is invisible and copying a type is cheap. Equality
is structural, through the edges. No interning; a table of types is
an optimization taken when it is measured.

## 4. The tree

Three sums.

- `expr_t`: a type and a node. The nodes are integer literal, name,
  unary, binary, assignment, call, cast, conditional. A name holds a
  non-owning pointer to its symbol. Whether an expression is an
  lvalue follows from its node (the name of an object, a
  dereference), it is not stored. A compound assignment carries the
  type it is computed in, the usual arithmetic conversions of its
  operands.
- `stmt_t`: expression, declaration, block, if, while, for, return.
  A `for` carries its init as a statement, so a declaration in the
  init costs nothing.
- `unit_t`: the translation unit. It owns every symbol in a `deque`,
  so symbol addresses are stable for the life of the unit, and holds
  one `function_decl_t` per function symbol, with a body when the
  unit defines the function and none when it only declares it.

A symbol is a name, a type and a storage: local, parameter, global,
function. Scopes live in the parser and die with it; after parsing
there are only symbols. The tree carries no source locations: the
parser diagnoses with the token in hand, and the emitter diagnoses
nothing.

Every conversion is a cast node the parser inserted. The emitter
converts nothing on its own; when it sees an operand, the operand
already has the type the operation needs. The one exception is
named in the tree: a compound assignment carries the type it is
computed in, and the emitter converts the loaded lhs into it and
the result back, because the lhs is evaluated once and cannot be
wrapped in a cast node twice. The type rules of C live in one
place, the parser's conversion functions.

## 5. The parser

Hand-written recursive descent, one function per production, named
after it, so the code reads against the grammar. The parser types
each expression at construction through a small set of free
functions: integer promotion, the usual arithmetic conversions,
assignment conversion, lvalue conversion, function and array decay.
One pass, one tree. A separate type-check pass would be a second
tree and a second walk for no invariant the compiler needs.

## 6. The emitter

`emit(unit, out)` walks the tree and writes `.ll` text, one IR
instruction per operation in the source, with nothing added and
nothing left out. The rules:

- Every local and parameter is an `alloca` in the entry block, every
  read a load, every write a store. The front end builds no SSA. A
  local lives in its slot for the whole activation (G12), which is
  also what `register` will one day change, for the locals it
  names.
- Two entry points: `rvalue(expr)` yields the value, `lvalue(expr)`
  yields the address. Both return a value handle: the IR register or
  constant, and its IR type.
- Operands are emitted left to right (G13). Short-circuit operators
  branch; the result joins in a `phi`.
- Every load from an alloca is `freeze`d (G5). The literal back end
  lowers a `freeze` to nothing; it is there for the IR, which is a
  document people read.
- Every function is `null_pointer_is_valid` (G9), for the same
  reason.
- `add`, `sub`, `mul`, `shl` carry no `nsw`, `nuw`; `getelementptr`
  carries no `inbounds`; division and shifts are the plain
  instruction, whatever the operands, because what the machine does
  with them is the promise (G1, G3, G4, G6).
- The IR names of symbols are the emitter's, kept in a side table
  from symbol to name; the tree is not written after parsing.

A reader checking a promise opens `emit.cc`, finds the instruction,
and reads what is not there.

## 7. Errors

The first error is fatal: `file:line:column: error: message`, the
form clang and Emacs agree on, exit status 1. The lexer or the
parser throws `error_t` with the location in hand; the driver
catches it and prints. Recovery is the complexity dropped first, and
nothing in the method needs it: the corpus is correct programs.

## 8. The driver

`rcc [-S] [-c] [-o out] file.c`. `-S` stops at the `.ll`, the way
`cc -S` stops at assembly: the IR is rcc's assembly, and the pinned
`.ll` in the corpus is what `-S` wrote. `-c` stops at the object,
which `llc` wrote. The default output is `a.out`, `<stem>.o` or
`<stem>.ll`. There is no `-O`: there is one pipeline, and it is
literal. Processes are spawned by fork and exec, never `system`;
the temporary files are removed on every way out.

Exit status: 0; 1 for a diagnosed error or a failed system call; 2
for a usage error; 3 for an internal error, a tree shape the
emitter does not handle; `llc`'s or `clang`'s own status when one of
them fails.

## 9. The build and the layout

GNU make, one executable. The Makefile is short enough to read
whole; meson would add dependency scanning, a test runner and a
compile database, and three lines of make, a shell script and
`compile_flags.txt` cover the three.

    Makefile          build, test, clean
    compile_flags.txt what clangd needs
    docs/guarantees.md  the meaning: what happens where ISO C says nothing
    docs/cost.md        the cost: what each construct compiles to
    docs/rationale.md   the reasoning behind draft 1 of the guarantees
    src/lex.hh .cc    tokens and tokenize()
    src/type.hh .cc   type_t, constructors, equality
    src/ast.hh        the tree
    src/parse.hh .cc  parse()
    src/emit.hh .cc   emit()
    src/error.hh      error_t
    src/main.cc       the driver, and the back end's flag table
    tests/oracle      the oracle script
    tests/corpus/     the divergence programs, their pinned .ll

## 10. The first slice

The subset probe 3 needs: `int` and the other integer types,
locals with initializers, parameters, the arithmetic, comparison,
logical and assignment operators, `if`, `while`, `for`, `return`,
calls to functions declared or defined in the file. The milestone
program computes `a + 1 > a` at `INT_MAX` and returns it as the
exit status: 0 under rcc, 1 under `clang -O2`. No pointers, no
strings, no `printf`.

The second slice is pointers, which brings G6, G7, G8 and G11 and
their probes, and string literals.

## 11. Not yet

Structs, unions, enums, globals, string literals, `switch`, `goto`,
`sizeof`, type names in casts, typedefs, `static`, `register`,
floating point. Each arrives when a probe needs it. The preprocessor
stays external for good.
