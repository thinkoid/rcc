/* An object declared extern twice and read. */
extern int x;
extern int x;
int
f(void)
{
        return x;
}
