/* & of a cast is an error at the &. */
int
f(int x)
{
        int *p = &(int) x;
        return 0;
}
