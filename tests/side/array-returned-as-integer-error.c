/* A returned array decays, and converts to no integer result. */
int
f(void)
{
        int a[3];
        return a;
}
