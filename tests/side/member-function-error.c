/* A function designator is no record, 6.5.3.4p1. */
struct s
{
        int a;
};

struct s *g(void);

int
f(void)
{
        return g.a;
}
