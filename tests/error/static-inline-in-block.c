/* static inline on a function in a block: a function of block scope
   takes no storage class but extern, 6.7.2p13. */
int
main(void)
{
        static inline int twice(int x);

        return 0;
}
