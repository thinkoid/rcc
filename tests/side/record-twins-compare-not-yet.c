/* Two complete records of one tag compared through pointers, not yet in
   rcc's words. */
struct s
{
        int a;
};

int
f(struct s *p)
{
        struct s
        {
                int a;
        } *q = 0;
        return p == q;
}
