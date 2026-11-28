/* A record is no operand of ++, 6.5.4.1p1. */
struct s
{
        int a;
};

void
f(struct s *p)
{
        ++*p;
}
