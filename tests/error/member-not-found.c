/* A name that is no member of the struct, 6.5.3.4p2. */
struct point
{
        int x, y;
};

int
depth(struct point *p)
{
        return p->z;
}

int
main(void)
{
        return 0;
}
