/* static in an array parameter's brackets is not yet. */
int
f(int a[static 3])
{
        return a[0];
}
