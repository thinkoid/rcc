/* A register variable in the register an assembler name gives it,
   which is not yet. */
int
main(void)
{
        register int x __asm__("x19") = 1;

        return x;
}
