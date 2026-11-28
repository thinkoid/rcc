/* An assignment of a record, a copy, which rcc-v2 does not write yet. */
struct s
{
        int a;
};

void
f(struct s *p, struct s *q)
{
        *p = *q;
}
