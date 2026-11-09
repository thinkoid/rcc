/* cost.md chapter 1, no merging: the same expression written twice is
   computed twice.  The listing of twice() must show two loads of a,
   two of b, two imull and the addl %ecx, %eax of the two products.
   In twice_constant(), 12 * 34 is an integer constant expression,
   folded in the parser to 408 (backend.md 6.4): the listing must show
   two addl $408, %eax, one per product written, and no imul. */
int twice(int a, int b)
{
    return a * b + a * b;
}

int twice_constant(int a)
{
    return a + 12 * 34 + 12 * 34;
}
