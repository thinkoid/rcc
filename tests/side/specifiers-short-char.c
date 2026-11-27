/* short char: 'char' cannot join the type specifiers before it,
   char after short, an error at 'char' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        short char x;
        return 0;
}
