/* A constant as the left operand of an assignment that is the right
   operand of another, 6.5.17.1p2: said at the second '=', after the
   first left operand is found assignable. */
int
main(void)
{
        int x;
        x = 0 = x;
        return x;
}
