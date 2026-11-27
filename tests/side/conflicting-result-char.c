/* Two declarations of one function of types of two kinds, which are not
   compatible, 6.7.1p5, 6.2.5p18: a result of char and then of signed
   char.  The error is at the later declaration's name. */
char g(void);

signed char
g(void)
{
        return 0;
}
