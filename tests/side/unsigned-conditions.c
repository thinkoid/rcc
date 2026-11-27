/* The conditions on the unsigned types, C7 of the survey: an object
   alone is compared in place at its width, cmpl $0, -4(%rbp) and cmpq;
   a comparison with an immediate in place, with jae, jbe, ja or jb; !,
   && and || on their operands at their own types. */
int
alone(void)
{
        unsigned u = 1;
        unsigned long ul = 1;
        if (u)
                return 1;
        if (ul)
                return 2;
        return 0;
}

int
below(void)
{
        unsigned u = 1;
        if (u < 5)
                return 1;
        if (u <= 0x80000000)
                return 2;
        return 0;
}

int
above(void)
{
        unsigned long ul = 1;
        if (ul > 5)
                return 1;
        if (ul >= 4294967296u)
                return 2;
        return 0;
}

int
logical(void)
{
        unsigned u = 1;
        unsigned long ul = 2;
        if (!u || u && ul)
                return 1;
        return 0;
}

int
narrowed(void)
{
        unsigned long ul = 1;
        if ((unsigned) ul)
                return 1;
        return 0;
}

int
loops(void)
{
        unsigned u = 10;
        int n = 0;
        while (u > 0)
                u = u - 1;
        for (u = 0; u < 4294967295u; u = u + 0x10000000)
                n = n + 1;
        return n;
}
