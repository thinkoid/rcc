/* An extern declaration in a block is not yet, at its storage class. */
int
f(void)
{
        extern int e;
        return 0;
}
