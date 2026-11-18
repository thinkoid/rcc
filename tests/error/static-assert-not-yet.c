/* A static assertion on the size of a struct, which rcc does not know
   yet: the expression is refused by what it needs, never taken as
   true. */
struct pair
{
        int a, b;
};

_Static_assert(sizeof(struct pair) == 8, "pair");

int
main(void)
{
        return 0;
}
