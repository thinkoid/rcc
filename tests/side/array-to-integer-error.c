/* An array decays to a pointer, which converts to no integer as if by
   assignment, 6.5.17.2p1. */
int
f(void)
{
        int a[3];
        int x = a;
        return x;
}
