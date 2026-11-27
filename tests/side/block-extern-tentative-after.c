/* A block's extern of an object a later tentative definition defines. */
int
f(void)
{
        extern int g;
        return g;
}
int g;
