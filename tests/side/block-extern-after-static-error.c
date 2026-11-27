/* A block's extern of a name the block declared static. */
int
f(void)
{
        static int e;
        extern int e;
        return e;
}
