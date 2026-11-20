/* The address of a variable declared register, 6.5.4.2p1. */
int
main(void)
{
        register int r = 1;
        int *p = &r;

        return 0;
}
