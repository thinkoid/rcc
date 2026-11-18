/* A struct at file scope with a brace initializer, the initializer of a
   record: not yet. */
struct point
{
        int x, y;
};

struct point origin = { 0, 0 };

int
main(void)
{
        return 0;
}
