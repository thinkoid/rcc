/* A member of type int * through a pointer stored, 6.5.17.2p2: the
   value converted to the member's type and stored at its offset, a null
   pointer constant among the values, and the value of the assignment,
   the member's after the store, 6.5.17.1p3. */
struct s
{
        long x;
        int *m;
};

void
store(struct s *p, int *q)
{
        p->m = q;
}

void
null(struct s *p)
{
        p->m = 0;
}

int *
stored(struct s *p, int *q)
{
        return p->m = q;
}

void
address(struct s *p, int *q)
{
        *p->m = *q;
}
