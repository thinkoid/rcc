/* A function defined twice: the second definition is an error at its
   name, 6.9.1p5. */
int
f(void)
{
        return 1;
}

int
f(void)
{
        return 2;
}
