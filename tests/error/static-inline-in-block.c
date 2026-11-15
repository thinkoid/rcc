/* static inline in a block, where static is not yet. */
int
main(void)
{
        static inline int twice(int x);

        return 0;
}
