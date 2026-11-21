/* A generic selection naming one type twice, long and long int: no two
   generic associations shall specify compatible types, 6.5.2.1p2. */
int
main(void)
{
        long n = 1;

        return _Generic(n, long: 0, long int: 1);
}
