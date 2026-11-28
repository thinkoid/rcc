/* An array of arrays a typedef name makes is not yet, at its '['. */
typedef int A[4];

int
f(void)
{
        A m[3];
        return 0;
}
