/* A block's extern that nothing reads writes nothing. */
int
f(void)
{
        extern int e;
        return 0;
}
