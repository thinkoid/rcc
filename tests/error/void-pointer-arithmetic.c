/* Arithmetic on a pointer to void, which 6.5.7p2 refuses, since void
   is no complete object type: GNU C takes it as a step of a byte, and
   rcc so, with a warning, and the program compiles. */
int
main(void)
{
        void *p = 0;

        p = p + 1;
        return 0;
}
