/* A statement expression whose value is a record's, here an
   assignment's of two structs: rcc does not read a record's value
   yet. */
struct point
{
        int x, y;
};

struct point a, b;

int
main(void)
{
        ({ a = b; });

        return a.x;
}
