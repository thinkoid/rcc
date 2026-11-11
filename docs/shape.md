# rcc: the shape of the compiler

The parts, what each owns, and where they meet. The headers under
`src/` are this document in C++. A change to the shape changes both.

## 0. tl;dr

- Six parts in a line: preprocessor (external), lexer, parser,
  generator, assembler and linker (external). One tree between parser
  and generator, typed as it is built. The generator is c1, rcc's own
  back end, and `backend.md` is its design; `llvm.md` records the
  LLVM pipeline it replaced.
- rcc is a literal compiler. The instruction written is the
  instruction executed. c1 writes the instruction of `backend.md`'s
  table for each operator of the tree, in the written order, and owns
  the registers by the discipline of `backend.md` chapter 4; nothing
  between the tree and the assembler reasons about the program.
  `cost.md` is the record of what each construct compiles to.
- The guarantees in `guarantees.md` are the meaning half: what a
  program denotes where ISO C says nothing. Under a literal back end
  most of them are the machine's own behaviour, reached by writing
  nothing in between.
- Closed sets are `std::variant`; recursion edges are `unique_ptr`
  for a fixed arity and `vector` for a list. Types are values,
  compared structurally.
- The first error is fatal. No recovery.
- GNU make, one executable, a flat `src/`. The test is two scripts,
  the oracle over `tests/corpus/` and the cost corpus over
  `tests/cost/`, each diffing the assembly `rcc -S` writes against
  its pin.

## 1. The pipeline

`rcc file.c` runs `clang -E` on the file, tokenizes the result,
parses it into one tree per translation unit, prints the warnings,
and generates x86-64 assembly from the tree into a buffer. With `-S`
the buffer is written to the output. Otherwise it goes to a
temporary `.s`, which `clang -c` assembles into the object for `-c`,
and which `clang` assembles and links into an executable without
it. LLVM is what any cc is to a compiler that writes assembly:
`clang -E` preprocesses, `clang -c` assembles, `clang` links. There
is no IR, and nothing optimizes the program on the way.

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
parser arbitrary lookahead for free. A string or character token
keeps its spelling, quotes and escapes included; two functions
beside `tokenize` yield the bytes of a string and the value of a
character constant, and cannot fail, because the lexer refused every
malformed escape when it made the token.

## 3. The types

A type is the machine's view: void, an integer of a width and a
signedness, a pointer to a type, an array of a type and a count, a
function of a result and parameters. The C spelling is not kept:
`long` and `long long` are one type on a target where both are 64
bits. The edges from a type to the types it names are
`shared_ptr< const type_t >`: a type is never written after it is
made, so sharing is invisible and copying a type is cheap. Equality
is structural, through the edges. No interning; a table of types is
an optimization taken when it is measured. The size and the
alignment of a type are the type's, `size_of` and `align_of`, the
x86-64 ABI's numbers; the parser folds `sizeof` from them and c1
sizes and aligns the slots from them.

## 4. The tree

Three sums.

- `expr_t`: a type and a node. The nodes are integer or character
  literal, name, string literal, unary, binary, assignment, call,
  cast, conditional. A name holds a non-owning pointer to its
  symbol. A string literal holds its bytes and is typed `char [N +
  1]`. Whether an expression is an lvalue follows from its node (the
  name of an object, a string literal, a dereference), it is not
  stored. A compound assignment carries the type it is computed in,
  the usual arithmetic conversions of its operands. Pointer
  arithmetic is a binary add or sub of the pointer's type with the
  integer operand widened to 64 bits in its own signedness; the
  difference of two
  pointers is a binary sub of type `long`; a subscript is a
  dereference of an add, as 6.5.3.2p2 defines it; `sizeof` is an
  integer literal the parser folded.
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
parser diagnoses with the token in hand, and the generator
diagnoses nothing.

Every conversion is a cast node the parser inserted. The generator
converts nothing on its own; when it sees an operand, the operand
already has the type the operation needs. The one exception is
named in the tree: a compound assignment carries the type it is
computed in, and the generator converts the loaded lhs into it and
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

## 6. The generator

`generate(unit, file, out)` walks the tree and writes x86-64 assembly
text in AT&T syntax, one instruction, or the machine's fixed
sequence, per operator, with nothing added and nothing left out but
the 32-byte alignment of a loop's head, which is layout, not an
operator's code. `file` is the source's name, for the `.file`
directive. The rules, each the code of a `backend.md` chapter:

- The state is per function, reset at each definition: the slots,
  the held values, the register variables, the frame's size, the
  label numbers. Only the string literals run across the file.
- Every local and parameter not declared `register` has a slot off
  `rbp`, assigned at its declaration, none shared; every read is a
  load and every write a store (chapter 5, G12).
- A held value carries its register and its width: the width to
  which the register's bits are the value extended by its type's
  signedness, set by the instruction that wrote it. A widening cast
  that finds the value already valid to its width writes nothing
  (6.3, the width rule).
- A new value takes the lowest free of the nine caller-saved
  registers. When none is free, the oldest held value is spilled to
  a slot and reloaded when it is consumed (chapter 4, 6.2). A
  division or a variable shift first moves the values out of the
  registers it pins (6.5); a call first spills every held value
  (chapter 8).
- A `register` variable takes the next free of the five callee-saved
  registers, by declaration order, and gives it back at the end of
  its block (chapter 4). Its read is a fixed value: the register
  itself, never spilled, freed or written. An instruction that would
  write its destination works on a copy, and a held read is copied
  before the variable is written, so the read keeps the old value
  (G13). The registers the prologue pushes are counted from the tree
  before the body, since the slots lie below them.
