/* A struct initialized by an expression that is neither a list nor a
   struct's value, 6.7.11p14, p17: an error. */
struct point
{
        int x, y;
};

struct point origin = 0;

int
main(void)
{
        return origin.x;
}
