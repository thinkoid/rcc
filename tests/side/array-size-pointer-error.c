/* A size of no integer type is an error at the size, 6.7.7.3p1. */
int
f(void)
{
        int a[(int *) 0];
        return 0;
}
