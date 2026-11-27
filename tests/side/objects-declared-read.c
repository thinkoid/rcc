/* An object declared extern alone and read: the GOT's load, and
   nothing in the data. */
extern int x;
int
f(void)
{
        return x;
}
