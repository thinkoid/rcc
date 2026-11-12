/* backend.md 6.3 and aarch64.md 6.3, a[i] by a long index, beside
   subscript's int: on x86-64 a long index is used as it is, with no
   widening, and on aarch64 an int index and a long one cost the same,
   the operand reading either.

   On x86-64 the listing must show, for the store and again for the
   load, the array's address taken with leaq -28(%rbp), %rax and i
   loaded with movq -8(%rbp), %rcx and no movslq; then the store
   movl %edx, (%rax,%rcx,4) and the load movl (%rax,%rcx,4), %eax.

   On aarch64 the listing must show, for the store and again for the
   load, the array's address taken with add x0, x29, #28 and i loaded
   with ldr x1; then the store str w2, [x0, x1, lsl #2] and the load
   ldr w0, [x0, x1, lsl #2], lsl where subscript has sxtw, and as many
   instructions as subscript. */
int subscript(long i, int v)
{
    int a[4];

    a[i] = v;
    return a[i];
}
