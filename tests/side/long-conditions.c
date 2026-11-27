/* The conditions on a long and on a cast to its own type, C7 of the
   survey: a long tested alone is cmpq $0, -8(%rbp) in place on x86-64,
   its load and cbz x0 on aarch64; a == 0 is the cmpq in place too, and
   cmp x0, #0 on aarch64; a comparison with an immediate is in place on
   x86-64, & is testq.  An int under a cast to its own type is the
   object as it is, so if ((int) i) is cmpl $0, -4(%rbp), and an int
   narrowed from a long is the long's movq and cmpl $0, %eax. */
int
alone(void)
{
        long l = 1;
        if (l)
                return 1;
        return 0;
}

int
equal_zero(void)
{
        long l = 1;
        if (l == 0)
                return 1;
        return 0;
}

int
less(void)
{
        long l = 1;
        if (l < 5)
                return 1;
        return 0;
}

int
less_long(void)
{
        long l = 1;
        long m = 2;
        while (l < m)
                l = l + 1;
        return 0;
}

int
masked(void)
{
        long l = 1;
        if (l & 1)
                return 1;
        return 0;
}

int
sum(void)
{
        long l = 1;
        long m = 2;
        if (l + m)
                return 1;
        return 0;
}

int
negated(void)
{
        long l = 1;
        if (!l)
                return 1;
        return 0;
}

int
logical(void)
{
        long l = 1;
        int i = 2;
        if (l && i || !l)
                return 1;
        return 0;
}

int
cast_to_own_type(void)
{
        int i = 1;
        if ((int) i)
                return 1;
        return 0;
}

int
narrowed(void)
{
        long l = 1;
        if ((int) l)
                return 1;
        return 0;
}

int
widened(void)
{
        int i = 1;
        if ((long) i)
                return 1;
        return 0;
}

int
loops(void)
{
        long l = 10;
        int n = 0;
        do
                n = n + 1;
        while (l = l - 1);
        for (l = 0; l < 0x100000000; l = l + 0x10000000)
                n = n + 1;
        return n;
}
