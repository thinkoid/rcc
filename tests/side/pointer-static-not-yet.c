/* A pointer of static storage is not yet, at its name. */
int
f(void)
{
        static int *p;
        return 0;
}
