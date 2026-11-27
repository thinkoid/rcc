/* char long: 'long' cannot join the type specifiers before it,
   long after char, an error at 'long' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        char long x;
        return 0;
}
