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
  compared structurally, and a struct or a union by its identity.
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
keeps its spelling, its encoding prefix, quotes and escapes
included; the functions beside `tokenize` yield a literal's prefix,
the elements of a string in an encoding, and the value of a
character constant, and cannot fail, because the lexer refused every
malformed escape and character when it made the token.

The source reaches the lexer as UTF-8, and its literals keep it. A
literal of no prefix is in the literal encoding, UTF-8 as `clang`
has it (J.3.5): its bytes are the text's as written, and a
universal character name (6.4.3) gives the UTF-8 of its code point. A
literal of `u8` is the same, its text checked to be UTF-8. One of
`L`, `u` or `U` has the text's UTF-8 decoded into code points, each
encoded in the literal's encoding: UTF-32 for `L`, the wide literal
encoding as `clang` has it (J.3.5), and for `U`, UTF-16 for `u`, a
code point past `FFFF` a surrogate pair. A sequence that is no UTF-8
is an error there. An octal or hexadecimal escape gives an element's
value itself, bounded by the element's type (6.4.4.5p9): a byte, 16
bits for `u`, 32 for `L` and `U`. A universal character name is four
or eight hex digits and no surrogate and no code point past `10FFFF`
(6.4.3p2); in a literal it may name any other code point. A
character constant holds one character. One of no prefix holds a
byte, so a character outside ASCII, `'é'`, is an error, as `clang`
has it; one of `u` holds one UTF-16 code unit, so `u'😀'` is an error
(6.4.4.5p10); one of a prefix holds no more than one character, and
one of no prefix with more is not yet. `u8'x'` is C23's, and the
dialect rcc preprocesses in has none, so it is not yet.

