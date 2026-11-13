/* The slot's cost, rr: s += i a thousand million times, the counter i
   and the sum s both register variables.  The loop carries both from
   one iteration to the next in registers and nothing through memory:
   this is the baseline for sr.c, rs.c and ss.c.  The sum is unsigned,
   since it overflows an int; modulo 2^32 it is 3051657984,
   0xb5e49b00.  Its low byte is 0, as is that of a sum never added to,
   so the exit status is its second byte, 155. */
int
main(void)
{
    register int i;
    register unsigned s = 0;
    for (i = 0; i != 1000000000; ++i)
        s += i;
    return (s >> 8) & 255;
}
