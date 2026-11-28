/* An array parameter's size that is no constant is not yet, at the
   size. */
int
f(int n, int a[n])
{
        return a[0];
}
