/* A function that returns a constant of type int: the constant row at
   32 bits, then the jump to the one epilogue, which every return
   writes, the last one included.

   zero() and small() are one mov on both targets; high() is 16 bits
   that are not the lowest, one mov on aarch64; chunks() is two chunks
   over zeros, a mov and a movk; largest() is 2147483647, one mov over
   ones; bitmask() is 0x00ff00ff, a bitmask, one mov written in hex;
   octal(), hexadecimal() and binary() are spellings of small values in
   the other bases.  twice() returns twice, each a mov and a jump, and
   main() returns a constant, so it writes no return of 0. */
int
zero(void)
{
        return 0;
}

int
small(void)
{
        return 7;
}

int
high(void)
{
        return 65536;
}

int
chunks(void)
{
        return 305419896;
}

int
largest(void)
{
        return 2147483647;
}

int
bitmask(void)
{
        return 0x00ff00ff;
}

int
octal(void)
{
        return 0755;
}

int
hexadecimal(void)
{
        return 0x7F;
}

int
binary(void)
{
        return 0b101;
}

int
twice(void)
{
        return 1;
        return 2;
}

int
main(void)
{
        return 3;
}
