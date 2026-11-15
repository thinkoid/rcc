/* A function declared static in a block: a function of block scope
   takes no storage class but extern, 6.7.2p13. */
int
main(void)
{
        static int helper(int x);

        return 0;
}
