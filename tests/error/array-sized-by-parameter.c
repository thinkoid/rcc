/* A local array whose size is a parameter: a variable length array,
   which is not yet, though a parameter's array of the same size is the
   pointer the parameter is adjusted to. */
int
g(int n)
{
        int a[n];

        return 0;
}

int
main(void)
{
        return g(3);
}
