/* cost.md chapter 1, no merging: the same expression written twice is
   computed twice.  In twice_constant(), 12 * 34 is an integer constant
   expression, folded in the parser to 408 (backend.md 6.4 and
   aarch64.md 6.4), and each of the two is added.

   On x86-64 the listing of twice() must show two loads of a, two of b,
   two imull and the addl %ecx, %eax of the two products; and of
   twice_constant(), two addl $408, %eax and no imul.

   On aarch64 the listing of twice() must show two loads of a, two of b,
   two mul and the add w0, w0, w1 of the two products; and of
   twice_constant(), two add w0, w0, #408 and no mul. */
int twice(int a, int b)
{
    return a * b + a * b;
}

int twice_constant(int a)
{
    return a + 12 * 34 + 12 * 34;
}
