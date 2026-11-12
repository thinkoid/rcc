/* cost.md chapter 1, no folding: x / 4 is a division.

   On x86-64 the listing must show x loaded into eax, movl $4, %ecx,
   since idiv takes no immediate, then cltd and idivl %ecx, and no sar,
   shr or add of the sign.

   On aarch64 the listing must show x loaded into w0, mov w1, #4, since
   sdiv takes no immediate, then sdiv w0, w0, w1, and no asr, lsr or add
   of the sign. */
int div_four(int x)
{
    return x / 4;
}
