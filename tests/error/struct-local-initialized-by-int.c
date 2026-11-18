/* A local struct initialized by an int: a record's initializer is a
   list or a record compatible with it, 6.7.11p14, and an int is
   neither, an error of incompatible types. */
struct point
{
        int x, y;
};

int
main(void)
{
        struct point q = 1;

        return q.x;
}
