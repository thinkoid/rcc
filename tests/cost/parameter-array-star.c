/* A prototype whose parameter's array is sized by '*', a variable
   length array of unspecified size (6.7.7.3p4), which a declaration
   that is no definition may use: the parameter is adjusted to a
   pointer to int whatever the size (6.7.7.4p6), so the '*' is the size
   left out.  The definition after it, the program of
   parameter-array-size.c, is compatible with it.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of parameter-array-size.c.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
int f(int n, int a[*]);

int
f(int n, int a[n])
{
        return a[0];
}
