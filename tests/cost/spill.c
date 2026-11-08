/* backend.md 6.2, depth and spill: ten nested right operands over six
   parameters, each read where it is written, so eleven values are
   held at once and the nine expression registers run out.  The
   listing must show the eleven loads in source order, the oldest held
   values, the first a and then the first b, stored with movq to spill
   slots when the tenth and the eleventh need a register, and each
   reloaded with movq when the subtraction that consumes it comes. */
int spill(int a, int b, int c, int d, int e, int f)
{
    return a - (b - (c - (d - (e - (f - (a - (b - (c - (d - e)))))))));
}
