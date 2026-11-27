/* An argument to a function defined with (), which is (void) too. */
int
g()
{
        return 1;
}

int
f(void)
{
        return g(1);
}
