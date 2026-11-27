/* A block's static initialized by a call. */
int g(void);
int
f(void)
{
        static int s = g();
        return s;
}
