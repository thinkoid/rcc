/* backend.md chapter 4, register variables: the loop of for.c with
   register on the parameter, the sum and the counter.  The three take
   rbx, r12 and r13 in declaration order.  The listing must show the
   three pushed in the prologue and popped in reverse in the epilogue,
   n moved from edi into ebx, s and i set with movl $0, %r12d and
   movl $0, %r13d, the test cmpl %ebx, %r13d with both registers as
   operands, and each assignment computed in eax and moved back into
   its register; no operand off rbp at all, where for.c loads and
   stores the counter at every use. */
int sum(register int n)
{
    register int s = 0;
    for (register int i = 0; i < n; i = i + 1)
        s = s + i;
    return s;
}
