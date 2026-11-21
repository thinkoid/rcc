/* An array designator for a struct, which is no array, 6.7.11p7. */
struct point
{
        int x, y;
};

struct point corner = { [0] = 1 };

int
main(void)
{
        return corner.x;
}
