/* A pointer subscripted by a pointer is an error at the [. */
int
f(int *p, int *q)
{
        return p[q];
}
