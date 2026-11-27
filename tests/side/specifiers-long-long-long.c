/* long long long: 'long' cannot join the type specifiers before it,
   a third long, an error at 'long' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        long long long x;
        return 0;
}
