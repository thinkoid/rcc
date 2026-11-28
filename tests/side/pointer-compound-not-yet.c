/* A compound assignment of a pointer is not yet, at its operator. */
int
f(void)
{
        int x;
        int *p = &x;
        p += 1;
        return 0;
}
