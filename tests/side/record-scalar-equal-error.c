/* Records do not compare, 6.5.10p2. */
struct s
{
        int a;
};

int
f(struct s *p, struct s *q)
{
        return *p == *q;
}
