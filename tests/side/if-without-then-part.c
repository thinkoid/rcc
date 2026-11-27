/* An if whose then part is missing: the else starts an expression
   statement in its place, and else is no expression. */
int
main(void)
{
        int a = 1;
        if (a)
                else;
        return 0;
}
