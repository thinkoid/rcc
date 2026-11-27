/* Calls with long values: an argument of type long converted to its
   int parameter, the low half of the long stored to the argument's
   4-byte slot, movq -8(%rbp), %rax and movl %eax, -12(%rbp), and a
   constant folded to the int it is; an int function returning a long
   value, its low half, and a long function returning an int value,
   extended, and a call's int result widened. */
int g(int x);
int g2(int x, int y);

int
argument(void)
{
        long a = 1;
        return g(a);
}

int
argument_constant(void)
{
        return g(0x123456789);
}

int
arguments(void)
{
        long a = 1;
        long b = 2;
        return g2(a + b, a * 0x100000000);
}

int
returns_long(void)
{
        long a = 1;
        return a;
}

long
returns_int(void)
{
        int a = 1;
        return a;
}

long
result_widened(void)
{
        long a = 1;
        return g(a) + a;
}

long
result_cast(void)
{
        return (long) g(1);
}

long
held_across(void)
{
        long a = 1;
        long b = 2;
        return a * b + g(b);
}
