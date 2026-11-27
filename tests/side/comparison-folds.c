/* A comparison of two constants folds to 1 or 0 where it is built, and
   ! of a constant to 1 or 0, as rcc folds them; the folds compose
   bottom-up, so 3 < 2 + 2 compares 3 with 4 and !(1 < 2) is !1, 0.
   No comparison is an identity: a constant beside a local leaves the
   node.  A folded comparison is the constant a target's niche takes:
   (1 < 2) + a is a + 1, addl $1, %eax.

   Each function returns one expression of constants, or of a local and
   constants, its name what it shows. */
int
less_true(void)
{
        int a = 7;
        return 1 < 2;
}

int
less_negative(void)
{
        int a = 7;
        return -1 < 0;
}

int
less_sum(void)
{
        int a = 7;
        return 3 < 2 + 2;
}

int
equal_chain(void)
{
        int a = 7;
        return 1 == 1 == 1;
}

int
greater_false(void)
{
        int a = 7;
        return 1 > 2;
}

int
negated_comparison(void)
{
        int a = 7;
        return !(1 < 2);
}

int
negated_zero(void)
{
        int a = 7;
        return !0;
}

int
negated_twice(void)
{
        int a = 7;
        return !!5;
}

int
negated_constant(void)
{
        int a = 7;
        return !5;
}

int
folded_operand(void)
{
        int a = 7;
        return (1 < 2) + a;
}

int
no_identity(void)
{
        int a = 7;
        return (a == 0) + (a != 0) + (a < 0) + (0 < a);
}
