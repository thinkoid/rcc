/* A member of a struct type read as a value, which copies the struct:
   not yet; its own members are read. */
struct point
{
        int x, y;
};

struct line
{
        struct point from, to;
};

int
first(struct line *l)
{
        int x = l->to.x;

        l->from;
        return x;
}

int
main(void)
{
        return 0;
}
