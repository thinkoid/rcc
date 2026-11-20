/* The difference of two pointers to different types. */
int
main(void)
{
        int a = 1;
        char c = 2;
        int *p = &a;
        char *q = &c;

        return p - q;
}
