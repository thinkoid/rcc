/* A record's value as a comma's operand is not yet. */
struct s
{
        int a;
};

void
f(int n, struct s *p)
{
        *p, n;
}
