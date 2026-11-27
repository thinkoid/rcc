/* signed unsigned: 'unsigned' cannot join the type specifiers before it,
   unsigned after signed, an error at 'unsigned' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        signed unsigned x;
        return 0;
}
