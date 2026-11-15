/* GNU C's __float128, as an x86-64 glibc names _Float128 by it: the
   typedef is accepted, and a local, which needs the object's size and
   layout, is not yet. */
typedef __float128 _Float128;

_Float128 strtof128(const char *s, char **end);

int
main(void)
{
        _Float128 x;

        return 0;
}
