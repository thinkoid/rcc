/* A block's static initialized by a local. */
int
f(void)
{
        int a = 1;
        static int s = a;
        return s;
}
