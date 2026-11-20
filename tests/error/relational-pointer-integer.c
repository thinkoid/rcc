/* A pointer ordered against an integer. */
int
main(void)
{
        int a = 1;
        int *p = &a;

        return p < 1;
}
