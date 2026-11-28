/* A pointer, an array's decay among them, is subtracted from no
   integer. */
int
f(void)
{
        int a[3];
        return 1 - a;
}
