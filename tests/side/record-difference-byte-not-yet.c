/* The difference of two pointers to a record of one byte, which rcc
   does not divide. */
struct s
{
        char c;
};

long
f(struct s *p, struct s *q)
{
        return p - q;
}
