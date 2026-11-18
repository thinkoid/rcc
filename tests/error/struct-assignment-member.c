/* A member of a struct's assignment, (a = b).x: the assignment is no
   lvalue, and the member of a record's value is not yet. */
struct point
{
        int x, y;
};

struct point a, b, c;
int n;

int
main(void)
{
        n = (a = b).x;

        return 0;
}
