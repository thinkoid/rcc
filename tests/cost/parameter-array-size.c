/* A parameter's array whose size names an earlier parameter: n is in
   scope from the end of its declarator, 6.2.1p7, and the size, no
   integer constant expression, is taken as left out (6.7.7.3p5), since
   the parameter is adjusted to a pointer to int whatever the size
   (6.7.7.4p6).  The function is a definition, so the size is evaluated
   on entry for its effects alone (6.9.2p11): a read of n, discarded.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of int f(int n, int *a) with the same body after n;.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
int
f(int n, int a[n])
{
        return a[0];
}
