/* Conditions as jumps, 7.1, each under if (C) return 1; return 0;: the
   condition's own jump, to the label past the then part where it is
   0.  x86-64 compares a local in place, cmpl $0, -4(%rbp), and a load
   against an immediate, cmpl $5, -4(%rbp); the rows of + - | ^ and
   unary - set ZF from their value and jump on it with no cmp, addl and
   je, and & is test, testl $1, -4(%rbp); (a + b) == 0 is that jump the
   other way, jne; any other value is compared with 0 in its register,
   cmpl $0, %eax, a constant too.  aarch64 jumps on a comparison's flags
   with b.cc after cmp, cmp w0, #4096 or a register for a constant cmp
   does not encode, a == 0 too, cmp w0, #0 and b.ne; and on any other
   value with cbz, a sum's too, add w0, w0, w1 and cbz w0.

   Each function's name is its condition's form. */
int
local(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a)
                return 1;
        return 0;
}

int
negation(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (!a)
                return 1;
        return 0;
}

int
sum(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a + b)
                return 1;
        return 0;
}

int
difference(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a - b)
                return 1;
        return 0;
}

int
inclusive_or(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a | b)
                return 1;
        return 0;
}

int
exclusive_or(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a ^ b)
                return 1;
        return 0;
}

int
negative(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (-a)
                return 1;
        return 0;
}

int
bitwise_and(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a & b)
                return 1;
        return 0;
}

int
bitwise_and_one(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a & 1)
                return 1;
        return 0;
}

int
one_bitwise_and(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (1 & a)
                return 1;
        return 0;
}

int
sum_equal_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if ((a + b) == 0)
                return 1;
        return 0;
}

int
sum_not_equal_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if ((a + b) != 0)
                return 1;
        return 0;
}

int
zero_equal_sum(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (0 == a + b)
                return 1;
        return 0;
}

int
bitwise_and_equal_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if ((a & b) == 0)
                return 1;
        return 0;
}

int
bitwise_and_one_not_equal_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if ((a & 1) != 0)
                return 1;
        return 0;
}

int
negative_equal_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (-a == 0)
                return 1;
        return 0;
}

int
equal_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a == 0)
                return 1;
        return 0;
}

int
not_equal_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a != 0)
                return 1;
        return 0;
}

int
product(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a * b)
                return 1;
        return 0;
}

int
product_equal_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a * b == 0)
                return 1;
        return 0;
}

int
assignment(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a = b)
                return 1;
        return 0;
}

int
assignment_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a = 0)
                return 1;
        return 0;
}

int
seven(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (7)
                return 1;
        return 0;
}

int
zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (0)
                return 1;
        return 0;
}

int
folded(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (1 < 2)
                return 1;
        return 0;
}

int
less_4096(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a < 4096)
                return 1;
        return 0;
}

int
less_8192(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a < 8192)
                return 1;
        return 0;
}

int
less_minus_4096(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a < -4096)
                return 1;
        return 0;
}

int
less_4097(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a < 4097)
                return 1;
        return 0;
}

int
less_5000(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a < 5000)
                return 1;
        return 0;
}

int
less_maximum(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a < 2147483647)
                return 1;
        return 0;
}

int
equal_minimum(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a == -2147483647 - 1)
                return 1;
        return 0;
}

int
less_sum(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a < b + c)
                return 1;
        return 0;
}

int
sum_less(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a + b < c)
                return 1;
        return 0;
}

int
sum_less_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a + b < 0)
                return 1;
        return 0;
}

int
less_less(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a < b < c)
                return 1;
        return 0;
}

int
comparison_equal_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if ((a < b) == 0)
                return 1;
        return 0;
}

int
quotient(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a / b)
                return 1;
        return 0;
}

int
remainder(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a % b)
                return 1;
        return 0;
}

int
left_shift(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a << b)
                return 1;
        return 0;
}

int
right_shift_one(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a >> 1)
                return 1;
        return 0;
}

int
complement(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (~a)
                return 1;
        return 0;
}

int
sum_constant(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a + 5)
                return 1;
        return 0;
}

int
constant_difference(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (5 - a)
                return 1;
        return 0;
}

int
sum_of_sums(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a + b + c)
                return 1;
        return 0;
}

int
identity(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a + 0)
                return 1;
        return 0;
}

int
bitwise_and_sum(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a & (b + c))
                return 1;
        return 0;
}

int
sum_bitwise_and_one(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if ((a + b) & 1)
                return 1;
        return 0;
}

int
bitwise_and_quotient(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a & b / c)
                return 1;
        return 0;
}
