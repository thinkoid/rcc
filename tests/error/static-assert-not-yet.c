/* A static assertion on the size of a struct rcc does not lay out yet,
   a packed one: the expression is refused by what it needs, never
   taken as true. */
struct flags
{
        int a;
        char b;
} __attribute__((packed));
_Static_assert(sizeof(struct flags) == 5, "flags");
int
main(void)
{
        return 0;
}
