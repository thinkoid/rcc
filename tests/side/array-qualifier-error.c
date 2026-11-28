/* A qualifier in the brackets of an array is a parameter's alone,
   6.7.7.3p1. */
int
f(void)
{
        int a[const 3];
        return 0;
}
