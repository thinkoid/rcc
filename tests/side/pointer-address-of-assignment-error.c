/* & of an assignment is an error at the &. */
int
f(int x)
{
        int y;
        int *p = &(y = x);
        return 0;
}
