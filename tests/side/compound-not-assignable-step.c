/* A step's value is no lvalue, so it is no compound assignment's left
   operand. */
void v(void);

int
main(void)
{
        int a = 1;
        int b = 2;
        a++ += 1;
        return a;
}
