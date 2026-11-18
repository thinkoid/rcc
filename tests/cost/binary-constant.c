/* A binary constant is the number it spells, 6.4.4.2p4, p5, of the
   type the list of octal and hexadecimal constants gives it, p6: the
   prefix 0b is a spelling, as 0x is, and costs nothing.  low() masks
   with an int, high() compares with an unsigned int, 32 ones, and
   wide() adds an unsigned long.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with 0xa, 0xffffffff and 0x100000000ul
   for its binary constants.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
int
low(int x)
{
        return x & 0b1010;
}

int
high(unsigned x)
{
        return x < 0b11111111111111111111111111111111;
}

unsigned long
wide(unsigned long x)
{
        return x + 0b100000000000000000000000000000000ul;
}
