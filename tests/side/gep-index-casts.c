/* An index of an object under a cast to a type of its width of the
   other sign, which aarch64's index form reads as the cast, its value
   extended again by the cast's sign: p[(unsigned char) c] on a signed
   char c is ldrsb w1, uxtb w1, w1 and x1, lsl #2, p[(signed char) u]
   ldrb w2, sxtb w2, w2 and w2, sxtw #2, and p[(unsigned) i] ldr w1 and
   w1, uxtw #2. */
int
narrow(int *p, signed char c, unsigned char u, short s, unsigned short us)
{
        return p[(unsigned char) c] + p[(signed char) u] +
               p[(unsigned short) s] + p[(short) us];
}

int
wide(int *p, int i, unsigned u, long l, unsigned long ul)
{
        return p[(unsigned) i] + p[(int) u] + p[(unsigned long) l] +
               p[(long) ul];
}

char
through_char(char *p, signed char c, int i)
{
        return p[(unsigned char) c] + p[(unsigned) i] + *(p + (unsigned) i);
}

long
values(long *p, signed char c, int i)
{
        return *(p - (unsigned char) c) + *((unsigned) i + p) +
               (p + (unsigned short) c)[1];
}
