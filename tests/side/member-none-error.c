/* A member access names a member of its record, 6.5.3.4p1. */
struct s
{
        int a;
};

int
f(struct s *p)
{
        return p->b;
}
