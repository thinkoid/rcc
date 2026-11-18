/* sizeof of an incomplete struct, which has no size, 6.5.4.4p1. */
struct point;

int
main(void)
{
        return sizeof(struct point);
}
