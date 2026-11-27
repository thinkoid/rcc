/* A call that no ')' closes: the error is where it was expected. */
int g(int a);

int
f(void)
{
        return g(1;
}
