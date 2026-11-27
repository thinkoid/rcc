/* char char: 'char' cannot join the type specifiers before it,
   a second char, an error at 'char' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        char char x;
        return 0;
}
