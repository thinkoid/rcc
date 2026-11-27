/* A function's name as an int's initializer: its pointer converts to
   no integer as if by assignment, 6.5.17.2p1, an error at the '='. */
int g(void);

int
f(void)
{
        int x = g;
        return x;
}
