/* A compound assignment of a struct, which 6.5.17.3p1 takes of no
   record: an error. */
struct point
{
        int x, y;
};

struct point a, b, c;
int n;

int
main(void)
{
        a += b;

        return 0;
}
