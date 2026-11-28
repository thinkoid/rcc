/* A sum in parentheses is no modifiable lvalue, 6.5.17.1p2. */
void v(void);

int
main(void)
{
        int a = 1;
        int b = 2;
        (a + 1) += 2;
        return a;
}
