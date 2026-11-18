/* A conditional of a struct and an int: two arms meet as two
   arithmetic values, two compatible records or the pairs of pointers,
   6.5.16p3, and a record and an integer in none of them. */
struct point
{
        int x, y;
};

struct point origin;
int n;

int
main(void)
{
        return n ? origin : 1;
}
