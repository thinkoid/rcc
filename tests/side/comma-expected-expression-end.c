/* A comma's right operand is an assignment expression, 6.5.18p1: a ')'
   after the comma is its missing expression. */
int
main(void)
{
        int a = 1;
        return (a, );
}
