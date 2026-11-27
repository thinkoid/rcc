/* A block's static initialized by a parameter. */
int
f(int a)
{
        static int s = a;
        return s;
}
