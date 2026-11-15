/* A typedef for void as the only parameter, unnamed: the list of no
   parameters, as (void) is (6.7.7.4p9), whatever spells the type.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of int f(void) with the same body and without the
   typedef.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
typedef void V;

int
f(V)
{
        return 0;
}
