/* A conditional is no lvalue, its footnote 110, so it is no prefix
   step's operand. */

int
main(void)
{
        int a;
        int b;
        int c;
        ++(a ? b : c);
        return a;
}
