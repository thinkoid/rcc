/* A pointer stored through a pointer to int is an error at the =. */
int
f(int *p, int *q)
{
        *p = q;
        return 0;
}
