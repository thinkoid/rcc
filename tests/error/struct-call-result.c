/* A call of a function declared with a struct result, which returns
   the struct by value: the declaration is accepted, and the call is not
   yet. */
struct point
{
        int x, y;
};

struct point origin(void);

int
main(void)
{
        origin();
        return 0;
}