- Three entry points: `rvalue(e)` yields the value in a register,
  `cbranch(e, label, sense)` jumps on its truth and yields no value
  (chapter 7), and `effect(e)` writes its side effects and yields no
  value: a constant assigned to memory is one store, and a compound
  assignment or a step on an object in memory is the memory form,
  the operator on the object, `addl $1, -8(%rbp)` (6.6). `!`, `&&`
  and `||` are the shape of the jumps; any other condition is its
  value compared with 0 at its own width.
- An lvalue is an `address_t`: a slot off `rbp`, a label off `rip`,
  or a held base with a displacement or with a held index scaled by
  1, 2, 4 or 8. It is the operand of the one `mov` that reads or
  writes the object (6.3, G8 to G11).
- Operands are generated left to right, and a commutative operator's
  operands are not swapped (6.1, G13).
- The body is written into a buffer. The prologue is written after
  it, once the frame's size is known, then the body and the one
  epilogue (chapter 5).
- The string literals are one `.data` block after the last function,
  one label per literal written, never merged (chapter 10, G17).

A reader checking a promise opens c1's source, finds the function that
writes the row, and reads what is not there.

## 7. Errors

The first error is fatal: `file:line:column: error: message`, the
form clang and Emacs agree on, exit status 1. The lexer or the
parser throws `error_t` with the location in hand; the driver
catches it and prints. Recovery is the complexity dropped first, and
nothing in the method needs it: the corpus is correct programs.

## 8. The driver

`rcc [-w] [-S|-c] [-o out] file.c`. `-S` stops at the assembly, and
the pins of both corpora are what `-S` wrote. `-c` stops at the
object, which `clang -c` assembled. Without either, `clang` links an
executable. The default output is `<stem>.s`, `<stem>.o` or `a.out`.
There is no `-O`: there is one pipeline, and it is literal, so `-O`
is an unknown option and a usage error. Processes are spawned by fork
and exec, never `system`; the temporary `.s` of `-c` and of a link is
removed on every way out.

A warning does not stop the translation. The parser records it in
the unit, and the driver prints the unit's warnings after a
successful parse, on standard error in the error's format, `warning`
in place of `error`. `-w` silences them, and no warning changes the
exit status. The one warning is a sixth live `register` variable's,
which takes a slot (`backend.md` chapter 4).

Exit status: 0; 1 for a diagnosed error or a failed system call; 2
for a usage error; 3 for an internal error, a construct c1 does not
handle yet; `clang`'s own status when it fails.

## 9. The build and the layout

GNU make, one executable. The Makefile is short enough to read
whole; meson would add dependency scanning, a test runner and a
compile database, and three lines of make, a shell script and
`compile_flags.txt` cover the three.

    Makefile          build, test, bench, clean
    compile_flags.txt what clangd needs
    docs/guarantees.md  the meaning: what happens where ISO C says nothing
    docs/cost.md        the cost: what each construct compiles to
    docs/rationale.md   the reasoning behind draft 1 of the guarantees
    docs/findings.md    what the measurement found, explained from the ground up
    docs/backend.md     the design of rcc's own back end, x86-64 first
    docs/llvm.md        the LLVM sprint: the literal pipeline, where its judgment showed, why it left
    docs/design-calls.md  the back end's open calls, weighed one by one
    src/lex.hh .cc    tokens and tokenize()
    src/type.hh .cc   type_t, constructors, equality
    src/ast.hh        the tree
    src/parse.hh .cc  parse()
    src/c1.hh         generate(), the code generator
    src/c1-x86-64.cc  the generator for x86-64
    src/c1-common.hh .cc
                      the part of c1 that is the same for every target
    src/error.hh      error_t and warning_t
    src/main.cc       the driver
    tests/oracle      the oracle script
    tests/corpus/     the divergence programs, their pinned .s
    tests/costs       the cost corpus script
    tests/cost/       one program per cost-model row, its pinned .s
    tests/bench       the measurement script
    tests/perf/       the benchmark programs, plain and tuned

## 10. The slices

The first slice was the subset probe 3 needed: `int` and the other
integer types, locals with initializers, parameters, the arithmetic,
comparison, logical and assignment operators, `if`, `while`, `for`,
`return`, calls to functions declared or defined in the file. The
milestone program computes `a + 1 > a` at `INT_MAX` and returns it
as the exit status: 0 under rcc, 1 under `clang -O2`.

The second slice was pointers and string literals, which brought
G6, G7, G8, G11 and G17 and their probes: pointer declarators and `void
*`, `&` and `*`, arrays of constant size as locals and as parameters
adjusted to pointers, subscripts, decay, casts with a type name, the
null pointer constant, a pointer as a condition, `++`, `--`, `+=`
and `-=` on pointers, `p + n`, `p - q` and pointer comparison,
`sizeof`, string and character literals with their escapes and
adjacent concatenation, and `const` accepted and dropped, since the
type is the machine's view. The oracle stayed on exit statuses: a
string probe returns a byte of the string.

Both slices first compiled through the LLVM pipeline (`llvm.md`).
c1 was written in the same two slices and a third, `register`
variables in the five callee-saved registers (`backend.md` chapter
4). When the three had landed and every program of the corpus and
of the cost corpus built through c1 and matched its pin, the LLVM
pipeline was retired.

## 11. Not yet

Structs, unions, enums, global variables, brace initializers and
`char s[] = "..."`, function pointers, wide and `u8` strings,
`volatile` (c1 writes every access it meets, an unused one
included, so the access needs nothing more; the qualifier waits for
the type model, as `const`'s checks do),
`restrict`, `switch`, `goto`, typedefs, `static`, a `register`
array (6.3.2.1p3), floating point. Each arrives when a probe needs
it. The preprocessor stays external for good.
