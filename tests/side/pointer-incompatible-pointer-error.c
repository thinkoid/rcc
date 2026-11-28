/* A pointer to a pointer initializing a pointer to int is an error at
   the =. */
int
f(void)
{
        int x;
        int *p = &x;
        int **pp = &p;
        int *q = pp;
        return 0;
}
