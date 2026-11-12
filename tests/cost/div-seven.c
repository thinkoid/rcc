/* cost.md chapter 1, no folding: x / 7 is a division.

   On x86-64 the listing must show x loaded into eax, movl $7, %ecx,
   since idiv takes no immediate, cltd and idivl %ecx, and no multiply
   by a magic constant.

   On aarch64 the listing must show x loaded into w0, mov w1, #7, since
   sdiv takes no immediate, then sdiv w0, w0, w1, and no multiply by a
   magic constant. */
int div_seven(int x)
{
    return x / 7;
}
