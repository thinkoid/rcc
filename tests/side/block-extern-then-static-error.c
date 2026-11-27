/* A static object at file scope after a block's extern of its name. */
int
f(void)
{
        extern int g;
        return g;
}
static int g;
