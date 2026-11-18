/* A struct assigned to an int: no record converts to an integer,
   6.5.17.2p1, an error of incompatible types. */
struct point
{
        int x, y;
};

struct point origin;
int n;

int
main(void)
{
        n = origin;

        return n;
}
