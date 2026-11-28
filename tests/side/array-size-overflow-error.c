/* A size that overflowed is warned of, and its wrapped value, no
   positive one, is the error. */
int
f(void)
{
        int a[2147483647 + 1];
        return 0;
}
