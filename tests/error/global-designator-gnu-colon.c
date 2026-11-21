/* GNU C's obsolete member designator, y: 2 for .y = 2, which is not
   yet. */
struct p
{
        int x, y;
};

struct p d = { y : 2 };

int
main(void)
{
        return d.y;
}
