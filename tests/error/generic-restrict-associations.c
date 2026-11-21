/* A generic selection naming int *restrict * and int **: rcc drops
   restrict, so the two are one type to it and compatible, which no two
   associations may be, 6.5.2.1p2, where C makes them two types,
   6.7.4.1p11, and the selection valid. */
int
main(void)
{
        int *p = 0;
        int **pp = &p;

        return _Generic(pp, int *restrict *: 1, int **: 0);
}
