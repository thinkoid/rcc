/* Each function counts its labels from 1, the epilogue's, so each
   return of f jumps to .Lf.1 and g's to .Lg.1. */
int
f(void)
{
        return 1;
}

int
g(void)
{
        int a = 2;
        return a;
}
