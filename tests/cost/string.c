/* backend.md chapter 10 and aarch64.md chapter 10, a string literal: a
   labeled array of bytes in .data, writable (G17).

   On x86-64 the listing must show the literal's address taken with
   leaq .Ls0(%rip), %rax, stored to s's slot and loaded back; the byte
   loaded and sign-extended in one movsbl 1(%rax), %eax, the constant
   index as the displacement; and the literal after the function, under
   .data, as .Ls0 and its .string, in no read-only section.

   On aarch64 the listing must show the literal's address formed with
   adrp x0, .Ls0 and add x0, x0, :lo12:.Ls0, stored to s's slot and
   loaded back; the byte loaded in one ldrb w0, [x0, #1], the constant
   index as the displacement and the load the unsigned one, since plain
   char is unsigned; and the literal after the function, under .data,
   as .Ls0 and its .string, in no read-only section. */
int second(void)
{
    char *s = "abc";

    return s[1];
}
