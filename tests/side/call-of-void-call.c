/* A void call's result called: the void value is the error first,
   6.3.2.2p1, at the second '('. */
void g(void);

int
f(void)
{
        return g()();
}
