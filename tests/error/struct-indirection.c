/* Indirection through a pointer to an incomplete struct: the pointer
   is ordinary, and the lvalue, which has no size to address it by, is
   not yet. */
struct point;
int
touch(struct point *p)
{
        *p;
        return 0;
}
int
main(void)
{
        return touch(0);
}
