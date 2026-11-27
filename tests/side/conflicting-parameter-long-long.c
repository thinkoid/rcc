/* Two declarations of one function of types of two kinds, which are not
   compatible, 6.7.1p5, 6.2.5p18: a parameter of long and then of long
   long, two types of one width and sign.  The error is at the later
   declaration's name. */
int g(long x);

int
g(long long x)
{
        return 0;
}
