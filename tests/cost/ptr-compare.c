/* backend.md 6.3, p < q: the addresses compared unsigned (G7).  The
   listing must show the two addresses loaded with movq, one
   cmpq %rcx, %rax of the 64-bit registers, and setb, the unsigned
   condition, not setl, widened with movzbl %al, %eax. */
int less(int *p, int *q)
{
    return p < q;
}
