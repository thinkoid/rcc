/* char short: 'short' cannot join the type specifiers before it,
   short after char, an error at 'short' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        char short x;
        return 0;
}
