/* cost.md chapter 1, no folding: x / 7 is a division.  The listing
   must show x loaded into eax, movl $7, %ecx, since idiv takes no
   immediate, cltd and idivl %ecx, and no multiply by a magic
   constant. */
int div_seven(int x)
{
    return x / 7;
}
