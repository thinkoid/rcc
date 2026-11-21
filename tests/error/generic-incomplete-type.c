/* A generic association of an incomplete type, a struct before its
   list: the type name shall specify a complete object type, 6.5.2.1p2,
   in C23, which C2y relaxes. */
struct later;

int
main(void)
{
        int n = 1;

        return _Generic(n, struct later: 1, default: 0);
}
