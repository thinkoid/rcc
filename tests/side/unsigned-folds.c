/* The folds of the unsigned operators, on the bits modulo 2^N: a
   division of the most negative value by -1 is left to the instruction
   signed, and folded unsigned, 0x80000000 / -1 being 0; a right shift
   is logical; a comparison is of the bits, so -1 < 0u is 0.  The
   identities are the signed ones': u / 1 and u >> 32 are u. */
unsigned
quotient_top_bit(void)
{
        return 0x80000000 / -1;
}

int
signed_quotient_top_bit(void)
{
        return (-2147483647 - 1) / -1;
}

unsigned long
long_quotient_top_bit(void)
{
        return 0x8000000000000000 / -1;
}

unsigned
remainder(void)
{
        return 4294967295u % 10u;
}

unsigned
right_shift(void)
{
        return 0x80000000 >> 4;
}

unsigned long
long_right_shift(void)
{
        return 0xffffffffffffffff >> 60;
}

int
comparison(void)
{
        return -1 < 0u;
}

int
long_comparison(void)
{
        return 0xffffffffffffffff > 1;
}

unsigned
wrapped(void)
{
        return 4294967295u + 2u;
}

unsigned
division_by_zero(void)
{
        return 5u / 0u;
}

unsigned
identities(void)
{
        unsigned u = 7;
        return (u / 1) + (u >> 32) + (u & 4294967295u);
}
