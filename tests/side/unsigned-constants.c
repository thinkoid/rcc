/* The constants of the unsigned types, 6.4.4.2p6: u or U, and an octal
   or hexadecimal one past int's range, 0x80000000 an unsigned and
   0xffffffffffffffff an unsigned long.  A constant's value is what its
   register holds, so the unsigned 4294967295 is -1, movl $-1, %eax and
   cmpl $-1, -4(%rbp) with setb; -1u is the negation of 1u, folded to
   that. */
unsigned
unsigned_max(void)
{
        return 4294967295u;
}

unsigned
minus_one(void)
{
        return -1u;
}

int
compared_hexadecimal(void)
{
        int x = 1;
        return x < 0x80000000;
}

int
compared_unsigned_max(void)
{
        int x = 1;
        return x < 0xffffffff;
}

int
compared_octal(void)
{
        int x = 1;
        return x < 020000000000;
}

int
compared_suffix(void)
{
        int x = 1;
        return x < 5u;
}

int
compared_unsigned_long(void)
{
        int x = 1;
        return x < 5ul;
}

int
compared_unsigned_long_max(void)
{
        int x = 1;
        return x < 0xffffffffffffffff;
}

int
compared_top_bit(void)
{
        int x = 1;
        return x < 0x8000000000000000;
}

unsigned long
unsigned_long_max(void)
{
        return 0xffffffffffffffff;
}

unsigned long
top_bit(void)
{
        return 9223372036854775808u;
}

long
widened(void)
{
        return 4294967295u;
}

long
widened_past_unsigned(void)
{
        return 4294967296u;
}

int
stores(void)
{
        unsigned x;
        unsigned long y;
        x = 4294967295;
        x = 0x123456789;
        x = 0x5555555555555555;
        y = 0x8000000000000000;
        y = 0;
        y = 4294967295u;
        return 0;
}
