/* A block's static object with an initializer, in .data. */
int
f(void)
{
        static int s = 5;
        return s;
}
