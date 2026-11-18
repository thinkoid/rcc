/* aarch64.md chapter 8, an outgoing area past an immediate's reach:
   521 arguments, 513 of them past the eighth, make an area of 4112
   bytes, which the sub opens in two parts, as the frame is (5.2).  The
   last stacked argument, a signed char, is 4096 bytes above sp, past
   the 4095 bytes a strb's offset reaches, so its store goes through
   x16 formed from sp.

   On x86-64 the listing must show the arguments past the sixth pushed,
   the last first, and no outgoing area.

   On aarch64 the listing must show sub sp, sp, #4096 and
   sub sp, sp, #16 before the copies, the last copy's
   add x16, sp, #4096 and strb w17, [x16], the others to [sp, #8k], and
   after the bl add sp, sp, #4096 and add sp, sp, #16. */
#define L8   long, long, long, long, long, long, long, long
#define L64  L8, L8, L8, L8, L8, L8, L8, L8
#define L512 L64, L64, L64, L64, L64, L64, L64, L64

#define A8   0, 0, 0, 0, 0, 0, 0, 0
#define A64  A8, A8, A8, A8, A8, A8, A8, A8
#define A512 A64, A64, A64, A64, A64, A64, A64, A64

long f(L8, L512, signed char);

long
outgoing(void)
{
        return f(A8, A512, 1);
}
