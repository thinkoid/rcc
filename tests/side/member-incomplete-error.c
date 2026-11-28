/* A member access reads a complete record, 6.5.3.4p1. */
struct s;

int
f(struct s *p)
{
        return p->a;
}
