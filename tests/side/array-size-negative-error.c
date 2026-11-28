/* A negative size is an error at the size, 6.7.7.3p1. */
int
f(void)
{
        int a[-1];
        return 0;
}
