/* An array is assigned no array either. */
int
f(void)
{
        int a[3];
        int b[3];
        a = b;
        return 0;
}
