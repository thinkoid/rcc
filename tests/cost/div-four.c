/* Chapter 1, no folding: x / 4 is a division.  The listing must show
   the constant 4 in a register, cltd and idiv, and no sar, shr or
   add of the sign. */
int div_four(int x)
{
    return x / 4;
}
