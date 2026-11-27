/* Two declarations of one function of types of two kinds, which are not
   compatible, 6.7.1p5, 6.2.5p18: a definition of a long result, then a
   declaration of long long.  The error is at the later declaration's
   name. */
long
g(void)
{
        return 0;
}

long long g(void);
