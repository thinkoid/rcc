/* An object declared const assigned to: a const lvalue is no
   modifiable lvalue, 6.3.2.1p1, which 6.5.17.1p2 asks of the left
   operand. */
const int limit = 1;

int
main(void)
{
        limit = 2;
        return 0;
}
