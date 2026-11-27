/* An object declared extern three times and read. */
extern int x;
extern int x;
extern int x;
int
f(void)
{
        return x;
}
