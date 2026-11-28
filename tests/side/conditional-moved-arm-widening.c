/* The join of an unsigned c ? y : n whose second arm, a call's int
   result, a mov brought into the first arm's remainder's edx: the join
   is valid to 64 bits, as the 32-bit mov wrote it, so no widening of it
   writes an extension, through a comma, through an assignment, as a
   switch's index, or under a cast; on aarch64 no arm moves, and the
   call's result is extended, mov w0, w0. */
int g(int x);

long
through_comma(void)
{
        int a;
        unsigned y;
        unsigned n;
        int x;
        long l;
        l = (x, a ? y % n : g(a));
        return l;
}

long
through_assignment(void)
{
        int a;
        unsigned y;
        unsigned n;
        unsigned u;
        long l;
        l = (u = a ? y % n : g(a));
        return l;
}

int
through_switch(void)
{
        int a;
        unsigned y;
        unsigned n;
        switch (a ? y % n : g(a)) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 3: return 4;
        }
        return 0;
}

long
cast(void)
{
        int a;
        unsigned y;
        unsigned n;
        long l;
        l = (long) (a ? y % n : g(a));
        return l;
}
