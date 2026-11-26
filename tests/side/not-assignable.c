/* A constant as the left operand of an assignment, which no
   modifiable lvalue is, 6.5.17.1p2: said at the '='. */
int
main(void)
{
        int x = 0;
        7 = x;
        return x;
}
