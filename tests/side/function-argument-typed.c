/* A function as the argument of an unsigned char parameter: it decays
   to a pointer, which converts to no integer, 6.5.17.2p1, the error at
   the argument. */
int h(void);
int g(unsigned char a);

int
f(void)
{
        return g(h);
}
