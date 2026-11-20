/* A parameter's name in redundant parentheses, before the array it is
   adjusted from (6.7.7.4p6): the suffix after the parentheses is the
   parameter's own, so its size may name an earlier parameter, which is
   taken as left out (6.7.7.3p5), and its brackets may hold static.
   Nothing is evaluated for either size.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of int f(int n, int *a) and int g(int *b) with the same
   bodies.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
int
f(int n, int(a)[n])
{
        return a[0];
}

int
g(int(b)[static 3])
{
        return b[2];
}
