/* A void call's value as an if's condition, 6.3.2.2p1: the error is at
   the condition's first token. */
void g(void);

int
f(void)
{
        if (g())
                return 1;
        return 0;
}
