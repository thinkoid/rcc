/* backend.md 6.2 and aarch64.md 6.2, depth and spill: a value held
   when no expression register is free is stored to a spill slot, the
   oldest first, and reloaded when the operation that consumes it
   comes.  Sixteen nested right operands over eight parameters, each
   read where it is written, hold seventeen values at once, more than
   either target has expression registers.

   On x86-64 the listing must show the seven oldest held values, the
   first a to g, stored with movq to spill slots from -32(%rbp) down as
   the tenth to the sixteenth loads need a register, the seventeenth
   operand, the last a, the operand of the first subtraction,
   subl -4(%rbp), %r9d (backend.md 6.1), and each held value reloaded
   with movq, into rax or rcx, when the subtraction that consumes it
   comes.

   On aarch64 the listing must show the seventeen loads in source
   order into w0 to w7 and w9 to w15, x8 left out; the two oldest held
   values, the first a and the first b, stored with str x0, [x29, #48]
   and str x1, [x29, #56] when the sixteenth and the seventeenth need
   a register; and each reloaded with ldr, into x0 or x1, when the
   subtraction that consumes it comes. */
int spill_fifteen(int a, int b, int c, int d, int e, int f, int g, int h)
{
    return a - (b - (c - (d - (e - (f - (g - (h - (a - (b - (c - (d - (e -
           (f - (g - (h - a)))))))))))))));
}
