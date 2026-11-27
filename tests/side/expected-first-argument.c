/* A comma that no argument precedes: the error is at the ','. */
int g(int a, int b);

int
f(void)
{
        return g(, 1);
}
