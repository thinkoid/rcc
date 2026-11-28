/* A pointer under unary + is an error at the operator. */
int
f(void)
{
        int x;
        int *p = &x;
        return +p;
}
