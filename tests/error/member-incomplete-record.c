/* A member access into an incomplete struct, whose members are not
   known, 6.5.3.4p2 and 6.7.3.4p5. */
struct point;

int
first(struct point *p)
{
        return p->x;
}

int
main(void)
{
        return 0;
}
