/* A call assigned to, which is no lvalue, 6.5.17.1p2. */
int g(void);

int
f(void)
{
        g() = 1;
        return 0;
}
