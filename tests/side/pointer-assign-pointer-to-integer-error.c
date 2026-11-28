/* A pointer assigned to an integer is an error at the =. */
int
f(void)
{
        int x;
        int *p = &x;
        x = p;
        return x;
}
