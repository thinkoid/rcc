/* The folds and the identities at both views, C6.3 of the survey: an
   operator on two constants is the constant, wrapped at the operation's
   width, so 0x7fffffffffffffff + 1 is the most negative long; a
   division by 0, and the most negative value divided by -1, is left to
   the instruction.  A constant that makes the operator the identity
   leaves the other operand, by its view: a << 32 is a on an int and
   shlq $32 on a long; l & -1 is l, and l & 4294967295 is no identity. */
long
folded_sum(void)
{
        return 0x7fffffffffffffff + 1;
}

long
folded_product(void)
{
        return 0x100000000 * 0x100000000;
}

int
folded_int_sum(void)
{
        return 2147483647 + 1;
}

long
folded_shift(void)
{
        return 1L << 40;
}

long
folded_shift_masked(void)
{
        return 1L << 65;
}

long
folded_right_shift(void)
{
        return -0x100000000 >> 4;
}

long
folded_quotient(void)
{
        return 0x123456789 / 3;
}

int
folded_comparison(void)
{
        return 0x100000000 < 0x123456789;
}

long
folded_negation(void)
{
        return -(-9223372036854775807 - 1);
}

long
folded_complement(void)
{
        return ~0x100000000;
}

long
division_by_zero(void)
{
        return 5L / 0;
}

long
most_negative_by_minus_one(void)
{
        return (-9223372036854775807 - 1) / -1;
}

int
shift_32_int(void)
{
        int a = 1;
        return a << 32;
}

long
shift_32_long(void)
{
        long a = 1;
        return a << 32;
}

long
shift_64_long(void)
{
        long a = 1;
        return a << 64;
}

long
identities(void)
{
        long l = 1;
        return (l + 0) + (l * 1) + (l & -1) + (l | 0) + (l ^ 0) + (l / 1) +
               (0 + l) + (1 * l);
}

long
no_identity(void)
{
        long l = 1;
        return l & 4294967295;
}

long
sum_of_zero(void)
{
        int i = 1;
        return i + 0L;
}
