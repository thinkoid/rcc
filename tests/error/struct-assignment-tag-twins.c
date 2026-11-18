/* An assignment between two structs of one tag declared in two scopes:
   C23 may make the two compatible, 6.2.7p1, which rcc does not compare
   yet, so the assignment is not yet. */
struct point
{
        int x, y;
} a;

int
main(void)
{
        struct point
        {
                int x, y;
        } b;

        a = b;

        return 0;
}
