/* cost.md chapter 1, no folding: x / 4 is a division.  The listing
   must show x loaded into eax, movl $4, %ecx, since idiv takes no
   immediate, then cltd and idivl %ecx, and no sar, shr or add of the
   sign. */
int div_four(int x)
{
    return x / 4;
}
