/* A definition whose parameter a marking attribute marks: the
   parameter is an object rcc makes, so the definition is not yet. */
int
twice(__attribute__((__aligned__(8))) int x)
{
        return x + x;
}
