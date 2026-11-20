/* backend.md 6.2 and aarch64.md 6.2, depth and spill: a value held when
   no expression register is free is stored to a spill slot, the oldest
   first, and reloaded when the operation that consumes it comes.  Ten
   nested right operands over six parameters, each read where it is
   written, hold eleven values at once.

   On x86-64, whose nine expression registers run out, the listing must
   show the first ten loads in source order, the oldest held value, the
   first a, stored with movq to a spill slot when the tenth needs a
   register, the eleventh operand, the last e, the operand of the first
   subtraction, subl -20(%rbp), %eax (backend.md 6.1), and a reloaded
   with movq when the subtraction that consumes it comes.

   On aarch64 the listing is the eleven loads in source order into w0
   to w7 and w9 to w11, then the ten subtractions, with no spill slot,
   since fifteen expression registers hold eleven values; the row is not
   reached there, and spill-fifteen.c reaches it. */
int
spill(int a, int b, int c, int d, int e, int f)
{
        return a - (b - (c - (d - (e - (f - (a - (b - (c - (d - e)))))))));
}
