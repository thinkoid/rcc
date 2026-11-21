/* shape.md chapter 5, a local array whose size is a parameter: a
   variable length array, though a parameter's array of the same size
   is the pointer the parameter is adjusted to.  Neither generator
   writes the stack allocation yet, so the program has no pin on
   either target. */
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
