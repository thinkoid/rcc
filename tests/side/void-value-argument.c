/* A void call's value as an argument, 6.3.2.2p1: the error is at the
   argument's first token. */
void g(void);
int h(int a);

int
f(void)
{
        return h(g());
}
