/* A struct returned from a function of int: the value converts as if
   by assignment, 6.8.7.5p3, and no record converts to an integer,
   6.5.17.2p1. */
struct point
{
        int x, y;
};

struct point origin;

int
main(void)
{
        return origin;
}
