/* A call whose callee is an expression and not a function's name, which
   is not yet. */
int main(void)
{
    int a = 1;
    int *p = &a;

    return (*p)();
}
