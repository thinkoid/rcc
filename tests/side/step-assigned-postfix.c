/* A postfix step's value is no lvalue, so it is no assignment's left
   operand, said at the '='. */
int g(int x);

int
main(void)
{
        int a = 1;
        int b = 2;
        a++ = 1;
        return a;
}
