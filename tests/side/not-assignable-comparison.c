/* A comparison as the left operand of an assignment: = binds looser,
   so a < b = 1 is (a < b) = 1, and a comparison is a value, no
   lvalue, 6.5.17.1p2, said at the '='. */
int
main(void)
{
        int a = 1;
        int b = 2;
        a < b = 1;
        return 0;
}
