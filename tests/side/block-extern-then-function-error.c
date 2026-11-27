/* A function defined after a block's extern object of its name: the
   file adopts the block's object, and the definition is of another
   kind. */
int
f(void)
{
        extern int g;
        return g;
}
int
g(void)
{
        return 0;
}
