/* A static object in a block is not yet, at its storage class. */
int
f(void)
{
        static int s = 2147483647 + 1;
        return 0;
}
