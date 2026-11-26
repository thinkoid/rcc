/* The negation of a local as the left operand of an assignment: -x is
   a value, no lvalue, 6.5.4.3p3, said at the '='. */
int
main(void)
{
        int x = 1;
        -x = 2;
        return x;
}
