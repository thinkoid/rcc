/* Two pointers are no sum, an error at the +. */
int
f(void)
{
        int x;
        int *p = &x;
        return p + p == 0;
}
