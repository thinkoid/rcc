/* The order of a compound assignment, 6.5.17.3p4, and the pins of its
   operation on x86-64: the right operand first, a call in it before the
   object's read, held outside rax so that the read takes it, a
   division's outside rax and rdx, a shift's count where cl is likely;
   /= and %= keep their own pin, rax and rdx, so a value held around
   them moves out of their way.

   Each function is one shape, its name the shape. */
int g(int x);

int
assignment_on_right(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a += (a = 2);
        return a;
}

int
call_on_right(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a -= g(b);
        return a;
}

int
call_on_right_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a -= g(b);
}

int
call_on_right_added(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a += g(b);
}

int
call_on_right_stored(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        c = a += g(b);
        return c;
}

int
chained(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a += b += c;
        return a;
}

int
self(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a += a;
        return a;
}

int
initializer(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        int d = a += b;
        return d;
}

int
in_logical_and(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        if (a += b && c)
                return 1;
        return 0;
}

int
compared(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        if ((a += b) < 3)
                return 1;
        return 0;
}

int
equal_to_zero(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        if ((a -= b) == 0)
                return 1;
        return 0;
}

int
loop_condition(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        while (a -= 1)
                b = b + 1;
        return b;
}

int
held_sum(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return b + (a += c);
}

int
held_sum_stored(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        c = b + (a += c);
        return c;
}

int
held_product(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return b * (a -= c);
}

int
held_quotient(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return b + (a /= c);
}

int
held_remainder(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return b - (a %= c);
}

int
held_shift(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return b + (a <<= c);
}

int
shift_by_sum(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return a <<= b + c;
}

int
quotient_on_right(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a -= b / c;
        return a;
}

int
remainder_on_right(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a += b % c;
        return a;
}

int
shift_on_right(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a += b << c;
        return a;
}

int
divide_by_quotient(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        b /= a / c;
        return b;
}

int
divide_by_remainder(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        b /= a % c;
        return b;
}

int
divide_by_remainder_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return b /= a % c;
}

int
shift_by_remainder(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        b <<= a % c;
        return b;
}

int
shift_by_remainder_value(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        return b <<= a % c;
}

int
multiply_by_call(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a *= g(b);
        return a;
}

int
remainder_by_call(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a %= g(b);
        return a;
}

int
shift_by_call(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a <<= g(b);
        return a;
}

int
comma_on_right(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        c += (a, b);
        return c;
}

int
comma_product(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        c *= (a, 5);
        return c;
}

int
step_on_right(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        a += b++;
        return a;
}

int
narrow_on_right(void)
{
        int a = 7;
        int b = 2;
        int c = 3;
        char d = 'd';
        a += d;
        return a;
}
