/* A function declared, then defined twice: the second definition is an
   error at its name, 6.9.1p5. */
int g(void);

int
g(void)
{
        return 1;
}

int
g(void)
{
        return 2;
}
