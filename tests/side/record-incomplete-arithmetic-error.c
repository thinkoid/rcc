/* Pointer arithmetic needs a pointer to a complete object type,
   6.5.7p2. */
struct s;

struct s *
f(struct s *p)
{
        return p + 1;
}
