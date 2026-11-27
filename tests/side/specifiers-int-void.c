/* int void: 'void' cannot join the type specifiers before it,
   void after int, an error at 'void' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        int void x;
        return 0;
}
