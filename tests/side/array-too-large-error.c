/* An array whose size in bytes no long holds is an error at its '['. */
int
f(void)
{
        int a[4611686018427387904];
        return 0;
}
