/* A member declared const assigned to through a pointer to its record:
   the member's lvalue has its const-qualified type, 6.5.3.4p4, no
   modifiable lvalue, 6.3.2.1p1. */
struct point
{
        const int x;
        int y;
};

void
move(struct point *p)
{
        p->x = 1;
}

int
main(void)
{
        return 0;
}
