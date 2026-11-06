/* Chapter 3, &x: the alloca's address.  The listing must show x's
   slot address taken with lea, stored to p's slot, and *p loaded
   through it; x stays in memory because its address is taken, as
   every local does. */
int address(int a)
{
    int x = a;
    int *p = &x;

    return *p;
}
