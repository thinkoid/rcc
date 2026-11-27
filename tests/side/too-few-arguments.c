/* Fewer arguments than parameters, 6.5.3.3p2: the error is at the ')'. */
int g(int a, int b);

int
f(void)
{
        return g(1);
}
