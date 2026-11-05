/* Chapter 1, no moving; chapter 2, -disable-machine-licm.  A constant
   used in a loop is materialized in the loop.  The listing must show
   the movabs of the 64-bit constant inside the loop, after the test
   at the top, and not once before it; machine LICM would move it
   before the loop. */
long constant_in_loop(long s, int n)
{
    for (int i = 0; i < n; i = i + 1)
        s = s + 81985529216486895;
    return s;
}
