/* Two arms of one record are a record's value, not yet, in rcc's words
   at the '?'. */
struct s
{
        int a;
};

void
f(int c, struct s *p, struct s *q)
{
        c ? *p : *q;
}
