/* A record's value is no condition, 6.8.5.2p1. */
struct s
{
        int a;
};

int
f(struct s *p)
{
        if (*p)
                return 1;
        return 0;
}
