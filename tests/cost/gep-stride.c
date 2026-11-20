/* backend.md 6.3 and aarch64.md 6.3, m[i] as a pointer, of a size that
   is not a scale: the index multiplied by the size, then added.  m[i]
   on int m[2][3] is a row of 12 bytes, taken as a pointer.  On x86-64
   the product is the addressing form's index at scale 1, and the
   array's slot its displacement off rbp.

   On x86-64 the listing must show the index loaded and widened by one
   movslq -4(%rbp), %rax, scaled by imulq $12, %rax, %rax and added to
   the array's slot by the one leaq -28(%rbp,%rax,1), %rax, with no
   leaq of the array's address first.

   On aarch64 the listing must show the array's address from
   add x0, x29, #20, the index loaded and widened by one
   ldrsw x1, [x29, #16] before the multiply, the size held with
   mov x2, #12, since mul takes no immediate, the index scaled by
   mul x1, x1, x2 and added with add x0, x0, x1. */
int *
row(int i)
{
        int m[2][3];
        int *p;

        p = m[i];
        return p;
}
