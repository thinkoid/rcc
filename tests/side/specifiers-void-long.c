/* void long: 'long' cannot join the type specifiers before it,
   long after void, an error at 'long' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        void long x;
        return 0;
}
