/* An if takes one else: the second starts an expression statement,
   and else is no expression. */
int
main(void)
{
        int a = 1;
        if (a)
                ;
        else
                ;
        else;
        return 0;
}
