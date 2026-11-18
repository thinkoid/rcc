/* A comma whose right operand is a struct's assignment, as a
   statement: a comma of a record is refused as its value, though the
   statement discards it, and is not yet. */
struct point
{
        int x, y;
};

struct point a, b, c;
int n;

int
main(void)
{
        n++, a = b;

        return 0;
}
