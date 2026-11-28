/* A record member's value for its effect is not yet. */
struct t
{
        int k;
};

struct s
{
        long x;
        struct t in;
};

void
f(struct s *p)
{
        p->in;
}
