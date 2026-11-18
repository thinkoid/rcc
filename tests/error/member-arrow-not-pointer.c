/* -> on a struct, which is no pointer, 6.5.3.4p2. */
struct point
{
        int x, y;
};

extern struct point origin;

int
main(void)
{
        return origin->x;
}
