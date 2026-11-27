/* The rows of objects-places-declared.c on objects the unit defines,
   whose place takes no register. */
int k(int x);
int g;
int h;

int
quotient_returned(void)
{
        int a;
        int b;
        return g = a / b;
}

int
shift_returned(void)
{
        int a;
        int b;
        return g = a << b;
}

int
quotient_stored(void)
{
        int a;
        int b;
        g = a / b;
        return 0;
}

int
shift_stored(void)
{
        int a;
        int b;
        g = a << b;
        return 0;
}

int
call_stored(void)
{
        int a;
        g = k(a);
        return 0;
}

int
call_returned(void)
{
        int a;
        return g = k(a);
}

int
constant_returned(void)
{
        return g = 7;
}

int
object_returned(void)
{
        return g = h;
}

int
sum_held(void)
{
        int a;
        int b;
        return a + (g = b);
}

int
call_plus(void)
{
        int a;
        int b;
        g = k(a) + b;
        return 0;
}

int
logical_stored(void)
{
        int a;
        int b;
        int c;
        g = a + (b && c);
        return 0;
}

int
divisor(void)
{
        int a;
        return a / g;
}

int
remainder(void)
{
        int a;
        return a % g;
}

int
test(void)
{
        int a;
        if (a & g)
                return 1;
        return 0;
}

int
incremented(void)
{
        g = g + 1;
        return 0;
}

int
compared(void)
{
        if (g < 5)
                return 1;
        return 0;
}

int
product(void)
{
        return g * 5;
}
