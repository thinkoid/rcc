/* The atomic type specifier on an atomic type name and on a qualified
   one, which 6.7.3.5p3 forbids: the specifier's type name is judged for
   an array type and a function type alone, so each is taken, and each
   _Atomic written draws its warning, two for x and one for y. */
_Atomic(_Atomic int) x;
_Atomic(const int) y;

int
main(void)
{
        return x + y;
}
