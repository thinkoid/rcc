/* unsigned unsigned: 'unsigned' cannot join the type specifiers before it,
   a second unsigned, an error at 'unsigned' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        unsigned unsigned x;
        return 0;
}
