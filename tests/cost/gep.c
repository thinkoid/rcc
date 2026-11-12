/* backend.md 6.3 and aarch64.md 6.3, p + n for an element of 4 bytes:
   one instruction forms the sum, the index scaled by 4 within it.
   a + i is stored to a pointer variable and feeds no load.

   On x86-64 the listing must show the array's address taken with
   leaq -20(%rbp), %rax, i loaded and widened with movslq %ecx, %rcx,
   the sum formed by the lea of the addressing form,
   leaq (%rax,%rcx,4), %rax, and the sum stored to p's slot with
   movq.

   On aarch64 the listing must show the array's address taken with
   add x0, x29, #20, i loaded with ldr w1 and not widened, the sum
   formed by add x0, x0, w1, sxtw #2, the operand extending and scaling
   the index, and the sum stored to p's slot with str x0. */
int *gep(int i)
{
    int a[4];
    int *p;

    p = a + i;
    return p;
}
