/* A struct's value initializing a local int: no record converts to an
   integer as if by assignment, 6.5.17.2p1 through 6.7.11p12, an error
   of the conversion, said at the =. */
struct point
{
        int x, y;
};

struct point origin;

int
main(void)
{
        int x = origin;

        return x;
}
