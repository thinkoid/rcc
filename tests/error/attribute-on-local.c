/* A local an attribute aligns: rcc makes the object, so it would have
   to honour the alignment, which is not yet.  A dropped attribute on
   the local before it changes nothing. */
int
main(void)
{
        int a __attribute__((unused)) = 0;
        int b __attribute__((__aligned__(16))) = 1;

        return a + b;
}
