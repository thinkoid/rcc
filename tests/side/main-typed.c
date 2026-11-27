/* main of a long result and of typed parameters, which rcc does not
   check: it compiles as any function, and returns 0 when it falls off
   its end. */
long
main(char c, long l)
{
        if (c)
                return l;
}
