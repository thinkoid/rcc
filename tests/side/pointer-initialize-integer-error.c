/* A constant other than 0 initializing a pointer is an error at the =. */
int
f(void)
{
        int *p = 1;
        return 0;
}
