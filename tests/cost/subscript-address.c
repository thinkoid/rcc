/* backend.md 6.3 and aarch64.md 6.3, &p[i]: the address a subscript
   designates, taken as a value and no access made (6.5.4.2p3).  With a
   constant index of 0 the address is the bare base, as *p's is, so
   &p[0] is p and no instruction.  Any other index is one instruction.

   On x86-64 the listing must show zero() returning p as it was loaded,
   with no instruction for the address, and third() and at() each with
   one lea of the addressing form: leaq 12(%rax), %rax, and
   leaq (%rax,%rcx,4), %rax after the index is widened with movslq.

   On aarch64 the listing must show zero() returning p as it was
   loaded, with no instruction for the address, and third() and at()
   each with one add, the machine having no lea: add x0, x0, #12, and
   add x0, x0, w1, sxtw #2, the operand extending and scaling the
   index. */
int *zero(int *p)
{
    return &p[0];
}

int *third(int *p)
{
    return &p[3];
}

int *at(int *p, int i)
{
    return &p[i];
}
