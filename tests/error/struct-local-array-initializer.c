/* A local array of records initialized by a list: a local array's
   initializer of any kind is not yet, an array of records' among
   them. */
struct point
{
        int x, y;
};

int
main(void)
{
        struct point a[2] = { { 1, 2 } };

        return a[0].x;
}
