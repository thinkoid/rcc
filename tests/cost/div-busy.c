/* backend.md 6.5 and aarch64.md 6.5, the pinned instructions: a
   division that wants its operands in fixed registers first moves out
   the values held there.  In a + b / c, a is held when the division
   comes and c is loaded last.

   On x86-64, where the held a sits in eax and c is loaded into edx, the
   listing must show, in each function, the two moves out of the pinned
   registers, c from rdx and a from rax, each to a free register, then b
   moved into eax, cltd, idivl, the quotient (eax) or the remainder
   (edx) moved into b's register, and the add into a's new register.

   On aarch64 the listing is the division on the registers the loads
   took, a in w0, b in w1 and c in w2: sdiv w1, w1, w2 in div_busy(),
   sdiv w3, w1, w2 and msub w1, w3, w2, w1 in rem_busy(), then
   add w0, w0, w1, and no move, since no instruction pins a register;
   the row is not reached there. */
int div_busy(int a, int b, int c)
{
    return a + b / c;
}

int rem_busy(int a, int b, int c)
{
    return a + b % c;
}
