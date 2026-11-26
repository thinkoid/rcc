/* Two constants fold where the node is built, as rcc folds them: the
   exact value wrapped to 32 bits, as the instruction wraps it, so
   2147483647 + 1 is -2147483648 with no diagnostic.  A fold is
   bottom-up and in the grammar's association: 1 + 2 + a is (1 + 2) + a,
   folded to 3 + a, and a + 1 + 2 is (a + 1) + 2, two operations.  A
   folded constant is the operand a target's niche takes: a & (1 - 1)
   is a & 0, kept.

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
