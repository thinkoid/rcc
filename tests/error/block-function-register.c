/* A function declared register in a block: a function of block scope
   takes no storage class but extern, 6.7.2p13. */
int
main(void)
{
        register int twice(int x);

        return 0;
}
