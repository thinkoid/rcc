/* Two declarations of one function of types of two kinds, which are not
   compatible, 6.7.1p5, 6.2.5p18: a definition of a char parameter, then
   a declaration of signed char.  The error is at the later declaration's
   name. */
int
g(char x)
{
        return 0;
}

int g(signed char x);
