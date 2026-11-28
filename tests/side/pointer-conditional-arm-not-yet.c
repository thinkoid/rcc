/* A pointer arm of a conditional is not yet, at its '?'. */
int
f(int a)
{
        int x;
        int *p = &x;
        int *q = a ? p : 0;
        return q == p;
}
