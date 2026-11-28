/* A function subscripted is an error at the [. */
int g(void);

int
f(void)
{
        return g[1];
}
