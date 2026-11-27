/* A product as the left operand of an assignment: a * 5 is a value, no
   lvalue, 6.5.17.1p2, said at the '='. */
int
main(void)
{
        int a = 1;
        return a * 5 = 3;
}
