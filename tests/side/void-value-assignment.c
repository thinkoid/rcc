/* A void call's value assigned, 6.3.2.2p1: the error is at the '='. */
void g(void);

int
f(void)
{
        int x;
        x = g();
        return x;
}
