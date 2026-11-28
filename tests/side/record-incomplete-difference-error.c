/* A difference of pointers needs pointers to complete object types,
   6.5.7p3. */
struct s;

long
f(struct s *p, struct s *q)
{
        return p - q;
}
