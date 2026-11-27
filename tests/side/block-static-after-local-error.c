/* A block's static of a name the block declared as a local. */
int
f(void)
{
        int s;
        static int s;
        return s;
}
