/* An object assigned through a pointer to const: *p is an lvalue of
   the const-qualified type, 6.5.4.2p4, no modifiable lvalue,
   6.3.2.1p1. */
int
main(void)
{
        int a = 1;
        const int *p = &a;

        *p = 3;
        return 0;
}
