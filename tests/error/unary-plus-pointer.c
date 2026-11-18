/* Unary + on a pointer: the operand of + has arithmetic type,
   6.5.4.3p1. */
int
main(void)
{
        int x = 0;
        int *p = &x;

        return +p != 0;
}
