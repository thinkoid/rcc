/* A struct's value read for its effect alone, which reads its object:
   not yet. */
struct point
{
        int x, y;
};

void
touch(struct point *p)
{
        *p;
}

int
main(void)
{
        return 0;
}
