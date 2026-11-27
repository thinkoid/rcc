/* A block's static's initializer is an expression. */
int
f(void)
{
        static int s = ;
        return s;
}
