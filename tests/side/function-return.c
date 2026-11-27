/* A function's name returned as an int, 6.8.7.5p3, 6.5.17.2p1: the
   error is at the expression's first token. */
int g(void);

int
f(void)
{
        return g;
}
