/* A record's value for its effect is not yet, in rcc's words at the
   statement's first token. */
struct s
{
        int a;
};

void
f(struct s *p)
{
        *p;
}
