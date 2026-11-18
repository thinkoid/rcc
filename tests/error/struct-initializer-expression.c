/* A struct at file scope initialized by a struct's value, 6.7.11p14,
   which is no constant expression, p5: an error. */
struct point
{
        int x, y;
};

struct point origin = { 0, 0 };
struct point copy = origin;

int
main(void)
{
        return copy.x;
}
