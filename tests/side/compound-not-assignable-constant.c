/* A compound assignment's left operand is a modifiable lvalue,
   6.5.17.1p2, said at the operator: a constant is none. */
void v(void);

int
main(void)
{
        int a = 1;
        int b = 2;
        5 += a;
        return a;
}
