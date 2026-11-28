/* A typedef of a variable length array is rcc's not yet, at its name. */
int
f(int n)
{
        typedef int A[n];
        return 0;
}
