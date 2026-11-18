/* A local of a struct incomplete at the end of its declarator, 6.7.1p8. */
struct point;

int
main(void)
{
        struct point p;

        return 0;
}
