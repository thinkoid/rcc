/* A block's extern of an object the file only declares after it: the
   GOT's, in both functions. */
int
f(void)
{
        extern int g;
        return g;
}
extern int g;
int
h(void)
{
        return g;
}
