/* GNU C's a ?: b of a struct a: the condition shall be a scalar,
   6.5.16p2, an error, not a construct rcc lacks. */
struct point
{
        int x, y;
};

struct point origin;

int
main(void)
{
        return origin ?: 1;
}
