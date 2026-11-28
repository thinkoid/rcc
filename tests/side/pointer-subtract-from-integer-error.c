/* A pointer subtracted from an integer is an error at the -. */
int
f(void)
{
        int x;
        int *p = &x;
        return 1 - p == 0;
}
