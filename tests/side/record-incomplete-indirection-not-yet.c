/* *p of a pointer to an incomplete record, whose place only & and
   sizeof could take, is not yet, in rcc's words. */
struct s;

struct s *
f(struct s *p)
{
        return &*p;
}
