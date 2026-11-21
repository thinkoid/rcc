/* A pointer to int initialized from a pointer to const int: the
   pointee loses its const, which 6.5.17.2p1 forbids, so rcc warns, and
   the program compiles, the conversion the cast would make. */
int
main(void)
{
        int a = 1;
        const int *p = &a;
        int *q = p;

        return *q - 1;
}
