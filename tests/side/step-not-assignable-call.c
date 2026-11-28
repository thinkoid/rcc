/* A call's value is no lvalue, so it is no step's operand. */
int g(int x);

int
main(void)
{
        int a = 1;
        int b = 2;
        g(a)++;
        return a;
}
