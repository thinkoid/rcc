/* A pointer passed for an integer is an error at the argument. */
int g(int x);

int
f(int *p)
{
        return g(p);
}
