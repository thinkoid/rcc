/* A struct's assignment whose value is an argument: the value of a
   record, which c1 does not read yet, is not yet. */
struct point
{
        int x, y;
};

int use(int);
struct point a, b, c;
int n;

int
main(void)
{
        use(a = b);

        return 0;
}
