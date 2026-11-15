/* A function's name where an integer is wanted: the designator converts
   to a pointer to the function, 6.3.2.1p4, which converts to an integer
   only through a cast, 6.5.17.2p1. */
int f(void);

int
main(void)
{
        long n = f;

        return n != 0;
}
