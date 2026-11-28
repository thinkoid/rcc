/* A local and a typedef name of one name in one block. */
int
f(void)
{
        int T = 1;
        typedef int T;
        return T;
}
