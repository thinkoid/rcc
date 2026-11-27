/* A void call's value as a while's condition, 6.3.2.2p1. */
void g(void);

int
f(void)
{
        while (g())
                ;
        return 0;
}
