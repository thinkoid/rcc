/* The place of an assignment to an object only declared is taken
   first, its GOT's load into a register left outside what a pinned
   instruction within the value would move it out of and outside the
   register the consumer names: g = a / b takes rcx, outside rax and
   rdx; g = a << b rax, outside rcx; return g = 7 rcx on x86-64 and x1
   on aarch64, outside the result's register; g = k(a) rax, which the
   call spills and the store reloads. */
int k(int x);
extern int g;
extern int h;

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
