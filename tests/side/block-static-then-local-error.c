/* A local of a name the block declared static: the error is at the
   local's name. */
int
f(void)
{
        static int s;
        int s;
        return s;
}
