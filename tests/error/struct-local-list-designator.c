/* A member designator in a local record's list, 6.7.11p1, p8, which is
   not yet, as every designator is. */
struct point
{
        int x, y;
};

int
main(void)
{
        struct point q = { .y = 1 };

        return q.y;
}
