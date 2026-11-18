/* A struct passed for an int parameter: an argument converts as if by
   assignment, 6.5.3.3p2, and no record converts to an integer,
   6.5.17.2p1. */
struct point
{
        int x, y;
};

int use(int);
struct point origin;

int
main(void)
{
        return use(origin);
}
