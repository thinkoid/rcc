/* Indirection through a pointer to a struct: the pointer is ordinary,
   and the struct it points to is not yet. */
struct point;

int touch(struct point *p)
{
    *p;
    return 0;
}

int main(void)
{
    return touch(0);
}
