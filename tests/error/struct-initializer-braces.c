/* A member that is a scalar initialized in two pairs of braces, where
   6.7.11p12 gives a scalar one: an error, as two pairs around a scalar
   object are. */
struct point
{
        int x, y;
};

struct point corner = { { { 1 } }, 2 };

int
main(void)
{
        return corner.x;
}
