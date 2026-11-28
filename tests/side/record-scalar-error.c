/* A record's value is no operand of an arithmetic operator, 6.5.7p2. */
struct s
{
        int a;
};

int
f(struct s *p)
{
        return *p + 1;
}
