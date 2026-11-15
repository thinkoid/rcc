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
- GNU make, one executable, a flat `src/`. The test is four
  scripts: the oracle over `tests/corpus/` and the cost corpus over
  `tests/cost/`, each diffing the assembly `rcc -S` writes against
  its pin; the error corpus over `tests/error/`, diffing what
  `rcc -S` prints on standard error and its exit status against
  theirs; and the probes of `tests/probe/`, which run rcc's code
  against `clang`'s.

## 1. The pipeline

`rcc file.c` runs `clang -E` on the file, tokenizes the result,
parses it into one tree per translation unit, prints the warnings,
and generates the target's assembly from the tree into a buffer.
With `-S` the buffer is written to the output. Otherwise it goes to
a temporary `.s`, which `clang -c` assembles into the object for `-c`,
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

The keywords are C17's, those outside the subset included, so the
parser refuses each by its name. GNU C's spellings in the
implementation's name space, which a system header writes, are
keywords too: `__const`, `__restrict__`, `__signed__`, `__inline` and
their like are the keyword they spell, and `__attribute__`, `__asm__`,
`__extension__`, `__int128` and `__float128` are tokens of their own.

## 3. The types

A type is the machine's view: void, an integer of a width and a
signedness, a pointer to a type, an array of a type and a count, a
function of a result and parameters, or a type rcc declares and does
not use yet: a struct, a union, an enum; a floating type, real or
complex, GNU C's `__float128` among them; GNU C's `__int128`;
`__builtin_va_list` and clang's SVE types; a type a typedef names
through `volatile`, `_Atomic` or an attribute rcc does not honour.
Such a type carries an identity, which equality compares, and the
words a refusal of its use prints; it has no size and no alignment,
and a pointer to it is an ordinary pointer. The C spelling is not
kept:
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
  unit defines the function and none when it only declares it. A
  definition carries the most register variables it has live at
  once, its parameters among them.

A symbol is a name, a type and a storage: local, parameter, global,
function, type name, enumerator. A global is an object declared
`extern` at file scope and defined elsewhere; nothing is written for
it, and the parser refuses its use, so no expression names one yet. A
type name is a typedef's: its type is the type the name stands for,
it has no object, and nothing is written for it. An enumerator is an
enumeration constant: its type is its enum's, its value is not
computed, and the parser refuses its use, so no expression names one
either. A function or a global may carry two facts more: the name
`__asm__` gives it in the assembly, which c1 writes wherever it writes
the symbol, and a marking, the words its use is refused with when an
attribute changes what the program depends on. A local or parameter the
parser gives a register carries its index, which of the target's
register variables it takes: each
takes the next in declaration order and gives it back at its block's
end (`backend.md` chapter 4), so the index is the count of those live
where it is declared. The parser decides this, and the generator
reads the decision from the tree. Scopes live in the parser and die
with it; each holds its ordinary identifiers and, apart, its tags of
structs, unions and enums. After parsing there are only symbols. The
tree carries no source locations: the parser diagnoses with the token
in hand, and the generator diagnoses nothing.

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

A system header is written in GNU C, and the parser reads it as it
is. It conforms with what a correct program depends on, the size and
the layout of a type and the symbol a name links to, and leaves hints
and warnings for later: a declaration it cannot honour is accepted,
and its use is refused. `restrict`, `_Noreturn`, and `inline` on a
declaration, are dropped, and so is `__extension__` before one. An
attribute is of one of two kinds. One that only licenses an
optimizer, asks for a diagnostic, or describes what a compiler that
calls each function as written does not depend on, `nonnull`, `pure`,
`format` and their like, is dropped. Any other, known or not, marks
what it is said of: a typedef through one names a not-yet type of its
own, a marked function or `extern` object is refused where an
expression names it, and on a local, which rcc would make, the
attribute is not yet. An attribute among the declaration specifiers
is said of each declarator, as one after the declarator is. One on a
parameter marks the function, whose calls pass the parameter, and its
definition. On a struct, a union or an enum every attribute is
dropped, since the type is a not-yet type. `__asm__ ("name")` after
the declarator of a function or an `extern` object is the symbol's
name in the assembly: glibc declares `scanf` as `__isoc99_scanf`, and
the call names that.

