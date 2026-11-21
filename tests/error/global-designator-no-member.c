/* A member designator of a name the struct has no member of,
   6.7.11p8. */
struct point
{
        int x, y;
};

struct point corner = { .z = 1 };

int
main(void)
{
        return corner.x;
}
