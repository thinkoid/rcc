/* A member through a cast over a sum, ((struct s *) (q + 1))->m, reads
   the pointer under the cast, rcc's uncast(): the sum's address plus
   the member's offset; and through a cast of a pointer to a record of
   another type. */
struct s
{
        int a;
        int m;
};

struct t
{
        long l;
        struct s in;
};

int
sum(int *q)
{
        return ((struct s *) (q + 1))->m;
}

int
cast(struct t *p)
{
        return ((struct s *) p)->m;
}

int
nested(int *q)
{
        return ((struct t *) (q + 2))->in.m;
}

void
stored(int *q, int v)
{
        ((struct s *) (q + 3))->m = v;
}
