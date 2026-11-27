/* short short: 'short' cannot join the type specifiers before it,
   a second short, an error at 'short' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        short short x;
        return 0;
}
