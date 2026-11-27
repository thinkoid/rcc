/* A block's extern is a name of its block alone. */
int
f(void)
{
        extern int e;
        return e;
}
int
h(void)
{
        return e;
}
