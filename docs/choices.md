# rcc: the implementation-defined choices

ISO C leaves some behaviour to the implementation and asks for a
document of each choice (4p10); C23's Annex J.3 lists them. This page
gives rcc's choice for each item of J.3 that the subset reaches, per
target, and the pin or the probe that fails if the choice changes.
Each choice is what the compiler does, as `rcc -target <name> -S`
writes it. A pin of `tests/cost/`, `tests/corpus/` or `tests/error/`
holds the output for both targets; a probe of `tests/probe/` checks
rcc against `clang` on the target of the machine that runs it. Two
choices are promises, G2 and G3 of `guarantees.md`, and their rows
point there. A row fixed by none has its entry in `TODO`.

## What the table leaves out

- The preprocessor is the system compiler's, `clang -E` for the
  target, so translation phases 1 to 4 are `clang`'s: J.3.2(2),
  J.3.3(1) and J.3.12. So are the values of the macros of
  `<limits.h>`, `<stdint.h>` and `<float.h>`, J.3.14(1), and rcc's
  types are the types they describe: `CHAR_MIN` is -128 on x86-64
  and 0 on aarch64. A `#pragma` the preprocessor passes on is rcc's,
  and has its row.
- Floating point, J.3.7: rcc has no floating arithmetic, and refuses
  a floating constant where an expression uses it.
- What rcc refuses: a bit-field, J.3.10(1) to (5); an alignment
  specifier, and an `aligned` attribute on a record, so no extended
  alignment, J.3.14(4) and (5). It has no extended integer type, so
  J.3.6(2) has none to rank.
- The C library's and the host's: a freestanding environment,
  J.3.3(2), (3) and (7), since rcc compiles for a hosted one; the
  environment, J.3.3(5) to (12); the library, J.3.13. Threads,
  J.3.14(2), are out of scope for now.

## The table

