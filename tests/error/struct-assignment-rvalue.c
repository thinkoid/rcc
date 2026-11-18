/* An assignment to a comma of a struct, which is no lvalue, 6.5.17.1p2:
   an error. */
struct point
{
        int x, y;
};

struct point a, b, c;
int n;

int
main(void)
{
        (n, a) = b;

        return 0;
}
