/* A block's static's initializer is a constant expression, warned of
   where a signed overflow reaches it, 6.6p4. */
int
f(void)
{
        static int s = 2147483647 + 1;
        return s;
}
