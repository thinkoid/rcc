/* void int: 'int' cannot join the type specifiers before it,
   int after void, an error at 'int' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        void int x;
        return 0;
}
