/* A block's static of a name the block declared extern: the error is
   at the static's name. */
int
f(void)
{
        extern int s;
        static int s;
        return s;
}
