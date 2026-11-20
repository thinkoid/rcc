/* shape.md chapter 5: the zeroing of a local record initialized by a
   list, the whole object, padding included, from the lowest address up
   in units of the record's alignment, at most 8 bytes, as a record's
   copy goes (G18), before the first listed value is stored.  The
   records are of 8 bytes at an alignment of 4, two units; of 24 at 8,
   three; of 200 at 8, twenty-five, more than eight; of 12 at 4 with
   padding after each char; and a union of 4 at 2 whose first member
   holds 3.  point_fewer(), wide_fewer() and large_fewer() list fewer
   values than members, padded_full() every member, point_empty() none,
   and union_shorter() the union's first member, which leaves its last
   byte. */
struct point
{
        int x, y;
};

struct wide
{
        long a, b, c;
};

struct large
{
        long a[25];
};

struct padded
{
        char c;
        int i;
        char d;
};

union either
{
        char c[3];
        short s;
};

void use(void *);

void
point_fewer(int x)
{
        struct point q = { x };

        use(&q);
}

void
wide_fewer(long a)
{
        struct wide w = { a, a + 1 };

        use(&w);
}

void
large_fewer(void)
{
        struct large l = { 1, 2 };

        use(&l);
}

void
padded_full(char c, int i)
{
        struct padded p = { c, i, c };

        use(&p);
}

void
point_empty(void)
{
        struct point q = { };

        use(&q);
}

void
union_shorter(void)
{
        union either e = { { 'a' } };

        use(&e);
}
