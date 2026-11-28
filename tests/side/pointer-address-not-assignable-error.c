/* An address is no lvalue, and its assignment an error at the =. */
int
f(int x)
{
        &x = 0;
        return 0;
}
