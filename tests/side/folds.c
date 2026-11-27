/* Two constants fold where the node is built, as rcc folds them: the
   exact value wrapped to 32 bits, as the instruction wraps it, so
   2147483647 + 1 is -2147483648 and 65536 * 65536 is 0 with no
   diagnostic.  A quotient truncates toward zero and a remainder has
   the dividend's sign, -7 / 2 is -3 and -7 % 2 is -1, but a division
   that has no value is not folded: 1 / 0, 1 % 0 and the most negative
   int by -1 are their nodes, the instruction written, with no
   diagnostic.  A shift's count is taken modulo 32, as the instruction
   masks it, and >> of a negative value is arithmetic, so 1 << 33 is 2,
   1 << -1 is -2147483648 and -1 >> 40 is -1; 2 << 31 is 0, which
   draws nothing.  A fold is bottom-up and in the grammar's association:
   1 + 2 + a is (1 + 2) + a, folded to 3 + a, and a + 1 + 2 is
   (a + 1) + 2, two operations, as a * 2 * 3 is.  A folded constant is
   the operand a target's niche takes: a & (1 - 1) is a & 0, kept.

   Each function returns one expression of constants, or of a local
   and constants, its name what it shows. */
int
wraps_up(void)
{
        int a = 7;
        return 2147483647 + 1;
}

int
wraps_down(void)
{
        int a = 7;
        return (-2147483647 - 1) - 1;
}

int
difference(void)
{
        int a = 7;
        return 3 - 5;
}

int
and(void)
{
        int a = 7;
        return 6 & 3;
}

int
or(void)
{
        int a = 7;
        return 6 | 3;
}

int
xor(void)
{
        int a = 7;
        return 6 ^ 3;
}

int
negative_min(void)
{
        int a = 7;
        return -(-2147483647 - 1);
}

int
negative_sum(void)
{
        int a = 7;
        return -(5 + 2);
}

int
complement_of_xor(void)
{
        int a = 7;
        return ~(6 ^ 3);
}

int
left_first(void)
{
        int a = 7;
        return 1 + 2 + a;
}

int
right_kept(void)
{
        int a = 7;
        return a + 1 + 2;
}

int
to_zero(void)
{
        int a = 7;
        return a & (1 - 1);
}

int
product(void)
{
        int a = 7;
        return 6 * 3;
}

int
wraps_product(void)
{
        int a = 7;
        return 65536 * 65536;
}

int
negative_product(void)
{
        int a = 7;
        return -7 * 3;
}

int
product_of_sum(void)
{
        int a = 7;
        return (2 + 3) * 4;
}

int
product_kept(void)
{
        int a = 7;
        return a * 2 * 3;
}

int
shl_sign(void)
{
        int a = 7;
        return 1 << 31;
}

int
shl_past_sign(void)
{
        int a = 7;
        return 2 << 31;
}

int
shl_32(void)
{
        int a = 7;
        return 1 << 32;
}

int
shl_33(void)
{
        int a = 7;
        return 1 << 33;
}

int
shl_minus_one(void)
{
        int a = 7;
        return 1 << -1;
}

int
shr_negative(void)
{
        int a = 7;
        return -8 >> 1;
}

int
shr_40(void)
{
        int a = 7;
        return -1 >> 40;
}

int
shl_of_sum(void)
{
        int a = 7;
        return (1 + 2) << 4;
}

int
quotient(void)
{
        int a = 7;
        return 60 / 7;
}

int
quotient_negative(void)
{
        int a = 7;
        return -7 / 2;
}

int
remainder_negative(void)
{
        int a = 7;
        return -7 % 2;
}

int
remainder_by_negative(void)
{
        int a = 7;
        return 7 % -3;
}

int
by_zero(void)
{
        int a = 7;
        return 1 / 0;
}

int
remainder_by_zero(void)
{
        int a = 7;
        return 1 % 0;
}

int
zero_by_zero(void)
{
        int a = 7;
        return 0 / 0;
}

int
min_by_minus_one(void)
{
        int a = 7;
        return (-2147483647 - 1) / -1;
}

int
min_remainder_by_minus_one(void)
{
        int a = 7;
        return (-2147483647 - 1) % -1;
}

int
min_by_one(void)
{
        int a = 7;
        return (-2147483647 - 1) / 1;
}

int
by_zero_kept(void)
{
        int a = 7;
        return a + 1 / 0;
}
