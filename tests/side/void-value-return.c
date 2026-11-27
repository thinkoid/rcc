/* A void call's value returned from an int function, 6.3.2.2p1: the
   error is at the expression's first token. */
void g(void);

int
f(void)
{
        return g();
}
