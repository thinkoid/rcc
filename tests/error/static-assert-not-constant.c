/* A static assertion of an object's value: the expression is an
   integer constant expression, 6.7.12p3, and an object's value is
   none, 6.6p8. */
int limit = 4;

_Static_assert(limit > 2, "limit");

int
main(void)
{
        return 0;
}
