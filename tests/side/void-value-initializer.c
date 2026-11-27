/* A void call's value as an initializer, 6.3.2.2p1: the error is at the
   initializer's first token. */
void g(void);

int
f(void)
{
        int x = g();
        return x;
}
