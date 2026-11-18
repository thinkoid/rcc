/* A struct's assignment whose value is the right operand of another,
   a = b = c: the value of a record, which c1 does not read yet, is not
   yet. */
struct point
{
        int x, y;
};

struct point a, b, c;
int n;

int
main(void)
{
        a = b = c;

        return 0;
}
