/* The slot's cost, rs: rr.c with the counter i a register variable
   and the sum s in a stack slot.  The loop carries s through memory:
   each iteration loads the sum from its slot, adds i and stores it
   back, and the next loads it again.  The exit status is the sum's
   second byte, 155, as rr.c explains. */
int
main(void)
{
    register int i;
    unsigned s = 0;
    for (i = 0; i != 1000000000; ++i)
        s += i;
    return (s >> 8) & 255;
}
