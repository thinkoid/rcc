/* A member of a const record assigned to: the member's lvalue has the
   so-qualified version of its type, 6.5.3.4p3, no modifiable lvalue,
   6.3.2.1p1. */
const struct point
{
        int x;
        int y;
} origin;

int
main(void)
{
        origin.x = 1;
        return 0;
}
