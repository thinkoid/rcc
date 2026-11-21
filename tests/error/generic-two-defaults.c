/* A generic selection with two default associations, where it may
   have one, 6.5.2.1p2. */
int
main(void)
{
        int n = 1;

        return _Generic(n, default: 0, default: 1);
}
