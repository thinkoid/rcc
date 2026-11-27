/* short int int: 'int' cannot join the type specifiers before it,
   a second int after short, an error at 'int' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        short int int x;
        return 0;
}
