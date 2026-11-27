/* =< is no operator: a = is an assignment, and its right operand
   starts at the <, which is no expression. */
int
main(void)
{
        int a = 1;
        int b = 2;
        return a = < b;
}
