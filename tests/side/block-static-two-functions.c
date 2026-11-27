/* Static objects of one name in two functions: f.s and g.s. */
int
f(void)
{
        static int s = 1;
        return s;
}
int
g(void)
{
        static int s = 2;
        return s;
}
