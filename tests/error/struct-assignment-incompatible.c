/* An assignment of a struct from a struct of another type, which
   6.5.17.2p1 does not take: an error. */
struct point
{
        int x, y;
};

struct other
{
        int x, y;
} other;
struct point a, b, c;
int n;

int
main(void)
{
        a = other;

        return 0;
}
