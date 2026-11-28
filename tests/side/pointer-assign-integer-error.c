/* An integer assigned to a pointer is an error at the =. */
int
f(int x)
{
        int *p;
        p = x;
        return 0;
}
