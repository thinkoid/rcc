/* A return without a value as an if's then part, in a function that
   returns int, 6.8.7.5p1. */
int
main(void)
{
        int a = 1;
        if (a)
                return;
        return 0;
}
