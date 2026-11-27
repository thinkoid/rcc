/* short long: 'long' cannot join the type specifiers before it,
   long after short, an error at 'long' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        short long x;
        return 0;
}
