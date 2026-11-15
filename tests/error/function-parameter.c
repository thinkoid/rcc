/* A parameter of function type, adjusted to a pointer to the function,
   6.7.7.4p7, and called: a call through a pointer, the name of one
   included, is not yet. */
int f(int g(void))
{
    return g();
}

int main(void)
{
    return 0;
}
