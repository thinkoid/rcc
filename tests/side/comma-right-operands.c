/* A comma on the right of an operator, rcc's operand_of() on x86-64:
   the operator's left operand first, then the comma's left operand for
   its effect, then the comma's right operand as the instruction's own,
   in memory, addl -12(%rbp), %eax for a + (b, c), or an immediate,
   addl $5, %eax for a + (b, 5); a shift's count pins rcx whatever it
   is, so the shifted value avoids rcx even where the count is the
   immediate.  A comparison reads no comma through: a < (b, 5) loads 5.
   aarch64 reads none: the comma's value is the operand, a shift's count
   of its own type.

   Each function is one shape, its name the shape. */
int g(int x);

int
immediate(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a + (b, 5);
}

int
immediate_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a + (b, 0);
}

int
nested(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a + (b, (c, d));
}

int
product_immediate(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a * (b, 5);
}

int
product_load(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a * (b, c);
}

int
quotient(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a / (b, c);
}

int
quotient_constant(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a / (b, 5);
}

int
remainder(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a % (b, c);
}

int
shift_immediate(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return d + (a << (b, 3));
}

int
shift_variable(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return d + (a << (b, c));
}

int
shift_of_comma(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return (b, 1) << a;
}

int
and_value(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a & (b, 5);
}

int
and_condition(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        if (a & (b, c))
                return 1;
        return 0;
}

int
and_condition_immediate(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        if (a & (b, 1))
                return 1;
        return 0;
}

int
and_condition_first(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        if ((b, 1) & a)
                return 1;
        return 0;
}

int
constant_first(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return 5 + (b, a);
}

int
constant_minus(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return 5 - (b, a);
}

int
constant_on_left(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return (b, 5) + a;
}

int
compared_immediate(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a < (b, 5);
}

int
compared_load(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a < (b, c);
}

int
compared_on_left(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return (a, b) < 5;
}

int
compared_with_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        if ((a, b + c) == 0)
                return 1;
        return 0;
}

int
equal_to_zero_comma(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        if (a == (b, 0))
                return 1;
        return 0;
}

int
pinned_within(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return d + (a + (b / c, d));
}

int
division_in_left(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a + (b / c, d);
}

int
division_on_right(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a + (b, c / d);
}

int
count_division(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a << (b, c % d);
}

int
divisor_assignment(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a / (b = c + d, b);
}

int
count_assignment(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a << (b = c + d, b);
}

int
left_held_named(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return (a - b) + (c, d);
}

int
call_in_comma(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int d = 4;
        return a + (g(b), c);
}

long
long_sum(void)
{
        long a = 4294967296;
        long b = 2;
        long c = 3;
        return a + (b, c);
}

long
long_immediate(void)
{
        long a = 4294967296;
        long b = 2;
        return a * (b, 5);
}

long
long_wide(void)
{
        long a = 4294967296;
        long b = 2;
        return a + (b, 8589934592);
}

long
long_shift(void)
{
        long a = 4294967296;
        int c = 3;
        return a << (c, c);
}

unsigned
unsigned_quotient(void)
{
        unsigned a = 7;
        unsigned b = 2;
        return a / (b, b);
}

int
narrow_right(void)
{
        int a = 1;
        char c = 'c';
        return a + (a, c);
}

int
count_of_its_own_type(void)
{
        int a = 1;
        int b = 2;
        return a << (b, 5L);
}

int
sign_changed(void)
{
        short s = -1;
        int b = 2;
        return (b, (unsigned short) s);
}

int
sign_changed_operand(void)
{
        int a = 1;
        int b = 2;
        unsigned u = 3;
        return a + (b, (int) u);
}

int
sign_changed_count(void)
{
        int a = 1;
        int b = 2;
        unsigned u = 3;
        return a << (b, (int) u);
}
