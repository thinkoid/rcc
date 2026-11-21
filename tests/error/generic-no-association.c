/* A generic selection without a default whose controlling expression
   is compatible with none of its associations' types: it shall be
   compatible with exactly one, 6.5.2.1p2. */
int
main(void)
{
        long n = 1;

        return _Generic(n, int: 0, long long: 1);
}
