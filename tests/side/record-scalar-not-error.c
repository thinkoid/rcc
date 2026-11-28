/* A record's value is no operand of !, 6.5.4.3p1. */
struct s
{
        int a;
};

int
f(struct s *p)
{
        return !*p;
}
