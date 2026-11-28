/* A size that overflowed is warned of, and its wrapped value taken. */
int
f(void)
{
        int a[65536 * 65536 + 1];
        a[0] = 1;
        return a[0];
}
