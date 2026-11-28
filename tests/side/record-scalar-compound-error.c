/* A record is no modifiable lvalue of a compound assignment,
   6.5.17.3p1. */
struct s
{
        int a;
};

void
f(struct s *p)
{
        *p += 1;
}
