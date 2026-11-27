/* A local of a parameter's name in the body's outermost block, which
   is the parameters' block, 6.9.2p10: the error is at the local's
   name. */
int
g(int a)
{
        int a;
        return a;
}
