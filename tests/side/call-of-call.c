/* A call's result called, 6.5.3.3p1: the error is at the second '('. */
int g(void);

int
f(void)
{
        return g()();
}
