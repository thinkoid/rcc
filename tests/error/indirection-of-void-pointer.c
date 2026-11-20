/* The unary * on a pointer to void, which designates nothing to
   read. */
int
main(void)
{
        int a = 1;
        void *p = &a;

        *p;
        return 0;
}
