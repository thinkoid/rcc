/* An array initializes no array, 6.7.11p17. */
int
f(void)
{
        int b[3];
        int a[3] = b;
        return 0;
}
