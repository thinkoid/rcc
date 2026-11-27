/* A function's name negated, 6.5.4.3p1: the error is at the '-'. */
int g(void);

int
f(void)
{
        return -g;
}
