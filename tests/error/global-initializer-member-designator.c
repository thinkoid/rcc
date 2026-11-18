/* A member designator, .x =, 6.7.11p1, p8, which is not yet, as every
   designator is. */
struct point
{
        int x, y;
};

struct point corner = { .y = 1 };

int
main(void)
{
        return corner.y;
}
