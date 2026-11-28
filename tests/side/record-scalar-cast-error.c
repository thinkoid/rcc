/* A record's value is no operand of a cast, 6.5.5p2. */
struct s
{
        int a;
};

int
f(struct s *p)
{
        return (int) *p;
}
