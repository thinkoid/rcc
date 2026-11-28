/* Pointer arithmetic is not yet, at its operator. */
int
f(void)
{
        int x;
        int *p = &x;
        return p + 1 == 0;
}
