/* A record's value is no operand of &&, 6.5.14p2. */
struct s
{
        int a;
};

int
f(struct s *p, int n)
{
        return n && *p;
}
