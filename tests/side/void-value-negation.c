/* A void call's value negated, 6.3.2.2p1. */
void g(void);

int
f(void)
{
        return -g();
}
