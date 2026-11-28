/* '*' as a size is a parameter's alone, 6.7.7.3p4. */
int
f(void)
{
        int a[*];
        return 0;
}
