/* A type name takes no storage class, 6.7.8p1: static in a cast is an
   error at the 'static', in rcc's words. */
int
f(void)
{
        int x = 0;
        return (static int) x;
}
