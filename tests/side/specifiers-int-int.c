/* int int: 'int' cannot join the type specifiers before it,
   a second int, an error at 'int' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        int int x;
        return 0;
}
