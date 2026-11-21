/* A pointer to int initialized from 0 cast to a pointer to const void:
   no null pointer constant, which 6.3.2.3p3 names of 0 cast to void *
   alone, so the pointee loses its const, which 6.5.17.2p1 forbids, and
   rcc warns; the program compiles, the conversion the cast would
   make. */
int
main(void)
{
        int *p = (const void *) 0;

        return p != 0;
}
