/* An array of functions is an error at the parameter list, 6.7.7.3p1. */
int
f(void)
{
        int g[3](void);
        return 0;
}
