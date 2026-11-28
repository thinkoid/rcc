/* A record's value is no index, 6.5.3.2p1. */
struct s
{
        int a;
};

int
f(int *q, struct s *p)
{
        return q[*p];
}
