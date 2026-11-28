/* An element of an incomplete type is an error at the first '[',
   6.7.7.3p1. */
int
f(void)
{
        int m[3][];
        return 0;
}
