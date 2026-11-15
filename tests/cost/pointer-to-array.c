/* backend.md 6.3 and aarch64.md 6.3, p[i][j] on a pointer to an array:
   p + i steps by the array's 12 bytes, the row decays to its address,
   and the element is read through it.  A parameter int m[][3] is
   adjusted to this type, int (*)[3], 6.7.7.4p6, so the two are one
   parameter.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with int p[][3] for int (*p)[3].

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
int
element(int (*p)[3], int i, int j)
{
        return p[i][j];
}
