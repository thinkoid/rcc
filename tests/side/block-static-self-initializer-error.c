/* A block's static initialized by itself, in scope from the end of its
   declarator. */
int
f(void)
{
        static int s = s;
        return s;
}
