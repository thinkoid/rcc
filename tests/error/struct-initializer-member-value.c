/* A member of a struct type at file scope initialized, in the list, by
   a struct's value, 6.7.11p14, which is no constant expression, p5: an
   error. */
struct point
{
        int x, y;
};

struct line
{
        struct point from, to;
};

struct point origin = { 0, 0 };
struct line diagonal = { origin, { 1, 1 } };

int
main(void)
{
        return diagonal.to.x;
}
