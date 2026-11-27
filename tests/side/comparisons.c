/* The six comparisons as values, 6.5.9, 6.5.10, each an int of 1 or
   0, on each pair of operand classes: two locals, a local and a
   constant, the constant on the left, and 0 on the right.  x86-64
   compares a local in memory, cmpl -8(%rbp), %eax, or in place against
   an immediate, cmpl $5, -4(%rbp), then setcc into the low byte and
   movzbl; aarch64 loads both, cmp w0, w1 or cmp w0, #5, then cset.  A
   constant on the left is the comparison mirrored, 5 < a as a > 5, so
   it writes the mirrored operator's setcc and cset, setg and gt.

   Each function returns one comparison, its name the operator and the
   operands' classes. */
int
less_locals(void)
{
        int a = 3;
        int b = 5;
        return a < b;
}

int
less_constant(void)
{
        int a = 3;
        int b = 5;
        return a < 5;
}

int
less_constant_left(void)
{
        int a = 3;
        int b = 5;
        return 5 < a;
}

int
less_zero(void)
{
        int a = 3;
        int b = 5;
        return a < 0;
}

int
greater_locals(void)
{
        int a = 3;
        int b = 5;
        return a > b;
}

int
greater_constant(void)
{
        int a = 3;
        int b = 5;
        return a > 5;
}

int
greater_constant_left(void)
{
        int a = 3;
        int b = 5;
        return 5 > a;
}

int
greater_zero(void)
{
        int a = 3;
        int b = 5;
        return a > 0;
}

int
less_equal_locals(void)
{
        int a = 3;
        int b = 5;
        return a <= b;
}

int
less_equal_constant(void)
{
        int a = 3;
        int b = 5;
        return a <= 5;
}

int
less_equal_constant_left(void)
{
        int a = 3;
        int b = 5;
        return 5 <= a;
}

int
less_equal_zero(void)
{
        int a = 3;
        int b = 5;
        return a <= 0;
}

int
greater_equal_locals(void)
{
        int a = 3;
        int b = 5;
        return a >= b;
}

int
greater_equal_constant(void)
{
        int a = 3;
        int b = 5;
        return a >= 5;
}

int
greater_equal_constant_left(void)
{
        int a = 3;
        int b = 5;
        return 5 >= a;
}

int
greater_equal_zero(void)
{
        int a = 3;
        int b = 5;
        return a >= 0;
}

int
equal_locals(void)
{
        int a = 3;
        int b = 5;
        return a == b;
}

int
equal_constant(void)
{
        int a = 3;
        int b = 5;
        return a == 5;
}

int
equal_constant_left(void)
{
        int a = 3;
        int b = 5;
        return 5 == a;
}

int
equal_zero(void)
{
        int a = 3;
        int b = 5;
        return a == 0;
}

int
not_equal_locals(void)
{
        int a = 3;
        int b = 5;
        return a != b;
}

int
not_equal_constant(void)
{
        int a = 3;
        int b = 5;
        return a != 5;
}

int
not_equal_constant_left(void)
{
        int a = 3;
        int b = 5;
        return 5 != a;
}

int
not_equal_zero(void)
{
        int a = 3;
        int b = 5;
        return a != 0;
}
