/* A cast to a struct, which is no scalar, 6.5.5p2. */
struct point
{
        int x, y;
};

int
main(void)
{
        int x = 0;

        (struct point) x;
        return 0;
}
