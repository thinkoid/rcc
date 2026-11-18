/* A cast of a struct, which is no scalar, 6.5.5p2. */
struct point
{
        int x, y;
};

int
first(struct point *p)
{
        return (int) *p;
}

int
main(void)
{
        return 0;
}
