/* A member of type short through a pointer stored, 6.5.17.2p2: the
   value converted to the member's type and stored at its offset, a
   constant and 0 among the values, and the value of the assignment, the
   member's after the store, 6.5.17.1p3. */
struct s
{
        long x;
        short m;
};

void
store(struct s *p, short v)
{
        p->m = v;
}

void
converted(struct s *p, int v)
{
        p->m = v;
}

void
constant(struct s *p)
{
        p->m = 7;
}

void
zero(struct s *p)
{
        p->m = 0;
}

int
stored(struct s *p, int v)
{
        return p->m = v;
}

void
copied(struct s *p, struct s *q)
{
        p->m = q->m;
}
