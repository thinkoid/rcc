/* A parameter called, which is no function, 6.5.3.3p1: the error is at
   the call's '(' and names it. */
int
f(int a)
{
        return a(1);
}
