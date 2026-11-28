/* A comma's value is no lvalue, 6.5.18p2 and its footnote 114, so it
   is no step's operand. */
int g(int x);

int
main(void)
{
        int a = 1;
        int b = 2;
        (a, b)++;
        return a;
}
