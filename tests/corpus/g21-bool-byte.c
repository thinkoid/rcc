/* G21: a _Bool reads as its byte.  A byte of 2 written into a _Bool
   through an unsigned char *, which 6.2.6.1p5 leaves undefined to read,
   reads as 2 under rcc: 2 in arithmetic, true in a condition and
   unequal to 1; b-- yields the 2 it read and stores (_Bool) (2 - 1),
   1.  The exit status is the sum of 32 for the read, 4 for the
   condition, 2 for the inequality and 1 for the byte b-- stores: 39.
   It is 101 where b-- yields another value than 2: clang -O2 does not
   read a _Bool as its byte, and it exits 101. */
_Bool b;

int
main(void)
{
        unsigned char *bytes = (unsigned char *) &b;
        int status = 0;

        *bytes = 2;

        status += 16 * b;

        if (b)
                status += 4;

        if (b != 1)
                status += 2;

        if (b-- != 2)
                return 101;

        status += *bytes;

        return status;
}