The types a header declares and rcc does not use are not-yet types:
the complex types, `_Complex` among the specifiers of a floating
type; GNU C's `__float128` and `__int128`, signed or unsigned;
clang's names of the SVE types, which no header declares. A typedef
through `volatile` or `_Atomic` names a not-yet type of its own; both
qualifiers are not yet anywhere else, and so is `_Atomic (` *type
name* `)`. The definition of a `static inline` function, as glibc's
`bits/byteswap.h` defines `__bswap_16`, is accepted: its body is
skipped to the closing brace, c1 writes nothing for it, and an
expression that names the function is refused. `static` on anything
else is not yet.

## 6. The generator

`generate(unit, target, file, out)` calls the target's generator,
`generate_x86_64()` or `generate_aarch64()`. `file` is the source's
name, for the `.file` directive. The x86-64 generator walks the tree
and writes assembly text in AT&T syntax, one instruction, or the
machine's fixed sequence, per operator, with nothing added and
nothing left out but the 32-byte alignment of a loop's head, which
is layout, not an operator's code. The aarch64 generator is the same
walk under the rules of `aarch64.md`. The x86-64 rules, each the
code of a `backend.md` chapter:

- The state is per function, reset at each definition: the slots,
  the held values, the frame's size, the label numbers. Only the
  string literals run across the file.
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
- A `register` variable is in the callee-saved register of the five
  its symbol's index names (chapter 4). Its read is borrowed: the
  register itself, never spilled or freed, and written only by a
  compound assignment in place. An instruction that would write its
  destination works on a copy, and a held read is copied before the
  variable is written, so the read keeps the old value (G13). The
  registers the prologue pushes are the definition's count, known
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
  one label per joined literal, never merged (chapter 10, G17).

A reader checking a promise opens c1's source, finds the function that
writes the row, and reads what is not there.

## 7. Errors

The first error is fatal: `file:line:column: error: message`, the
form clang and Emacs agree on, exit status 1. The lexer or the
parser throws `error_t` with the location in hand; the driver
catches it and prints. Recovery is the complexity dropped first, and
nothing in the method needs it: the corpora that build are correct
programs, and each program of the error corpus, `tests/error/`, draws
one diagnostic. Its pin, per target, is what rcc printed on standard
error and the exit status, so a message reworded or moved, or a
refusal turned into an acceptance, fails its row.

## 8. The driver

`rcc [-w] [-S|-c] [-target name] [-o out] file.c`. `-S` stops at the
assembly, and the pins of the oracle and of the cost corpus are what
`-S` wrote. `-c` stops at the object, which `clang -c` assembled.
Without either, `clang` links an executable. The default output is
`<stem>.s`, `<stem>.o` or `a.out`. There is no `-O`: there is one
pipeline, and it is literal, so `-O` is an unknown option and a usage
error. Processes are spawned by fork and exec, never `system`; the
temporary `.s` of `-c` and of a link is removed on every way out.

A warning does not stop the translation. The parser records it in
the unit, and the driver prints the unit's warnings after a
successful parse, on standard error in the error's format, `warning`
in place of `error`. `-w` silences them, and no warning changes the
exit status. The one warning is for a live `register` variable past
the target's count, the sixth on x86-64, which takes a slot
(`backend.md` chapter 4).

Exit status: 0; 1 for a diagnosed error or a failed system call; 2
for a usage error; 3 for an internal error, a construct c1 does not
handle yet; `clang`'s own status when it fails.

## 9. The build and the layout

