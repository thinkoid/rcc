/* The complement of a local as the left operand of an assignment: ~x
   is a value, no lvalue, 6.5.4.3p4, said at the '='. */
int
main(void)
{
        int x = 1;
        return ~x = 2;
}
