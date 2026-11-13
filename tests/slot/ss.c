/* The slot's cost, ss: rr.c with the counter i and the sum s both in
   stack slots.  The loop carries both through memory: each iteration
   loads the sum, adds i and stores it back, stores the incremented
   counter, and the next loads both again.  The exit status is the
   sum's second byte, 155, as rr.c explains. */
int
main(void)
{
    int i;
    unsigned s = 0;
    for (i = 0; i != 1000000000; ++i)
        s += i;
    return (s >> 8) & 255;
}
