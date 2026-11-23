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
point there. A row fixed by none has its entry in `TODO`. Two choices
the text leaves to the implementation and J.3 does not list, of
5.2.5.2 and of 6.2.7p3, have the last rows.

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
- What rcc refuses: an alignment specifier, and an `aligned`
  attribute on a record, so no extended alignment, J.3.14(4) and (5).
  It has no extended integer type, so J.3.6(2) has none to rank.
- The C library's and the host's: a freestanding environment,
  J.3.3(2), (3) and (7), since rcc compiles for a hosted one; the
  environment, J.3.3(5) to (12); the library, J.3.13. Threads,
  J.3.14(2), are out of scope for now.

## The table

| J.3 | Clause | x86-64 | aarch64 | Fixed by |
|---|---|---|---|---|
| J.3.2(1) | 3.13, 5.1.1.3 | `file:line:column: error: message` on standard error, the first error fatal, exit status 1; a warning in the same form, `warning` for `error`; c1's error, which has no position, `rcc: error: message` (`shape.md` chapters 7 and 8) | the same | every pin of `tests/error/`: `undeclared-name.c`, `warning-before-error.c` for a warning, and `frame-of-2-gib.c` for c1's error |
| J.3.3(4) | 5.1.2.3.2 | any function type: a definition of `main` is compiled as written, `int main(int argc, char **argv, char **envp)` and `void main(void)` among them, and reaching its `}` returns 0 | the same | `tests/corpus/main-envp.c`, for `envp` and the `}`; `tests/corpus/main-void.c`, for `void main(void)` |
| J.3.4(1) | 6.4.2 | none: an identifier is ASCII letters, digits and `_`; a character outside ASCII is an error, and so is a universal character name, which the preprocessor writes as its UTF-8 | the same | `tests/error/identifier-outside-ascii.c` |
| J.3.4(2) | 5.2.5.2, 6.4.2 | all: the symbol is the whole name | the same | `tests/cost/long-names.c` |
| J.3.5(1) | 3.7 | 8 | the same | the `braces` probe |
| J.3.5(2) | 5.2.1 | ASCII for the basic character set, `'A'` is 65; past it, J.3.5(6) | the same | `tests/cost/volatile.c` |
| J.3.5(3) | 5.2.3 | `\a` 7, `\b` 8, `\f` 12, `\n` 10, `\r` 13, `\t` 9, `\v` 11 | the same | `tests/corpus/header-escapes.c` |
| J.3.5(4) | 6.2.5 | a byte of the character's UTF-8, read as plain `char`: `"é"[0]` is -61 | `"é"[0]` is 195 | `tests/cost/widen.c` for the read, `tests/cost/utf8-string.c` for the byte |
| J.3.5(5) | 6.2.5, 6.3.1.1 | `signed char` | `unsigned char` | `tests/cost/widen.c`, `tests/cost/string.c`, the `straight` probe |
| J.3.5(6) | 6.2.9, 6.4.4.5 | UTF-8: the text's bytes as written, `"é"` is `\303\251` | the same | `tests/cost/utf8-string.c`, and `tests/corpus/header-wide.c` by the size of `"é"` |
| J.3.5(7) | 6.2.9, 6.4.4.5 | UTF-32, a code point per element of `wchar_t`, which is `int`: `L'é'` is 233 | the same, of `wchar_t`, which is `unsigned int` | `tests/cost/wide-string.c`, `tests/corpus/header-wide.c` |
| J.3.5(8) | 6.4.4.5, 5.1.1.2 | the identity: the source is UTF-8, and a literal of no prefix keeps its bytes | the same | `tests/cost/utf8-string.c` |
| J.3.5(9) | 6.4.4.5 | none: two characters or more, `'ab'`, are not yet, and a character of more than one byte of UTF-8, `'é'`, is an error | the same | `tests/error/multi-character-constant.c`, `tests/error/character-too-large.c` |
| J.3.5(10) | 6.4.4.5 | none, an error: two characters, `L'ab'`; a character of two UTF-16 units after `u`, `u'😀'`; an escape past the element's width | the same | `tests/error/wide-character-multiple.c`, `tests/error/wide-character-too-large.c`, `tests/error/wide-escape-out-of-range.c` |
| J.3.5(11) | 6.4.4.5 | no locale: the source's UTF-8 decoded to the code point | the same | `tests/cost/wide-string.c` |
| J.3.5(12) | 6.4.5 | no locale: the source's UTF-8 decoded, a code point per element, and text that is no UTF-8 an error | the same | `tests/cost/wide-string.c`, `tests/error/wide-invalid-utf8.c` |
| J.3.5(13) | 6.4.5 | an escape is the byte it names, `"\xff"` is `\377`; a universal character name is its UTF-8 | the same | `tests/cost/global-string.c`, `tests/corpus/header-wide.c` |
| J.3.5(14) | 6.10.10.3 | UTF-32, as where the macro is defined: the C library's headers define it, so a unit that includes none has it undefined | the same | `tests/cost/wide-string.c`, whose one header, `stddef.h`, leaves it undefined |
| J.3.6(1) | 6.2.5 | none: `__int128` is not yet | the same | `tests/error/int128-local.c` |
| J.3.6(3) | 6.3.1.3 | G2 | G2 | the `memory` probe, the `fields` probe |
| J.3.6(4) | 6.5.1 | the bits of two's complement; `>>`, G3 | the same | `tests/cost/shift.c`, the `straight` probe |
| J.3.8(1) | 6.3.2.3 | the address as a 64-bit integer: to a narrower integer its low bits, from an integer extended by the integer's signedness | the same | the `memory` probe, `tests/cost/ptr-int.c` |
| J.3.8(2) | 6.5.7 | `long`, 64 bits | the same | `tests/cost/ptr-diff.c` |
| J.3.9(1) | 6.7.2 | five registers, `rbx` and `r12` to `r15`, taken by declaration; a variable past them takes a slot, with a warning (`cost.md` chapter 1) | ten, `x19` to `x28` | `tests/cost/register-rules.c`, `tests/error/warning-before-error.c` |
| J.3.9(2) | 6.7.5 | none: a `static inline` function is compiled as the static function it is, each call made; `inline` on a definition of external linkage is not yet | the same | `tests/cost/static-inline.c`, `tests/error/inline-definition.c` |
| J.3.10(1) | 6.7.3, 6.7.3.2 | signed: a plain `int` bit-field is a `signed int` one, as `clang` has it, so its read extends its sign (`shape.md` chapter 5) | the same | the `fields` probe, `tests/cost/bit-field-read.c` |
| J.3.10(2) | 6.7.3.2 | every integer type rcc has: `char`, `signed char`, `unsigned char`, `short`, `int`, `long` and `long long`, the unsigned types of the four, and an enumerated type | the same | the `layout` probe |
| J.3.10(3) | 6.7.3.2 | yes, as their plain types: `_Atomic` is dropped with a warning, so a bit-field of `_Atomic int` is an `int` bit-field (`shape.md` chapter 3) | the same | `tests/error/bit-field-atomic.c`, `tests/cost/atomic.c` |
| J.3.10(4) | 6.7.3.2 | no: a bit-field the rest of its container cannot hold starts the next container, the unit of its declared type, of that type's size at a multiple of it (`shape.md` chapter 3) | the same | the `layout` probe |
| J.3.10(5) | 6.7.3.2 | low-order to high-order: the first field of a container in its lowest bits, and each next field in the bits above (`shape.md` chapter 3) | the same | the `layout` probe, `tests/cost/bit-field-data.c` |
| J.3.10(6) | 6.7.3.2 | each member at its type's alignment, which for a scalar is its size, by `clang`'s rule (`shape.md` chapter 3) | the same | the `layout` probe, `tests/cost/struct-object.c` |
| J.3.10(7) | 6.7.3.3 | by its values, `unsigned int`, `unsigned long`, `int` or `long`, as `clang` chooses (`shape.md` chapter 5); under `packed`, not yet | the same | `tests/cost/enum.c`, the `enums` probe, `tests/error/enum-attribute.c` |
| J.3.11(1) | 6.7.4 | each load and each store the source writes, at the object's width, a bit-field's at its unit's width (`shape.md` chapter 5) | the same | `tests/cost/volatile.c`, `tests/cost/bit-field-volatile.c` |
| J.3.12(10) | 6.10.8 | `pack`: a record whose list opens while it sets a packing is not yet, and one after the packing is popped or reset is laid out (`shape.md` chapter 2); `weak` is not yet, refused at its name; `GCC visibility` is read, and a declaration of a symbol of external linkage under a visibility other than `default` is not yet, refused at its name; each other `#pragma` the preprocessor passes on is skipped | the same | `tests/error/pragma-pack.c`, `tests/error/pragma-pack-pop.c`, `tests/error/pragma-pack-name.c`, `tests/error/pragma-weak.c`, `tests/error/pragma-visibility.c` |
| J.3.14(3) | 6.2.6.1 | J.3.14(6)'s sizes, little-endian, two's complement: `0x01020304` is the bytes `04 03 02 01` | the same | the `memory` probe |
| J.3.14(6) | 6.5.4.4 | LP64: `short` 2, `int` 4, `long`, `long long` and a pointer 8, each aligned to its size; a record by J.3.10(6) | the same | the `layout` probe |
| none | 5.2.5.2 | a frame of at most 2 GiB less 16 bytes: a local larger is an error at its name, and locals that pass it together are an error that names the function, each ending `rcc's frame on x86-64 holds at most 2 GiB less 16 bytes` (`shape.md` chapter 7) | 16 MiB less 16 bytes, refused the same way | `tests/error/local-too-large.c` and `tests/error/frame-of-2-gib.c` on x86-64, `tests/error/local-of-16-mib.c` and `tests/error/frame-of-16-mib.c` on aarch64 |
| none | 6.2.7 | the composite of an enumerated type and its integer type is the integer type, whichever declaration comes first, as `clang` has it (`shape.md` chapter 3) | the same | `tests/corpus/compatible-types.c` |
