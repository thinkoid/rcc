/* Unary + of a local as the left operand of an assignment: +x is the
   operand's value, no lvalue, 6.5.4.3p2, said at the '='. */
int
main(void)
{
        int x = 1;
        +x = 2;
        return x;
}
