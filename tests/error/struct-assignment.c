/* An assignment of a struct, which copies its object: the parser takes
   it, and c1 does not copy a record yet. */
struct point
{
        int x, y;
};

void
copy(struct point *to, struct point *from)
{
        *to = *from;
}

int
main(void)
{
        return 0;
}
