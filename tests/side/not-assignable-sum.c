/* A sum as the left operand of an assignment: a + b is a value, no
   lvalue, 6.5.17.1p2, said at the '='. */
int
main(void)
{
        int a = 1;
        int b = 2;
        a + b = 3;
        return a;
}
