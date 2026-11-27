/* Two declarations of one function of types of two kinds, which are not
   compatible, 6.7.1p5, 6.2.5p18: the second parameter of long and then
   of long long.  The error is at the later declaration's name. */
int g(int a, long b);

int
g(int a, long long b)
{
        return a;
}
