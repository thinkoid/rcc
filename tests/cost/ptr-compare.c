/* backend.md 6.3 and aarch64.md 6.3, p < q: the addresses compared
   unsigned (G7).

   On x86-64 the listing must show the first address loaded with movq,
   one cmpq -16(%rbp), %rax of it and the second in its slot, at 64
   bits, and setb, the unsigned condition, not setl, widened with
   movzbl %al, %eax.

   On aarch64 the listing must show the two addresses loaded into x0
   and x1, one cmp x0, x1 of the 64-bit registers, and cset w0, lo, the
   unsigned condition, not lt. */
int less(int *p, int *q)
{
    return p < q;
}
