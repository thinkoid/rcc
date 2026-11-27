/* The constants whose type is long, 6.4.4.2p6: a decimal one past
   int's range, an octal or hexadecimal one past unsigned's, and any
   with l, L, ll or LL that a long holds.  The value is what the
   register holds: a constant of 64 bits in int's range is movq $c,
   %rax, and one past it movabsq on x86-64; aarch64 writes it by mov,
   movk or a bitmask's mov.  -2147483648 is the negation of the long
   2147483648, folded, a long in int's range. */
long
decimal_int_max(void)
{
        return 2147483647;
}

long
decimal_past_int(void)
{
        return 2147483648;
}

long
negative_int_min(void)
{
        return -2147483648;
}

long
decimal_past_unsigned(void)
{
        return 4294967296;
}

long
decimal_unsigned_max(void)
{
        return 4294967295;
}

long
decimal_long_max(void)
{
        return 9223372036854775807;
}

long
negative_long_max(void)
{
        return -9223372036854775807;
}

long
hexadecimal(void)
{
        return 0x123456789;
}

long
hexadecimal_past_unsigned(void)
{
        return 0x100000000;
}

long
hexadecimal_long_max(void)
{
        return 0x7fffffffffffffff;
}

long
octal_past_unsigned(void)
{
        return 040000000000;
}

long
binary_past_unsigned(void)
{
        return 0b100000000000000000000000000000000;
}

long
suffix_l(void)
{
        return 5l;
}

long
suffix_upper_l(void)
{
        return 5L;
}

long
suffix_ll(void)
{
        return 5ll;
}

long
suffix_upper_ll(void)
{
        return 5LL;
}

long
suffix_l_hexadecimal(void)
{
        return 0x80000000l;
}

long
suffix_l_int_max(void)
{
        return 2147483647L;
}

long
suffix_l_minus_one(void)
{
        return -1L;
}

long
zero(void)
{
        return 0L;
}

int
compared_past_int(void)
{
        int x = 1;
        return x < 2147483648;
}

int
compared_long_max(void)
{
        int x = 1;
        return x < 9223372036854775807;
}

int
compared_suffix(void)
{
        int x = 1;
        return x < 5l;
}

int
compared_hexadecimal(void)
{
        int x = 1;
        return x < 0x100000000;
}
