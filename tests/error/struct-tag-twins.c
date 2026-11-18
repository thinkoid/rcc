/* Two structs of one tag, one at file scope and one in a block, of the
   same members: C23 makes them compatible, 6.2.7p1, which rcc does not
   compare, so the conversion between pointers to them is not yet. */
struct point
{
        int x, y;
};

struct point *here;

int
main(void)
{
        struct point
        {
                int x, y;
        } *there = here;

        return there != 0;
}
