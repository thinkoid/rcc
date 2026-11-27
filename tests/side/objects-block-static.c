/* A block's static object initialized past its type's range: the
   warning of a signed overflow, and the object in .data as f.s. */
int
f(void)
{
        static int s = 2147483647 + 1;
        return 0;
}
