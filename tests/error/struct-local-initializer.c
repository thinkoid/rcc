/* A local struct initialized by a struct's value, which copies the
   struct: not yet. */
struct point
{
        int x, y;
};

int
first(struct point *p)
{
        struct point q = *p;

        return 0;
}

int
main(void)
{
        return 0;
}
