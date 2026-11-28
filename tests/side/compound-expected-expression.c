/* A compound assignment's right operand is an assignment expression,
   6.5.17.1p1: a ';' is its missing expression. */
void v(void);

int
main(void)
{
        int a = 1;
        int b = 2;
        a += ;
        return a;
}
