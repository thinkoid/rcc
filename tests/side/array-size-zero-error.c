/* A constant size is greater than zero, 6.7.7.3p1, an error at the
   size. */
int
f(void)
{
        int a[0];
        return 0;
}
