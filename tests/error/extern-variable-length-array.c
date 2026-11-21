/* A variable length array a block declares extern: an identifier of a
   variably modified type has no linkage, 6.7.7.3p2. */
void
f(int n)
{
        extern char buf[n];
}

int
main(void)
{
        f(1);
        return 0;
}
