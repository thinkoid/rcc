/* A block's extern in an inner block. */
int
f(void)
{
        int r = 0;
        {
                extern int e;
                r = e;
        }
        return r;
}
