/* A parameter's array whose size names an earlier parameter: n is in
   scope from the end of its declarator, 6.2.1p7, and the size, no
   integer constant expression, is taken as left out (6.7.7.3p5), since
   the parameter is adjusted to a pointer to int whatever the size
   (6.7.7.4p6).  Nothing is evaluated for the size.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of int f(int n, int *a) with the same body.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
int
f(int n, int a[n])
{
        return a[0];
}
