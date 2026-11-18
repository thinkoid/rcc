/* A static assertion on the size of a struct rcc does not lay out yet,
   one with a bit-field: the expression is refused by what it needs,
   never taken as true. */
struct flags
{
        unsigned a : 1, b : 1;
};
_Static_assert(sizeof(struct flags) == 4, "flags");
int
main(void)
{
        return 0;
}