An integer constant is decimal, octal, hexadecimal or binary, `0b`
or `0B` and the digits 0 and 1 (6.4.4.2p1), and takes its suffix
whole, the bit-precise `wb` and `WB` among the suffixes; the parser
reads its value and its type, the first of 6.4.4.2p6's list that
holds the value, a binary constant's the list of an octal or a
hexadecimal one, and refuses a bit-precise constant, of a
`_BitInt` type, as not yet. A floating constant is a
token of its own, in the grammar of 6.4.4.3p1: decimal or
hexadecimal, a fraction, an exponent, which a hexadecimal one must
have, and a suffix, `f`, `l` or one of the decimal ones, which p2
keeps off a hexadecimal constant. Its value is not computed: the
parser refuses the constant where an expression uses it, and a
floating constant in the skipped body of a system header's `static
inline` function, as clang's `tgmath.h` writes `0.F`, is never read.

A line marker names the file and the line of the text after it, and
its flags, after the name, say more of that text: `# 33
"/usr/include/bits/byteswap.h" 3 4`. Flag 1 enters a file, 2 returns
to one, 3 marks a system header's text, and 4 text read as if in an
implicit `extern "C"`. A marker without 3 ends a system header's
text. The lexer keeps the flag 3 in the location, so each token says
whether it came from a system header; the parser asks it of a `static
inline` function (chapter 5). The other flags change nothing. A
program may write a line marker itself, and `clang -E` writes it into
its output again, flags included, as a marker of its own.

The keywords are C17's, those outside the subset included, so the
parser refuses each by its name. GNU C's spellings in the
implementation's name space, which a system header writes, are
keywords too: `__const`, `__restrict__`, `__signed__`, `__inline` and
their like are the keyword they spell, and `__attribute__`, `__asm__`,
`__extension__`, `__int128` and `__float128` are tokens of their own.

## 3. The types

A type is the machine's view: void, an integer of a width and a
signedness, a pointer to a type, an array of a type and a count, a
function of a result and parameters, a struct or a union, or a type
rcc declares and does not use yet: an enum in its own list or marked
by an attribute (chapter 5); a floating type, real or complex, GNU
C's `__float128` among them; `_Bool`; GNU C's `__int128`;
`__builtin_va_list` and clang's SVE types; an atomic type; a type a
typedef names through an attribute rcc does not honour, or through an
array size rcc cannot compute. Such a type
carries an identity, which equality compares, and the words a
refusal of its use prints; it has no size and no alignment, and a
pointer to it is an ordinary pointer. The type carries no qualifier:
`const`, `volatile` and `restrict` are dropped (chapter 5). The C
spelling is not kept:
`long` and `long long` are one type on a target where both are 64
bits. Any other enumerated type is the integer type compatible with
it (chapter 5), so `enum color { red, green }` and `unsigned int`
are one type, and so are two enumerations of one integer type, which
C makes distinct types (6.7.3.4p9): a conversion between pointers to
the two draws no diagnostic, as one between `long *` and `long long
*` draws none. The edges from a type to the types it names are
`shared_ptr< const type_t >`: a type is never written after it is
made, so sharing is invisible and copying a type is cheap. Equality
is structural, through the edges. No interning; a table of types is
an optimization taken when it is measured. The size and the
alignment of a type are the type's, `size_of` and `align_of`, the
same on both targets, the sizes LP64's; the parser folds `sizeof`
from them and c1
sizes and aligns the slots from them. A pointer to a function is a
pointer as any other: 8 bytes, an address, of a type that compares
through the function's result and parameters. The types of the wide
characters are the platform's, as `clang` has them on each target:
`wchar_t` is `int` on x86-64 and `unsigned int` on aarch64, 4 bytes
on both, so `(wchar_t) -1 < 0` holds on the first alone; `char16_t`
is `unsigned short` and `char32_t` `unsigned int` on both. The tree
keeps no spelling, so each is its integer type.

A struct or a union is a record, and its type holds a pointer to the
record, which the unit owns in a `deque`, as it owns the symbols. The
parser makes the record incomplete at the first mention of its tag,
or at a list without one, and completes it in place at the list's
`}` (6.7.3.4p5). So `struct internal *p;` before the list points to
the type the list completes, and a member points to its own record.
Two records are one type when they are one record: one tag in one
scope declares one type (6.7.3.4p4), and each list without a tag one
of its own (p9). Equality stops at a record, so a record compares by
its identity where every other type compares by its structure, and a
record whose members point to it compares. C23 makes two complete
records of one tag and of the same members compatible (6.2.7p1); rcc
compares no two records' members, so where two such meet, a
conversion between pointers to them among them, the use is not yet.

A complete record has its members, each a name, a type and an offset,
its size and its alignment, laid out by the rule `clang` has for both
targets, which is one rule for the types rcc has: each member of a
struct at the offset so far rounded up to the member's alignment, in
the order declared (6.7.3.2p16, p17); each member of a union at 0
(p18); the record's alignment its members' largest, and its size the
end of its members rounded up to that alignment (p19), so that each
element of an array of it is aligned. `size_of` and `align_of` read
the layout, and the parser folds `sizeof`, `_Alignof` and `offsetof`
from it. A record whose list holds a construct rcc does not lay out
yet is complete and has no layout: a bit-field, an anonymous member,
an attribute that marks, `packed` and `aligned` among them, a
flexible array member, a list of no member, or a member of a type
rcc does not use yet. It keeps the words of the first such construct,
and a use that needs its layout is refused by them, as a not-yet
type's is; a pointer to it is an ordinary pointer, so a program's
`FILE *` is what it was.

## 4. The tree

Three sums.

- `expr_t`: a type and a node. The nodes are integer or character
  literal, name, string literal, unary, binary, assignment, call,
  cast, conditional. A name holds a non-owning pointer to its
  symbol. A string literal holds its elements' bytes and the
  elements' width and is typed `char [N + 1]`, or for a wide one an
  array of `N + 1` of its element type (chapter 5). Whether an
  expression is an lvalue follows from its node (the
  name of an object, a string literal, a dereference of an object
  pointer), it is not stored; a function designator is none. A compound assignment carries the type it is computed in,
  the usual arithmetic conversions of its operands. A record's
  assignment is a plain assignment of the record's type, its two
  operands the lvalues of one record, for the generator to copy the
  right's object into the left's. Pointer
  arithmetic is a binary add or sub of the pointer's type with the
  integer operand widened to 64 bits in its own signedness; the
  difference of two
  pointers is a binary sub of type `long`; a subscript is a
  dereference of an add, as 6.5.3.2p2 defines it, whose operands the
  parser checks first, a pointer to a complete object type and an
  integer (p1), so that its errors name the subscript; a member
  access is the dereference of the record's address plus the
  member's offset in bytes, `p->m` as `*(T *) ((char *) p + 8)` and
  `s.m` as `(&s)->m`, as the Seventh Edition compiler's first pass
  wrote `a->b` as `*(a + offset)`, so it has no node of its own
  (chapter 5); `sizeof` is an integer literal the parser folded. A
  unary `+` has no node: it is
  the promotion of its operand (6.5.4.3p2), a cast, and over an lvalue
  that no promotion converts, a cast to the lvalue's own type, since
  the result is a value and no lvalue; so `+x` is `(T) x`, `T` the
  promoted type, and the source's `+` is no instruction. `!` is the
  one operator whose operand the tree leaves unpromoted: `!E` is `(0
  == E)` (6.5.4.3p5), whose comparison would promote a `char`, but a
  promotion changes no zero, so the operand is tested against zero
  at its own width, as a condition is, `cmpb $0, -1(%rbp)` for `!c`
  on a `char`, and the result is the `int` the standard gives. A
  conditional of two void operands has type `void` (6.5.16p6) and
  stands where a value is dropped, as a call of a void function does;
  one void operand and one of a value is an error (p3). Two pointer
  operands, or a pointer and a null pointer constant, meet at one
  type (p7): the null pointer constant takes the other's, before a
  pointer to `void` and an object pointer meet at `void *`, so `c ?
  (void *) 0 : p` is an `int *` for an `int *p`; `==` and `!=` test a
  pointer against a null pointer constant in the pointer's type, a
  test of null (6.5.10p6). A call's
  callee is the name of a function, the direct call, or an
  expression of
  pointer-to-function type, the call through a pointer; c1 tells the
  two apart by the callee's type. A function designator where a value
  is wanted, a function's name or `*p` through a pointer to a
  function, decays as an array does, a cast to the pointer over the
  designator: over a name it is the function's address, over `*p` it
  is `p`'s value. `&f` is that cast too, so `f` and `&f` as values
  are one node. A value of pointer-to-function type is loaded,
  stored, passed, returned and compared as any pointer, and no node
  is particular to it.
- `stmt_t`: expression, declaration, block, if, switch, while, do,
  for, break, continue, label, case label, goto, return. A `for`
  carries its init as a statement, so a declaration in the init costs
  nothing. `break` and `continue` name no statement: c1 finds the loop
  or the switch they leave or go on with as it generates the
  statements around them. A label and a `goto` carry the label's name.
  A label is a statement of its own, so it may stand before a
  declaration and before a block's closing brace; a labeled statement
  where one statement is wanted, the body of an `if`, a `switch` or a
  loop, is a block of the label and the statement, which opens no
  scope. A switch carries its controlling expression, promoted; its
  body; its cases, each a value, converted to the promoted type, and
  the number of its label; and the number of its default's label, or
  none. A `case` or `default` label in the body is a case label, a
  statement of its own as a label is, which carries the number. The
  parser numbers a function's case and default labels from 0 in the
  order written, and c1 gives each number a label of the function.
- `unit_t`: the translation unit. It owns every symbol in a `deque`,
  so symbol addresses are stable for the life of the unit, and holds
  one `function_decl_t` per function symbol, with a body when the
  unit defines the function and none when it only declares it. A
  definition carries the count of register variables it uses, its
  parameters among them, the highest index plus one. It holds one `object_decl_t` per
  object it defines, in the order of the definitions, with the
  initializer when one was written: c1 writes the data from them. The
  parser folds an initializer into that data, the subobjects it gives
  a value, each a byte offset in the object, a type, a scalar's or an
  array of `char`'s, and a value: a constant of the type; an address
  constant, a symbol, a global, a function or a string literal, and a
  byte offset from it; or the bytes of a string literal that
  initialize an array of `char`. They are in the order of their
  offsets, and every byte none of them holds is zero, so a brace
  list reaches c1 with its brace elision resolved and its omitted
  elements implied. A record's list is the same subobjects, its
  members' and theirs at their offsets in the object, so the data
  holds no record and no padding of its own.

A symbol is a name, a type, a storage and a linkage. The storage is
local, parameter, global, function, type name or enumerator. A global
is an object of static storage duration: one declared at file scope,
or one a block declares `static` or `extern`. One the unit defines
has an entry in the unit's objects, and that entry is how c1 tells it
from one the unit only declares, which has none and for which nothing
is written: the first is reached PC-relative and the second through
the GOT. A block's `static` object has an entry always. The linkage
is external, internal or none (6.2.2): a function or an object at
file scope has external linkage, or internal when declared `static`;
a function or an object a block declares `extern`, or a function it
declares, has the linkage of the visible declaration of the name, or
external without one (chapter 5); and every other name has none, a
block's `static` object among them. One entity of linkage is one
symbol, whichever scope declares it first. c1 writes `.globl` for a
symbol of external linkage alone, so a `static` function or object is
local to the file's object, which is the one difference in its
listing. A
parameter of a prototype is a symbol of storage parameter, never a
global, even where nothing defines its function. A type name is a typedef's: its type is the type the name stands for,
it has no object, and nothing is written for it. An enumerator is an
enumeration constant: it carries its type and its value, and an
expression that names one is the literal of its value, so no name in
the tree is an enumerator's. A function or a global may carry two
facts more: the name
`__asm__` gives it in the assembly, which c1 writes wherever it writes
the symbol, and which a block's `static` object takes from the parser
as its label, `next.count` (`backend.md` chapter 10); and a marking, the words its use is refused with when an
attribute changes what the program depends on. A local or parameter the
parser gives a register carries its index, which of the target's
register variables it takes: each
takes the next in declaration order and gives it back at its block's
end (`backend.md` chapter 4), so the index is one above the highest of
those live where it is declared and of those the closed inner blocks
of its block took, since its object is live from the entry into its
block (6.2.4p6). The parser decides this, and the generator
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

A declarator is C's: pointers, then the name or a declarator in
parentheses, then array suffixes or one parameter list. The
declarator in parentheses derives from what the suffixes after it
derive (6.7.7.1p6), so the parser reads those suffixes first, then
the declarator inside the parentheses, and goes on after the
suffixes: `void (*signal(int, void (*)(int)))(int)` is a function
returning a pointer to a function. The same production without the
name is the abstract declarator of a type name, `(void (*)(int)) 0`,
`sizeof (int [3])`, and of a parameter that leaves its name out. A
parameter of function type is adjusted to a pointer to the function,
as one of array type to a pointer to its element (6.7.7.4p6, p7). A
typedef name of function type stands for the type, and a pointer
through it is a pointer to the function; a function declared through
it is not yet. A typedef defines no function: a function definition's
storage class is `extern` or `static` (6.9.2p4), so a body after a
typedef's function declarator is an error of its own.

A parameter list opens a scope at its `(` and closes it at its `)`,
the function prototype scope of 6.2.1p4. Each parameter is declared in
it as its declarator completes (6.2.1p7), so a later parameter sees
it: `int f(int n, int a[n])` sizes `a` by `n`, and in `typedef int T;
int f(int T, T x);` the second `T` names the parameter, not the type.
A tag first written in the list is declared there and ends with it,
so the `struct S` of `void f(struct S *);` is no struct declared after
it. A definition opens the scope again as its body's (6.9.2p10): the
body sees the parameters and the tags of the list that makes the name
a function, which in `int (*g(int a))(int b) { ... }` is `(int a)`
alone. An unnamed parameter of type `void` as the only one is the
empty list (6.7.7.4p9), whatever spells the type, a typedef name
included.

The array a parameter is adjusted from, the suffix right after the
parameter's name or the place of it, takes `static` and the type
qualifiers inside its brackets before the size, as `regex.h` writes
`regmatch_t __pmatch[__restrict __nmatch]`. They are dropped,
`restrict` is checked as on a pointer, and `_Atomic` there is not yet.
Its size need not be constant: one that is not is taken as left out,
as 6.7.7.3p5 takes it in a prototype, since the parameter is the
pointer either way, and nothing is evaluated for it. Every other array
suffix keeps the rules of an array, so `int (*a)[n]` points to a
variable length array, which is not yet. At file scope such a size is
an error of the program, not a construct to come: an object of static
storage duration has no variable length array type, and an identifier
of a variably modified type has block or prototype scope (6.7.7.3p2),
so `extern int n; int a[n];` and a typedef of the array there are
refused in words of their own.

A pointer to a function is a value as any pointer is, with the type
rules of the standard: assignment and argument passing take one of
its own type or a null pointer constant, and a pointer to void only
through a cast (6.5.17.2p1); `==` and `!=` compare it with one of its
type or a null pointer constant (6.5.10p2), and `<`, `<=`, `>` and
`>=` are an error on it (6.5.9p2); it is a condition, an operand of
`!`, `&&` and `||`, and an arm of `?:`; a cast converts it to any
pointer or integer type and back, with no instruction. Arithmetic on
it is an error, and so is `restrict` on it.

A function designator where a value is wanted converts to the pointer
to the function (6.3.2.1p4), as an array decays, and `&f` is that
pointer too (6.5.4.2p3). `*p` on a pointer to a function is the
function designator again (p4), which converts back, so `(*p)(x)`,
`(**p)(x)` and `p(x)` are one call; a designator is no lvalue, so
`*p = q` and `&*p = q` are errors. A call's callee is any expression
that is a pointer to a function after the conversion (6.5.3.3p1). The
result type, the argument conversions, the count check and the
promotions of a variadic pointee come from the pointed-to function
type, as from a named function's; that is the call site's type,
whatever function the pointer was converted from (G16). A function's
name, in parentheses or not, stays the callee of a direct call;
`(*f)(x)` and `(&f)(x)` go through the pointer the source formed.

A system header is written in GNU C, and the parser reads it as it
is. It conforms with what a correct program depends on, the size and
the layout of a type and the symbol a name links to, and leaves hints
and warnings for later: a declaration it cannot honour is accepted,
and its use is refused. `restrict`, `_Noreturn`, and `inline` on a
declaration, are dropped, and so is `__extension__` before one. An
attribute is of one of two kinds. One that only licenses an
optimizer, asks for a diagnostic, or describes what a compiler that
calls each function as written does not depend on, `nonnull`, `pure`,
`format`, `may_alias` and their like, is dropped. Any other, known or
not, marks what it is said of: a typedef through one names a not-yet
type of its own, a marked function or object at file scope is refused
where an expression names it, and on a local or the definition of an
object, which rcc would make, the attribute is not yet. An attribute
among the declaration specifiers
is said of each declarator, as one after the declarator is, and one
may begin a declaration, in a block as at file scope:
`__attribute__ ((unused)) int x = 1;` in a block declares `x`, the
attribute dropped. Attributes before a `;` or an expression begin no
declaration. Before a `;` they stand on a null statement, as GNU C's
`__attribute__ ((fallthrough));` does, dropped or not yet by their
kind, and the statement writes nothing; before an expression they
are an error. One on a
parameter marks the function, whose calls pass the parameter, and its
definition. On a struct or a union, or on a member, one that marks
changes the layout, as `packed` and `aligned` do, and makes the
record one rcc does not lay out yet (chapter 3); on an enum one that
marks makes the enum a not-yet type of its words (below). `__asm__
("name")` after the declarator of a function or of an object at file
scope is the symbol's name in the assembly: glibc declares `scanf` as
`__isoc99_scanf`, and the call names that.

`volatile` is dropped as `const` is, wherever `const` may stand:
among the declaration specifiers, after a `*`, in a type name, once
or more. It asks that each access the source writes be made
(6.7.4.1p8), and c1 writes every access of every object, an unused
one included, at the object's width: `*p;` is a load, and `*p + *p`
is two. What an access to a volatile object is, which p8 leaves to
the implementation, is thus each load and each store c1 writes. A
typedef through `volatile` names the type itself: glibc's
`pthread_spinlock_t` is `int`. A `register volatile` variable is a
register variable as any other, and there rcc departs from
7.13.2.1p3, which keeps the value of a volatile local of the function
that called `setjmp`: `longjmp` restores the callee-saved registers,
so a register variable takes back the value it had at `setjmp`,
volatile or not. Every other local is in its slot and keeps the value
last stored. No conversion that discards `const` or `volatile` is
diagnosed, nor an assignment to a `const` object (chapter 11).

An enumeration is the integer type compatible with it, and each of
its constants is its number, so an object, a parameter, a result, a
cast, `sizeof`, a `switch` and the arithmetic of one are those of its
integer type, and c1 sees integers. Which type that is, 6.7.3.3p13
leaves to the implementation (J.3.10), and rcc's choice is `clang`'s
on both targets, by the values of the constants:

- `unsigned int` when no value is negative and it holds every one,
  else `unsigned long`;
- `int` when a value is negative and it holds every one, else `long`;
- no type for a negative value and one past `LONG_MAX` together, an
  error (p4).

So `enum color { red, green }` is an `unsigned int`: an object `c`
of it promotes to `unsigned int` (6.3.1.1p2), and `c - 1 < 0` is
false.

A constant is an `int` when `int` holds every value of its
enumeration, and of the enumerated type otherwise (6.7.3.3p15): in
`enum { A, B = 0x80000000 }` both are `unsigned int`. While the list
is read, each constant has the type p12 gives it. The first without
`=` is the `int` 0. One with `=` takes an integer constant expression
(p4), as an `int` where `int` holds it and in its own type otherwise.
Any other is the previous value plus one, in the previous constant's
type, or in the 64-bit type of its signedness where that type does
not hold the sum, and an error past the 64-bit types. So `enum { A =
0x7fffffff, B, C = sizeof B }` gives `C` the size of a `long`, 8. A
value that needs what rcc does not have yet, the size of a struct rcc
does not lay out among them, is refused where it stands, in a system
header as in the program; the headers that parse write none. A
constant's scope begins after its enumerator (6.2.1p7), so `enum { A
= A + 1 }` in a block reads an outer `A`.

Until the `}` of its list an enumeration is incomplete (6.7.3.4p5):
its tag names a not-yet type there, a pointer to which is taken, and
each other use is not yet. A tag without a list names the type its
list made, and is an error before the list (6.7.3.4p2). GNU C's
forward declaration, `enum e;` before `enum e { ... }`, is that
error: a pointer made to the type before its list would not point to
the integer type the list completes it as. C23's enum type specifier,
the fixed underlying type of `enum e : long { ... }` (6.7.3.3p5), is
not yet: no header and no program at hand writes it. An attribute
that marks, `packed` among them, makes the enumeration a not-yet type
of its words, since `packed` makes the type the narrowest integer that
holds its values and rcc does not; the constants keep their values,
and an `int` constant its use.

A struct or a union specifier with a list reads its members with the
declaration's own machinery (6.7.3.2p1): specifiers with no storage
class and no function specifier, then declarators, each a name and a
type, or a bit-field's width after a `:`. A static assertion among
them is checked where it stands, and `__extension__` before one is
dropped. The list opens no scope: a tag or an enumeration constant
declared in it is declared where the record is (6.2.1p4), so `struct
s { enum e { A, B } k; };` declares `A` and `B` beside `s`, and the
members' names are the record's own (6.2.3p1). A member is of a
complete object type (6.7.3.2p3, p11): a function, `void`, an
incomplete record, the record being defined among them, and a second
member of one name are errors, and so is an array of unknown size
anywhere but last in a struct of two named members or more, where it
is a flexible array member (p20). A member declaration with no
declarator declares an anonymous member where its specifier is a
struct or a union without a tag (p15), and otherwise the tag or the
constants of its specifier alone. In a list a system header writes,
the rule for a system header holds as for a typedef's array size: a
refusal of the not-yet kind makes the record one rcc does not lay out,
of the refusal's words, and the parse skips to the list's closing
brace, so a struct with an `_Atomic` member parses there; an error
stays the error it is, and the program's own list is refused where it
stands. clang's `__int128_t` and `__uint128_t`, which glibc's
`sys/user.h` writes in a member list, are the 128-bit integer types.

`sizeof` and `_Alignof ( type-name )` of a record are its layout's
(6.5.4.4p3, p4), and of an incomplete record errors (p1); `_Alignof`
of an expression, which GNU C takes, is not yet. `__builtin_offsetof
( type-name , member-designator )`, which `stddef.h` defines
`offsetof` by, is the offset in bytes of the subobject the designator
names (7.21p4), a member's name, then `.name` and `[constant]` in any
number, folded to a `size_t` constant. A record is an expression only
as an lvalue, by a name or by an indirection: its address by `&` and
its size by `sizeof` are taken, and through a pointer to an incomplete
record the lvalue is not yet, since c1 addresses one by its size.
An object of a complete record is an object as any other, a local, a
static one, one at file scope and one declared `extern`: c1 sizes and
aligns its slot and its data by the layout, and its address and its
size are taken. One of an incomplete record is an error where it has
no linkage, by the end of its declarator (6.7.1p8), and so is one with
an initializer (6.7.11p4) or a tentative definition of internal
linkage (6.9.3p3); a tentative definition of external linkage takes
the record as it is at the end of the unit (6.9.3p2), which must then
be complete. What reads or writes the whole object of a record is not
yet, each in words of its own: a local's initializer, which is stores
at run time; a `register` one, which no register holds; a record
passed or returned by value, by a definition's parameter or result,
by an argument and by a call; and its value, read as an operand or
for its effect, a member's of a record type among them. A cast to a
record or of one is an error, since a record is no scalar (6.5.5p2).

`a = b` on two records is a simple assignment (6.5.17.2p1): `a` a
modifiable lvalue of a complete record (6.3.2.1p1), `b` of a
compatible record, which in rcc is the same record, so two records of
one tag are not yet and any other right operand is an error. The
compound assignments take no record (6.5.17.3p1). The tree is the
assignment node of the record's type, the two lvalues in the order
written, and nothing else: the generator copies the object, the left
operand's place first (G13). Its value is not yet as any record's
value is, so the assignment stands where a value is discarded: a
statement, a `for`'s first or third clause, the left operand of a
comma and the operand of a cast to `void`. Used as an operand, `f(a
= b)` and `a = b = c`, as the record of a member, `(a = b).x`, or as
the right operand of a comma, even one discarded, it is refused in
the words of a record's value, and so is a conditional of two
records. The two objects may overlap (6.5.17.2p3): exactly, as in `a
= a`, which the standard defines, or in part, which it leaves
undefined; the tree is the same, and the bytes are those the copy's
order leaves.

`E.m` and `P->m` designate a member (6.5.3.4p3, p4): `E` a struct or
a union, `P` a pointer to one after its decay, complete and laid out,
and `m` a member of it (p1, p2); each is an lvalue, since every record
rcc makes an expression of is one, and `f().m` has no record to be
the member of yet. The tree is the dereference of the record's
address plus the member's offset, `*(T *) ((char *) P + offset)`, a
cast to the member's pointer type over a sum of a pointer to bytes:
`P` for `P->m`, `&E` for `E.m` (`(&E)->m` is `E.m`, p4's footnote),
and `&*X` is `X` (6.5.4.2p3). A member at offset 0 is `*(T *) P`,
with no sum. The pointer is read through its casts between pointer
types and the byte sum a member access made, so `s.a.b.c` and
`p->in.l` are one offset from the record's address. A member of an
array type decays where its value is used, so `p->a[i]` is a
subscript of the member; one of a function pointer type is called
through, `p->f(x)`; and one of a record type is an lvalue whose own
members are reached. `&p->m`, `sizeof p->m`, `p->m++` and `p->m +=
e` are those of any lvalue, and a union's members are each at 0, so
a member read after another written reads the bytes at its own
width.

The types a header declares and rcc does not use are not-yet types:
the complex types, `_Complex` among the specifiers of a floating
type; GNU C's `__float128` and `__int128`, signed or unsigned;
clang's names of the SVE types, which no header declares; `_Bool`,
which `stdatomic.h` declares functions to return. An atomic type is a
not-yet type of its own, since an access of an atomic object, `x++`
on one among them, is one atomic instruction, which c1 does not
write. `_Atomic (` *type name* `)`, the atomic type specifier, names
one wherever it is written, as `stdatomic.h` declares `atomic_int`,
and so does a typedef through the `_Atomic` qualifier; the qualifier
is not yet anywhere else.

A typedef of an array whose size needs what rcc does not have yet
names a not-yet type whose words say what the size needs, as a size
by the size of a struct with a bit-field does: the typedef is
accepted and a local of the type is refused with "not yet: bit-field
in struct ...". The parser catches a refusal of the not-yet kind
alone, only while it reads the size of a typedef's array suffix, and
only when it has found the `]` that closes the suffix first, matching
the brackets and parentheses in the size; the parse goes on there. A
size that is wrong stays the error it is, and so does such a size
outside a typedef: of a local, a parameter, a cast or a `sizeof`.
Since the parse of the size stops at its first refusal, an error of
the program after it in the size is not seen.

A function of internal linkage may be an inline function (6.7.5p7),
and `inline` suggests only that its calls be fast (p6), so it writes
nothing. A `static inline` definition in the program's own source is
thus compiled as the static function it is, `inline` dropped, and
such a declaration without a body declares a static function. One a
system header writes, as glibc's `bits/byteswap.h` defines
`__bswap_16`, is accepted and not compiled: a header's body need not
be one rcc can compile, and a program that never calls the function
does not pay for it. Its body is skipped to the closing brace, c1
writes nothing for it, and an expression that names the function is
refused, before the definition as after it; a system header's `static
inline` declaration without a body is not yet. The line marker in
force at the declaration's `static` says which of the two it is
(chapter 2).

`static` at file scope gives a function or an object internal
linkage (6.2.2p3). The rules of linkage are the standard's: `extern`
on a later declaration keeps the linkage of the earlier one (p4), and
a function declared without a storage class is as if declared
`extern` (p5), so `static int f(void); int f(void) { ... }` defines a
function of internal linkage, and `static int n; extern int n;`
declares one object of it. An object at file scope declared without a
storage class has external linkage (p5), so `static int n; int n;`
gives `n` both linkages, which p7 leaves undefined; it is an error,
and so is a `static` declaration after one of external linkage. The
function called at program startup is named `main` (5.1.2.3.2p1) and
is found by that name from outside the file, so `static` on a
declaration of the function `main` is an error: the link would find
no `main`. So is a function specifier on any declaration of `main`, a
definition among them (6.7.5p4). A static object at file scope keeps
every rule of an object at file scope below, and its tentative
definition defines it in the file. A name of internal linkage that an
expression uses, outside the operand of `sizeof`, has its definition
in the file (6.9.1p3), so a `static` function called and never defined
is an error at the end of the file: the assembler would leave its
symbol undefined, and the linker would bind the call to another file's
function of the name. A call of a system header's `static inline`
function before its definition is not yet there.

`static` in a block declares an object of static storage duration
(6.2.4p3) and no linkage (6.2.2p6): a name of the block's, declared
once there (6.7.1p4), which hides an outer one and is hidden by an
inner one as a local is. It is an object the file defines, as one at
file scope is: its initializer takes every form a global's takes,
constant, and is folded into its data the same way, so the object is
initialized once, before the program starts, and keeps its value from
call to call; without one it is zero. Its declaration makes no
statement. An array of unknown size takes its size from its
initializer and is an error without one (6.7.1p8). A function declared
`static` in a block is an error (6.7.2p13), and `static` on a
parameter, in a type name and in the declaration of a `for` is one
too.

A block may declare a function, and an object `extern`. The name has
the block's scope (6.2.1p4) and denotes an entity of linkage: a
function declared without a storage class has the linkage it would
have declared `extern` (6.2.2p5), which is that of the visible
declaration of the name when that one has linkage, and external
otherwise (p4). Every declaration of a name of external linkage
denotes one entity (p2), so the block's name is the symbol of the
name's declaration at file scope before the block, or of an
enclosing block's, or the first declaration of the symbol, which a
later one at file scope or in another block meets. Each declaration of
the entity has its kind and its type (6.2.7p2), an error otherwise:
`int f(void) { int g(int); return g(1); } long g(int x) { ... }`
conflicts at the definition. One that gives the name the other
linkage is the error of 6.2.2p7. A block declares the name of a local
once and no other name for it (6.7.1p4), so `int n; extern int n;` in
one block is an error. Such a declaration makes no object and no
statement, and takes no initializer (6.7.11p6); a call of the function
and a use of the object are those of one declared at file scope, so
c1 writes nothing of its own for them. A function of block scope takes
no storage class but `extern` (6.7.2p13), so `register` on one is an
error too. After the block's end the name is gone: a call there needs
a declaration at file scope, as any call does. A declaration of a
`for` may declare a function too, as C23 lets it. A function declared
through a typedef of function type is not yet in a block either.

A static assertion, `_Static_assert (` *constant expression* `,`
*string literal* `);` or C23's form without the string (6.7.12p1), is
a declaration of its own at file scope, in a block and as a `for`'s
first clause, and declares nothing, so the tree keeps nothing of it
and c1 writes nothing. Its expression is an integer constant
expression (p3), folded as an array's size is: one that needs what
rcc does not have yet, the size of a struct rcc does not lay out or a
floating constant, is refused by its words, and one that compares
equal to 0 is the error of p2, `static assertion failed: ...` with
the string, adjacent literals joined. `_Static_assert` is C17's
keyword (chapter 2) and C23's alternative spelling of `static_assert`
(6.4.1p3); in the dialect rcc preprocesses in, `static_assert` is a
macro of `assert.h` that spells `_Static_assert`, and without the
header an identifier, as `clang` takes it there. One in a struct's or
a union's member list is checked where it stands, as a member
declaration (6.7.3.2p1).

An object at file scope is defined by a declaration with an
initializer, `extern` or not, and tentatively by one without an
initializer and without `extern` (6.9.3p1, p2); `extern` alone
declares it. Every declaration of the name is of one object, of one
linkage, and of one type by the tree's equality: `extern int n;
int n; int n = 1;` declare one object, and two tentative definitions
define one, which is zero (6.9.3p2). A second initializer is a second
external definition, an error (6.9.1p5). An array declared without its
size and with it is one object of the composite type, the array of
known size (6.2.7p3, p5): `extern int a[]; int a[3];` and `int a[3];
extern int a[];` declare an `int [3]`, and `extern int a[2]; extern
int a[3];` conflict. Only the outermost bound may be left out. Until a
declaration gives the size, `a[i]` and `a` as a pointer are what they
are for any array, and `sizeof a` is the error of an incomplete type,
as is arithmetic on `&a`, a pointer to that incomplete type, which has
no size to step by (6.5.7p2): `&a + 1`, `++` and `-` on it.
An array of unknown size that only tentative definitions define is one
element at the end of the file (6.9.3p2) if its linkage is external. A
tentative definition of internal linkage is not of an incomplete type
(p3), so a `static` array that no declaration sizes is an error there,
at the name in its first tentative definition. rcc asks this of the
composite type at the end of the file, so `static int a[]; static int
a[3];` declare one `int [3]` (6.2.7p5). The definition of an object of
a not-yet type, or of one an attribute marks, is not yet.

The initializer is a constant expression (6.7.11p5), converted to the
object's type as by assignment (p12) and folded. It is an integer
constant expression, a null pointer of any spelling, an integer cast
to a pointer, or an address constant (6.6p9, p11): the address of an
object of static storage or of a function, plus or minus an integer
constant. The address is written with `&`, a subscript by a constant,
`*`, a pointer cast and the decay of an array or a function
designator, as in `&n`, `&a[3]`, `a + 2`, `&m[1][2]`, `&m[1]`,
`on_signal`, `&stdout` and `(char *) &n + 1`. It may be of an object
or a function the file only declares, and of a string literal, which
is an object of static storage like any other: `"abc" + 1`. An
address cast to an integer of 8 bytes, `(long) &n`, is the address,
which 6.6p14 lets an implementation take; cast to a narrower integer
it is an error, since the loader fills in no narrower field. No
object's value is read, so `&a[n]` and `n` with `n` an object are no
constant expressions, an error. An array of `char`, `signed char` or
`unsigned char` takes a string literal, in braces or not (6.7.11p15).
An array of unknown size takes the literal's size with its
terminating zero, one with room for the zero takes it, one the
literal fills exactly takes none, and a literal longer than the array
is an error (p3). An array of a wide literal's element type takes the
literal the same way, its elements and its zero (p16): an array of
`wchar_t` takes `L"..."`, and so does one of `int` on x86-64, where
`wchar_t` is `int`, and one of `unsigned int` on aarch64, which takes
`U"..."` too; a wide literal for an array of `char` is an error.

A character constant of no prefix is an `int` (6.4.4.5p11), and one
of `L`, `u` or `U` its one code unit, of `wchar_t`, `char16_t` or
`char32_t` (p13 to p15): `L'é'` is 233. A string literal of `u8` is
an array of `char`, as the dialect rcc preprocesses in has it, C17's
6.4.5p6, where C23 makes it `char8_t`; one of `L`, `u` or `U` an
array of the element type (C23 6.4.5p6). Adjacent literals are one
(p5): the prefixed ones among them have one prefix (p2), an error
otherwise, and the whole takes it, so `"a" L"b"` is `L"ab"`, each
literal's text read in that encoding, a literal's of no prefix
among them, which is then to be UTF-8 too.

Any other array takes a brace-enclosed list (p17), and a scalar may
take its expression in one pair of braces (p12). The parser walks the
list against the type, in subscript order (p18): an element whose
initializer begins with a brace takes that list, and one that does
not, an array, takes as many initializers from the enclosing list as
its own elements do and leaves the rest to the next (p21). So `int
m[2][3] = { { 1 }, 4, 5 };` gives `m[0][0]`, `m[1][0]` and `m[1][1]`,
and `{ 1, 2, 3, 4, 5, 6 }` gives the six in order. A string literal
alone in its place, before a `,` or the `}`, initializes an element
that is an array of `char` whole, so `char names[][4] = { "Jan", "Feb"
};` holds two strings and `{ { 'J', 'a', 'n' }, "Feb" }` spells the
first. The elements the list leaves out are zero (p22); the empty
initializer `{}` leaves the whole object zero (p11), and is an error
for an array of unknown size (p4), which otherwise takes its size from
the elements the list gives (p23). A list that goes on past what its
object holds, `int a[2] = { 1, 2, 3 }`, is an error (p3), and so is a
second expression for a scalar and a second pair of braces around
one. A trailing comma is taken (p1). Each element is a constant
expression as above, converted to the element's type; a designator,
`[2] = 5` or `.x = 1`, is not yet.

A record takes a brace-enclosed list too (p17), walked against its
members by the same rule: a struct's in declaration order, a union's
first member alone (p18), each member that is a record or an array
taking its own list or as many initializers as it needs (p21), and a
member that is an array of `char` a string literal alone. The walk is
one rule at any depth, so an array of records, a record of arrays and
a record of records take their lists alike, `{ 1, 2, 3, 4 }` the two
points of a line in order. Every member rcc lays out is named, so
none is passed over (p10). The bytes no member's value holds, the
padding included, are zero (p11, p22), which the data writes as any
other run of zeros. A list that goes on past the record's members, or
a union's first, is an error (p3). An address constant is a member's
value as it is a scalar's, `&line.to.x` the line's label and the
member's offset, and so is a function's name, so a table of records
holds its function pointers as data. A record's value initializes a
record (p14), but is no constant expression (p5), an error here,
whether it stands for the whole object or for a member; any other
expression without braces initializes no record.

In an expression a global is what a local is, an lvalue of its type;
one declared elsewhere of a not-yet type is refused where it is named.

A label name has function scope (6.2.1p3) and a name space of its own
(6.2.3p1): an identifier and a colon are a label wherever a block item
or a statement starts, a typedef name's included, and a variable and a
label may share a name. The parser keeps the function's labels apart
from its scopes. A label defined twice in a function is an error
(6.8.2p3). A `goto` may name a label defined after it, in any block of
the function, so each `goto`'s name is checked at the function's end,
and the first that names no label is the error (6.8.7.2p1). A `goto`
into the scope of a variable length array is an error by the same
paragraph; rcc has no such array, so nothing is checked. The parser
counts the statements around the parse that a `break` may leave and
that a `continue` may go on with: a loop counts for both, and a
`switch` for a `break` alone. A `break` with neither around it is an
error (6.8.7.4p1), and so is a `continue` outside a loop (6.8.7.3p1).

A `switch`'s controlling expression is an integer (6.8.5.3p1), and a
pointer there is an error; it is promoted (p5). The parser keeps the
switches around the parse as a stack, and a `case` or `default` label
belongs to the innermost (p4), in whatever block or loop of its body it
stands, so Duff's device parses. A `case` or `default` label is a
label (6.8.2p1): it stands where a named one may, before a statement, a
declaration or a block's closing brace. A `case` takes an integer
constant expression (6.8.5.3p3), folded; anything else is an error. Its
value is converted to the promoted type (p5), and a second case of one
switch with the same value after the conversion is an error that names
the value and the line of the first (p3), so `case 1:` and `case
4294967297:` collide on an `int`. A second `default` in one switch is
an error that names the line of the first (p3), and a `case` or a
`default` outside a switch is one too (6.8.2p2). GNU C's case range,
`case 1 ... 5:`, is not yet.

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
  string literals and the symbols the file defines run across the
  file.
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
  register itself, never spilled or freed, and written only by an
  assignment: a compound one in place, or the row that computes the
  value assigned (chapter 4). An instruction that would write its
  destination works on a copy, and a held read is copied before the
  variable is written, so the read keeps the old value (G13). The
  registers the prologue pushes are the definition's count, known
  before the body, since the slots lie below them.
- Three entry points: `rvalue(e)` yields the value in a register,
  `cbranch(e, label, sense)` jumps on its truth and yields no value
  (chapter 7), and `effect(e)` writes its side effects and yields no
  value: a constant assigned to memory is one store, and a compound
  assignment or a step on an object in memory is the memory form,
  the operator on the object, `addl $1, -8(%rbp)` (6.6); a
  conditional is the jump to its second arm, each arm for its effect
  and the jump past the second, with no join, and so is a void one
  wherever it stands. `!`, `&&`
  and `||` are the shape of the jumps; any other condition is its
  value compared with 0 at its own width, or on x86-64 a jump on the
  flags of its operator's own instruction where that sets them
  (`backend.md` 7.1).
- An lvalue is an `address_t`: a slot off `rbp`, a label off `rip`,
  or a held base with a displacement or with a held index scaled by
  1, 2, 4 or 8. It is the operand of the one `mov` that reads or
  writes the object (6.3, G8 to G11).
- Operands are generated left to right, and a commutative operator's
  operands are not swapped (6.1, G13), but for a constant on the left
  of a comparison or of `+`, `*`, `&`, `|` or `^`, which is taken on
  the right, `5 < x` as `x > 5` and `5 + x` as `x + 5`.
- The body is written into a buffer. The prologue is written after
  it, once the frame's size is known, then the body and the one
  epilogue (chapter 5).
- The string literals are one `.data` block after the last function,
  one label per joined literal, never merged (chapter 10, G17); a
  wide one is aligned to its element, `.p2align 2`, and its elements
  and the zero are one `.short` or `.long`, `.long 104, 233, 0` for
  `L"hé"`. The
  objects the file defines follow, those with an initializer in one
  `.data` block and the others in one `.bss` block, in the order of
  their definitions; the text is the same on both targets, and is
  `c1-common`'s.

A record's assignment reaches each generator in `effect(e)`, the one
context the parser lets it stand in. Each copies the object there,
inline, unit by unit (G18; `backend.md` 6.6, `aarch64.md` 6.6).

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

An initializer's error is said at one of two places. One of its
conversion as by assignment is said at the `=`, as in `int *p = 1;`;
one of a value the initializer does not have is said at its first
token, so `int x = g();`, for a `void g(void)`, is refused at `g`
(6.3.2.2p1), at file scope and in a block alike.

## 8. The driver

`rcc [-w] [-S|-c] [-target name] [-o out] file.c`. `-S` stops at the
assembly, and the pins of the oracle and of the cost corpus are what
`-S` wrote. `-c` stops at the object, which `clang -c` assembled.
Without either, `clang` links an executable. The default output is
`<stem>.s`, `<stem>.o` or `a.out`. With `-S`, `-o -` writes the
assembly to the standard output, as cc does; an object or an
executable is no text, so `-o -` with `-c` or a link is a usage
error, and `tests/driver` checks both. There is no `-O`: there is one
pipeline, and it is literal, so `-O` is an unknown option and a usage
error. Processes are spawned by fork and exec, never `system`; the
temporary `.s` of `-c` and of a link is removed on every way out.

A warning does not stop the translation. The parser adds it to a
list the driver owns and passes to the parse, so an error that ends
the parse loses none of those recorded before it. The driver prints
the list once, on standard error in the error's format, `warning` in
place of `error`: after a parse that succeeds, or before the error
that ends one, the order in which the parse met them all, as each
warning stands before the first error in the source. `-w` silences
the warnings, and no warning changes the exit status. The one warning
is for a `register` variable whose register would be past the
target's count, the sixth on x86-64 and the eleventh on aarch64,
which takes a slot (`backend.md` chapter 4).

Exit status: 0; 1 for a diagnosed error or a failed system call; 2
for a usage error; 3 for an internal error, a construct c1 does not
handle yet; `clang`'s own status when it fails.

A construct c1 does not handle yet is refused as `rcc: internal
error: c1: not yet: a frame of 2 GiB or more, in function 'main'`.
The tree carries no positions (chapter 4), so the place it names is
the function c1 was generating, quoted as the parser quotes a name.
The message starts as every refusal of c1 does, `c1: not yet:`, which
`tests/costs` reads as a construct whose slice is not in the tree.

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
    tests/driver      the driver's command-line script
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
rcc and under `clang -O2`. The types a header declares for another
compiler's sake, the body of a header's `static inline` function,
skipped, and
the function-pointer declarator bring `stdlib.h`, `sys/types.h`,
`signal.h` and most headers of the C library and of POSIX through as
well: `header-stdlib` and `header-signal` in the corpus call `atoi`,
`strtol`, `signal` and `raise`. A function is a value, its address,
and a call goes through a pointer to one, in both generators
(`backend.md` chapters 8 and 10): `header-qsort` sorts with its own
comparison, and `header-handler` installs its own signal handler and
raises the signal. `volatile` dropped, `_Atomic` and `_Bool`
declared, a floating constant taken as a token, and a typedef's array
of a size rcc cannot compute bring `stdatomic.h` through, and on
aarch64 `signal.h` and `sys/wait.h` with glibc's default feature set.
The scope of a parameter list and the qualifiers inside a parameter's
array brackets bring `regex.h` through, and `header-regex` in the
corpus declares a function in the shape of `regexec`. So 57 of 58
headers of the C library and of POSIX parse on each target;
`tgmath.h` stops at a function declared with `(...)` for its whole
parameter list. Then the objects at file scope: a program defines,
declares and uses its own globals, with integer constants as initializers, and reaches the C
library's, so `header-stdout` writes to `stdout` (`backend.md` and
`aarch64.md` chapter 10). With address constants and strings as
initializers, `header-months` keeps a table of names in an array of
`char` and reaches `stdout`, `putchar` and `fputs` through global
pointers to them. With brace-enclosed lists, `header-calendar` keeps
K&R's tables of the days of the months and of their names (sections
5.7 and 5.8 of the second edition). With `static`, a function and an
object of internal linkage and a block's object of static storage,
`header-getch` keeps K&R's buffer of `getch` and `ungetch` (sections
4.3 and 4.6) private to its file. With `do`, `break`, `continue`,
`goto` and labels, `header-itoa` runs K&R's `itoa`, `reverse` and
`trim` (sections 3.6 and 3.7). With `switch`, compiled by the Seventh
Edition compiler's rule (`backend.md` 7.3), `header-count` counts
digits, white space and other characters as K&R's section 3.4 does.
With `enum`, each enumeration the integer type compatible with it and
each constant its number (chapter 5), `header-escapes` names K&R's
`enum escapes` by a switch on one and counts the days of a year over
an `enum months` (section 2.3). With a function declared in a block
and an object declared `extern` in one (chapter 5), `header-squeeze`
calls K&R's `squeeze` and its `strcat` (section 2.8), defined after
the block that declares them, and the C library's `strlen` and
`strcmp` without their header. With `?:` of two `void` operands
(chapter 4), `header-collatz` takes each step of the walk from 27 by
a conditional of two void calls.

## 11. Not yet

A local's initializer of a struct or a union and each use that
reads or writes one whole (chapter 5: a `register` one, a record by
value and its value), the layout of a struct or a
union with a bit-field, an anonymous member, an attribute that marks,
`packed` and `aligned` among them, a flexible array member, no
member, or a member of a type rcc does not use yet (chapter 3), C23's
compatibility of two records of one tag, floating point, `_Bool` and
`__int128` (each type is declared; a pointer to one is an ordinary
pointer, and a use that needs the object is refused; a floating
constant is a token, refused where an expression uses it),
an enum's fixed underlying type, the use of an enum in its own list
but behind a pointer, an object of an enum an attribute marks, an
object of a typedef whose array size rcc
cannot compute, a variable length array (a parameter's array of a
size that is not constant is the pointer the parameter is adjusted to,
and the size is not evaluated), a designator in
an initializer list, a local's brace initializer, a scalar's or an
array's, and a local array's initializer of any kind, `char s[] =
"..."` among them, a declaration through a typedef of function
type, a UTF-8 character constant, `u8'x'`, a character constant of
more than one character and no prefix, the checks of the qualifiers
(`const`
and `volatile` are dropped, so an assignment to a `const` object and
a conversion that discards a qualifier draw no diagnostic, and
`(const void)` is taken as `(void)`), an atomic object, GNU C's case
range, `case 1 ... 5:`, the body of a system header's `static inline`
function, whose use is refused,
and a system header's `static inline` declaration without a body, a
`register` array (6.3.2.1p3), `inline` on a function
definition without `static`, what an attribute that rcc does
not honour marks (an object of a typedef so declared, the use of a
function or of an object at file scope, a local, the definition of a
function or of an object at file scope),
`__asm__` as a statement, at file scope or on a local, and C's
keywords outside the subset: `_Complex` outside the specifiers of a
floating type, `_Imaginary`, the `_Atomic` qualifier outside a
typedef, `_Alignas`, `_Alignof` of an expression, `_Generic`,
`_Thread_local`. Each
arrives when a probe needs it. The preprocessor stays external for
good.
