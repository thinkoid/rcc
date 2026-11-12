/* backend.md 6.3 and aarch64.md 6.1 and 6.3, the remainder whose left
   operand is a register variable's read: the variable is not written,
   and the result is in the lowest free register.

   On x86-64 the listing must show, in each function, b + 1 in ecx, a
   copied from its register into eax, where the division wants it,
   movl %ebx, %eax, then cltd and idivl %ecx in rem(), xorl %edx, %edx
   and divl %ecx in urem(), and the remainder moved from edx,
   movl %edx, %eax.

   On aarch64 the listing must show, in each function, b + 1 in w0, the
   quotient in a register taken for it, sdiv w1, w19, w0 in rem() and
   udiv w1, w19, w0 in urem(), then msub w0, w1, w0, w19: msub is the
   last instruction to read the right operand, so its result takes that
   operand's register, w19 is read twice and not written, and no mov
   follows before the branch to the epilogue. */
int rem(register int a, int b)
{
    return a % (b + 1);
}

unsigned urem(register unsigned a, unsigned b)
{
    return a % (b + 1);
}
