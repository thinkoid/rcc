/* A function's name assigned to an int, 6.5.17.2p1: the error is at the
   '='. */
int g(void);

int
f(void)
{
        int x;
        x = g;
        return x;
}
