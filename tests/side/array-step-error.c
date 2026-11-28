/* An array is no modifiable lvalue for a step either. */
int
f(void)
{
        int a[3];
        a++;
        return 0;
}
