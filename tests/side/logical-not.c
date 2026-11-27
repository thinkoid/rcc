/* The logical negation !e, 6.5.4.3p5, as a value: an int, 1 where e is
   0 and 0 otherwise.  Its operand is loaded and tested against 0,
   cmpl $0, %eax and sete on x86-64, cmp w0, #0 and cset w0, eq on
   aarch64: a load is not compared in place, as it is in a == 0.  Each
   ! is a test of its own, so !!a writes two, sete after sete, and
   !(a < b) the setl of the comparison, then the test of its value.

   negation() is !a, double_negation() !!a, negated_comparison()
   !(a < b), negated_sum() ! of a sum, and equal_zero() a == 0, the
   comparison that !a is equivalent to. */
int
negation(void)
{
        int a = 3;
        return !a;
}

int
double_negation(void)
{
        int a = 3;
        return !!a;
}

int
negated_comparison(void)
{
        int a = 3;
        int b = 5;
        return !(a < b);
}

int
negated_sum(void)
{
        int a = 3;
        int b = 5;
        return !(a + b);
}

int
equal_zero(void)
{
        int a = 3;
        return a == 0;
}
