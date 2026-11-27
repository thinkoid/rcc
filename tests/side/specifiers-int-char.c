/* int char: 'char' cannot join the type specifiers before it,
   char after int, an error at 'char' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        int char x;
        return 0;
}
