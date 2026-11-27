/* The conversions between int and long, B5 of the survey, by every
   route: a cast, a return, an assignment, an initializer, the operands
   of an operator and of a comparison, and the operands of a shift,
   each promoted on its own.  A narrowing writes nothing, (int) l being
   the movq of l; a widening of a load is its extending load; of a
   computed value, its extension where the value is not valid to 64
   bits; a cast to its own type writes nothing, a node all the same;
   and a cast of a constant folds to the constant of the new type,
   (int) 0x123456789 being 591751049. */
int
cast_narrowing(void)
{
        long l = 1;
        return (int) l;
}

long
cast_widening(void)
{
        int i = 1;
        return (long) i;
}

long
cast_widening_sum(void)
{
        int a = 1;
        int b = 2;
        return (long) (a + b);
}

int
cast_narrowing_sum(void)
{
        long a = 1;
        long b = 2;
        return (int) (a + b);
}

long
cast_own_type(void)
{
        long l = 1;
        return (long) l + (long long) l;
}

int
cast_own_type_int(void)
{
        int i = 1;
        return (int) i + (signed) i;
}

int
cast_constant_narrowing(void)
{
        return (int) 0x123456789;
}

long
cast_constant_widening(void)
{
        return (long) -1;
}

int
cast_constant_int_min(void)
{
        return (int) 2147483648;
}

int
returned_long(void)
{
        long l = 1;
        return l;
}

long
returned_int(void)
{
        int i = 1;
        return i;
}

int
assigned(void)
{
        int i = 1;
        long l = 2;
        i = l;
        l = i;
        l = i + 1;
        i = l + 1;
        return i;
}

int
initialized(void)
{
        long l = 2;
        int i = l;
        long m = i;
        long n = i * 2;
        return i;
}

long
operands(void)
{
        int i = 1;
        long l = 2;
        return (i + l) * (l - i) / (i | l);
}

int
compared(void)
{
        int i = 1;
        long l = 2;
        return (i < l) + (l == i) + (i >= l);
}

long
shift_long_count(void)
{
        int i = 1;
        long n = 3;
        return i << n;
}

long
shift_int_count(void)
{
        long l = 1;
        int n = 3;
        return l << n;
}

long
shift_long_constant_count(void)
{
        int i = 1;
        return i << 3L;
}

long
negation_of_cast(void)
{
        int i = 1;
        return -(long) i;
}

long
plus(void)
{
        long l = 1;
        int i = 2;
        return +l + +i;
}
