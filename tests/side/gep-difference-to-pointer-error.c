/* A difference initializing a pointer is an error at the =. */
int
f(int *p, int *q)
{
        int *r = p - q;
        return 0;
}
