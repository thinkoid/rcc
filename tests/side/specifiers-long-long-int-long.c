/* long long int long: 'long' cannot join the type specifiers before it,
   a third long after long long int, an error at 'long' in rcc's words,
   6.7.3.1p2. */
int
f(void)
{
        long long int long x;
        return 0;
}
