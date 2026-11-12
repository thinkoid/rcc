/* backend.md chapter 10 and aarch64.md chapter 10, a string literal: a
   labeled array of bytes in .data, writable (G17).

   On x86-64 the listing must show the literal's address taken with
   leaq .Ls0(%rip), %rax, stored to s's slot and loaded back; the byte
   loaded and sign-extended in one movsbl 1(%rax), %eax, the constant
   index as the displacement; and the literal after the function, under
   .data, as .Ls0 and its .string, in no read-only section. */
int second(void)
{
    char *s = "abc";

    return s[1];
}
