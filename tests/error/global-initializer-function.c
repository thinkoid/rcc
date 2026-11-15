/* A function's name as a pointer's initializer, an address constant,
   6.6p11, not yet. */
int zero(void);
int (*handler)(void) = zero;

int
main(void)
{
        return handler();
}
