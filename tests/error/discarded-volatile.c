/* A pointer to int initialized from a pointer to volatile int: the
   pointee loses its volatile, which 6.5.17.2p1 forbids, so rcc warns,
   and the program compiles, the conversion the cast would make. */
int
main(void)
{
        int a = 1;
        volatile int *p = &a;
        int *q = p;

        return *q - 1;
}
