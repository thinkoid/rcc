/* long void: 'void' cannot join the type specifiers before it,
   void after long, an error at 'void' in rcc's words, 6.7.3.1p2. */
int
f(void)
{
        long void x;
        return 0;
}
