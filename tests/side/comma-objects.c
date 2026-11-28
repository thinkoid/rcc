/* A comma whose right operand is an object of static storage: defined,
   g, read in place as g(%rip); only declared, h, read through the
   address the GOT holds, loaded into a register outside those the
   operation leaves for last; and a block's static, a label of the
   function's.  x86-64 reads each through a comma on the right of an
   operator as the instruction's own operand, aarch64 writes the
   comma's value. */
int g = 5;
extern int h;

int
sum_defined(void)
{
        int a = 1;
        int b = 2;
        return a + (b, g);
}

int
sum_declared(void)
{
        int a = 1;
        int b = 2;
        return a + (b, h);
}

int
sum_static(void)
{
        static int s = 3;
        int a = 1;
        int b = 2;
        return a + (b, s);
}

int
quotient_declared(void)
{
        int a = 1;
        int b = 2;
        return a / (b, h);
}

int
remainder_defined(void)
{
        int a = 1;
        int b = 2;
        return a % (b, g);
}

int
shift_declared(void)
{
        int a = 1;
        int b = 2;
        return a << (b, h);
}

int
and_condition_declared(void)
{
        int a = 1;
        int b = 2;
        if (a & (b, h))
                return 1;
        return 0;
}

int
compared_declared(void)
{
        int a = 1;
        int b = 2;
        return a < (b, h);
}

int
value_declared(void)
{
        int b = 2;
        return (b, h);
}

int
stored_into_declared(void)
{
        int a = 1;
        h = (a, g);
        return h;
}

int
left_global(void)
{
        int b = 2;
        return (g, h) + b;
}

int
held_named(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return (a - b) + (c, h);
}
