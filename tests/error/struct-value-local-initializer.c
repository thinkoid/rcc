/* A struct's value initializing a local int: the value is the
   initializer's, refused at its first token, not yet. */
struct point
{
        int x, y;
};

struct point origin;

int
main(void)
{
        int x = origin;

        return x;
}
