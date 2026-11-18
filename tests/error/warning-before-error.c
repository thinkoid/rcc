/* Warnings recorded before an error: the register variables past the
   target's count, the sixth on x86-64 and the eleventh on aarch64, take
   slots, each with a warning, and the address of the last is then an
   error, 6.5.4.2p1.  The warnings print first, in the order met, then
   the error. */
int
f(void)
{
        register int a, b, c, d, e, g, h, i, j, l, k;

        return *&k;
}

int
main(void)
{
        return 0;
}
