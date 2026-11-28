/* A returned record converts to no scalar result, 6.8.7.5p3,
   6.5.17.2p1. */
struct s
{
        int a;
};

int
f(struct s *p)
{
        return *p;
}
