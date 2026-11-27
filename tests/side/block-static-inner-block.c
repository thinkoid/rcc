/* A static object of the outer block and one of an inner block, read
   together. */
int
f(void)
{
        static int s = 1;
        {
                static int t = 2;
                return s + t;
        }
}