GNU make, one executable. The Makefile is short enough to read
whole; meson would add dependency scanning, a test runner and a
compile database, and three lines of make, a shell script and
`compile_flags.txt` cover the three.

    Makefile          build, test, bench, slots, clean
    compile_flags.txt what clangd needs
    docs/guarantees.md  the meaning: what happens where ISO C says nothing
    docs/cost.md        the cost: what each construct compiles to
    docs/rationale.md   the reasoning behind draft 1 of the guarantees
    docs/findings.md    what the measurement found, explained from the ground up
    docs/backend.md     the design of rcc's own back end, x86-64 first
    docs/aarch64.md     the design of the back end for aarch64
    docs/autoincrement.md  the design of *p++ on a register pointer, for aarch64
    docs/llvm.md        the LLVM sprint: the literal pipeline, where its judgment showed, why it left
    docs/design-calls.md  the back end's open calls, weighed one by one
    docs/slot.md        what a stack slot costs, explained from the ground up
    src/lex.hh .cc    tokens and tokenize()
    src/type.hh .cc   type_t, constructors, equality
    src/ast.hh        the tree
    src/parse.hh .cc  parse()
    src/target.hh     target_t, the facts of a target
    src/c1.hh         generate(), the code generator
    src/c1.cc         generate(), which calls the target's generator
    src/c1-x86-64.cc  the generator for x86-64
    src/c1-aarch64.cc the generator for aarch64
    src/c1-common.hh .cc
                      the part of c1 that is the same for every target
    src/c1-stack.hh   the register stack, which each generator includes
    src/c1-statements.hh
                      the statements, which each generator includes
    src/error.hh      error_t and warning_t
    src/main.cc       the driver
    tests/oracle      the oracle script
    tests/corpus/     the divergence programs and their expected statuses
    tests/corpus/x86-64/
                      their pinned .s for x86-64, one directory per target
    tests/corpus/aarch64/
                      their pinned .s for aarch64
    tests/costs       the cost corpus script
    tests/cost/       one program per cost-model row
    tests/cost/x86-64/
                      their pinned .s for x86-64, one directory per target
    tests/cost/aarch64/
                      their pinned .s for aarch64
    tests/errors      the error corpus script
    tests/error/      one program per diagnostic rcc can print
    tests/error/x86-64/
                      their pinned standard error and exit status for
                      x86-64, one directory per target
    tests/error/aarch64/
                      their pinned standard error and exit status for
                      aarch64
    tests/probes      the probe script
    tests/probe/      the probes: rcc's code against clang -O0's
    tests/bench       the measurement script
    tests/perf/       the benchmark programs, plain and tuned
    tests/perf/aarch64/
                      the tuned sources written for aarch64, one
                      directory per target that has its own
    tests/slots       the slot's cost script
    tests/slot/       its four loops, each variable in a register or a slot

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

The second target, aarch64, is written in the same three slices
(`aarch64.md` chapter 14). All three, integers, control and calls,
then pointers, arrays and string literals, then register variables,
are in the tree, with their pins and the probes that run rcc's code
against `clang`'s. Its measurement, on two machines, is in `cost.md`
chapter 4.

The fourth slice is a program with its headers. `typedef`, `extern`
at file scope, the types a header declares and the program never
opens, and the GNU C a system header is written in (chapter 5) bring
`stdio.h`, `string.h`, `ctype.h`, `unistd.h`, `stdint.h` and
`stddef.h` through the parser. It is parser work but for the name
`__asm__` gives a symbol, which c1 writes at the label and at the
call. The milestone program, `header-stdio` in the corpus, includes
`stdio.h` and `string.h`, formats a line with `snprintf`, checks it
with `strcmp` and prints it: the same line and the same status under
rcc and under `clang -O2`. `stdlib.h` and `sys/types.h` stop at a
`static` function with a body, and `signal.h` at a function-pointer
declarator.

## 11. Not yet

Structs, unions, enums, floating point and `__int128` (each type is
declared, a struct's or union's members skipped and an enum's
constants declared without their values; a pointer to one is an ordinary pointer, and a
use that needs the object, or a constant's value, is refused), global
variables (an object declared `extern` at file scope is accepted and
its use refused; a definition at file scope is refused), `extern` in
a block, brace initializers and
`char s[] = "..."`, function pointers and a declaration through a
typedef of function type, wide and `u8` strings,
`volatile` outside a typedef (c1 writes every access it meets, an
unused one included, so the access needs nothing more; the qualifier
waits for the type model, as `const`'s checks do),
`switch`, `goto`, `static` outside a `static inline` definition, whose
use is refused, a `register`
array (6.3.2.1p3), `inline` on a function definition,
what an attribute that rcc does not honour marks (an object of a
typedef so declared, the use of a function or an `extern` object, a
local, a function's definition), `__asm__` as a statement, at file
scope or on a local, and C's keywords outside the subset: `_Bool`,
`_Complex` outside the specifiers of a floating type, `_Imaginary`,
`_Atomic` outside a typedef, `_Alignas`, `_Alignof`, `_Generic`,
`_Static_assert`, `_Thread_local`. Each arrives when a
probe needs it. The preprocessor stays external for good.
