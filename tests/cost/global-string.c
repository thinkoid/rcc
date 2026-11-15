/* backend.md chapter 10 and aarch64.md chapter 10, the data of an
   array of char that a string literal initializes, 6.7.11p15: the
   array's own bytes under its own label, in the escapes of the
   literals' block, and no literal of its own.  .string appends the
   terminating zero when the array has room for it, .ascii none when
   the literal fills the array, and .zero writes the elements past the
   literal's.  pointer, beside them, is a pointer to a literal, two
   objects: the literal in the literals' block and the pointer in .data.

   On x86-64 and on aarch64 the listing is the same text but for one
   line: the literals' block, .Ls0 as .string "abc"; then .data, each
   array under .size and no alignment: whole, of unknown size, as
   .size whole, 4 and .string "abc"; padded as .string "abc" and
   .zero 4; full as .ascii "abc"; braced, signed_ and unsigned_ as
   .string "abc"; escaped as .string "a\tb\n\"\\\001\377"; pointer as
   .p2align 3 and .quad .Ls0; and long_, of 22 bytes, as
   .string "sixteen or more bytes", under .p2align 4 on x86-64, which
   the psABI asks of an array of 16 bytes or more, and under none on
   aarch64. */
char whole[] = "abc";
char padded[8] = "abc";
char full[3] = "abc";
char braced[] = { "abc" };
signed char signed_[] = "abc";
unsigned char unsigned_[4] = "abc";
char escaped[] = "a\tb\n\"\\\001\377";
char *pointer = "abc";
char long_[] = "sixteen or more bytes";
