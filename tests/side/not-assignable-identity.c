/* An identity as the left operand of an assignment: a + 0 leaves the
   value of a, which is no lvalue, 6.5.17.1p2, said at the '='. */
int
main(void)
{
        int a = 1;
        return (a + 0) = 3;
}
