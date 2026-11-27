/* A void call's value as the operand of !, 6.3.2.2p1: the error is at
   the !. */
void g(void);

int
f(void)
{
        return !g();
}
