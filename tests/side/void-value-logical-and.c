/* A void call's value as an operand of &&, 6.3.2.2p1: the error is at
   the operator. */
void g(void);

int
f(int a)
{
        return a && g();
}
