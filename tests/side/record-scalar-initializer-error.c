/* A record converts as if by assignment to no scalar, 6.5.17.2p1. */
struct s
{
        int a;
};

int
f(struct s *p)
{
        int x = *p;
        return x;
}
