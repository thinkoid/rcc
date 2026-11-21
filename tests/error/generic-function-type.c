/* A generic association of a function type: the type name shall
   specify a complete object type, 6.5.2.1p2. */
int
main(void)
{
        int n = 1;

        return _Generic(n, int(void): 1, default: 0);
}
