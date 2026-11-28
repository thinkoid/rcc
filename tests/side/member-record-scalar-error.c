/* A record member's value is no operand of an arithmetic operator,
   6.5.7p2. */
struct t
{
        int k;
};

struct s
{
        long x;
        struct t in;
};

int
f(struct s *p)
{
        return p->in + 1;
}
