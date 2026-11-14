/* Assignment to an expression that is not an lvalue: a + 1 is a
   value, not an object. */
int main(void)
{
    int a = 1;

    a + 1 = 2;
    return a;
}
