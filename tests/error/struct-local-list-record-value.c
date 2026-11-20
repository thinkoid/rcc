/* A record's value in a scalar's place in a local record's list: p
   initializes l.from, 6.7.11p14, which the walk reaches as the scalar
   l.from.x, and a record's value there is not yet. */
struct point
{
        int x, y;
};

struct line
{
        struct point from, to;
};

int
main(void)
{
        struct point p = { 1, 2 };
        struct line l = { p, p };

        return l.to.y;
}
