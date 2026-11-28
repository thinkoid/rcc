/* Pointers to two records are pointers to incompatible types,
   6.5.17.2p1. */
struct s
{
        int a;
};
struct t
{
        int a;
};

void
f(struct s *p, struct t *q)
{
        p = q;
}
