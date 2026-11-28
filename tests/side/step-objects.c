/* ++ and -- of an object of static storage: defined, g, stepped in
   place for its effect, addl $1, g(%rip), and loaded and stored through
   its label for its value; only declared, h, through the address the
   GOT holds, the place formed first and held across the load and the
   store, outside the register the consumer names; and a block's
   static, a label of the function's. */
int g = 5;
extern int h;
int k(int x);

int
effect_defined(void)
{
        g++;
        --g;
        return g;
}

int
effect_declared(void)
{
        h++;
        --h;
        return h;
}

int
effect_static(void)
{
        static int s = 3;
        s++;
        return s;
}

int
value_defined(void)
{
        return g++;
}

int
value_declared(void)
{
        return h++;
}

int
prefix_value_declared(void)
{
        return ++h;
}

int
value_static(void)
{
        static int s = 3;
        return --s;
}

int
stored_declared(void)
{
        int a = 1;
        a = h--;
        return a;
}

int
sum_declared(void)
{
        int a = 1;
        return a + h++;
}

int
condition_declared(void)
{
        if (++h)
                return 1;
        return 0;
}

int
condition_postfix_declared(void)
{
        if (h--)
                return 1;
        return 0;
}

int
divisor_declared(void)
{
        int a = 7;
        return a / h++;
}

int
held_across_call(void)
{
        return h++ + k(g);
}
