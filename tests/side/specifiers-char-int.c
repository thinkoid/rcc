/* char int: 'int' cannot join the type specifiers before it,
   int after char, an error at 'int' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        char int x;
        return 0;
}
