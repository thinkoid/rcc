/* E1 op= E2 on an object of static storage: defined, g, its place a
   label, read and written in place, addl $5, g(%rip); only declared, h,
   its place the address the GOT holds, formed first and held across
   the right operand, outside the registers the operation's own
   instruction pins, rax and rdx for /= and %=, rcx for a shift by a
   count that is no constant, and outside the one the consumer names;
   and a block's static, a label of the function's. */
int g = 5;
extern int h;
int k(int x);

int
effect_defined(void)
{
        int a = 2;
        g += a;
        g -= 3;
        g <<= a;
        g >>= 1;
        g *= a;
        g /= a;
        g %= 3;
        return g;
}

int
effect_declared(void)
{
        int a = 2;
        h += a;
        h &= 7;
        h <<= a;
        h ^= 1;
        h *= 5;
        h /= a;
        h %= a;
        return h;
}

int
effect_static(void)
{
        static int s = 3;
        int a = 2;
        s += a;
        s |= 4;
        return s;
}

int
value_declared(void)
{
        int a = 2;
        return h += a;
}

int
value_declared_immediate(void)
{
        return h -= 5;
}

int
value_declared_product(void)
{
        return h *= 12;
}

int
value_declared_quotient(void)
{
        int a = 2;
        return h /= a;
}

int
value_declared_remainder(void)
{
        int a = 2;
        return h %= a;
}

int
value_declared_shift(void)
{
        int a = 2;
        return h <<= a;
}

int
stored_declared(void)
{
        int a = 2;
        int c = 3;
        c = h += a;
        return c;
}

int
held_declared(void)
{
        int a = 2;
        int b = 3;
        return b + (h /= a);
}

int
call_on_right_declared(void)
{
        int b = 3;
        return h -= k(b);
}

int
condition_declared(void)
{
        int a = 2;
        if ((h -= a) == 0)
                return 1;
        return 0;
}

int
local_by_global(void)
{
        int a = 2;
        a += h;
        a *= g;
        return a;
}
