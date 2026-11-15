/* A local a marking attribute among its specifiers aligns, which rcc
   would have to honour, and is not yet. */
int
main(void)
{
        int __attribute__((__aligned__(16))) x = 0;

        return x;
}
