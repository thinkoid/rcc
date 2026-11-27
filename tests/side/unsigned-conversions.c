/* The conversions among int, unsigned, long and unsigned long, B5 of
   the survey, by every route.  A change of sign alone writes nothing
   and is a node all the same, which an operand in memory looks through,
   a + (int) u being addl -8(%rbp), %eax, and a condition too, if ((int)
   u) comparing the object in place; a widening of an unsigned load is
   the load, valid to 64 bits; of an unsigned value computed at 32 bits,
   nothing; of one narrowed to 32 bits, movl %eax, %eax.  A cast of a
   constant folds by the sign of the type it converts from, so (long)
   (unsigned) -1 is 4294967295. */
int
change_of_sign_operand(void)
{
        int a = 1;
        unsigned u = 2;
        return a + (int) u;
}

int
change_of_sign_condition(void)
{
        unsigned u = 2;
        if ((int) u)
                return 1;
        return 0;
}

unsigned
change_of_sign_unsigned(void)
{
        int i = 1;
        unsigned u = 2;
        return u + (unsigned) i;
}

long
widened_load(void)
{
        unsigned u = 2;
        return u;
}

unsigned long
widened_load_unsigned_long(void)
{
        unsigned u = 2;
        return (unsigned long) u;
}

unsigned long
widened_int(void)
{
        int i = -1;
        return i;
}

long
narrowed_then_widened(void)
{
        long l = -1;
        return (unsigned) l;
}

unsigned
narrowed_unsigned_long(void)
{
        unsigned long l = 1;
        return l;
}

long
constants(void)
{
        return (long) (unsigned) -1 + (unsigned long) -1 +
               (unsigned) 0x123456789 + (long) 0x80000000;
}

unsigned
assigned(void)
{
        unsigned u;
        unsigned long ul = 1;
        long l = 2;
        int i = 3;
        u = ul;
        u = l;
        u = i;
        ul = u;
        l = u;
        i = u;
        return u;
}

long
initialized(void)
{
        unsigned u = -1;
        unsigned long ul = u;
        long l = u;
        int i = ul;
        return l + i;
}

long
operands(void)
{
        int i = 1;
        unsigned u = 2;
        long l = 3;
        unsigned long ul = 4;
        return (i + u) + (u + l) + (l + ul) + (u * ul) + (i / u) + (l % u);
}

unsigned long
long_long_and_unsigned_long(void)
{
        long long ll = 1;
        unsigned long ul = 2;
        return ll / ul;
}

long
shift_counts(void)
{
        unsigned u = 1;
        long l = 3;
        unsigned long ul = 4;
        return (u << l) + (l >> u) + (ul >> l) + (l << ul);
}

long
negation(void)
{
        unsigned u = 1;
        return -u;
}
