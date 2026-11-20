/* The address of a value that is not an lvalue. */
int
main(void)
{
        int a = 1;
        int *p = &(a + 1);

        return 0;
}
