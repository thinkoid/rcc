/* A block's extern of a static object at file scope that a local
   hides: the walk stops at the local, the declaration gives external
   linkage, and the name would have both, 6.2.2p7. */
static int g;
int
f(void)
{
        int g;
        {
                extern int g;
                return g;
        }
}
