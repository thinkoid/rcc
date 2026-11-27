/* A block's extern of a static object at file scope takes its
   internal linkage, 6.2.2p4. */
static int g = 1;
int
f(void)
{
        extern int g;
        return g;
}
