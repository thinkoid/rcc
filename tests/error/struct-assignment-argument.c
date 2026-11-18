/* A struct's assignment whose value is an argument past a variadic
   function's parameters, where a record passes by value: the value of
   a record, which c1 does not read yet, is not yet. */
struct point
{
        int x, y;
};

int use(int, ...);
struct point a, b, c;
int n;

int
main(void)
{
        use(0, a = b);

        return 0;
}
