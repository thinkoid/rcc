/* A call between two declarations of types of two kinds: the call is
   parsed against the first declaration, an unsigned parameter, and
   draws nothing; the error is the definition's, at its name. */
int g(unsigned x);

int
f(void)
{
        return g(1);
}

int
g(int x)
{
        return x;
}
