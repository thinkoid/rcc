/* backend.md chapter 8 and aarch64.md chapter 8, f(a, b): each argument
   a hidden local of the call, stored to a slot of its own as it is
   evaluated and loaded from the slot into its argument register for the
   call.

   On x86-64 the listing must show the subq $16, %rsp of a function that
   calls, the parameters stored to their slots, b and then a each loaded
   into eax and stored to an argument slot of its own, -12(%rbp)
   and -16(%rbp), those two slots loaded into edi and esi, call f@PLT,
   and the result left in eax.

   On aarch64 the listing must show the parameters stored to their
   slots, b and then a each loaded into w0 and stored to an argument
   slot of its own, [x29, #24] and [x29, #28], those two slots loaded
   into w0 and w1, bl f, and the result left in w0. */
int f(int a, int b);

int call(int a, int b)
{
    return f(b, a);
}
