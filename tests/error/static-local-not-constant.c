/* A block's static object initialized by the value of a local: an
   object of static storage takes a constant expression, 6.7.11p5, and
   is initialized once, before the program starts, 6.2.4p3. */
int
main(void)
{
        int n = 1;
        static int m = n;

        return m;
}
