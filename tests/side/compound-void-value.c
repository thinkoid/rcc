/* A compound assignment reads its right operand's value, so a void one
   is an error at the operator, 6.3.2.2p1. */
void v(void);

int
main(void)
{
        int a = 1;
        int b = 2;
        a += v();
        return a;
}
