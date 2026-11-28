/* *p as an operand: x86-64 names the object through the pointer as the
   instruction's operand, addl (%rcx), %eax, idivl (%rcx), imull $5,
   (%rax), %eax, cmpl $5, (%rax), where aarch64 loads it. */
int g(void);
int *gp(void);

int
arithmetic(int *p, int x)
{
        return (x + *p) + (x - *p) + (x & *p) + (x | *p) + (x ^ *p) + (x * *p);
}

int
on_the_left(int *p, int x)
{
        return (*p + x) + (*p - x) + (*p * x) + (*p & x) + (*p ^ 1);
}

int
constants(int *p)
{
        return (*p + 1) + (1 + *p) + (*p - 1) + (1 - *p) + (*p * 5) + (5 * *p) +
               (*p / 5);
}

int
divisions(int *p, int x)
{
        return (x / *p) + (*p / x) + (x % *p) + (*p % x);
}

int
shifts(int *p, int x)
{
        return (x << *p) + (*p << x) + (*p >> 3) + (x >> *p);
}

int
unary(int *p)
{
        return -*p + ~*p + !*p;
}

int
comparisons(int *p, int *q, int x)
{
        return (x < *p) + (*p < x) + (*p < 5) + (5 < *p) + (*p == 5) +
               (*p == 0) + (*p < *q);
}

int
logical(int *p, int *q)
{
        return *p && *q || *p;
}

int
through_calls(int x)
{
        return (*gp() + x) + (x + *gp());
}

int
long_operands(long l, int *p)
{
        return l + *p;
}

int
assigned(int *p, int x)
{
        int y = *p;
        x = *p;
        return x + y;
}
