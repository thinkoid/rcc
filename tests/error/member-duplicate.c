/* Two members of one name in one struct, which has one name space for
   its members, 6.2.3p1. */
struct point
{
        int x;
        long x;
};

int
main(void)
{
        return 0;
}
