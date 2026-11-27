/* long short: 'short' cannot join the type specifiers before it,
   short after long, an error at 'short' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        long short x;
        return 0;
}
