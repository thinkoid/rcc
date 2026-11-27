/* A block's extern of a name the block declared as a local. */
int
f(void)
{
        int e;
        extern int e;
        return e;
}
