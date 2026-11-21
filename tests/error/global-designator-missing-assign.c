/* A member designator not followed by its =, 6.7.11p1. */
struct point
{
        int x, y;
};

struct point corner = { .y 1 };

int
main(void)
{
        return corner.y;
}
