/* Two complete records of one tag, which C23 makes compatible where
   their members agree, 6.2.7p1, not yet in rcc's words. */
struct s
{
        int a;
};

void
f(struct s *p)
{
        struct s
        {
                int a;
        } *q;
        q = p;
}
