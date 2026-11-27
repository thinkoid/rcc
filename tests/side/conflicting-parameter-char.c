/* Two declarations of one function of types of two kinds, which are not
   compatible, 6.7.1p5, 6.2.5p18: a parameter of char and then of signed
   char, the type plain char is on x86-64.  The error is at the later
   declaration's name. */
int g(char x);

int
g(signed char x)
{
        return 0;
}