| J.3 | Clause | x86-64 | aarch64 | Fixed by |
|---|---|---|---|---|
| J.3.2(1) | 3.13, 5.1.1.3 | `file:line:column: error: message` on standard error, the first error fatal, exit status 1; a warning in the same form, `warning` for `error` (`shape.md` chapters 7 and 8) | the same | every pin of `tests/error/`: `undeclared-name.c`, and `warning-before-error.c` for a warning |
| J.3.3(4) | 5.1.2.3.2 | any function type: a definition of `main` is compiled as written, `int main(int argc, char **argv, char **envp)` and `void main(void)` among them, and reaching its `}` returns 0 | the same | none |
| J.3.4(1) | 6.4.2 | none: an identifier is ASCII letters, digits and `_`; a character outside ASCII is an error, and so is a universal character name, which the preprocessor writes as its UTF-8 | the same | none |
| J.3.4(2) | 5.2.5.2, 6.4.2 | all: the symbol is the whole name | the same | none |
| J.3.5(1) | 3.7 | 8 | the same | the `braces` probe |
| J.3.5(2) | 5.2.1 | ASCII for the basic character set, `'A'` is 65; past it, J.3.5(6) | the same | `tests/cost/volatile.c` |
| J.3.5(3) | 5.2.3 | `\a` 7, `\b` 8, `\f` 12, `\n` 10, `\r` 13, `\t` 9, `\v` 11 | the same | `tests/corpus/header-escapes.c`, all but `\f` |
| J.3.5(4) | 6.2.5 | a byte of the character's UTF-8, read as plain `char`: `"é"[0]` is -61 | `"é"[0]` is 195 | `tests/cost/widen.c` for the read; none for the byte |
| J.3.5(5) | 6.2.5, 6.3.1.1 | `signed char` | `unsigned char` | `tests/cost/widen.c`, `tests/cost/string.c`, the `straight` probe |
| J.3.5(6) | 6.2.9, 6.4.4.5 | UTF-8: the text's bytes as written, `"é"` is `\303\251` | the same | `tests/corpus/header-wide.c`, by the size of `"é"` alone |
| J.3.5(7) | 6.2.9, 6.4.4.5 | UTF-32, a code point per element of `wchar_t`, which is `int`: `L'é'` is 233 | the same, of `wchar_t`, which is `unsigned int` | `tests/cost/wide-string.c`, `tests/corpus/header-wide.c` |
| J.3.5(8) | 6.4.4.5, 5.1.1.2 | the identity: the source is UTF-8, and a literal of no prefix keeps its bytes | the same | as J.3.5(6) |
| J.3.5(9) | 6.4.4.5 | none: two characters or more, `'ab'`, are not yet, and a character of more than one byte of UTF-8, `'é'`, is an error | the same | `tests/error/multi-character-constant.c`, `tests/error/character-too-large.c` |
| J.3.5(10) | 6.4.4.5 | none, an error: two characters, `L'ab'`; a character of two UTF-16 units after `u`, `u'😀'`; an escape past the element's width | the same | `tests/error/wide-character-multiple.c`, `tests/error/wide-character-too-large.c`, `tests/error/wide-escape-out-of-range.c` |
| J.3.5(11) | 6.4.4.5 | no locale: the source's UTF-8 decoded to the code point | the same | `tests/cost/wide-string.c` |
| J.3.5(12) | 6.4.5 | no locale: the source's UTF-8 decoded, a code point per element, and text that is no UTF-8 an error | the same | `tests/cost/wide-string.c`, `tests/error/wide-invalid-utf8.c` |
| J.3.5(13) | 6.4.5 | an escape is the byte it names, `"\xff"` is `\377`; a universal character name is its UTF-8 | the same | `tests/cost/global-string.c`, `tests/corpus/header-wide.c` |
| J.3.5(14) | 6.10.10.3 | UTF-32, as where the macro is defined: the C library's headers define it, so a unit that includes none has it undefined | the same | `tests/cost/wide-string.c`, whose one header, `stddef.h`, leaves it undefined |
| J.3.6(1) | 6.2.5 | none: `__int128` is not yet | the same | `tests/error/int128-local.c` |
| J.3.6(3) | 6.3.1.3 | G2 | G2 | the `memory` probe |
| J.3.6(4) | 6.5.1 | the bits of two's complement; `>>`, G3 | the same | `tests/cost/shift.c`, the `straight` probe |
| J.3.8(1) | 6.3.2.3 | the address as a 64-bit integer: to a narrower integer its low bits, from an integer extended by the integer's signedness | the same | the `memory` probe, `tests/cost/ptr-int.c` |
| J.3.8(2) | 6.5.7 | `long`, 64 bits | the same | `tests/cost/ptr-diff.c` |
| J.3.9(1) | 6.7.2 | five registers, `rbx` and `r12` to `r15`, taken by declaration; a variable past them takes a slot, with a warning (`cost.md` chapter 1) | ten, `x19` to `x28` | `tests/cost/register-rules.c`, `tests/error/warning-before-error.c` |
| J.3.9(2) | 6.7.5 | none: a `static inline` function is compiled as the static function it is, each call made; `inline` on a definition of external linkage is not yet | the same | `tests/cost/static-inline.c`, `tests/error/inline-definition.c` |
| J.3.10(6) | 6.7.3.2 | each member at its type's alignment, which for a scalar is its size, by `clang`'s rule (`shape.md` chapter 3) | the same | the `layout` probe, `tests/cost/struct-object.c` |
| J.3.10(7) | 6.7.3.3 | by its values, `unsigned int`, `unsigned long`, `int` or `long`, as `clang` chooses (`shape.md` chapter 5); under `packed`, not yet | the same | `tests/cost/enum.c`, the `enums` probe, `tests/error/enum-attribute.c` |
| J.3.11(1) | 6.7.4 | each load and each store the source writes, at the object's width (`shape.md` chapter 5) | the same | `tests/cost/volatile.c` |
| J.3.12(10) | 6.10.8 | none: each `#pragma` the preprocessor passes on is skipped, `pack` among them | the same | none |
| J.3.14(3) | 6.2.6.1 | J.3.14(6)'s sizes, little-endian, two's complement: `0x01020304` is the bytes `04 03 02 01` | the same | the `memory` probe |
| J.3.14(6) | 6.5.4.4 | LP64: `short` 2, `int` 4, `long`, `long long` and a pointer 8, each aligned to its size; a record by J.3.10(6) | the same | the `layout` probe |
