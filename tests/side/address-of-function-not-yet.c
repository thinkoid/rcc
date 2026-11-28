/* The address of a function is not yet, at its name. */
int g(void);

int
f(void)
{
        return &g == 0;
}
