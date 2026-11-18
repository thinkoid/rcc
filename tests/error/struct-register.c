/* A struct declared register, which no register holds: not yet. */
struct point
{
        int x, y;
};

int
main(void)
{
        register struct point p;

        return 0;
}
