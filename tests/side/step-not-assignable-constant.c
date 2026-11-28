/* A postfix step's operand is a modifiable lvalue, 6.5.3.5p1, said at
   the operator: a constant is none. */
int g(int x);

int
main(void)
{
        int a = 1;
        int b = 2;
        5 ++;
        return a;
}
