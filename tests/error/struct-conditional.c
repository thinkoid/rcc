/* A conditional whose arms are structs, as an assignment's right
   operand: the value of a record, which c1 does not read yet, is not
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
        a = n ? b : c;

        return 0;
}
