/* An assignment's value is no lvalue, 6.5.17.1p3, so it is no step's
   operand. */
int g(int x);

int
main(void)
{
        int a = 1;
        int b = 2;
        (a = 1)++;
        return a;
}
