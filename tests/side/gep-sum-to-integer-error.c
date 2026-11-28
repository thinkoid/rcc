/* A sum of a pointer initializing an int is an error at the =. */
int
f(int *p, int i)
{
        int x = p + i;
        return x;
}
