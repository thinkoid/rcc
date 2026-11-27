/* A block's extern of an object the file declares extern before it. */
extern int g;
int
f(void)
{
        extern int g;
        return g;
}
