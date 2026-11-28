/* Arithmetic on a pointer to an array is not yet, at the operator. */
int
f(void)
{
        int a[3];
        return (&a + 1) == 0;
}
