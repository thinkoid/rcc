/* A constant that makes an operator the identity leaves the other
   operand, which is still evaluated: x + 0, x - 0, x | 0, x ^ 0 and
   x & -1 on the right, and on the left for the operators that commute,
   0 + x, 0 | x, 0 ^ x and -1 & x.  rcc drops them after the body is
   parsed and rcc-v2 where the node would be built; the listings are
   the same, (a + 0) + 0 and -(a + 0) among them.  0 - x, x & 0,
   x | -1, x ^ -1 and x - x are kept, and so is (a + 1) - 1, which no
   fold reaches.  A dropped identity on the right of another operator
   leaves a local there, the operand in memory on x86-64.

   Each function returns a local under one operator and a constant, its
   name what it shows. */
int
add_zero(void)
{
        int a = 7;
        return a + 0;
}

int
zero_add(void)
{
        int a = 7;
        return 0 + a;
}

int
sub_zero(void)
{
        int a = 7;
        return a - 0;
}

int
zero_sub(void)
{
        int a = 7;
        return 0 - a;
}

int
and_zero(void)
{
        int a = 7;
        return a & 0;
}

int
zero_and(void)
{
        int a = 7;
        return 0 & a;
}

int
and_all(void)
{
        int a = 7;
        return a & -1;
}

int
all_and(void)
{
        int a = 7;
        return -1 & a;
}

int
or_zero(void)
{
        int a = 7;
        return a | 0;
}

int
zero_or(void)
{
        int a = 7;
        return 0 | a;
}

int
xor_zero(void)
{
        int a = 7;
        return a ^ 0;
}

int
zero_xor(void)
{
        int a = 7;
        return 0 ^ a;
}

int
or_all(void)
{
        int a = 7;
        return a | -1;
}

int
xor_all(void)
{
        int a = 7;
        return a ^ -1;
}

int
itself(void)
{
        int a = 7;
        return a - a;
}

int
twice(void)
{
        int a = 7;
        return (a + 0) + 0;
}

int
under_negation(void)
{
        int a = 7;
        return -(a + 0);
}

int
parenthesized(void)
{
        int a = 7;
        return a + (0);
}

int
folded_zero(void)
{
        int a = 7;
        return a + (1 - 1);
}

int
negative_zero(void)
{
        int a = 7;
        return a + -0;
}

int
not_folded(void)
{
        int a = 7;
        return a + 1 - 1;
}

int
right_operand(void)
{
        int a = 7;
        return 7 - (a + 0);
}
