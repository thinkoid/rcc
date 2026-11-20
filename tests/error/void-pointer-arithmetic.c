/* Arithmetic on a pointer to void, which has no element size. */
int
main(void)
{
        void *p = 0;

        p = p + 1;
        return 0;
}
