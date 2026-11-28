/* An empty member list, whose behavior 6.7.3.2p10 leaves undefined and
   GNU C takes, which rcc does not lay out yet, at the sizeof that asks
   its layout. */
struct s
{
};

int
pointer(struct s *p)
{
        return p != 0;
}

unsigned long
size(void)
{
        return sizeof(struct s);
}
