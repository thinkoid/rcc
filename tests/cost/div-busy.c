/* backend.md 6.5, the pinned instructions: a division whose dividend
   is not in eax and whose divisor is in edx.  In a + b / c the held a
   sits in eax and c is loaded into edx when the division comes.  The
   listing must show, in each function, the two moves out of the
   pinned registers, c from rdx and a from rax, each to a free
   register, then b moved into eax, cltd, idivl, the quotient (eax) or
   the remainder (edx) moved into b's register, and the add into a's
   new register. */
int div_busy(int a, int b, int c)
{
    return a + b / c;
}

int rem_busy(int a, int b, int c)
{
    return a + b % c;
}
