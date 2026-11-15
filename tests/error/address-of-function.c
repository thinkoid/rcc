/* &f on a function: its address is a pointer to the function,
   6.5.4.2p3, which is not yet. */
int f(void);

int
main(void)
{
        int (*p)(void) = &f;

        return 0;
}
