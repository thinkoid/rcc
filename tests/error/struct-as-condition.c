/* A struct as an if's condition, which shall be a scalar, 6.8.5.2p1:
   an error, not a construct rcc lacks. */
struct point
{
        int x, y;
};

struct point origin;

int
main(void)
{
        if (origin)
                return 1;

        return 0;
}
