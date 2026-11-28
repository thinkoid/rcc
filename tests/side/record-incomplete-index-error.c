/* A subscript's pointer points to a complete object type, 6.5.3.2p1. */
struct s;

struct s *
f(struct s *p)
{
        return &p[1];
}
