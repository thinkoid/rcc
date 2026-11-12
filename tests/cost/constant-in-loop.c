/* cost.md chapter 1, no moving: a constant used in a loop is written in
   the loop.

   On x86-64 the listing must show the movabsq of the 64-bit constant
   inside the loop, after the test at the top and its jge, and not once
   before it.

   On aarch64 the listing must show the four instructions that form the
   64-bit constant, mov x1, #52719 and three movk, inside the loop,
   after the test at the top and its b.ge, and not once before it. */
long constant_in_loop(long s, int n)
{
    for (int i = 0; i < n; i = i + 1)
        s = s + 81985529216486895;
    return s;
}
