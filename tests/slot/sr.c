/* The slot's cost, sr: rr.c with the counter i in a stack slot and
   the sum s a register variable.  The loop carries i through memory:
   each iteration stores the incremented counter to its slot, and the
   next loads it back to test it.  The exit status is the sum's second
   byte, 155, as rr.c explains. */
int
main(void)
{
    int i;
    register unsigned s = 0;
    for (i = 0; i != 1000000000; ++i)
        s += i;
    return (s >> 8) & 255;
}
