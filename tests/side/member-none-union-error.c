/* A member access names a member of its union, 6.5.3.4p1. */
union u
{
        int a;
};

int
f(union u *p)
{
        return p->b;
}
