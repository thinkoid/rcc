/* backend.md chapter 4 and aarch64.md chapter 4, register variables:
   the loop of for.c with register on the parameter, the sum and the
   counter.  The three take the first three register-variable registers
   in declaration order.

   On x86-64 the three are rbx, r12 and r13, and the listing must show
   them pushed in the prologue and popped in reverse in the epilogue, n
   moved from edi into ebx, s and i set with movl $0, %r12d and
   movl $0, %r13d, the test cmpl %ebx, %r13d with both registers as
   operands, and each assignment computed in eax and moved back into its
   register; no operand off rbp at all, where for.c loads and stores the
   counter at every use.

   On aarch64 the three are x19, x20 and x21, and the listing must show
   them stored to the save area by stp x19, x20 and str x21 and loaded
   back by ldp and ldr, n moved from w0 into w19, s and i set with
   mov w20, #0 and mov w21, #0, the test cmp w21, w19 with both
   registers as operands, and each assignment computed into w0 with no
   copy, add w0, w20, w21 and add w0, w21, #1, and moved back into its
   register; no slot at all. */
int sum(register int n)
{
    register int s = 0;
    for (register int i = 0; i < n; i = i + 1)
        s = s + i;
    return s;
}
