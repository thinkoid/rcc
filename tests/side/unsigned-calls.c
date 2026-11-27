/* Calls with unsigned values: an unsigned argument converted to its
   int parameter, a change of sign, stored as it is; an unsigned long
   one narrowed; a call's int result added to an unsigned, converted to
   unsigned, and the sum widened to the long result. */
int g(int x);

long
argument(void)
{
        unsigned u = 1;
        return g(u) + u;
}

int
argument_unsigned_long(void)
{
        unsigned long ul = 1;
        return g(ul);
}

int
argument_constant(void)
{
        return g(4294967295u) + g(0xffffffffffffffff);
}

unsigned
returns_unsigned(void)
{
        int a = -1;
        return a;
}
