/* An integer passed for a pointer is an error at the argument. */
int g(int *p);

int
f(int x)
{
        return g(x);
}
